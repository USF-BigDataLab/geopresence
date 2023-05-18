package server

import (
	"encoding/json"
	"fmt"
	"hash"
	"io"
	"log"
	"math"
	"os"
	"os/signal"
	"strconv"
	"strings"
	"time"

	"geopresence/chord"
	"geopresence/chord/models"
	"geopresence/geode"
	"geopresence/messages"
)

type ServerCfg struct {
	GeohashUpperBound string // Also chord node id
	GeohashLowerBound string // Geohashes added below this are ignored
	ChordHostname     string // Local host name and listen port
	CreateNewChord    bool   // True to create a new ring
	SisterId          string // Existing node Id
	SisterHostname    string // Existing node name and port, blank for new ring
	ChordTimeout      int    // Duration to wait for response from remote nodes in seconds
	ChordMaxIdle      int    // Duration in seconds between stabalizing
}

func configJson() string {
	sc := &ServerCfg{
		GeohashUpperBound: "789bcdef",
		GeohashLowerBound: "6789bcde",
		ChordHostname:     "10.0.0.1:12345",
		CreateNewChord:    false,
		SisterId:          "ab123",
		SisterHostname:    "10.0.0.2:12345",
		ChordTimeout:      5,
		ChordMaxIdle:      60,
	}
	bytes, err := json.Marshal(sc)
	if err != nil {
		return err.Error()
	}
	return string(bytes)
}

func Start(args []string) error {
	if len(args) < 1 {
		return fmt.Errorf("no filepath for configuration")
	}
	path := args[0]
	log.Printf("Attempting to load run configuration from filepath %s", path)
	sc, err := fetchConfig(path)
	if err != nil {
		err = fmt.Errorf("error fetching config: %w. use format: %s", err, configJson())
		return err
	}
	log.Printf("Run configuration file loaded successfully")

	return runGeoserver(sc)
}

func chordNameToControl(chn string) (controlName string, err error) {
	splits := strings.SplitAfter(chn, ":")
	if len(splits) != 2 {
		return "", fmt.Errorf("chord hostname is invalid: %s", chn)
	}
	domain := splits[0]
	chordPortStr := splits[1]
	chordPort, err := strconv.ParseUint(chordPortStr, 10, 16)
	if err != nil {
		return "", fmt.Errorf("chord hostname %s has error parsing port: %w", chn, err)
	}
	// Check for -1 to avoid overflow to 0 on subsequent +1
	if chordPort > math.MaxUint16-1 {
		return "", fmt.Errorf("chord hostname %s has port too large", chn)
	}
	controlPort := chordPort + 1
	controlPortStr := strconv.FormatUint(controlPort, 10)
	return domain + controlPortStr, nil
}

func runGeoserver(sc *ServerCfg) error {
	cnode, err := startChord(sc)
	if err != nil {
		return err
	}
	defer cnode.Stop()

	controlName, err := chordNameToControl(sc.ChordHostname)
	if err != nil {
		return err
	}

	msgChan := make(chan *messages.MessageHandler)
	defer close(msgChan)
	connHandler := NewConnHandler(controlName, msgChan)
	err = connHandler.Open()
	if err != nil {
		return err
	}
	defer connHandler.Close()

	gp := geode.NewGeode(sc.GeohashUpperBound, 8, uint(len(sc.GeohashUpperBound)))
	geoRange, err := geode.NewGeoRange(gp, sc.GeohashUpperBound, sc.GeohashLowerBound)
	if err != nil {
		return err
	}
	defer gp.Close()

	go handleMessages(msgChan, cnode, geoRange)

	c := make(chan os.Signal, 1)
	signal.Notify(c, os.Interrupt)
	<-time.After(10 * time.Second)
	<-c
	return nil
}

func handleMessages(msgChan chan *messages.MessageHandler, cnode *chord.Node, gr *geode.GeoRange) {
	for mh := range msgChan {
		go handleMessage(mh, cnode, gr)
	}
}

func handleMessage(mh *messages.MessageHandler, cnode *chord.Node, gr *geode.GeoRange) {
	msg, err := mh.Receive()
	if err != nil {
		log.Printf("error receiving message: %s", err)
		return
	}
	switch mt := msg.MsgType.(type) {
	case *messages.Message_AddGeohashRequest:
		handleAddGeohashRequest(msg.GetAddGeohashRequest(), mh, cnode, gr)
	case nil:
		log.Printf("received message with invalid type: %T", mt)
	default:
		log.Printf("received message without handler implemented: %T", mt)
	}
}

func handleAddGeohashRequest(msg *messages.AddGeohashRequest, mh *messages.MessageHandler,
	cnode *chord.Node, gr *geode.GeoRange) {
	defer mh.Close()
	for _, geohash := range msg.GetGeohashes() {
		isInRange, err := gr.TryAddGeohash(geohash)
		if isInRange || err != nil {
			continue
		}
		// Valid geohash owned by another node
		tgtNode, err := cnode.Find(geohash)
		if err != nil {
			log.Printf("error finding target for: %s, %s", geohash, err.Error())
			continue
		}
		controlName, err := chordNameToControl(tgtNode.GetAddr())
		if err != nil {
			log.Printf("error converting chord to control name: %s", err.Error())
			continue
		}
		err = messages.SendAddHashRequest(controlName, []string{geohash})
		if err != nil {
			log.Printf("error forwarding hash request: %s", err)
			continue
		}
	}
	// Send any update to client?
}

func startChord(sc *ServerCfg) (*chord.Node, error) {
	var sister *models.Node
	if sc.CreateNewChord {
		sister = nil
	} else {
		sister = chord.NewInode(sc.SisterId, sc.SisterHostname)
	}

	cnf := chord.DefaultConfig()
	cnf.Id = sc.GeohashUpperBound
	cnf.Addr = sc.ChordHostname
	cnf.Hash = func() hash.Hash { return geode.NewTruncatedHash(5) }
	cnf.Timeout = time.Duration(sc.ChordTimeout) * time.Second
	cnf.MaxIdle = time.Duration(sc.ChordMaxIdle) * time.Second

	h, err := chord.NewNode(cnf, sister)
	if err != nil {
		return nil, err
	}
	return h, nil
}

func fetchConfig(path string) (*ServerCfg, error) {
	cfgFile, err := os.Open(path)
	if err != nil {
		return nil, fmt.Errorf("unable to open file: %w", err)
	}
	defer cfgFile.Close()

	return readConfig(cfgFile)
}

func readConfig(rdr io.Reader) (*ServerCfg, error) {
	fileBytes, err := io.ReadAll(rdr)
	if err != nil {
		return nil, fmt.Errorf("error reading config file: %w", err)
	}

	rc := &ServerCfg{}
	err = json.Unmarshal(fileBytes, rc)
	if err != nil {
		return nil, fmt.Errorf("error unmarshalling config file: %w", err)
	}

	return rc, nil
}
