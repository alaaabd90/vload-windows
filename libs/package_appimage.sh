#!/bin/bash

sudo apt-get install fuse -y

cp -r linux64 vload.AppDir

# The file for Appimage

rm vload.AppDir/launcher

cat >vload.AppDir/vload.desktop <<-EOF
[Desktop Entry]
Name=vload
Exec=echo "vload started"
Icon=vload
Type=Application
Categories=Network
EOF

cat >vload.AppDir/AppRun <<-EOF
#!/bin/bash
echo "PATH: \${PATH}"
echo "vload runing on: \$APPDIR"
LD_LIBRARY_PATH=\${APPDIR}/usr/lib QT_PLUGIN_PATH=\${APPDIR}/usr/plugins \${APPDIR}/vload -appdata "\$@"
EOF

chmod +x vload.AppDir/AppRun

# build

# AppImageKit (the old repo) no longer carries this asset on its "latest"
# release (confirmed: /releases/latest redirects to tag 13, which 404s on
# the file too) - the tool moved to the AppImage/appimagetool repo's
# rolling "continuous" release tag.
curl -fLSO https://github.com/AppImage/appimagetool/releases/download/continuous/appimagetool-x86_64.AppImage
chmod +x appimagetool-x86_64.AppImage
./appimagetool-x86_64.AppImage vload.AppDir

# clean

rm appimagetool-x86_64.AppImage
rm -rf vload.AppDir
