package messages

import "net"

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
