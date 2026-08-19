#!/bin/bash
set -e

source libs/env_deploy.sh
DEST=$DEPLOYMENT/windows-arm64
rm -rf $DEST
mkdir -p $DEST

#### copy exe ####
cp $BUILD/vload.exe $DEST

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
"$WINDEPLOYQT" vload.exe --no-compiler-runtime --no-system-d3d-compiler --no-opengl-sw --verbose 2
rm -rf translations
rm -rf libEGL.dll libGLESv2.dll Qt6Pdf.dll

# Confirmed via an actual built package: the official win64_msvc2022_arm64_cross_compiled
# Qt SDK doesn't ship libcrypto/libssl or a qopensslbackend.dll TLS plugin at
# all (unlike the x64 job's custom SDK repack, which bundles OpenSSL) - these
# globs always no-op on arm64. This is harmless, not a bug to work around:
# Qt6's TLS backend is a runtime-selected plugin, windeployqt already deployed
# qschannelbackend.dll (Windows' native TLS stack) as the available backend,
# and proxy-protocol TLS (VMess/VLESS/Trojan/REALITY/etc.) goes through the
# Go core's own statically-linked crypto/tls, entirely independent of Qt.
# Kept as a no-op/best-effort in case a future official arm64 Qt SDK adds
# OpenSSL support - harmless either way since windeployqt already guarantees
# a working TLS backend is present.
cp "$QT_ROOT_DIR"/bin/libcrypto-3*.dll . 2>/dev/null || true
cp "$QT_ROOT_DIR"/bin/libssl-3*.dll . 2>/dev/null || true

popd

#### prepare deployment ####
cp $BUILD/*.pdb $DEPLOYMENT
