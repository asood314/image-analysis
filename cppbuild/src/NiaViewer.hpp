#ifndef NIA_VIEWER_HPP
#define NIA_VIEWER_HPP

#include <gtkmm-2.4/gtkmm.h>
#include "ImSeries.hpp"
#include "FileManager.hpp"

class NiaViewer : public Gtk::ScrolledWindow
{

public:
  enum ImageMode{ GRAY=0,RGB };

protected:
  ImSeries* m_data;
  uint8_t m_view_w,m_view_z,m_view_p,m_view_t;
  uint8_t m_red,m_green,m_blue;
  uint16_t m_grayMin,m_grayMax,m_redMin,m_redMax,m_greenMin,m_greenMax,m_blueMin,m_blueMax;
  ImageMode m_mode;
  Gtk::Image m_displayImage;
  Glib::RefPtr<Gdk::Pixbuf> m_pixbuf;
  Glib::RefPtr<Gdk::Pixbuf> createPixbuf(ImFrame* frame);
  Glib::RefPtr<Gdk::Pixbuf> createPixbuf(ImStack* stack);
  void autoscaleGray();
  void autoscaleRGB();
  void updateImage();
  
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
  void setRed(uint8_t chan){ m_red = chan; }
  void setGreen(uint8_t chan){ m_green = chan; }
  void setBlue(uint8_t chan){ m_blue = chan; }

};

#endif
