#include "NiaViewer.hpp"

NiaViewer::NiaViewer() :
  ScrolledWindow(),
  m_view_w(0), m_view_z(0), m_view_p(0), m_view_t(0),
  m_red(255), m_green(255), m_blue(255),
  m_grayMin(0), m_grayMax(65535),
  m_redMin(0), m_redMax(65535), m_greenMin(0), m_greenMax(65535), m_blueMin(0), m_blueMax(65535),
  m_mode(GRAY),
  m_width(0),m_height(0),m_zoom(1.0)
{
  m_data = NULL;
  m_colors[0].r = 0xff;
  m_colors[0].g = 0x00;
  m_colors[0].b = 0x00;
  m_colors[1].r = 0x00;
  m_colors[1].g = 0xff;
  m_colors[1].b = 0x00;
  m_colors[2].r = 0x00;
  m_colors[2].g = 0x00;
  m_colors[2].b = 0xff;
  add(m_eventBox);
  m_eventBox.set_events(Gdk::BUTTON_PRESS_MASK);
  m_eventBox.signal_button_press_event().connect(sigc::mem_fun(*this, &NiaViewer::on_button_press));
  m_eventBox.add(m_displayImage);
}

NiaViewer::~NiaViewer()
{
  if(m_data){
    delete m_data;
    uint8_t* buf = m_pixbuf->get_pixels();
    if(buf) delete[] buf;
  }
  for(std::vector<ImRecord*>::iterator it = m_records.begin(); it != m_records.end(); it++){
    if(*it) delete *it;
  }
}

bool NiaViewer::on_button_press(GdkEventButton* evt)
{
  std::cout << "Mouse click at (" << evt->x << "," << evt->y << ")" << std::endl;
  return true;
}

void NiaViewer::setData(ImSeries* data)
{
  uint8_t* buf = NULL;
  if(m_data){
    delete m_data;
    buf = m_pixbuf->get_pixels();
  }
  m_data = data;
  m_width = m_data->fourLocation(0,0)->frame(0,0)->width();
  m_height = m_data->fourLocation(0,0)->frame(0,0)->height();
  for(std::vector<ImRecord*>::iterator it = m_records.begin(); it != m_records.end(); it++){
    if(*it) delete *it;
  }
  m_records.clear();
  m_records.assign(m_data->npos()*m_data->nt(),NULL);
  m_view_w = 0;
  m_view_z = 0;
  m_view_p = 0;
  m_view_t = 0;
  if(m_mode == GRAY){
    autoscaleGray();
    m_pixbuf = createPixbuf(m_data->fourLocation(0,0)->frame(0,0));
    if(buf) delete[] buf;
  }
  else if(m_mode == RGB){
    autoscaleRGB();
    m_pixbuf = createPixbuf(m_data->fourLocation(0,0));
    if(buf) delete[] buf;
  }
  Glib::RefPtr<Gdk::Pixbuf> pb = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB,false,8,m_width*m_zoom,m_height*m_zoom);
  m_pixbuf->scale(pb,0,0,m_width*m_zoom,m_height*m_zoom,0,0,m_zoom,m_zoom,Gdk::INTERP_TILES);
  m_displayImage.set(pb);
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
  if(mode == GRAY){
    m_colors[0].r = 0xff;
    m_colors[0].g = 0x00;
    m_colors[0].b = 0x00;
    m_colors[1].r = 0x00;
    m_colors[1].g = 0xff;
    m_colors[1].b = 0x00;
    m_colors[2].r = 0x00;
    m_colors[2].g = 0x00;
    m_colors[2].b = 0xff;
  }
  else if(mode == RGB){
    m_colors[0].r = 0xff;
    m_colors[0].g = 0xff;
    m_colors[0].b = 0xff;
    m_colors[1].r = 0xff;
    m_colors[1].g = 0x00;
    m_colors[1].b = 0xff;
    m_colors[2].r = 0x00;
    m_colors[2].g = 0xff;
    m_colors[2].b = 0xff;
  }
  autoscale();
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
  if(!m_data) return;
  uint8_t* buf = m_pixbuf->get_pixels();
  if(m_mode == GRAY){
    m_pixbuf = createPixbuf(m_data->fourLocation(m_view_p,m_view_t)->frame(m_view_w,m_view_z));
    if(buf) delete[] buf;
  }
  else if(m_mode == RGB){
    m_pixbuf = createPixbuf(m_data->fourLocation(m_view_p,m_view_t));
    if(buf) delete[] buf;
  }
  Glib::RefPtr<Gdk::Pixbuf> pb = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB,false,8,m_width*m_zoom,m_height*m_zoom);
  m_pixbuf->scale(pb,0,0,m_width*m_zoom,m_height*m_zoom,0,0,m_zoom,m_zoom,Gdk::INTERP_TILES);
  m_displayImage.set(pb);
}

void NiaViewer::zoomIn()
{
  m_zoom += 0.1;
  Glib::RefPtr<Gdk::Pixbuf> pb = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB,false,8,m_width*m_zoom,m_height*m_zoom);
  m_pixbuf->scale(pb,0,0,m_width*m_zoom,m_height*m_zoom,0,0,m_zoom,m_zoom,Gdk::INTERP_TILES);
  m_displayImage.set(pb);
}

void NiaViewer::zoomOut()
{
  m_zoom -= 0.1;
  Glib::RefPtr<Gdk::Pixbuf> pb = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB,false,8,m_width*m_zoom,m_height*m_zoom);
  m_pixbuf->scale(pb,0,0,m_width*m_zoom,m_height*m_zoom,0,0,m_zoom,m_zoom,Gdk::INTERP_TILES);
  m_displayImage.set(pb);
}

void NiaViewer::unzoom()
{
  m_zoom = 1.0;
  Glib::RefPtr<Gdk::Pixbuf> pb = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB,false,8,m_width*m_zoom,m_height*m_zoom);
  m_pixbuf->scale(pb,0,0,m_width*m_zoom,m_height*m_zoom,0,0,m_zoom,m_zoom,Gdk::INTERP_TILES);
  m_displayImage.set(pb);
}

void NiaViewer::toggleMask(Mask* m)
{
  for(std::vector<Mask*>::iterator it = m_masks.begin(); it != m_masks.end(); it++){
    if(m->equals(**it)){
      m_masks.erase(it);
      updateImage();
      return;
    }
  }
  m_masks.push_back(m);
  updateImage();
}

void NiaViewer::toggleSignalMask()
{
  if(!m_data) return;
  int index = m_view_p*m_data->nt() + m_view_t;
  if(m_records.at(index)) toggleMask(m_records.at(index)->getSignalMask(m_view_w));
}

void NiaViewer::togglePunctaMask()
{
  if(!m_data) return;
  int index = m_view_p*m_data->nt() + m_view_t;
  if(m_records.at(index)) toggleMask(m_records.at(index)->getPunctaMask(m_view_w,false));
}

void NiaViewer::toggleSynapseMask()
{
  if(!m_data) return;
  int index = m_view_p*m_data->nt() + m_view_t;
  if(m_records.at(index)) toggleMask(m_records.at(index)->getSynapseMask(false));
}

void NiaViewer::toggleRegionMask()
{
  if(!m_data) return;
  int index = m_view_p*m_data->nt() + m_view_t;
  if(m_records.at(index)) toggleMask(m_records.at(index)->getRegionMask(true));
}

void NiaViewer::setRecords(std::vector<ImRecord*> recs)
{
  for(std::vector<ImRecord*>::iterator it = m_records.begin(); it != m_records.end(); it++){
    if(*it) delete *it;
  }
  m_records.clear();
  for(std::vector<ImRecord*>::iterator it = recs.begin(); it != recs.end(); it++) m_records.push_back(*it);
}

void NiaViewer::setCurrentRecord(ImRecord* rec)
{
  if(!m_data) return;
  int index = m_view_p*m_data->nt() + m_view_t;
  if(m_records[index]) delete m_records[index];
  m_records[index] = rec;
}

Glib::RefPtr<Gdk::Pixbuf> NiaViewer::createPixbuf(ImFrame* frame)
{
  unsigned nmasks = m_masks.size();
  uint8_t* buf = new uint8_t[frame->width()*frame->height()*3];
  double sf = 255.0 / (m_grayMax - m_grayMin);
  uint64_t index = 0;
  for(uint32_t j = 0; j < frame->height(); j++){
    for(uint32_t i = 0; i < frame->width(); i++){
      buf[index] = 0;
      buf[index+1] = 0;
      buf[index+2] = 0;
      bool masked = false;
      for(unsigned k = 0; k < nmasks; k++){
	if(m_masks.at(k)->getValue(i,j) > 0){
	  unsigned colorIndex = k % m_ncolors;
	  buf[index] = buf[index] |m_colors[colorIndex].r;
	  buf[index+1] = buf[index+1] | m_colors[colorIndex].g;
	  buf[index+2] = buf[index+2] |m_colors[colorIndex].b;
	  masked = true;
	}
      }
      if(!masked){
	uint16_t val = frame->getPixel(i,j);
	uint8_t scaled_p;
	if(val > m_grayMax) scaled_p = 255;
	else if(val < m_grayMin) scaled_p = 0;
	else scaled_p = (uint8_t)(sf * (val - m_grayMin));
	buf[index] = scaled_p;
	buf[index+1] = scaled_p;
	buf[index+2] = scaled_p;
      }
      index += 3;
    }
  }
  Glib::RefPtr<Gdk::Pixbuf> retval = Gdk::Pixbuf::create_from_data(buf, Gdk::COLORSPACE_RGB, false, 8, frame->width(), frame->height(), frame->width()*3);
  //delete[] buf;
  return retval;
}

Glib::RefPtr<Gdk::Pixbuf> NiaViewer::createPixbuf(ImStack* stack)
{
  unsigned nmasks = m_masks.size();
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
	    buf[index] = 0;
	    buf[index+1] = 0;
	    buf[index+2] = 0;
	    bool masked = false;
	    for(unsigned k = 0; k < nmasks; k++){
	      if(m_masks.at(k)->getValue(i,j) > 0){
		unsigned colorIndex = k % m_ncolors;
		buf[index] = buf[index] | m_colors[colorIndex].r;
		buf[index+1] = buf[index+1] | m_colors[colorIndex].g;
		buf[index+2] = buf[index+2] | m_colors[colorIndex].b;
		masked = true;
	      }
	    }
	    if(!masked){
	      uint16_t rval = stack->frame(m_red,m_view_z)->getPixel(i,j);
	      uint16_t gval = stack->frame(m_green,m_view_z)->getPixel(i,j);
	      uint16_t bval = stack->frame(m_blue,m_view_z)->getPixel(i,j);
	      buf[index] = (uint8_t)(sfRed * (rval-(rval-m_redMax)*(rval>m_redMax) - m_redMin)*(rval > m_redMin));
	      buf[index+1] = (uint8_t)(sfGreen * (gval-(gval-m_greenMax)*(gval>m_greenMax) - m_greenMin)*(gval > m_greenMin));
	      buf[index+2] = (uint8_t)(sfBlue * (bval-(bval-m_blueMax)*(bval>m_blueMax) - m_blueMin)*(bval > m_blueMin));
	    }
	    index += 3;
	  }
	}
      }
      else{
	for(uint32_t j = 0; j < height; j++){
	  for(uint32_t i = 0; i < width; i++){
	    buf[index] = 0;
	    buf[index+1] = 0;
	    buf[index+2] = 0;
	    bool masked = false;
	    for(unsigned k = 0; k < nmasks; k++){
	      if(m_masks.at(k)->getValue(i,j) > 0){
		unsigned colorIndex = k % m_ncolors;
		buf[index] = buf[index] | m_colors[colorIndex].r;
		buf[index+1] = buf[index+1] | m_colors[colorIndex].g;
		buf[index+2] = buf[index+2] | m_colors[colorIndex].b;
		masked = true;
	      }
	    }
	    if(!masked){
	      uint16_t rval = stack->frame(m_red,m_view_z)->getPixel(i,j);
	      uint16_t gval = stack->frame(m_green,m_view_z)->getPixel(i,j);
	      buf[index] = (uint8_t)(sfRed * (rval-(rval-m_redMax)*(rval>m_redMax) - m_redMin)*(rval > m_redMin));
	      buf[index+1] = (uint8_t)(sfGreen * (gval-(gval-m_greenMax)*(gval>m_greenMax) - m_greenMin)*(gval > m_greenMin));
	      buf[index+2] = 0;
	    }
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
	  buf[index+2] = 0;
	  bool masked = false;
	  for(unsigned k = 0; k < nmasks; k++){
	    if(m_masks.at(k)->getValue(i,j) > 0){
	      unsigned colorIndex = k % m_ncolors;
	      buf[index] = buf[index] | m_colors[colorIndex].r;
	      buf[index+1] = buf[index+1] | m_colors[colorIndex].g;
	      buf[index+2] = buf[index+2] | m_colors[colorIndex].b;
	      masked = true;
	    }
	  }
	  if(!masked){
	    uint16_t rval = stack->frame(m_red,m_view_z)->getPixel(i,j);
	    uint16_t bval = stack->frame(m_blue,m_view_z)->getPixel(i,j);
	    buf[index] = (uint8_t)(sfRed * (rval-(rval-m_redMax)*(rval>m_redMax) - m_redMin)*(rval > m_redMin));
	    buf[index+1] = 0;
	    buf[index+2] = (uint8_t)(sfBlue * (bval-(bval-m_blueMax)*(bval>m_blueMax) - m_blueMin)*(bval > m_blueMin));
	  }
	  index += 3;
	}
      }
    }
    else{
      for(uint32_t j = 0; j < height; j++){
	for(uint32_t i = 0; i < width; i++){
	  buf[index] = 0;
	  buf[index+1] = 0;
	  buf[index+2] = 0;
	  bool masked = false;
	  for(unsigned k = 0; k < nmasks; k++){
	    if(m_masks.at(k)->getValue(i,j) > 0){
	      unsigned colorIndex = k % m_ncolors;
	      buf[index] = buf[index] | m_colors[colorIndex].r;
	      buf[index+1] = buf[index+1] | m_colors[colorIndex].g;
	      buf[index+2] = buf[index+2] | m_colors[colorIndex].b;
	      masked = true;
	    }
	  }
	  if(!masked){
	    uint16_t rval = stack->frame(m_red,m_view_z)->getPixel(i,j);
	    buf[index] = (uint8_t)(sfRed * (rval-(rval-m_redMax)*(rval>m_redMax) - m_redMin)*(rval > m_redMin));
	    buf[index+1] = 0;
	    buf[index+2] = 0;
	  }
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
	  buf[index+1] = 0;
	  buf[index+2] = 0;
	  bool masked = false;
	  for(unsigned k = 0; k < nmasks; k++){
	    if(m_masks.at(k)->getValue(i,j) > 0){
	      unsigned colorIndex = k % m_ncolors;
	      buf[index] = buf[index] | m_colors[colorIndex].r;
	      buf[index+1] = buf[index+1] | m_colors[colorIndex].g;
	      buf[index+2] = buf[index+2] | m_colors[colorIndex].b;
	      masked = true;
	    }
	  }
	  if(!masked){
	    uint16_t gval = stack->frame(m_green,m_view_z)->getPixel(i,j);
	    uint16_t bval = stack->frame(m_blue,m_view_z)->getPixel(i,j);
	    buf[index] = 0;
	    buf[index+1] = (uint8_t)(sfGreen * (gval-(gval-m_greenMax)*(gval>m_greenMax) - m_greenMin)*(gval > m_greenMin));
	    buf[index+2] = (uint8_t)(sfBlue * (bval-(bval-m_blueMax)*(bval>m_blueMax) - m_blueMin)*(bval > m_blueMin));
	  }
	  index += 3;
	}
      }
    }
    else{
      for(uint32_t j = 0; j < height; j++){
	for(uint32_t i = 0; i < width; i++){
	  buf[index] = 0;
	  buf[index+1] = 0;
	  buf[index+2] = 0;
	  bool masked = false;
	  for(unsigned k = 0; k < nmasks; k++){
	    if(m_masks.at(k)->getValue(i,j) > 0){
	      unsigned colorIndex = k % m_ncolors;
	      buf[index] = buf[index] | m_colors[colorIndex].r;
	      buf[index+1] = buf[index+1] | m_colors[colorIndex].g;
	      buf[index+2] = buf[index+2] | m_colors[colorIndex].b;
	      masked = true;
	    }
	  }
	  if(!masked){
	    uint16_t gval = stack->frame(m_green,m_view_z)->getPixel(i,j);
	    buf[index] = 0;
	    buf[index+1] = (uint8_t)(sfGreen * (gval-(gval-m_greenMax)*(gval>m_greenMax) - m_greenMin)*(gval > m_greenMin));
	    buf[index+2] = 0;
	  }
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
	buf[index+2] = 0;
	bool masked = false;
	for(unsigned k = 0; k < nmasks; k++){
	  if(m_masks.at(k)->getValue(i,j) > 0){
	    unsigned colorIndex = k % m_ncolors;
	    buf[index] = buf[index] | m_colors[colorIndex].r;
	    buf[index+1] = buf[index+1] | m_colors[colorIndex].g;
	    buf[index+2] = buf[index+2] | m_colors[colorIndex].b;
	    masked = true;
	  }
	}
	if(!masked){
	  uint16_t bval = stack->frame(m_blue,m_view_z)->getPixel(i,j);
	  buf[index] = 0;
	  buf[index+1] = 0;
	  buf[index+2] = (uint8_t)(sfBlue * (bval-(bval-m_blueMax)*(bval>m_blueMax) - m_blueMin)*(bval > m_blueMin));
	}
	index += 3;
      }
    }
  }
  Glib::RefPtr<Gdk::Pixbuf> retval = Gdk::Pixbuf::create_from_data(buf, Gdk::COLORSPACE_RGB, false, 8, width, height, width*3);
  //delete[] buf;
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
