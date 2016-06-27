#include "FileSelector.hpp"
#include <iostream>
#include <stdio.h>

FileSelector::FileSelector(FileManager* fm, const Glib::ustring& title, Gtk::FileChooserAction act) :
  Gtk::FileChooserDialog(title,act),
  m_selectButton("Select File"),
  m_diskoveryButton("Auto load Diskovery files"),
  m_manualButton("Manual"),
  m_orderLabel("Frame Ordering:"),
  m_dimensionLabel("Image Series Dimensions:"),
  m_wLabel("wavelengths"),m_zLabel("z-frames"),m_pLabel("positions"),m_tLabel("timepoints"),
  m_addButton("Add Image Series"),
  m_cancelButton("Cancel"),
  m_nextRow(0)
{
  m_fileManager = fm;
  m_columnRecord.add(m_idColumn);
  m_columnRecord.add(m_fileNameColumn);
  m_refTreeModel = Gtk::ListStore::create(m_columnRecord);
  m_treeView.set_model(m_refTreeModel);
  m_treeView.append_column("",m_idColumn);
  m_treeView.append_column("Selected File(s)",m_fileNameColumn);
  m_treeWindow.add(m_treeView);
  m_hboxBig.pack_start(m_treeWindow, Gtk::PACK_EXPAND_WIDGET);

  Gtk::RadioButton::Group group = m_diskoveryButton.get_group();
  m_manualButton.set_group(group);
  m_manualButton.set_active();
  m_orderField.set_max_length(5);
  m_orderField.set_width_chars(10);
  m_orderField.set_text("PTWZ");
  m_hbox1.pack_start(m_diskoveryButton, Gtk::PACK_SHRINK);
  m_hbox1.pack_start(m_manualButton, Gtk::PACK_SHRINK);
  m_vbox1.pack_end(m_orderField, Gtk::PACK_SHRINK);
  m_vbox1.pack_end(m_orderLabel, Gtk::PACK_SHRINK);
  m_hbox1.pack_start(m_vbox1, Gtk::PACK_EXPAND_PADDING);
  m_configBox.pack_start(m_hbox1, Gtk::PACK_SHRINK);
  m_configBox.pack_start(m_dimensionLabel, Gtk::PACK_SHRINK);

  m_wField.set_max_length(15);
  m_zField.set_max_length(15);
  m_pField.set_max_length(15);
  m_tField.set_max_length(15);
  m_wField.set_width_chars(10);
  m_zField.set_width_chars(10);
  m_pField.set_width_chars(10);
  m_tField.set_width_chars(10);
  m_wField.set_text("1");
  m_zField.set_text("1");
  m_pField.set_text("1");
  m_tField.set_text("1");
  m_wBox.pack_start(m_wLabel, Gtk::PACK_SHRINK);
  m_zBox.pack_start(m_zLabel, Gtk::PACK_SHRINK);
  m_pBox.pack_start(m_pLabel, Gtk::PACK_SHRINK);
  m_tBox.pack_start(m_tLabel, Gtk::PACK_SHRINK);
  m_wBox.pack_start(m_wField, Gtk::PACK_SHRINK);
  m_zBox.pack_start(m_zField, Gtk::PACK_SHRINK);
  m_pBox.pack_start(m_pField, Gtk::PACK_SHRINK);
  m_tBox.pack_start(m_tField, Gtk::PACK_SHRINK);
  m_hbox2.pack_start(m_wBox, Gtk::PACK_EXPAND_PADDING);
  m_hbox2.pack_start(m_zBox, Gtk::PACK_EXPAND_PADDING);
  m_hbox2.pack_start(m_pBox, Gtk::PACK_EXPAND_PADDING);
  m_hbox2.pack_start(m_tBox, Gtk::PACK_EXPAND_PADDING);
  m_configBox.pack_start(m_hbox2, Gtk::PACK_SHRINK);

  m_hbox3.pack_end(m_addButton, Gtk::PACK_SHRINK);
  m_hbox3.pack_end(m_cancelButton, Gtk::PACK_SHRINK);
  m_configBox.pack_start(m_hbox3, Gtk::PACK_SHRINK);
  m_hboxBig.pack_start(m_configBox, Gtk::PACK_SHRINK);

  m_selectButton.signal_clicked().connect(sigc::mem_fun(*this, &FileSelector::on_select_button_clicked));
  m_addButton.signal_clicked().connect(sigc::mem_fun(*this, &FileSelector::on_add_button_clicked));
  m_cancelButton.signal_clicked().connect(sigc::mem_fun(*this, &FileSelector::on_cancel_button_clicked));
  
  Gtk::Box* vbox = get_vbox();
  vbox->pack_start(m_selectButton, Gtk::PACK_SHRINK);
  vbox->pack_start(m_hboxBig, Gtk::PACK_SHRINK);
  vbox->show_all_children();
  add_button("Finished", Gtk::RESPONSE_OK);
}

FileSelector::~FileSelector()
{
}

void FileSelector::on_select_button_clicked()
{
  Gtk::TreeModel::Row row = *(m_refTreeModel->append());
  row[m_idColumn] = m_nextRow;
  m_nextRow++;
  row[m_fileNameColumn] = get_filename();
  m_names.push_back(get_filename());
}

void FileSelector::on_add_button_clicked()
{
  FileManager::input_file infile;
  infile.fnames = m_names;
  Glib::ustring ordString = m_orderField.get_text();
  infile.order[3-ordString.find_first_of('W')] = FileManager::WAVELENGTH;
  infile.order[3-ordString.find_first_of('Z')] = FileManager::ZSLICE;
  infile.order[3-ordString.find_first_of('P')] = FileManager::POSITION;
  infile.order[3-ordString.find_first_of('T')] = FileManager::TIMEPOINT;
  sscanf(m_wField.get_text().c_str(),"%hhu",&infile.nw);
  sscanf(m_zField.get_text().c_str(),"%hhu",&infile.nz);
  sscanf(m_tField.get_text().c_str(),"%hhu",&infile.nt);
  sscanf(m_pField.get_text().c_str(),"%hhu",&infile.np);
  m_fileManager->addInputFile(infile);
  m_refTreeModel->clear();
  m_names.clear();
}

void FileSelector::on_cancel_button_clicked()
{
  m_refTreeModel->clear();
  m_names.clear();
}
