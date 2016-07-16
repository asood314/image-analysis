all:
	g++ src/NiaBuffer.cpp -o obj/NiaBuffer.o -c -std=c++11 -I/opt/local/include/
	g++ src/NiaUtils.cpp -o obj/NiaUtils.o -c -std=c++11
	g++ src/Mask.cpp -o obj/Mask.o -c -std=c++11
	g++ src/LocalizedObject.cpp -o obj/LocalizedObject.o -c -std=c++11
	g++ src/Cluster.cpp -o obj/Cluster.o -c -std=c++11
	g++ src/Synapse.cpp -o obj/Synapse.o -c -std=c++11
	g++ src/SynapseCollection.cpp -o obj/SynapseCollection.o -c -std=c++11
	g++ src/Region.cpp -o obj/Region.o -c -std=c++11
	g++ src/ImRecord.cpp -o obj/ImRecord.o -c -std=c++11 -I/opt/local/include/
	g++ src/ImFrame.cpp -o obj/ImFrame.o -c -std=c++11 -I/opt/local/include/
	g++ src/ImStack.cpp -o obj/ImStack.o -c -std=c++11 -I/opt/local/include/
	g++ src/ImSeries.cpp -o obj/ImSeries.o -c -std=c++11 -I/opt/local/include/
	g++ src/ImageAnalysisToolkit.cpp -o obj/ImageAnalysisToolkit.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ src/FileManager.cpp -o obj/FileManager.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ src/BatchWindow.cpp -o obj/BatchWindow.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ src/BatchService.cpp -o obj/BatchService.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ src/FileSelector.cpp -o obj/FileSelector.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ src/ConfigurationDialog.cpp -o obj/ConfigurationDialog.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ src/NiaViewer.cpp -o obj/NiaViewer.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ src/NiaCore.cpp -o obj/NiaCore.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ src/main.cpp -o obj/main.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ -o launcher obj/*.o -std=c++11 -headerpad_max_install_names `pkg-config gtkmm-2.4 --libs` -lboost_filesystem-mt -lboost_system-mt -lboost_thread-mt

debug:
	g++ -g src/NiaBuffer.cpp -o obj/NiaBuffer.o -c -std=c++11 -I/opt/local/include/
	g++ -g src/NiaUtils.cpp -o obj/NiaUtils.o -c -std=c++11
	g++ -g src/Mask.cpp -o obj/Mask.o -c -std=c++11
	g++ -g src/LocalizedObject.cpp -o obj/LocalizedObject.o -c -std=c++11
	g++ -g src/Cluster.cpp -o obj/Cluster.o -c -std=c++11
	g++ -g src/Synapse.cpp -o obj/Synapse.o -c -std=c++11
	g++ -g src/SynapseCollection.cpp -o obj/SynapseCollection.o -c -std=c++11
	g++ -g src/Region.cpp -o obj/Region.o -c -std=c++11
	g++ -g src/ImRecord.cpp -o obj/ImRecord.o -c -std=c++11 -I/opt/local/include/
	g++ -g src/ImFrame.cpp -o obj/ImFrame.o -c -std=c++11 -I/opt/local/include/
	g++ -g src/ImStack.cpp -o obj/ImStack.o -c -std=c++11 -I/opt/local/include/
	g++ -g src/ImSeries.cpp -o obj/ImSeries.o -c -std=c++11 -I/opt/local/include/
	g++ -g src/ImageAnalysisToolkit.cpp -o obj/ImageAnalysisToolkit.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ -g src/FileManager.cpp -o obj/FileManager.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ -g src/BatchService.cpp -o obj/BatchService.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ -g src/BatchWindow.cpp -o obj/BatchWindow.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ -g src/FileSelector.cpp -o obj/FileSelector.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ -g src/ConfigurationDialog.cpp -o obj/ConfigurationDialog.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ -g src/NiaViewer.cpp -o obj/NiaViewer.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ -g src/NiaCore.cpp -o obj/NiaCore.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ -g src/main.cpp -o obj/main.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ -o launcher obj/*.o -std=c++11 `pkg-config gtkmm-2.4 --libs` -lboost_filesystem-mt -lboost_system-mt -lboost_thread-mt

front:
	g++ src/FileManager.cpp -o obj/FileManager.o -c -std=c++11 -I/opt/local/include/boost `pkg-config gtkmm-2.4 --cflags`
	g++ src/BatchWindow.cpp -o obj/BatchWindow.o -c -std=c++11 -I/opt/local/include/boost `pkg-config gtkmm-2.4 --cflags`
	g++ src/BatchService.cpp -o obj/BatchService.o -c -std=c++11 -I/opt/local/include/boost `pkg-config gtkmm-2.4 --cflags`
	g++ src/FileSelector.cpp -o obj/FileSelector.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ src/ConfigurationDialog.cpp -o obj/ConfigurationDialog.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ src/NiaViewer.cpp -o obj/NiaViewer.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ src/NiaCore.cpp -o obj/NiaCore.o -c -std=c++11 -I/opt/local/include/ `pkg-config gtkmm-2.4 --cflags`
	g++ src/main.cpp -o obj/main.o -c -std=c++11 -I/opt/local/include/boost `pkg-config gtkmm-2.4 --cflags`
	g++ -o launcher obj/*.o -std=c++11 -headerpad_max_install_names `pkg-config gtkmm-2.4 --libs` -lboost_filesystem-mt -lboost_system-mt -lboost_thread-mt

back:
	g++ src/NiaBuffer.cpp -o obj/NiaBuffer.o -c -std=c++11 -I/opt/local/include/
	g++ src/NiaUtils.cpp -o obj/NiaUtils.o -c -std=c++11
	g++ src/Mask.cpp -o obj/Mask.o -c -std=c++11
	g++ src/LocalizedObject.cpp -o obj/LocalizedObject.o -c -std=c++11
	g++ src/Cluster.cpp -o obj/Cluster.o -c -std=c++11
	g++ src/Synapse.cpp -o obj/Synapse.o -c -std=c++11
	g++ src/SynapseCollection.cpp -o obj/SynapseCollection.o -c -std=c++11
	g++ src/Region.cpp -o obj/Region.o -c -std=c++11
	g++ src/ImRecord.cpp -o obj/ImRecord.o -c -std=c++11 -I/opt/local/include/
	g++ src/ImFrame.cpp -o obj/ImFrame.o -c -std=c++11 -I/opt/local/include/
	g++ src/ImStack.cpp -o obj/ImStack.o -c -std=c++11 -I/opt/local/include/
	g++ src/ImSeries.cpp -o obj/ImSeries.o -c -std=c++11 -I/opt/local/include/
	g++ src/ImageAnalysisToolkit.cpp -o obj/ImageAnalysisToolkit.o -c -std=c++11 -I/opt/local/include/boost `pkg-config gtkmm-2.4 --cflags`

exec:
	g++ src/main.cpp -o obj/main.o -c -std=c++11 -I/opt/local/include/boost `pkg-config gtkmm-2.4 --cflags`
	g++ -o launcher obj/*.o -std=c++11 -headerpad_max_install_names `pkg-config gtkmm-2.4 --libs` -lboost_filesystem-mt -lboost_system-mt -lboost_thread-mt

clean:
	rm -f launcher obj/*.o
