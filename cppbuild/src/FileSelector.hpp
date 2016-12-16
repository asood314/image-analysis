#ifndef FILE_SELECTOR_HPP
#define FILE_SELECTOR_HPP

#include <gtkmm-2.4/gtkmm.h>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include "FileManager.hpp"
#include "ImageAnalysisToolkit.hpp"
#include "FileConverter.hpp"

class FileSelector : public Gtk::FileChooserDialog
{

protected:
  FileManager* m_fileManager;
  std::vector<std::string> m_names;
  ImageAnalysisToolkit* m_toolkit;
  std::vector<ImRecord*>* m_recs;
  int m_nextRow;
  Gtk::Button m_selectButton;
  Glib::RefPtr<Gtk::ListStore> m_refTreeModel;
  Gtk::TreeModel::ColumnRecord m_columnRecord;
  Gtk::TreeModelColumn<int> m_idColumn;
  Gtk::TreeModelColumn<Glib::ustring> m_fileNameColumn;
  Gtk::TreeView m_treeView;
  Gtk::ScrolledWindow m_treeWindow;
  Gtk::RadioButton m_diskoveryButton, m_manualButton;
  Gtk::Label m_orderLabel;
  Gtk::Entry m_orderField;
  Gtk::Label m_dimensionLabel;
  Gtk::Entry m_wField,m_zField,m_pField,m_tField;
  Gtk::Label m_wLabel,m_zLabel,m_pLabel,m_tLabel;
  Gtk::Label m_resolutionLabel,m_nameLabel;
  Gtk::Entry m_resolutionEntry,m_seriesName;
  Gtk::Button m_addButton,m_clearButton,m_cancelButton,m_finishButton;
  Gtk::HBox m_hboxBig,m_hbox1,m_hbox2,m_hbox3,m_hbox4,m_hbox5,m_hbox6;
  Gtk::VBox m_configBox,m_vbox1,m_wBox,m_zBox,m_pBox,m_tBox;
  void on_select_button_clicked();
  void on_add_button_clicked();
  void on_clear_button_clicked();
  void on_cancel_button_clicked();
  void on_finish_button_clicked();

public:
  FileSelector(FileManager* fm, ImageAnalysisToolkit* iat, std::vector<ImRecord*>* recs, const Glib::ustring& title, Gtk::FileChooserAction act=Gtk::FILE_CHOOSER_ACTION_OPEN);
  virtual ~FileSelector();

};

#endif
