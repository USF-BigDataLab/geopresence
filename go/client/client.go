package client

import (
	"fmt"
	"geopresence/messages"
	"log"
	"strconv"
)

func Start(args []string) error {
	if len(args) < 2 {
		return fmt.Errorf("use: client <command> <arguments>\n" +
			"example: client add host1.com:12001 8gpcxc4h3n")
	}
	cmd := args[0]
	addr := args[1]
	cmdArgs := args[2:]
	switch cmd {
	case "add":
		return messages.SendAddHashRequest(addr, cmdArgs)
	case "polyq":
		return runPolyqueryRequest(addr, cmdArgs)
	default:
		return fmt.Errorf("unknown operation %s", cmd)
	}
}

func runPolyqueryRequest(addr string, latlongs []string) error {
	if len(latlongs)%2 != 0 {
		return fmt.Errorf("must have even count of lat/long")
	}
	pairCount := len(latlongs) / 2
	latitudes := make([]float32, pairCount, pairCount)
	longitudes := make([]float32, pairCount, pairCount)
	for i := 0; i < pairCount; i++ {
		lat, err := strconv.ParseFloat(latlongs[i*2], 32)
		if err != nil {
			return err
		}
		long, err := strconv.ParseFloat(latlongs[i*2+1], 32)
		if err != nil {
			return err
		}
		latitudes[i] = float32(lat)
		longitudes[i] = float32(long)
	}
	log.Printf("Sending Polyquery Request")
	response, err := messages.SendPolyqueryRequest(addr, latitudes, longitudes)
	if err != nil {
		return fmt.Errorf("when receiving response: %w", err)
	}
	switch mt := response.MsgType.(type) {
	case *messages.Message_PolyqueryResponse:
		log.Printf("%v", response.GetPolyqueryResponse().GetGeodes())
	case nil:
		return fmt.Errorf("received message with invalid type: %T", mt)
	default:
		return fmt.Errorf("received incorrect message type: %T", mt)
	}
	return nil
}
