#ifndef CONFIGURATION_DIALOG_HPP
#define CONFIGURATION_DIALOG_HPP

#include <gtkmm-2.4/gtkmm.h>
#include "ImageAnalysisToolkit.hpp"
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

class ConfigurationDialog : public Gtk::Dialog
{

protected:
  ImageAnalysisToolkit* m_toolkit;
  int m_nchannels;
  Gtk::CheckButton m_masterBox,m_splitConfigBox;
  Gtk::Entry m_masterEntry,m_pfiEntry,m_saturationEntry;
  std::vector<Gtk::Entry*> m_lwEntry,m_radiusEntry,m_reclusterEntry,m_nrtEntry,m_peakEntry,m_floorEntry,m_bkgEntry;
  Gtk::ComboBox m_modeBox;
  Gtk::TreeModelColumn<ImageAnalysisToolkit::MasterMode> m_colMode;
  Gtk::TreeModelColumn<std::string> m_colString;
  Gtk::TreeModel::ColumnRecord m_columns;
  Glib::RefPtr<Gtk::ListStore> m_refTreeModel;
  Gtk::Label m_modeLabel,m_pfiLabel,m_saturationLabel,m_lwLabel,m_radiusLabel,m_reclusterLabel,m_nrtLabel,m_peakLabel,m_floorLabel,m_bkgLabel;

  std::vector<Gtk::CheckButton*> m_synapseChannels;
  std::vector<Gtk::Entry*> m_channelEntries;
  std::vector<Gtk::HBox*> m_channelBoxes;
  Gtk::RadioButton m_overlapButton,m_distanceButton;
  Gtk::CheckButton m_reqAllButton;
  Gtk::Entry m_thresholdEntry,m_requirementsEntry,m_descriptionEntry;
  Gtk::Label m_thresholdLabel,m_requirementsLabel,m_descriptionLabel;
  Gtk::Button m_addButton,m_removeButton;
  Glib::RefPtr<Gtk::ListStore> m_refTreeModel2;
  Gtk::TreeModel::ColumnRecord m_columns2;
  Gtk::TreeModelColumn<Glib::ustring> m_descriptionColumn;
  Gtk::TreeModelColumn<int> m_indexColumn;
  int m_scIndex;
  Gtk::TreeView m_treeView;

  Gtk::Entry m_threadEntry,m_divideEntry,m_postChanEntry;
  Gtk::Label m_threadLabel,m_postChanLabel;
  Gtk::CheckButton m_zprojBox,m_divideBox,m_writeTablesBox;

  Gtk::Notebook m_Notebook;
  Gtk::ScrolledWindow m_synapseWindow;
  Gtk::VBox m_analysisBox,m_synapseBox,m_batchBox;
  Gtk::HBox m_hbox1,m_hbox2,m_hbox3,m_hbox4,m_hbox5,m_hbox6,m_hbox7,m_hbox8,m_hbox9,m_hbox10,m_hbox11,m_hbox12,m_hbox13,m_hbox14,m_hbox15,m_hbox16;
  Gtk::VBox m_vbox1,m_vbox2,m_vbox3;
  Gtk::HSeparator m_hsep1;

  void on_split_button_clicked();
  void on_add_button_clicked();
  void on_remove_button_clicked();

public:
  ConfigurationDialog(ImageAnalysisToolkit* iat, int nchan, int maxThreads=-1, bool zproject=true);
  virtual ~ConfigurationDialog();
  int nchannels(){ return m_nchannels; }
  int getMaster();
  ImageAnalysisToolkit::MasterMode getMode();
  int getBitDepth(){ return boost::lexical_cast<int>(m_saturationEntry.get_text()); }
  int getPunctaFindingIterations(){ return boost::lexical_cast<int>(m_pfiEntry.get_text()); }
  bool doSeparateConfigs(){ return m_splitConfigBox.get_active(); }
  double getLocalWindow(int index = 0){ return boost::lexical_cast<double>(m_lwEntry[index]->get_text()); }
  double getRadius(int index = 0){ return boost::lexical_cast<double>(m_radiusEntry[index]->get_text()); }
  double getRecluster(int index = 0){ return boost::lexical_cast<double>(m_reclusterEntry[index]->get_text()); }
  double getNoiseRemovalThreshold(int index = 0){ return boost::lexical_cast<double>(m_nrtEntry[index]->get_text()); }
  double getPeak(int index = 0){ return boost::lexical_cast<double>(m_peakEntry[index]->get_text()); }
  double getFloor(int index = 0){ return boost::lexical_cast<double>(m_floorEntry[index]->get_text()); }
  double getBackgroundThreshold(int index = 0){ return boost::lexical_cast<double>(m_bkgEntry[index]->get_text()); }
  std::vector<std::string> getChannelNames();
  int getThreads(){ return boost::lexical_cast<int>(m_threadEntry.get_text()); }
  bool getZProject(){ return m_zprojBox.get_active(); }
  int getDivisor();
  bool getWriteTables(){ return m_writeTablesBox.get_active(); }
  int getPostChan(){ return boost::lexical_cast<int>(m_postChanEntry.get_text()); }

};

#endif
