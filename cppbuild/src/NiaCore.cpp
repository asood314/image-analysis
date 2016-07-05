#include "NiaCore.hpp"
#include "FileSelector.hpp"

NiaCore::NiaCore()
{
  init();
}

NiaCore::NiaCore(ImSeries* ser)
{
  m_viewer.setData(ser);
  init();
}

NiaCore::~NiaCore()
{
}

void NiaCore::init()
{
  set_title("Neuron Image Analysis");
  set_default_size(1000,1100);
  add(m_vbox);

  m_refActionGroup = Gtk::ActionGroup::create();
  m_refActionGroup->add(Gtk::Action::create("fileMenu","File"));
  m_refActionGroup->add(Gtk::Action::create("load","Load Images"),Gtk::AccelKey("<control>O"),sigc::mem_fun(*this, &NiaCore::on_menu_load));
  m_refActionGroup->add(Gtk::Action::create("quit","Quit"));
  m_refActionGroup->add(Gtk::Action::create("viewMenu","View"));
  m_refActionGroup->add(Gtk::Action::create("single","Single Wavelength"));
  m_refActionGroup->add(Gtk::Action::create("chan0","Channel 0"),Gtk::AccelKey("<control>0"),sigc::bind<uint8_t>(sigc::mem_fun(m_viewer, &NiaViewer::setWavelength),0));
  m_refActionGroup->add(Gtk::Action::create("chan1","Channel 1"),Gtk::AccelKey("<control>1"),sigc::bind<uint8_t>(sigc::mem_fun(m_viewer, &NiaViewer::setWavelength),1));
  m_refActionGroup->add(Gtk::Action::create("chan2","Channel 2"),Gtk::AccelKey("<control>2"),sigc::bind<uint8_t>(sigc::mem_fun(m_viewer, &NiaViewer::setWavelength),2));
  m_refActionGroup->add(Gtk::Action::create("composite","Color Composite"),Gtk::AccelKey("<control>C"),sigc::bind<NiaViewer::ImageMode>(sigc::mem_fun(m_viewer, &NiaViewer::setMode),NiaViewer::RGB));
  m_refActionGroup->add(Gtk::Action::create("setRed","Set Red Channel"));
  m_refActionGroup->add(Gtk::Action::create("redChan0","Channel 0"),sigc::bind<uint8_t>(sigc::mem_fun(m_viewer, &NiaViewer::setRed),0));
  m_refActionGroup->add(Gtk::Action::create("redChan1","Channel 1"),sigc::bind<uint8_t>(sigc::mem_fun(m_viewer, &NiaViewer::setRed),1));
  m_refActionGroup->add(Gtk::Action::create("redChan2","Channel 2"),sigc::bind<uint8_t>(sigc::mem_fun(m_viewer, &NiaViewer::setRed),2));
  m_refActionGroup->add(Gtk::Action::create("setGreen","Set Green Channel"));
  m_refActionGroup->add(Gtk::Action::create("greenChan0","Channel 0"),sigc::bind<uint8_t>(sigc::mem_fun(m_viewer, &NiaViewer::setGreen),0));
  m_refActionGroup->add(Gtk::Action::create("greenChan1","Channel 1"),sigc::bind<uint8_t>(sigc::mem_fun(m_viewer, &NiaViewer::setGreen),1));
  m_refActionGroup->add(Gtk::Action::create("greenChan2","Channel 2"),sigc::bind<uint8_t>(sigc::mem_fun(m_viewer, &NiaViewer::setGreen),2));
  m_refActionGroup->add(Gtk::Action::create("setBlue","Set Blue Channel"));
  m_refActionGroup->add(Gtk::Action::create("blueChan0","Channel 0"),sigc::bind<uint8_t>(sigc::mem_fun(m_viewer, &NiaViewer::setBlue),0));
  m_refActionGroup->add(Gtk::Action::create("blueChan1","Channel 1"),sigc::bind<uint8_t>(sigc::mem_fun(m_viewer, &NiaViewer::setBlue),1));
  m_refActionGroup->add(Gtk::Action::create("blueChan2","Channel 2"),sigc::bind<uint8_t>(sigc::mem_fun(m_viewer, &NiaViewer::setBlue),2));
  m_refActionGroup->add(Gtk::Action::create("navMenu","Navigate"));
  m_refActionGroup->add(Gtk::Action::create("prevP","Previous Position"),Gtk::AccelKey(GDK_KEY_Left,Gdk::SHIFT_MASK),sigc::mem_fun(m_viewer, &NiaViewer::prevPosition));
  m_refActionGroup->add(Gtk::Action::create("nextP","Next Position"),Gtk::AccelKey(GDK_KEY_Right,Gdk::SHIFT_MASK),sigc::mem_fun(m_viewer, &NiaViewer::nextPosition));
  m_refActionGroup->add(Gtk::Action::create("prevT","Previous Timepoint"),Gtk::AccelKey(GDK_KEY_Left,Gdk::HYPER_MASK),sigc::mem_fun(m_viewer, &NiaViewer::prevTimepoint));
  m_refActionGroup->add(Gtk::Action::create("nextT","Next Timepoint"),Gtk::AccelKey(GDK_KEY_Right,Gdk::HYPER_MASK),sigc::mem_fun(m_viewer, &NiaViewer::nextTimepoint));
  m_refActionGroup->add(Gtk::Action::create("prevZ","Previous Z-slice"),Gtk::AccelKey(GDK_KEY_Down,Gdk::SHIFT_MASK),sigc::mem_fun(m_viewer, &NiaViewer::prevZ));
  m_refActionGroup->add(Gtk::Action::create("nextZ","Next Z-slice"),Gtk::AccelKey(GDK_KEY_Up,Gdk::SHIFT_MASK),sigc::mem_fun(m_viewer, &NiaViewer::nextZ));
  m_refActionGroup->add(Gtk::Action::create("analyzeMenu","Analyze"));
  m_refActionGroup->add(Gtk::Action::create("batch","Start Batch Process"),Gtk::AccelKey("<control>B"),sigc::mem_fun(*this, &NiaCore::on_start_batch_jobs));

  m_refUIManager = Gtk::UIManager::create();
  m_refUIManager->insert_action_group(m_refActionGroup);
  add_accel_group(m_refUIManager->get_accel_group());

  Glib::ustring ui_info = 
    "<ui>"
    " <menubar name='menuBar'>"
    "  <menu action='fileMenu'>"
    "   <menuitem action='load'/>"
    "   <menuitem action='quit'/>"
    "  </menu>"
    "  <menu action='viewMenu'>"
    "   <menu action='single'>"
    "    <menuitem action='chan0'/>"
    "    <menuitem action='chan1'/>"
    "    <menuitem action='chan2'/>"
    "   </menu>"
    "   <menuitem action='composite'/>"
    "   <menu action='setRed'>"
    "    <menuitem action='redChan0'/>"
    "    <menuitem action='redChan1'/>"
    "    <menuitem action='redChan2'/>"
    "   </menu>"
    "   <menu action='setGreen'>"
    "    <menuitem action='greenChan0'/>"
    "    <menuitem action='greenChan1'/>"
    "    <menuitem action='greenChan2'/>"
    "   </menu>"
    "   <menu action='setBlue'>"
    "    <menuitem action='blueChan0'/>"
    "    <menuitem action='blueChan1'/>"
    "    <menuitem action='blueChan2'/>"
    "   </menu>"
    "  </menu>"
    "  <menu action='navMenu'>"
    "   <menuitem action='prevP'/>"
    "   <menuitem action='nextP'/>"
    "   <menuitem action='prevT'/>"
    "   <menuitem action='nextT'/>"
    "   <menuitem action='prevZ'/>"
    "   <menuitem action='nextZ'/>"
    "  </menu>"
    "  <menu action='analyzeMenu'>"
    "   <menuitem action='batch'/>"
    "  </menu>"
    " </menubar>"
    "</ui>";
  try{ m_refUIManager->add_ui_from_string(ui_info); }
  catch(const Glib::Error& ex){}
  Gtk::Widget* pMenubar = m_refUIManager->get_widget("/menuBar");
  if(pMenubar) m_vbox.pack_start(*pMenubar, Gtk::PACK_SHRINK);

  m_vbox.pack_start(m_viewer, Gtk::PACK_EXPAND_WIDGET);

  show_all_children();
}

void NiaCore::on_menu_load()
{
  FileSelector fcd(&m_fileManager,"",Gtk::FILE_CHOOSER_ACTION_OPEN);
  fcd.set_transient_for(*this);

  Gtk::FileFilter filt;
  filt.set_name("TIFF files");
  filt.add_pattern("*.tif");
  filt.add_pattern("*.tiff");
  filt.add_pattern("*.TIF");
  filt.add_pattern("*.TIFF");
  fcd.add_filter(filt);

  int result = fcd.run();
  if(result == Gtk::RESPONSE_OK){
    m_viewer.setData(m_fileManager.load());
    m_fileManager.clearInputFiles();
  }
}

void NiaCore::on_start_batch_jobs()
{
  FileSelector fs(m_batchService.fileManager(),"",Gtk::FILE_CHOOSER_ACTION_OPEN);
  fs.set_transient_for(*this);

  Gtk::FileFilter filt;
  filt.set_name("TIFF files");
  filt.add_pattern("*.tif");
  filt.add_pattern("*.tiff");
  filt.add_pattern("*.TIF");
  filt.add_pattern("*.TIFF");
  fs.add_filter(filt);

  int result = fs.run();
  if(result != Gtk::RESPONSE_OK) return;
  fs.hide();
  
  ImageAnalysisToolkit* biat = m_batchService.iat();
  ConfigurationDialog cd(biat,m_batchService.fileManager()->getNW(0),m_batchService.maxThreads(),m_batchService.zproject());
  cd.set_transient_for(*this);
  result = cd.run();
  if(result != Gtk::RESPONSE_OK) return;
  biat->setMaster(cd.getMaster());
  biat->setMode(cd.getMode());
  biat->setMaxPunctaFindingIterations(cd.getPunctaFindingIterations());
  biat->setLocalWindow(cd.getLocalWindow());
  biat->setMinPunctaRadius(cd.getRadius());
  biat->setReclusterThreshold(cd.getRecluster());
  biat->setNoiseRemovalThreshold(cd.getNoiseRemovalThreshold());
  biat->setPeakThreshold(cd.getPeak());
  biat->setFloorThreshold(cd.getFloor());
  m_batchService.setMaxThreads(cd.getThreads());
  m_batchService.setZProject(cd.getZProject());
  cd.hide();
  
  Gtk::FileChooserDialog fcd("",Gtk::FILE_CHOOSER_ACTION_SAVE);
  fcd.set_transient_for(*this);
  fcd.add_button("Cancel",Gtk::RESPONSE_CANCEL);
  fcd.add_button("Run",Gtk::RESPONSE_OK);
  result = fcd.run();
  if(result == Gtk::RESPONSE_OK){
    m_batchService.setName(fcd.get_filename());
    m_batchService.run();
  }
}
