package main

import (
	"fmt"
	"os"
	_ "unsafe"

	"grpc_server"

	"github.com/matsuridayo/libneko/neko_common"
	"github.com/sagernet/sing-box/constant"
)

func main() {
	fmt.Println("sing-box:", constant.Version)
	fmt.Println()

	// nekobox_core
	if len(os.Args) > 1 && os.Args[1] == "nekobox" {
		neko_common.RunMode = neko_common.RunMode_NekoBox_Core
		grpc_server.RunCore(setupCore, &server{})
		return
	}

	// this build only runs as a nekobox_core gRPC server, not as a
	// standalone sing-box CLI (cmd/sing-box is "package main", not
	// importable as a library in current sing-box-vload)
	fmt.Println("usage: nekobox_core nekobox")
	os.Exit(1)
}
