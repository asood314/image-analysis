#!/bin/bash

cp /opt/local/lib/libgtkmm-2.4.1.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libatkmm-1.6.1.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libgtk-x11-2.0.0.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libgdkmm-2.4.1.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libgiomm-2.4.1.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libpangomm-1.4.1.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libglibmm-2.4.1.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libcairomm-1.0.1.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libsigc-2.0.0.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libgdk-x11-2.0.0.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libpangocairo-1.0.0.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libgio-2.0.0.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libXrender.1.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libXinerama.1.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libXi.6.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libXrandr.2.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libXcursor.1.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libXcomposite.1.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libXdamage.1.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libXfixes.3.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libX11.6.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libXext.6.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libatk-1.0.0.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libcairo.2.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libgdk_pixbuf-2.0.0.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libpangoft2-1.0.0.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libpango-1.0.0.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libgobject-2.0.0.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libglib-2.0.0.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libintl.8.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libfontconfig.1.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libfreetype.6.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libboost_filesystem-mt.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libboost_system-mt.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libboost_thread-mt.dylib Nia.app/Contents/MacOS/
cp /usr/lib/libc++.1.dylib Nia.app/Contents/MacOS/
cp /usr/lib/libSystem.B.dylib Nia.app/Contents/MacOS/

install_name_tool -change /opt/local/lib/libgtkmm-2.4.1.dylib @executable_path/libgtkmm-2.4.1.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libatkmm-1.6.1.dylib @executable_path/libatkmm-1.6.1.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libgtk-x11-2.0.0.dylib @executable_path/libgtk-x11-2.0.0.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libgdkmm-2.4.1.dylib @executable_path/libgdkmm-2.4.1.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libgiomm-2.4.1.dylib @executable_path/libgiomm-2.4.1.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libpangomm-1.4.1.dylib @executable_path/libpangomm-1.4.1.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libglibmm-2.4.1.dylib @executable_path/libglibmm-2.4.1.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libcairomm-1.0.1.dylib @executable_path/libcairomm-1.0.1.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libsigc-2.0.0.dylib @executable_path/libsigc-2.0.0.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libgdk-x11-2.0.0.dylib @executable_path/libgdk-x11-2.0.0.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libpangocairo-1.0.0.dylib @executable_path/libpangocairo-1.0.0.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libgio-2.0.0.dylib @executable_path/libgio-2.0.0.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libXrender.1.dylib @executable_path/libXrender.1.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libXinerama.1.dylib @executable_path/libXinerama.1.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libXi.6.dylib @executable_path/libXi.6.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libXrandr.2.dylib @executable_path/libXrandr.2.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libXcursor.1.dylib @executable_path/libXcursor.1.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libXcomposite.1.dylib @executable_path/libXcomposite.1.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libXdamage.1.dylib @executable_path/libXdamage.1.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libXfixes.3.dylib @executable_path/libXfixes.3.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libX11.6.dylib @executable_path/libX11.6.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libXext.6.dylib @executable_path/libXext.6.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libatk-1.0.0.dylib @executable_path/libatk-1.0.0.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libcairo.2.dylib @executable_path/libcairo.2.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libgdk_pixbuf-2.0.0.dylib @executable_path/libgdk_pixbuf-2.0.0.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libpangoft2-1.0.0.dylib @executable_path/libpangoft2-1.0.0.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libpango-1.0.0.dylib @executable_path/libpango-1.0.0.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libgobject-2.0.0.dylib @executable_path/libgobject-2.0.0.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libglib-2.0.0.dylib @executable_path/libglib-2.0.0.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libintl.8.dylib @executable_path/libintl.8.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libfontconfig.1.dylib @executable_path/libfontconfig.1.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libfreetype.6.dylib @executable_path/libfreetype.6.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libboost_filesystem-mt.dylib @executable_path/libboost_filesystem-mt.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libboost_system-mt.dylib @executable_path/libboost_system-mt.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /opt/local/lib/libboost_thread-mt.dylib @executable_path/libboost_thread-mt.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /usr/lib/libc++.1.dylib @executable_path/libc++.1.dylib Nia.app/Contents/MacOS/launcher
install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/launcher
