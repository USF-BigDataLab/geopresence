package server

import (
	"geopresence/messages"
	"net"
)

type ConnHandler struct {
	listenAddr string
	msgChan    chan *messages.MessageHandler
	listener   net.Listener
}

func NewConnHandler(listenAddr string, msgChan chan *messages.MessageHandler) *ConnHandler {
	return &ConnHandler{
		listenAddr: listenAddr,
		msgChan:    msgChan,
	}
}

func (ch *ConnHandler) Open() error {
	listener, err := net.Listen("tcp", ch.listenAddr)
	if err != nil {
		return err
	}
	ch.listener = listener
	go ch.listenForConnections()
	return nil
}

func (ch *ConnHandler) listenForConnections() {
	for {
		conn, err := ch.listener.Accept()
		if err != nil {
			// This happens when the listener is closed
			return
		}
		ch.msgChan <- messages.NewMessageHandler(conn)
	}
}

func (ch *ConnHandler) Close() {
	ch.listener.Close()
}
