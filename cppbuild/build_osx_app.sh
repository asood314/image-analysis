#!/bin/bash

make
cp launcher Nia.app/Contents/MacOS/
./make_osx_icon.sh /Users/asood/Downloads/test.png
mv Nia.icns Nia.app/Contents/Resources/
./install_dylibs.sh