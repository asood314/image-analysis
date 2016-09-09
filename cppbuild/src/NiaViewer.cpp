#include "NiaViewer.hpp"

NiaViewer::NiaViewer() :
  VBox(),
  m_view_w(0), m_view_z(0), m_view_p(0), m_view_t(0),
  m_red(255), m_green(255), m_blue(255),
  m_grayMin(0), m_grayMax(65535),
  m_redMin(0), m_redMax(65535), m_greenMin(0), m_greenMax(65535), m_blueMin(0), m_blueMax(65535),
  m_mode(GRAY),
  m_width(0),m_height(0),m_zoom(1.0),
  m_grayMinLabel("Gray Min."),m_grayMaxLabel("Gray Max."),
  m_redMinLabel("Red Min."),m_redMaxLabel("Red Max."),m_greenMinLabel("Green Min."),m_greenMaxLabel("Green Max."),m_blueMinLabel("Blue Min."),m_blueMaxLabel("Blue Max."),
  m_scaleHideButton("Hide"),
  m_alignment(Gtk::ALIGN_CENTER,Gtk::ALIGN_CENTER,0.0,0.0),
  m_prevButton(0)
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
  m_pixelSelector = true;
  m_punctaSelector = false;
  m_synapseSelector = false;
  m_regionSelector = false;
  m_axisSelector = false;

  m_grayMinEntry.set_max_length(5);
  m_grayMinEntry.set_width_chars(5);
  m_grayMinEntry.set_text(boost::lexical_cast<std::string>((int)m_grayMin));
  m_grayMinEntry.signal_activate().connect(sigc::mem_fun(*this, &NiaViewer::scale));
  m_grayMaxEntry.set_max_length(5);
  m_grayMaxEntry.set_width_chars(5);
  m_grayMaxEntry.set_text(boost::lexical_cast<std::string>((int)m_grayMax));
  m_grayMaxEntry.signal_activate().connect(sigc::mem_fun(*this, &NiaViewer::scale));
  m_redMinEntry.set_max_length(5);
  m_redMinEntry.set_width_chars(5);
  m_redMinEntry.set_text(boost::lexical_cast<std::string>((int)m_redMin));
  m_redMinEntry.signal_activate().connect(sigc::mem_fun(*this, &NiaViewer::scale));
  m_redMaxEntry.set_max_length(5);
  m_redMaxEntry.set_width_chars(5);
  m_redMaxEntry.set_text(boost::lexical_cast<std::string>((int)m_redMax));
  m_redMaxEntry.signal_activate().connect(sigc::mem_fun(*this, &NiaViewer::scale));
  m_greenMinEntry.set_max_length(5);
  m_greenMinEntry.set_width_chars(5);
  m_greenMinEntry.set_text(boost::lexical_cast<std::string>((int)m_greenMin));
  m_greenMinEntry.signal_activate().connect(sigc::mem_fun(*this, &NiaViewer::scale));
  m_greenMaxEntry.set_max_length(5);
  m_greenMaxEntry.set_width_chars(5);
  m_greenMaxEntry.set_text(boost::lexical_cast<std::string>((int)m_greenMax));
  m_greenMaxEntry.signal_activate().connect(sigc::mem_fun(*this, &NiaViewer::scale));
  m_blueMinEntry.set_max_length(5);
  m_blueMinEntry.set_width_chars(5);
  m_blueMinEntry.set_text(boost::lexical_cast<std::string>((int)m_blueMin));
  m_blueMinEntry.signal_activate().connect(sigc::mem_fun(*this, &NiaViewer::scale));
  m_blueMaxEntry.set_max_length(5);
  m_blueMaxEntry.set_width_chars(5);
  m_blueMaxEntry.set_text(boost::lexical_cast<std::string>((int)m_blueMax));
  m_blueMaxEntry.signal_activate().connect(sigc::mem_fun(*this, &NiaViewer::scale));
  m_scaleHideButton.signal_clicked().connect(sigc::mem_fun(*this, &NiaViewer::hideScaleBox));
  m_vbox1.pack_start(m_grayMinLabel,Gtk::PACK_SHRINK);
  m_vbox1.pack_start(m_grayMinEntry,Gtk::PACK_SHRINK);
  m_scaleBox.pack_start(m_vbox1,Gtk::PACK_SHRINK,15);
  m_vbox2.pack_start(m_grayMaxLabel,Gtk::PACK_SHRINK);
  m_vbox2.pack_start(m_grayMaxEntry,Gtk::PACK_SHRINK);
  m_scaleBox.pack_start(m_vbox2,Gtk::PACK_SHRINK,15);
  m_scaleBox.pack_start(m_vsep1,Gtk::PACK_SHRINK,30);
  m_vbox3.pack_start(m_redMinLabel,Gtk::PACK_SHRINK);
  m_vbox3.pack_start(m_redMinEntry,Gtk::PACK_SHRINK);
  m_scaleBox.pack_start(m_vbox3,Gtk::PACK_SHRINK,15);
  m_vbox4.pack_start(m_redMaxLabel,Gtk::PACK_SHRINK);
  m_vbox4.pack_start(m_redMaxEntry,Gtk::PACK_SHRINK);
  m_scaleBox.pack_start(m_vbox4,Gtk::PACK_SHRINK,15);
  m_vbox5.pack_start(m_greenMinLabel,Gtk::PACK_SHRINK);
  m_vbox5.pack_start(m_greenMinEntry,Gtk::PACK_SHRINK);
  m_scaleBox.pack_start(m_vbox5,Gtk::PACK_SHRINK,15);
  m_vbox6.pack_start(m_greenMaxLabel,Gtk::PACK_SHRINK);
  m_vbox6.pack_start(m_greenMaxEntry,Gtk::PACK_SHRINK);
  m_scaleBox.pack_start(m_vbox6,Gtk::PACK_SHRINK,15);
  m_vbox7.pack_start(m_blueMinLabel,Gtk::PACK_SHRINK);
  m_vbox7.pack_start(m_blueMinEntry,Gtk::PACK_SHRINK);
  m_scaleBox.pack_start(m_vbox7,Gtk::PACK_SHRINK,15);
  m_vbox8.pack_start(m_blueMaxLabel,Gtk::PACK_SHRINK);
  m_vbox8.pack_start(m_blueMaxEntry,Gtk::PACK_SHRINK);
  m_scaleBox.pack_start(m_vbox8,Gtk::PACK_SHRINK,15);
  m_scaleBox.pack_start(m_vsep2,Gtk::PACK_SHRINK,30);
  m_scaleBox.pack_start(m_scaleHideButton,Gtk::PACK_SHRINK,15);
  //m_mainBox.pack_start(m_scaleBox,Gtk::PACK_SHRINK);
  pack_start(m_scaleBox,Gtk::PACK_SHRINK);
  pack_start(m_swin,Gtk::PACK_EXPAND_WIDGET);

  m_swin.add(m_alignment);
  m_alignment.add(m_eventBox);
  m_eventBox.set_events(Gdk::BUTTON_PRESS_MASK);
  m_eventBox.signal_button_press_event().connect(sigc::mem_fun(*this, &NiaViewer::on_button_press));
  m_eventBox.add(m_displayImage);
  //m_eventBox.set_size_request(0,0);
  //m_mainBox.pack_start(m_eventBox,false,false);

  //add(m_mainBox);
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
  if(!m_data) return false;
  LocalizedObject::Point thisClick;
  thisClick.x = (int)(evt->x / m_zoom);
  thisClick.y = (int)(evt->y / m_zoom);
  if(evt->type == GDK_BUTTON_PRESS){
    if(m_pixelSelector){
      std::cout << "Location (" << thisClick.x << "," << thisClick.y << ") has intensity " << currentFrame()->getPixel(thisClick.x,thisClick.y) << std::endl;
      return true;
    }
    else if(m_punctaSelector){
      ImRecord* rec = currentRecord();
      if(!rec){
	std::cout << "Couldn't find image record" << std::endl;
	return false;
      }
      Cluster* c = NULL;
      if(m_mode == GRAY) c = rec->selectPunctum(m_view_w,thisClick);
      else if(m_mode == RGB) c = rec->selectPunctum(thisClick);
      if(c){
	toggleMask(c->getMask(m_width,m_height,false));
	std::cout << "Center: (" << c->center().x << "," << c->center().y << "), Size: " << c->size() << "\nPeak Intensity: " << c->peak() << ", Integrated Intensity: " << c->integratedIntensity() << std::endl;
	return true;
      }
      std::cout << "Couldn't find any puncta" << std::endl;
      return false;
    }
    else if(m_synapseSelector){
      ImRecord* rec = currentRecord();
      if(!rec) return false;
      Synapse* s = NULL;
      s = rec->selectSynapse(thisClick);
      if(s){
	toggleMask(s->getMask(m_width,m_height,false));
	//std::cout << "Center: (" << c->center().x << "," << c->center().y << "), Size: " << c->size() << "\nPeak Intensity: " << c->peak() << ", Integrated Intensity: " << c->integratedIntensity() << std::endl;
	return true;
      }
      return false;
    }
    else if(m_regionSelector){
      if(evt->button == 1){
	LocalizedObject::Point lastClick;
	if(m_prevClicks.size() > 0) lastClick = m_prevClicks.at(m_prevClicks.size()-1);
	if(thisClick.x == lastClick.x && thisClick.y == lastClick.y){
	  if(m_prevClicks.size() > 2){
	    Region* r = new Region(m_prevClicks);
	    ImRecord* rec = currentRecord();
	    if(!rec){
	      rec = new ImRecord(getNW(),m_width,m_height);
	      rec->setResolutionXY(m_data->resolutionXY());
	      setCurrentRecord(rec);
	    }
	    rec->addRegion(r);
	    toggleMask(r->getMask(m_width,m_height,true));
	  }
	  m_prevClicks.clear();
	}
	else m_prevClicks.push_back(thisClick);
      }
      else if(evt->button == 3){
	m_prevClicks.clear();
	if(m_prevButton == 3){
	  ImRecord* rec = currentRecord();
	  if(rec){
	    int lastReg = rec->nRegions() - 1;
	    removeMask(rec->getRegion(lastReg)->getMask(m_width,m_height,true));
	    if(lastReg >= 0) rec->removeRegion(lastReg);
	  }
	}
      }
      m_prevButton = evt->button;
      return true;
    }
    else if(m_axisSelector){
      if(evt->button == 1){
	LocalizedObject::Point lastClick;
	if(m_prevClicks.size() > 0) lastClick = m_prevClicks.at(m_prevClicks.size()-1);
	if(thisClick.x == lastClick.x && thisClick.y == lastClick.y){
	  if(m_prevClicks.size() > 1){
	    ImRecord* rec = currentRecord();
	    if(rec){
	      int maxContained = -1;
	      Region* bestReg = NULL;
	      for(int i = 0; i < rec->nRegions(); i++){
		int nContained = 0;
		Region* r = rec->getRegion(i);
		for(std::vector<LocalizedObject::Point>::iterator it = m_prevClicks.begin(); it != m_prevClicks.end(); it++){
		  if(r->contains(*it)) nContained++;
		}
		if(nContained > maxContained){
		  maxContained = nContained;
		  bestReg = r;
		}
	      }
	      if(bestReg){
		bestReg->setAxis(m_prevClicks);
		toggleMask(bestReg->getMask(m_width,m_height,true));
	      }
	    }
	  }
	  m_prevClicks.clear();
	}
	else m_prevClicks.push_back(thisClick);
      }
      else if(evt->button == 3){
	m_prevClicks.clear();
	if(m_prevButton == 3){
	  ImRecord* rec = currentRecord();
	  if(rec){
	    for(int i = 0; i < rec->nRegions(); i++){
	      Region* r = rec->getRegion(i);
	      if(r->contains(thisClick)){
		r->clearAxis();
		break;
	      }
	    }
	  }
	}
      }
      m_prevButton = evt->button;
      return true;
    }
  }
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
  m_records.assign(m_data->npos()*m_data->nt()*m_data->fourLocation(0,0)->nz(),NULL);
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
  m_eventBox.hide();
  m_eventBox.set_size_request(m_width*m_zoom,m_height*m_zoom);
  m_eventBox.show();
}

void NiaViewer::zproject()
{
  if(!m_data) return;
  m_view_z = 0;
  int np = m_data->npos();
  int nt = m_data->nt();
  int nz = m_data->fourLocation(0,0)->nz();
  m_data->zproject();
  if(m_records.size() > np*nt){
    unsigned index = 0;
    for(int p = 0; p < np; p++){
      for(int t = 0; t < nt; t++){
	index++;
	for(int z = 1; z < nz; z++){
	  if(m_records[index]) delete m_records[index];
	  m_records.erase(m_records.begin()+index);
	}
      }
    }
  }
  autoscale();
  updateImage();
}

void NiaViewer::scale()
{
  if(m_mode == GRAY){
    m_grayMin = boost::lexical_cast<int>(m_grayMinEntry.get_text());
    m_grayMax = boost::lexical_cast<int>(m_grayMaxEntry.get_text());
    updateImage();
  }
  else if(m_mode == RGB){
    m_redMin = boost::lexical_cast<int>(m_redMinEntry.get_text());
    m_redMax = boost::lexical_cast<int>(m_redMaxEntry.get_text());
    m_greenMin = boost::lexical_cast<int>(m_greenMinEntry.get_text());
    m_greenMax = boost::lexical_cast<int>(m_greenMaxEntry.get_text());
    m_blueMin = boost::lexical_cast<int>(m_blueMinEntry.get_text());
    m_blueMax = boost::lexical_cast<int>(m_blueMaxEntry.get_text());
    updateImage();
  }
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

void NiaViewer::setWavelength(int w)
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
  m_eventBox.hide();
  m_eventBox.set_size_request(m_width*m_zoom,m_height*m_zoom);
  m_eventBox.show();
}

void NiaViewer::displayMask(Mask* m)
{
  if(!m_data) return;
  uint8_t* buf = m_pixbuf->get_pixels();
  double sf = 255.0 / (m_grayMax - m_grayMin);
  uint64_t index = 0;
  for(int j = 0; j < m->height(); j++){
    for(int i = 0; i < m->width(); i++){
      buf[index] = 0;
      buf[index+1] = 0;
      buf[index+2] = 0;
      int val = m->getValue(i,j);
      uint8_t scaled_p;
      if(val > m_grayMax) scaled_p = 255;
      else if(val < m_grayMin) scaled_p = 0;
      else scaled_p = (uint8_t)(sf * (val - m_grayMin));
      //buf[index] = scaled_p;
      //buf[index+((val%6)/2)] = scaled_p;
      //buf[index+2] = scaled_p;
      buf[index+(val%3)] = scaled_p;
      index += 3;
    }
  }
  Glib::RefPtr<Gdk::Pixbuf> pb = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB,false,8,m_width*m_zoom,m_height*m_zoom);
  m_pixbuf->scale(pb,0,0,m_width*m_zoom,m_height*m_zoom,0,0,m_zoom,m_zoom,Gdk::INTERP_TILES);
  m_displayImage.set(pb);
  m_eventBox.hide();
  m_eventBox.set_size_request(m_width*m_zoom,m_height*m_zoom);
  m_eventBox.show();
  delete m;
}

void NiaViewer::showContourMap()
{
  ImRecord* rec = currentRecord();
  if(!rec) return;
  //displayMask(rec->getContourMap(m_view_w));
  displayMask(rec->segment(m_view_w));
}

void NiaViewer::zoomIn()
{
  m_zoom += 0.1;
  Glib::RefPtr<Gdk::Pixbuf> pb = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB,false,8,m_width*m_zoom,m_height*m_zoom);
  m_pixbuf->scale(pb,0,0,m_width*m_zoom,m_height*m_zoom,0,0,m_zoom,m_zoom,Gdk::INTERP_TILES);
  m_displayImage.set(pb);
  m_eventBox.hide();
  m_eventBox.set_size_request(m_width*m_zoom,m_height*m_zoom);
  m_eventBox.show();
}

void NiaViewer::zoomOut()
{
  m_zoom -= 0.1;
  Glib::RefPtr<Gdk::Pixbuf> pb = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB,false,8,m_width*m_zoom,m_height*m_zoom);
  m_pixbuf->scale(pb,0,0,m_width*m_zoom,m_height*m_zoom,0,0,m_zoom,m_zoom,Gdk::INTERP_TILES);
  m_displayImage.set(pb);
  m_eventBox.hide();
  m_eventBox.set_size_request(m_width*m_zoom,m_height*m_zoom);
  m_eventBox.show();
}

void NiaViewer::unzoom()
{
  m_zoom = 1.0;
  Glib::RefPtr<Gdk::Pixbuf> pb = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB,false,8,m_width*m_zoom,m_height*m_zoom);
  m_pixbuf->scale(pb,0,0,m_width*m_zoom,m_height*m_zoom,0,0,m_zoom,m_zoom,Gdk::INTERP_TILES);
  m_displayImage.set(pb);
  m_eventBox.hide();
  m_eventBox.set_size_request(m_width*m_zoom,m_height*m_zoom);
  m_eventBox.show();
}

void NiaViewer::toggleMask(Mask* m)
{
  if(!m) return;
  for(std::vector<Mask*>::iterator it = m_masks.begin(); it != m_masks.end(); it++){
    if(m->equals(**it)){
      m_masks.erase(it);
      delete m;
      updateImage();
      return;
    }
  }
  m_masks.push_back(m);
  updateImage();
}

void NiaViewer::removeMask(Mask* m)
{
  if(!m) return;
  for(std::vector<Mask*>::iterator it = m_masks.begin(); it != m_masks.end(); it++){
    if(m->equals(**it)){
      delete *it;
      m_masks.erase(it);
      delete m;
      updateImage();
      return;
    }
  }
}

void NiaViewer::toggleSignalMask()
{
  if(!m_data) return;
  int nz = m_data->fourLocation(m_view_p,m_view_t)->nz();
  int index = m_view_p*m_data->nt()*nz + m_view_t*nz + m_view_z;
  if(m_records.at(index)) toggleMask(m_records.at(index)->getSignalMask(m_view_w)->getCopy());
}

void NiaViewer::togglePunctaMask()
{
  if(!m_data) return;
  int nz = m_data->fourLocation(m_view_p,m_view_t)->nz();
  int index = m_view_p*m_data->nt()*nz + m_view_t*nz + m_view_z;
  if(m_records.at(index)) toggleMask(m_records.at(index)->getPunctaMask(m_view_w,false));
}

void NiaViewer::toggleSynapseMask()
{
  if(!m_data) return;
  int nz = m_data->fourLocation(m_view_p,m_view_t)->nz();
  int index = m_view_p*m_data->nt()*nz + m_view_t*nz + m_view_z;
  if(m_records.at(index)) toggleMask(m_records.at(index)->getSynapseMask(false));
}

void NiaViewer::toggleRegionMask()
{
  if(!m_data) return;
  int nz = m_data->fourLocation(m_view_p,m_view_t)->nz();
  int index = m_view_p*m_data->nt()*nz + m_view_t*nz + m_view_z;
  if(m_records.at(index)) toggleMask(m_records.at(index)->getRegionMask(true));
}

void NiaViewer::clearMasks()
{
  for(std::vector<Mask*>::iterator it = m_masks.begin(); it != m_masks.end(); it++){
    if((*it)) delete *it;
  }
  m_masks.clear();
  updateImage();
}

void NiaViewer::setRecords(std::vector<ImRecord*> recs)
{
  for(std::vector<ImRecord*>::iterator it = m_records.begin(); it != m_records.end(); it++){
    if(*it) delete *it;
  }
  m_records.clear();
  for(std::vector<ImRecord*>::iterator it = recs.begin(); it != recs.end(); it++) m_records.push_back(*it);
}

ImRecord* NiaViewer::getRecord(int pos, int t, int z)
{
  if(!m_data) return NULL;
  int nz = m_data->fourLocation(pos,t)->nz();
  return m_records.at(pos*m_data->nt()*nz + t*nz + z);
}

ImRecord* NiaViewer::currentRecord()
{
  if(!m_data) return NULL;
  int nz = m_data->fourLocation(m_view_p,m_view_t)->nz();
  return m_records.at(m_view_p*m_data->nt()*nz + m_view_t*nz + m_view_z);
}

void NiaViewer::setCurrentRecord(ImRecord* rec)
{
  if(!m_data) return;
  int nz = m_data->fourLocation(m_view_p,m_view_t)->nz();
  int index = m_view_p*m_data->nt()*nz + m_view_t*nz + m_view_z;
  if(m_records[index]) delete m_records[index];
  m_records[index] = rec;
}

void NiaViewer::shareRegions()
{
  ImRecord* rec = currentRecord();
  if(!rec) return;
  int nt = m_data->nt();
  int nz = m_data->fourLocation(m_view_p,m_view_t)->nz();
  for(int z = 0; z < nz; z++){
    if(z == m_view_z) continue;
    int index = m_view_p*nt*nz + m_view_t*nz + z;
    ImRecord* rec2 = m_records[index];
    if(!rec2){
      ImFrame* frame = currentFrame();
      rec2 = new ImRecord(getNW(),frame->width(),frame->height());
      rec2->setResolutionXY(m_data->resolutionXY());
      m_records[index] = rec2;
    }
    rec2->clearRegions();
    for(int r = 0; r < rec->nRegions(); r++) rec2->addRegion(rec->getRegion(r)->getCopy());
  }
}

Glib::RefPtr<Gdk::Pixbuf> NiaViewer::createPixbuf(ImFrame* frame)
{
  unsigned nmasks = m_masks.size();
  uint8_t* buf = new uint8_t[frame->width()*frame->height()*3];
  double sf = 255.0 / (m_grayMax - m_grayMin);
  uint64_t index = 0;
  for(int j = 0; j < frame->height(); j++){
    for(int i = 0; i < frame->width(); i++){
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
	int val = frame->getPixel(i,j);
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
  int width = stack->frame(0,0)->width();
  int height = stack->frame(0,0)->height();
  uint8_t* buf = new uint8_t[width*height*3];
  double sfRed = 255.0 / (m_redMax - m_redMin);
  double sfGreen = 255.0 / (m_greenMax - m_greenMin);
  double sfBlue = 255.0 / (m_blueMax - m_blueMin);
  uint64_t index = 0;
  if(m_red < 255){
    if(m_green < 255){
      if(m_blue < 255){
	for(int j = 0; j < height; j++){
	  for(int i = 0; i < width; i++){
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
	      int rval = stack->frame(m_red,m_view_z)->getPixel(i,j);
	      int gval = stack->frame(m_green,m_view_z)->getPixel(i,j);
	      int bval = stack->frame(m_blue,m_view_z)->getPixel(i,j);
	      buf[index] = (uint8_t)(sfRed * (rval-(rval-m_redMax)*(rval>m_redMax) - m_redMin)*(rval > m_redMin));
	      buf[index+1] = (uint8_t)(sfGreen * (gval-(gval-m_greenMax)*(gval>m_greenMax) - m_greenMin)*(gval > m_greenMin));
	      buf[index+2] = (uint8_t)(sfBlue * (bval-(bval-m_blueMax)*(bval>m_blueMax) - m_blueMin)*(bval > m_blueMin));
	    }
	    index += 3;
	  }
	}
      }
      else{
	for(int j = 0; j < height; j++){
	  for(int i = 0; i < width; i++){
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
	      int rval = stack->frame(m_red,m_view_z)->getPixel(i,j);
	      int gval = stack->frame(m_green,m_view_z)->getPixel(i,j);
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
      for(int j = 0; j < height; j++){
	for(int i = 0; i < width; i++){
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
	    int rval = stack->frame(m_red,m_view_z)->getPixel(i,j);
	    int bval = stack->frame(m_blue,m_view_z)->getPixel(i,j);
	    buf[index] = (uint8_t)(sfRed * (rval-(rval-m_redMax)*(rval>m_redMax) - m_redMin)*(rval > m_redMin));
	    buf[index+1] = 0;
	    buf[index+2] = (uint8_t)(sfBlue * (bval-(bval-m_blueMax)*(bval>m_blueMax) - m_blueMin)*(bval > m_blueMin));
	  }
	  index += 3;
	}
      }
    }
    else{
      for(int j = 0; j < height; j++){
	for(int i = 0; i < width; i++){
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
	    int rval = stack->frame(m_red,m_view_z)->getPixel(i,j);
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
      for(int j = 0; j < height; j++){
	for(int i = 0; i < width; i++){
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
	    int gval = stack->frame(m_green,m_view_z)->getPixel(i,j);
	    int bval = stack->frame(m_blue,m_view_z)->getPixel(i,j);
	    buf[index] = 0;
	    buf[index+1] = (uint8_t)(sfGreen * (gval-(gval-m_greenMax)*(gval>m_greenMax) - m_greenMin)*(gval > m_greenMin));
	    buf[index+2] = (uint8_t)(sfBlue * (bval-(bval-m_blueMax)*(bval>m_blueMax) - m_blueMin)*(bval > m_blueMin));
	  }
	  index += 3;
	}
      }
    }
    else{
      for(int j = 0; j < height; j++){
	for(int i = 0; i < width; i++){
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
	    int gval = stack->frame(m_green,m_view_z)->getPixel(i,j);
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
    for(int j = 0; j < height; j++){
      for(int i = 0; i < width; i++){
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
	  int bval = stack->frame(m_blue,m_view_z)->getPixel(i,j);
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
  for(int j = 0; j < frame->height(); j++){
    for(int i = 0; i < frame->width(); i++){
      int val = frame->getPixel(i,j);
      if(val < m_grayMin) m_grayMin = val;
      if(val > m_grayMax) m_grayMax = val;
    }
  }
  m_grayMinEntry.set_text(boost::lexical_cast<std::string>(m_grayMin));
  m_grayMaxEntry.set_text(boost::lexical_cast<std::string>(m_grayMax));
}

void NiaViewer::autoscaleRGB()
{
  if(m_red < 255){
    m_redMin = 65535;
    m_redMax = 0;
    ImFrame* frame = m_data->fourLocation(m_view_p,m_view_t)->frame(m_red,m_view_z);
    for(int j = 0; j < frame->height(); j++){
      for(int i = 0; i < frame->width(); i++){
	int val = frame->getPixel(i,j);
	if(val < m_redMin) m_redMin = val;
	if(val > m_redMax) m_redMax = val;
      }
    }
  }
  if(m_green < 255){
    m_greenMin = 65535;
    m_greenMax = 0;
    ImFrame* frame = m_data->fourLocation(m_view_p,m_view_t)->frame(m_green,m_view_z);
    for(int j = 0; j < frame->height(); j++){
      for(int i = 0; i < frame->width(); i++){
	int val = frame->getPixel(i,j);
	if(val < m_greenMin) m_greenMin = val;
	if(val > m_greenMax) m_greenMax = val;
      }
    }
  }
  if(m_blue < 255){
    m_blueMin = 65535;
    m_blueMax = 0;
    ImFrame* frame = m_data->fourLocation(m_view_p,m_view_t)->frame(m_blue,m_view_z);
    for(int j = 0; j < frame->height(); j++){
      for(int i = 0; i < frame->width(); i++){
	int val = frame->getPixel(i,j);
	if(val < m_blueMin) m_blueMin = val;
	if(val > m_blueMax) m_blueMax = val;
      }
    }
  }
  m_redMinEntry.set_text(boost::lexical_cast<std::string>(m_redMin));
  m_redMaxEntry.set_text(boost::lexical_cast<std::string>(m_redMax));
  m_greenMinEntry.set_text(boost::lexical_cast<std::string>(m_greenMin));
  m_greenMaxEntry.set_text(boost::lexical_cast<std::string>(m_greenMax));
  m_blueMinEntry.set_text(boost::lexical_cast<std::string>(m_blueMin));
  m_blueMaxEntry.set_text(boost::lexical_cast<std::string>(m_blueMax));
}
