package main

import (
	"geopresence/client"
	"geopresence/server"
	"log"
	"os"
)

func main() {
	if len(os.Args) < 2 {
		log.Panic("No arguments found")
	}
	command := os.Args[1]
	var err error
	args := os.Args[2:]
	switch command {
	case "client":
		err = client.Start(args)
	case "server":
		err = server.Start(args)
	default:
		log.Panicf("Invalid command %v", command)
	}
	if err != nil {
		log.Panic(err)
	}
}
