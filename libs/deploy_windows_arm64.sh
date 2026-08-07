#!/bin/bash
set -e

source libs/env_deploy.sh
DEST=$DEPLOYMENT/windows-arm64
rm -rf $DEST
mkdir -p $DEST

#### copy exe ####
cp $BUILD/nekobox.exe $DEST

#### deploy qt & DLL runtime ####
pushd $DEST
# Called by full path rather than bare "windeployqt" - install-qt-action's
# set-env adds Qt's bin dir to GITHUB_PATH for *later steps*, but this
# script runs bash from within the same "Generate MakeFile and Build" step
# that installed Qt, and that PATH update doesn't reliably reach a bash
# subshell mid-step on Windows runners (confirmed: "windeployqt: command
# not found", exit 127, even though the arm64 app itself built and linked
# successfully). $QT_ROOT_DIR itself *is* already reliable here - it's used
# below for the OpenSSL DLLs - so use it directly instead of trusting PATH.
"$QT_ROOT_DIR/bin/windeployqt.exe" nekobox.exe --no-compiler-runtime --no-system-d3d-compiler --no-opengl-sw --verbose 2
rm -rf translations
rm -rf libEGL.dll libGLESv2.dll Qt6Pdf.dll

# QT_ROOT_DIR is set by install-qt-action (v4+) directly to the Qt install's
# root, unlike the x64 job's hand-rolled qtsdk/Qt layout - and OpenSSL DLL
# filenames vary by architecture (e.g. -x64 vs -arm64 suffix), so this globs
# rather than hardcoding a name, since it hasn't been verified against an
# actual arm64 Qt SDK layout yet.
cp "$QT_ROOT_DIR"/bin/libcrypto-3*.dll . 2>/dev/null || true
cp "$QT_ROOT_DIR"/bin/libssl-3*.dll . 2>/dev/null || true

popd

#### prepare deployment ####
cp $BUILD/*.pdb $DEPLOYMENT
