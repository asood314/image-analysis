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
cp /opt/local/lib/libxcb.1.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libXau.6.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libXdmcp.6.dylib Nia.app/Contents/MacOS/
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
cp /opt/local/lib/libgmodule-2.0.0.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libpng16.16.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libz.1.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libbz2.1.0.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libgthread-2.0.0.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libharfbuzz.0.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libgraphite2.3.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libpixman-1.0.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libX11-xcb.1.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libxcb-render.0.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libffi.6.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libiconv.2.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libpcre.1.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libexpat.1.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libboost_filesystem-mt.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libboost_system-mt.dylib Nia.app/Contents/MacOS/
cp /opt/local/lib/libboost_thread-mt.dylib Nia.app/Contents/MacOS/
#cp /usr/lib/libc++.1.dylib Nia.app/Contents/MacOS/
#cp /usr/lib/libSystem.B.dylib Nia.app/Contents/MacOS/
#cp /usr/lib/libresolv.9.dylib Nia.app/Contents/MacOS/
#cp /usr/lib/libobjc.A.dylib Nia.app/Contents/MacOS/

install_name_tool -change /opt/local/lib/libgtkmm-2.4.1.dylib @executable_path/libgtkmm-2.4.1.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libatkmm-1.6.1.dylib @executable_path/libatkmm-1.6.1.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libgtk-x11-2.0.0.dylib @executable_path/libgtk-x11-2.0.0.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libgdkmm-2.4.1.dylib @executable_path/libgdkmm-2.4.1.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libgiomm-2.4.1.dylib @executable_path/libgiomm-2.4.1.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libpangomm-1.4.1.dylib @executable_path/libpangomm-1.4.1.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libglibmm-2.4.1.dylib @executable_path/libglibmm-2.4.1.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libcairomm-1.0.1.dylib @executable_path/libcairomm-1.0.1.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libsigc-2.0.0.dylib @executable_path/libsigc-2.0.0.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libgdk-x11-2.0.0.dylib @executable_path/libgdk-x11-2.0.0.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libpangocairo-1.0.0.dylib @executable_path/libpangocairo-1.0.0.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libgio-2.0.0.dylib @executable_path/libgio-2.0.0.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libXrender.1.dylib @executable_path/libXrender.1.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libXinerama.1.dylib @executable_path/libXinerama.1.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libXi.6.dylib @executable_path/libXi.6.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libXrandr.2.dylib @executable_path/libXrandr.2.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libXcursor.1.dylib @executable_path/libXcursor.1.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libXcomposite.1.dylib @executable_path/libXcomposite.1.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libXdamage.1.dylib @executable_path/libXdamage.1.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libXfixes.3.dylib @executable_path/libXfixes.3.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libX11.6.dylib @executable_path/libX11.6.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libXext.6.dylib @executable_path/libXext.6.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libatk-1.0.0.dylib @executable_path/libatk-1.0.0.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libcairo.2.dylib @executable_path/libcairo.2.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libgdk_pixbuf-2.0.0.dylib @executable_path/libgdk_pixbuf-2.0.0.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libpangoft2-1.0.0.dylib @executable_path/libpangoft2-1.0.0.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libpango-1.0.0.dylib @executable_path/libpango-1.0.0.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libgobject-2.0.0.dylib @executable_path/libgobject-2.0.0.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libglib-2.0.0.dylib @executable_path/libglib-2.0.0.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libintl.8.dylib @executable_path/libintl.8.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libfontconfig.1.dylib @executable_path/libfontconfig.1.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libfreetype.6.dylib @executable_path/libfreetype.6.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
#install_name_tool -change /usr/lib/libc++.1.dylib @executable_path/libc++.1.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libgtkmm-2.4.1.dylib

install_name_tool -change /opt/local/lib/libatkmm-1.6.1.dylib @executable_path/libatkmm-1.6.1.dylib Nia.app/Contents/MacOS/libatkmm-1.6.1.dylib
install_name_tool -change /opt/local/lib/libatk-1.0.0.dylib @executable_path/libatk-1.0.0.dylib Nia.app/Contents/MacOS/libatkmm-1.6.1.dylib
install_name_tool -change /opt/local/lib/libglibmm-2.4.1.dylib @executable_path/libglibmm-2.4.1.dylib Nia.app/Contents/MacOS/libatkmm-1.6.1.dylib
install_name_tool -change /opt/local/lib/libgobject-2.0.0.dylib @executable_path/libgobject-2.0.0.dylib Nia.app/Contents/MacOS/libatkmm-1.6.1.dylib
install_name_tool -change /opt/local/lib/libglib-2.0.0.dylib @executable_path/libglib-2.0.0.dylib Nia.app/Contents/MacOS/libatkmm-1.6.1.dylib
install_name_tool -change /opt/local/lib/libintl.8.dylib @executable_path/libintl.8.dylib Nia.app/Contents/MacOS/libatkmm-1.6.1.dylib
install_name_tool -change /opt/local/lib/libsigc-2.0.0.dylib @executable_path/libsigc-2.0.0.dylib Nia.app/Contents/MacOS/libatkmm-1.6.1.dylib
#install_name_tool -change /usr/lib/libc++.1.dylib @executable_path/libc++.1.dylib Nia.app/Contents/MacOS/libatkmm-1.6.1.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libatkmm-1.6.1.dylib

install_name_tool -change /opt/local/lib/libgtk-x11-2.0.0.dylib @executable_path/libgtk-x11-2.0.0.dylib Nia.app/Contents/MacOS/libgtk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libgdk-x11-2.0.0.dylib @executable_path/libgdk-x11-2.0.0.dylib Nia.app/Contents/MacOS/libgtk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libXrender.1.dylib @executable_path/libXrender.1.dylib Nia.app/Contents/MacOS/libgtk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libXinerama.1.dylib @executable_path/libXinerama.1.dylib Nia.app/Contents/MacOS/libgtk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libXi.6.dylib @executable_path/libXi.6.dylib Nia.app/Contents/MacOS/libgtk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libXrandr.2.dylib @executable_path/libXrandr.2.dylib Nia.app/Contents/MacOS/libgtk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libXcursor.1.dylib @executable_path/libXcursor.1.dylib Nia.app/Contents/MacOS/libgtk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libXcomposite.1.dylib @executable_path/libXcomposite.1.dylib Nia.app/Contents/MacOS/libgtk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libXdamage.1.dylib @executable_path/libXdamage.1.dylib Nia.app/Contents/MacOS/libgtk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libXfixes.3.dylib @executable_path/libXfixes.3.dylib Nia.app/Contents/MacOS/libgtk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libX11.6.dylib @executable_path/libX11.6.dylib Nia.app/Contents/MacOS/libgtk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libXext.6.dylib @executable_path/libXext.6.dylib Nia.app/Contents/MacOS/libgtk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libatk-1.0.0.dylib @executable_path/libatk-1.0.0.dylib Nia.app/Contents/MacOS/libgtk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libcairo.2.dylib @executable_path/libcairo.2.dylib Nia.app/Contents/MacOS/libgtk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libgdk_pixbuf-2.0.0.dylib @executable_path/libgdk_pixbuf-2.0.0.dylib Nia.app/Contents/MacOS/libgtk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libgio-2.0.0.dylib @executable_path/libgio-2.0.0.dylib Nia.app/Contents/MacOS/libgtk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libpangoft2-1.0.0.dylib @executable_path/libpangoft2-1.0.0.dylib Nia.app/Contents/MacOS/libgtk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libpango-1.0.0.dylib @executable_path/libpango-1.0.0.dylib Nia.app/Contents/MacOS/libgtk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libgobject-2.0.0.dylib @executable_path/libgobject-2.0.0.dylib Nia.app/Contents/MacOS/libgtk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libglib-2.0.0.dylib @executable_path/libglib-2.0.0.dylib Nia.app/Contents/MacOS/libgtk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libintl.8.dylib @executable_path/libintl.8.dylib Nia.app/Contents/MacOS/libgtk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libfontconfig.1.dylib @executable_path/libfontconfig.1.dylib Nia.app/Contents/MacOS/libgtk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libfreetype.6.dylib @executable_path/libfreetype.6.dylib Nia.app/Contents/MacOS/libgtk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libgmodule-2.0.0.dylib @executable_path/libgmodule-2.0.0.dylib Nia.app/Contents/MacOS/libgtk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libpangocairo-1.0.0.dylib @executable_path/libpangocairo-1.0.0.dylib Nia.app/Contents/MacOS/libgtk-x11-2.0.0.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libgtk-x11-2.0.0.dylib

install_name_tool -change /opt/local/lib/libgdk-x11-2.0.0.dylib @executable_path/libgdk-x11-2.0.0.dylib Nia.app/Contents/MacOS/libgdk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libgio-2.0.0.dylib @executable_path/libgio-2.0.0.dylib Nia.app/Contents/MacOS/libgdk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libpangocairo-1.0.0.dylib @executable_path/libpangocairo-1.0.0.dylib Nia.app/Contents/MacOS/libgdk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libpango-1.0.0.dylib @executable_path/libpango-1.0.0.dylib Nia.app/Contents/MacOS/libgdk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libcairo.2.dylib @executable_path/libcairo.2.dylib Nia.app/Contents/MacOS/libgdk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libgdk_pixbuf-2.0.0.dylib @executable_path/libgdk_pixbuf-2.0.0.dylib Nia.app/Contents/MacOS/libgdk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libXrender.1.dylib @executable_path/libXrender.1.dylib Nia.app/Contents/MacOS/libgdk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libXinerama.1.dylib @executable_path/libXinerama.1.dylib Nia.app/Contents/MacOS/libgdk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libXi.6.dylib @executable_path/libXi.6.dylib Nia.app/Contents/MacOS/libgdk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libXrandr.2.dylib @executable_path/libXrandr.2.dylib Nia.app/Contents/MacOS/libgdk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libXcursor.1.dylib @executable_path/libXcursor.1.dylib Nia.app/Contents/MacOS/libgdk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libXcomposite.1.dylib @executable_path/libXcomposite.1.dylib Nia.app/Contents/MacOS/libgdk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libXdamage.1.dylib @executable_path/libXdamage.1.dylib Nia.app/Contents/MacOS/libgdk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libXfixes.3.dylib @executable_path/libXfixes.3.dylib Nia.app/Contents/MacOS/libgdk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libX11.6.dylib @executable_path/libX11.6.dylib Nia.app/Contents/MacOS/libgdk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libXext.6.dylib @executable_path/libXext.6.dylib Nia.app/Contents/MacOS/libgdk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libglib-2.0.0.dylib @executable_path/libglib-2.0.0.dylib Nia.app/Contents/MacOS/libgdk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libintl.8.dylib @executable_path/libintl.8.dylib Nia.app/Contents/MacOS/libgdk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libfontconfig.1.dylib @executable_path/libfontconfig.1.dylib Nia.app/Contents/MacOS/libgdk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libfreetype.6.dylib @executable_path/libfreetype.6.dylib Nia.app/Contents/MacOS/libgdk-x11-2.0.0.dylib
install_name_tool -change /opt/local/lib/libgobject-2.0.0.dylib @executable_path/libgobject-2.0.0.dylib Nia.app/Contents/MacOS/libgdk-x11-2.0.0.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libgdk-x11-2.0.0.dylib

install_name_tool -change /opt/local/lib/libgdkmm-2.4.1.dylib @executable_path/libgdkmm-2.4.1.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libgiomm-2.4.1.dylib @executable_path/libgiomm-2.4.1.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libpangomm-1.4.1.dylib @executable_path/libpangomm-1.4.1.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libglibmm-2.4.1.dylib @executable_path/libglibmm-2.4.1.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libcairomm-1.0.1.dylib @executable_path/libcairomm-1.0.1.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libsigc-2.0.0.dylib @executable_path/libsigc-2.0.0.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libgdk-x11-2.0.0.dylib @executable_path/libgdk-x11-2.0.0.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libgtk-x11-2.0.0.dylib @executable_path/libgtk-x11-2.0.0.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libpangocairo-1.0.0.dylib @executable_path/libpangocairo-1.0.0.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libgio-2.0.0.dylib @executable_path/libgio-2.0.0.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libXrender.1.dylib @executable_path/libXrender.1.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libXinerama.1.dylib @executable_path/libXinerama.1.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libXi.6.dylib @executable_path/libXi.6.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libXrandr.2.dylib @executable_path/libXrandr.2.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libXcursor.1.dylib @executable_path/libXcursor.1.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libXcomposite.1.dylib @executable_path/libXcomposite.1.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libXdamage.1.dylib @executable_path/libXdamage.1.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libXfixes.3.dylib @executable_path/libXfixes.3.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libX11.6.dylib @executable_path/libX11.6.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libXext.6.dylib @executable_path/libXext.6.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libatk-1.0.0.dylib @executable_path/libatk-1.0.0.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libcairo.2.dylib @executable_path/libcairo.2.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libgdk_pixbuf-2.0.0.dylib @executable_path/libgdk_pixbuf-2.0.0.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libpangoft2-1.0.0.dylib @executable_path/libpangoft2-1.0.0.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libpango-1.0.0.dylib @executable_path/libpango-1.0.0.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libgobject-2.0.0.dylib @executable_path/libgobject-2.0.0.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libglib-2.0.0.dylib @executable_path/libglib-2.0.0.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libintl.8.dylib @executable_path/libintl.8.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libfontconfig.1.dylib @executable_path/libfontconfig.1.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libfreetype.6.dylib @executable_path/libfreetype.6.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
#install_name_tool -change /usr/lib/libc++.1.dylib @executable_path/libc++.1.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libgdkmm-2.4.1.dylib

install_name_tool -change /opt/local/lib/libgiomm-2.4.1.dylib @executable_path/libgiomm-2.4.1.dylib Nia.app/Contents/MacOS/libgiomm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libsigc-2.0.0.dylib @executable_path/libsigc-2.0.0.dylib Nia.app/Contents/MacOS/libgiomm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libgmodule-2.0.0.dylib @executable_path/libgmodule-2.0.0.dylib Nia.app/Contents/MacOS/libgiomm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libgio-2.0.0.dylib @executable_path/libgio-2.0.0.dylib Nia.app/Contents/MacOS/libgiomm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libgobject-2.0.0.dylib @executable_path/libgobject-2.0.0.dylib Nia.app/Contents/MacOS/libgiomm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libglib-2.0.0.dylib @executable_path/libglib-2.0.0.dylib Nia.app/Contents/MacOS/libgiomm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libintl.8.dylib @executable_path/libintl.8.dylib Nia.app/Contents/MacOS/libgiomm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libglibmm-2.4.1.dylib @executable_path/libglibmm-2.4.1.dylib Nia.app/Contents/MacOS/libgiomm-2.4.1.dylib
#install_name_tool -change /usr/lib/libc++.1.dylib @executable_path/libc++.1.dylib Nia.app/Contents/MacOS/libgiomm-2.4.1.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libgiomm-2.4.1.dylib

install_name_tool -change /opt/local/lib/libpangomm-1.4.1.dylib @executable_path/libpangomm-1.4.1.dylib Nia.app/Contents/MacOS/libpangomm-1.4.1.dylib
install_name_tool -change /opt/local/lib/libglibmm-2.4.1.dylib @executable_path/libglibmm-2.4.1.dylib Nia.app/Contents/MacOS/libpangomm-1.4.1.dylib
install_name_tool -change /opt/local/lib/libcairomm-1.0.1.dylib @executable_path/libcairomm-1.0.1.dylib Nia.app/Contents/MacOS/libpangomm-1.4.1.dylib
install_name_tool -change /opt/local/lib/libsigc-2.0.0.dylib @executable_path/libsigc-2.0.0.dylib Nia.app/Contents/MacOS/libpangomm-1.4.1.dylib
install_name_tool -change /opt/local/lib/libpangocairo-1.0.0.dylib @executable_path/libpangocairo-1.0.0.dylib Nia.app/Contents/MacOS/libpangomm-1.4.1.dylib
install_name_tool -change /opt/local/lib/libcairo.2.dylib @executable_path/libcairo.2.dylib Nia.app/Contents/MacOS/libpangomm-1.4.1.dylib
install_name_tool -change /opt/local/lib/libpangoft2-1.0.0.dylib @executable_path/libpangoft2-1.0.0.dylib Nia.app/Contents/MacOS/libpangomm-1.4.1.dylib
install_name_tool -change /opt/local/lib/libpango-1.0.0.dylib @executable_path/libpango-1.0.0.dylib Nia.app/Contents/MacOS/libpangomm-1.4.1.dylib
install_name_tool -change /opt/local/lib/libgobject-2.0.0.dylib @executable_path/libgobject-2.0.0.dylib Nia.app/Contents/MacOS/libpangomm-1.4.1.dylib
install_name_tool -change /opt/local/lib/libglib-2.0.0.dylib @executable_path/libglib-2.0.0.dylib Nia.app/Contents/MacOS/libpangomm-1.4.1.dylib
install_name_tool -change /opt/local/lib/libintl.8.dylib @executable_path/libintl.8.dylib Nia.app/Contents/MacOS/libpangomm-1.4.1.dylib
install_name_tool -change /opt/local/lib/libfontconfig.1.dylib @executable_path/libfontconfig.1.dylib Nia.app/Contents/MacOS/libpangomm-1.4.1.dylib
install_name_tool -change /opt/local/lib/libfreetype.6.dylib @executable_path/libfreetype.6.dylib Nia.app/Contents/MacOS/libpangomm-1.4.1.dylib
#install_name_tool -change /usr/lib/libc++.1.dylib @executable_path/libc++.1.dylib Nia.app/Contents/MacOS/libpangomm-1.4.1.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libpangomm-1.4.1.dylib

install_name_tool -change /opt/local/lib/libglibmm-2.4.1.dylib @executable_path/libglibmm-2.4.1.dylib Nia.app/Contents/MacOS/libglibmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libsigc-2.0.0.dylib @executable_path/libsigc-2.0.0.dylib Nia.app/Contents/MacOS/libglibmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libgobject-2.0.0.dylib @executable_path/libgobject-2.0.0.dylib Nia.app/Contents/MacOS/libglibmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libgmodule-2.0.0.dylib @executable_path/libgmodule-2.0.0.dylib Nia.app/Contents/MacOS/libglibmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libglib-2.0.0.dylib @executable_path/libglib-2.0.0.dylib Nia.app/Contents/MacOS/libglibmm-2.4.1.dylib
install_name_tool -change /opt/local/lib/libintl.8.dylib @executable_path/libintl.8.dylib Nia.app/Contents/MacOS/libglibmm-2.4.1.dylib
#install_name_tool -change /usr/lib/libc++.1.dylib @executable_path/libc++.1.dylib Nia.app/Contents/MacOS/libglibmm-2.4.1.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libglibmm-2.4.1.dylib

install_name_tool -change /opt/local/lib/libcairomm-1.0.1.dylib @executable_path/libcairomm-1.0.1.dylib Nia.app/Contents/MacOS/libcairomm-1.0.1.dylib
install_name_tool -change /opt/local/lib/libsigc-2.0.0.dylib @executable_path/libsigc-2.0.0.dylib Nia.app/Contents/MacOS/libcairomm-1.0.1.dylib
install_name_tool -change /opt/local/lib/libfreetype.6.dylib @executable_path/libfreetype.6.dylib Nia.app/Contents/MacOS/libcairomm-1.0.1.dylib
install_name_tool -change /opt/local/lib/libcairo.2.dylib @executable_path/libcairo.2.dylib Nia.app/Contents/MacOS/libcairomm-1.0.1.dylib
install_name_tool -change /opt/local/lib/libXrender.1.dylib @executable_path/libXrender.1.dylib Nia.app/Contents/MacOS/libcairomm-1.0.1.dylib
install_name_tool -change /opt/local/lib/libX11.6.dylib @executable_path/libX11.6.dylib Nia.app/Contents/MacOS/libcairomm-1.0.1.dylib
install_name_tool -change /opt/local/lib/libXext.6.dylib @executable_path/libXext.6.dylib Nia.app/Contents/MacOS/libcairomm-1.0.1.dylib
install_name_tool -change /opt/local/lib/libpng16.16.dylib @executable_path/libpng16.16.dylib Nia.app/Contents/MacOS/libcairomm-1.0.1.dylib
install_name_tool -change /opt/local/lib/libz.1.dylib @executable_path/libz.1.dylib Nia.app/Contents/MacOS/libcairomm-1.0.1.dylib
#install_name_tool -change /usr/lib/libc++.1.dylib @executable_path/libc++.1.dylib Nia.app/Contents/MacOS/libcairomm-1.0.1.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libcairomm-1.0.1.dylib

install_name_tool -change /opt/local/lib/libsigc-2.0.0.dylib @executable_path/libsigc-2.0.0.dylib Nia.app/Contents/MacOS/libsigc-2.0.0.dylib
#install_name_tool -change /usr/lib/libc++.1.dylib @executable_path/libc++.1.dylib Nia.app/Contents/MacOS/libsigc-2.0.0.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libsigc-2.0.0.dylib

install_name_tool -change /opt/local/lib/libpangocairo-1.0.0.dylib @executable_path/libpangocairo-1.0.0.dylib Nia.app/Contents/MacOS/libpangocairo-1.0.0.dylib
install_name_tool -change /opt/local/lib/libcairo.2.dylib @executable_path/libcairo.2.dylib Nia.app/Contents/MacOS/libpangocairo-1.0.0.dylib
install_name_tool -change /opt/local/lib/libpangoft2-1.0.0.dylib @executable_path/libpangoft2-1.0.0.dylib Nia.app/Contents/MacOS/libpangocairo-1.0.0.dylib
install_name_tool -change /opt/local/lib/libpango-1.0.0.dylib @executable_path/libpango-1.0.0.dylib Nia.app/Contents/MacOS/libpangocairo-1.0.0.dylib
install_name_tool -change /opt/local/lib/libgobject-2.0.0.dylib @executable_path/libgobject-2.0.0.dylib Nia.app/Contents/MacOS/libpangocairo-1.0.0.dylib
install_name_tool -change /opt/local/lib/libgthread-2.0.0.dylib @executable_path/libgthread-2.0.0.dylib Nia.app/Contents/MacOS/libpangocairo-1.0.0.dylib
install_name_tool -change /opt/local/lib/libglib-2.0.0.dylib @executable_path/libglib-2.0.0.dylib Nia.app/Contents/MacOS/libpangocairo-1.0.0.dylib
install_name_tool -change /opt/local/lib/libintl.8.dylib @executable_path/libintl.8.dylib Nia.app/Contents/MacOS/libpangocairo-1.0.0.dylib
install_name_tool -change /opt/local/lib/libharfbuzz.0.dylib @executable_path/libharfbuzz.0.dylib Nia.app/Contents/MacOS/libpangocairo-1.0.0.dylib
install_name_tool -change /opt/local/lib/libfontconfig.1.dylib @executable_path/libfontconfig.1.dylib Nia.app/Contents/MacOS/libpangocairo-1.0.0.dylib
install_name_tool -change /opt/local/lib/libfreetype.6.dylib @executable_path/libfreetype.6.dylib Nia.app/Contents/MacOS/libpangocairo-1.0.0.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libpangocairo-1.0.0.dylib

install_name_tool -change /opt/local/lib/libgio-2.0.0.dylib @executable_path/libgio-2.0.0.dylib Nia.app/Contents/MacOS/libgio-2.0.0.dylib
install_name_tool -change /opt/local/lib/libgobject-2.0.0.dylib @executable_path/libgobject-2.0.0.dylib Nia.app/Contents/MacOS/libgio-2.0.0.dylib
install_name_tool -change /opt/local/lib/libgmodule-2.0.0.dylib @executable_path/libgmodule-2.0.0.dylib Nia.app/Contents/MacOS/libgio-2.0.0.dylib
install_name_tool -change /opt/local/lib/libglib-2.0.0.dylib @executable_path/libglib-2.0.0.dylib Nia.app/Contents/MacOS/libgio-2.0.0.dylib
install_name_tool -change /opt/local/lib/libintl.8.dylib @executable_path/libintl.8.dylib Nia.app/Contents/MacOS/libgio-2.0.0.dylib
install_name_tool -change /opt/local/lib/libz.1.dylib @executable_path/libz.1.dylib Nia.app/Contents/MacOS/libgio-2.0.0.dylib
#install_name_tool -change /usr/lib/libresolv.9.dylib @executable_path/libresolv.9.dylib Nia.app/Contents/MacOS/libgio-2.0.0.dylib
#install_name_tool -change /usr/lib/libobjc.A.dylib @executable_path/libobjc.A.dylib Nia.app/Contents/MacOS/libgio-2.0.0.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libgio-2.0.0.dylib

install_name_tool -change /opt/local/lib/libXrender.1.dylib @executable_path/libXrender.1.dylib Nia.app/Contents/MacOS/libXrender.1.dylib
install_name_tool -change /opt/local/lib/libX11.6.dylib @executable_path/libX11.6.dylib Nia.app/Contents/MacOS/libXrender.1.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libXrender.1.dylib

install_name_tool -change /opt/local/lib/libXinerama.1.dylib @executable_path/libXinerama.1.dylib Nia.app/Contents/MacOS/libXinerama.1.dylib
install_name_tool -change /opt/local/lib/libX11.6.dylib @executable_path/libX11.6.dylib Nia.app/Contents/MacOS/libXinerama.1.dylib
install_name_tool -change /opt/local/lib/libXext.6.dylib @executable_path/libXext.6.dylib Nia.app/Contents/MacOS/libXinerama.1.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libXinerama.1.dylib

install_name_tool -change /opt/local/lib/libXi.6.dylib @executable_path/libXi.6.dylib Nia.app/Contents/MacOS/libXi.6.dylib
install_name_tool -change /opt/local/lib/libX11.6.dylib @executable_path/libX11.6.dylib Nia.app/Contents/MacOS/libXi.6.dylib
install_name_tool -change /opt/local/lib/libXext.6.dylib @executable_path/libXext.6.dylib Nia.app/Contents/MacOS/libXi.6.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libXi.6.dylib

install_name_tool -change /opt/local/lib/libXrandr.2.dylib @executable_path/libXrandr.2.dylib Nia.app/Contents/MacOS/libXrandr.2.dylib
install_name_tool -change /opt/local/lib/libXrender.1.dylib @executable_path/libXrender.1.dylib Nia.app/Contents/MacOS/libXrandr.2.dylib
install_name_tool -change /opt/local/lib/libX11.6.dylib @executable_path/libX11.6.dylib Nia.app/Contents/MacOS/libXrandr.2.dylib
install_name_tool -change /opt/local/lib/libXext.6.dylib @executable_path/libXext.6.dylib Nia.app/Contents/MacOS/libXrandr.2.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libXrandr.2.dylib

install_name_tool -change /opt/local/lib/libXcursor.1.dylib @executable_path/libXcursor.1.dylib Nia.app/Contents/MacOS/libXcursor.1.dylib
install_name_tool -change /opt/local/lib/libXrender.1.dylib @executable_path/libXrender.1.dylib Nia.app/Contents/MacOS/libXcursor.1.dylib
install_name_tool -change /opt/local/lib/libXfixes.3.dylib @executable_path/libXfixes.3.dylib Nia.app/Contents/MacOS/libXcursor.1.dylib
install_name_tool -change /opt/local/lib/libX11.6.dylib @executable_path/libX11.6.dylib Nia.app/Contents/MacOS/libXcursor.1.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libXcursor.1.dylib

install_name_tool -change /opt/local/lib/libXcomposite.1.dylib @executable_path/libXcomposite.1.dylib Nia.app/Contents/MacOS/libXcomposite.1.dylib
install_name_tool -change /opt/local/lib/libX11.6.dylib @executable_path/libX11.6.dylib Nia.app/Contents/MacOS/libXcomposite.1.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libXcomposite.1.dylib

install_name_tool -change /opt/local/lib/libXdamage.1.dylib @executable_path/libXdamage.1.dylib Nia.app/Contents/MacOS/libXdamage.1.dylib
install_name_tool -change /opt/local/lib/libXfixes.3.dylib @executable_path/libXfixes.3.dylib Nia.app/Contents/MacOS/libXdamage.1.dylib
install_name_tool -change /opt/local/lib/libX11.6.dylib @executable_path/libX11.6.dylib Nia.app/Contents/MacOS/libXdamage.1.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libXdamage.1.dylib

install_name_tool -change /opt/local/lib/libXfixes.3.dylib @executable_path/libXfixes.3.dylib Nia.app/Contents/MacOS/libXfixes.3.dylib
install_name_tool -change /opt/local/lib/libX11.6.dylib @executable_path/libX11.6.dylib Nia.app/Contents/MacOS/libXfixes.3.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libXfixes.3.dylib

install_name_tool -change /opt/local/lib/libX11.6.dylib @executable_path/libX11.6.dylib Nia.app/Contents/MacOS/libX11.6.dylib
install_name_tool -change /opt/local/lib/libxcb.1.dylib @executable_path/libxcb.1.dylib Nia.app/Contents/MacOS/libX11.6.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libX11.6.dylib

install_name_tool -change /opt/local/lib/libxcb.1.dylib @executable_path/libxcb.1.dylib Nia.app/Contents/MacOS/libxcb.1.dylib
install_name_tool -change /opt/local/lib/libXau.6.dylib @executable_path/libXau.6.dylib Nia.app/Contents/MacOS/libxcb.1.dylib
install_name_tool -change /opt/local/lib/libXdmcp.6.dylib @executable_path/libXdmcp.6.dylib Nia.app/Contents/MacOS/libxcb.1.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libxcb.1.dylib

install_name_tool -change /opt/local/lib/libXau.6.dylib @executable_path/libXau.6.dylib Nia.app/Contents/MacOS/libXau.6.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libXau.6.dylib

install_name_tool -change /opt/local/lib/libXdmcp.6.dylib @executable_path/libXdmcp.6.dylib Nia.app/Contents/MacOS/libXdmcp.6.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libXdmcp.6.dylib

install_name_tool -change /opt/local/lib/libXext.6.dylib @executable_path/libXext.6.dylib Nia.app/Contents/MacOS/libXext.6.dylib
install_name_tool -change /opt/local/lib/libX11.6.dylib @executable_path/libX11.6.dylib Nia.app/Contents/MacOS/libXext.6.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libXext.6.dylib

install_name_tool -change /opt/local/lib/libX11-xcb.1.dylib @executable_path/libX11-xcb.1.dylib Nia.app/Contents/MacOS/libX11-xcb.1.dylib
install_name_tool -change /opt/local/lib/libX11.6.dylib @executable_path/libX11.6.dylib Nia.app/Contents/MacOS/libX11-xcb.1.dylib
install_name_tool -change /opt/local/lib/libxcb.1.dylib @executable_path/libxcb.1.dylib Nia.app/Contents/MacOS/libX11-xcb.1.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libX11-xcb.1.dylib

install_name_tool -change /opt/local/lib/libxcb-render.0.dylib @executable_path/libxcb-render.0.dylib Nia.app/Contents/MacOS/libxcb-render.0.dylib
install_name_tool -change /opt/local/lib/libX11.6.dylib @executable_path/libX11.6.dylib Nia.app/Contents/MacOS/libxcb-render.0.dylib
install_name_tool -change /opt/local/lib/libxcb.1.dylib @executable_path/libxcb.1.dylib Nia.app/Contents/MacOS/libxcb-render.0.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libxcb-render.0.dylib

install_name_tool -change /opt/local/lib/libatk-1.0.0.dylib @executable_path/libatk-1.0.0.dylib Nia.app/Contents/MacOS/libatk-1.0.0.dylib
install_name_tool -change /opt/local/lib/libgobject-2.0.0.dylib @executable_path/libgobject-2.0.0.dylib Nia.app/Contents/MacOS/libatk-1.0.0.dylib
install_name_tool -change /opt/local/lib/libglib-2.0.0.dylib @executable_path/libglib-2.0.0.dylib Nia.app/Contents/MacOS/libatk-1.0.0.dylib
install_name_tool -change /opt/local/lib/libintl.8.dylib @executable_path/libintl.8.dylib Nia.app/Contents/MacOS/libatk-1.0.0.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libatk-1.0.0.dylib

install_name_tool -change /opt/local/lib/libcairo.2.dylib @executable_path/libcairo.2.dylib Nia.app/Contents/MacOS/libcairo.2.dylib
install_name_tool -change /opt/local/lib/libpixman-1.0.dylib @executable_path/libpixman-1.0.dylib Nia.app/Contents/MacOS/libcairo.2.dylib
install_name_tool -change /opt/local/lib/libfontconfig.1.dylib @executable_path/libfontconfig.1.dylib Nia.app/Contents/MacOS/libcairo.2.dylib
install_name_tool -change /opt/local/lib/libfreetype.6.dylib @executable_path/libfreetype.6.dylib Nia.app/Contents/MacOS/libcairo.2.dylib
install_name_tool -change /opt/local/lib/libXrender.1.dylib @executable_path/libXrender.1.dylib Nia.app/Contents/MacOS/libcairo.2.dylib
install_name_tool -change /opt/local/lib/libX11.6.dylib @executable_path/libX11.6.dylib Nia.app/Contents/MacOS/libcairo.2.dylib
install_name_tool -change /opt/local/lib/libXext.6.dylib @executable_path/libXext.6.dylib Nia.app/Contents/MacOS/libcairo.2.dylib
install_name_tool -change /opt/local/lib/libxcb.1.dylib @executable_path/libxcb.1.dylib Nia.app/Contents/MacOS/libcairo.2.dylib
install_name_tool -change /opt/local/lib/libX11-xcb.1.dylib @executable_path/libX11-xcb.1.dylib Nia.app/Contents/MacOS/libcairo.2.dylib
install_name_tool -change /opt/local/lib/libxcb-render.0.dylib @executable_path/libxcb-render.0.dylib Nia.app/Contents/MacOS/libcairo.2.dylib
install_name_tool -change /opt/local/lib/libz.1.dylib @executable_path/libz.1.dylib Nia.app/Contents/MacOS/libcairo.2.dylib
install_name_tool -change /opt/local/lib/libpng16.16.dylib @executable_path/libpng16.16.dylib Nia.app/Contents/MacOS/libcairo.2.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libcairo.2.dylib

install_name_tool -change /opt/local/lib/libpixman-1.0.dylib @executable_path/libpixman-1.0.dylib Nia.app/Contents/MacOS/libpixman-1.0.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libpixman-1.0.dylib

install_name_tool -change /opt/local/lib/libgdk_pixbuf-2.0.0.dylib @executable_path/libgdk_pixbuf-2.0.0.dylib Nia.app/Contents/MacOS/libgdk_pixbuf-2.0.0.dylib
install_name_tool -change /opt/local/lib/libgio-2.0.0.dylib @executable_path/libgio-2.0.0.dylib Nia.app/Contents/MacOS/libgdk_pixbuf-2.0.0.dylib
install_name_tool -change /opt/local/lib/libgobject-2.0.0.dylib @executable_path/libgobject-2.0.0.dylib Nia.app/Contents/MacOS/libgdk_pixbuf-2.0.0.dylib
install_name_tool -change /opt/local/lib/libglib-2.0.0.dylib @executable_path/libglib-2.0.0.dylib Nia.app/Contents/MacOS/libgdk_pixbuf-2.0.0.dylib
install_name_tool -change /opt/local/lib/libgmodule-2.0.0.dylib @executable_path/libgmodule-2.0.0.dylib Nia.app/Contents/MacOS/libgdk_pixbuf-2.0.0.dylib
install_name_tool -change /opt/local/lib/libintl.8.dylib @executable_path/libintl.8.dylib Nia.app/Contents/MacOS/libgdk_pixbuf-2.0.0.dylib
install_name_tool -change /opt/local/lib/libpng16.16.dylib @executable_path/libpng16.16.dylib Nia.app/Contents/MacOS/libgdk_pixbuf-2.0.0.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libgdk_pixbuf-2.0.0.dylib

install_name_tool -change /opt/local/lib/libpangoft2-1.0.0.dylib @executable_path/libpangoft2-1.0.0.dylib Nia.app/Contents/MacOS/libpangoft2-1.0.0.dylib
install_name_tool -change /opt/local/lib/libpango-1.0.0.dylib @executable_path/libpango-1.0.0.dylib Nia.app/Contents/MacOS/libpangoft2-1.0.0.dylib
install_name_tool -change /opt/local/lib/libgobject-2.0.0.dylib @executable_path/libgobject-2.0.0.dylib Nia.app/Contents/MacOS/libpangoft2-1.0.0.dylib
install_name_tool -change /opt/local/lib/libgthread-2.0.0.dylib @executable_path/libgthread-2.0.0.dylib Nia.app/Contents/MacOS/libpangoft2-1.0.0.dylib
install_name_tool -change /opt/local/lib/libglib-2.0.0.dylib @executable_path/libglib-2.0.0.dylib Nia.app/Contents/MacOS/libpangoft2-1.0.0.dylib
install_name_tool -change /opt/local/lib/libintl.8.dylib @executable_path/libintl.8.dylib Nia.app/Contents/MacOS/libpangoft2-1.0.0.dylib
install_name_tool -change /opt/local/lib/libharfbuzz.0.dylib @executable_path/libharfbuzz.0.dylib Nia.app/Contents/MacOS/libpangoft2-1.0.0.dylib
install_name_tool -change /opt/local/lib/libfontconfig.1.dylib @executable_path/libfontconfig.1.dylib Nia.app/Contents/MacOS/libpangoft2-1.0.0.dylib
install_name_tool -change /opt/local/lib/libfreetype.6.dylib @executable_path/libfreetype.6.dylib Nia.app/Contents/MacOS/libpangoft2-1.0.0.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libpangoft2-1.0.0.dylib

install_name_tool -change /opt/local/lib/libpango-1.0.0.dylib @executable_path/libpango-1.0.0.dylib Nia.app/Contents/MacOS/libpango-1.0.0.dylib
install_name_tool -change /opt/local/lib/libgobject-2.0.0.dylib @executable_path/libgobject-2.0.0.dylib Nia.app/Contents/MacOS/libpango-1.0.0.dylib
install_name_tool -change /opt/local/lib/libgthread-2.0.0.dylib @executable_path/libgthread-2.0.0.dylib Nia.app/Contents/MacOS/libpango-1.0.0.dylib
install_name_tool -change /opt/local/lib/libglib-2.0.0.dylib @executable_path/libglib-2.0.0.dylib Nia.app/Contents/MacOS/libpango-1.0.0.dylib
install_name_tool -change /opt/local/lib/libintl.8.dylib @executable_path/libintl.8.dylib Nia.app/Contents/MacOS/libpango-1.0.0.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libpango-1.0.0.dylib

install_name_tool -change /opt/local/lib/libgobject-2.0.0.dylib @executable_path/libgobject-2.0.0.dylib Nia.app/Contents/MacOS/libgobject-2.0.0.dylib
install_name_tool -change /opt/local/lib/libglib-2.0.0.dylib @executable_path/libglib-2.0.0.dylib Nia.app/Contents/MacOS/libgobject-2.0.0.dylib
install_name_tool -change /opt/local/lib/libintl.8.dylib @executable_path/libintl.8.dylib Nia.app/Contents/MacOS/libgobject-2.0.0.dylib
install_name_tool -change /opt/local/lib/libffi.6.dylib @executable_path/libffi.6.dylib Nia.app/Contents/MacOS/libgobject-2.0.0.dylib
#install_name_tool -change /usr/lib/libresolv.9.dylib @executable_path/libresolv.9.dylib Nia.app/Contents/MacOS/libgobject-2.0.0.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libgobject-2.0.0.dylib

install_name_tool -change /opt/local/lib/libglib-2.0.0.dylib @executable_path/libglib-2.0.0.dylib Nia.app/Contents/MacOS/libglib-2.0.0.dylib
install_name_tool -change /opt/local/lib/libintl.8.dylib @executable_path/libintl.8.dylib Nia.app/Contents/MacOS/libglib-2.0.0.dylib
install_name_tool -change /opt/local/lib/libiconv.2.dylib @executable_path/libiconv.2.dylib Nia.app/Contents/MacOS/libglib-2.0.0.dylib
install_name_tool -change /opt/local/lib/libpcre.1.dylib @executable_path/libpcre.1.dylib Nia.app/Contents/MacOS/libglib-2.0.0.dylib
#install_name_tool -change /usr/lib/libresolv.9.dylib @executable_path/libresolv.9.dylib Nia.app/Contents/MacOS/libglib-2.0.0.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libglib-2.0.0.dylib

install_name_tool -change /opt/local/lib/libintl.8.dylib @executable_path/libintl.8.dylib Nia.app/Contents/MacOS/libintl.8.dylib
install_name_tool -change /opt/local/lib/libiconv.2.dylib @executable_path/libiconv.2.dylib Nia.app/Contents/MacOS/libintl.8.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libintl.8.dylib

install_name_tool -change /opt/local/lib/libfontconfig.1.dylib @executable_path/libfontconfig.1.dylib Nia.app/Contents/MacOS/libfontconfig.1.dylib
install_name_tool -change /opt/local/lib/libfreetype.6.dylib @executable_path/libfreetype.6.dylib Nia.app/Contents/MacOS/libfontconfig.1.dylib
install_name_tool -change /opt/local/lib/libexpat.1.dylib @executable_path/libexpat.1.dylib Nia.app/Contents/MacOS/libfontconfig.1.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libfontconfig.1.dylib

install_name_tool -change /opt/local/lib/libfreetype.6.dylib @executable_path/libfreetype.6.dylib Nia.app/Contents/MacOS/libfreetype.6.dylib
install_name_tool -change /opt/local/lib/libz.1.dylib @executable_path/libz.1.dylib Nia.app/Contents/MacOS/libfreetype.6.dylib
install_name_tool -change /opt/local/lib/libbz2.1.0.dylib @executable_path/libbz2.1.0.dylib Nia.app/Contents/MacOS/libfreetype.6.dylib
install_name_tool -change /opt/local/lib/libpng16.16.dylib @executable_path/libpng16.16.dylib Nia.app/Contents/MacOS/libfreetype.6.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libfreetype.6.dylib

install_name_tool -change /opt/local/lib/libffi.6.dylib @executable_path/libffi.6.dylib Nia.app/Contents/MacOS/libffi.6.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libffi.6.dylib

install_name_tool -change /opt/local/lib/libiconv.2.dylib @executable_path/libiconv.2.dylib Nia.app/Contents/MacOS/libiconv.2.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libiconv.2.dylib

install_name_tool -change /opt/local/lib/libpcre.1.dylib @executable_path/libpcre.1.dylib Nia.app/Contents/MacOS/libpcre.1.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libpcre.1.dylib

install_name_tool -change /opt/local/lib/libexpat.1.dylib @executable_path/libexpat.1.dylib Nia.app/Contents/MacOS/libexpat.1.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libexpat.1.dylib

install_name_tool -change /opt/local/lib/libgmodule-2.0.0.dylib @executable_path/libgmodule-2.0.0.dylib Nia.app/Contents/MacOS/libgmodule-2.0.0.dylib
install_name_tool -change /opt/local/lib/libglib-2.0.0.dylib @executable_path/libglib-2.0.0.dylib Nia.app/Contents/MacOS/libgmodule-2.0.0.dylib
install_name_tool -change /opt/local/lib/libintl.8.dylib @executable_path/libintl.8.dylib Nia.app/Contents/MacOS/libgmodule-2.0.0.dylib
#install_name_tool -change /usr/lib/libresolv.9.dylib @executable_path/libresolv.9.dylib Nia.app/Contents/MacOS/libgmodule-2.0.0.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libgmodule-2.0.0.dylib

install_name_tool -change /opt/local/lib/libgthread-2.0.0.dylib @executable_path/libgthread-2.0.0.dylib Nia.app/Contents/MacOS/libgthread-2.0.0.dylib
install_name_tool -change /opt/local/lib/libglib-2.0.0.dylib @executable_path/libglib-2.0.0.dylib Nia.app/Contents/MacOS/libgthread-2.0.0.dylib
install_name_tool -change /opt/local/lib/libintl.8.dylib @executable_path/libintl.8.dylib Nia.app/Contents/MacOS/libgthread-2.0.0.dylib
#install_name_tool -change /usr/lib/libresolv.9.dylib @executable_path/libresolv.9.dylib Nia.app/Contents/MacOS/libgthread-2.0.0.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libgthread-2.0.0.dylib

install_name_tool -change /opt/local/lib/libpng16.16.dylib @executable_path/libpng16.16.dylib Nia.app/Contents/MacOS/libpng16.16.dylib
install_name_tool -change /opt/local/lib/libz.1.dylib @executable_path/libz.1.dylib Nia.app/Contents/MacOS/libpng16.16.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libpng16.16.dylib

install_name_tool -change /opt/local/lib/libz.1.dylib @executable_path/libz.1.dylib Nia.app/Contents/MacOS/libz.1.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libz.1.dylib

install_name_tool -change /opt/local/lib/libbz2.1.0.dylib @executable_path/libbz2.1.0.dylib Nia.app/Contents/MacOS/libbz2.1.0.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libbz2.1.0.dylib

install_name_tool -change /opt/local/lib/libharfbuzz.0.dylib @executable_path/libharfbuzz.0.dylib Nia.app/Contents/MacOS/libharfbuzz.0.dylib
install_name_tool -change /opt/local/lib/libglib-2.0.0.dylib @executable_path/libglib-2.0.0.dylib Nia.app/Contents/MacOS/libharfbuzz.0.dylib
install_name_tool -change /opt/local/lib/libintl.8.dylib @executable_path/libintl.8.dylib Nia.app/Contents/MacOS/libharfbuzz.0.dylib
install_name_tool -change /opt/local/lib/libfreetype.6.dylib @executable_path/libfreetype.6.dylib Nia.app/Contents/MacOS/libharfbuzz.0.dylib
install_name_tool -change /opt/local/lib/libgraphite2.3.dylib @executable_path/libgraphite2.3.dylib Nia.app/Contents/MacOS/libharfbuzz.0.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libharfbuzz.0.dylib

install_name_tool -change /opt/local/lib/libgraphite2.3.dylib @executable_path/libgraphite2.3.dylib Nia.app/Contents/MacOS/libgraphite2.3.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libgraphite2.3.dylib

install_name_tool -change /opt/local/lib/libboost_filesystem-mt.dylib @executable_path/libboost_filesystem-mt.dylib Nia.app/Contents/MacOS/libboost_filesystem-mt.dylib
install_name_tool -change /opt/local/lib/libboost_system-mt.dylib @executable_path/libboost_system-mt.dylib Nia.app/Contents/MacOS/libboost_filesystem-mt.dylib
#install_name_tool -change /usr/lib/libc++.1.dylib @executable_path/libc++.1.dylib Nia.app/Contents/MacOS/libboost_filesystem-mt.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libboost_filesystem-mt.dylib

install_name_tool -change /opt/local/lib/libboost_system-mt.dylib @executable_path/libboost_system-mt.dylib Nia.app/Contents/MacOS/libboost_system-mt.dylib
#install_name_tool -change /usr/lib/libc++.1.dylib @executable_path/libc++.1.dylib Nia.app/Contents/MacOS/libboost_system-mt.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libboost_system-mt.dylib

install_name_tool -change /opt/local/lib/libboost_thread-mt.dylib @executable_path/libboost_thread-mt.dylib Nia.app/Contents/MacOS/libboost_thread-mt.dylib
install_name_tool -change /opt/local/lib/libboost_system-mt.dylib @executable_path/libboost_system-mt.dylib Nia.app/Contents/MacOS/libboost_thread-mt.dylib
#install_name_tool -change /usr/lib/libc++.1.dylib @executable_path/libc++.1.dylib Nia.app/Contents/MacOS/libboost_thread-mt.dylib
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/libboost_thread-mt.dylib




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
#install_name_tool -change /usr/lib/libc++.1.dylib @executable_path/libc++.1.dylib Nia.app/Contents/MacOS/launcher
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/libSystem.B.dylib Nia.app/Contents/MacOS/launcher