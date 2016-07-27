#include "ImageAnalysisToolkit.hpp"

ImageAnalysisToolkit::ImageAnalysisToolkit()
{
  m_master = 255;
  m_mode = OVERRIDE;
  m_punctaFindingIterations = 10;
  m_saturationThreshold = 4094;
  m_localWindow.push_back(5.0);
  m_localWindowIncrement.push_back(0.5);
  m_minPunctaRadius.push_back(0.1);
  m_reclusterThreshold.push_back(3.0);
  m_noiseRemovalThreshold.push_back(2.0);
  m_peakThreshold.push_back(3.0);
  m_floorThreshold.push_back(2.0);
}

ImageAnalysisToolkit::~ImageAnalysisToolkit()
{
  for(std::vector<SynapseCollection*>::iterator it = m_synapseDefinitions.begin(); it != m_synapseDefinitions.end(); it++){
    if(*it) delete *it;
  }
}

void ImageAnalysisToolkit::makeSeparateConfigs(uint8_t nchan)
{
  for(uint8_t i = 1; i < nchan; i++){
    m_localWindow.push_back(m_localWindow.at(0));
    m_localWindowIncrement.push_back(m_localWindowIncrement.at(0));
    m_minPunctaRadius.push_back(m_minPunctaRadius.at(0));
    m_reclusterThreshold.push_back(m_reclusterThreshold.at(0));
    m_noiseRemovalThreshold.push_back(m_noiseRemovalThreshold.at(0));
    m_peakThreshold.push_back(m_peakThreshold.at(0));
    m_floorThreshold.push_back(m_floorThreshold.at(0));
  }
}

void ImageAnalysisToolkit::makeSingleConfig()
{
  uint8_t nchan = m_localWindow.size();
  for(uint8_t i = 1; i < nchan; i++){
    m_localWindow.erase(m_localWindow.begin()+1);
    m_localWindowIncrement.erase(m_localWindowIncrement.begin()+1);
    m_minPunctaRadius.erase(m_minPunctaRadius.begin()+1);
    m_reclusterThreshold.erase(m_reclusterThreshold.begin()+1);
    m_noiseRemovalThreshold.erase(m_noiseRemovalThreshold.begin()+1);
    m_peakThreshold.erase(m_peakThreshold.begin()+1);
    m_floorThreshold.erase(m_floorThreshold.begin()+1);
  }
}

void ImageAnalysisToolkit::standardAnalysis(ImStack* stack, ImRecord* rec, int arg_zplane)
{
  ImStack* analysisStack = stack;
  int zplane = arg_zplane;
  if(zplane < 0){
    if(stack->nz() > 1) analysisStack = stack->zprojection();
    zplane = 0;
  }

  if(m_master == 255){
    for(uint8_t w = 0; w < analysisStack->nwaves(); w++) findSignal(analysisStack->frame(w,zplane),rec,w);
  }
  else{
    findSignal(analysisStack->frame(m_master,zplane),rec,m_master);
    Mask* m = rec->getSignalMask(m_master);
    if(m_mode == OVERRIDE){
      for(uint8_t w = 0; w < analysisStack->nwaves(); w++){
	if(w == m_master) continue;
	ImFrame* f = analysisStack->frame(w,zplane);
	double mean = f->mean(m);
	double std = f->std(m);
	double threshold = mean + std;
	Mask* m2 = m->getCopy();
	for(uint16_t i = 0; i < f->width(); i++){
	  for(uint16_t j = 0; j < f->height(); j++){
	    if(f->getPixel(i,j) > threshold) m2->setValue(i,j,1);
	  }
	}
	rec->setSignalMask(w,m2);
      }
    }
    else{ //m_mode == OR
      for(uint8_t w = 0; w < analysisStack->nwaves(); w++){
	if(w == m_master) continue;
	findSignal(analysisStack->frame(w,zplane),rec,w);
	Mask* m2 = rec->getSignalMask(w);
	m2->OR(*m);
      }
    }
  }
  nia::nout << "Done signal finding" << "\n";
  
  for(uint8_t w = 0; w < analysisStack->nwaves(); w++){
    findPuncta(analysisStack->frame(w,zplane),rec,w);
  }

  findSynapses(rec);
  
}

void ImageAnalysisToolkit::findSignal(ImFrame* frame, ImRecord* rec, uint8_t chan)
{
  uint16_t globalThreshold = findThreshold(frame);
  rec->setThreshold(chan,globalThreshold);
  Mask* m = new Mask(frame->width(),frame->height());
  for(uint16_t i = 0; i < frame->width(); i++){
    for(uint16_t j = 0; j < frame->height(); j++){
      if(frame->getPixel(i,j) > globalThreshold){
	m->setValue(i,j,1);
      }
    }
  }
  bool unfilled = true;
  while(unfilled){
    unfilled = false;
    for(uint16_t i = 1; i < frame->width()-1; i++){
      for(uint16_t j = 1; j < frame->height()-1; j++){
	if(m->getValue(i,j) > 0) continue;
	uint8_t sum = m->getValue(i-1,j-1) + m->getValue(i,j-1) + m->getValue(i+1,j-1) + m->getValue(i-1,j) + m->getValue(i+1,j) + m->getValue(i-1,j+1) + m->getValue(i,j+1) + m->getValue(i+1,j+1);
	if(sum > 4){
	  m->setValue(i,j,1);
	  unfilled = true;
	}
      }
    }
  }
  bool unpruned = true;
  while(unpruned){
    unpruned = false;
    for(uint16_t i = 1; i < frame->width()-1; i++){
      for(uint16_t j = 1; j < frame->height()-1; j++){
	if(m->getValue(i,j) < 1) continue;
	uint8_t sum = m->getValue(i-1,j-1) + m->getValue(i,j-1) + m->getValue(i+1,j-1) + m->getValue(i-1,j) + m->getValue(i+1,j) + m->getValue(i-1,j+1) + m->getValue(i,j+1) + m->getValue(i+1,j+1);
	if(sum < 3){
	  m->setValue(i,j,0);
	  unpruned = true;
	}
      }
    }
  }
  uint8_t configChan = chan;
  if(configChan >= m_noiseRemovalThreshold.size()) configChan = 0;
  double radius = m_minPunctaRadius.at(configChan);
  uint32_t sizeThreshold = (uint32_t)(m_noiseRemovalThreshold.at(configChan)*3.14159*radius*radius/(rec->resolutionXY()*rec->resolutionXY()));
  std::vector<uint16_t> borderX;
  std::vector<uint16_t> borderY;
  std::vector<uint16_t> clusterX;
  std::vector<uint16_t> clusterY;
  Mask* used = m->getCopy();
  for(uint16_t i = 0; i < frame->width(); i++){
    for(uint16_t j = 0; j < frame->height(); j++){
      if(used->getValue(i,j) != 1) continue;
      clusterX.push_back(i);
      clusterY.push_back(j);
      borderX.push_back(i);
      borderY.push_back(j);
      used->setValue(i,j,0);
      uint32_t size = 1;
      while(borderX.size() > 0){
	int bi = borderX.at(0);
	int bj = borderY.at(0);
	int left = bi-1;
	int right = bi+2;
	int top = bj-1;
	int bottom = bj+2;
	if(left < 0) left++;
	if(right >= frame->width()) right--;
	if(top < 0) top++;
	if(bottom >= frame->height()) bottom--;
	for(int di = left; di < right; di++){
	  for(int dj = top; dj < bottom; dj++){
	    uint8_t val = used->getValue(di,dj);
	    if(val > 0){
	      used->setValue(di,dj,0);
	      clusterX.push_back(di);
	      clusterY.push_back(dj);
	      borderX.push_back(di);
	      borderY.push_back(dj);
	      size++;
	    }
	  }
	}
	borderX.erase(borderX.begin());
	borderY.erase(borderY.begin());
      }
      if(size < sizeThreshold){
	for(int k = 0; k < size; k++){
	  m->setValue(clusterX.at(k),clusterY.at(k),0);
	}
      }
    }
  }
  rec->setSignalMask(chan,m);
  delete used;
}

uint16_t ImageAnalysisToolkit::findThreshold(ImFrame* frame)
{
  Mask* m = new Mask(frame->width(),frame->height());
  double lowerLimit = frame->mode();
  double mode = lowerLimit;
  double best = frame->mean();
  double upperLimit = 2*best;//best + 6*ndim.std(w,z,t,p);
  int nsteps = 10;
  double step = (upperLimit - lowerLimit) / nsteps;
  if(step < 1.0){
    step = 1.0;
    nsteps = (int)(upperLimit - lowerLimit);
  }
  double fom = 0;
  int maxClusters = 0;
  bool finished = false;
  while(!finished){
    finished = true;
    for(int istep = 0; istep < nsteps+1; istep++){
      double globalThreshold = lowerLimit + istep*step;
      for(uint16_t i = 0; i < frame->width(); i++){
	for(uint16_t j = 0; j < frame->height(); j++){
	  uint16_t value = frame->getPixel(i,j);
	  if(value > globalThreshold){
	    m->setValue(i,j,1);
	  }
	  else m->setValue(i,j,0);
	}
      }
      bool unpruned = true;
      while(unpruned){
	unpruned = false;
	for(uint16_t i = 1; i < frame->width()-1; i++){
	  for(uint16_t j = 1; j < frame->height()-1; j++){
	    if(m->getValue(i,j) < 1) continue;
	    uint8_t sum = m->getValue(i-1,j-1) + m->getValue(i,j-1) + m->getValue(i+1,j-1) + m->getValue(i-1,j) + m->getValue(i+1,j) + m->getValue(i-1,j+1) + m->getValue(i,j+1) + m->getValue(i+1,j+1);
	    if(sum < 3){
	      m->setValue(i,j,0);
	      unpruned = true;
	    }
	  }
	}
      }
      int maxSize = 0;
      int64_t avgSigSize = 0;
      int64_t avgSigSize2 = 0;
      int nSigClusters = 0;
      std::vector<uint16_t> borderX;
      std::vector<uint16_t> borderY;
      Mask* subMask = m->getCopy();
      for(uint16_t i = 0; i < frame->width(); i++){
	for(uint16_t j = 0; j < frame->height(); j++){
	  if(subMask->getValue(i,j) != 1) continue;
	  borderX.push_back(i);
	  borderY.push_back(j);
	  subMask->setValue(i,j,0);
	  int64_t size = 1;
	  while(borderX.size() > 0){
	    int bi = borderX.at(0);
	    int bj = borderY.at(0);
	    int left = bi-1;
	    int right = bi+2;
	    int top = bj-1;
	    int bottom = bj+2;
	    if(left < 0) left++;
	    if(right >= frame->width()) right--;
	    if(top < 0) top++;
	    if(bottom >= frame->height()) bottom--;
	    for(int di = left; di < right; di++){
	      for(int dj = top; dj < bottom; dj++){
		uint16_t val = m->getValue(di,dj);
		if(val > 0 && subMask->getValue(di,dj) == 1){
		  subMask->setValue(di,dj,0);
		  borderX.push_back(di);
		  borderY.push_back(dj);
		  size++;
		}
	      }
	    }
	    borderX.erase(borderX.begin());
	    borderY.erase(borderY.begin());
	  }
	  if(size > maxSize){
	    maxSize = (int)size;
	    //  maxCluster = c;
	  }
	  avgSigSize += size;
	  avgSigSize2 += size*size;
	  nSigClusters++;
	}
      }

      int maxBkgSize = 0;
      int avgSize = 0;
      int nBkgClusters = 0;
      subMask->copy(*m);
      for(uint16_t i = 0; i < frame->width(); i++){
	for(uint16_t j = 0; j < frame->height(); j++){
	  if(subMask->getValue(i,j) != 0) continue;
	  borderX.push_back(i);
	  borderY.push_back(j);
	  subMask->setValue(i,j,1);
	  int size = 1;
	  while(borderX.size() > 0){
	    int bi = borderX.at(0);
	    int bj = borderY.at(0);
	    int left = bi-1;
	    int right = bi+2;
	    int top = bj-1;
	    int bottom = bj+2;
	    if(left < 0) left++;
	    if(right >= frame->width()) right--;
	    if(top < 0) top++;
	    if(bottom >= frame->height()) bottom--;
	    for(int di = left; di < right; di++){
	      for(int dj = top; dj < bottom; dj++){
		int val = m->getValue(di,dj);
		if(val < 1 && subMask->getValue(di,dj) == 0){
		  subMask->setValue(di,dj,1);
		  borderX.push_back(di);
		  borderY.push_back(dj);
		  size++;
		}
	      }
	    }
	    borderX.erase(borderX.begin());
	    borderY.erase(borderY.begin());
	  }
	  if(size > maxBkgSize){
	    maxBkgSize = size;
	  }
	  avgSize += size;
	  nBkgClusters++;
	}
      }

      
      double ifom = ((double)avgSigSize2)*avgSize/avgSigSize;
      if(((double)avgSigSize)/(frame->height()*frame->width()) < 0.01) break;
      if(nBkgClusters > 0) ifom = ifom/nBkgClusters;
      else ifom = 0;
      if(nSigClusters >= maxClusters){
	fom = ifom;
	maxClusters = nSigClusters;
	best = globalThreshold;
	finished = false;
      }
      else if(ifom > fom){
	fom = ifom;
	best = globalThreshold;
	finished = false;
      }
      delete subMask;
      //nia::nout << globalThreshold << ", " << avgSigSize2 << ", " << avgSigSize << ", " << nSigClusters << ", " << avgSize << ", " << nBkgClusters << ":\t" << ifom << "\n";
    }
    if(step < 1.01) break;
    if(best < 0) break;
    if(best == upperLimit){
      lowerLimit = best - step;
      upperLimit = best + 9*step;
      fom *= 0.999;
    }
    else if(best == lowerLimit){
      lowerLimit = best - 9*step;
      upperLimit = best + step;
      fom *= 0.999;
    }
    else{
      lowerLimit = best - step;
      upperLimit = best + step;
      step = (upperLimit - lowerLimit) / nsteps;
      if(step < 1.0){
	step = 1.0;
	nsteps = (int)(upperLimit - lowerLimit);
      }
    }
  }
  delete m;
  //nia::nout << "Best threshold: " << best << "\n";
  return (uint16_t)best;
}

void ImageAnalysisToolkit::findPuncta(ImFrame* frame, ImRecord* rec, uint8_t chan)
{
  findSaturatedPuncta(frame,rec,chan);
  
  Mask* m = rec->getSignalMask(chan);
  std::vector<LocalizedObject::Point> localMaxima;
  std::vector<LocalizedObject::Point> upperLeft;
  std::vector<LocalizedObject::Point> lowerRight;
  std::vector<uint16_t> intensities;
  uint8_t configChan = chan;
  if(configChan >= m_localWindow.size()) configChan = 0;
  int minSignal = (int)std::min(m_localWindow.at(configChan) / (rec->resolutionXY()*rec->resolutionXY()),(double)m->sum());
  int initialWindowSize = (int)(sqrt(minSignal) / 2);
  int lwi = (int)(m_localWindowIncrement.at(configChan) / rec->resolutionXY());
  for(uint16_t i = 0; i < frame->width(); i++){
    for(uint16_t j = 0; j < frame->height(); j++){
      if(m->getValue(i,j) < 1){
	continue;
      }
      uint16_t x1 = std::max(i-lwi,0);
      uint16_t x2 = std::min((int)frame->width(),i+lwi);
      uint16_t y1 = std::max(j-lwi,0);
      uint16_t y2 = std::min((int)frame->height(),j+lwi);
      
      uint16_t val = frame->getPixel(i,j);
      bool isMax = true;
      for(int di = i-1; di < i+2; di++){
	if(di < 0 || di >= frame->width()) continue;
	for(int dj = j-1; dj < j+2; dj++){
	  if(dj < 0 || dj >= frame->height()) continue;
	  if(frame->getPixel(di,dj) > val){
	    isMax = false;
	    break;
	  }
	}
      }
      if(!isMax) continue;
		
      localMaxima.push_back(LocalizedObject::Point(i,j));
      intensities.push_back(val);
      //int x1 = Math.std::max(i-initialWindowSize,0);
      //int x2 = Math.std::min(ndim.getWidth(),i+initialWindowSize);
      //int y1 = Math.std::max(j-initialWindowSize,0);
      //int y2 = Math.std::min(ndim.getHeight(),j+initialWindowSize);
      int nSignal = m->sum(x1,x2,y1,y2);
      while(nSignal < minSignal){
	//int step = (int)(Math.sqrt(minSignal/nSignal - 1) * initialWindowSize) + 1;
	x1 = std::max(x1-lwi,0);
	x2 = std::min((int)frame->width(),x2+lwi);
	y1 = std::max(y1-lwi,0);
	y2 = std::min((int)frame->height(),y2+lwi);
	//x1 = Math.std::max(x1-step,0);
	//x2 = Math.std::min(ndim.getWidth(),x2+step);
	//y1 = Math.std::max(y1-step,0);
	//y2 = Math.std::min(ndim.getHeight(),y2+step);
	nSignal = m->sum(x1,x2,y1,y2);
      }
      upperLeft.push_back(LocalizedObject::Point(x1,y1));
      lowerRight.push_back(LocalizedObject::Point(x2,y2));
    }
  }
  for(uint32_t i = 0; i < intensities.size(); i++){
    uint16_t max = intensities.at(i);
    uint16_t imax = i;
    for(uint32_t j = i+1; j < intensities.size(); j++){
      if(intensities.at(j) > max){
	max = intensities.at(j);
	imax = j;
      }
    }
    intensities.at(imax) = intensities.at(i);
    intensities.at(i) = max;
    LocalizedObject::Point temp = localMaxima.at(i);
    localMaxima.at(i) = localMaxima.at(imax);
    localMaxima.at(imax) = temp;
    temp = upperLeft.at(i);
    upperLeft.at(i) = upperLeft.at(imax);
    upperLeft.at(imax) = temp;
    temp = lowerRight.at(i);
    lowerRight.at(i) = lowerRight.at(imax);
    lowerRight.at(imax) = temp;
  }

  int32_t nRemoved = -1;
  int32_t nNew = 0;
  uint8_t count = 0;
  Mask* cMask = new Mask(frame->width(),frame->height());
  Mask* used = m->getCopy();
  used->subtract(*(rec->getPunctaMask(chan,false)));
  double radius = m_minPunctaRadius.at(configChan);
  double punctaAreaThreshold = 3.14159*radius*radius/(rec->resolutionXY()*rec->resolutionXY());
  uint32_t retryThreshold = (uint32_t)(m_reclusterThreshold.at(configChan)*punctaAreaThreshold) + 1;
  while(nNew > nRemoved && count < m_punctaFindingIterations){
    //nNew = 0;
    nRemoved = 0;
    int32_t nPuncta = (int32_t)rec->nPuncta(chan);
    for(int32_t j = nPuncta-1; j >= nPuncta-nNew; j--){
      Cluster* c = rec->getPunctum(chan,j);
      if(c->size() < retryThreshold){
	std::vector<LocalizedObject::Point> pts = c->getPoints();
	for(std::vector<LocalizedObject::Point>::iterator it = pts.begin(); it != pts.end(); it++){
	  used->setValue(it->x,it->y,1);
	}
	rec->removePunctum(chan,j);
	nRemoved++;
      }
    }

    nNew = 0;
    for(uint32_t s = 0; s < localMaxima.size(); s++){
      LocalizedObject::Point lm = localMaxima.at(s);
      if(used->getValue(lm.x,lm.y) == 0) continue;
      LocalizedObject::Point ul = upperLeft.at(s);
      LocalizedObject::Point lr = lowerRight.at(s);
      uint16_t Imax = frame->getPixel(lm.x,lm.y);
      double localMedian;
      double localStd;
      frame->getMedianStd(ul.x,lr.x,ul.y,lr.y,used,localMedian,localStd);
      double minThreshold = std::min(localMedian + (Imax - localMedian)/2, localMedian + m_floorThreshold.at(configChan)*localStd);
      double localThreshold = localMedian + m_peakThreshold.at(configChan)*localStd;
      double minIntensity = punctaAreaThreshold*(localMedian + m_floorThreshold.at(configChan)*localStd);
      //if(minIntensity < 1.0) nia::nout << "Intensity threshold ridiculously small: " << minIntensity << "\n";
      if(Imax < localThreshold) continue;
      localThreshold = Imax;
      Cluster* c = new Cluster();
      std::vector<uint16_t> borderX;
      std::vector<uint16_t> borderY;
      std::vector<uint16_t> borderVal;
      borderX.push_back(lm.x);
      borderY.push_back(lm.y);
      borderVal.push_back(Imax);
      used->setValue(lm.x,lm.y,0);
      cMask->setValue(lm.x,lm.y,1);
      double sumI = Imax;
      while(borderX.size() > 0){
	uint32_t nborder = borderX.size();
	for(uint32_t b = 0; b < nborder; b++){
	  int bi = borderX.at(0);
	  int bj = borderY.at(0);
	  int left = bi-1;
	  int right = bi+2;
	  int top = bj-1;
	  int bottom = bj+2;
	  if(left < 0) left++;
	  if(right >= frame->width()) right--;
	  if(top < 0) top++;
	  if(bottom >= frame->height()) bottom--;
	  c->addPoint(bi,bj);
	  double upperLimit = Imax;
	  if((borderVal.at(0) - minThreshold)/(Imax - minThreshold) < 0.3) upperLimit = borderVal.at(0);
	  for(int di = left; di < right; di++){
	    for(int dj = top; dj < bottom; dj++){
	      double val = used->getValue(di,dj)*((double)frame->getPixel(di,dj) - minThreshold) / (upperLimit - minThreshold);
	      if(val < 0.001 || val > 1.0) continue;
	      used->setValue(di,dj,0);
	      borderX.push_back(di);
	      borderY.push_back(dj);
	      borderVal.push_back(frame->getPixel(di,dj));
	      sumI += frame->getPixel(di,dj);
	      cMask->setValue(di,dj,1);
	    }
	  }
	  borderX.erase(borderX.begin());
	  borderY.erase(borderY.begin());
	  borderVal.erase(borderVal.begin());
	}
      }
      bool unfilled = true;
      int size = (int)c->size();
      int x1 = std::max(1,lm.x - size);
      int x2 = std::min((int)frame->width()-1,lm.x + size);
      int y1 = std::max(1,lm.y - size);
      int y2 = std::min((int)frame->height()-1,lm.y + size);
      while(unfilled){
	unfilled = false;
	for(int i = x1; i < x2; i++){
	  for(int j = y1; j < y2; j++){
	    if(cMask->getValue(i,j) > 0 || m->getValue(i,j) < 1) continue;
	    int sum = cMask->getValue(i-1,j-1) + cMask->getValue(i,j-1) + cMask->getValue(i+1,j-1) + cMask->getValue(i-1,j) + cMask->getValue(i+1,j) + cMask->getValue(i-1,j+1) + cMask->getValue(i,j+1) + cMask->getValue(i+1,j+1);
	    if(sum > 4){
	      cMask->setValue(i,j,1);
	      used->setValue(i,j,0);
	      c->addPoint(i,j);
	      sumI += frame->getPixel(i,j);
	      unfilled = true;
	    }
	  }
	}
      }
      for(int i = x1; i < x2; i++){
	for(int j = y1; j < y2; j++){
	  if(cMask->getValue(i,j) < 1 || m->getValue(i,j) < 1) continue;
	  int sum = cMask->getValue(i-1,j-1) + cMask->getValue(i,j-1) + cMask->getValue(i+1,j-1) + cMask->getValue(i-1,j) + cMask->getValue(i+1,j) + cMask->getValue(i-1,j+1) + cMask->getValue(i,j+1) + cMask->getValue(i+1,j+1);
	  if(sum < 3){
	    cMask->setValue(i,j,0);
	    used->setValue(i,j,1);
	    sumI -= frame->getPixel(i,j);
	    c->removePoint(LocalizedObject::Point(i,j));
	  }
	}
      }
      if(sumI < minIntensity){
	for(int i = c->size()-1; i >=0; i--){
	  LocalizedObject::Point pt = c->getPoint(i);
	  used->setValue(pt.x,pt.y,1);
	  cMask->setValue(pt.x,pt.y,0);
	}
	delete c;
	continue;
      }
      c->computeCenter();
      if(c->contains(c->center())){
	if(c->size() > 10.6/(rec->resolutionXY()*rec->resolutionXY())){
	  LocalizedObject::Point seed = c->getPoint(0);
	  nia::nout << "Found ridiculously large punctum of size " << c->size() << " seeded at (" << seed.x << "," << seed.y << ")" << "\n";
	  for(int i = c->size()-1; i >=0; i--){
	    LocalizedObject::Point pt = c->getPoint(i);
	    cMask->setValue(pt.x,pt.y,0);
	  }
	  delete c;
	  continue;
	}
	rec->addPunctum(chan,c);
	nNew++;
	for(int i = c->size()-1; i >=0; i--){
	  LocalizedObject::Point pt = c->getPoint(i);
	  cMask->setValue(pt.x,pt.y,0);
	}
      }
      else{
	for(int i = c->size()-1; i >=0; i--){
	  LocalizedObject::Point pt = c->getPoint(i);
	  used->setValue(pt.x,pt.y,1);
	  cMask->setValue(pt.x,pt.y,0);
	}
	delete c;
      }
    }
    count++;
  }
  delete cMask;
  delete used;

  resolveOverlaps(frame,rec,chan);
  nia::nout << "Found " << rec->nPuncta(chan) << " puncta in channel " << (int)chan << " after " << (int)count << " iterations." << "\n";
}

void ImageAnalysisToolkit::findSaturatedPuncta(ImFrame* frame, ImRecord* rec, uint8_t chan)
{
  //nia::nout << "Find saturated puncta" << "\n";
  Mask* m = rec->getSignalMask(chan);
  Mask* used = m->getCopy();
  Mask* cMask = new Mask(frame->width(),frame->height());
  std::vector<uint16_t> borderSatX;
  std::vector<uint16_t> borderSatY;
  std::vector<uint16_t> borderUnsatX;
  std::vector<uint16_t> borderUnsatY;
  std::vector<uint16_t> borderVal;
  uint8_t configChan = chan;
  if(configChan >= m_localWindow.size()) configChan = 0;
  uint32_t minMinSignal = (uint32_t)std::min(m_localWindow.at(configChan) / (rec->resolutionXY()*rec->resolutionXY()),(double)m->sum());
  int lwi = (int)(m_localWindowIncrement.at(configChan) / rec->resolutionXY());
  for(uint16_t i = 0; i < frame->width(); i++){
    for(uint16_t j = 0; j < frame->height(); j++){
      uint16_t value = frame->getPixel(i,j);
      if(used->getValue(i,j)*value < m_saturationThreshold) continue;
      Cluster* c = new Cluster();
      c->setPeakIntensity(value);
      uint32_t totI = value;
      c->addPoint(i,j);
      used->setValue(i,j,0);
      cMask->setValue(i,j,1);
      borderSatX.push_back(i);
      borderSatY.push_back(j);
      while(borderSatX.size() > 0){
	int bi = borderSatX.at(0);
	int bj = borderSatY.at(0);
	int left = bi-1;
	int right = bi+2;
	int top = bj-1;
	int bottom = bj+2;
	if(left < 0) left++;
	if(right >= frame->width()) right--;
	if(top < 0) top++;
	if(bottom >= frame->height()) bottom--;
	for(int x = left; x < right; x++){
	  for(int y = top; y < bottom; y++){
	    value = frame->getPixel(x,y);
	    if(used->getValue(x,y) > 0){
	      if(value > m_saturationThreshold){
		borderSatX.push_back(x);
		borderSatY.push_back(y);
	      }
	      else{
		borderUnsatX.push_back(x);
		borderUnsatY.push_back(y);
		borderVal.push_back(value);
	      }
	      c->addPoint(x,y);
	      used->setValue(x,y,0);
	      cMask->setValue(x,y,1);
	      totI += value;
	    }
	  }
	}
	borderSatX.erase(borderSatX.begin());
	borderSatY.erase(borderSatY.begin());
      }

      c->computeCenter();
      LocalizedObject::Point center = c->center();
      int x1 = std::max(center.x-lwi,0);
      int x2 = std::min((int)frame->width(),center.x+lwi);
      int y1 = std::max(center.y-lwi,0);
      int y2 = std::min((int)frame->height(),center.y+lwi);
      int nSignal = m->sum(x1,x2,y1,y2);
      int minSignal = std::max(minMinSignal,4*c->size());
      while(nSignal < minSignal){
	x1 = std::max(x1-lwi,0);
	x2 = std::min((int)frame->width(),x2+lwi);
	y1 = std::max(y1-lwi,0);
	y2 = std::min((int)frame->height(),y2+lwi);
	nSignal = m->sum(x1,x2,y1,y2);
      }
      uint16_t Imax = m_saturationThreshold;
      double localMedian = frame->median(x1,x2,y1,y2,used);
      double localStd = frame->std(x1,x2,y1,y2,used);
      double localThreshold = std::min(localMedian + (Imax - localMedian)/2, localMedian + m_floorThreshold.at(configChan)*localStd);
      while(borderUnsatX.size() > 0){
	uint16_t upperLimit = Imax;
	if((borderVal.at(0) - localThreshold)/(Imax - localThreshold) < 0.3) upperLimit = borderVal.at(0);
	if(upperLimit <= localThreshold){
	  borderUnsatX.erase(borderUnsatX.begin());
	  borderUnsatY.erase(borderUnsatY.begin());
	  borderVal.erase(borderVal.begin());
	  continue;
	}
	int bi = borderUnsatX.at(0);
	int bj = borderUnsatY.at(0);
	int left = bi-1;
	int right = bi+2;
	int top = bj-1;
	int bottom = bj+2;
	if(left < 0) left++;
	if(right >= frame->width()) right--;
	if(top < 0) top++;
	if(bottom >= frame->height()) bottom--;
	for(int x = left; x < right; x++){
	  for(int y = top; y < bottom; y++){
	    uint16_t pixelValue = frame->getPixel(x,y);
	    double val = used->getValue(x,y)*(pixelValue - localThreshold) / (upperLimit - localThreshold);
	    if(val < 0.001 || val > 1.0) continue;
	    borderUnsatX.push_back(x);
	    borderUnsatY.push_back(y);
	    borderVal.push_back(pixelValue);
	    c->addPoint(x,y);
	    used->setValue(x,y,0);
	    cMask->setValue(x,y,1);
	    totI += pixelValue;
	  }
	}
	borderUnsatX.erase(borderUnsatX.begin());
	borderUnsatY.erase(borderUnsatY.begin());
	borderVal.erase(borderVal.begin());
      }

      bool unfilled = true;
      while(unfilled){
	unfilled = false;
	for(uint16_t x = x1+1; x < x2-1; x++){
	  for(uint16_t y = y1+1; y < y2-1; y++){
	    if(cMask->getValue(x,y) > 0 || m->getValue(x,y) < 1) continue;
	    int sum = cMask->getValue(x-1,y-1) + cMask->getValue(x,y-1) + cMask->getValue(x+1,y-1) + cMask->getValue(x-1,y) + cMask->getValue(x+1,y) + cMask->getValue(x-1,y+1) + cMask->getValue(x,y+1) + cMask->getValue(x+1,y+1);
	    if(sum > 4){
	      cMask->setValue(x,y,1);
	      used->setValue(x,y,0);
	      c->addPoint(x,y);
	      totI += frame->getPixel(x,y);
	      unfilled = true;
	    }
	  }
	}
      }
      for(uint16_t x = x1+1; x < x2-1; x++){
	for(uint16_t y = y1+1; y < y2-1; y++){
	  if(cMask->getValue(x,y) < 1 || m->getValue(x,y) < 1 || frame->getPixel(x,y) > m_saturationThreshold) continue;
	  int sum = cMask->getValue(x-1,y-1) + cMask->getValue(x,y-1) + cMask->getValue(x+1,y-1) + cMask->getValue(x-1,y) + cMask->getValue(x+1,y) + cMask->getValue(x-1,y+1) + cMask->getValue(x,y+1) + cMask->getValue(x+1,y+1);
	  if(sum < 3){
	    cMask->setValue(x,y,0);
	    used->setValue(x,y,1);
	    c->removePoint(LocalizedObject::Point(x,y));
	    totI -= frame->getPixel(x,y);
	  }
	}
      }

      if(c->size() < 2){
	for(int x = c->size()-1; x >=0; x--){
	  LocalizedObject::Point pt = c->getPoint(x);
	  used->setValue(pt.x,pt.y,1);
	}
	cMask->clear(x1,x2,y1,y2);
	delete c;
	continue;
      }
      center = c->center();
      int index = c->indexOf(center);
      if(index < c->size()){
	if(c->size() > 10.6/(rec->resolutionXY()*rec->resolutionXY())){
	  LocalizedObject::Point seed = c->getPoint(0);
	  nia::nout << "Found ridiculously large punctum of size " << c->size() << " seeded at (" << seed.x << "," << seed.y << ")" << "\n";
	  cMask->clear(x1,x2,y1,y2);
	  delete c;
	  continue;
	}
	LocalizedObject::Point tmp = c->getPoint(0);
	c->setPoint(0,c->getPoint(index));
	c->setPoint(index,tmp);
	c->setIntegratedIntensity(totI);
	rec->addPunctum(chan,c);
      }
      else{
	for(int x = c->size()-1; x >=0; x--){
	  LocalizedObject::Point pt = c->getPoint(x);
	  used->setValue(pt.x,pt.y,1);
	}
	delete c;
      }
      cMask->clear(x1,x2,y1,y2);
      borderSatX.clear();
      borderSatY.clear();
      borderUnsatX.clear();
      borderUnsatY.clear();
      borderVal.clear();
    }
  }
  delete used;
  delete cMask;
}

void ImageAnalysisToolkit::resolveOverlaps(ImFrame* frame, ImRecord* rec, uint8_t chan)
{
  //nia::nout << "Resolve overlaps" << "\n";
  std::vector<Cluster*> clusters = rec->puncta(chan);
  std::vector<Cluster*> satClusters;
  //std::vector<Cluster*> unsatClusters;
  std::vector<Cluster*> newClusters;
  std::vector<Gaus2D> fs;
  Mask* m = new Mask(frame->width(),frame->height());//rec->getPunctaMask(chan,false);
  for(std::vector<Cluster*>::iterator it = clusters.begin(); it != clusters.end(); it++){
    uint16_t imax = (*it)->peak();
    if(imax > m_saturationThreshold){
      satClusters.push_back((*it)->getCopy());
    }
    else{
      //unsatClusters.push_back(*it);
      LocalizedObject::Point peak = (*it)->getPoint(0);
      LocalizedObject::Point trough = (*it)->getPoint((*it)->size()-1);
      uint16_t imin = frame->getPixel(trough.x,trough.y);
      std::vector<LocalizedObject::Point> pts = (*it)->getPoints();
      for(std::vector<LocalizedObject::Point>::iterator jt = pts.begin(); jt != pts.end(); jt++){
	m->setValue(jt->x,jt->y,1);
	uint16_t val = frame->getPixel(jt->x,jt->y);
	if(val < imin){
	  imin = val;
	  //trough = *jt;
	}
	if(val > imax){
	  imax = val;
	  peak = *jt;
	}
      }
      fs.push_back(Gaus2D(imax,peak.x,peak.y,(-0.5*(*it)->size()/3.14159)/log(double(imin)/imax)));
      Cluster* c = new Cluster();
      c->addPoint(peak);
      c->setPeakIntensity(imax);
      c->computeCenter();
      newClusters.push_back(c);
      m->setValue(peak.x,peak.y,0);
    }
  }
  //nia::nout << "Original: (" << (int)unsatClusters[0]->getPoint(0).x << "," << (int)unsatClusters[0]->getPoint(0).y << "), " << (int)unsatClusters[0]->size() << "\n";
  //nia::nout << "Original: (" << (int)unsatClusters[1]->getPoint(0).x << "," << (int)unsatClusters[1]->getPoint(0).y << "), " << (int)unsatClusters[1]->size() << "\n";
  //nia::nout << "Original: (" << (int)unsatClusters[2]->getPoint(0).x << "," << (int)unsatClusters[2]->getPoint(0).y << "), " << (int)unsatClusters[2]->size() << "\n";
  
  double maxDist = 5.0 / (rec->resolutionXY()*rec->resolutionXY());
  for(int i = 0; i < frame->width(); i++){
    for(int j = 0; j < frame->height(); j++){
      if(m->getValue(i,j) < 1) continue;
      int value = frame->getPixel(i,j);
      double maxDiff = -10.0;
      int maxK = -1;
      double minDiff = 999999.0;
      int minK = -1;
      double minDist = 9999999.0;
      int distK = -1;
      for(uint32_t k = 0; k < newClusters.size(); k++){
	double kdist = newClusters.at(k)->distanceTo(LocalizedObject::Point(i,j));
	if(kdist > maxDist) continue;
	if(kdist < minDist){
	  minDist = kdist;
	  distK = k;
	}
	double kDiff = fs.at(k).calculate(i,j)/value - 1.0;
	if(kDiff > maxDiff){
	  maxDiff = kDiff;
	  maxK = k;
	}
	if(fabs(kDiff) < fabs(minDiff)){
	  minDiff = kDiff;
	  minK = k;
	}
      }
      if(minDiff > -0.1 && minK >= 0) newClusters.at(minK)->addPoint(i,j);
      else if(maxDiff > -1.0) newClusters.at(maxK)->addPoint(i,j);
      else if(distK >= 0) newClusters.at(distK)->addPoint(i,j);
    }
  }
  //nia::nout << "New: (" << (int)newClusters[0]->getPoint(0).x << "," << (int)newClusters[0]->getPoint(0).y << "), " << (int)newClusters[0]->size() << "\n";
  //nia::nout << "New: (" << (int)newClusters[1]->getPoint(0).x << "," << (int)newClusters[1]->getPoint(0).y << "), " << (int)newClusters[1]->size() << "\n";
  //nia::nout << "New: (" << (int)newClusters[2]->getPoint(0).x << "," << (int)newClusters[2]->getPoint(0).y << "), " << (int)newClusters[2]->size() << "\n";
  
  m->clear(0,frame->width(),0,frame->height());
  std::vector<uint16_t> borderX;
  std::vector<uint16_t> borderY;
  for(std::vector<Cluster*>::iterator it = newClusters.begin(); it != newClusters.end(); it++){
    std::vector<LocalizedObject::Point> pts = (*it)->getPoints();
    for(std::vector<LocalizedObject::Point>::iterator jt = pts.begin(); jt != pts.end(); jt++) m->setValue(jt->x,jt->y,1);
    LocalizedObject::Point peak = *(pts.begin());
    (*it)->clearPoints();
    borderX.push_back(peak.x);
    borderY.push_back(peak.y);
    while(borderX.size() > 0){
      uint16_t nborder = borderX.size();
      for(uint16_t b = 0; b < nborder; b++){
	int bi = borderX.at(0);
	int bj = borderY.at(0);
	int left = bi-1;
	int right = bi+2;
	int top = bj-1;
	int bottom = bj+2;
	if(left < 0) left++;
	if(right >= frame->width()) right--;
	if(top < 0) top++;
	if(bottom >= frame->height()) bottom--;
	(*it)->addPoint(bi,bj,frame->getPixel(bi,bj));
	for(int i = left; i < right; i++){
	  for(int j = top; j < bottom; j++){
	    if(m->getValue(i,j) > 0){
	      m->setValue(i,j,0);
	      borderX.push_back(i);
	      borderY.push_back(j);
	    }
	  }
	}
	borderX.erase(borderX.begin());
	borderY.erase(borderY.begin());
      }
    }
    for(std::vector<LocalizedObject::Point>::iterator jt = pts.begin(); jt != pts.end(); jt++) m->setValue(jt->x,jt->y,0);
  }
  
  rec->clearPuncta(chan);
  for(std::vector<Cluster*>::iterator it = satClusters.begin(); it != satClusters.end(); it++) rec->addPunctum(chan,*it);
  uint8_t configChan = chan;
  if(configChan >= m_minPunctaRadius.size()) configChan = 0;
  double punctaAreaThreshold = 3.14159*m_minPunctaRadius.at(configChan)*m_minPunctaRadius.at(configChan)/(rec->resolutionXY()*rec->resolutionXY());
  uint32_t areaThreshold = (uint32_t)(m_reclusterThreshold.at(configChan)*punctaAreaThreshold);
  for(std::vector<Cluster*>::iterator it = newClusters.begin(); it != newClusters.end(); it++){
    if(!(*it)) continue;
    if((*it)->size() > areaThreshold){
      rec->addPunctum(chan, *it);
      continue;
    }
    int maxBorder = 0;
    int iSize = (*it)->size();
    for(std::vector<Cluster*>::iterator jt = newClusters.begin(); jt != it; jt++){
      if(!(*jt)) continue;
      int jSize = (*jt)->size();
      if(jSize < 2*iSize) continue;
      if((*it)->distanceTo(**jt) > iSize+jSize) continue;
      int jBorder = (*jt)->getBorderLength(*it);
      if(jBorder > maxBorder){
	maxBorder = jBorder;
      }
    }
    for(std::vector<Cluster*>::iterator jt = it+1; jt != newClusters.end(); jt++){
      if(!(*jt)) continue;
      int jSize = (*jt)->size();
      if(jSize < 2*iSize) continue;
      if((*it)->distanceTo(**jt) > iSize+jSize) continue;
      int jBorder = (*jt)->getBorderLength(*it);
      if(jBorder > maxBorder){
	maxBorder = jBorder;
      }
    }
    if(maxBorder > sqrt(iSize)){
      delete *it;
      *it = NULL;
      continue;
    }
    if(iSize > 3) rec->addPunctum(chan,*it);
  }
  delete m;
}

void ImageAnalysisToolkit::findSynapses(ImRecord* rec)
{
  for(int icol = 0; icol < rec->nSynapseCollections(); icol++){
    int nSynapses = 0;
    SynapseCollection* sc = rec->getSynapseCollection(icol);
    std::vector<uint8_t> chan = sc->channels();
    uint8_t nchans = sc->nChannels();
    int* np = new int[nchans];
    int* pi = new int[nchans];
    int maxIndex = 1;
    for(int i = 0; i < nchans; i++){
      np[i] = rec->nPuncta(chan.at(i));
      pi[i] = 0;
    }
    for(int i = 1; i < nchans; i++) maxIndex *= np[i];
    for(int j = 0; j < np[0]; j++){
      Synapse* best = NULL;
      double bestScore = -999;
      Cluster* c = rec->getPunctum(chan.at(0),j);
      int sumpi = 1;
      while(sumpi > 0){
	Synapse* s = new Synapse();
	s->addPunctum(rec->getPunctum(chan.at(0),j),j);
	bool potentialSynapse = true;
	for(int i = 1; i < nchans; i++){
	  Cluster* c2 = rec->getPunctum(chan.at(i),pi[i]);
	  if(c->peakToPeakDistance2(c2) > 2*(c->size()+c2->size())){
	    potentialSynapse = false;
	    for(int k = i; k > 0; k--){
	      pi[k]++;
	      if(pi[k] < np[k]) break;
	      pi[k] = 0;
	    }
	    for(int k = i+1; k < nchans; k++) pi[k] = 0;
	    break;
	  }
	  s->addPunctum(c2,pi[i]);
	}
	if(potentialSynapse){
	  if(sc->computeColocalization(s)){
	    if(s->colocalizationScore() > bestScore){
	      bestScore = s->colocalizationScore();
	      if(best) delete best;
	      best = s;
	    }
	    else delete s;
	  }
	  else delete s;
	  for(int i = nchans-1; i > 0; i--){
	    pi[i]++;
	    if(pi[i] < np[i]) break;
	    pi[i] = 0;
	  }
	}
	else delete s;
	sumpi = 0;
	for(int i = 1; i < nchans; i++) sumpi += pi[i];
      }
      if(best){
	best->computeCenter();
	sc->addSynapse(best);
	nSynapses++;
      }
    }
    delete[] np;
    delete[] pi;
    nia::nout << "Found " << nSynapses << " synapses of type " << icol << ": " << sc->description() << "\n";
  }
}

void ImageAnalysisToolkit::write(std::ofstream& fout)
{
  char* buf = new char[2000];
  buf[0] = (char)m_master;
  if(m_mode == OVERRIDE) buf[1] = (char)0;
  else buf[1] = (char)1;
  buf[2] = (char)m_postChan;
  buf[3] = (char)m_punctaFindingIterations;
  NiaUtils::writeShortToBuffer(buf,4,m_saturationThreshold);
  uint8_t nchan = m_localWindow.size();
  buf[6] = (char)nchan;
  uint32_t offset = 7;
  for(uint8_t i = 0; i < nchan; i++){
    NiaUtils::writeDoubleToBuffer(buf,offset,m_localWindow.at(i));
    NiaUtils::writeDoubleToBuffer(buf,offset+4,m_localWindowIncrement.at(i));
    NiaUtils::writeDoubleToBuffer(buf,offset+8,m_minPunctaRadius.at(i));
    NiaUtils::writeDoubleToBuffer(buf,offset+12,m_reclusterThreshold.at(i));
    NiaUtils::writeDoubleToBuffer(buf,offset+16,m_noiseRemovalThreshold.at(i));
    NiaUtils::writeDoubleToBuffer(buf,offset+20,m_peakThreshold.at(i));
    NiaUtils::writeDoubleToBuffer(buf,offset+24,m_floorThreshold.at(i));
    offset += 28;
  }
  fout.write(buf,offset);
  delete[] buf;
}

void ImageAnalysisToolkit::read(std::ifstream& fin)
{
  char* buf = new char[2000];
  fin.read(buf,7);
  m_master = (uint8_t)buf[0];
  if((uint8_t)buf[1] == 0) m_mode = OVERRIDE;
  else m_mode = OR;
  m_postChan = (uint8_t)buf[2];
  m_punctaFindingIterations = (uint8_t)buf[3];
  m_saturationThreshold = NiaUtils::convertToShort(buf[4],buf[5]);
  uint8_t nchan = (uint8_t)buf[6];
  fin.read(buf,nchan*28);
  uint32_t offset = 0;
  for(uint8_t i = 0; i < nchan; i++){
    m_localWindow.push_back(NiaUtils::convertToDouble(buf[offset],buf[offset+1],buf[offset+2],buf[offset+3]));
    offset += 4;
    m_localWindowIncrement.push_back(NiaUtils::convertToDouble(buf[offset],buf[offset+1],buf[offset+2],buf[offset+3]));
    offset += 4;
    m_minPunctaRadius.push_back(NiaUtils::convertToDouble(buf[offset],buf[offset+1],buf[offset+2],buf[offset+3]));
    offset += 4;
    m_reclusterThreshold.push_back(NiaUtils::convertToDouble(buf[offset],buf[offset+1],buf[offset+2],buf[offset+3]));
    offset += 4;
    m_noiseRemovalThreshold.push_back(NiaUtils::convertToDouble(buf[offset],buf[offset+1],buf[offset+2],buf[offset+3]));
    offset += 4;
    m_peakThreshold.push_back(NiaUtils::convertToDouble(buf[offset],buf[offset+1],buf[offset+2],buf[offset+3]));
    offset += 4;
    m_floorThreshold.push_back(NiaUtils::convertToDouble(buf[offset],buf[offset+1],buf[offset+2],buf[offset+3]));
    offset += 4;
  }
  delete[] buf;
}

uint8_t ImageAnalysisToolkit::getBitDepth()
{
  uint8_t bd;
  uint16_t st = m_saturationThreshold + 2;
  for(bd = 0; st > 1; bd++) st = st / 2;
  return bd;
}
