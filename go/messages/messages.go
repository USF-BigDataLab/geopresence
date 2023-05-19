package messages

import (
	"log"
	"net"
)

func SendAddHashRequest(addr string, geohashes []string) error {
	conn, err := net.Dial("tcp", addr)
	if err != nil {
		return err
	}
	msg := &Message{
		MsgType: &Message_AddGeohashRequest{&AddGeohashRequest{Geohashes: geohashes}},
	}
	msgHandler := NewMessageHandler(conn)
	defer msgHandler.Close()
	err = msgHandler.Send(msg)
	if err != nil {
		return err
	}
	// Wait for remote terminate or response?
	return nil
}

func SendPolyqueryRequest(addr string, latitudes, longitudes []float32) (*Message, error) {
	conn, err := net.Dial("tcp", addr)
	if err != nil {
		return nil, err
	}
	msg := &Message{MsgType: &Message_PolyqueryRequest{&PolyqueryRequest{
		Latitudes:  latitudes,
		Longitudes: longitudes}},
	}
	msgHandler := NewMessageHandler(conn)
	defer msgHandler.Close()
	err = msgHandler.Send(msg)
	if err != nil {
		return nil, err
	}
	log.Printf("PolyqueryRequest sent, awaiting response")
	return msgHandler.Receive()
}

func (mh *MessageHandler) RespondPolyquery(geoNums []uint32) {
	msg := &Message{
		MsgType: &Message_PolyqueryResponse{&PolyqueryResponse{Geodes: geoNums}},
	}
	mh.Send(msg)
}
