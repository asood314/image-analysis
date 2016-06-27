#include "NiaViewer.hpp"

NiaViewer::NiaViewer() :
  ScrolledWindow(),
  m_view_w(0), m_view_z(0), m_view_p(0), m_view_t(0),
  m_red(255), m_green(255), m_blue(255),
  m_grayMin(0), m_grayMax(65535),
  m_redMin(0), m_redMax(65535), m_greenMin(0), m_greenMax(65535), m_blueMin(0), m_blueMax(65535),
  m_mode(GRAY)
{
  m_data = NULL;
  add(m_displayImage);
}

NiaViewer::~NiaViewer()
{
  if(m_data) delete m_data;
}

void NiaViewer::setData(ImSeries* data)
{
  if(m_data) delete m_data;
  m_data = data;
  m_view_w = 0;
  m_view_z = 0;
  m_view_p = 0;
  m_view_t = 0;
  if(m_mode == GRAY){
    autoscaleGray();
    //void* buf = m_pixbuf->get_data();
    m_pixbuf = createPixbuf(m_data->fourLocation(0,0)->frame(0,0));
    //if(buf) free(buf);
  }
  else if(m_mode == RGB){
    autoscaleRGB();
    //void* buf = m_pixbuf->get_data();
    m_pixbuf = createPixbuf(m_data->fourLocation(0,0));
    //if(buf) free(buf);
  }
  m_displayImage.set(m_pixbuf);
}

void NiaViewer::autoscale()
{
  if(m_mode == GRAY) autoscaleGray();
  else if(m_mode == RGB) autoscaleRGB();
  updateImage();
}

void NiaViewer::setMode(NiaViewer::ImageMode mode)
{
  m_mode = mode;
  updateImage();
}

void NiaViewer::setWavelength(uint8_t w)
{
  m_mode = GRAY;
  m_view_w = w;
  updateImage();
}

void NiaViewer::prevZ()
{
  if(m_view_z > 0){
    m_view_z--;
    updateImage();
  }
}

void NiaViewer::nextZ()
{
  if(m_view_z < m_data->fourLocation(m_view_p,m_view_t)->nz()-1){
    m_view_z++;
    updateImage();
  }
}

void NiaViewer::prevPosition()
{
  if(m_view_p > 0){
    m_view_p--;
    updateImage();
  }
}

void NiaViewer::nextPosition()
{
  if(m_view_p < m_data->npos()-1){
    m_view_p++;
    updateImage();
  }
}

void NiaViewer::prevTimepoint()
{
  if(m_view_t > 0){
    m_view_t--;
    updateImage();
  }
}

void NiaViewer::nextTimepoint()
{
  if(m_view_t < m_data->nt()-1){
    m_view_t++;
    updateImage();
  }
}

void NiaViewer::updateImage()
{
  if(m_mode == GRAY){
    //void* buf = m_pixbuf->get_data();
    m_pixbuf = createPixbuf(m_data->fourLocation(m_view_p,m_view_t)->frame(m_view_w,m_view_z));
    //if(buf) free(buf);
  }
  else if(m_mode == RGB){
    //void* buf = m_pixbuf->get_data();
    m_pixbuf = createPixbuf(m_data->fourLocation(m_view_p,m_view_t));
    //if(buf) free(buf);
  }
  m_displayImage.set(m_pixbuf);
}

Glib::RefPtr<Gdk::Pixbuf> NiaViewer::createPixbuf(ImFrame* frame)
{
  uint8_t* buf = new uint8_t[frame->width()*frame->height()*3];
  double sf = 255.0 / (m_grayMax - m_grayMin);
  uint64_t index = 0;
  for(uint32_t j = 0; j < frame->height(); j++){
    for(uint32_t i = 0; i < frame->width(); i++){
      uint8_t scaled_p = (uint8_t)(sf * (frame->getPixel(i,j) - m_grayMin));
      buf[index] = scaled_p;
      buf[index+1] = scaled_p;
      buf[index+2] = scaled_p;
      index += 3;
    }
  }
  Glib::RefPtr<Gdk::Pixbuf> retval = Gdk::Pixbuf::create_from_data(buf, Gdk::COLORSPACE_RGB, false, 8, frame->width(), frame->height(), frame->width()*3);
  delete[] buf;
  return retval;
}

Glib::RefPtr<Gdk::Pixbuf> NiaViewer::createPixbuf(ImStack* stack)
{
  uint16_t width = stack->frame(0,0)->width();
  uint16_t height = stack->frame(0,0)->height();
  uint8_t* buf = new uint8_t[width*height*3];
  double sfRed = 255.0 / (m_redMax - m_redMin);
  double sfGreen = 255.0 / (m_greenMax - m_greenMin);
  double sfBlue = 255.0 / (m_blueMax - m_blueMin);
  uint64_t index = 0;
  if(m_red < 255){
    if(m_green < 255){
      if(m_blue < 255){
	for(uint32_t j = 0; j < height; j++){
	  for(uint32_t i = 0; i < width; i++){
	    buf[index] = (uint8_t)(sfRed * (stack->frame(m_red,m_view_z)->getPixel(i,j) - m_redMin));
	    buf[index+1] = (uint8_t)(sfGreen * (stack->frame(m_green,m_view_z)->getPixel(i,j) - m_greenMin));
	    buf[index+2] = (uint8_t)(sfBlue * (stack->frame(m_blue,m_view_z)->getPixel(i,j) - m_blueMin));
	    index += 3;
	  }
	}
      }
      else{
	for(uint32_t j = 0; j < height; j++){
	  for(uint32_t i = 0; i < width; i++){
	    buf[index] = (uint8_t)(sfRed * (stack->frame(m_red,m_view_z)->getPixel(i,j) - m_redMin));
	    buf[index+1] = (uint8_t)(sfGreen * (stack->frame(m_green,m_view_z)->getPixel(i,j) - m_greenMin));
	    buf[index+2] = 0;
	    index += 3;
	  }
	}
      }
    }
    else if(m_blue < 255){
      for(uint32_t j = 0; j < height; j++){
	for(uint32_t i = 0; i < width; i++){
	  buf[index] = (uint8_t)(sfRed * (stack->frame(m_red,m_view_z)->getPixel(i,j) - m_redMin));
	  buf[index+1] = 0;
	  buf[index+2] = (uint8_t)(sfBlue * (stack->frame(m_blue,m_view_z)->getPixel(i,j) - m_blueMin));;
	  index += 3;
	}
      }
    }
    else{
      for(uint32_t j = 0; j < height; j++){
	for(uint32_t i = 0; i < width; i++){
	  buf[index] = (uint8_t)(sfRed * (stack->frame(m_red,m_view_z)->getPixel(i,j) - m_redMin));
	  buf[index+1] = 0;
	  buf[index+2] = 0;
	  index += 3;
	}
      }
    }
  }
  else if(m_green < 255){
    if(m_blue < 255){
      for(uint32_t j = 0; j < height; j++){
	for(uint32_t i = 0; i < width; i++){
	  buf[index] = 0;
	  buf[index+1] = (uint8_t)(sfGreen * (stack->frame(m_green,m_view_z)->getPixel(i,j) - m_greenMin));
	  buf[index+2] = (uint8_t)(sfBlue * (stack->frame(m_blue,m_view_z)->getPixel(i,j) - m_blueMin));
	  index += 3;
	}
      }
    }
    else{
      for(uint32_t j = 0; j < height; j++){
	for(uint32_t i = 0; i < width; i++){
	  buf[index] = 0;
	  buf[index+1] = (uint8_t)(sfGreen * (stack->frame(m_green,m_view_z)->getPixel(i,j) - m_greenMin));
	  buf[index+2] = 0;
	  index += 3;
	}
      }
    }
  }
  else if(m_blue < 255){
    for(uint32_t j = 0; j < height; j++){
      for(uint32_t i = 0; i < width; i++){
	buf[index] = 0;
	buf[index+1] = 0;
	buf[index+2] = (uint8_t)(sfBlue * (stack->frame(m_blue,m_view_z)->getPixel(i,j) - m_blueMin));
	index += 3;
      }
    }
  }
  Glib::RefPtr<Gdk::Pixbuf> retval = Gdk::Pixbuf::create_from_data(buf, Gdk::COLORSPACE_RGB, false, 8, width, height, width*3);
  delete[] buf;
  return retval;
}

void NiaViewer::autoscaleGray()
{
  m_grayMin = 65535;
  m_grayMax = 0;
  ImFrame* frame = m_data->fourLocation(m_view_p,m_view_t)->frame(m_view_w,m_view_z);
  for(uint32_t j = 0; j < frame->height(); j++){
    for(uint32_t i = 0; i < frame->width(); i++){
      uint16_t val = frame->getPixel(i,j);
      if(val < m_grayMin) m_grayMin = val;
      if(val > m_grayMax) m_grayMax = val;
    }
  }
}

void NiaViewer::autoscaleRGB()
{
  if(m_red < 255){
    m_redMin = 65535;
    m_redMax = 0;
    ImFrame* frame = m_data->fourLocation(m_view_p,m_view_t)->frame(m_red,m_view_z);
    for(uint32_t j = 0; j < frame->height(); j++){
      for(uint32_t i = 0; i < frame->width(); i++){
	uint16_t val = frame->getPixel(i,j);
	if(val < m_redMin) m_redMin = val;
	if(val > m_redMax) m_redMax = val;
      }
    }
  }
  if(m_green < 255){
    m_greenMin = 65535;
    m_greenMax = 0;
    ImFrame* frame = m_data->fourLocation(m_view_p,m_view_t)->frame(m_green,m_view_z);
    for(uint32_t j = 0; j < frame->height(); j++){
      for(uint32_t i = 0; i < frame->width(); i++){
	uint16_t val = frame->getPixel(i,j);
	if(val < m_greenMin) m_greenMin = val;
	if(val > m_greenMax) m_greenMax = val;
      }
    }
  }
  if(m_blue < 255){
    m_blueMin = 65535;
    m_blueMax = 0;
    ImFrame* frame = m_data->fourLocation(m_view_p,m_view_t)->frame(m_blue,m_view_z);
    for(uint32_t j = 0; j < frame->height(); j++){
      for(uint32_t i = 0; i < frame->width(); i++){
	uint16_t val = frame->getPixel(i,j);
	if(val < m_blueMin) m_blueMin = val;
	if(val > m_blueMax) m_blueMax = val;
      }
    }
  }
}
