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
  Gtk::CheckButton m_masterBox;
  Gtk::Entry m_masterEntry,m_pfiEntry,m_saturationEntry,m_lwEntry,m_lwiEntry,m_radiusEntry,m_reclusterEntry,m_nrtEntry,m_peakEntry,m_floorEntry;
  Gtk::ComboBox m_modeBox;
  Gtk::TreeModelColumn<ImageAnalysisToolkit::MasterMode> m_colMode;
  Gtk::TreeModelColumn<std::string> m_colString;
  Gtk::TreeModel::ColumnRecord m_columns;
  Glib::RefPtr<Gtk::ListStore> m_refTreeModel;
  Gtk::Label m_modeLabel,m_pfiLabel,m_saturationLabel,m_lwLabel,m_lwiLabel,m_radiusLabel,m_reclusterLabel,m_nrtLabel,m_peakLabel,m_floorLabel;

  std::vector<Gtk::CheckButton*> m_synapseChannels;
  std::vector<Gtk::Entry*> m_channelEntries;
  std::vector<Gtk::HBox*> m_channelBoxes;
  Gtk::RadioButton m_overlapButton,m_distanceButton;
  Gtk::CheckButton m_reqAllButton;
  Gtk::Entry m_thresholdEntry,m_requirementsEntry,m_descriptionEntry;
  Gtk::Label m_thresholdLabel,m_requirementsLabel,m_descriptionLabel;
  Gtk::Button m_addButton;
  Glib::RefPtr<Gtk::ListStore> m_refTreeModel2;
  Gtk::TreeModel::ColumnRecord m_columns2;
  Gtk::TreeModelColumn<Glib::ustring> m_descriptionColumn;
  Gtk::TreeModelColumn<uint8_t> m_indexColumn;
  uint8_t m_scIndex;
  Gtk::TreeView m_treeView;

  Gtk::Entry m_threadEntry,m_divideEntry,m_postChanEntry;
  Gtk::Label m_threadLabel,m_postChanLabel;
  Gtk::CheckButton m_zprojBox,m_divideBox,m_writeTablesBox;

  Gtk::Notebook m_Notebook;
  Gtk::ScrolledWindow m_synapseWindow;
  Gtk::VBox m_analysisBox,m_synapseBox,m_batchBox;
  Gtk::HBox m_hbox1,m_hbox2,m_hbox3,m_hbox4,m_hbox5,m_hbox6,m_hbox7,m_hbox8,m_hbox9,m_hbox10,m_hbox11,m_hbox12,m_hbox13,m_hbox14;
  Gtk::VBox m_vbox1,m_vbox2,m_vbox3;

  void on_add_button_clicked();

public:
  ConfigurationDialog(ImageAnalysisToolkit* iat, uint8_t nchan, int maxThreads=-1, bool zproject=true);
  virtual ~ConfigurationDialog();
  uint8_t getMaster();
  ImageAnalysisToolkit::MasterMode getMode();
  uint8_t getPunctaFindingIterations(){ return (uint8_t)boost::lexical_cast<int>(m_pfiEntry.get_text()); }
  double getLocalWindow(){ return boost::lexical_cast<double>(m_lwEntry.get_text()); }
  double getRadius(){ return boost::lexical_cast<double>(m_radiusEntry.get_text()); }
  double getRecluster(){ return boost::lexical_cast<double>(m_reclusterEntry.get_text()); }
  double getNoiseRemovalThreshold(){ return boost::lexical_cast<double>(m_nrtEntry.get_text()); }
  double getPeak(){ return boost::lexical_cast<double>(m_peakEntry.get_text()); }
  double getFloor(){ return boost::lexical_cast<double>(m_floorEntry.get_text()); }
  std::vector<std::string> getChannelNames();
  uint8_t getThreads(){ return (uint8_t)boost::lexical_cast<int>(m_threadEntry.get_text()); }
  bool getZProject(){ return m_zprojBox.get_active(); }
  int getDivisor();
  bool getWriteTables(){ return m_writeTablesBox.get_active(); }
  uint8_t getPostChan(){ return (uint8_t)boost::lexical_cast<int>(m_postChanEntry.get_text()); }

};

#endif
