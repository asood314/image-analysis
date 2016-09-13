#include "FileSelector.hpp"
#include <iostream>
#include <stdio.h>

FileSelector::FileSelector(FileManager* fm, ImageAnalysisToolkit* iat, std::vector<ImRecord*>* recs, const Glib::ustring& title, Gtk::FileChooserAction act) :
  Gtk::FileChooserDialog(title,act),
  m_selectButton("Select File"),
  m_diskoveryButton("Auto load Diskovery files"),
  m_manualButton("Manual"),
  m_orderLabel("Frame Ordering:"),
  m_dimensionLabel("Image Series Dimensions:"),
  m_wLabel("wavelengths"),m_zLabel("z-frames"),m_pLabel("positions"),m_tLabel("timepoints"),
  m_resolutionLabel("Image Resolution: "),
  m_nameLabel("Name: "),
  m_addButton("Add Image Series"),
  m_clearButton("Clear"),
  m_cancelButton("Cancel"),
  m_finishButton("Finished"),
  m_nextRow(0)
{
  m_fileManager = fm;
  m_recs = recs;
  m_toolkit = iat;
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
  m_orderField.set_text("WZPT");
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

  m_resolutionEntry.set_max_length(10);
  m_resolutionEntry.set_width_chars(10);
  m_resolutionEntry.set_text("0.115");
  m_hbox3.pack_start(m_resolutionLabel, Gtk::PACK_SHRINK);
  m_hbox3.pack_end(m_resolutionEntry, Gtk::PACK_SHRINK);
  m_configBox.pack_start(m_hbox3, Gtk::PACK_SHRINK);

  m_seriesName.set_max_length(30);
  m_seriesName.set_width_chars(15);
  m_seriesName.set_text("Series1");
  m_hbox4.pack_start(m_nameLabel, Gtk::PACK_SHRINK);
  m_hbox4.pack_end(m_seriesName, Gtk::PACK_SHRINK);
  m_configBox.pack_start(m_hbox4, Gtk::PACK_SHRINK);

  m_hbox5.pack_start(m_clearButton, Gtk::PACK_SHRINK);
  m_hbox5.pack_start(m_addButton, Gtk::PACK_SHRINK);
  m_configBox.pack_start(m_hbox5, Gtk::PACK_SHRINK);
  m_hboxBig.pack_start(m_configBox, Gtk::PACK_SHRINK);

  m_hbox6.pack_end(m_finishButton, Gtk::PACK_SHRINK, 10);
  m_hbox6.pack_end(m_cancelButton, Gtk::PACK_SHRINK, 10);

  m_selectButton.signal_clicked().connect(sigc::mem_fun(*this, &FileSelector::on_select_button_clicked));
  m_addButton.signal_clicked().connect(sigc::mem_fun(*this, &FileSelector::on_add_button_clicked));
  m_clearButton.signal_clicked().connect(sigc::mem_fun(*this, &FileSelector::on_clear_button_clicked));
  m_cancelButton.signal_clicked().connect(sigc::mem_fun(*this, &FileSelector::on_cancel_button_clicked));
  m_finishButton.signal_clicked().connect(sigc::mem_fun(*this, &FileSelector::on_finish_button_clicked));
  
  Gtk::Box* vbox = get_vbox();
  vbox->pack_start(m_selectButton, Gtk::PACK_SHRINK);
  vbox->pack_start(m_hboxBig, Gtk::PACK_SHRINK);
  vbox->pack_start(m_hbox6, Gtk::PACK_SHRINK);
  vbox->show_all_children();
}

FileSelector::~FileSelector()
{
}

void FileSelector::on_select_button_clicked()
{
  std::string phil = get_filename();
  if(phil.find(".nia") != std::string::npos){
    std::ifstream fin(phil.c_str(),std::ifstream::binary);
    char buf[500];
    fin.read(buf,7);
    fin.close();
    FileManager::input_file infile;
    if(!(buf[0] == 'V' && buf[1] == 'V' && buf[2] == 'V')) infile = FileConverter::read(m_fileManager,m_toolkit,m_recs,phil,0);
    else{
      int version = NiaUtils::convertToInt(buf[3],buf[4],buf[5],buf[6]);
      infile = FileConverter::read(m_fileManager,m_toolkit,m_recs,phil,version);
    }
    m_seriesName.set_text(infile.sname);
    m_wField.set_text(boost::lexical_cast<std::string>(infile.nw));
    m_zField.set_text(boost::lexical_cast<std::string>(infile.nz));
    m_pField.set_text(boost::lexical_cast<std::string>(infile.np));
    m_tField.set_text(boost::lexical_cast<std::string>(infile.nt));
    m_orderField.set_text(FileManager::orderString(infile.order));
    for(unsigned i = 0; i < infile.fnames.size(); i++){
      Gtk::TreeModel::Row row = *(m_refTreeModel->append());
      row[m_idColumn] = m_nextRow;
      m_nextRow++;
      row[m_fileNameColumn] = infile.fnames[i];
      m_names.push_back(infile.fnames[i]);
    }
    m_resolutionEntry.set_text(boost::lexical_cast<std::string>(m_recs->at(m_recs->size()-1)->resolutionXY()));
    /*
    fin.read(buf,len);
    m_seriesName.set_text(std::string(buf,len));
    fin.read(buf,12);
    int nw = (int)buf[0];
    int nz = (int)buf[1];
    int np = (int)buf[2];
    int nt = (int)buf[3];
    m_wField.set_text(boost::lexical_cast<std::string>(nw));
    m_zField.set_text(boost::lexical_cast<std::string>(nz));
    m_pField.set_text(boost::lexical_cast<std::string>(np));
    m_tField.set_text(boost::lexical_cast<std::string>(nt));
    char minibuf[4];
    for(int i = 4; i < 8; i++) minibuf[i-4] = buf[i];
    m_orderField.set_text(std::string(minibuf));
    int nfiles = NiaUtils::convertToInt(buf[8],buf[9],buf[10],buf[11]);
    for(int i = 0; i < nfiles; i++){
      Gtk::TreeModel::Row row = *(m_refTreeModel->append());
      row[m_idColumn] = m_nextRow;
      m_nextRow++;
      fin.read(buf,4);
      len = NiaUtils::convertToInt(buf[0],buf[1],buf[2],buf[3]);
      fin.read(buf,len);
      std::string fname(buf,len);
      row[m_fileNameColumn] = fname;
      m_names.push_back(fname);
    }
    m_toolkit->read(fin);
    if(m_recs){
      int prevSize = m_recs->size();
      fin.read(buf,1);
      int nrecs = np*nt;
      if((int)buf[0] < 1) nrecs *= nz;
      for(int i = 0; i < nrecs; i++){
	ImRecord* rec = new ImRecord();
	rec->read(fin);
	m_recs->push_back(rec);
      }
      m_resolutionEntry.set_text(boost::lexical_cast<std::string>(m_recs->at(prevSize)->resolutionXY()));
    }
    fin.close();
    */
  }
  else{
    Gtk::TreeModel::Row row = *(m_refTreeModel->append());
    row[m_idColumn] = m_nextRow;
    m_nextRow++;
    row[m_fileNameColumn] = phil;
    m_names.push_back(phil);
  }
}

void FileSelector::on_add_button_clicked()
{
  FileManager::input_file infile;
  infile.fnames = m_names;
  infile.sname = m_seriesName.get_text();
  Glib::ustring ordString = m_orderField.get_text();
  infile.order[3-ordString.find_first_of('W')] = FileManager::WAVELENGTH;
  infile.order[3-ordString.find_first_of('Z')] = FileManager::ZSLICE;
  infile.order[3-ordString.find_first_of('P')] = FileManager::POSITION;
  infile.order[3-ordString.find_first_of('T')] = FileManager::TIMEPOINT;
  sscanf(m_wField.get_text().c_str(),"%d",&infile.nw);
  sscanf(m_zField.get_text().c_str(),"%d",&infile.nz);
  sscanf(m_tField.get_text().c_str(),"%d",&infile.nt);
  sscanf(m_pField.get_text().c_str(),"%d",&infile.np);
  infile.resolutionXY = boost::lexical_cast<double>(m_resolutionEntry.get_text());
  m_fileManager->addInputFile(infile);
  m_refTreeModel->clear();
  m_names.clear();
}

void FileSelector::on_clear_button_clicked()
{
  m_refTreeModel->clear();
  m_names.clear();
}

void FileSelector::on_cancel_button_clicked()
{
  response(Gtk::RESPONSE_CANCEL);
}

void FileSelector::on_finish_button_clicked()
{
  if(m_fileManager->nInputFiles() == 0){
    Gtk::MessageDialog messenger(*this,"No image series has been added.");
    messenger.run();
    return;
  }
  if(m_refTreeModel->children().size() > 0){
    Gtk::MessageDialog messenger(*this,"Files selected but not added.");
    messenger.run();
    return;
  }
  response(Gtk::RESPONSE_OK);
}
