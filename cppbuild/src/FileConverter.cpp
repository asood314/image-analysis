#include "FileConverter.hpp"

FileConverter::FileConverter() :
  Gtk::FileChooserDialog("",Gtk::FILE_CHOOSER_ACTION_OPEN),
  m_selectButton("Select File"),m_convertButton("Convert File(s)"),m_quitButton("Quit"),
  m_oldLabel("From Version:"),m_newLabel("To Version:")
{
  m_columnRecord.add(m_fileNameColumn);
  m_refTreeModel = Gtk::ListStore::create(m_columnRecord);
  m_treeView.set_model(m_refTreeModel);
  m_treeView.append_column("Selected File(s)",m_fileNameColumn);
  m_treeWindow.add(m_treeView);
  m_hbox1.pack_start(m_treeWindow,Gtk::PACK_EXPAND_WIDGET);
  
  m_columns.add(m_colVersion);
  m_columns.add(m_colString);
  m_refTreeModelOld = Gtk::ListStore::create(m_columns);
  m_oldBox.set_model(m_refTreeModelOld);
  Gtk::TreeModel::Row row = *(m_refTreeModelOld->append());
  row[m_colVersion] = 0;
  row[m_colString] = "0.0";
  m_oldBox.pack_start(m_colString);
  m_oldBox.set_active(0);

  m_refTreeModelNew = Gtk::ListStore::create(m_columns);
  m_newBox.set_model(m_refTreeModelNew);
  row = *(m_refTreeModelNew->append());
  row[m_colVersion] = 0;
  row[m_colString] = "0.0";
  m_newBox.pack_start(m_colString);
  m_newBox.set_active(0);

  m_vbox1.pack_start(m_oldLabel,Gtk::PACK_SHRINK);
  m_vbox1.pack_start(m_oldBox,Gtk::PACK_SHRINK);
  m_vbox2.pack_start(m_newLabel,Gtk::PACK_SHRINK);
  m_vbox2.pack_start(m_newBox,Gtk::PACK_SHRINK);
  m_hbox1.pack_start(m_vbox1,Gtk::PACK_SHRINK,20);
  m_hbox1.pack_start(m_vbox2,Gtk::PACK_SHRINK,20);
  
  m_selectButton.signal_clicked().connect(sigc::mem_fun(*this, &FileConverter::on_select_button_clicked));
  m_convertButton.signal_clicked().connect(sigc::mem_fun(*this, &FileConverter::on_convert_button_clicked));
  m_quitButton.signal_clicked().connect(sigc::mem_fun(*this, &FileConverter::on_quit_button_clicked));
  m_hbox2.pack_end(m_quitButton,Gtk::PACK_SHRINK,10);
  m_hbox2.pack_end(m_convertButton,Gtk::PACK_SHRINK,10);
  
  Gtk::Box* vbox = get_vbox();
  vbox->pack_start(m_selectButton, Gtk::PACK_SHRINK);
  vbox->pack_start(m_hbox1, Gtk::PACK_SHRINK);
  vbox->pack_start(m_hbox2, Gtk::PACK_SHRINK);
  vbox->show_all_children();
}

void FileConverter::on_select_button_clicked()
{
  std::string phil = get_filename();
  Gtk::TreeModel::Row row = *(m_refTreeModel->append());
  row[m_fileNameColumn] = phil;
  m_files.push_back(phil);
}

void FileConverter::on_quit_button_clicked()
{
  response(Gtk::RESPONSE_OK);
}

void FileConverter::on_convert_button_clicked()
{
  Gtk::TreeModel::iterator it_old = m_oldBox.get_active();
  int v_old = (*it_old)[m_colVersion];
  Gtk::TreeModel::iterator it_new = m_newBox.get_active();
  int v_new = (*it_new)[m_colVersion];
  for(std::vector<std::string>::iterator it = m_files.begin(); it != m_files.end(); it++){
    for(std::vector<ImRecord*>::iterator jt = m_records.begin(); jt != m_records.end(); jt++){
      if(*jt) delete *jt;
    }
    m_records.clear();
    m_fileManager.clearInputFiles();
    if(v_old == 0) readV00(&m_fileManager,&m_kit,&m_records,*it);
    else continue;
    if(v_new == 0) writeV00(&m_fileManager,&m_kit,&m_records,*it,0);
    else continue;
  }
  m_refTreeModel->clear();
  m_files.clear();
}

FileManager::input_file FileConverter::read(FileManager* fm, ImageAnalysisToolkit* kit, std::vector<ImRecord*>* recs, std::string filename, int version)
{
  if(version == 0) return readV00(fm,kit,recs,filename);
  else return readV01(fm,kit,recs,filename);
}

void FileConverter::write(FileManager* fm, ImageAnalysisToolkit* kit, std::vector<ImRecord*>* recs, std::string filename, int seriesID, int version)
{
  if(version == 0) writeV00(fm,kit,recs,filename,seriesID);
  else writeV01(fm,kit,recs,filename,seriesID);
}

FileManager::input_file FileConverter::readV00(FileManager* fm, ImageAnalysisToolkit* kit, std::vector<ImRecord*>* recs, std::string filename)
{
  std::ifstream fin(filename.c_str(),std::ifstream::binary);
  FileManager::input_file infile;
  char buf[500];
  fin.read(buf,4);
  int len = NiaUtils::convertToInt(buf[0],buf[1],buf[2],buf[3]);
  fin.read(buf,len);
  infile.sname = std::string(buf,len);
  fin.read(buf,12);
  infile.nw = (int)buf[0];
  infile.nz = (int)buf[1];
  infile.np = (int)buf[2];
  infile.nt = (int)buf[3];
  char minibuf[4];
  for(int i = 4; i < 8; i++) minibuf[i-4] = buf[i];
  Glib::ustring ordString = std::string(minibuf);
  infile.order[3-ordString.find_first_of('W')] = FileManager::WAVELENGTH;
  infile.order[3-ordString.find_first_of('Z')] = FileManager::ZSLICE;
  infile.order[3-ordString.find_first_of('P')] = FileManager::POSITION;
  infile.order[3-ordString.find_first_of('T')] = FileManager::TIMEPOINT;
  int nfiles = NiaUtils::convertToInt(buf[8],buf[9],buf[10],buf[11]);
  for(int i = 0; i < nfiles; i++){
    fin.read(buf,4);
    len = NiaUtils::convertToInt(buf[0],buf[1],buf[2],buf[3]);
    fin.read(buf,len);
    std::string fname(buf,len);
    infile.fnames.push_back(fname);
  }
  kit->read(fin);
  if(recs){
    int prevSize = recs->size();
    fin.read(buf,1);
    int nrecs = infile.np*infile.nt;
    if((int)buf[0] < 1) nrecs *= infile.nz;
    for(int i = 0; i < nrecs; i++){
      ImRecord* rec = new ImRecord();
      rec->read(fin,0);
      recs->push_back(rec);
    }
    infile.resolutionXY = recs->at(prevSize)->resolutionXY();
  }
  //fm->addInputFile(infile);
  fin.close();
  return infile;
}

void FileConverter::writeV00(FileManager* fm, ImageAnalysisToolkit* kit, std::vector<ImRecord*>* recs, std::string filename, int seriesID)
{
  std::ofstream fout(filename.c_str(),std::ofstream::binary);
  fm->saveInputFiles(fout,seriesID);
  kit->write(fout);
  char buf[1];
  if(recs->size() > fm->getNP(0)*fm->getNT(0)) buf[0] = 0;
  else buf[0] = 1;
  fout.write(buf,1);
  for(std::vector<ImRecord*>::iterator rit = recs->begin(); rit != recs->end(); rit++){
    (*rit)->write(fout,0);
  }
  fout.close();
}

FileManager::input_file FileConverter::readV01(FileManager* fm, ImageAnalysisToolkit* kit, std::vector<ImRecord*>* recs, std::string filename)
{
  std::ifstream fin(filename.c_str(),std::ifstream::binary);
  FileManager::input_file infile;
  char buf[500];
  fin.read(buf,7);
  fin.read(buf,4);
  int len = NiaUtils::convertToInt(buf[0],buf[1],buf[2],buf[3]);
  fin.read(buf,len);
  infile.sname = std::string(buf,len);
  fin.read(buf,12);
  infile.nw = (int)buf[0];
  infile.nz = (int)buf[1];
  infile.np = (int)buf[2];
  infile.nt = (int)buf[3];
  char minibuf[4];
  for(int i = 4; i < 8; i++) minibuf[i-4] = buf[i];
  Glib::ustring ordString = std::string(minibuf);
  infile.order[3-ordString.find_first_of('W')] = FileManager::WAVELENGTH;
  infile.order[3-ordString.find_first_of('Z')] = FileManager::ZSLICE;
  infile.order[3-ordString.find_first_of('P')] = FileManager::POSITION;
  infile.order[3-ordString.find_first_of('T')] = FileManager::TIMEPOINT;
  int nfiles = NiaUtils::convertToInt(buf[8],buf[9],buf[10],buf[11]);
  for(int i = 0; i < nfiles; i++){
    fin.read(buf,4);
    len = NiaUtils::convertToInt(buf[0],buf[1],buf[2],buf[3]);
    fin.read(buf,len);
    std::string fname(buf,len);
    infile.fnames.push_back(fname);
  }
  kit->read(fin);
  if(recs){
    int prevSize = recs->size();
    fin.read(buf,1);
    int nrecs = infile.np*infile.nt;
    if((int)buf[0] < 1) nrecs *= infile.nz;
    for(int i = 0; i < nrecs; i++){
      ImRecord* rec = new ImRecord();
      rec->read(fin,1);
      recs->push_back(rec);
    }
    infile.resolutionXY = recs->at(prevSize)->resolutionXY();
  }
  //fm->addInputFile(infile);
  fin.close();
  return infile;
}

void FileConverter::writeV01(FileManager* fm, ImageAnalysisToolkit* kit, std::vector<ImRecord*>* recs, std::string filename, int seriesID)
{
  char buf[10];
  std::ofstream fout(filename.c_str(),std::ofstream::binary);
  buf[0] = 'V';
  buf[1] = 'V';
  buf[2] = 'V';
  NiaUtils::writeIntToBuffer(buf,3,1);
  fout.write(buf,7);
  fm->saveInputFiles(fout,seriesID);
  kit->write(fout);
  if(recs->size() > fm->getNP(0)*fm->getNT(0)) buf[0] = 0;
  else buf[0] = 1;
  fout.write(buf,1);
  for(std::vector<ImRecord*>::iterator rit = recs->begin(); rit != recs->end(); rit++){
    (*rit)->write(fout,1);
  }
  fout.close();
}
