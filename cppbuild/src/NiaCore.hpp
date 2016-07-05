#ifndef NIA_CORE_HPP
#define NIA_CORE_HPP

#include <gtkmm-2.4/gtkmm.h>
//#include "ImSeries.hpp"
//#include "ImageAnalysisToolkit.hpp"
#include "BatchService.hpp"
//#include "FileManager.hpp"
#include "ConfigurationDialog.hpp"
#include "NiaViewer.hpp"

class NiaCore : public Gtk::Window
{

protected:
  //back-end components
  FileManager m_fileManager;
  ImageAnalysisToolkit m_iat;
  BatchService m_batchService;

  //gui components
  Gtk::VBox m_vbox;
  NiaViewer m_viewer;
  Glib::RefPtr<Gtk::UIManager> m_refUIManager;
  Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;

  //signal handlers
  void on_menu_load();
  void on_start_batch_jobs();
  
public:
  NiaCore();
  NiaCore(ImSeries* ser);
  ~NiaCore();
  void init();

};

#endif
