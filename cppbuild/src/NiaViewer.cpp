#include "NiaViewer.hpp"

NiaViewer::NiaViewer() :
  VBox(),
  m_view_w(0), m_view_z(0), m_view_p(0), m_view_t(0), m_centerZ(0.0), m_zwindow(500.0),
  m_red(255), m_green(255), m_blue(255),
  m_grayMin(0), m_grayMax(65535),
  m_redMin(0), m_redMax(65535), m_greenMin(0), m_greenMax(65535), m_blueMin(0), m_blueMax(65535),
  m_mode(GRAY), m_imageType(CONFOCAL), m_analMode(AUTOMATIC),
  m_width(0),m_height(0),m_zoom(1.0),
  m_grayMinLabel("Gray Min."),m_grayMaxLabel("Gray Max."),
  m_redMinLabel("Red Min."),m_redMaxLabel("Red Max."),m_greenMinLabel("Green Min."),m_greenMaxLabel("Green Max."),m_blueMinLabel("Blue Min."),m_blueMaxLabel("Blue Max."),
  m_autoscaleButton("Auto Scale"),
  m_scaleHideButton("Hide"),
  m_alignment(Gtk::ALIGN_CENTER,Gtk::ALIGN_CENTER,0.0,0.0),
  m_prevButton(0)
{
  m_data = NULL;
  m_toolkit = NULL;
  m_currentPunctum = NULL;
  m_colors[0].r = 0xff;
  m_colors[0].g = 0x00;
  m_colors[0].b = 0x00;
  m_colors[1].r = 0x00;
  m_colors[1].g = 0xff;
  m_colors[1].b = 0x00;
  m_colors[2].r = 0x00;
  m_colors[2].g = 0x00;
  m_colors[2].b = 0xff;
  m_colors[3].r = 0xff;
  m_colors[3].g = 0xff;
  m_colors[3].b = 0x00;
  m_colors[4].r = 0xff;
  m_colors[4].g = 0x00;
  m_colors[4].b = 0xff;
  m_colors[5].r = 0x00;
  m_colors[5].g = 0xff;
  m_colors[5].b = 0xff;
  m_pixelSelector = true;
  m_segmentSelector = false;
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
  m_autoscaleButton.signal_clicked().connect(sigc::mem_fun(*this, &NiaViewer::autoscale));
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
  m_scaleBox.pack_start(m_autoscaleButton,Gtk::PACK_SHRINK,15);
  m_scaleBox.pack_start(m_scaleHideButton,Gtk::PACK_SHRINK,15);
  pack_start(m_scaleBox,Gtk::PACK_SHRINK);
  pack_start(m_swin,Gtk::PACK_EXPAND_WIDGET);

  m_swin.add(m_alignment);
  m_alignment.add(m_eventBox);
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
  if(!m_data) return false;
  LocalizedObject::Point thisClick;
  thisClick.x = (int)(evt->x / m_zoom);
  thisClick.y = (int)(evt->y / m_zoom);
  if(evt->type == GDK_BUTTON_PRESS){
    if(m_analMode == MANUAL_ADD){
      ImRecord* rec = currentRecord();
      if(!rec){
	std::cout << "Couldn't find image record" << std::endl;
	return false;
      }
      clearMasks();
      Mask* pMask = rec->getPunctaMask(m_view_w);
      toggleMask(pMask);
      if(evt->button == 1){
	ImFrame* frame = currentFrame();
	ImFrame* dFrame = frame->derivative(m_toolkit->kernelWidth());
	std::vector<LocalizedObject::Point> seeds;
	std::vector<int> borderX,borderY;
	borderX.push_back(thisClick.x);
	borderY.push_back(thisClick.y);
	while(borderX.size() > 0){
	  bool isMinimum = true;
	  int bx = borderX[0]-1;
	  if(bx < 0) bx = 0;
	  int by = borderY[0]-1;
	  if(by < 0) by = 0;
	  int ex = borderX[0]+2;
	  if(ex > frame->width()) ex = frame->width();
	  int ey = borderY[0]+2;
	  if(ey > frame->height()) ey = frame->height();
	  int base = dFrame->getPixel(borderX[0],borderY[0]);
	  for(int x = bx; x < ex; x++){
	    for(int y = by; y < ey; y++){
	      if(dFrame->getPixel(x,y) < base){
		borderX.push_back(x);
		borderY.push_back(y);
		isMinimum = false;
	      }
	    }
	  }
	  if(isMinimum){
	    seeds.push_back(LocalizedObject::Point(borderX[0],borderY[0]));
	  }
	  borderX.erase(borderX.begin());
	  borderY.erase(borderY.begin());
	}
	borderX.push_back(thisClick.x);
	borderY.push_back(thisClick.y);
	while(borderX.size() > 0){
	  bool isMaximum = true;
	  int bx = borderX[0]-1;
	  if(bx < 0) bx = 0;
	  int by = borderY[0]-1;
	  if(by < 0) by = 0;
	  int ex = borderX[0]+2;
	  if(ex > frame->width()) ex = frame->width();
	  int ey = borderY[0]+2;
	  if(ey > frame->height()) ey = frame->height();
	  int base = frame->getPixel(borderX[0],borderY[0]);
	  for(int x = bx; x < ex; x++){
	    for(int y = by; y < ey; y++){
	      if(frame->getPixel(x,y) > base){
		borderX.push_back(x);
		borderY.push_back(y);
		isMaximum = false;
	      }
	    }
	  }
	  if(isMaximum){
	    seeds.push_back(LocalizedObject::Point(borderX[0],borderY[0]));
	  }
	  borderX.erase(borderX.begin());
	  borderY.erase(borderY.begin());
	}
	for(std::vector<LocalizedObject::Point>::iterator pit = seeds.begin(); pit != seeds.end(); pit++){
	  if(pMask->getValue(pit->x,pit->y) > 0) continue;
	  std::vector<int> borderX;
	  std::vector<int> borderY;
	  borderX.push_back(pit->x);
	  borderY.push_back(pit->y);
	  Cluster* c = new Cluster();
	  c->addPoint(pit->x,pit->y,frame->getPixel(pit->x,pit->y));
	  pMask->setValue(pit->x,pit->y,1);
	  while(borderX.size() > 0){
	    int bx = borderX[0]-1;
	    if(bx < 0) bx = 0;
	    int by = borderY[0]-1;
	    if(by < 0) by = 0;
	    int ex = borderX[0]+2;
	    if(ex > frame->width()) ex = frame->width();
	    int ey = borderY[0]+2;
	    if(ey > frame->height()) ey = frame->height();
	    int base = dFrame->getPixel(borderX[0],borderY[0]);
	    for(int x = bx; x < ex; x++){
	      for(int y = by; y < ey; y++){
		if(pMask->getValue(x,y) < 1 && dFrame->getPixel(x,y) > base){
		  c->addPoint(x,y,frame->getPixel(x,y));
		  pMask->setValue(x,y,1);
		  borderX.push_back(x);
		  borderY.push_back(y);
		}
	      }
	    }
	    borderX.erase(borderX.begin());
	    borderY.erase(borderY.begin());
	  }
	  rec->addPunctum(m_view_w,c);
	  updateImage();
	  delete dFrame;
	  return true;
	}
	return false;
      }
      else if(evt->button == 3){
	Cluster* c = rec->selectPunctumStrict(m_view_w,thisClick);
	if(c){
	  for(std::vector<LocalizedObject::Point>::iterator it = c->begin(); it != c->end(); it++) pMask->setValue(it->x,it->y,0);
	  rec->removePunctum(m_view_w,c);
	}
	else return false;
	updateImage();
	return true;
      }
      return false;
    }
    else if(m_analMode == MANUAL_EDIT){
      ImRecord* rec = currentRecord();
      if(!rec){
	std::cout << "Couldn't find image record" << std::endl;
	return false;
      }
      if(evt->button == 1){
	Cluster* c = rec->selectPunctumStrict(m_view_w,thisClick);
	if(c){
	  if(m_currentPunctum != c){
	    if(m_currentPunctum) toggleMask(m_currentPunctum->getMask(m_width,m_height,false));
	    m_currentPunctum = c;
	    toggleMask(m_currentPunctum->getMask(m_width,m_height,false));
	  }
	  return true;
	}
	if(!m_currentPunctum) return false;
	ImFrame* frame = currentFrame();
	toggleMask(m_currentPunctum->getMask(m_width,m_height,false));
	m_currentPunctum->addPoint(thisClick,frame->getPixel(thisClick.x,thisClick.y));
	toggleMask(m_currentPunctum->getMask(m_width,m_height,false));
	return true;
      }
      else if(evt->button == 3){
	if(!m_currentPunctum) return false;
	ImFrame* frame = currentFrame();
	toggleMask(m_currentPunctum->getMask(m_width,m_height,false));
	m_currentPunctum->removePoint(thisClick,frame->getPixel(thisClick.x,thisClick.y));
	toggleMask(m_currentPunctum->getMask(m_width,m_height,false));
	return true;
      }
      return false;
    }
    else if(m_pixelSelector){
      std::cout << "Location (" << thisClick.x << "," << thisClick.y << ") has intensity " << currentFrame()->getPixel(thisClick.x,thisClick.y) << std::endl;
      return true;
    }
    else if(m_segmentSelector){
      ImRecord* rec = currentRecord();
      if(!rec){
	std::cout << "Couldn't find image record" << std::endl;
	return false;
      }
      Segment* s = NULL;
      s = rec->selectSegment(thisClick);
      if(s){
	toggleMask(s->getMask(m_width,m_height,false));
	std::cout << "Center: (" << s->cluster()->center().x << "," << s->cluster()->center().y << "), Size: " << s->cluster()->size() << "\nCircularity: " << s->circularity() << ", Eigenvector 1: " << s->eigenVector1() << ", Eigenvector 2: " << s->eigenVector2() << std::endl;
	return true;
      }
      std::cout << "Couldn't find any segments" << std::endl;
      return false;
    }
    else if(m_punctaSelector){
      ImRecord* rec = currentRecord();
      if(!rec){
	std::cout << "Couldn't find image record" << std::endl;
	return false;
      }
      if(m_imageType == CONFOCAL){
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
      else if(m_imageType == STORM){
	bool retVal = false;
	if(m_mode == GRAY)
	  retVal = rec->selectStormCluster(m_view_w,thisClick.x*200*rec->resolutionXY(),thisClick.y*200*rec->resolutionXY());
	else if(m_mode == RGB)
	  retVal =  rec->selectStormCluster(thisClick.x*200*rec->resolutionXY(),thisClick.y*200*rec->resolutionXY());
	if(retVal) updateImage();
	return retVal;
      }
    }
    else if(m_synapseSelector){
      ImRecord* rec = currentRecord();
      if(!rec) return false;
      if(m_imageType == CONFOCAL){
	Synapse* s = NULL;
	s = rec->selectSynapse(thisClick);
	if(s){
	  toggleMask(s->getMask(m_width,m_height,false));
	  //std::cout << "Center: (" << c->center().x << "," << c->center().y << "), Size: " << c->size() << "\nPeak Intensity: " << c->peak() << ", Integrated Intensity: " << c->integratedIntensity() << std::endl;
	  return true;
	}
	return false;
      }
      else if(m_imageType == STORM){
	if(rec->selectStormSynapse(thisClick.x/5.0,thisClick.y/5.0)){
	  updateImage();
	  return true;
	}
	return false;
      }
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
	    toggleMask(r->getMask(m_width,m_height,rec->nRegions()-1,true));
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
	    removeMask(rec->getRegion(lastReg)->getMask(m_width,m_height,lastReg,true));
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

void NiaViewer::toggleImageType()
{
  if(m_imageType == CONFOCAL){
    m_imageType = STORM;
    ImRecord* rec = currentRecord();
    if(!rec){
      updateImage();
      return;
    }
    if(m_mode == GRAY){
      m_centerZ = 0.0;
      int nClusters = rec->nStormClusters(m_view_w);
      for(int i = 0; i < nClusters; i++){
	StormCluster* sc = rec->stormCluster(m_view_w,i);
	int nMol = sc->nMolecules();
	for(int j = 0; j < nMol; j++){
	  StormData::Blink b = sc->molecule(j);
	  m_centerZ += b.z / nMol;
	}
      }
      m_centerZ = m_centerZ / nClusters;
    }
    else if(m_mode == RGB){
      m_centerZ = 0.0;
      int nClustersRed = 0;
      int nClustersGreen = 0;
      int nClustersBlue = 0;
      if(m_red < 255){
	nClustersRed = rec->nStormClusters(m_red);
	for(int i = 0; i < nClustersRed; i++){
	  StormCluster* sc = rec->stormCluster(m_red,i);
	  int nMol = sc->nMolecules();
	  for(int j = 0; j < nMol; j++){
	    StormData::Blink b = sc->molecule(j);
	    m_centerZ += b.z / nMol;
	  }
	}
      }
      if(m_green < 255){
	nClustersGreen = rec->nStormClusters(m_green);
	for(int i = 0; i < nClustersGreen; i++){
	  StormCluster* sc = rec->stormCluster(m_green,i);
	  int nMol = sc->nMolecules();
	  for(int j = 0; j < nMol; j++){
	    StormData::Blink b = sc->molecule(j);
	    m_centerZ += b.z / nMol;
	  }
	}
      }
      if(m_blue < 255){
	nClustersBlue = rec->nStormClusters(m_blue);
	for(int i = 0; i < nClustersBlue; i++){
	  StormCluster* sc = rec->stormCluster(m_blue,i);
	  int nMol = sc->nMolecules();
	  for(int j = 0; j < nMol; j++){
	    StormData::Blink b = sc->molecule(j);
	    m_centerZ += b.z / nMol;
	  }
	}
      }
      int totalClusters = nClustersRed + nClustersGreen + nClustersBlue;
      if(totalClusters > 0) m_centerZ = m_centerZ / totalClusters;
    }
  }
  else m_imageType = CONFOCAL;
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
    m_colors[3].r = 0xff;
    m_colors[3].g = 0xff;
    m_colors[3].b = 0x00;
    m_colors[4].r = 0xff;
    m_colors[4].g = 0x00;
    m_colors[4].b = 0xff;
    m_colors[5].r = 0x00;
    m_colors[5].g = 0xff;
    m_colors[5].b = 0xff;
  }
  else if(mode == RGB){
    m_colors[0].r = 0xff;
    m_colors[0].g = 0x00;
    m_colors[0].b = 0xff;
    m_colors[1].r = 0x00;
    m_colors[1].g = 0xff;
    m_colors[1].b = 0xff;
    m_colors[2].r = 0xff;
    m_colors[2].g = 0x80;
    m_colors[2].b = 0x00;
    m_colors[3].r = 0x00;
    m_colors[3].g = 0xff;
    m_colors[3].b = 0x80;
    m_colors[4].r = 0x80;
    m_colors[4].g = 0x00;
    m_colors[4].b = 0xff;
    m_colors[5].r = 0xff;
    m_colors[5].g = 0xff;
    m_colors[5].b = 0xff;
  }
  autoscale();
  updateImage();
}

void NiaViewer::setWavelength(int w)
{
  m_mode = GRAY;
  m_colors[0].r = 0xff;
  m_colors[0].g = 0x00;
  m_colors[0].b = 0x00;
  m_colors[1].r = 0x00;
  m_colors[1].g = 0xff;
  m_colors[1].b = 0x00;
  m_colors[2].r = 0x00;
  m_colors[2].g = 0x00;
  m_colors[2].b = 0xff;
  m_colors[3].r = 0xff;
  m_colors[3].g = 0xff;
  m_colors[3].b = 0x00;
  m_colors[4].r = 0xff;
  m_colors[4].g = 0x00;
  m_colors[4].b = 0xff;
  m_colors[5].r = 0x00;
  m_colors[5].g = 0xff;
  m_colors[5].b = 0xff;
  m_view_w = w;
  updateImage();
}

void NiaViewer::prevZ()
{
  m_centerZ -= 50.0;
  if(m_view_z > 0){
    m_view_z--;
  }
  updateImage();
}

void NiaViewer::nextZ()
{
  m_centerZ += 50.0;
  if(m_view_z < m_data->fourLocation(m_view_p,m_view_t)->nz()-1){
    m_view_z++;
  }
  updateImage();
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
  if(m_imageType == STORM){
    m_zoom = 1.0;
    displayStormImage();
    return;
  }
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

void NiaViewer::showDerivative()
{
  if(!m_data) return;
  m_data->fourLocation(m_view_p,m_view_t)->derivative();
  autoscale();
  updateImage();
}

void NiaViewer::showDerivative2()
{
  if(!m_data) return;
  m_data->fourLocation(m_view_p,m_view_t)->d2EigenvectorMax();
  autoscale();
  updateImage();
}

void NiaViewer::showStats()
{
  if(!m_data) return;
  ImFrame* frame = m_data->fourLocation(m_view_p,m_view_t)->frame(m_view_w,m_view_z);
  int min = frame->min();
  if(m_masks.size() > 0 && m_masks[0] != NULL){
    Mask* m = m_masks[0];
    std::cout << "mode: " << frame->mode(min,m) << "\nmedian: " << frame->median(min,m) << "\nmean: " << frame->mean(m) << "\nstd: " << frame->std(m) << std::endl;
  }
  std::cout << "mode: " << frame->mode(min) << "\nmedian: " << frame->median(min) << "\nmean: " << frame->mean() << "\nstd: " << frame->std() << std::endl;
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
      if(val > 0){
	Color col = m_colors[val%m_ncolors];
	buf[index] = col.r;
	buf[index+1] = col.g;
	buf[index+2] = col.b;
      }
      //buf[index+(val%3)] = scaled_p;
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

  Mask* m = new Mask(m_width,m_height);
  ImFrame* dFrame = m_data->fourLocation(m_view_p,m_view_t)->frame(m_view_w,m_view_z)->derivative();
  Mask* pMask = rec->getPunctaMask(m_view_w);
  int dx[8] = {-1,-1,-1,0,0,1,1,1};
  int dy[8] = {-1,0,1,-1,1,-1,0,1};
  for(int i = 1; i < m_width-1; i++){
    for(int j = 1; j < m_height-1; j++){
      if(pMask->getValue(i,j) < 1) continue;
      int stepX = 1;
      int stepY = 1;
      int x = i;
      int y = j;
      int minD = dFrame->getPixel(i,j);
      while(stepX != 0 || stepY != 0){
	stepX = 0;
	stepY = 0;
	for(int k = 0; k < 8; k++){
	  if(x+dx[k] < 0 || x+dx[k] >= m_width) continue;
	  if(y+dy[k] < 0 || y+dy[k] >= m_height) continue;
	  int val = dFrame->getPixel(x+dx[k],y+dy[k]);
	  if(val < minD){
	    minD = val;
	    stepX = dx[k];
	    stepY = dy[k];
	  }
	}
	x += stepX;
	y += stepY;
      }
      m->setValue(x,y,1);
    }
  }
  delete dFrame;
  delete pMask;
  toggleMask(m);
	
  //displayMask(rec->getContourMap(m_view_w));
  //displayMask(rec->segment(m_view_w));
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
      if(m != *it) delete *it;
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
  if(m_records.at(index)){
    if(m_mode == RGB){
      if(m_red < 255 && m_red >= 0) toggleMask(m_records.at(index)->getPunctaMask(m_red,false));
      if(m_green < 255 && m_green >= 0) toggleMask(m_records.at(index)->getPunctaMask(m_green,false));
      if(m_blue < 255 && m_blue >= 0) toggleMask(m_records.at(index)->getPunctaMask(m_blue,false));
    }
    else toggleMask(m_records.at(index)->getPunctaMask(m_view_w,false));
  }
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
  ImRecord* rec = m_records[index];
  if(rec){
    for(int i = 0; i < rec->nRegions(); i++) toggleMask(rec->getRegion(i)->getMask(rec->imWidth(),rec->imHeight(),i,true));
  }
}

void NiaViewer::toggleSegmentMask()
{
  if(!m_data) return;
  int nz = m_data->fourLocation(m_view_p,m_view_t)->nz();
  int index = m_view_p*m_data->nt()*nz + m_view_t*nz + m_view_z;
  ImRecord* rec = m_records[index];
  if(rec){
    for(int i = 0; i < rec->nSegments(); i++) toggleMask(rec->getSegment(i)->getMask(rec->imWidth(),rec->imHeight(),true,i));
  }
}

void NiaViewer::toggleStormMask()
{
  if(!m_data) return;
  int nz = m_data->fourLocation(m_view_p,m_view_t)->nz();
  int index = m_view_p*m_data->nt()*nz + m_view_t*nz + m_view_z;
  if(m_records.at(index)) toggleMask(m_records.at(index)->getStormClusterLocations(m_view_w));
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

void NiaViewer::alignStormData()
{
  if(!m_data) return;
  ImFrame* frame = currentFrame();
  ImRecord* rec = currentRecord();
  if(!rec) return;
  std::vector<LocalizedObject::Point> cents = rec->getStormClusterCenters(m_view_w);
  int maxI = 0;
  double minScale = 1.12;
  double maxScale = 1.2;
  double step = 0.1;
  double bestScale = 1.0;
  int bestX = 0;
  int bestY = 0;
  for(double scale = minScale; scale < maxScale; scale += step){
    int minShiftX = -frame->width();
    int maxShiftX = frame->width();
    int minShiftY = -frame->height();
    int maxShiftY = frame->height();
    int stepX = (maxShiftX - minShiftX) / 10;
    int stepY = (maxShiftY - minShiftY) / 10;
    while(maxShiftX - minShiftX > 2 && maxShiftY - minShiftY > 2){
      for(int shiftX = minShiftX; shiftX < maxShiftX; shiftX += stepX){
	for(int shiftY = minShiftY; shiftY < maxShiftY; shiftY += stepY){
	  int I = 0;
	  int bx = -shiftX;
	  int ex = frame->width()-1;
	  if(bx < 0){
	    bx = 0;
	    ex -= shiftX;
	  }
	  int by = -shiftY;
	  int ey = frame->height()-1;
	  if(by < 0){
	    by = 0;
	    ey -= shiftY;
	  }
	  for(std::vector<LocalizedObject::Point>::iterator pit = cents.begin(); pit != cents.end(); pit++){
	    int yprime = pit->y*scale;
	    if(pit->x < bx || pit->x > ex || yprime < by || yprime > ey) continue;
	    I += frame->getPixel(pit->x + shiftX, yprime + shiftY);
	  }
	  if(I > maxI){
	    maxI = I;
	    bestX = shiftX;
	    bestY = shiftY;
	    bestScale = scale;
	  }
	}
      }
      minShiftX = bestX - stepX;
      maxShiftX = bestX + stepX;
      minShiftY = bestY - stepY;
      maxShiftY = bestY + stepY;
      stepX = (maxShiftX - minShiftX) / 10;
      if(stepX < 1) stepX = 1;
      stepY = (maxShiftY - minShiftY) / 10;
      if(stepY < 1) stepY = 1;
    }
  }
  std::cout << bestScale << ", " << bestX << ", " << bestY << std::endl;
  rec->shiftStormData(bestX,bestY,bestScale);
}

void NiaViewer::shareRegionsZ()
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

void NiaViewer::shareRegionsT()
{
  ImRecord* rec = currentRecord();
  if(!rec) return;
  int nt = m_data->nt();
  int nz = m_data->fourLocation(m_view_p,m_view_t)->nz();
  for(int t = 0; t < nt; t++){
    if(t == m_view_t) continue;
    int index = m_view_p*nt*nz + t*nz + m_view_z;
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

void NiaViewer::convertRegions()
{
  ImRecord* rec = currentRecord();
  if(!rec) return;
  rec->convertRegionsToSegments(m_view_w);
  toggleSegmentMask();
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

void NiaViewer::saveTimeSeries(std::string basename)
{
  if(!m_data) return;
  for(int t = 0; t < m_data->nt(); t++){
    std::string filename = basename;
    filename.append("_t");
    filename.append(boost::lexical_cast<std::string>(t));
    filename.append(".png");
    m_view_t = t;
    updateImage();
    m_pixbuf->save(filename,"png");
  }
}

void NiaViewer::displayStormImage()
{
  uint8_t* buf = m_pixbuf->get_pixels();
  if(m_mode == GRAY){
    m_pixbuf = createPixbufStorm(m_view_w);
    if(buf) delete[] buf;
  }
  else if(m_mode == RGB){
    m_pixbuf = createPixbufStorm();
    if(buf) delete[] buf;
  }
  int w = m_width*5;
  int h = m_height*5;
  Glib::RefPtr<Gdk::Pixbuf> pb = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB,false,8,w*m_zoom,h*m_zoom);
  m_pixbuf->scale(pb,0,0,w*m_zoom,h*m_zoom,0,0,m_zoom,m_zoom,Gdk::INTERP_TILES);
  m_displayImage.set(pb);
  m_eventBox.hide();
  m_eventBox.set_size_request(w*m_zoom,h*m_zoom);
  m_eventBox.show();
}

Glib::RefPtr<Gdk::Pixbuf> NiaViewer::createPixbufStorm(int chan)
{
  ImRecord* rec = currentRecord();
  int nClusters = rec->nStormClusters(chan);
  double resolution = 1000.0 * rec->resolutionXY() / 5.0;
  int w = 5*m_width;
  int h = 5*m_height;
  uint8_t* buf = new uint8_t[w*h*3];
  //double sf = 255.0 / (m_grayMax - m_grayMin);
  for(int i = 0; i < w*h*3; i++){
      buf[i] = 0;
  }
  for(int i = 0; i < nClusters; i++){
    StormCluster* sc = rec->stormCluster(chan,i);
    if(sc->isSelected()){
      for(int j = 0; j < sc->nMolecules(); j++){
	StormData::Blink b = sc->molecule(j);
	if(fabs(b.z - m_centerZ) > m_zwindow) continue;
	int x = (int)(b.x / resolution);
	if(x < 0 || x >= w) continue;
	int y = (int)(b.y / resolution);
	if(y < 0 || y >= h) continue;
	int index = 3*w*y + 3*x;
	unsigned colorIndex = i % m_ncolors;
	buf[index] = m_colors[colorIndex].r;
	buf[index+1] = m_colors[colorIndex].g;
	buf[index+2] = m_colors[colorIndex].b;
      }
    }
    else{
      for(int j = 0; j < sc->nMolecules(); j++){
	StormData::Blink b = sc->molecule(j);
	if(fabs(b.z - m_centerZ) > m_zwindow) continue;
	int x = (int)(b.x / resolution);
	if(x < 0 || x >= w) continue;
	int y = (int)(b.y / resolution);
	if(y < 0 || y >= h) continue;
	int index = 3*w*y + 3*x;
	buf[index] = 255;
	buf[index+1] = 255;
	buf[index+2] = 255;
      }
    }
  }
      
  Glib::RefPtr<Gdk::Pixbuf> retval = Gdk::Pixbuf::create_from_data(buf, Gdk::COLORSPACE_RGB, false, 8, w, h, w*3);
  return retval;
}

Glib::RefPtr<Gdk::Pixbuf> NiaViewer::createPixbufStorm()
{
  ImRecord* rec = currentRecord();
  double resolution = 1000.0 * rec->resolutionXY() / 5.0;
  int w = 5*m_width;
  int h = 5*m_height;
  uint8_t* buf = new uint8_t[w*h*3];
  //double sf = 255.0 / (m_grayMax - m_grayMin);
  for(int i = 0; i < w*h*3; i++){
      buf[i] = 0;
  }
  if(m_red >= 0 && m_red < 255){
    int nClusters = rec->nStormClusters(m_red);
    uint8_t rval = 255;
    uint8_t gval,bval;
    for(int i = 0; i < nClusters; i++){
      StormCluster* sc = rec->stormCluster(m_red,i);
      if(sc->isSelected()){
	gval = 255;
	bval = 255;
      }
      else{
	gval = 0;
	bval = 0;
      }
      for(int j = 0; j < sc->nMolecules(); j++){
	StormData::Blink b = sc->molecule(j);
	if(fabs(b.z - m_centerZ) > m_zwindow) continue;
	int x = (int)(b.x / resolution);
	if(x < 0 || x >= w) continue;
	int y = (int)(b.y / resolution);
	if(y < 0 || y >= h) continue;
	int index = 3*w*y + 3*x;
	buf[index] = buf[index] | rval;
	buf[index+1] = buf[index+1] | gval;
	buf[index+2] = buf[index+2] | bval;
      }
    }
  }
  if(m_green >= 0 && m_green < 255){
    int nClusters = rec->nStormClusters(m_green);
    uint8_t gval = 255;
    uint8_t rval,bval;
    for(int i = 0; i < nClusters; i++){
      StormCluster* sc = rec->stormCluster(m_green,i);
      if(sc->isSelected()){
	rval = 255;
	bval = 255;
      }
      else{
	rval = 0;
	bval = 0;
      }
      for(int j = 0; j < sc->nMolecules(); j++){
	StormData::Blink b = sc->molecule(j);
	if(fabs(b.z - m_centerZ) > m_zwindow) continue;
	int x = (int)(b.x / resolution);
	if(x < 0 || x >= w) continue;
	int y = (int)(b.y / resolution);
	if(y < 0 || y >= h) continue;
	int index = 3*w*y + 3*x;
	buf[index] = buf[index] | rval;
	buf[index+1] = buf[index+1] | gval;
	buf[index+2] = buf[index+2] | bval;
      }
    }
  }
  if(m_blue >= 0 && m_blue < 255){
    int nClusters = rec->nStormClusters(m_blue);
    uint8_t bval = 255;
    uint8_t gval,rval;
    for(int i = 0; i < nClusters; i++){
      StormCluster* sc = rec->stormCluster(m_blue,i);
      if(sc->isSelected()){
	gval = 255;
	rval = 255;
      }
      else{
	gval = 0;
	rval = 0;
      }
      for(int j = 0; j < sc->nMolecules(); j++){
	StormData::Blink b = sc->molecule(j);
	if(fabs(b.z - m_centerZ) > m_zwindow) continue;
	int x = (int)(b.x / resolution);
	if(x < 0 || x >= w) continue;
	int y = (int)(b.y / resolution);
	if(y < 0 || y >= h) continue;
	int index = 3*w*y + 3*x;
	buf[index] = buf[index] | rval;
	buf[index+1] = buf[index+1] | gval;
	buf[index+2] = buf[index+2] | bval;
      }
    }
  }
      
  Glib::RefPtr<Gdk::Pixbuf> retval = Gdk::Pixbuf::create_from_data(buf, Gdk::COLORSPACE_RGB, false, 8, w, h, w*3);
  return retval;
}

void NiaViewer::setZWindow()
{
  Gtk::Entry entry;
  entry.set_max_length(15);
  entry.set_width_chars(10);
  entry.set_text(boost::lexical_cast<std::string>(m_zwindow));
  Gtk::MessageDialog md("Select z window size");
  md.get_message_area()->pack_start(entry,Gtk::PACK_SHRINK);
  entry.show();
  md.run();
  m_zwindow = boost::lexical_cast<double>(entry.get_text());
}

void NiaViewer::manualStormAlignment()
{
  ImRecord* rec = currentRecord();
  if(!rec) return;
  Gtk::Entry scale_entry,x_entry,y_entry;
  Gtk::HBox hbox;
  Gtk::VBox vbox1,vbox2,vbox3;
  Gtk::Label scale_label("Y scale:");
  Gtk::Label x_label("X shift:");
  Gtk::Label y_label("Y shift:");
  scale_entry.set_max_length(15);
  scale_entry.set_width_chars(10);
  scale_entry.set_text("1.0");
  x_entry.set_max_length(15);
  x_entry.set_width_chars(10);
  x_entry.set_text("0");
  y_entry.set_max_length(15);
  y_entry.set_width_chars(10);
  y_entry.set_text("0");
  vbox1.pack_start(scale_label,Gtk::PACK_SHRINK);
  vbox1.pack_start(scale_entry,Gtk::PACK_SHRINK);
  vbox2.pack_start(x_label,Gtk::PACK_SHRINK);
  vbox2.pack_start(x_entry,Gtk::PACK_SHRINK);
  vbox3.pack_start(y_label,Gtk::PACK_SHRINK);
  vbox3.pack_start(y_entry,Gtk::PACK_SHRINK);
  hbox.pack_start(vbox1,Gtk::PACK_SHRINK);
  hbox.pack_start(vbox2,Gtk::PACK_SHRINK);
  hbox.pack_start(vbox3,Gtk::PACK_SHRINK);
  Gtk::MessageDialog md("Enter manual STORM alignment");
  md.get_message_area()->pack_start(hbox,Gtk::PACK_SHRINK);
  md.show_all_children();
  md.run();
  double scale = boost::lexical_cast<double>(scale_entry.get_text());
  int xshift = boost::lexical_cast<int>(x_entry.get_text());
  int yshift = boost::lexical_cast<int>(y_entry.get_text());
  std::cout << scale << ", " << xshift << ", " << yshift << std::endl;
  rec->shiftStormData(xshift,yshift,scale);
}
