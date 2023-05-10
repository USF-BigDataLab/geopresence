package client

import (
	"fmt"
	"geopresence/messages"
)

func Start(args []string) error {
	if len(args) < 2 {
		return fmt.Errorf("require target address and operation")
	}
	addr := args[0]
	cmd := args[1]
	cmdArgs := args[2:]
	switch cmd {
	case "add":
		return messages.SendAddHashRequest(addr, cmdArgs)
	default:
		return fmt.Errorf("unknown operation %s", cmd)
	}
}
