package main

import (
	"encoding/json"
	"fmt"
	"hash"
	"io"
	"log"
	"os"
	"os/signal"
	"time"

	"geopresence/chord"
	"geopresence/chord/models"
	"geopresence/geode"
)

type RunConfig struct {
	Id             string // Node identifier
	Hostname       string // Local host name and listen port
	CreateNewChord bool   // True to create a new ring
	SisterId       string // Existing node Id
	SisterHostname string // Existing node name and port, blank for new ring
	ChordTimeout   int    // Duration to wait for response from remote nodes in seconds
	ChordMaxIdle   int    // Duration in seconds between stabalizing
}

func main() {
	if len(os.Args) < 2 {
		log.Panic("no arguments found")
	}

	path := os.Args[1]
	log.Printf("Attempting to load run configuration from filepath %s", path)
	rc, err := fetchConfig(path)
	if err != nil {
		err = fmt.Errorf("error fetching config: %w. use format: %s", err, runConfigJson())
		log.Panic(err)
	}
	log.Printf("Run configuration file loaded successfully")

	err = runGeode(rc)
	if err != nil {
		log.Panic(err)
	}
	os.Exit(0)
}

func fetchConfig(path string) (*RunConfig, error) {
	cfgFile, err := os.Open(path)
	if err != nil {
		return nil, fmt.Errorf("unable to open file: %w", err)
	}
	defer cfgFile.Close()

	return readConfig(cfgFile)
}

func readConfig(rdr io.Reader) (*RunConfig, error) {
	fileBytes, err := io.ReadAll(rdr)
	if err != nil {
		return nil, fmt.Errorf("error reading config file: %w", err)
	}

	rc := &RunConfig{}
	err = json.Unmarshal(fileBytes, rc)
	if err != nil {
		return nil, fmt.Errorf("error unmarshalling config file: %w", err)
	}

	return rc, nil
}

func runConfigJson() string {
	rc := &RunConfig{
		Id:             "9x000",
		Hostname:       "10.0.0.1:12345",
		CreateNewChord: false,
		SisterId:       "ab123",
		SisterHostname: "10.0.0.2:12345",
		ChordTimeout:   5,
		ChordMaxIdle:   60,
	}
	bytes, err := json.Marshal(rc)
	if err != nil {
		return err.Error()
	}
	return string(bytes)
}

func runGeode(rc *RunConfig) error {
	var sister *models.Node
	if rc.CreateNewChord {
		sister = nil
	} else {
		sister = chord.NewInode(rc.SisterId, rc.SisterHostname)
	}

	cnf := chord.DefaultConfig()
	cnf.Id = rc.Id
	cnf.Addr = rc.Hostname
	cnf.Hash = func() hash.Hash { return geode.NewTruncatedHash(5) }
	cnf.Timeout = time.Duration(rc.ChordTimeout) * time.Second
	cnf.MaxIdle = time.Duration(rc.ChordMaxIdle) * time.Second

	h, err := chord.NewNode(cnf, sister)
	if err != nil {
		return err
	}

	// Todo: Set up geode, listener
	// gp := NewGeode("9x", 16, 2)
	// defer gp.Free()
	// gp.AddGeohash("8gpfwr1b0z")
	// gp.PrintStdout()

	c := make(chan os.Signal, 1)
	signal.Notify(c, os.Interrupt)
	<-time.After(10 * time.Second)
	<-c
	h.Stop()
	return nil
}

func createNode(id string, addr string, sister *models.Node) (*chord.Node, error) {
	cnf := chord.DefaultConfig()
	cnf.Timeout = 10 * time.Millisecond
	cnf.MaxIdle = 100 * time.Millisecond

	n, err := chord.NewNode(cnf, sister)
	return n, err
}
