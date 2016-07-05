#include "ConfigurationDialog.hpp"

ConfigurationDialog::ConfigurationDialog(ImageAnalysisToolkit* iat, uint8_t nchan, int maxThreads, bool zproject) :
  m_masterBox("Set master channel"),
  m_modeLabel("Select master mode"),
  m_pfiLabel("Set max puncta finding iterations"),
  m_saturationLabel("Set bit depth"),
  m_lwLabel("Set local window size (um^2)"),
  m_radiusLabel("Set min puncta radius (um)"),
  m_reclusterLabel("Set recluster threshold (N * min radius)"),
  m_nrtLabel("Set noise removal threshold (N * min radius)"),
  m_peakLabel("Set peak intensity threshold (N std above local median)"),
  m_floorLabel("Set floor intensity threshold (N std above local median)"),
  m_overlapButton("Use overlap threshold"),
  m_distanceButton("Use distance threshold"),
  m_thresholdLabel("Set colocalization threshold"),
  m_reqAllButton("Require all channels colocalize"),
  m_requirementsLabel("Set required colocalizations"),
  m_descriptionLabel("Add synapse description"),
  m_addButton("Add Synapse Type"),
  m_scIndex(0),
  m_threadLabel("Set maximum number of threads"),
  m_zprojBox("Do Z-projections")
{
  m_toolkit = iat;

  int master;
  if(m_toolkit->master() == 255){
    m_masterBox.set_active(false);
    master = -1;
  }
  else{
    m_masterBox.set_active(true);
    master = m_toolkit->master();
  }
  m_masterEntry.set_max_length(3);
  m_masterEntry.set_width_chars(5);
  m_masterEntry.set_text(boost::lexical_cast<std::string>(master));
  m_vbox1.pack_start(m_masterBox, Gtk::PACK_SHRINK);
  m_vbox1.pack_start(m_masterEntry, Gtk::PACK_SHRINK);
  m_columns.add(m_colMode);
  m_columns.add(m_colString);
  m_refTreeModel = Gtk::ListStore::create(m_columns);
  m_modeBox.set_model(m_refTreeModel);
  Gtk::TreeModel::Row row = *(m_refTreeModel->append());
  row[m_colMode] = ImageAnalysisToolkit::OVERRIDE;
  row[m_colString] = "OVERRIDE";
  row = *(m_refTreeModel->append());
  row[m_colMode] = ImageAnalysisToolkit::OR;
  row[m_colString] = "OR";
  m_modeBox.pack_start(m_colString);
  m_vbox2.pack_start(m_modeLabel, Gtk::PACK_SHRINK);
  m_vbox2.pack_start(m_modeBox, Gtk::PACK_SHRINK);
  m_hbox1.pack_start(m_vbox1, Gtk::PACK_SHRINK);
  m_hbox1.pack_start(m_vbox2, Gtk::PACK_SHRINK);
  m_analysisBox.pack_start(m_hbox1, Gtk::PACK_SHRINK);

  m_saturationEntry.set_max_length(3);
  m_saturationEntry.set_width_chars(5);
  m_saturationEntry.set_text(boost::lexical_cast<std::string>((int)m_toolkit->getBitDepth()));
  m_hbox2.pack_start(m_saturationLabel, Gtk::PACK_SHRINK);
  m_hbox2.pack_start(m_saturationEntry, Gtk::PACK_SHRINK);
  m_analysisBox.pack_start(m_hbox2, Gtk::PACK_SHRINK);

  m_pfiEntry.set_max_length(3);
  m_pfiEntry.set_width_chars(5);
  m_pfiEntry.set_text(boost::lexical_cast<std::string>((int)m_toolkit->maxPunctaFindingIterations()));
  m_hbox3.pack_start(m_pfiLabel, Gtk::PACK_SHRINK);
  m_hbox3.pack_start(m_pfiEntry, Gtk::PACK_SHRINK);
  m_analysisBox.pack_start(m_hbox3, Gtk::PACK_SHRINK);

  m_radiusEntry.set_max_length(3);
  m_radiusEntry.set_width_chars(5);
  m_radiusEntry.set_text(boost::lexical_cast<std::string>(m_toolkit->minPunctaRadius()));
  m_hbox4.pack_start(m_radiusLabel, Gtk::PACK_SHRINK);
  m_hbox4.pack_start(m_radiusEntry, Gtk::PACK_SHRINK);
  m_analysisBox.pack_start(m_hbox4, Gtk::PACK_SHRINK);

  m_nrtEntry.set_max_length(3);
  m_nrtEntry.set_width_chars(5);
  m_nrtEntry.set_text(boost::lexical_cast<std::string>(m_toolkit->noiseRemovalThreshold()));
  m_hbox5.pack_start(m_nrtLabel, Gtk::PACK_SHRINK);
  m_hbox5.pack_start(m_nrtEntry, Gtk::PACK_SHRINK);
  m_analysisBox.pack_start(m_hbox5, Gtk::PACK_SHRINK);

  m_reclusterEntry.set_max_length(3);
  m_reclusterEntry.set_width_chars(5);
  m_reclusterEntry.set_text(boost::lexical_cast<std::string>(m_toolkit->reclusterThreshold()));
  m_hbox6.pack_start(m_reclusterLabel, Gtk::PACK_SHRINK);
  m_hbox6.pack_start(m_reclusterEntry, Gtk::PACK_SHRINK);
  m_analysisBox.pack_start(m_hbox6, Gtk::PACK_SHRINK);

  m_lwEntry.set_max_length(3);
  m_lwEntry.set_width_chars(5);
  m_lwEntry.set_text(boost::lexical_cast<std::string>(m_toolkit->localWindow()));
  m_hbox7.pack_start(m_lwLabel, Gtk::PACK_SHRINK);
  m_hbox7.pack_start(m_lwEntry, Gtk::PACK_SHRINK);
  m_analysisBox.pack_start(m_hbox7, Gtk::PACK_SHRINK);

  m_peakEntry.set_max_length(3);
  m_peakEntry.set_width_chars(5);
  m_peakEntry.set_text(boost::lexical_cast<std::string>(m_toolkit->peakThreshold()));
  m_hbox8.pack_start(m_peakLabel, Gtk::PACK_SHRINK);
  m_hbox8.pack_start(m_peakEntry, Gtk::PACK_SHRINK);
  m_analysisBox.pack_start(m_hbox8, Gtk::PACK_SHRINK);

  m_floorEntry.set_max_length(3);
  m_floorEntry.set_width_chars(5);
  m_floorEntry.set_text(boost::lexical_cast<std::string>(m_toolkit->floorThreshold()));
  m_hbox9.pack_start(m_floorLabel, Gtk::PACK_SHRINK);
  m_hbox9.pack_start(m_floorEntry, Gtk::PACK_SHRINK);
  m_analysisBox.pack_start(m_hbox9, Gtk::PACK_SHRINK);

  m_synapseChannels.assign(nchan, NULL);
  for(uint8_t i = 0; i < nchan; i++){
    m_synapseChannels.at(i) = new  Gtk::CheckButton(boost::lexical_cast<std::string>((int)i));
    m_synapseChannels.at(i)->set_active(true);
    m_synapseBox.pack_start(*(m_synapseChannels.at(i)), Gtk::PACK_SHRINK);
  }

  m_synapseBox.pack_start(m_thresholdLabel, Gtk::PACK_SHRINK);
  Gtk::RadioButton::Group group = m_overlapButton.get_group();
  m_distanceButton.set_group(group);
  m_thresholdEntry.set_max_length(5);
  m_thresholdEntry.set_width_chars(5);
  m_overlapButton.set_active();
  m_thresholdEntry.set_text("0");
  m_vbox3.pack_start(m_overlapButton, Gtk::PACK_SHRINK);
  m_vbox3.pack_start(m_distanceButton, Gtk::PACK_SHRINK);
  m_hbox10.pack_start(m_vbox3, Gtk::PACK_SHRINK);
  m_hbox10.pack_start(m_thresholdEntry, Gtk::PACK_SHRINK);
  m_synapseBox.pack_start(m_hbox10, Gtk::PACK_SHRINK);

  m_reqAllButton.set_active(true);
  m_requirementsEntry.set_max_length(20);
  m_requirementsEntry.set_width_chars(10);
  std::ostringstream s;
  s << "{0";
  for(uint8_t i = 1; i < nchan; i++) s << "," << (int)i;
  s << "}";
  m_requirementsEntry.set_text(s.str());
  m_synapseBox.pack_start(m_reqAllButton, Gtk::PACK_SHRINK);
  m_hbox11.pack_start(m_requirementsLabel, Gtk::PACK_SHRINK);
  m_hbox11.pack_start(m_requirementsEntry, Gtk::PACK_SHRINK);
  m_synapseBox.pack_start(m_hbox11, Gtk::PACK_SHRINK);

  m_descriptionEntry.set_max_length(200);
  m_descriptionEntry.set_text("All colocalized");
  m_addButton.signal_clicked().connect(sigc::mem_fun(*this,&ConfigurationDialog::on_add_button_clicked));
  m_synapseBox.pack_start(m_descriptionLabel, Gtk::PACK_SHRINK);
  m_synapseBox.pack_start(m_descriptionEntry, Gtk::PACK_SHRINK);
  m_synapseBox.pack_start(m_addButton, Gtk::PACK_SHRINK);

  m_columns2.add(m_indexColumn);
  m_columns2.add(m_descriptionColumn);
  //m_columns2.add(m_reqColumn);
  m_refTreeModel2 = Gtk::ListStore::create(m_columns2);
  m_treeView.set_model(m_refTreeModel2);
  m_treeView.append_column("Current Synapse Definitions",m_descriptionColumn);
  //m_treeView.append_column("Colocalization Requirements",m_reqColumn);
  std::vector<SynapseCollection*> syncol = m_toolkit->synapseDefinitions();
  for(std::vector<SynapseCollection*>::iterator it = syncol.begin(); it != syncol.end(); it++){
    Gtk::TreeModel::Row row = *(m_refTreeModel2->append());
    row[m_indexColumn] = m_scIndex;
    m_scIndex++;
    row[m_descriptionColumn] = (*it)->description();
  }
  m_synapseBox.pack_start(m_treeView,Gtk::PACK_EXPAND_WIDGET);
  m_synapseWindow.add(m_synapseBox);

  m_threadEntry.set_max_length(3);
  m_threadEntry.set_width_chars(5);
  m_threadEntry.set_text(boost::lexical_cast<std::string>(maxThreads));
  m_zprojBox.set_active(zproject);
  m_hbox12.pack_start(m_threadLabel, Gtk::PACK_SHRINK);
  m_hbox12.pack_start(m_threadEntry, Gtk::PACK_SHRINK);
  m_batchBox.pack_start(m_hbox12, Gtk::PACK_SHRINK);
  m_batchBox.pack_start(m_zprojBox, Gtk::PACK_SHRINK);

  m_Notebook.append_page(m_analysisBox,"Analysis Config");
  m_Notebook.append_page(m_synapseWindow,"Synapse Config");
  m_Notebook.append_page(m_batchBox,"Batch Config");
  get_vbox()->pack_start(m_Notebook);
  add_button("Cancel", Gtk::RESPONSE_CANCEL);
  add_button("Finished", Gtk::RESPONSE_OK);

  show_all_children();
}

ConfigurationDialog::~ConfigurationDialog()
{
  for(std::vector<Gtk::CheckButton*>::iterator it = m_synapseChannels.begin(); it != m_synapseChannels.end(); it++){
    if(*it) delete *it;
  }
}

void ConfigurationDialog::on_add_button_clicked()
{
  std::vector<uint8_t> chans;
  for(uint8_t i = 0; i < m_synapseChannels.size(); i++){
    if(m_synapseChannels.at(i)->get_active()) chans.push_back(i);
  }
  SynapseCollection* sc = new SynapseCollection(chans);
  if(m_overlapButton.get_active()){
    sc->setUseOverlap(true);
    sc->setOverlapThreshold((uint32_t)boost::lexical_cast<int>(m_thresholdEntry.get_text()));
  }
  else{
    sc->setUseOverlap(false);
    sc->setDistanceThreshold(boost::lexical_cast<double>(m_thresholdEntry.get_text()));
  }
  if(m_reqAllButton.get_active()) sc->setRequireAll(true);
  else{
    sc->setRequireAll(false);
    boost::char_separator<char> sep("{}");
    std::string reqs = m_requirementsEntry.get_text();
    boost::tokenizer< boost::char_separator<char> > tokens(reqs,sep);
    for(const auto& t : tokens){
      boost::char_separator<char> sep2(",");
      boost::tokenizer< boost::char_separator<char> > tokens2(t,sep2);
      std::vector<uint8_t> reqChans;
      for(const auto& t2 : tokens2) reqChans.push_back((uint8_t)boost::lexical_cast<int>(t2));
      sc->addRequiredColocalization(reqChans);
    }
  }
  sc->setDescription(m_descriptionEntry.get_text());
  m_toolkit->addSynapseDefinition(sc);
  Gtk::TreeModel::Row row = *(m_refTreeModel2->append());
  row[m_indexColumn] = m_scIndex;
  m_scIndex++;
  row[m_descriptionColumn] = sc->description();  
}

uint8_t ConfigurationDialog::getMaster()
{
  if(m_masterBox.get_active()) return (uint8_t)boost::lexical_cast<int>(m_masterEntry.get_text());
  return 255;
}

ImageAnalysisToolkit::MasterMode ConfigurationDialog::getMode()
{
  Gtk::TreeModel::iterator it = m_modeBox.get_active();
  if(it) return (*it)[m_colMode];
  return ImageAnalysisToolkit::OVERRIDE;
}
