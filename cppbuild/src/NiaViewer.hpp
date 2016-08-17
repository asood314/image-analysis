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
  Gtk::EventBox m_eventBox;
  Gtk::Label m_grayMinLabel,m_grayMaxLabel,m_redMinLabel,m_redMaxLabel,m_greenMinLabel,m_greenMaxLabel,m_blueMinLabel,m_blueMaxLabel;
  Gtk::Entry m_grayMinEntry,m_grayMaxEntry,m_redMinEntry,m_redMaxEntry,m_greenMinEntry,m_greenMaxEntry,m_blueMinEntry,m_blueMaxEntry;
  Gtk::VBox /*m_mainBox,*/m_vbox1,m_vbox2,m_vbox3,m_vbox4,m_vbox5,m_vbox6,m_vbox7,m_vbox8;
  Gtk::Alignment m_alignment;
  Gtk::ScrolledWindow m_swin;
  Gtk::VSeparator m_vsep1,m_vsep2;
  Gtk::HBox m_scaleBox;
  Gtk::Button m_scaleHideButton;
  Gtk::Image m_displayImage;
  Glib::RefPtr<Gdk::Pixbuf> m_pixbuf;
  std::vector<Mask*> m_masks;
  static const unsigned m_ncolors = 3;
  Color m_colors[m_ncolors];
  bool m_pixelSelector,m_punctaSelector,m_synapseSelector,m_regionSelector;
  std::vector<LocalizedObject::Point> m_prevClicks;
  Glib::RefPtr<Gdk::Pixbuf> createPixbuf(ImFrame* frame);
  Glib::RefPtr<Gdk::Pixbuf> createPixbuf(ImStack* stack);
  void autoscaleGray();
  void autoscaleRGB();
  void updateImage();
  bool on_button_press(GdkEventButton* evt);
  void scale();
  
public:
  NiaViewer();
  virtual ~NiaViewer();
  void autoscale();
  void setMode(ImageMode mode);
  void setWavelength(int w);
  void prevZ();
  void nextZ();
  void prevPosition();
  void nextPosition();
  void prevTimepoint();
  void nextTimepoint();
  void setData(ImSeries* data);
  void displayMask(Mask* m);
  void showContourMap();
  void zproject();
  void zoomIn();
  void zoomOut();
  void unzoom();
  void toggleMask(Mask* m);
  void toggleSignalMask();
  void togglePunctaMask();
  void toggleSynapseMask();
  void toggleRegionMask();
  void clearMasks();
  void setRecords(std::vector<ImRecord*> recs);
  void setCurrentRecord(ImRecord* rec);
  void shareRegions();
  std::vector<ImRecord*> records(){ return m_records; }
  int getNW(){
    if(m_data) return m_data->fourLocation(m_view_p,m_view_t)->nwaves();
    return 0;
  }
  int viewW(){ return m_view_w; }
  int viewZ(){ return m_view_z; }
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
    m_punctaSelector = false;
    m_synapseSelector = false;
    m_regionSelector = false;
  }
  void setPunctaSelector(){
    m_pixelSelector = false;
    m_punctaSelector = true;
    m_synapseSelector = false;
    m_regionSelector = false;
  }
  void setSynapseSelector(){
    m_pixelSelector = false;
    m_punctaSelector = false;
    m_synapseSelector = true;
    m_regionSelector = false;
  }
  void setRegionSelector(){
    m_pixelSelector = false;
    m_punctaSelector = false;
    m_synapseSelector = false;
    m_regionSelector = true;
  }
  ImSeries* data(){ return m_data; }
  void saveScreenshot(std::string filename){ m_pixbuf->save(filename,"png"); }
  void unscale(){ if(m_data) m_data->divide(16); }

};

#endif
