all:
	g++ src/ImFrame.cpp -o obj/ImFrame.o -c -std=c++11 -I/opt/local/include/boost `pkg-config gtkmm-2.4 --cflags`
	g++ src/ImStack.cpp -o obj/ImStack.o -c -std=c++11 -I/opt/local/include/boost `pkg-config gtkmm-2.4 --cflags`
	g++ src/ImSeries.cpp -o obj/ImSeries.o -c -std=c++11 -I/opt/local/include/boost `pkg-config gtkmm-2.4 --cflags`
	g++ src/FileManager.cpp -o obj/FileManager.o -c -std=c++11 -I/opt/local/include/boost `pkg-config gtkmm-2.4 --cflags`
	g++ src/FileSelector.cpp -o obj/FileSelector.o -c -std=c++11 -I/opt/local/include/boost `pkg-config gtkmm-2.4 --cflags`
	g++ src/NiaViewer.cpp -o obj/NiaViewer.o -c -std=c++11 -I/opt/local/include/boost `pkg-config gtkmm-2.4 --cflags`
	g++ src/NiaCore.cpp -o obj/NiaCore.o -c -std=c++11 -I/opt/local/include/boost `pkg-config gtkmm-2.4 --cflags`
	g++ src/main.cpp -o obj/main.o -c -std=c++11 -I/opt/local/include/boost `pkg-config gtkmm-2.4 --cflags`
	g++ -o launcher obj/*.o -std=c++11 `pkg-config gtkmm-2.4 --libs` -lboost_filesystem-mt -lboost_system-mt

lib:
	g++ src/ImFrame.cpp -o obj/ImFrame.o -c -std=c++11 -I/opt/local/include/boost `pkg-config gtkmm-2.4 --cflags`

exec:
	g++ src/main.cpp -o obj/main.o -c -std=c++11 -I/opt/local/include/boost `pkg-config gtkmm-2.4 --cflags`
	g++ -o launcher obj/*.o -std=c++11 `pkg-config gtkmm-2.4 --libs` -lboost_filesystem-mt -lboost_system-mt

clean:
	rm -f launcher obj/*.o