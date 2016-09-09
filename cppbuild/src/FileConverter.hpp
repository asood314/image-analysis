#ifndef FILECONVERTER_HPP
#define FILECONVERTER_HPP

#include <gtkmm-2.4/gtkmm.h>
#include "FileManager.hpp"
#include "ImageAnalysisToolkit.hpp"

class FileConverter : public Gtk::FileChooserDialog
{

protected:
  static FileManager::input_file readV00(FileManager* fm, ImageAnalysisToolkit* kit, std::vector<ImRecord*>* recs, std::string filename);
  static void writeV00(FileManager* fm, ImageAnalysisToolkit* kit, std::vector<ImRecord*>* recs, std::string filename, int seriesID);
  static FileManager::input_file readV01(FileManager* fm, ImageAnalysisToolkit* kit, std::vector<ImRecord*>* recs, std::string filename);
  static void writeV01(FileManager* fm, ImageAnalysisToolkit* kit, std::vector<ImRecord*>* recs, std::string filename, int seriesID);
  //static void convert(int oldVersion, int newVersion, std::string filename);

  FileManager m_fileManager;
  ImageAnalysisToolkit m_kit;
  std::vector<ImRecord*> m_records;
  std::vector<std::string> m_files;

  Gtk::ComboBox m_oldBox,m_newBox;
  Gtk::TreeModelColumn<int> m_colVersion;
  Gtk::TreeModelColumn<std::string> m_colString;
  Gtk::TreeModel::ColumnRecord m_columns;
  Glib::RefPtr<Gtk::ListStore> m_refTreeModelOld,m_refTreeModelNew;
  Gtk::Label m_oldLabel,m_newLabel;

  Gtk::Button m_selectButton,m_convertButton,m_quitButton;
  Glib::RefPtr<Gtk::ListStore> m_refTreeModel;
  Gtk::TreeModel::ColumnRecord m_columnRecord;
  Gtk::TreeModelColumn<Glib::ustring> m_fileNameColumn;
  Gtk::TreeView m_treeView;
  Gtk::ScrolledWindow m_treeWindow;

  Gtk::HBox m_hbox1,m_hbox2;
  Gtk::VBox m_vbox1,m_vbox2;

  void on_select_button_clicked();
  void on_convert_button_clicked();
  void on_quit_button_clicked();

public:
  FileConverter();
  virtual ~FileConverter(){}
  static FileManager::input_file read(FileManager* fm, ImageAnalysisToolkit* kit, std::vector<ImRecord*>* recs, std::string filename, int version);
  static void write(FileManager* fm, ImageAnalysisToolkit* kit, std::vector<ImRecord*>* recs, std::string filename, int seriesID, int version);
};

#endif
