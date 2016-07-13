#ifndef NIA_VIEWER_HPP
#define NIA_VIEWER_HPP

#include <gtkmm-2.4/gtkmm.h>
#include "ImSeries.hpp"
#include "FileManager.hpp"
#include "ImRecord.hpp"

class NiaViewer : public Gtk::ScrolledWindow
{

public:
  enum ImageMode{ GRAY=0,RGB };

  typedef struct mask_color{
    uint8_t r,g,b;
  } Color;

protected:
  ImSeries* m_data;
  std::vector<ImRecord*> m_records;
  uint16_t m_width,m_height;
  double m_zoom;
  uint8_t m_view_w,m_view_z,m_view_p,m_view_t;
  uint8_t m_red,m_green,m_blue;
  uint16_t m_grayMin,m_grayMax,m_redMin,m_redMax,m_greenMin,m_greenMax,m_blueMin,m_blueMax;
  ImageMode m_mode;
  Gtk::EventBox m_eventBox;
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
  
public:
  NiaViewer();
  virtual ~NiaViewer();
  void autoscale();
  void setMode(ImageMode mode);
  void setWavelength(uint8_t w);
  void prevZ();
  void nextZ();
  void prevPosition();
  void nextPosition();
  void prevTimepoint();
  void nextTimepoint();
  void setData(ImSeries* data);
  void zoomIn();
  void zoomOut();
  void unzoom();
  void toggleMask(Mask* m);
  void toggleSignalMask();
  void togglePunctaMask();
  void toggleSynapseMask();
  void toggleRegionMask();
  void clearMasks(){ m_masks.clear(); }
  void setRecords(std::vector<ImRecord*> recs);
  void setCurrentRecord(ImRecord* rec);
  std::vector<ImRecord*> records(){ return m_records; }
  uint8_t getNW(){
    if(m_data) return m_data->fourLocation(m_view_p,m_view_t)->nwaves();
    return 0;
  }
  uint8_t viewW(){ return m_view_w; }
  ImFrame* currentFrame(){
    if(m_data) return m_data->fourLocation(m_view_p,m_view_t)->frame(m_view_w,m_view_z);
    return NULL;
  }
  ImStack* currentStack(){
    if(m_data) return m_data->fourLocation(m_view_p,m_view_t);
    return NULL;
  }
  ImRecord* currentRecord(){
    if(m_data) return m_records.at(m_view_p*m_data->nt() + m_view_t);
    return NULL;
  }
  void setRed(uint8_t chan){ m_red = chan; }
  void setGreen(uint8_t chan){ m_green = chan; }
  void setBlue(uint8_t chan){ m_blue = chan; }
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

};

#endif
