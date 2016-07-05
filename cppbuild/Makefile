all:
	g++ -g src/LocalizedObject.cpp -o obj/LocalizedObject.o -c -std=c++11
	g++ -g src/Cluster.cpp -o obj/Cluster.o -c -std=c++11
	g++ -g src/Synapse.cpp -o obj/Synapse.o -c -std=c++11
	g++ -g src/SynapseCollection.cpp -o obj/SynapseCollection.o -c -std=c++11
	g++ -g src/Region.cpp -o obj/Region.o -c -std=c++11
	g++ -g src/Mask.cpp -o obj/Mask.o -c -std=c++11
	g++ -g src/NiaUtils.cpp -o obj/NiaUtils.o -c -std=c++11
	g++ -g src/ImRecord.cpp -o obj/ImRecord.o -c -std=c++11
	g++ -g src/ImFrame.cpp -o obj/ImFrame.o -c -std=c++11
	g++ -g src/ImStack.cpp -o obj/ImStack.o -c -std=c++11
	g++ -g src/ImSeries.cpp -o obj/ImSeries.o -c -std=c++11
	g++ -g src/ImageAnalysisToolkit.cpp -o obj/ImageAnalysisToolkit.o -c -std=c++11
	g++ -g src/FileManager.cpp -o obj/FileManager.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ -g src/BatchService.cpp -o obj/BatchService.o -c -std=c++11 -I/opt/local/include/
	g++ -g src/FileSelector.cpp -o obj/FileSelector.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ src/ConfigurationDialog.cpp -o obj/ConfigurationDialog.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ -g src/NiaViewer.cpp -o obj/NiaViewer.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ -g src/NiaCore.cpp -o obj/NiaCore.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ -g src/main.cpp -o obj/main.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ -o launcher obj/*.o -std=c++11 `pkg-config gtkmm-2.4 --libs` -lboost_filesystem-mt -lboost_system-mt -lboost_thread-mt

front:
	g++ src/FileSelector.cpp -o obj/FileSelector.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ src/ConfigurationDialog.cpp -o obj/ConfigurationDialog.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ src/NiaViewer.cpp -o obj/NiaViewer.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ src/NiaCore.cpp -o obj/NiaCore.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ src/main.cpp -o obj/main.o -c -std=c++11 -I/opt/local/include/boost `pkg-config gtkmm-2.4 --cflags`
	g++ -o launcher obj/*.o -std=c++11 `pkg-config gtkmm-2.4 --libs` -lboost_filesystem-mt -lboost_system-mt -lboost_thread-mt

back:
	g++ src/LocalizedObject.cpp -o obj/LocalizedObject.o -c -std=c++11
	g++ src/Cluster.cpp -o obj/Cluster.o -c -std=c++11
	g++ src/Synapse.cpp -o obj/Synapse.o -c -std=c++11
	g++ src/SynapseCollection.cpp -o obj/SynapseCollection.o -c -std=c++11
	g++ src/Region.cpp -o obj/Region.o -c -std=c++11
	g++ src/Mask.cpp -o obj/Mask.o -c -std=c++11
	g++ src/NiaUtils.cpp -o obj/NiaUtils.o -c -std=c++11
	g++ src/ImRecord.cpp -o obj/ImRecord.o -c -std=c++11
	g++ src/ImFrame.cpp -o obj/ImFrame.o -c -std=c++11
	g++ src/ImStack.cpp -o obj/ImStack.o -c -std=c++11
	g++ src/ImSeries.cpp -o obj/ImSeries.o -c -std=c++11
	g++ src/ImageAnalysisToolkit.cpp -o obj/ImageAnalysisToolkit.o -c -std=c++11
	g++ src/FileManager.cpp -o obj/FileManager.o -c -std=c++11 -I/opt/local/include/boost `pkg-config gtkmm-2.4 --cflags`
	g++ -g src/BatchService.cpp -o obj/BatchService.o -c -std=c++11 -I/opt/local/include/boost

exec:
	g++ src/main.cpp -o obj/main.o -c -std=c++11 -I/opt/local/include/boost `pkg-config gtkmm-2.4 --cflags`
	g++ -o launcher obj/*.o -std=c++11 `pkg-config gtkmm-2.4 --libs` -lboost_filesystem-mt -lboost_system-mt -lboost_thread-mt

clean:
	rm -f launcher obj/*.o
