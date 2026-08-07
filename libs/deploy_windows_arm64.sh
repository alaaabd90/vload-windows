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

# windeployqt isn't reliably at "$QT_ROOT_DIR/bin/windeployqt.exe" for the
# win64_msvc2022_arm64_cross_compiled package - confirmed missing there on
# an actual run ("No such file or directory"), unlike the x64 job's SDK
# layout. Search the whole qtsdk tree install-qt-action populated (its
# parent) rather than guessing the exact subpath, and fail loudly with a
# directory listing if it's genuinely not shipped anywhere in the package,
# so the actual layout is visible in the CI log instead of just "not found".
WINDEPLOYQT=$(find "$(dirname "$QT_ROOT_DIR")" -iname "windeployqt.exe" 2>/dev/null | head -1)
if [ -z "$WINDEPLOYQT" ]; then
  echo "ERROR: windeployqt.exe not found anywhere under $(dirname "$QT_ROOT_DIR")" >&2
  echo "--- tree of $(dirname "$QT_ROOT_DIR") (3 levels) ---" >&2
  find "$(dirname "$QT_ROOT_DIR")" -maxdepth 3 >&2
  exit 1
fi
echo "Using windeployqt: $WINDEPLOYQT"
"$WINDEPLOYQT" nekobox.exe --no-compiler-runtime --no-system-d3d-compiler --no-opengl-sw --verbose 2
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
