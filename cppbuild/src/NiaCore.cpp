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
  m_threads.join_all();
}

void NiaCore::init()
{
  set_title("Neuron Image Analysis v0.7");
  set_default_size(1200,1000);
  add(m_vbox);

  m_refActionGroup = Gtk::ActionGroup::create();
  m_refActionGroup->add(Gtk::Action::create("fileMenu","File"));
  m_refActionGroup->add(Gtk::Action::create("load","Load Images"),Gtk::AccelKey("<control>O"),sigc::mem_fun(*this, &NiaCore::on_menu_load));
  m_refActionGroup->add(Gtk::Action::create("unscale","Fix Myka's Images"),sigc::mem_fun(m_viewer, &NiaViewer::unscale));
  m_refActionGroup->add(Gtk::Action::create("loadMMR","Load MetaMorph Regions"),sigc::mem_fun(*this, &NiaCore::on_load_regions));
  m_refActionGroup->add(Gtk::Action::create("loadStorm","Load Storm Data"),sigc::mem_fun(*this, &NiaCore::on_load_storm_data_clicked));
  m_refActionGroup->add(Gtk::Action::create("save","Save"),Gtk::AccelKey("<control><shift>S"),sigc::mem_fun(*this, &NiaCore::on_save));
  m_refActionGroup->add(Gtk::Action::create("screenshot","Save Screenshot"),sigc::mem_fun(*this, &NiaCore::on_save_screenshot));
  m_refActionGroup->add(Gtk::Action::create("timeseries","Save Time Series"),sigc::mem_fun(*this, &NiaCore::on_save_timeseries));
  m_refActionGroup->add(Gtk::Action::create("quit","Quit"),Gtk::AccelKey("<control>Q"),sigc::mem_fun(*this, &NiaCore::on_quit));
  m_refActionGroup->add(Gtk::Action::create("viewMenu","View"));
  m_refActionGroup->add(Gtk::Action::create("single","Single Wavelength"));
  m_refActionGroup->add(Gtk::Action::create("chan0","Channel 0"),Gtk::AccelKey("<control>0"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setWavelength),0));
  m_refActionGroup->add(Gtk::Action::create("chan1","Channel 1"),Gtk::AccelKey("<control>1"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setWavelength),1));
  m_refActionGroup->add(Gtk::Action::create("chan2","Channel 2"),Gtk::AccelKey("<control>2"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setWavelength),2));
  m_refActionGroup->add(Gtk::Action::create("chan3","Channel 3"),Gtk::AccelKey("<control>3"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setWavelength),3));
  m_refActionGroup->add(Gtk::Action::create("chan4","Channel 4"),Gtk::AccelKey("<control>4"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setWavelength),4));
  m_refActionGroup->add(Gtk::Action::create("chan5","Channel 5"),Gtk::AccelKey("<control>5"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setWavelength),5));
  m_refActionGroup->add(Gtk::Action::create("chan6","Channel 6"),Gtk::AccelKey("<control>6"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setWavelength),6));
  m_refActionGroup->add(Gtk::Action::create("chan7","Channel 7"),Gtk::AccelKey("<control>7"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setWavelength),7));
  m_refActionGroup->add(Gtk::Action::create("chan8","Channel 8"),Gtk::AccelKey("<control>8"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setWavelength),8));
  m_refActionGroup->add(Gtk::Action::create("chan9","Channel 9"),Gtk::AccelKey("<control>9"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setWavelength),9));
  m_refActionGroup->add(Gtk::Action::create("composite","Color Composite"),Gtk::AccelKey("<control>C"),sigc::bind<NiaViewer::ImageMode>(sigc::mem_fun(m_viewer, &NiaViewer::setMode),NiaViewer::RGB));
  m_refActionGroup->add(Gtk::Action::create("setRed","Set Red Channel"));
  m_refActionGroup->add(Gtk::Action::create("redChan0","Channel 0"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setRed),0));
  m_refActionGroup->add(Gtk::Action::create("redChan1","Channel 1"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setRed),1));
  m_refActionGroup->add(Gtk::Action::create("redChan2","Channel 2"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setRed),2));
  m_refActionGroup->add(Gtk::Action::create("redChan3","Channel 3"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setRed),3));
  m_refActionGroup->add(Gtk::Action::create("redChan4","Channel 4"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setRed),4));
  m_refActionGroup->add(Gtk::Action::create("redChan5","Channel 5"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setRed),5));
  m_refActionGroup->add(Gtk::Action::create("redChan6","Channel 6"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setRed),6));
  m_refActionGroup->add(Gtk::Action::create("redChan7","Channel 7"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setRed),7));
  m_refActionGroup->add(Gtk::Action::create("redChan8","Channel 8"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setRed),8));
  m_refActionGroup->add(Gtk::Action::create("redChan9","Channel 9"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setRed),9));
  m_refActionGroup->add(Gtk::Action::create("setGreen","Set Green Channel"));
  m_refActionGroup->add(Gtk::Action::create("greenChan0","Channel 0"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setGreen),0));
  m_refActionGroup->add(Gtk::Action::create("greenChan1","Channel 1"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setGreen),1));
  m_refActionGroup->add(Gtk::Action::create("greenChan2","Channel 2"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setGreen),2));
  m_refActionGroup->add(Gtk::Action::create("greenChan3","Channel 3"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setGreen),3));
  m_refActionGroup->add(Gtk::Action::create("greenChan4","Channel 4"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setGreen),4));
  m_refActionGroup->add(Gtk::Action::create("greenChan5","Channel 5"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setGreen),5));
  m_refActionGroup->add(Gtk::Action::create("greenChan6","Channel 6"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setGreen),6));
  m_refActionGroup->add(Gtk::Action::create("greenChan7","Channel 7"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setGreen),7));
  m_refActionGroup->add(Gtk::Action::create("greenChan8","Channel 8"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setGreen),8));
  m_refActionGroup->add(Gtk::Action::create("greenChan9","Channel 9"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setGreen),9));
  m_refActionGroup->add(Gtk::Action::create("setBlue","Set Blue Channel"));
  m_refActionGroup->add(Gtk::Action::create("blueChan0","Channel 0"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setBlue),0));
  m_refActionGroup->add(Gtk::Action::create("blueChan1","Channel 1"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setBlue),1));
  m_refActionGroup->add(Gtk::Action::create("blueChan2","Channel 2"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setBlue),2));
  m_refActionGroup->add(Gtk::Action::create("blueChan3","Channel 3"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setBlue),3));
  m_refActionGroup->add(Gtk::Action::create("blueChan4","Channel 4"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setBlue),4));
  m_refActionGroup->add(Gtk::Action::create("blueChan5","Channel 5"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setBlue),5));
  m_refActionGroup->add(Gtk::Action::create("blueChan6","Channel 6"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setBlue),6));
  m_refActionGroup->add(Gtk::Action::create("blueChan7","Channel 7"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setBlue),7));
  m_refActionGroup->add(Gtk::Action::create("blueChan8","Channel 8"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setBlue),8));
  m_refActionGroup->add(Gtk::Action::create("blueChan9","Channel 9"),sigc::bind<int>(sigc::mem_fun(m_viewer, &NiaViewer::setBlue),9));
  m_refActionGroup->add(Gtk::Action::create("zproj","Z-projection"),Gtk::AccelKey("<control>Z"),sigc::mem_fun(m_viewer, &NiaViewer::zproject));
  m_refActionGroup->add(Gtk::Action::create("scale","Adjust Scale"),Gtk::AccelKey("<control>A"),sigc::mem_fun(m_viewer, &NiaViewer::showScaleBox));
  m_refActionGroup->add(Gtk::Action::create("zoomin","Zoom In"),Gtk::AccelKey(GDK_KEY_equal,Gdk::CONTROL_MASK),sigc::mem_fun(m_viewer, &NiaViewer::zoomIn));
  m_refActionGroup->add(Gtk::Action::create("zoomout","Zoom Out"),Gtk::AccelKey(GDK_KEY_minus,Gdk::CONTROL_MASK),sigc::mem_fun(m_viewer, &NiaViewer::zoomOut));
  m_refActionGroup->add(Gtk::Action::create("unzoom","Unzoom"),Gtk::AccelKey("<control>U"),sigc::mem_fun(m_viewer, &NiaViewer::unzoom));
  m_refActionGroup->add(Gtk::Action::create("maskMenu","Masks"));
  m_refActionGroup->add(Gtk::Action::create("sigMask","Signal Mask"),Gtk::AccelKey("<control>F"),sigc::mem_fun(m_viewer, &NiaViewer::toggleSignalMask));
  m_refActionGroup->add(Gtk::Action::create("puncMask","Puncta Mask"),Gtk::AccelKey("<control>P"),sigc::mem_fun(m_viewer, &NiaViewer::togglePunctaMask));
  m_refActionGroup->add(Gtk::Action::create("synMask","Synapse Mask"),Gtk::AccelKey("<control>S"),sigc::mem_fun(m_viewer, &NiaViewer::toggleSynapseMask));
  m_refActionGroup->add(Gtk::Action::create("regMask","Region Mask"),Gtk::AccelKey("<control>R"),sigc::mem_fun(m_viewer, &NiaViewer::toggleRegionMask));
  m_refActionGroup->add(Gtk::Action::create("segMask","Segment Mask"),Gtk::AccelKey("<control><shift>R"),sigc::mem_fun(m_viewer, &NiaViewer::toggleSegmentMask));
  m_refActionGroup->add(Gtk::Action::create("stormMask","Storm Mask"),Gtk::AccelKey("<control>T"),sigc::mem_fun(m_viewer, &NiaViewer::toggleStormMask));
  m_refActionGroup->add(Gtk::Action::create("conMap","Contour Map"),sigc::mem_fun(m_viewer, &NiaViewer::showContourMap));
  m_refActionGroup->add(Gtk::Action::create("clearMask","Clear Masks"),Gtk::AccelKey("<control>N"),sigc::mem_fun(m_viewer, &NiaViewer::clearMasks));
  m_refActionGroup->add(Gtk::Action::create("viewStorm","Storm Image"),Gtk::AccelKey("<control><shift>T"),sigc::mem_fun(m_viewer, &NiaViewer::toggleImageType));
  m_refActionGroup->add(Gtk::Action::create("stormZWin","Set Z Window"),sigc::mem_fun(m_viewer, &NiaViewer::setZWindow));
  m_refActionGroup->add(Gtk::Action::create("derivative","Derivative"),sigc::mem_fun(m_viewer, &NiaViewer::showDerivative));
  m_refActionGroup->add(Gtk::Action::create("derivative2","2nd Derivative"),sigc::mem_fun(m_viewer, &NiaViewer::showDerivative2));
  m_refActionGroup->add(Gtk::Action::create("stats","Image Statistics"),sigc::mem_fun(m_viewer, &NiaViewer::showStats));
  m_refActionGroup->add(Gtk::Action::create("navMenu","Navigate"));
  //Windows keys
  m_refActionGroup->add(Gtk::Action::create("prevP","Previous Position"),Gtk::AccelKey(GDK_KEY_Left,Gdk::CONTROL_MASK),sigc::mem_fun(m_viewer, &NiaViewer::prevPosition));
  m_refActionGroup->add(Gtk::Action::create("nextP","Next Position"),Gtk::AccelKey(GDK_KEY_Right,Gdk::CONTROL_MASK),sigc::mem_fun(m_viewer, &NiaViewer::nextPosition));
  m_refActionGroup->add(Gtk::Action::create("prevT","Previous Timepoint"),Gtk::AccelKey(GDK_KEY_Left,Gdk::CONTROL_MASK | Gdk::SHIFT_MASK),sigc::mem_fun(m_viewer, &NiaViewer::prevTimepoint));
  m_refActionGroup->add(Gtk::Action::create("nextT","Next Timepoint"),Gtk::AccelKey(GDK_KEY_Right,Gdk::CONTROL_MASK | Gdk::SHIFT_MASK),sigc::mem_fun(m_viewer, &NiaViewer::nextTimepoint));
  m_refActionGroup->add(Gtk::Action::create("prevZ","Previous Z-slice"),Gtk::AccelKey(GDK_KEY_Down,Gdk::CONTROL_MASK),sigc::mem_fun(m_viewer, &NiaViewer::prevZ));
  m_refActionGroup->add(Gtk::Action::create("nextZ","Next Z-slice"),Gtk::AccelKey(GDK_KEY_Up,Gdk::CONTROL_MASK),sigc::mem_fun(m_viewer, &NiaViewer::nextZ));
  //OSX keys
  //m_refActionGroup->add(Gtk::Action::create("prevP","Previous Position"),Gtk::AccelKey(GDK_KEY_Left,Gdk::SHIFT_MASK),sigc::mem_fun(m_viewer, &NiaViewer::prevPosition));
  //m_refActionGroup->add(Gtk::Action::create("nextP","Next Position"),Gtk::AccelKey(GDK_KEY_Right,Gdk::SHIFT_MASK),sigc::mem_fun(m_viewer, &NiaViewer::nextPosition));
  //m_refActionGroup->add(Gtk::Action::create("prevT","Previous Timepoint"),Gtk::AccelKey(GDK_KEY_Left,Gdk::HYPER_MASK),sigc::mem_fun(m_viewer, &NiaViewer::prevTimepoint));
  //m_refActionGroup->add(Gtk::Action::create("nextT","Next Timepoint"),Gtk::AccelKey(GDK_KEY_Right,Gdk::HYPER_MASK),sigc::mem_fun(m_viewer, &NiaViewer::nextTimepoint));
  //m_refActionGroup->add(Gtk::Action::create("prevZ","Previous Z-slice"),Gtk::AccelKey(GDK_KEY_Down,Gdk::SHIFT_MASK),sigc::mem_fun(m_viewer, &NiaViewer::prevZ));
  //m_refActionGroup->add(Gtk::Action::create("nextZ","Next Z-slice"),Gtk::AccelKey(GDK_KEY_Up,Gdk::SHIFT_MASK),sigc::mem_fun(m_viewer, &NiaViewer::nextZ));
  m_refActionGroup->add(Gtk::Action::create("analyzeMenu","Analyze"));
  m_refActionGroup->add(Gtk::Action::create("config","Configure"),sigc::mem_fun(*this, &NiaCore::on_configure_clicked));
  m_refActionGroup->add(Gtk::Action::create("alignStorm","Align Storm Data"),sigc::mem_fun(m_viewer, &NiaViewer::alignStormData));
  m_refActionGroup->add(Gtk::Action::create("alignStormMan","Manual Storm Alignment"),sigc::mem_fun(m_viewer, &NiaViewer::manualStormAlignment));
  m_refActionGroup->add(Gtk::Action::create("findout","Find Outliers"),sigc::mem_fun(*this, &NiaCore::on_find_outliers_clicked));
  m_refActionGroup->add(Gtk::Action::create("filter","Filter"),sigc::mem_fun(*this, &NiaCore::on_filter_clicked));
  m_refActionGroup->add(Gtk::Action::create("findsigMenu","Find Signal"));
  m_refActionGroup->add(Gtk::Action::create("findsigOne","Current channel"),sigc::bind<bool>(sigc::mem_fun(*this, &NiaCore::on_find_signal_clicked),false));
  m_refActionGroup->add(Gtk::Action::create("findsigAll","All channels"),sigc::bind<bool>(sigc::mem_fun(*this, &NiaCore::on_find_signal_clicked),true));
  m_refActionGroup->add(Gtk::Action::create("appThres","Apply Threshold"),sigc::mem_fun(*this, &NiaCore::on_apply_threshold_clicked));
  m_refActionGroup->add(Gtk::Action::create("findpuncMenu","Find Puncta"));
  m_refActionGroup->add(Gtk::Action::create("findpuncOne","Current channel"),sigc::bind<bool>(sigc::mem_fun(*this, &NiaCore::on_find_puncta_clicked),false));
  m_refActionGroup->add(Gtk::Action::create("findpuncAll","All channels"),sigc::bind<bool>(sigc::mem_fun(*this, &NiaCore::on_find_puncta_clicked),true));
  m_refActionGroup->add(Gtk::Action::create("manualMenu","Manual Tools"));
  m_refActionGroup->add(Gtk::Action::create("manualAdd","Add Puncta"),Gtk::AccelKey("<control>M"),sigc::bind<NiaViewer::AnalysisMode>(sigc::mem_fun(m_viewer, &NiaViewer::setAnalysisMode),NiaViewer::MANUAL_ADD));
  m_refActionGroup->add(Gtk::Action::create("manualEdit","Edit Puncta"),Gtk::AccelKey("<control>E"),sigc::bind<NiaViewer::AnalysisMode>(sigc::mem_fun(m_viewer, &NiaViewer::setAnalysisMode),NiaViewer::MANUAL_EDIT));
  m_refActionGroup->add(Gtk::Action::create("endManual","End Manual Editing"),Gtk::AccelKey("<control>X"),sigc::bind<NiaViewer::AnalysisMode>(sigc::mem_fun(m_viewer, &NiaViewer::setAnalysisMode),NiaViewer::AUTOMATIC));
  m_refActionGroup->add(Gtk::Action::create("findsyn","Find Synapses"),sigc::mem_fun(*this, &NiaCore::on_find_synapses_clicked));
  m_refActionGroup->add(Gtk::Action::create("fullanal","Full Analysis"),sigc::mem_fun(*this, &NiaCore::on_full_analysis_clicked));
  m_refActionGroup->add(Gtk::Action::create("density","Print Synapse Density Table"),sigc::mem_fun(*this, &NiaCore::on_print_density));
  m_refActionGroup->add(Gtk::Action::create("batch","Start Batch Process"),Gtk::AccelKey("<control>B"),sigc::mem_fun(*this, &NiaCore::on_start_batch_jobs));
  m_refActionGroup->add(Gtk::Action::create("bpd","Batch Print Tables"),sigc::mem_fun(*this, &NiaCore::on_batch_print_density));
  m_refActionGroup->add(Gtk::Action::create("toolMenu","Tools"));
  m_refActionGroup->add(Gtk::Action::create("pixSel","Pixel Selector"),sigc::mem_fun(m_viewer, &NiaViewer::setPixelSelector));
  m_refActionGroup->add(Gtk::Action::create("segSel","Segment Selector"),sigc::mem_fun(m_viewer, &NiaViewer::setSegmentSelector));
  m_refActionGroup->add(Gtk::Action::create("punSel","Puncta Selector"),sigc::mem_fun(m_viewer, &NiaViewer::setPunctaSelector));
  m_refActionGroup->add(Gtk::Action::create("synSel","Synapse Selector"),sigc::mem_fun(m_viewer, &NiaViewer::setSynapseSelector));
  m_refActionGroup->add(Gtk::Action::create("regSel","Region Selector"),sigc::mem_fun(m_viewer, &NiaViewer::setRegionSelector));
  m_refActionGroup->add(Gtk::Action::create("axSel","Axis Selector"),sigc::mem_fun(m_viewer, &NiaViewer::setAxisSelector));
  m_refActionGroup->add(Gtk::Action::create("regShare","Share Regions"));
  m_refActionGroup->add(Gtk::Action::create("regShareZ","Across Z-planes"),sigc::mem_fun(m_viewer, &NiaViewer::shareRegionsZ));
  m_refActionGroup->add(Gtk::Action::create("regShareT","Across Timepoints"),sigc::mem_fun(m_viewer, &NiaViewer::shareRegionsT));
  m_refActionGroup->add(Gtk::Action::create("regConvert","Convert Regions To Segments"),sigc::mem_fun(m_viewer, &NiaViewer::convertRegions));

  m_refUIManager = Gtk::UIManager::create();
  m_refUIManager->insert_action_group(m_refActionGroup);
  add_accel_group(m_refUIManager->get_accel_group());

  Glib::ustring ui_info = 
    "<ui>"
    " <menubar name='menuBar'>"
    "  <menu action='fileMenu'>"
    "   <menuitem action='load'/>"
    "   <menuitem action='unscale'/>"
    "   <menuitem action='loadMMR'/>"
    "   <menuitem action='loadStorm'/>"
    "   <menuitem action='save'/>"
    "   <menuitem action='screenshot'/>"
    "   <menuitem action='timeseries'/>"
    "   <menuitem action='quit'/>"
    "  </menu>"
    "  <menu action='viewMenu'>"
    "   <menu action='single'>"
    "    <menuitem action='chan0'/>"
    "    <menuitem action='chan1'/>"
    "    <menuitem action='chan2'/>"
    "    <menuitem action='chan3'/>"
    "    <menuitem action='chan4'/>"
    "    <menuitem action='chan5'/>"
    "    <menuitem action='chan6'/>"
    "    <menuitem action='chan7'/>"
    "    <menuitem action='chan8'/>"
    "    <menuitem action='chan9'/>"
    "   </menu>"
    "   <menuitem action='composite'/>"
    "   <menu action='setRed'>"
    "    <menuitem action='redChan0'/>"
    "    <menuitem action='redChan1'/>"
    "    <menuitem action='redChan2'/>"
    "    <menuitem action='redChan3'/>"
    "    <menuitem action='redChan4'/>"
    "    <menuitem action='redChan5'/>"
    "    <menuitem action='redChan6'/>"
    "    <menuitem action='redChan7'/>"
    "    <menuitem action='redChan8'/>"
    "    <menuitem action='redChan9'/>"
    "   </menu>"
    "   <menu action='setGreen'>"
    "    <menuitem action='greenChan0'/>"
    "    <menuitem action='greenChan1'/>"
    "    <menuitem action='greenChan2'/>"
    "    <menuitem action='greenChan3'/>"
    "    <menuitem action='greenChan4'/>"
    "    <menuitem action='greenChan5'/>"
    "    <menuitem action='greenChan6'/>"
    "    <menuitem action='greenChan7'/>"
    "    <menuitem action='greenChan8'/>"
    "    <menuitem action='greenChan9'/>"
    "   </menu>"
    "   <menu action='setBlue'>"
    "    <menuitem action='blueChan0'/>"
    "    <menuitem action='blueChan1'/>"
    "    <menuitem action='blueChan2'/>"
    "    <menuitem action='blueChan3'/>"
    "    <menuitem action='blueChan4'/>"
    "    <menuitem action='blueChan5'/>"
    "    <menuitem action='blueChan6'/>"
    "    <menuitem action='blueChan7'/>"
    "    <menuitem action='blueChan8'/>"
    "    <menuitem action='blueChan9'/>"
    "   </menu>"
    "   <menuitem action='zproj'/>"
    "   <menuitem action='scale'/>"
    "   <menu action='maskMenu'>"
    "    <menuitem action='sigMask'/>"
    "    <menuitem action='puncMask'/>"
    "    <menuitem action='synMask'/>"
    "    <menuitem action='regMask'/>"
    "    <menuitem action='segMask'/>"
    "    <menuitem action='stormMask'/>"
    "    <menuitem action='conMap'/>"
    "    <menuitem action='clearMask'/>"
    "   </menu>"
    "   <menuitem action='viewStorm'/>"
    "   <menuitem action='stormZWin'/>"
    "   <menuitem action='derivative'/>"
    "   <menuitem action='derivative2'/>"
    "   <menuitem action='zoomin'/>"
    "   <menuitem action='zoomout'/>"
    "   <menuitem action='unzoom'/>"
    "   <menuitem action='stats'/>"
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
    "   <menuitem action='config'/>"
    "   <menuitem action='alignStorm'/>"
    "   <menuitem action='alignStormMan'/>"
    "   <menuitem action='findout'/>"
    "   <menuitem action='filter'/>"
    "   <menu action='findsigMenu'>"
    "    <menuitem action='findsigOne'/>"
    "    <menuitem action='findsigAll'/>"
    "    <menuitem action='appThres'/>"
    "   </menu>"
    "   <menu action='findpuncMenu'>"
    "    <menuitem action='findpuncOne'/>"
    "    <menuitem action='findpuncAll'/>"
    "    <menu action='manualMenu'>"
    "     <menuitem action='manualAdd'/>"
    "     <menuitem action='manualEdit'/>"
    "     <menuitem action='endManual'/>"
    "    </menu>"
    "   </menu>"
    "   <menuitem action='findsyn'/>"
    "   <menuitem action='fullanal'/>"
    "   <menuitem action='density'/>"
    "   <menuitem action='batch'/>"
    "   <menuitem action='bpd'/>"
    "  </menu>"
    "  <menu action='toolMenu'>"
    "   <menuitem action='pixSel'/>"
    "   <menuitem action='segSel'/>"
    "   <menuitem action='punSel'/>"
    "   <menuitem action='synSel'/>"
    "   <menuitem action='regSel'/>"
    "   <menuitem action='axSel'/>"
    "   <menu action='regShare'>"
    "    <menuitem action='regShareZ'/>"
    "    <menuitem action='regShareT'/>"
    "   </menu>"
    "   <menuitem action='regConvert'/>"
    "  </menu>"
    " </menubar>"
    "</ui>";
  try{ m_refUIManager->add_ui_from_string(ui_info); }
  catch(const Glib::Error& ex){}
  Gtk::Widget* pMenubar = m_refUIManager->get_widget("/menuBar");
  if(pMenubar) m_vbox.pack_start(*pMenubar, Gtk::PACK_SHRINK);

  m_vbox.pack_start(m_viewer, Gtk::PACK_EXPAND_WIDGET);

  show_all_children();
  m_viewer.hideScaleBox();
  m_viewer.setToolkit(&m_iat);
}

void NiaCore::on_menu_load()
{
  std::vector<ImRecord*> records;
  m_fileManager.clearInputFiles();
  FileSelector fcd(&m_fileManager,&m_iat,&records,"",Gtk::FILE_CHOOSER_ACTION_OPEN);
  fcd.set_transient_for(*this);

  Gtk::FileFilter filt_all;
  filt_all.set_name("All files");
  filt_all.add_pattern("*.tif");
  filt_all.add_pattern("*.tiff");
  filt_all.add_pattern("*.TIF");
  filt_all.add_pattern("*.TIFF");
  filt_all.add_pattern("*.nia");
  fcd.add_filter(filt_all);

  Gtk::FileFilter filt;
  filt.set_name("TIFF files");
  filt.add_pattern("*.tif");
  filt.add_pattern("*.tiff");
  filt.add_pattern("*.TIF");
  filt.add_pattern("*.TIFF");
  fcd.add_filter(filt);

  Gtk::FileFilter filt2;
  filt2.set_name("NIA files");
  filt2.add_pattern("*.nia");
  fcd.add_filter(filt2);

  int result = fcd.run();
  if(result == Gtk::RESPONSE_OK){
    m_viewer.setData(m_fileManager.load());
    if(records.size() > 0) m_viewer.setRecords(records);
  }
}

void NiaCore::on_save()
{
  std::vector<ImRecord*> recs = m_viewer.records();
  for(std::vector<ImRecord*>::iterator it = recs.begin(); it != recs.end(); it++){
    if(!(*it)){
      Gtk::MessageDialog messenger(*this,"ERROR: At least one data record is uninitialized.");
      messenger.run();
      return;
    }
  }
  Gtk::FileChooserDialog fcd("",Gtk::FILE_CHOOSER_ACTION_SAVE);
  fcd.set_transient_for(*this);
  fcd.add_button("Cancel",Gtk::RESPONSE_CANCEL);
  fcd.add_button("Save",Gtk::RESPONSE_OK);
  Gtk::FileFilter filt2;
  filt2.set_name("NIA files");
  filt2.add_pattern("*.nia");
  fcd.add_filter(filt2);
  int result = fcd.run();
  if(result == Gtk::RESPONSE_OK){
    std::string filename = fcd.get_filename();
    if(filename.find(".nia") == std::string::npos){
      filename.append("_");
      filename.append(m_fileManager.getName(0));
      filename.append(".nia");
    }
    FileConverter::write(&m_fileManager,&m_iat,&recs,filename,0,nia::niaVersion);
    /*
    std::ofstream fout(filename.c_str(),std::ofstream::binary);
    m_fileManager.saveInputFiles(fout,0);
    m_iat.write(fout);
    char buf[1];
    if(m_batchService.zproject()) buf[0] = 1;
    else buf[0] = 0;
    fout.write(buf,1);
    std::vector<ImRecord*> recs = m_viewer.records();
    for(std::vector<ImRecord*>::iterator rit = recs.begin(); rit != recs.end(); rit++){
      (*rit)->write(fout);
    }
    fout.close();
    */
  }
}

void NiaCore::on_save_screenshot()
{
  if(!m_viewer.data()) return;
  Gtk::FileChooserDialog fcd("",Gtk::FILE_CHOOSER_ACTION_SAVE);
  fcd.set_transient_for(*this);
  fcd.add_button("Cancel",Gtk::RESPONSE_CANCEL);
  fcd.add_button("Save",Gtk::RESPONSE_OK);
  Gtk::FileFilter filt2;
  filt2.set_name("PNG files");
  filt2.add_pattern("*.png");
  fcd.add_filter(filt2);
  int result = fcd.run();
  if(result == Gtk::RESPONSE_OK){
    std::string filename = fcd.get_filename();
    if(filename.find(".png") == std::string::npos){
      filename.append(".png");
    }
    m_viewer.saveScreenshot(filename);
  }
}

void NiaCore::on_save_timeseries()
{
  if(!m_viewer.data()) return;
  Gtk::FileChooserDialog fcd("",Gtk::FILE_CHOOSER_ACTION_SAVE);
  fcd.set_transient_for(*this);
  fcd.add_button("Cancel",Gtk::RESPONSE_CANCEL);
  fcd.add_button("Save",Gtk::RESPONSE_OK);
  Gtk::FileFilter filt2;
  filt2.set_name("PNG files");
  filt2.add_pattern("*.png");
  fcd.add_filter(filt2);
  int result = fcd.run();
  if(result == Gtk::RESPONSE_OK){
    std::string filename = fcd.get_filename();
    m_viewer.saveTimeSeries(filename);
  }
}

void NiaCore::on_start_batch_jobs()
{
  std::vector<ImRecord*> recs;
  m_batchService.fileManager()->clearInputFiles();
  ImageAnalysisToolkit* biat = m_batchService.iat();
  FileSelector fs(m_batchService.fileManager(),biat,&recs,"Select input files",Gtk::FILE_CHOOSER_ACTION_OPEN);
  fs.set_transient_for(*this);

  Gtk::FileFilter filt;
  filt.set_name("TIFF files");
  filt.add_pattern("*.tif");
  filt.add_pattern("*.tiff");
  filt.add_pattern("*.TIF");
  filt.add_pattern("*.TIFF");
  fs.add_filter(filt);

  Gtk::FileFilter filt2;
  filt2.set_name("NIA files");
  filt2.add_pattern("*.nia");
  fs.add_filter(filt2);

  int result = fs.run();
  if(result != Gtk::RESPONSE_OK) return;
  fs.hide();
  
  ConfigurationDialog cd(biat,m_batchService.fileManager()->getNW(0),m_batchService.maxThreads(),m_batchService.zproject());
  cd.set_transient_for(*this);
  result = cd.run();
  if(result != Gtk::RESPONSE_OK) return;
  biat->setMaster(cd.getMaster());
  biat->setMode(cd.getMode());
  biat->clearContaminatedChannels();
  if(cd.doBackgroundSubtraction()){
    std::vector<int> contChans = cd.getContaminatedChannels();
    std::vector<int> bkgChans = cd.getBackgroundChannels();
    for(int i = 0; i < contChans.size(); i++){
      biat->setContaminatedChannel(contChans[i],bkgChans[i]);
    }
  }
  biat->setSubtractionAmount(cd.getSubtractionAmount());
  biat->setBitDepth(cd.getBitDepth());
  biat->setMaxPunctaFindingIterations(cd.getPunctaFindingIterations());
  biat->setMaxSignalFindingIterations(cd.getSignalFindingIterations());
  biat->setKernelWidth(cd.getKernelWidth());
  if(cd.doSeparateConfigs()){
    if(biat->nConfigs() < cd.nchannels()){
      biat->makeSingleConfig();
      biat->makeSeparateConfigs(cd.nchannels());
    }
    for(int i = 0; i < cd.nchannels(); i++){
      biat->setLocalWindow(i,cd.getLocalWindow(i));
      biat->setWindowSteps(i,cd.getWindowSteps(i));
      biat->setMinPunctaRadius(i,cd.getRadius(i));
      //biat->setMaxPunctaRadius(i,cd.getMaxRadius(i));
      biat->setReclusterThreshold(i,cd.getRecluster(i));
      biat->setNoiseRemovalThreshold(i,cd.getNoiseRemovalThreshold(i));
      biat->setPeakThreshold(i,cd.getPeak(i));
      biat->setFloorThreshold(i,cd.getFloor(i));
      biat->setBackgroundThreshold(i,cd.getBackgroundThreshold(i));
    }
  }
  else{
    biat->setLocalWindow(cd.getLocalWindow());
    biat->setWindowSteps(cd.getWindowSteps());
    biat->setMinPunctaRadius(cd.getRadius());
    //biat->setMaxPunctaRadius(cd.getMaxRadius());
    biat->setReclusterThreshold(cd.getRecluster());
    biat->setNoiseRemovalThreshold(cd.getNoiseRemovalThreshold());
    biat->setPeakThreshold(cd.getPeak());
    biat->setFloorThreshold(cd.getFloor());
    biat->setBackgroundThreshold(cd.getBackgroundThreshold());
  }
  biat->setChannelNames(cd.getChannelNames());
  biat->setPostChan(cd.getPostChan());
  m_batchService.setMaxThreads(cd.getThreads());
  m_batchService.setZProject(cd.getZProject());
  m_batchService.setDivisor(cd.getDivisor());
  m_batchService.setWriteTables(cd.getWriteTables());
  cd.hide();
  
  Gtk::FileChooserDialog fcd("Select save destination",Gtk::FILE_CHOOSER_ACTION_SAVE);
  fcd.set_transient_for(*this);
  fcd.add_button("Cancel",Gtk::RESPONSE_CANCEL);
  fcd.add_button("Run",Gtk::RESPONSE_OK);
  fcd.add_filter(filt2);
  result = fcd.run();
  if(result == Gtk::RESPONSE_OK){
    m_batchService.setName(fcd.get_filename());
    m_batchWindow.launch(m_batchService.fileManager()->ntasks(m_batchService.zproject()));
    m_batchService.setProgressWindow(&m_batchWindow);
    if(recs.size() > 0){
      m_threads.create_thread(boost::bind(&BatchService::run2,&m_batchService,recs));
    }
    else m_threads.create_thread(boost::bind(&BatchService::run,&m_batchService));
  }
}

void NiaCore::on_configure_clicked()
{
  int nchan = m_viewer.getNW();
  if(nchan == 0) nchan = 1;
  ConfigurationDialog cd(&m_iat,nchan);
  cd.set_transient_for(*this);
  int result = cd.run();
  if(result != Gtk::RESPONSE_OK) return;
  m_iat.setMaster(cd.getMaster());
  m_iat.setMode(cd.getMode());
  m_iat.clearContaminatedChannels();
  if(cd.doBackgroundSubtraction()){
    std::vector<int> contChans = cd.getContaminatedChannels();
    std::vector<int> bkgChans = cd.getBackgroundChannels();
    for(int i = 0; i < contChans.size(); i++){
      m_iat.setContaminatedChannel(contChans[i],bkgChans[i]);
    }
  }
  m_iat.setSubtractionAmount(cd.getSubtractionAmount());
  m_iat.setBitDepth(cd.getBitDepth());
  m_iat.setMaxPunctaFindingIterations(cd.getPunctaFindingIterations());
  m_iat.setMaxSignalFindingIterations(cd.getSignalFindingIterations());
  m_iat.setKernelWidth(cd.getKernelWidth());
  if(cd.doSeparateConfigs()){
    if(m_iat.nConfigs() < nchan){
      m_iat.makeSingleConfig();
      m_iat.makeSeparateConfigs(nchan);
    }
    for(int i = 0; i < nchan; i++){
      m_iat.setLocalWindow(i,cd.getLocalWindow(i));
      m_iat.setWindowSteps(i,cd.getWindowSteps(i));
      m_iat.setMinPunctaRadius(i,cd.getRadius(i));
      //m_iat.setMaxPunctaRadius(i,cd.getMaxRadius(i));
      m_iat.setReclusterThreshold(i,cd.getRecluster(i));
      m_iat.setNoiseRemovalThreshold(i,cd.getNoiseRemovalThreshold(i));
      m_iat.setPeakThreshold(i,cd.getPeak(i));
      m_iat.setFloorThreshold(i,cd.getFloor(i));
      m_iat.setBackgroundThreshold(i,cd.getBackgroundThreshold(i));
    }
  }
  else{
    m_iat.setLocalWindow(cd.getLocalWindow());
    m_iat.setWindowSteps(cd.getWindowSteps());
    m_iat.setMinPunctaRadius(cd.getRadius());
    //m_iat.setMaxPunctaRadius(cd.getMaxRadius());
    m_iat.setReclusterThreshold(cd.getRecluster());
    m_iat.setNoiseRemovalThreshold(cd.getNoiseRemovalThreshold());
    m_iat.setPeakThreshold(cd.getPeak());
    m_iat.setFloorThreshold(cd.getFloor());
    m_iat.setBackgroundThreshold(cd.getBackgroundThreshold());
  }
  m_iat.setChannelNames(cd.getChannelNames());
  m_iat.setPostChan(cd.getPostChan());
}

void NiaCore::on_find_outliers_clicked()
{
  ImFrame* frame = m_viewer.currentFrame();
  if(!frame) return;
  Mask* m = m_iat.findOutliers(frame);
  m_viewer.toggleMask(m->inverse());
  delete m;
}

void NiaCore::on_filter_clicked()
{
  ImFrame* frame = m_viewer.currentFrame();
  if(!frame) return;
  m_iat.filter(frame);
  m_viewer.autoscale();
}

void NiaCore::on_load_storm_data_clicked()
{
  ImFrame* frame = m_viewer.currentFrame();
  if(!frame) return;
  ImRecord* rec = m_viewer.currentRecord();
  if(!rec){
    rec = new ImRecord(m_viewer.getNW(),frame->width(),frame->height());
    rec->setResolutionXY(m_viewer.data()->resolutionXY());
    std::vector<std::string> chanNames = m_iat.getChannelNames();
    for(int i = 0; i < chanNames.size(); i++) rec->setChannelName(i,chanNames[i]);
    m_viewer.setCurrentRecord(rec);
  }

  Gtk::FileFilter filt;
  filt.set_name("CSV files");
  filt.add_pattern("*.csv");

  Gtk::FileChooserDialog fcd("Select input file",Gtk::FILE_CHOOSER_ACTION_OPEN);
  fcd.set_transient_for(*this);
  fcd.add_button("Cancel",Gtk::RESPONSE_CANCEL);
  fcd.add_button("OK",Gtk::RESPONSE_OK);
  fcd.add_filter(filt);
  int result = fcd.run();
  if(result == Gtk::RESPONSE_OK){
    StormData* sd = new StormData(fcd.get_filename());
    double bpc = rec->getBlinksPerCount(m_viewer.viewW(),frame,sd);
    rec->setStormClusters(m_viewer.viewW(),StormCluster::cluster(sd,bpc));
    delete sd;
  }
}

void NiaCore::on_find_signal_clicked(bool doAll)
{
  ImFrame* frame = m_viewer.currentFrame();
  if(!frame) return;
  ImRecord* rec = m_viewer.currentRecord();
  if(!rec){
    rec = new ImRecord(m_viewer.getNW(),frame->width(),frame->height());
    rec->setResolutionXY(m_viewer.data()->resolutionXY());
    std::vector<std::string> chanNames = m_iat.getChannelNames();
    for(int i = 0; i < chanNames.size(); i++) rec->setChannelName(i,chanNames[i]);
    m_viewer.setCurrentRecord(rec);
  }
  if(doAll){
    ImStack* stack = m_viewer.currentStack();
    m_iat.findSignal(stack,rec,m_viewer.viewZ());
    std::cout << nia::nout.buffer() << std::endl;
    nia::nout.clear();
  }
  else{
    //rec->setThreshold(m_viewer.viewW(),m_iat.findThreshold(frame));
    int chan = m_viewer.viewW();
    std::vector<int> contChans = m_iat.contaminatedChannels();
    for(int i = 0; i < contChans.size(); i++){
      if(contChans[i] == chan){
	std::vector<int> bkgChans = m_iat.backgroundChannels();
	Mask* bkgMask = rec->getSignalMask(bkgChans[i]);
	if(!bkgMask) return;
	ImFrame* subFrame = frame->backgroundSubtractedFrame(bkgMask,m_iat.subtractionAmount());
	m_iat.findSignal(subFrame,rec,m_viewer.viewW());
	delete subFrame;
	return;
      }
    }
    m_iat.findSignal(frame,rec,m_viewer.viewW());
  }
}

void NiaCore::on_apply_threshold_clicked()
{
  ImFrame* frame = m_viewer.currentFrame();
  if(!frame) return;
  ImRecord* rec = m_viewer.currentRecord();
  if(!rec){
    rec = new ImRecord(m_viewer.getNW(),frame->width(),frame->height());
    rec->setResolutionXY(m_viewer.data()->resolutionXY());
    std::vector<std::string> chanNames = m_iat.getChannelNames();
    for(int i = 0; i < chanNames.size(); i++) rec->setChannelName(i,chanNames[i]);
    m_viewer.setCurrentRecord(rec);
  }
  rec->setThreshold(m_viewer.viewW(),m_viewer.grayMin());
  delete m_iat.applyThreshold(frame,rec,m_viewer.viewW());
}

void NiaCore::on_find_puncta_clicked(bool doAll)
{
  ImFrame* frame = m_viewer.currentFrame();
  if(!frame) return;
  ImRecord* rec = m_viewer.currentRecord();
  if(!rec){
    rec = new ImRecord(m_viewer.getNW(),frame->width(),frame->height());
    rec->setResolutionXY(m_viewer.data()->resolutionXY());
    std::vector<std::string> chanNames = m_iat.getChannelNames();
    for(int i = 0; i < chanNames.size(); i++) rec->setChannelName(i,chanNames[i]);
    m_viewer.setCurrentRecord(rec);
  }
  if(doAll){
    ImStack* stack = m_viewer.currentStack();
    for(int w = 0; w < stack->nwaves(); w++){
      rec->clearPuncta(w);
      m_iat.findPuncta(stack->frame(w,m_viewer.viewZ()),rec,w);
    }
  }
  else{
    rec->clearPuncta(m_viewer.viewW());
    m_iat.findPuncta(frame,rec,m_viewer.viewW());
  }
  std::cout << nia::nout.buffer() << std::endl;
  nia::nout.clear();
}

void NiaCore::on_find_synapses_clicked()
{
  ImRecord* rec = m_viewer.currentRecord();
  if(!rec){
    return;
  }
  rec->clearSynapseCollections();
  std::vector<SynapseCollection*> syncol = m_iat.synapseDefinitions();
  for(std::vector<SynapseCollection*>::iterator it = syncol.begin(); it != syncol.end(); it++) rec->addSynapseCollection((*it)->emptyCopy());
  m_iat.findSynapses(rec);
  m_iat.findStormSynapses(rec);
  std::cout << nia::nout.buffer() << std::endl;
  nia::nout.clear();
}

void NiaCore::on_full_analysis_clicked()
{
  ImStack* stack = m_viewer.currentStack();
  if(!stack) return;
  ImRecord* rec = m_viewer.currentRecord();
  if(!rec){
    rec = new ImRecord(m_viewer.getNW(),stack->frame(0,0)->width(),stack->frame(0,0)->height());
    rec->setResolutionXY(m_viewer.data()->resolutionXY());
    std::vector<std::string> chanNames = m_iat.getChannelNames();
    for(int i = 0; i < chanNames.size(); i++) rec->setChannelName(i,chanNames[i]);
    m_viewer.setCurrentRecord(rec);
  }
  m_iat.standardAnalysis(stack,rec,m_viewer.viewZ());
  std::cout << nia::nout.buffer() << std::endl;
  nia::nout.clear();
}

void NiaCore::on_load_regions()
{
  ImFrame* frame = m_viewer.currentFrame();
  if(!frame) return;

  Gtk::FileChooserDialog fcd("",Gtk::FILE_CHOOSER_ACTION_OPEN);
  fcd.set_transient_for(*this);
  fcd.add_button("Cancel",Gtk::RESPONSE_CANCEL);
  fcd.add_button("Load",Gtk::RESPONSE_OK);
  Gtk::FileFilter filt2;
  filt2.set_name("RGN files");
  filt2.add_pattern("*.rgn");
  fcd.add_filter(filt2);
  int result = fcd.run();
  
  if(result == Gtk::RESPONSE_OK){
    ImRecord* rec = m_viewer.currentRecord();
    if(!rec){
      rec = new ImRecord(m_viewer.getNW(),frame->width(),frame->height());
      rec->setResolutionXY(m_viewer.data()->resolutionXY());
      m_viewer.setCurrentRecord(rec);
    }
    rec->loadMetaMorphRegions(fcd.get_filename());
  }
}

void NiaCore::on_print_density()
{
  ImRecord* rec = m_viewer.currentRecord();
  if(!rec) return;
  Gtk::FileChooserDialog fcd("",Gtk::FILE_CHOOSER_ACTION_SAVE);
  fcd.set_transient_for(*this);
  fcd.add_button("Cancel",Gtk::RESPONSE_CANCEL);
  fcd.add_button("Save",Gtk::RESPONSE_OK);
  Gtk::FileFilter filt2;
  filt2.set_name("CSV files");
  filt2.add_pattern("*.csv");
  fcd.add_filter(filt2);
  int result = fcd.run();
  if(result == Gtk::RESPONSE_OK){
    std::string filename = fcd.get_filename();
    if(filename.find(".csv") == std::string::npos) filename.append(".csv");
    rec->printSynapseDensityTable(m_iat.postChan(),filename);
  }
}

void NiaCore::on_batch_print_density()
{
  Gtk::FileChooserDialog fcd("",Gtk::FILE_CHOOSER_ACTION_SAVE);
  fcd.set_transient_for(*this);
  fcd.add_button("Cancel",Gtk::RESPONSE_CANCEL);
  fcd.add_button("Save",Gtk::RESPONSE_OK);
  Gtk::FileFilter filt2;
  filt2.set_name("CSV files");
  filt2.add_pattern("*.csv");
  fcd.add_filter(filt2);
  int result = fcd.run();
  if(result == Gtk::RESPONSE_OK){
    std::string basename = fcd.get_filename();
    int np = m_viewer.getNP();
    int nt = m_viewer.getNT();
    int nz = m_viewer.getNZ();
    for(int ip = 0; ip < np; ip++){
      for(int it = 0; it < nt; it++){
	for(int iz = 0; iz < nz; iz++){
	  ImRecord* rec = m_viewer.getRecord(ip,it,iz);
	  if(!rec) continue;
	  std::ostringstream filename;
	  filename << basename << "_" << m_fileManager.sname() << "_xy" << ip << "_t" << it << "_z" << iz << ".csv";
	  rec->printSynapseDensityTable(m_iat.postChan(),filename.str());
	}
      }
    }
  }
}
