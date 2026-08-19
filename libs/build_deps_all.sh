#!/bin/bash
set -e

cd libs

# 参数
if [ -z $cmake ]; then
  cmake="cmake"
fi
if [ -z $deps ]; then
  deps="deps"
fi

# libs/deps/...
mkdir -p $deps
cd $deps
if [ -z $NKR_PACKAGE ]; then
  INSTALL_PREFIX=$PWD/built
else
  INSTALL_PREFIX=$PWD/package
fi
rm -rf $INSTALL_PREFIX
mkdir -p $INSTALL_PREFIX

#### clean ####
clean() {
  rm -rf dl.zip yaml-* zxing-* protobuf
}

#### ZXing v2.0.0 ####
curl --ssl-no-revoke -L -o dl.zip https://github.com/nu-book/zxing-cpp/archive/refs/tags/v2.0.0.zip
unzip dl.zip

cd zxing-*
mkdir -p build
cd build

$cmake .. -GNinja -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=OFF -DBUILD_BLACKBOX_TESTS=OFF -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX
ninja && ninja install

cd ../..

#### yaml-cpp ####
curl --ssl-no-revoke -L -o dl.zip https://github.com/jbeder/yaml-cpp/archive/refs/tags/yaml-cpp-0.7.0.zip
unzip dl.zip

cd yaml-*
mkdir -p build
cd build

$cmake .. -GNinja -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTING=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX
ninja && ninja install

cd ../..

#### protobuf ####
git clone --recurse-submodules -b v21.4 --depth 1 --shallow-submodules https://github.com/protocolbuffers/protobuf

#备注：交叉编译要在 host 也安装 protobuf 并且版本一致,编译安装，同参数，安装到 /usr/local

mkdir -p protobuf/build
cd protobuf/build

$cmake .. -GNinja \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF \
  -Dprotobuf_MSVC_STATIC_RUNTIME=OFF \
  -Dprotobuf_BUILD_TESTS=OFF \
  -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX
ninja && ninja install

cd ../..

# When cross-compiling for a non-host target (TARGET_ARCH set by the CI
# workflow, e.g. arm64 from an x64 runner), the protoc.exe just built
# above is itself a target-arch binary - fine as a *library* to link
# against, but useless as the *code generator* tool the main app's own
# build invokes to turn .proto files into .cc/.h before compiling
# anything else (the x64 build host can't execute an arm64 exe at all -
# this is exactly what broke the arm64 CI job: "This version of
# .../protoc.exe is not compatible with the version of Windows you're
# running"). protoc's C++ output is architecture-independent text, so
# swapping in an official prebuilt host-native (win64) protoc.exe of the
# exact same version is safe and correct - only the generator tool
# changes; the cross-compiled libprotobuf* still links fine into the
# target-arch app.
if [ "$TARGET_ARCH" == "arm64" ]; then
  curl --ssl-no-revoke -L -o protoc-host.zip https://github.com/protocolbuffers/protobuf/releases/download/v21.4/protoc-21.4-win64.zip
  unzip -o -j protoc-host.zip bin/protoc.exe -d $INSTALL_PREFIX/bin
  rm -f protoc-host.zip
fi

####
clean
