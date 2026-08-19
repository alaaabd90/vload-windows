#!/bin/bash

# Debian Policy requires the Version field to start with a digit; strip a
# leading "v" from release tags like "v1.4.1-windows" so dpkg-deb accepts it.
version="${1#v}"

mkdir -p vload/DEBIAN
mkdir -p vload/opt
cp -r linux64 vload/opt/
mv vload/opt/linux64 vload/opt/vload
rm -rf vload/opt/vload/usr
rm vload/opt/vload/launcher

# basic
cat >vload/DEBIAN/control <<-EOF
Package: vload
Version: $version
Architecture: amd64
Maintainer: MatsuriDayo vloadha_matsuri@protonmail.com
Depends: libxcb-xinerama0, libqt5core5a, libqt5gui5, libqt5network5, libqt5widgets5, libqt5svg5, libqt5x11extras5, desktop-file-utils
Description: Qt based cross-platform GUI proxy configuration manager (backend: v2ray / sing-box)
EOF

cat >vload/DEBIAN/postinst <<-EOF
if [ ! -s /usr/share/applications/vload.desktop ]; then
    cat >/usr/share/applications/vload.desktop<<-END
[Desktop Entry]
Name=vload
Comment=Qt based cross-platform GUI proxy configuration manager (backend: sing-box)
Exec=sh -c "PATH=/opt/vload:\$PATH /opt/vload/vload -appdata"
Icon=/opt/vload/vload.png
Terminal=false
Type=Application
Categories=Network;Application;
END
fi

setcap cap_net_admin=ep /opt/vload/vload_core

update-desktop-database
EOF

sudo chmod 0755 vload/DEBIAN/postinst

# desktop && PATH

sudo dpkg-deb -Zxz --build vload
