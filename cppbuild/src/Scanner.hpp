#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <gtkmm-2.4/gtkmm.h>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <iostream>
#include <fstream>
#include "NiaBuffer.hpp"
#include "ImageAnalysisToolkit.hpp"
#include "BatchService.hpp"
#include "BatchWindow.hpp"
#include "FileConverter.hpp"

class Scanner : public Gtk::Window
{
  
protected:
  int m_maxThreads,m_activeThreads;
  boost::thread_group m_threadpool;
  boost::mutex m_mtx;
  std::string m_configFile,m_outputDir;
  std::vector<std::string> m_inputFiles;
  FileManager* m_fileManager;
  std::vector<int> m_ntasks;
  std::vector< std::vector<ImRecord*>* > m_records;
  ImSeries* m_data;

  Gtk::VBox m_vbox;
  Glib::RefPtr<Gtk::UIManager> m_refUIManager;
  Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;
  BatchWindow m_progressWindow;
  Gtk::ScrolledWindow m_scrolledWindow;
  Gtk::TextView m_textView;
  Glib::RefPtr<Gtk::TextBuffer> m_textBuffer;
  void run();

public:
  Scanner(){}
  Scanner(int argc, char** argv);
  ~Scanner(){}
  void run_analysis(ImSeries* data, ImageAnalysisToolkit* kit, int scanID, int p, int t, int seriesID);

};

#endif
