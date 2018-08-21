#include "ImRecord.hpp"
#include "FileConverter.hpp"
#include <iostream>
#include <fstream>
#include <boost/lexical_cast.hpp>
//----------Linux/Mac-----------
#include <sys/time.h>
#include <sys/resource.h>
//----------Linux/Mac-----------

typedef struct comparison_result{
  double n_false_positive = 0.0;
  double n_false_negative = 0.0;
  double n_true_positive = 0.0;
  double n_true_negative = 0.0;
  double n_merge = 0.0;
  double n_split = 0.0;
  int area = 0;
} Result;

Result add(Result res1, Result res2)
{
  Result retVal;
  //std::cout << res1.n_false_positive << "\t" << res2.n_false_positive << std::endl;
  //std::cout << res1.n_false_negative << "\t" << res2.n_false_negative << std::endl;
  //std::cout << res1.n_true_positive << "\t" << res2.n_true_positive << std::endl;
  //std::cout << res1.n_true_negative << "\t" << res2.n_true_negative << std::endl;
  retVal.n_false_positive = res1.n_false_positive + res2.n_false_positive;
  retVal.n_false_negative = res1.n_false_negative + res2.n_false_negative;
  retVal.n_true_positive = res1.n_true_positive + res2.n_true_positive;
  retVal.n_true_negative = res1.n_true_negative + res2.n_true_negative;
  retVal.n_merge = res1.n_merge + res2.n_merge;
  retVal.n_split = res1.n_split + res2.n_split;
  retVal.area = res1.area + res2.area;
  return retVal;
}

std::vector<Result> compareSynapses(ImRecord* truth, ImRecord* estimate, ImStack* stack, int col, int postChan)
{
  SynapseCollection* true_synapses = truth->getSynapseCollection(col);
  SynapseCollection* est_synapses = estimate->getSynapseCollection(col);
  std::vector<Mask*> true_peaks;
  std::vector<Mask*> est_peaks;
  std::vector<Mask*> true_puncta;
  std::vector<Mask*> est_puncta;
  int nchan = true_synapses->nChannels();
  for(int i = 0; i < nchan; i++){
    int chan = true_synapses->getChannel(i);
    true_peaks.push_back(new Mask(truth->imWidth(),truth->imHeight()));
    est_peaks.push_back(new Mask(truth->imWidth(),truth->imHeight()));
    true_puncta.push_back(truth->getSynapticPunctaMaskFromCollection(col,chan,false));
    est_puncta.push_back(estimate->getSynapticPunctaMaskFromCollection(col,chan,false));
  }
  for(int i = 0; i < true_synapses->nSynapses(); i++){
    Synapse* s = true_synapses->getSynapse(i);
    for(int j = 0; j < true_peaks.size(); j++){
      std::vector<LocalizedObject::Point>::iterator peakPoint = s->getPunctum(j)->begin();
      true_peaks[j]->setValue(peakPoint->x,peakPoint->y,i+1);
    }
  }
  for(int i = 0; i < est_synapses->nSynapses(); i++){
    Synapse* s = est_synapses->getSynapse(i);
    for(int j = 0; j < est_peaks.size(); j++){
      std::vector<LocalizedObject::Point>::iterator peakPoint = s->getPunctum(j)->begin();
      est_peaks[j]->setValue(peakPoint->x,peakPoint->y,1);
    }
  }
  
  std::vector<int> thresholds;
  for(int j = 0; j < nchan; j++){
    int Imin = 65536;
    int chan = true_synapses->getChannel(j);
    ImFrame* frame = stack->frame(chan,0);
    for(int i = 0; i < truth->nPuncta(chan); i++){
      std::vector<LocalizedObject::Point>::iterator peakPoint = truth->getPunctum(chan,i)->begin();
      int val = frame->getPixel(peakPoint->x,peakPoint->y);
      if(val < Imin) Imin = val;
    }
    for(int i = 0; i < estimate->nPuncta(chan); i++){
      std::vector<LocalizedObject::Point>::iterator peakPoint = estimate->getPunctum(chan,i)->begin();
      int val = frame->getPixel(peakPoint->x,peakPoint->y);
      if(val < Imin) Imin = val;
    }
    thresholds.push_back(Imin);
  }
  
  std::vector<LocalizedObject::Point> localMaxima;
  std::vector<int> starts;
  for(int k = 0; k < nchan; k++){
    starts.push_back(localMaxima.size());
    int chan = true_synapses->getChannel(k);
    ImFrame* frame = stack->frame(chan,0);
    for(int i = 0; i < frame->width(); i++){
      for(int j = 0; j < frame->height(); j++){
	int val = frame->getPixel(i,j);
	if(val < thresholds[k]) continue;
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
      }
    }
  }
  starts.push_back(localMaxima.size());

  std::vector<Result> retval;
  Mask* sigMask = estimate->getSignalMask(postChan);
  for(int i = 0; i < truth->nRegions(); i++){
    int area = 0;
    Region* reg = truth->getRegion(i);
    for(int x = 0; x < truth->imWidth(); x++){
      for(int y = 0; y < truth->imHeight(); y++){
	if(reg->contains(x,y)) area++;
      }
    }
    Result res;
    res.area = area;
    retval.push_back(res);
  }

  int nCombos = 1;
  std::vector<int> currentCombo;
  for(int i = 0; i < nchan; i++){
    nCombos *= (starts[i+1] - starts[i]);
    currentCombo.push_back(starts[i]);
  }
  for(int k = 0; k < nCombos; k++){
    int regIndex = -1;
    for(int i = 0; i <truth->nRegions() && regIndex < 0; i++){
      bool allIn = true;
      Region* reg = truth->getRegion(i);
      for(int j = 0; j < nchan; j++){
	LocalizedObject::Point lm = localMaxima[currentCombo[j]];
	if(!(reg->contains(lm.x,lm.y))) allIn = false;
      }
      if(allIn) regIndex = i;
    }
    if(regIndex < 0) continue;
    LocalizedObject::Point lm = localMaxima[currentCombo[0]];
    int avgX = lm.x;
    int avgY = lm.y;
    bool isTrueSynapse,isEstSynapse;
    int sid = true_peaks[0]->getValue(lm.x,lm.y);
    Synapse* tsyn = NULL;
    if(sid > 0){
      isTrueSynapse = true;
      tsyn = true_synapses->getSynapse(sid-1);
    }
    sid = est_peaks[0]->getValue(lm.x,lm.y);
    Synapse* esyn = NULL;
    if(sid > 0){
      isTrueSynapse = true;
      esyn = true_synapses->getSynapse(sid-1);
    }
    bool isTrueSynapticPuncta = (true_puncta[0]->getValue(lm.x,lm.y) > 0);
    bool isEstSynapticPuncta = (est_puncta[0]->getValue(lm.x,lm.y) > 0);
    for(int j = 1; j < nchan; j++){
      lm = localMaxima[currentCombo[j]];
      avgX += lm.x;
      avgY += lm.y;
      std::vector<LocalizedObject::Point>::iterator peakPoint = tsyn->getPunctum(j)->begin();
      if(lm.x != peakPoint->x || lm.y != peakPoint->y) isTrueSynapse = false;
      peakPoint = esyn->getPunctum(j)->begin();
      if(lm.x != peakPoint->x || lm.y != peakPoint->y) isEstSynapse = false;
      isTrueSynapticPuncta = isTrueSynapticPuncta && (true_puncta[j]->getValue(lm.x,lm.y) > 0);
      isEstSynapticPuncta = isEstSynapticPuncta && (est_puncta[j]->getValue(lm.x,lm.y) > 0);
    }
    avgX = avgX / nchan;
    avgY = avgY / nchan;
    bool nearby = true;
    for(int j = 0; j < nchan; j++){
      lm = localMaxima[currentCombo[j]];
      if(fabs(lm.x - avgX)/truth->resolutionXY() > 1.0 || fabs(lm.y - avgY)/truth->resolutionXY() > 1.0) nearby = false;
    }
    if(isTrueSynapse){
      if(isEstSynapse) retval[regIndex].n_true_positive += 1;
      else if(isEstSynapticPuncta) retval[regIndex].n_merge += 1;
      else retval[regIndex].n_false_negative += 1;
    }
    else if(isTrueSynapticPuncta){
      if(isEstSynapse) retval[regIndex].n_split += 1;
    }
    else{
      if(isEstSynapse) retval[regIndex].n_false_positive += 1;
      else if(nearby) retval[regIndex].n_true_negative += 1;
    }
  }
  for(int i = 0; i < nchan; i++){
    delete true_peaks[i];
    delete est_peaks[i];
    delete true_puncta[i];
    delete est_puncta[i];
  }
  return retval;
}

std::vector<Result> compare(ImRecord* truth, ImRecord* estimate, ImFrame* frame, int chan, int postChan)
{
  Mask* true_puncta_mask = truth->getPunctaMask(chan);
  Mask* est_puncta_mask = estimate->getPunctaMask(chan);
  Mask* true_peaks_mask = new Mask(frame->width(),frame->height());
  Mask* est_peaks_mask = new Mask(frame->width(),frame->height());
  for(int i = 0; i < truth->nPuncta(chan); i++){
    std::vector<LocalizedObject::Point>::iterator peakPoint = truth->getPunctum(chan,i)->begin();
    true_peaks_mask->setValue(peakPoint->x,peakPoint->y,1);
  }
  for(int i = 0; i < estimate->nPuncta(chan); i++){
    std::vector<LocalizedObject::Point>::iterator peakPoint = estimate->getPunctum(chan,i)->begin();
    est_peaks_mask->setValue(peakPoint->x,peakPoint->y,1);
  }
  std::vector<LocalizedObject::Point> localMaxima;
  for(int i = 0; i < frame->width(); i++){
    for(int j = 0; j < frame->height(); j++){
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
    }
  }
  std::vector<Result> retval;
  Mask* sigMask = estimate->getSignalMask(postChan);
  for(int i = 0; i < truth->nRegions(); i++){
    int area = 0;
    Region* reg = truth->getRegion(i);
    for(int x = 0; x < frame->width(); x++){
      for(int y = 0; y < frame->height(); y++){
	if(reg->contains(x,y)) area++;
      }
    }
    Result res;
    res.area = area;
    retval.push_back(res);
  }
  int minPunctaBrightness = 65536;
  for(std::vector<LocalizedObject::Point>::iterator lm = localMaxima.begin(); lm != localMaxima.end(); lm++){
    if(true_peaks_mask->getValue(lm->x,lm->y) > 0 || est_peaks_mask->getValue(lm->x,lm->y) > 0){
      int val = frame->getPixel(lm->x,lm->y);
      if(val < minPunctaBrightness) minPunctaBrightness = val;
    }
  }
  minPunctaBrightness--;
  for(std::vector<LocalizedObject::Point>::iterator lm = localMaxima.begin(); lm != localMaxima.end(); lm++){
    int regIndex = -1;
    for(int i = 0; i <truth->nRegions() && regIndex < 0; i++){
      if(truth->getRegion(i)->contains(lm->x,lm->y)) regIndex = i;
    }
    if(regIndex < 0) continue;
    if(true_peaks_mask->getValue(lm->x,lm->y) > 0){
      if(est_peaks_mask->getValue(lm->x,lm->y) > 0) retval[regIndex].n_true_positive += 1;
      else if(est_puncta_mask->getValue(lm->x,lm->y) > 0) retval[regIndex].n_merge += 1;
      else retval[regIndex].n_false_negative += 1;
    }
    else if(true_puncta_mask->getValue(lm->x,lm->y) > 0){
      if(est_peaks_mask->getValue(lm->x,lm->y) > 0) retval[regIndex].n_split += 1;
    }
    else{
      if(est_peaks_mask->getValue(lm->x,lm->y) > 0) retval[regIndex].n_false_positive += 1;
      else if(frame->getPixel(lm->x,lm->y) > minPunctaBrightness) retval[regIndex].n_true_negative += 1;
    }
  }
  delete true_peaks_mask;
  delete est_peaks_mask;
  delete true_puncta_mask;
  delete est_puncta_mask;
  return retval;
}

int main(int argc, char** argv)
{

  //----------Linux/Mac-----------
  struct rlimit lim;
  lim.rlim_cur = RLIM_INFINITY;
  lim.rlim_max = RLIM_INFINITY;
  setrlimit(RLIMIT_CPU,&lim);
  //----------Linux/Mac-----------
  
  std::vector<ImRecord*> true_recs;
  std::vector< std::vector<ImRecord*>* > est_recs;
  FileManager phil;
  ImageAnalysisToolkit kit;
  std::ifstream fin(argv[3]);
  if(!(fin.is_open())){
    return 1;
  }
  std::string line;
  std::string basedir = "";
  boost::char_separator<char> sep(":");
  int index = -1;
  int index2 = -1;
  std::vector< std::vector<Result> > results;
  //std::vector<Result> totals;
  ImSeries* data = NULL;
  while(getline(fin,line)){
    if(line.find("data directory") == 0){
      getline(fin,basedir);
      continue;
    }
    if(line.find("new image") == 0){
      est_recs.push_back(new std::vector<ImRecord*>());
      index++;
      index2 = -1;
      getline(fin,line);
      std::string fname = basedir + line;
      phil.clearInputFiles();
      phil.addInputFile(FileConverter::read(&phil,&kit,&true_recs,fname,1));
      if(index > 0){
	delete est_recs[index-1];
	delete true_recs[index-1];
	delete data;
      }
      phil.reset();
      data = phil.loadNext();
    }
    else{
      std::string fname = basedir + line;
      FileConverter::read(&phil,&kit,est_recs[index],fname,1);
      index2++;
      std::vector<Result> res;
      if(argv[1][0] == 'p'){
	int chan = boost::lexical_cast<int>(std::string(argv[2]));
	res = compare(true_recs[index],est_recs[index]->at(index2),data->fourLocation(0,0)->frame(chan,0),chan,kit.postChan());
      }
      else if(argv[1][0] == 's'){
	int col = boost::lexical_cast<int>(std::string(argv[2]));
	res = compareSynapses(true_recs[index],est_recs[index]->at(index2),data->fourLocation(0,0),col,kit.postChan());
      }
      else return 1;
      if(index == 0) results.push_back(res);
      else{
	for(int k = 0; k < res.size(); k++) results[index2].push_back(res[k]);
      }
      delete est_recs[index]->at(index2);
    }
  }
  fin.close();
  /*std::vector< std::vector<Result> > results;
  phil.reset();
  ImSeries* data = phil.loadNext();
  for(int i = 0; i < est_recs[0]->size(); i++){
    std::vector<Result> res = compare(true_recs[0],est_recs[0]->at(i),data->fourLocation(0,0)->frame(0,0),0);
    results.push_back(res);
  }
  for(int i = 1; i < true_recs.size(); i++){
    delete data;
    data = phil.loadNext();
    for(int j = 0; j < est_recs[i]->size(); j++){
      std::vector<Result> res = compare(true_recs[i],est_recs[i]->at(j),data->fourLocation(0,0)->frame(0,0),0);
      for(int k = 0; k < res.size(); k++) results[j].push_back(res[k]);
    }
  }
  */
  std::vector<Result> totals;
  for(int i = 0; i < results.size(); i++){
    Result tot = results[i][0];
    for(int j = 1; j < results[i].size(); j++) tot = add(tot, results[i][j]);
    totals.push_back(tot);
  }
  std::ofstream fout(argv[4]);
  for(int i = 0; i < totals.size(); i++){
    fout << totals[i].n_true_positive/(totals[i].n_true_positive + totals[i].n_merge + totals[i].n_false_negative) << ",";
    fout << totals[i].n_false_positive/(totals[i].n_false_positive + totals[i].n_true_negative) << ",";
    fout << totals[i].n_merge/(totals[i].n_true_positive + totals[i].n_merge + totals[i].n_false_negative) << ",";
    fout << totals[i].n_split/(totals[i].n_true_positive + totals[i].n_merge + totals[i].n_false_negative) << "\n";
  }
  fout.close();
}
