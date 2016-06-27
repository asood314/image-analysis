#ifndef NIA_CORE_HPP
#define NIA_CORE_HPP

#include <gtkmm-2.4/gtkmm.h>
#include "ImSeries.hpp"
//#include "ImRecord.hpp"
//#include "ImageAnalysisToolkit.hpp"
//#include "BatchService.hpp"
#include "FileManager.hpp"
#include "NiaViewer.hpp"

class NiaCore : public Gtk::Window
{

protected:
  //image data
  FileManager m_fileManager;

  //gui components
  Gtk::VBox m_vbox;
  NiaViewer m_viewer;
  Glib::RefPtr<Gtk::UIManager> m_refUIManager;
  Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;

  //signal handlers
  void on_menu_load();
  void on_channel0();
  void on_channel1();
  void on_channel2();
  void on_modeRGB();

public:
  NiaCore();
  NiaCore(ImSeries* ser);
  ~NiaCore();
  void init();

};

#endif
