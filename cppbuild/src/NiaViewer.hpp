#ifndef NIA_VIEWER_HPP
#define NIA_VIEWER_HPP

#include <gtkmm-2.4/gtkmm.h>
#include <boost/lexical_cast.hpp>
#include "ImSeries.hpp"
#include "FileManager.hpp"
#include "ImRecord.hpp"

class NiaViewer : public Gtk::VBox
{

public:
  enum ImageMode{ GRAY=0,RGB };
  enum ImageType{ CONFOCAL=0,STORM };

  typedef struct mask_color{
    uint8_t r,g,b;
  } Color;

protected:
  ImSeries* m_data;
  std::vector<ImRecord*> m_records;
  int m_width,m_height;
  double m_zoom;
  int m_view_w,m_view_z,m_view_p,m_view_t;
  int m_red,m_green,m_blue;
  int m_grayMin,m_grayMax,m_redMin,m_redMax,m_greenMin,m_greenMax,m_blueMin,m_blueMax;
  ImageMode m_mode;
  ImageType m_imageType;
  Gtk::EventBox m_eventBox;
  Gtk::Label m_grayMinLabel,m_grayMaxLabel,m_redMinLabel,m_redMaxLabel,m_greenMinLabel,m_greenMaxLabel,m_blueMinLabel,m_blueMaxLabel;
  Gtk::Entry m_grayMinEntry,m_grayMaxEntry,m_redMinEntry,m_redMaxEntry,m_greenMinEntry,m_greenMaxEntry,m_blueMinEntry,m_blueMaxEntry;
  Gtk::VBox /*m_mainBox,*/m_vbox1,m_vbox2,m_vbox3,m_vbox4,m_vbox5,m_vbox6,m_vbox7,m_vbox8;
  Gtk::Alignment m_alignment;
  Gtk::ScrolledWindow m_swin;
  Gtk::VSeparator m_vsep1,m_vsep2;
  Gtk::HBox m_scaleBox;
  Gtk::Button m_autoscaleButton,m_scaleHideButton;
  Gtk::Image m_displayImage;
  Glib::RefPtr<Gdk::Pixbuf> m_pixbuf;
  std::vector<Mask*> m_masks;
  static const unsigned m_ncolors = 6;
  Color m_colors[m_ncolors];
  bool m_pixelSelector,m_segmentSelector,m_punctaSelector,m_synapseSelector,m_regionSelector,m_axisSelector;
  std::vector<LocalizedObject::Point> m_prevClicks;
  int m_prevButton;
  Glib::RefPtr<Gdk::Pixbuf> createPixbuf(ImFrame* frame);
  Glib::RefPtr<Gdk::Pixbuf> createPixbuf(ImStack* stack);
  Glib::RefPtr<Gdk::Pixbuf> createPixbufStorm(int chan);
  Glib::RefPtr<Gdk::Pixbuf> createPixbufStorm();
  void autoscaleGray();
  void autoscaleRGB();
  void updateImage();
  void displayStormImage();
  bool on_button_press(GdkEventButton* evt);
  void scale();
  
public:
  NiaViewer();
  virtual ~NiaViewer();
  void autoscale();
  void setMode(ImageMode mode);
  void toggleImageType();
  void setWavelength(int w);
  void prevZ();
  void nextZ();
  void prevPosition();
  void nextPosition();
  void prevTimepoint();
  void nextTimepoint();
  void setData(ImSeries* data);
  void showDerivative();
  void showDerivative2();
  void showStats();
  void displayMask(Mask* m);
  void showContourMap();
  void zproject();
  void zoomIn();
  void zoomOut();
  void unzoom();
  void toggleMask(Mask* m);
  void removeMask(Mask* m);
  void toggleSignalMask();
  void togglePunctaMask();
  void toggleSynapseMask();
  void toggleRegionMask();
  void toggleStormMask();
  void clearMasks();
  void setRecords(std::vector<ImRecord*> recs);
  void setCurrentRecord(ImRecord* rec);
  ImRecord* getRecord(int pos, int t, int z);
  void alignStormData();
  void shareRegionsZ();
  void shareRegionsT();
  void convertRegions();
  std::vector<ImRecord*> records(){ return m_records; }
  int getNW(){
    if(m_data) return m_data->fourLocation(m_view_p,m_view_t)->nwaves();
    return 0;
  }
  int getNZ(){
    if(m_data) return m_data->fourLocation(m_view_p,m_view_t)->nz();
    return 0;
  }
  int getNT(){
    if(m_data) return m_data->nt();
    return 0;
  }
  int getNP(){
    if(m_data) return m_data->npos();
    return 0;
  }
  int viewW(){ return m_view_w; }
  int viewZ(){ return m_view_z; }
  int viewT(){ return m_view_t; }
  int viewP(){ return m_view_p; }
  ImFrame* currentFrame(){
    if(m_data) return m_data->fourLocation(m_view_p,m_view_t)->frame(m_view_w,m_view_z);
    return NULL;
  }
  ImStack* currentStack(){
    if(m_data) return m_data->fourLocation(m_view_p,m_view_t);
    return NULL;
  }
  ImRecord* currentRecord();
  void setRed(int chan){ m_red = chan; }
  void setGreen(int chan){ m_green = chan; }
  void setBlue(int chan){ m_blue = chan; }
  void hideScaleBox(){ m_scaleBox.hide(); }
  void showScaleBox(){ m_scaleBox.show(); }
  void setPixelSelector(){
    m_pixelSelector = true;
    m_segmentSelector = false;
    m_punctaSelector = false;
    m_synapseSelector = false;
    m_regionSelector = false;
    m_axisSelector = false;
  }
  void setSegmentSelector(){
    m_pixelSelector = false;
    m_segmentSelector = true;
    m_punctaSelector = false;
    m_synapseSelector = false;
    m_regionSelector = false;
    m_axisSelector = false;
  }
  void setPunctaSelector(){
    m_pixelSelector = false;
    m_segmentSelector = false;
    m_punctaSelector = true;
    m_synapseSelector = false;
    m_regionSelector = false;
    m_axisSelector = false;
  }
  void setSynapseSelector(){
    m_pixelSelector = false;
    m_segmentSelector = false;
    m_punctaSelector = false;
    m_synapseSelector = true;
    m_regionSelector = false;
    m_axisSelector = false;
  }
  void setRegionSelector(){
    m_pixelSelector = false;
    m_segmentSelector = false;
    m_punctaSelector = false;
    m_synapseSelector = false;
    m_regionSelector = true;
    m_axisSelector = false;
  }
  void setAxisSelector(){
    m_pixelSelector = false;
    m_segmentSelector = false;
    m_punctaSelector = false;
    m_synapseSelector = false;
    m_regionSelector = false;
    m_axisSelector = true;
  }
  ImSeries* data(){ return m_data; }
  void saveScreenshot(std::string filename){ m_pixbuf->save(filename,"png"); }
  void saveTimeSeries(std::string basename);
  void unscale(){ if(m_data) m_data->divide(16); }
  int grayMin(){ return m_grayMin; }

};

#endif
