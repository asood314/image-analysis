#include "ImageAnalysisToolkit.hpp"

ImageAnalysisToolkit::ImageAnalysisToolkit()
{
  m_master = 255;
  m_mode = OVERRIDE;
  m_punctaFindingIterations = 10;
  m_saturationThreshold = 4094;
  m_localWindow.push_back(5.0);
  m_backgroundThreshold.push_back(0.0);
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

void ImageAnalysisToolkit::makeSeparateConfigs(int nchan)
{
  for(int i = 1; i < nchan; i++){
    m_localWindow.push_back(m_localWindow.at(0));
    m_backgroundThreshold.push_back(m_backgroundThreshold.at(0));
    m_minPunctaRadius.push_back(m_minPunctaRadius.at(0));
    m_reclusterThreshold.push_back(m_reclusterThreshold.at(0));
    m_noiseRemovalThreshold.push_back(m_noiseRemovalThreshold.at(0));
    m_peakThreshold.push_back(m_peakThreshold.at(0));
    m_floorThreshold.push_back(m_floorThreshold.at(0));
  }
}

void ImageAnalysisToolkit::makeSingleConfig()
{
  int nchan = m_localWindow.size();
  for(int i = 1; i < nchan; i++){
    m_localWindow.erase(m_localWindow.begin()+1);
    m_backgroundThreshold.erase(m_backgroundThreshold.begin()+1);
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

  findSignal(analysisStack,rec,zplane);
  
  for(int w = 0; w < analysisStack->nwaves(); w++){
    rec->clearPuncta(w);
    findPuncta(analysisStack->frame(w,zplane),rec,w);
  }

  findSynapses(rec);
  
}

Mask* ImageAnalysisToolkit::findOutliers(ImFrame* frame)
{
  double median = frame->median();
  double std = frame->std();
  int threshold = (int)(median + 10*std);
  Mask* m = new Mask(frame->width(),frame->height(),1);
  for(int i = 0; i < frame->width(); i++){
    for(int j = 0; j < frame->height(); j++){
      if(frame->getPixel(i,j) > threshold) m->setValue(i,j,0);
    }
  }
  double std2 = frame->std(m);
  while(std2/std < 0.9){
    std = std2;
    threshold = (int)(median + 10*std);
    for(int i = 0; i < frame->width(); i++){
      for(int j = 0; j < frame->height(); j++){
	if(frame->getPixel(i,j) > threshold) m->setValue(i,j,0);
      }
    }
    std2 = frame->std(m);
  }
  std::cout << frame->median(m) << std::endl << std2 << std::endl;
  return m;
}

void ImageAnalysisToolkit::findSignal(ImStack* analysisStack, ImRecord* rec, int zplane)
{
  if(m_master == 255){
    for(int w = 0; w < analysisStack->nwaves(); w++){
      //rec->setThreshold(w,findThreshold(analysisStack->frame(w,zplane)));
      findSignal(analysisStack->frame(w,zplane),rec,w);
    }
  }
  else{
    //rec->setThreshold(m_master,findThreshold(analysisStack->frame(m_master,zplane)));
    findSignal(analysisStack->frame(m_master,zplane),rec,m_master);
    Mask* m = rec->getSignalMask(m_master);
    if(m_mode == OVERRIDE){
      for(int w = 0; w < analysisStack->nwaves(); w++){
	if(w == m_master) continue;
	ImFrame* f = analysisStack->frame(w,zplane);
	double mean = f->mean(m);
	double std = f->std(m);
	double threshold = mean + std;
	Mask* m2 = m->getCopy();
	for(int i = 0; i < f->width(); i++){
	  for(int j = 0; j < f->height(); j++){
	    if(f->getPixel(i,j) > threshold) m2->setValue(i,j,1);
	  }
	}
	bool unpruned = true;
	while(unpruned){
	  unpruned = false;
	  for(int i = 1; i < f->width()-1; i++){
	    for(int j = 1; j < f->height()-1; j++){
	      if(m2->getValue(i,j) < 1) continue;
	      int sum = m2->getValue(i-1,j-1) + m2->getValue(i,j-1) + m2->getValue(i+1,j-1) + m2->getValue(i-1,j) + m2->getValue(i+1,j) + m2->getValue(i-1,j+1) + m2->getValue(i,j+1) + m2->getValue(i+1,j+1);
	      if(sum < 3){
		m2->setValue(i,j,0);
		unpruned = true;
	      }
	    }
	  }
	}
	rec->setSignalMask(w,m2);
      }
    }
    else{ //m_mode == OR
      for(int w = 0; w < analysisStack->nwaves(); w++){
	if(w == m_master) continue;
	//rec->setThreshold(w,findThreshold(analysisStack->frame(w,zplane)));
	findSignal(analysisStack->frame(w,zplane),rec,w);
	Mask* m2 = rec->getSignalMask(w);
	m2->OR(*m);
      }
    }
  }
  nia::nout << "Done signal finding" << "\n";
}

void ImageAnalysisToolkit::findSignal(ImFrame* frame, ImRecord* rec, int chan)
{
  int globalThreshold = findThreshold(frame);
  while(globalThreshold >= 0){
    std::cout << "Using global threshold " << globalThreshold << std::endl;
    rec->setThreshold(chan,globalThreshold);
    Mask* densityMask = applyThreshold(frame,rec,chan);
    globalThreshold = findThreshold(frame,rec->getSignalMask(chan)->inverse(),densityMask->inverse());
    delete densityMask;
  }
}

Mask* ImageAnalysisToolkit::applyThreshold(ImFrame* frame, ImRecord* rec, int chan)
{
  //int globalThreshold = findThreshold(frame);
  //rec->setThreshold(chan,globalThreshold);
  int globalThreshold = rec->getThreshold(chan);
  Mask* m = new Mask(frame->width(),frame->height());
  for(int i = 0; i < frame->width(); i++){
    for(int j = 0; j < frame->height(); j++){
      if(frame->getPixel(i,j) > globalThreshold){
	m->setValue(i,j,1);
      }
    }
  }

  
  //Mask* signalStrengthMask = new Mask(frame->width(),frame->height());
  Mask* denseRegionMask = new Mask(frame->width(),frame->height());
  int windowSize = (int)(2.0 / rec->resolutionXY());
  int numerator = m->sum(0,windowSize,0,windowSize);
  double localMean1 = 0;
  double localMean2 = 0;
  double localMean3 = 0;
  double localMean4 = frame->mean(0,windowSize,0,windowSize,m);
  int den1 = 0;
  int den2 = 0;
  int den3 = 0;
  int den4 = numerator;
  Mask* outMask = findOutliers(frame);
  outMask->multiply(*m);
  double globalStd = frame->std(outMask);
  delete outMask;
  if(numerator > 0.4*windowSize*windowSize) denseRegionMask->setValue(0,0,1);
  //if(localMean4 - globalStd > globalThreshold) signalStrengthMask->setValue(0,0,1);
  localMean4 *= den4;//windowSize*windowSize;
  for(int i = 0; i < frame->width(); i++){
    int left = i - windowSize;
    if(left < 0) left = 0;
    int right = i + windowSize;
    if(right > frame->width()) right = frame->width();
    int top = 0;
    int bottom = windowSize;
    if(i > 0){
      for(int k = 0; k < bottom; k++){
	if(m->getValue(i-1,k) != 1) continue;
	localMean3 += frame->getPixel(i-1,k);
	localMean4 -= frame->getPixel(i-1,k);
	den3++;
	den4--;
      }
      if(left > 0){
	for(int k = 0; k < bottom; k++) numerator -= m->getValue(left-1,k);
	for(int k = 0; k < bottom; k++){
	  if(m->getValue(left-1,k) != 1) continue;
	  localMean3 -= frame->getPixel(left-1,k);
	  den3--;
	}
      }
      if(right < frame->width() - 1){
	for(int k = 0; k < bottom; k++) numerator += m->getValue(right,k);
	for(int k = 0; k < bottom; k++){
	  if(m->getValue(right,k) != 1) continue;
	  localMean4 += frame->getPixel(right,k);
	  den4++;
	}
      }
      if(numerator > 0.4*(right-left)*(bottom-top)) denseRegionMask->setValue(i,0,1);
      //if(localMean3/(windowSize*windowSize) - globalStd > globalThreshold) signalStrengthMask->setValue(i,0,1);
      //else if(localMean4/(windowSize*windowSize) - globalStd > globalThreshold) signalStrengthMask->setValue(i,0,1);
      //if(localMean3/den3 - globalStd > globalThreshold) signalStrengthMask->setValue(i,0,1);
      //else if(localMean4/den4 - globalStd > globalThreshold) signalStrengthMask->setValue(i,0,1);
    }
    
    for(int j = 1; j < frame->height(); j++){
      for(int k = left; k < i; k++){
	//if(m->getValue(k,j-1) != 1) continue;
	localMean2 += frame->getPixel(k,j-1);
	localMean3 -= frame->getPixel(k,j-1);
	den2++;
	den3--;
      }
      for(int k = i; k < right; k++){
	//if(m->getValue(k,j-1) != 1) continue;
	localMean1 += frame->getPixel(k,j-1);
	localMean4 -= frame->getPixel(k,j-1);
	den1++;
	den4--;
      }
      top = j - windowSize;
      if(top < 0) top = 0;
      bottom = j + windowSize-1;
      if(bottom > frame->height()) bottom = frame->height();
      if(top > 0){
	for(int k = left; k < right; k++) numerator -= m->getValue(k,top-1);
	for(int k = left; k < i; k++){
	  //if(m->getValue(k,top-1) != 1) continue;
	  localMean2 -= frame->getPixel(k,top-1);
	  den2--;
	}
	for(int k = i; k < right; k++){
	  //if(m->getValue(k,top-1) != 1) continue;
	  localMean1 -= frame->getPixel(k,top-1);
	  den1--;
	}
      }
      if(bottom < frame->height()){
	for(int k = left; k < right; k++) numerator += m->getValue(k,bottom);
	for(int k = left; k < i; k++){
	  //if(m->getValue(k,bottom) != 1) continue;
	  localMean3 += frame->getPixel(k,bottom);
	  den3++;
	}
	for(int k = i; k < right; k++){
	  //if(m->getValue(k,bottom) != 1) continue;
	  localMean4 += frame->getPixel(k,bottom);
	  den4++;
	}
      }
      if(numerator > 0.4*(right-left)*(bottom-top)) denseRegionMask->setValue(i,j,1);
      //if(m->sum(left,i,top,j) > 0.5*(i-left)*(j-top)) denseRegionMask->setValue(i,j,1);
      //else if(m->sum(left,i,j,bottom) > 0.5*(i-left)*(bottom-j)) denseRegionMask->setValue(i,j,1);
      //else if(m->sum(i,right,j,bottom) > 0.5*(right-i)*(bottom-j)) denseRegionMask->setValue(i,j,1);
      //else if(m->sum(i,right,top,j) > 0.5*(right-i)*(j-top)) denseRegionMask->setValue(i,j,1);
      
      //if(localMean1/(windowSize*windowSize) - globalStd > globalThreshold) signalStrengthMask->setValue(i,j,1);
      //else if(localMean2/(windowSize*windowSize) - globalStd > globalThreshold) signalStrengthMask->setValue(i,j,1);
      //else if(localMean3/(windowSize*windowSize) - globalStd > globalThreshold) signalStrengthMask->setValue(i,j,1);
      //else if(localMean4/(windowSize*windowSize) - globalStd > globalThreshold) signalStrengthMask->setValue(i,j,1);
      //if(localMean1/den1 - globalStd > globalThreshold) signalStrengthMask->setValue(i,j,1);
      //else if(localMean2/den2 - globalStd > globalThreshold) signalStrengthMask->setValue(i,j,1);
      //else if(localMean3/den3 - globalStd > globalThreshold) signalStrengthMask->setValue(i,j,1);
      //else if(localMean4/den4 - globalStd > globalThreshold) signalStrengthMask->setValue(i,j,1);
    }
    
    i++;
    left = i - windowSize;
    if(left < 0) left = 0;
    right = i + windowSize;
    if(right > frame->width()) right = frame->width();
    for(int k = top; k < frame->height(); k++){
      if(m->getValue(i-1,k) != 1) continue;
      localMean2 += frame->getPixel(i-1,k);
      localMean1 -= frame->getPixel(i-1,k);
      den2++;
      den1--;
    }
    if(m->getValue(i-1,frame->height()-1) == 1){
      localMean3 += frame->getPixel(i-1,frame->height()-1);
      localMean4 -= frame->getPixel(i-1,frame->height()-1);
      den3++;
      den4--;
    }
    if(left > 0){
      for(int k = top; k < bottom; k++) numerator -= m->getValue(i-windowSize-1,k);
      for(int k = top; k < frame->height(); k++){
	if(m->getValue(i-windowSize-1,k) != 1) continue;
	localMean2 -= frame->getPixel(i-windowSize-1,k);
	den2--;
      }
      if(m->getValue(i-windowSize-1,frame->height()-1) == 1){
	localMean3 -= frame->getPixel(i-windowSize-1,frame->height()-1);
	den3--;
      }
    }
    if(right < frame->width() - 1){
      for(int k = top; k < bottom; k++) numerator += m->getValue(i+windowSize,k);
      for(int k = top; k < frame->height(); k++){
	if(m->getValue(i+windowSize,k) != 1) continue;
	localMean1 += frame->getPixel(i+windowSize,k);
	den1++;
      }
      if(m->getValue(i+windowSize,frame->height()-1) == 1){
	localMean4 += frame->getPixel(i+windowSize,frame->height()-1);
	den4++;
      }
    }
    if(numerator > 0.4*(right-left)*(bottom-top)) denseRegionMask->setValue(i,frame->height()-1,1);
    //if(localMean1/(windowSize*windowSize) - globalStd > globalThreshold) signalStrengthMask->setValue(i,frame->height()-1,1);
    //else if(localMean2/(windowSize*windowSize) - globalStd > globalThreshold) signalStrengthMask->setValue(i,frame->height()-1,1);
    //if(localMean1/den1 - globalStd > globalThreshold) signalStrengthMask->setValue(i,frame->height()-1,1);
    //else if(localMean2/den2 - globalStd > globalThreshold) signalStrengthMask->setValue(i,frame->height()-1,1);

    for(int j = frame->height()-2; j >= 0; j--){
      for(int k = left; k < i; k++){
	//if(m->getValue(k,j) != 1) continue;
	localMean2 -= frame->getPixel(k,j);
	localMean3 += frame->getPixel(k,j);
	den2--;
	den3++;
      }
      for(int k = i; k < right; k++){
	//if(m->getValue(k,j) != 1) continue;
	localMean1 -= frame->getPixel(k,j);
	localMean4 += frame->getPixel(k,j);
	den1--;
	den4++;
      }
      top = j - windowSize + 1;
      if(top < 0) top = 0;
      bottom = j + windowSize;
      if(bottom >= frame->height()) bottom = frame->height();
      if(top > 0){
	for(int k = left; k < right; k++) numerator += m->getValue(k,top-1);
	for(int k = left; k < i; k++){
	  //if(m->getValue(k,top-1) != 1) continue;
	  localMean2 += frame->getPixel(k,top-1);
	  den2++;
	}
	for(int k = i; k < right; k++){
	  //if(m->getValue(k,top-1) != 1) continue;
	  localMean1 += frame->getPixel(k,top-1);
	  den1++;
	}
      }
      if(bottom < frame->height()){
	for(int k = left; k < right; k++) numerator -= m->getValue(k,bottom);
	for(int k = left; k < i; k++){
	  //if(m->getValue(k,bottom) != 1) continue;
	  localMean3 -= frame->getPixel(k,bottom);
	  den3--;
	}
	for(int k = i; k < right; k++){
	  //if(m->getValue(k,bottom) != 1) continue;
	  localMean4 -= frame->getPixel(k,bottom);
	  den4++;
	}
      }
      if(numerator > 0.4*(right-left)*(bottom-top)) denseRegionMask->setValue(i,j,1);
      //if(m->sum(left,i,top,j) > 0.5*(i-left)*(j-top)) denseRegionMask->setValue(i,j,1);
      //else if(m->sum(left,i,j,bottom) > 0.5*(i-left)*(bottom-j)) denseRegionMask->setValue(i,j,1);
      //else if(m->sum(i,right,j,bottom) > 0.5*(right-i)*(bottom-j)) denseRegionMask->setValue(i,j,1);
      //else if(m->sum(i,right,top,j) > 0.5*(right-i)*(j-top)) denseRegionMask->setValue(i,j,1);
      
      //if(localMean1/(windowSize*windowSize) - globalStd > globalThreshold) signalStrengthMask->setValue(i,j,1);
      //else if(localMean2/(windowSize*windowSize) - globalStd > globalThreshold) signalStrengthMask->setValue(i,j,1);
      //else if(localMean3/(windowSize*windowSize) - globalStd > globalThreshold) signalStrengthMask->setValue(i,j,1);
      //else if(localMean4/(windowSize*windowSize) - globalStd > globalThreshold) signalStrengthMask->setValue(i,j,1);
      //if(localMean1/den1 - globalStd > globalThreshold) signalStrengthMask->setValue(i,j,1);
      //else if(localMean2/den2 - globalStd > globalThreshold) signalStrengthMask->setValue(i,j,1);
      //else if(localMean3/den3 - globalStd > globalThreshold) signalStrengthMask->setValue(i,j,1);
      //else if(localMean4/den4 - globalStd > globalThreshold) signalStrengthMask->setValue(i,j,1);
    }
  }
  //denseRegionMask->OR(*signalStrengthMask);

  
  bool unpruned = true;
  while(unpruned){
    unpruned = false;
    for(int i = 1; i < frame->width()-1; i++){
      for(int j = 1; j < frame->height()-1; j++){
	if(m->getValue(i,j) < 1) continue;
	int sum = m->getValue(i-1,j-1) + m->getValue(i,j-1) + m->getValue(i+1,j-1) + m->getValue(i-1,j) + m->getValue(i+1,j) + m->getValue(i-1,j+1) + m->getValue(i,j+1) + m->getValue(i+1,j+1);
	if(sum < 2+2*denseRegionMask->getValue(i,j)){
	  m->setValue(i,j,0);
	  unpruned = true;
	}
      }
    }
  }
  
  bool unfilled = true;
  while(unfilled){
    unfilled = false;
    for(int i = 1; i < frame->width()-1; i++){
      for(int j = 1; j < frame->height()-1; j++){
	if(m->getValue(i,j) > 0 || denseRegionMask->getValue(i,j) > 0) continue;
	int sum = m->getValue(i-1,j-1) + m->getValue(i,j-1) + m->getValue(i+1,j-1) + m->getValue(i-1,j) + m->getValue(i+1,j) + m->getValue(i-1,j+1) + m->getValue(i,j+1) + m->getValue(i+1,j+1);
	if(sum > 4){
	  m->setValue(i,j,1);
	  unfilled = true;
	}
      }
    }
  }
  
  unpruned = true;
  while(unpruned){
    unpruned = false;
    for(int i = 1; i < frame->width()-1; i++){
      for(int j = 1; j < frame->height()-1; j++){
	if(m->getValue(i,j) < 1) continue;
	int sum = m->getValue(i-1,j-1) + m->getValue(i,j-1) + m->getValue(i+1,j-1) + m->getValue(i-1,j) + m->getValue(i+1,j) + m->getValue(i-1,j+1) + m->getValue(i,j+1) + m->getValue(i+1,j+1);
	if(sum < 3){
	  m->setValue(i,j,0);
	  unpruned = true;
	}
      }
    }
  }

  unfilled = true;
  while(unfilled){
    unfilled = false;
    for(int i = 1; i < frame->width()-1; i++){
      for(int j = 1; j < frame->height()-1; j++){
	if(m->getValue(i,j) > 0) continue;
	int sum = m->getValue(i-1,j-1) + m->getValue(i,j-1) + m->getValue(i+1,j-1) + m->getValue(i-1,j) + m->getValue(i+1,j) + m->getValue(i-1,j+1) + m->getValue(i,j+1) + m->getValue(i+1,j+1);
	if(sum > 5){
	  m->setValue(i,j,1);
	  unfilled = true;
	}
      }
    }
  }
  
  int configChan = chan;
  if(configChan >= m_noiseRemovalThreshold.size()) configChan = 0;
  double radius = m_minPunctaRadius.at(configChan);
  uint32_t sizeThreshold = (uint32_t)(m_noiseRemovalThreshold.at(configChan)*3.14159*radius*radius/(rec->resolutionXY()*rec->resolutionXY()));
  //std::cout << "sizeThreshold = " << sizeThreshold << std::endl;
  std::vector<int> borderX;
  std::vector<int> borderY;
  std::vector<int> clusterX;
  std::vector<int> clusterY;
  Mask* used = m->getCopy();
  for(int i = 0; i < frame->width(); i++){
    for(int j = 0; j < frame->height(); j++){
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
	    int val = used->getValue(di,dj);
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
      clusterX.clear();
      clusterY.clear();
    }
  }
  rec->setSignalMask(chan,m);
  delete used;
  return denseRegionMask;
}

int ImageAnalysisToolkit::findThreshold(ImFrame* frame)
{
  Mask* outMask = findOutliers(frame);
  ImFrame* dFrame = frame->derivative();
  Mask* m = new Mask(frame->width(),frame->height());
  double lowerLimit = frame->mode(outMask);
  double mode = lowerLimit;
  double best = frame->mean(outMask);
  double upperLimit = 2*best;//best + 6*ndim.std(w,z,t,p);
  int nsteps = 10;
  double step = (upperLimit - lowerLimit) / nsteps;
  if(step < 1.0){
    step = 1.0;
    nsteps = (int)(upperLimit - lowerLimit);
  }
  //std::cout << lowerLimit << ", " << upperLimit << ", " << nsteps << ", " << step << std::endl;
  double fom = 0;
  int maxClusters = 0;
  bool finished = false;
  int64_t penalty = (int64_t)(dFrame->median());
  std::cout << "Penalty = " << penalty << std::endl;
  while(!finished){
    finished = true;
    for(int istep = 0; istep < nsteps+1; istep++){
      double globalThreshold = lowerLimit + istep*step;
      //std::cout << globalThreshold << std::endl;
      for(int i = 0; i < frame->width(); i++){
	for(int j = 0; j < frame->height(); j++){
	  int value = frame->getPixel(i,j);
	  if(value > globalThreshold){
	    m->setValue(i,j,1);
	  }
	  else m->setValue(i,j,0);
	}
      }
      bool unpruned = true;
      while(unpruned){
	unpruned = false;
	for(int i = 1; i < frame->width()-1; i++){
	  for(int j = 1; j < frame->height()-1; j++){
	    if(m->getValue(i,j) < 1) continue;
	    int sum = m->getValue(i-1,j-1) + m->getValue(i,j-1) + m->getValue(i+1,j-1) + m->getValue(i-1,j) + m->getValue(i+1,j) + m->getValue(i-1,j+1) + m->getValue(i,j+1) + m->getValue(i+1,j+1);
	    if(sum < 4){
	      m->setValue(i,j,0);
	      unpruned = true;
	    }
	  }
	}
      }
      int64_t avgDerivative = 0;
      int64_t totalBorderSize = 0;
      int64_t avgSigSize = 0;
      int64_t avgSigSize2 = 0;
      int nSigClusters = 0;
      std::vector<int> borderX;
      std::vector<int> borderY;
      Mask* subMask = m->getCopy();
      subMask->multiply(*outMask);
      Mask* borderMask = m->getBorders();
      borderMask->multiply(*outMask);
      int64_t borderSizeThreshold = (int64_t)(3.14159/(0.11*0.11));
      for(int i = 0; i < frame->width(); i++){
	for(int j = 0; j < frame->height(); j++){
	  if(subMask->getValue(i,j) != 1) continue;
	  borderX.push_back(i);
	  borderY.push_back(j);
	  subMask->setValue(i,j,0);
	  int64_t size = 1;
	  int64_t borderDerivative = 0;
	  int64_t borderSize = 0;
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
	    
	    if(borderMask->getValue(bi,bj) > 0){
	      borderDerivative += dFrame->getPixel(bi,bj) - penalty;
	      borderSize++;
	    }
	    
	    for(int di = left; di < right; di++){
	      for(int dj = top; dj < bottom; dj++){
		int val = m->getValue(di,dj);
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
	  
	  if(size > borderSizeThreshold){
	    avgDerivative += borderDerivative*borderSize;
	    totalBorderSize += borderSize;
	  }
	  
	  avgSigSize += size;
	  avgSigSize2 += size*size;
	  nSigClusters++;
	}
      }

      int64_t avgSize = 0;
      int64_t avgSize2 = 0;
      int nBkgClusters = 0;
      subMask->copy(*m);
      for(int i = 0; i < frame->width(); i++){
	for(int j = 0; j < frame->height(); j++){
	  if(subMask->getValue(i,j) != 0) continue;
	  borderX.push_back(i);
	  borderY.push_back(j);
	  subMask->setValue(i,j,1);
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
	  avgSize += size;
	  avgSize2 += size*size;
	  nBkgClusters++;
	}
      }

      
      //double ifom = ((double)avgSigSize2)/avgSigSize * avgSize2/avgSize;
      //if(nSigClusters > 0) ifom = ifom/nSigClusters;
      double ifom = ((double)avgSigSize2)*avgSize/avgSigSize;
      //if(((double)avgSigSize)/(frame->height()*frame->width()) < 0.01) break;
      if(nBkgClusters > 0) ifom = ifom/nBkgClusters;
      else ifom = 0;
      ifom *= avgDerivative;
      //if(nBkgClusters > 0) ifom = ifom/nBkgClusters;
      //else ifom = 0;
      //if(nSigClusters > 0) ifom = ifom/nSigClusters;
      //else ifom = 0;
      if(totalBorderSize > 0) ifom = ifom/totalBorderSize;
      /*
      if(nSigClusters >= maxClusters){
	fom = ifom;
	maxClusters = nSigClusters;
	best = globalThreshold;
	finished = false;
      }
      */
      if(ifom > fom){
	fom = ifom;
	best = globalThreshold;
	finished = false;
      }
      delete subMask;
      delete borderMask;
      //std::cout << globalThreshold << ", " << avgSigSize2 << ", " << avgSigSize << ", " << nSigClusters << ", " << avgSize << ", " << nBkgClusters << ":\t" << ifom << std::endl;
      std::cout << globalThreshold  << ", " << avgDerivative << ", " << totalBorderSize << ", " << nSigClusters << ", " << avgSize << ", " << nBkgClusters <<  ":\t" << ifom << std::endl;
    }
    finished = false;
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
  delete outMask;
  delete dFrame;
  //nia::nout << "Best threshold: " << best << "\n";
  return (int)best;
}

int ImageAnalysisToolkit::findThreshold(ImFrame* frame, Mask* sigMask, Mask* outMask)
{
  ImFrame* dFrame = frame->derivative();
  Mask* m = new Mask(frame->width(),frame->height());
  double lowerLimit = frame->mode(outMask);
  double mode = lowerLimit;
  double best = frame->mean(outMask);
  double upperLimit = 2*best;//best + 6*ndim.std(w,z,t,p);
  int nsteps = 10;
  double step = (upperLimit - lowerLimit) / nsteps;
  if(step < 1.0){
    step = 1.0;
    nsteps = (int)(upperLimit - lowerLimit);
  }
  //std::cout << lowerLimit << ", " << upperLimit << ", " << nsteps << ", " << step << std::endl;
  double fom = 0;
  int maxClusters = 0;
  bool finished = false;
  int64_t penalty = (int64_t)(dFrame->median() * 2);
  std::cout << "Penalty = " << penalty << std::endl;
  while(!finished){
    finished = true;
    for(int istep = 0; istep < nsteps+1; istep++){
      double globalThreshold = lowerLimit + istep*step;
      //std::cout << globalThreshold << std::endl;
      for(int i = 0; i < frame->width(); i++){
	for(int j = 0; j < frame->height(); j++){
	  int value = frame->getPixel(i,j);
	  if(value > globalThreshold){
	    m->setValue(i,j,1);
	  }
	  else m->setValue(i,j,0);
	}
      }
      bool unpruned = true;
      while(unpruned){
	unpruned = false;
	for(int i = 1; i < frame->width()-1; i++){
	  for(int j = 1; j < frame->height()-1; j++){
	    if(m->getValue(i,j) < 1) continue;
	    int sum = m->getValue(i-1,j-1) + m->getValue(i,j-1) + m->getValue(i+1,j-1) + m->getValue(i-1,j) + m->getValue(i+1,j) + m->getValue(i-1,j+1) + m->getValue(i,j+1) + m->getValue(i+1,j+1);
	    if(sum < 4){
	      m->setValue(i,j,0);
	      unpruned = true;
	    }
	  }
	}
      }
      int64_t avgDerivative = 0;
      int64_t totalBorderSize = 0;
      int64_t avgSigSize = 0;
      int64_t avgSigSize2 = 0;
      int nSigClusters = 0;
      std::vector<int> borderX;
      std::vector<int> borderY;
      Mask* subMask = m->getCopy();
      subMask->multiply(*outMask);
      Mask* borderMask = m->getBorders();
      borderMask->multiply(*outMask);
      borderMask->multiply(*sigMask);
      int64_t borderSizeThreshold = (int64_t)(3.14159/(0.11*0.11));
      for(int i = 0; i < frame->width(); i++){
	for(int j = 0; j < frame->height(); j++){
	  if(subMask->getValue(i,j) != 1) continue;
	  bool bordersPrev = false;
	  borderX.push_back(i);
	  borderY.push_back(j);
	  subMask->setValue(i,j,0);
	  int64_t size = 1;
	  int64_t borderDerivative = 0;
	  int64_t borderSize = 0;
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
	    
	    if(borderMask->getValue(bi,bj) > 0){
	      borderDerivative += dFrame->getPixel(bi,bj) - penalty;
	      borderSize++;
	    }
	    
	    for(int di = left; di < right; di++){
	      for(int dj = top; dj < bottom; dj++){
		if(sigMask->getValue(di,dj) != 1) bordersPrev = true;
		int val = m->getValue(di,dj);
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
	  
	  if(bordersPrev){//size > borderSizeThreshold){
	    avgDerivative += borderDerivative*borderSize;
	    totalBorderSize += borderSize;
	  }
	  
	  avgSigSize += size;
	  avgSigSize2 += size*size;
	  nSigClusters++;
	}
      }

      int64_t avgSize = 0;
      int64_t avgSize2 = 0;
      int nBkgClusters = 0;
      subMask->copy(*m);
      for(int i = 0; i < frame->width(); i++){
	for(int j = 0; j < frame->height(); j++){
	  if(subMask->getValue(i,j) != 0) continue;
	  borderX.push_back(i);
	  borderY.push_back(j);
	  subMask->setValue(i,j,1);
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
	  avgSize += size;
	  avgSize2 += size*size;
	  nBkgClusters++;
	}
      }

      
      //double ifom = ((double)avgSigSize2)/avgSigSize * avgSize2/avgSize;
      //if(nSigClusters > 0) ifom = ifom/nSigClusters;
      double ifom = ((double)avgSigSize2)*avgSize/avgSigSize;
      //if(((double)avgSigSize)/(frame->height()*frame->width()) < 0.01) break;
      if(nBkgClusters > 0) ifom = ifom/nBkgClusters;
      else ifom = 0;
      ifom = avgDerivative;
      //if(nBkgClusters > 0) ifom = ifom/nBkgClusters;
      //else ifom = 0;
      //if(nSigClusters > 0) ifom = ifom/nSigClusters;
      //else ifom = 0;
      if(totalBorderSize > 0) ifom = ifom/totalBorderSize;
      /*
      if(nSigClusters >= maxClusters){
	fom = ifom;
	maxClusters = nSigClusters;
	best = globalThreshold;
	finished = false;
      }
      */
      if(ifom > fom){
	fom = ifom;
	best = globalThreshold;
	finished = false;
      }
      delete subMask;
      delete borderMask;
      //std::cout << globalThreshold << ", " << avgSigSize2 << ", " << avgSigSize << ", " << nSigClusters << ", " << avgSize << ", " << nBkgClusters << ":\t" << ifom << std::endl;
      std::cout << globalThreshold  << ", " << avgDerivative << ", " << totalBorderSize << ", " << nSigClusters << ", " << avgSize << ", " << nBkgClusters <<  ":\t" << ifom << std::endl;
    }
    finished = false;
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
  delete sigMask;
  delete outMask;
  delete dFrame;
  if(fom <= 0) return -1;
  //nia::nout << "Best threshold: " << best << "\n";
  return (int)best;
}

void ImageAnalysisToolkit::findPuncta(ImFrame* frame, ImRecord* rec, int chan)
{
  findSaturatedPuncta(frame,rec,chan);
  
  Mask* m = rec->getSignalMask(chan);
  bool delFlag = false;
  if(!m){
    m = new Mask(frame->width(),frame->height(),1);
    delFlag = true;
  }
  std::vector<LocalizedObject::Point> localMaxima;
  std::vector<LocalizedObject::Point> upperLeft;
  std::vector<LocalizedObject::Point> lowerRight;
  std::vector<int> intensities;
  int configChan = chan;
  if(configChan >= m_localWindow.size()) configChan = 0;
  int minSignal = (int)std::min(m_localWindow.at(configChan) / (rec->resolutionXY()*rec->resolutionXY()),(double)m->sum());
  int initialWindowSize = (int)(sqrt(minSignal) / 2);
  int lwi = (int)(0.5 / rec->resolutionXY());
  Mask* bkgMask = m->inverse();
  double bkgThreshold = frame->median(0,frame->width(),0,frame->height(),bkgMask) + m_backgroundThreshold.at(configChan) * frame->std(0,frame->width(),0,frame->height(),bkgMask);
  delete bkgMask;
  for(int i = 0; i < frame->width(); i++){
    for(int j = 0; j < frame->height(); j++){
      if(m->getValue(i,j) < 1){
	continue;
      }
      int x1 = std::max(i-lwi,0);
      int x2 = std::min(frame->width(),i+lwi);
      int y1 = std::max(j-lwi,0);
      int y2 = std::min(frame->height(),j+lwi);
      
      int val = frame->getPixel(i,j);
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
	x2 = std::min(frame->width(),x2+lwi);
	y1 = std::max(y1-lwi,0);
	y2 = std::min(frame->height(),y2+lwi);
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
  for(int i = 0; i < intensities.size(); i++){
    int max = intensities.at(i);
    int imax = i;
    for(int j = i+1; j < intensities.size(); j++){
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

  int nRemoved = -1;
  int nNew = 0;
  int count = 0;
  Mask* cMask = new Mask(frame->width(),frame->height());
  Mask* used = m->getCopy();
  used->subtract(*(rec->getPunctaMask(chan,false)));
  double radius = m_minPunctaRadius.at(configChan);
  double punctaAreaThreshold = 3.14159*radius*radius/(rec->resolutionXY()*rec->resolutionXY());
  int retryThreshold = (int)(m_reclusterThreshold.at(configChan)*punctaAreaThreshold) + 1;
  while(nNew > nRemoved && count < m_punctaFindingIterations){
    //nNew = 0;
    nRemoved = 0;
    int nPuncta = rec->nPuncta(chan);
    for(int j = nPuncta-1; j >= nPuncta-nNew; j--){
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
      int Imax = frame->getPixel(lm.x,lm.y);
      double localMedian;
      double localStd;
      frame->getMedianStd(ul.x,lr.x,ul.y,lr.y,used,m_saturationThreshold+2,localMedian,localStd);
      double minThreshold = std::min(localMedian + (Imax - localMedian)/2, localMedian + m_floorThreshold.at(configChan)*localStd);
      double localThreshold = localMedian + m_peakThreshold.at(configChan)*localStd;
      double minIntensity = punctaAreaThreshold*(localMedian + m_floorThreshold.at(configChan)*localStd);
      //if(minIntensity < 1.0) nia::nout << "Intensity threshold ridiculously small: " << minIntensity << "\n";
      if(Imax < localThreshold) continue;
      if(Imax < bkgThreshold) continue;
      localThreshold = Imax;
      Cluster* c = new Cluster();
      std::vector<int> borderX;
      std::vector<int> borderY;
      std::vector<int> borderVal;
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
      int size = c->size();
      int x1 = std::max(1,lm.x - size);
      int x2 = std::min(frame->width()-1,lm.x + size);
      int y1 = std::max(1,lm.y - size);
      int y2 = std::min(frame->height()-1,lm.y + size);
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
  if(delFlag) delete m;

  resolveOverlaps(frame,rec,chan);
  nia::nout << "Found " << rec->nPuncta(chan) << " puncta in channel " << chan << " after " << count << " iterations." << "\n";
 }

void ImageAnalysisToolkit::findSaturatedPuncta(ImFrame* frame, ImRecord* rec, int chan)
{
  //nia::nout << "Find saturated puncta" << "\n";
  Mask* m = rec->getSignalMask(chan);
  bool delFlag = false;
  if(!m){
    m = new Mask(frame->width(),frame->height(),1);
    delFlag = true;
  }
  Mask* used = m->getCopy();
  Mask* cMask = new Mask(frame->width(),frame->height());
  std::vector<int> borderSatX;
  std::vector<int> borderSatY;
  std::vector<int> borderUnsatX;
  std::vector<int> borderUnsatY;
  std::vector<int> borderVal;
  int configChan = chan;
  if(configChan >= m_localWindow.size()) configChan = 0;
  int minMinSignal = (int)std::min(m_localWindow.at(configChan) / (rec->resolutionXY()*rec->resolutionXY()),(double)m->sum());
  int lwi = (int)(0.5 / rec->resolutionXY());
  for(int i = 0; i < frame->width(); i++){
    for(int j = 0; j < frame->height(); j++){
      int value = frame->getPixel(i,j);
      if(used->getValue(i,j)*value < m_saturationThreshold) continue;
      Cluster* c = new Cluster();
      c->setPeakIntensity(value);
      int totI = value;
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
      int x2 = std::min(frame->width(),center.x+lwi);
      int y1 = std::max(center.y-lwi,0);
      int y2 = std::min(frame->height(),center.y+lwi);
      int nSignal = m->sum(x1,x2,y1,y2);
      int minSignal = std::max(minMinSignal,4*c->size());
      while(nSignal < minSignal){
	x1 = std::max(x1-lwi,0);
	x2 = std::min(frame->width(),x2+lwi);
	y1 = std::max(y1-lwi,0);
	y2 = std::min(frame->height(),y2+lwi);
	nSignal = m->sum(x1,x2,y1,y2);
      }
      int Imax = m_saturationThreshold;
      double localMedian;// = frame->median(x1,x2,y1,y2,used);
      double localStd;// = frame->std(x1,x2,y1,y2,used);
      frame->getMedianStd(x1,x2,y1,y2,used,m_saturationThreshold+2,localMedian,localStd);
      double localThreshold = std::min(localMedian + (Imax - localMedian)/2, localMedian + m_floorThreshold.at(configChan)*localStd);
      while(borderUnsatX.size() > 0){
	int upperLimit = Imax;
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
	    int pixelValue = frame->getPixel(x,y);
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
	for(int x = x1+1; x < x2-1; x++){
	  for(int y = y1+1; y < y2-1; y++){
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
      for(int x = x1+1; x < x2-1; x++){
	for(int y = y1+1; y < y2-1; y++){
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
  if(delFlag) delete m;
}

void ImageAnalysisToolkit::resolveOverlaps(ImFrame* frame, ImRecord* rec, int chan)
{
  //nia::nout << "Resolve overlaps" << "\n";
  std::vector<Cluster*> clusters = rec->puncta(chan);
  std::vector<Cluster*> satClusters;
  //std::vector<Cluster*> unsatClusters;
  std::vector<Cluster*> newClusters;
  std::vector<Gaus2D> fs;
  Mask* m = new Mask(frame->width(),frame->height());//rec->getPunctaMask(chan,false);
  for(std::vector<Cluster*>::iterator it = clusters.begin(); it != clusters.end(); it++){
    int imax = (*it)->peak();
    if(imax > m_saturationThreshold){
      satClusters.push_back((*it)->getCopy());
    }
    else{
      //unsatClusters.push_back(*it);
      LocalizedObject::Point peak = (*it)->getPoint(0);
      LocalizedObject::Point trough = (*it)->getPoint((*it)->size()-1);
      int imin = frame->getPixel(trough.x,trough.y);
      std::vector<LocalizedObject::Point> pts = (*it)->getPoints();
      for(std::vector<LocalizedObject::Point>::iterator jt = pts.begin(); jt != pts.end(); jt++){
	m->setValue(jt->x,jt->y,1);
	int val = frame->getPixel(jt->x,jt->y);
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
  //nia::nout << "Original: (" << unsatClusters[0]->getPoint(0).x << "," << unsatClusters[0]->getPoint(0).y << "), " << unsatClusters[0]->size() << "\n";
  //nia::nout << "Original: (" << unsatClusters[1]->getPoint(0).x << "," << unsatClusters[1]->getPoint(0).y << "), " << unsatClusters[1]->size() << "\n";
  //nia::nout << "Original: (" << unsatClusters[2]->getPoint(0).x << "," << unsatClusters[2]->getPoint(0).y << "), " << unsatClusters[2]->size() << "\n";
  
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
  //nia::nout << "New: (" << newClusters[0]->getPoint(0).x << "," << newClusters[0]->getPoint(0).y << "), " << newClusters[0]->size() << "\n";
  //nia::nout << "New: (" << newClusters[1]->getPoint(0).x << "," << newClusters[1]->getPoint(0).y << "), " << newClusters[1]->size() << "\n";
  //nia::nout << "New: (" << newClusters[2]->getPoint(0).x << "," << newClusters[2]->getPoint(0).y << "), " << newClusters[2]->size() << "\n";
  
  m->clear(0,frame->width(),0,frame->height());
  std::vector<int> borderX;
  std::vector<int> borderY;
  for(std::vector<Cluster*>::iterator it = newClusters.begin(); it != newClusters.end(); it++){
    std::vector<LocalizedObject::Point> pts = (*it)->getPoints();
    for(std::vector<LocalizedObject::Point>::iterator jt = pts.begin(); jt != pts.end(); jt++) m->setValue(jt->x,jt->y,1);
    LocalizedObject::Point peak = *(pts.begin());
    (*it)->clearPoints();
    borderX.push_back(peak.x);
    borderY.push_back(peak.y);
    while(borderX.size() > 0){
      int nborder = borderX.size();
      for(int b = 0; b < nborder; b++){
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
  int configChan = chan;
  if(configChan >= m_minPunctaRadius.size()) configChan = 0;
  double punctaAreaThreshold = 3.14159*m_minPunctaRadius.at(configChan)*m_minPunctaRadius.at(configChan)/(rec->resolutionXY()*rec->resolutionXY());
  int areaThreshold = (int)(m_reclusterThreshold.at(configChan)*punctaAreaThreshold);
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
    sc->clearSynapses();
    std::vector<int> chan = sc->channels();
    int nchans = sc->nChannels();
    int* np = new int[nchans];
    int* pi = new int[nchans];
    int nCombos = 1;
    for(int i = 0; i < nchans; i++){
      np[i] = rec->nPuncta(chan.at(i));
      nCombos *= np[i];
      pi[i] = 0;
    }
    if(nCombos == 0){
      delete[] np;
      delete[] pi;
      continue;
      nia::nout << "Found " << nSynapses << " synapses of type " << icol << ": " << sc->description() << "\n";
    }
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
  int nchan = m_localWindow.size();
  buf[6] = (char)nchan;
  uint32_t offset = 7;
  for(int i = 0; i < nchan; i++){
    NiaUtils::writeDoubleToBuffer(buf,offset,m_localWindow.at(i));
    NiaUtils::writeDoubleToBuffer(buf,offset+4,m_backgroundThreshold.at(i));
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
  m_master = (int)buf[0];
  if((int)buf[1] == 0) m_mode = OVERRIDE;
  else m_mode = OR;
  m_postChan = (int)buf[2];
  m_punctaFindingIterations = (int)buf[3];
  m_saturationThreshold = NiaUtils::convertToShort(buf[4],buf[5]);
  int nchan = (int)buf[6];
  fin.read(buf,nchan*28);
  uint32_t offset = 0;
  m_localWindow.clear();
  m_backgroundThreshold.clear();
  m_minPunctaRadius.clear();
  m_reclusterThreshold.clear();
  m_noiseRemovalThreshold.clear();
  m_peakThreshold.clear();
  m_floorThreshold.clear();
  for(int i = 0; i < nchan; i++){
    m_localWindow.push_back(NiaUtils::convertToDouble(buf[offset],buf[offset+1],buf[offset+2],buf[offset+3]));
    offset += 4;
    m_backgroundThreshold.push_back(NiaUtils::convertToDouble(buf[offset],buf[offset+1],buf[offset+2],buf[offset+3]));
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

int ImageAnalysisToolkit::getBitDepth()
{
  int bd;
  int st = m_saturationThreshold + 2;
  for(bd = 0; st > 1; bd++) st = st / 2;
  return bd;
}
