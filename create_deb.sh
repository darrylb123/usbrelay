#!/bin/bash -xe

version=$(cat setup.py | sed -rn "s/.*version = '(.*?)'.*/\1/p")
author=$(cat setup.py | sed -rn 's/.*author = "(.*?)".*/\1/p')
description=$(cat setup.py | sed -rn "s/.*description = '(.*?)'.*/\1/p" | sed 's/ from Python//')

mkdir -p deb/usr/{bin,lib} deb/DEBIAN
cp usbrelay deb/usr/bin/
cp libusbrelay.so deb/usr/lib/

cat >deb/DEBIAN/control<<EOM
Package: usbrelay
Version: $version
Section: custom
Priority: optional
Architecture: all
Essential: no
Installed-Size: $(du -s deb/usr/ | awk '{print $1}')
Maintainer: $author
Description: $description
EOM

dpkg-deb --build deb
dpkg-name -o deb.deb
