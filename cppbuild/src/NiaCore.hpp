#ifndef NIA_CORE_HPP
#define NIA_CORE_HPP

#include <gtkmm-2.4/gtkmm.h>
#include <boost/thread.hpp>
#include "BatchService.hpp"
#include "BatchWindow.hpp"
#include "ConfigurationDialog.hpp"
#include "NiaViewer.hpp"
#include "FileConverter.hpp"

class NiaCore : public Gtk::Window
{

protected:
  //back-end components
  FileManager m_fileManager;
  ImageAnalysisToolkit m_iat;
  BatchService m_batchService;
  boost::thread_group m_threads;

  //gui components
  Gtk::VBox m_vbox;
  NiaViewer m_viewer;
  Glib::RefPtr<Gtk::UIManager> m_refUIManager;
  Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;
  BatchWindow m_batchWindow;

  //signal handlers
  void on_menu_load();
  void on_save();
  void on_save_screenshot();
  void on_save_timeseries();
  void on_start_batch_jobs();
  void on_configure_clicked();
  void on_find_outliers_clicked();
  void on_filter_clicked();
  void on_load_storm_data_clicked();
  void on_find_signal_clicked(bool doAll);
  void on_apply_threshold_clicked();
  void on_find_puncta_clicked(bool doAll);
  void on_find_synapses_clicked();
  void on_full_analysis_clicked();
  void on_load_regions();
  void on_print_density();
  void on_batch_print_density();
  void on_quit(){ hide(); }
  
public:
  NiaCore();
  NiaCore(ImSeries* ser);
  ~NiaCore();
  void init();

};

#endif
