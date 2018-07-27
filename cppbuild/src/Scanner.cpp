#include "Scanner.hpp"

Scanner::Scanner(int argc, char** argv)
{
  set_title("Parameter Scanner - NIA v0.7");
  set_default_size(500,700);
  m_maxThreads = boost::thread::hardware_concurrency() - 1;
  m_activeThreads = 0;
  m_configFile = "default.cfg";
  m_outputDir = "";

  for(int i = 1; i < argc; i++){
    std::string str = argv[i];
    if(str.find("outdir=") == 0) m_outputDir = str.substr(7);
    else if(str.find("config=") == 0) m_configFile = str.substr(7);
    else m_inputFiles.push_back(str);
  }

  add(m_vbox);
  m_refActionGroup = Gtk::ActionGroup::create();
  m_refActionGroup->add(Gtk::Action::create("fileMenu","File"));
  m_refActionGroup->add(Gtk::Action::create("run","Run Scan"),Gtk::AccelKey("<control>R"),sigc::mem_fun(*this, &Scanner::run));
  m_refUIManager = Gtk::UIManager::create();
  m_refUIManager->insert_action_group(m_refActionGroup);
  add_accel_group(m_refUIManager->get_accel_group());

  Glib::ustring ui_info = 
    "<ui>"
    " <menubar name='menuBar'>"
    "  <menu action='fileMenu'>"
    "   <menuitem action='run'/>"
    "  </menu>"
    " </menubar>"
    "</ui>";
  try{ m_refUIManager->add_ui_from_string(ui_info); }
  catch(const Glib::Error& ex){}
  Gtk::Widget* pMenubar = m_refUIManager->get_widget("/menuBar");
  if(pMenubar) m_vbox.pack_start(*pMenubar, Gtk::PACK_SHRINK);

  m_textBuffer = Gtk::TextBuffer::create();
  m_textView.set_buffer(m_textBuffer);
  m_scrolledWindow.add(m_textView);
  m_scrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_AUTOMATIC);
  m_vbox.pack_start(m_scrolledWindow, Gtk::PACK_EXPAND_WIDGET);
  show_all_children();
}

void Scanner::run()
{
  std::vector<double> kernelWidth;
  std::vector<double> windowSize;
  std::vector<double> peakThreshold;
  std::vector<double> floorThreshold;
  std::vector<int> signalFindingIterations;
  std::vector<double> reclusterThreshold;
  std::ostringstream dispstr;
  dispstr << "--------------------SCAN PARAMETERS:\n--------------------\n\n";
  
  //----- Read configuration file -----
  std::ifstream phil(m_configFile);
  if(!(phil.is_open())){
    hide();
    return;
  }
  std::string line;
  boost::char_separator<char> sep(":");
  while(getline(phil,line)){
    dispstr << line << "\n";
    boost::tokenizer< boost::char_separator<char> > tokens(line,sep);
    boost::tokenizer< boost::char_separator<char> >::iterator tit = tokens.begin();
    std::string param = *tit;
    tit++;
    boost::char_separator<char> sep2(",");
    boost::tokenizer< boost::char_separator<char> > tokens2(*tit,sep2);
    if(param.compare("kernel width") == 0){
      for(const auto& t2 : tokens2) kernelWidth.push_back(boost::lexical_cast<double>(t2));
    }
    else if(param.compare("window size") == 0){
      for(const auto& t2 : tokens2) windowSize.push_back(boost::lexical_cast<double>(t2));
    }
    else if(param.compare("peak threshold") == 0){
      for(const auto& t2 : tokens2) peakThreshold.push_back(boost::lexical_cast<double>(t2));
    }
    else if(param.compare("floor threshold") == 0){
      for(const auto& t2 : tokens2) floorThreshold.push_back(boost::lexical_cast<double>(t2));
    }
    else if(param.compare("signal finding iterations") == 0){
      for(const auto& t2 : tokens2) signalFindingIterations.push_back(boost::lexical_cast<int>(t2));
    }
    else if(param.compare("recluster threshold") == 0){
      for(const auto& t2 : tokens2) reclusterThreshold.push_back(boost::lexical_cast<double>(t2));
    }
  }
  phil.close();
  //-----------------------------------

  dispstr << "\n--------------------\nINPUT FILES:\n--------------------\n\n";

  //----- Load images -----
  int tot_tasks = 0;
  std::vector<int> series_ntasks;
  std::vector< std::vector<ImRecord*>* > templateRecords;
  m_fileManager = new FileManager();
  for(std::vector<std::string>::iterator fit = m_inputFiles.begin(); fit != m_inputFiles.end(); fit++){
    dispstr << *fit << "\n";
    ImageAnalysisToolkit tmp;
    std::vector<ImRecord*>* recs = new std::vector<ImRecord*>();
    recs->clear();
    FileManager::input_file infile = FileConverter::read(m_fileManager,&tmp,recs,*fit,nia::niaVersion);
    templateRecords.push_back(recs);
    int itasks = infile.np * infile.nt;
    series_ntasks.push_back(itasks);
    tot_tasks += itasks;
    m_fileManager->addInputFile(infile);
  }

  m_textBuffer->set_text(dispstr.str());
  show_all_children();

  tot_tasks *= kernelWidth.size()*windowSize.size()*peakThreshold.size()*floorThreshold.size()*reclusterThreshold.size()*signalFindingIterations.size();
  m_progressWindow.launch(tot_tasks);
  m_fileManager->reset();
  ImSeries* m_data = m_fileManager->loadNext();
  int seriesID = 0;
  int scanID = 0;
  while(m_data){
    for(std::vector<double>::iterator kwit = kernelWidth.begin(); kwit != kernelWidth.end(); kwit++){
      for(std::vector<double>::iterator wsit = windowSize.begin(); wsit != windowSize.end(); wsit++){
	for(std::vector<double>::iterator ptit = peakThreshold.begin(); ptit != peakThreshold.end(); ptit++){
	  for(std::vector<double>::iterator ftit = floorThreshold.begin(); ftit != floorThreshold.end(); ftit++){
	    for(std::vector<double>::iterator rtit = reclusterThreshold.begin(); rtit != reclusterThreshold.end(); rtit++){
	      for(std::vector<int>::iterator sfit = signalFindingIterations.begin(); sfit != signalFindingIterations.end(); sfit++){
		ImageAnalysisToolkit* toolkit = new ImageAnalysisToolkit();
		toolkit->setSaturationThreshold(65534);
		toolkit->setKernelWidth(*kwit);
		toolkit->setLocalWindow(*wsit);
		toolkit->setPeakThreshold(*ptit);
		toolkit->setFloorThreshold(*ftit);
		toolkit->setReclusterThreshold(*rtit);
		toolkit->setMaxSignalFindingIterations(*sfit);
		std::vector<ImRecord*>* scanRecs = new std::vector<ImRecord*>();
		scanRecs->clear();
		for(int i = 0; i < series_ntasks[seriesID]; i++) scanRecs->push_back(templateRecords[seriesID]->at(i)->emptyCopy());
		m_records.push_back(scanRecs);
		for(int i = 0; i < scanRecs->at(0)->nSynapseCollections(); i++) toolkit->addSynapseDefinition(scanRecs->at(0)->getSynapseCollection(i)->emptyCopy());
		m_ntasks.push_back(series_ntasks[seriesID]);
		for(int ps = 0; ps < m_data->npos(); ps++){
		  for(int tm = 0; tm < m_data->nt(); tm++){
		    while(m_activeThreads == m_maxThreads) boost::this_thread::sleep(boost::posix_time::millisec(60000));
		    m_mtx.lock();
		    m_threadpool.create_thread(boost::bind(&Scanner::run_analysis, this, m_data, toolkit, scanID, ps, tm, seriesID));
		    m_activeThreads++;
		    boost::this_thread::sleep(boost::posix_time::millisec(1000));
		    m_mtx.unlock();
		  }
		}
		boost::this_thread::sleep(boost::posix_time::millisec(1000));
		scanID++;
	      }
	    }
	  }
	}
      }
    }
    m_threadpool.join_all();
    nia::nout << "Finished parameter scan for sample " << m_fileManager->getName(seriesID) << "\n";
    delete m_data;
    m_data = m_fileManager->loadNext();
    seriesID++;
  }
  delete m_fileManager;
  hide();
  //-----------------------
}

void Scanner::run_analysis(ImSeries* data, ImageAnalysisToolkit* kit, int scanID, int p, int t, int seriesID)
{
  //---------- Windows only ----------
  //SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED);
  //----------------------------------
  std::ostringstream scanstr;
  scanstr << "scan-kw" << kit->kernelWidth() << "ws" << kit->localWindow() << "pt" << kit->peakThreshold() << "ft" << kit->floorThreshold() << "rt" << kit->reclusterThreshold() << "sfi" << kit->maxSignalFindingIterations();
  nia::nout << "Starting sample " << m_fileManager->getName(seriesID) << ", xy position " << p << ", timepoint " << t << ", " << scanstr.str() << "\n";
  std::cout << "Starting sample " << m_fileManager->getName(seriesID) << ", xy position " << p << ", timepoint " << t << ", " << scanstr.str() << std::endl;
  ImStack* stack = data->fourLocation(p,t);
  ImRecord* record = m_records[scanID]->at(p*data->nt() + t);
  if(!record){
    record = new ImRecord(stack->nwaves(),stack->frame(0,0)->width(),stack->frame(0,0)->height());
    record->setResolutionXY(data->resolutionXY());
    m_records[scanID]->at(p*data->nt() + t) = record;
  }
  kit->standardAnalysis(stack,record,-1);
  boost::lock_guard<boost::mutex> guard(m_mtx);
  m_ntasks[scanID] -= 1;
  if(m_ntasks[scanID] == 0){
    std::ostringstream filename;
    filename << m_outputDir << scanstr.str() << "_" << m_fileManager->getName(seriesID) << ".nia";
    FileConverter::write(m_fileManager,kit,m_records[scanID],filename.str(),seriesID,nia::niaVersion);
    std::vector<ImRecord*>* rvec = m_records[scanID];
    for(std::vector<ImRecord*>::iterator rit = rvec->begin(); rit != rvec->end(); rit++) delete *rit;
    delete rvec;
    delete kit;
    nia::nout << "Finished sample " << m_fileManager->getName(seriesID) << ", " << scanstr.str() << "\n";
    std::cout << "Finished sample " << m_fileManager->getName(seriesID) << ", " << scanstr.str() << std::endl;
  }
  m_activeThreads--;
  m_progressWindow.taskCompleted();
  //---------- Windows only ----------
  //SetThreadExecutionState(ES_CONTINUOUS);
  //----------------------------------
}
