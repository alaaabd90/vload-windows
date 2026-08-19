#!/bin/bash
set -e

source libs/env_deploy.sh
[ "$GOOS" == "windows" ] && [ "$GOARCH" == "amd64" ] && DEST=$DEPLOYMENT/windows64 || true
[ "$GOOS" == "windows" ] && [ "$GOARCH" == "arm64" ] && DEST=$DEPLOYMENT/windows-arm64 || true
[ "$GOOS" == "linux" ] && [ "$GOARCH" == "amd64" ] && DEST=$DEPLOYMENT/linux64 || true
[ "$GOOS" == "linux" ] && [ "$GOARCH" == "arm64" ] && DEST=$DEPLOYMENT/linux-arm64 || true
if [ -z $DEST ]; then
  echo "Please set GOOS GOARCH"
  exit 1
fi
rm -rf $DEST
mkdir -p $DEST

export CGO_ENABLED=0

#### Go: updater ####
pushd go/cmd/updater
[ "$GOOS" == "darwin" ] || go build -o $DEST -trimpath -ldflags "-w -s"
[ "$GOOS" == "linux" ] && mv $DEST/updater $DEST/launcher || true
popd

#### Go: vload_core ####
pushd go/cmd/vload_core
# tfogo_checklinkname0 + -checklinkname=0: without these, tfo-go's Windows
# dial path is a stub that returns ErrPlatformUnsupported for every TCP Fast
# Open connection instead of falling back to a normal dial - see
# database64128/tfo-go's README ("Windows support with Go 1.23 and later").
go build -v -o $DEST -trimpath -ldflags "-w -s -checklinkname=0 -X github.com/matsuridayo/libneko/neko_common.Version_neko=$version_standalone" -tags "with_clash_api,with_gvisor,with_quic,with_wireguard,with_utls,tfogo_checklinkname0"
popd
