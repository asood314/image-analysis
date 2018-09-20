#include "NiaBuffer.hpp"
#include "ImRecord.hpp"
#include "FileConverter.hpp"
#include <iostream>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
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

typedef struct things_struct{
  ImFrame* frame;
  std::vector<Mask*> true_peaks;
  std::vector<Mask*> true_puncta;
  std::vector<LocalizedObject::Point> localMaxima;
  std::vector<int> regIndices;
  std::vector<int> starts;
  int nCombos;
  int col;
  int intype;
  std::vector< std::vector<Result> > retval;
} MyStuff;
  

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

void runPoints(SynapseCollection* true_synapses, MyStuff* stuff, ImRecord* estimate, std::vector<std::string>* scan_files, ImageAnalysisToolkit* iat, int offset)
{
  int nchan = true_synapses->nChannels();
  int chan1 = true_synapses->getChannel(1);
  FileManager fm;
  std::vector<int> currentCombo;
  for(int i = 0; i < nchan; i++) currentCombo.push_back(stuff->starts[i]);
  std::vector<Mask*> est_peaks,est_puncta;
  for(int i = 0; i < nchan; i++) est_peaks.push_back(new Mask(estimate->imWidth(),estimate->imHeight()));
  for(int s2 = 0; s2 < scan_files->size(); s2++){
    std::vector<ImRecord*> inrec;
    if(stuff->intype < 0){
      estimate->loadMetaMorphTraces(scan_files->at(s2),chan1,stuff->frame,true);
    }
    else if(stuff->intype > 0){
      estimate->loadPunctaAnalyzerPuncta(scan_files->at(s2),chan1,stuff->frame,true);
    }
    else{
      FileConverter::read(&fm,iat,&inrec,scan_files->at(s2),1);
      std::vector<Cluster*> puncs = inrec[0]->puncta(chan1);
      estimate->clearPuncta(chan1);
      for(std::vector<Cluster*>::iterator clit = puncs.begin(); clit != puncs.end(); clit++) estimate->addPunctum(chan1,(*clit)->getCopy());
    }
    //estimate->sortPuncta(chan1,stuff->frame);
    iat->findSynapses(estimate);
    SynapseCollection* est_synapses = estimate->getSynapseCollection(stuff->col);
    est_puncta.push_back(estimate->getSynapticPunctaMaskFromCollection(stuff->col,0,false));
    est_puncta.push_back(estimate->getSynapticPunctaMaskFromCollection(stuff->col,1,false));
    est_peaks[0]->clear(0,estimate->imWidth(),0,estimate->imHeight());
    est_peaks[1]->clear(0,estimate->imWidth(),0,estimate->imHeight());
    for(int i = 0; i < est_synapses->nSynapses(); i++){
      Synapse* s = est_synapses->getSynapse(i);
      for(int j = 0; j < est_peaks.size(); j++){
	std::vector<LocalizedObject::Point>::iterator peakPoint = s->getPunctum(j)->begin();
	est_peaks[j]->setValue(peakPoint->x,peakPoint->y,i+1);
      }
    }
    for(int i = 0; i < nchan; i++) currentCombo[i] = stuff->starts[i];
    for(int k = 0; k < stuff->nCombos; k++){
      int regIndex = stuff->regIndices[k];
      if(regIndex < 0){
	currentCombo[0]++;
	for(int i = 1; currentCombo[i-1] == stuff->starts[i] && i < nchan; i++){
	  currentCombo[i-1] = stuff->starts[i-1];
	  currentCombo[i] = currentCombo[i] + 1;
	}
	continue;
      }
      LocalizedObject::Point lm = stuff->localMaxima[currentCombo[0]];
      bool isTrueSynapse = false;
      bool isEstSynapse = false;
      int sid = stuff->true_peaks[0]->getValue(lm.x,lm.y);
      Synapse* tsyn = NULL;
      if(sid > 0){
	isTrueSynapse = true;
	tsyn = true_synapses->getSynapse(sid-1);
	//std::cout << currentCombo[0] << std::endl;
      }
      sid = est_peaks[0]->getValue(lm.x,lm.y);
      Synapse* esyn = NULL;
      if(sid > 0){
	isEstSynapse = true;
	esyn = est_synapses->getSynapse(sid-1);
      }
      bool isTrueSynapticPuncta = (stuff->true_puncta[0]->getValue(lm.x,lm.y) > 0);
      bool isEstSynapticPuncta = (est_puncta[0]->getValue(lm.x,lm.y) > 0);
      for(int j = 1; j < nchan; j++){
	lm = stuff->localMaxima[currentCombo[j]];
	if(tsyn){
	  std::vector<LocalizedObject::Point>::iterator peakPoint = tsyn->getPunctum(j)->begin();
	  if(lm.x != peakPoint->x || lm.y != peakPoint->y) isTrueSynapse = false;
	  //else std::cout << currentCombo[j] << std::endl;
	}
	if(esyn){
	  std::vector<LocalizedObject::Point>::iterator peakPoint = esyn->getPunctum(j)->begin();
	  if(lm.x != peakPoint->x || lm.y != peakPoint->y) isEstSynapse = false;
	}
	isTrueSynapticPuncta = isTrueSynapticPuncta && (stuff->true_puncta[j]->getValue(lm.x,lm.y) > 0);
	isEstSynapticPuncta = isEstSynapticPuncta && (est_puncta[j]->getValue(lm.x,lm.y) > 0);
      }
      if(isTrueSynapse){
	if(isEstSynapse) stuff->retval[offset + s2][regIndex].n_true_positive += 1;
	else if(isEstSynapticPuncta) stuff->retval[offset + s2][regIndex].n_merge += 1;
	else stuff->retval[offset + s2][regIndex].n_false_negative += 1;
      }
      else if(isTrueSynapticPuncta){
	if(isEstSynapse) stuff->retval[offset + s2][regIndex].n_split += 1;
      }
      else{
	if(isEstSynapse) stuff->retval[offset + s2][regIndex].n_false_positive += 1;
	else stuff->retval[offset + s2][regIndex].n_true_negative += 1;
      }
      currentCombo[0]++;
      for(int i = 1; currentCombo[i-1] == stuff->starts[i] && i < nchan; i++){
	currentCombo[i-1] = stuff->starts[i-1];
	currentCombo[i]++;
      }
    }
    estimate->clearPuncta(chan1);
    if(stuff->intype == 0) delete inrec[0];
    delete est_puncta[0];
    delete est_puncta[1];
    est_puncta.clear();
  }
  for(int i = 0; i < nchan; i++) delete est_peaks[i];
}

std::vector< std::vector<Result> > compareSynapses(ImRecord* truth, std::vector<std::string> scan_files, std::vector<std::string> scan_files2, ImStack* stack, ImageAnalysisToolkit* iat, int col, int postChan, int intype)
{
  MyStuff stuff;
  SynapseCollection* true_synapses = truth->getSynapseCollection(col);
  //std::vector<Mask*> true_peaks;
  //std::vector<Mask*> true_puncta;
  //std::vector<Mask*> est_peaks;
  //std::vector<Mask*> est_puncta;
  FileManager fm;
  int nchan = true_synapses->nChannels();
  /*
  for(int j = 0; j < nchan; j++){
    ImFrame* frame = stack->frame(true_synapses->getChannel(j),0);
    for(int i = 0; i < true_synapses->nSynapses(); i++){
      LocalizedObject::Point peak = *(true_synapses->getSynapse(i)->getPunctum(j)->begin());
      bool inROI = false;
      for(int k = 0; k < truth->nRegions(); k++){
	Region* reg = truth->getRegion(k);
	inROI = inROI || (reg->contains(peak.x,peak.y));
      }
      if(!inROI) std::cout << "Synapse " << i << ", Puncta " << j << " not in a region of interest" << std::endl;
      bool isLM = true;
      for(int x = peak.x - 1; x < peak.x + 2; x++){
	for(int y = peak.y - 1; y < peak.y + 2; y++){
	  if(frame->getPixel(x,y) > frame->getPixel(peak.x,peak.y)) isLM = false;
	}
      }
      if(!isLM) std::cout << "Synapse " << i << ", Puncta " << j << " not at local maximum" << std::endl;
    }
  }
  */
  int nsp = scan_files.size();
  for(int i = 0; i < nchan; i++){
    int chan = true_synapses->getChannel(i);
    stuff.true_peaks.push_back(new Mask(truth->imWidth(),truth->imHeight()));
    //est_peaks.push_back(new Mask(truth->imWidth(),truth->imHeight()));
    stuff.true_puncta.push_back(truth->getSynapticPunctaMaskFromCollection(col,chan,false));
  }
  for(int i = 0; i < true_synapses->nSynapses(); i++){
    Synapse* s = true_synapses->getSynapse(i);
    for(int j = 0; j < stuff.true_peaks.size(); j++){
      std::vector<LocalizedObject::Point>::iterator peakPoint = s->getPunctum(j)->begin();
      stuff.true_peaks[j]->setValue(peakPoint->x,peakPoint->y,i+1);
    }
  }

  std::vector<int> thresholds;
  for(int j = 0; j < nchan; j++){
    int Imin = 65536;
    int chan = true_synapses->getChannel(j);
    ImFrame* frame = stack->frame(chan,0);
    for(int i = 0; i < truth->nPuncta(chan); i++){
      std::vector<LocalizedObject::Point>::iterator peakPoint = truth->getPunctum(chan,i)->end()-1;
      int val = frame->getPixel(peakPoint->x,peakPoint->y);
      if(val < Imin) Imin = val;
    }
    thresholds.push_back(0.75*Imin);
  }

  //std::vector<LocalizedObject::Point> localMaxima;
  //std::vector<int> starts;
  for(int k = 0; k < nchan; k++){
    stuff.starts.push_back(stuff.localMaxima.size());
    int chan = true_synapses->getChannel(k);
    ImFrame* frame = stack->frame(chan,0);
    for(int i = 0; i < frame->width(); i++){
      for(int j = 0; j < frame->height(); j++){
	int val = frame->getPixel(i,j);
	//if(val < thresholds[k]) continue;
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
	bool inROI = false;
	for(int k = 0; k < truth->nRegions(); k++){
	  Region* reg = truth->getRegion(k);
	  inROI = inROI || (reg->contains(i,j));
	}
	if(!inROI) continue;

	stuff.localMaxima.push_back(LocalizedObject::Point(i,j));
      }
    }
  }
  stuff.starts.push_back(stuff.localMaxima.size());
  //std::cout << stuff.localMaxima.size() << std::endl;

  //std::vector< std::vector<Result> > retval;
  std::vector<Result> scan_res;
  Mask* sigMask = truth->getSignalMask(postChan);
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
    scan_res.push_back(res);
  }
  for(int i = 0; i < nsp*nsp; i++) stuff.retval.push_back(scan_res);

  stuff.nCombos = 1;
  std::vector<int> currentCombo;
  for(int i = 0; i < nchan; i++){
    stuff.nCombos *= (stuff.starts[i+1] - stuff.starts[i]);
    currentCombo.push_back(stuff.starts[i]);
  }
  //std::cout << stuff.nCombos << std::endl;
  //std::vector<int> regIndices;
  for(int k = 0; k < stuff.nCombos; k++){
    int regIndex = -1;
    for(int i = 0; i < truth->nRegions() && regIndex < 0; i++){
      bool allIn = true;
      Region* reg = truth->getRegion(i);
      for(int j = 0; j < nchan; j++){
	LocalizedObject::Point lm = stuff.localMaxima[currentCombo[j]];
	if(!(reg->contains(lm.x,lm.y))) allIn = false;
      }
      if(allIn) regIndex = i;
    }
    stuff.regIndices.push_back(regIndex);
    currentCombo[0]++;
    for(int i = 1; currentCombo[i-1] == stuff.starts[i] && i < nchan; i++){
      currentCombo[i-1] = stuff.starts[i-1];
      currentCombo[i] = currentCombo[i] + 1;
    }
  }

  stuff.intype = intype;
  stuff.col = col;
  stuff.frame = stack->frame(true_synapses->getChannel(1),0);
  for(int s = 0; s < nsp; s++){
    std::cout << s*nsp << "/" << nsp*nsp << " configurations completed." << std::endl;
    ImRecord* estimate;
    if(intype < 0){
      estimate = truth->emptyCopy();
      estimate->loadMetaMorphTraces(scan_files[s],true_synapses->getChannel(0),stuff.frame,true);
    }
    else if(intype > 0){
      estimate = truth->emptyCopy();
      estimate->loadPunctaAnalyzerPuncta(scan_files[s],true_synapses->getChannel(0),stuff.frame,true);
    }
    else{
      std::vector<ImRecord*> est_recs;
      FileConverter::read(&fm,iat,&est_recs,scan_files[s],1);
      estimate = est_recs[0];
    }
    //estimate->sortPuncta(true_synapses->getChannel(0),stack->frame(true_synapses->getChannel(0),0));
    //iat->findSynapses(estimate);
    runPoints(true_synapses,&stuff,estimate,&scan_files2,iat,s*nsp);
    /*
    for(int s2 = 0; s2 < nsp; s2++){
      FileConverter::read(&fm,&iat,&est_recs,scan_files[s2],1);
      std::vector<Cluster*> puncs = est_recs[1]->puncta(1);
      int chan1 = true_synapses->getChannel(1);
      estimate->clearPuncta(chan1);
      for(std::vector<Cluster*>::iterator clit = puncs.begin(); clit != puncs.end(); clit++) est_recs[0]->addPunctum(chan1,(*clit)->getCopy());
      iat.findSynapses(estimate);
      SynapseCollection* est_synapses = estimate->getSynapseCollection(col);
      est_puncta.push_back(estimate->getSynapticPunctaMaskFromCollection(col,0,false));
      est_puncta.push_back(estimate->getSynapticPunctaMaskFromCollection(col,1,false));
      est_peaks[0]->clear(0,estimate->imWidth(),0,estimate->imHeight());
      est_peaks[1]->clear(0,estimate->imWidth(),0,estimate->imHeight());
      for(int i = 0; i < est_synapses->nSynapses(); i++){
	Synapse* s = est_synapses->getSynapse(i);
	for(int j = 0; j < est_peaks.size(); j++){
	  std::vector<LocalizedObject::Point>::iterator peakPoint = s->getPunctum(j)->begin();
	  est_peaks[j]->setValue(peakPoint->x,peakPoint->y,i+1);
	}
      }
      for(int i = 0; i < nchan; i++) currentCombo[i] = starts[i];
      for(int k = 0; k < nCombos; k++){
	int regIndex = regIndices[k];
	if(regIndex < 0){
	  currentCombo[0]++;
	  for(int i = 1; currentCombo[i-1] == starts[i] && i < nchan; i++){
	    currentCombo[i-1] = starts[i-1];
	    currentCombo[i] = currentCombo[i] + 1;
	  }
	  continue;
	}
	LocalizedObject::Point lm = localMaxima[currentCombo[0]];
	int avgX = lm.x;
	int avgY = lm.y;
	bool isTrueSynapse = false;
	bool isEstSynapse = false;
	int sid = true_peaks[0]->getValue(lm.x,lm.y);
	Synapse* tsyn = NULL;
	if(sid > 0){
	  isTrueSynapse = true;
	  tsyn = true_synapses->getSynapse(sid-1);
	}
	sid = est_peaks[0]->getValue(lm.x,lm.y);
	Synapse* esyn = NULL;
	if(sid > 0){
	  isEstSynapse = true;
	  esyn = est_synapses->getSynapse(sid-1);
	}
	bool isTrueSynapticPuncta = (true_puncta[0]->getValue(lm.x,lm.y) > 0);
	bool isEstSynapticPuncta = (est_puncta[0]->getValue(lm.x,lm.y) > 0);
	for(int j = 1; j < nchan; j++){
	  lm = localMaxima[currentCombo[j]];
	  avgX += lm.x;
	  avgY += lm.y;
	  if(tsyn){
	    std::vector<LocalizedObject::Point>::iterator peakPoint = tsyn->getPunctum(j)->begin();
	    if(lm.x != peakPoint->x || lm.y != peakPoint->y) isTrueSynapse = false;
	  }
	  if(esyn){
	    std::vector<LocalizedObject::Point>::iterator peakPoint = esyn->getPunctum(j)->begin();
	    if(lm.x != peakPoint->x || lm.y != peakPoint->y) isEstSynapse = false;
	  }
	  isTrueSynapticPuncta = isTrueSynapticPuncta || (true_puncta[j]->getValue(lm.x,lm.y) > 0);
	  isEstSynapticPuncta = isEstSynapticPuncta || (est_puncta[j]->getValue(lm.x,lm.y) > 0);
	}
	avgX = avgX / nchan;
	avgY = avgY / nchan;
	bool nearby = true;
	for(int j = 0; j < nchan; j++){
	  lm = localMaxima[currentCombo[j]];
	  if(fabs(lm.x - avgX)/truth->resolutionXY() > 1.0 || fabs(lm.y - avgY)/truth->resolutionXY() > 1.0) nearby = false;
	}
	if(isTrueSynapse){
	  if(isEstSynapse) retval[s*nsp + s2][regIndex].n_true_positive += 1;
	  else if(isEstSynapticPuncta) retval[s*nsp + s2][regIndex].n_merge += 1;
	  else retval[s*nsp + s2][regIndex].n_false_negative += 1;
	}
	else if(isTrueSynapticPuncta){
	  if(isEstSynapse) retval[s*nsp + s2][regIndex].n_split += 1;
	}
	else{
	  if(isEstSynapse) retval[s*nsp + s2][regIndex].n_false_positive += 1;
	  else if(nearby) retval[s*nsp + s2][regIndex].n_true_negative += 1;
	}
	currentCombo[0]++;
	for(int i = 1; currentCombo[i-1] == starts[i] && i < nchan; i++){
	  currentCombo[i-1] = starts[i-1];
	  currentCombo[i]++;
	}
      }
      delete est_recs[1];
      est_recs.erase(est_recs.begin()+1);
      delete est_puncta[0];
      delete est_puncta[1];
      est_puncta.clear();
      //delete est_synapses;
    }
    */
    delete estimate;
  }
  //delete est_peaks[0];
  //delete est_puncta[0];
  for(int i = 0; i < nchan; i++){
    delete stuff.true_peaks[i];
    delete stuff.true_puncta[i];
  }
  //delete true_synapses;
  //for(int i = 0; i < 9; i++){
  //  for(int j = 0; j < truth->nRegions(); j++){
  //    std::cout << "::" << stuff.retval[i][j].n_true_positive << "," << stuff.retval[i][j].n_merge << "," << stuff.retval[i][j].n_false_negative << "::";
  //  }
  //  std::cout << std::endl;
  //}
  return stuff.retval;
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
  int minPunctaBrightness = 0;//65536;
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

  nia::niaVersion = 8;
  std::vector<ImRecord*> true_recs;
  FileManager phil;
  ImageAnalysisToolkit kit;
  std::ifstream fin(argv[4]);
  if(!(fin.is_open())){
    return 1;
  }
  std::string line;
  std::string basedir = "";
  int index = -1;
  int index2 = -1;
  std::vector< std::vector<Result> > results;
  std::vector<std::string> fnames,fnames2;
  ImSeries* data = NULL;
  int col_chan = boost::lexical_cast<int>(std::string(argv[2]));
  int intype = boost::lexical_cast<int>(std::string(argv[3]));
  while(getline(fin,line)){
    //std::cout << line << std::endl;
    if(line.find("data directory") == 0){
      getline(fin,basedir);
      continue;
    }
    else if(line.find("new image") == 0){
      index++;
      index2 = -1;
      getline(fin,line);
      std::string fname = basedir + line;
      phil.clearInputFiles();
      phil.addInputFile(FileConverter::read(&phil,&kit,&true_recs,fname,1));
      if(index > 0){
	delete true_recs[index-1];
	delete data;
      }
      phil.reset();
      data = phil.loadNext();
      for(int i = 0; i < true_recs[index]->nchannels(); i++) true_recs[index]->sortPuncta(i,data->fourLocation(0,0)->frame(i,0));
      fnames.clear();
      fnames2.clear();
    }
    else if(line.find("end image") == 0){
      if(argv[1][0] == 's'){
	if(index == 0) results = compareSynapses(true_recs[index],fnames,fnames2,data->fourLocation(0,0),&kit,col_chan,kit.postChan(),intype);
	else{
	  std::vector< std::vector<Result> > res = compareSynapses(true_recs[index],fnames,fnames2,data->fourLocation(0,0),&kit,col_chan,kit.postChan(),intype);
	  for(int k = 0; k < res.size(); k++){
	    for(int l = 0; l < res[k].size(); l++) results[k].push_back(res[k][l]);
	  }
	}
      }
      else if(argv[1][0] == 'p'){
	std::vector<std::string> fnames_chan = fnames;
	if(col_chan > 0) fnames_chan = fnames2;
	for(std::vector<std::string>::iterator fit = fnames_chan.begin(); fit != fnames_chan.end(); fit++){
	  ImRecord* estimate;
	  if(intype < 0){
	    estimate = true_recs[index]->emptyCopy();
	    estimate->loadMetaMorphTraces(*fit,col_chan,data->fourLocation(0,0)->frame(col_chan,0),true);
	  }
	  else if(intype > 0){
	    estimate = true_recs[index]->emptyCopy();
	    estimate->loadPunctaAnalyzerPuncta(*fit,col_chan,data->fourLocation(0,0)->frame(col_chan,0),true);
	  }
	  else{
	    std::vector<ImRecord*> est_recs;
	    FileConverter::read(&phil,&kit,&est_recs,*fit,1);
	    estimate = est_recs[0];
	  }
	  index2++;
	  std::vector<Result> res;
	  res = compare(true_recs[index],estimate,data->fourLocation(0,0)->frame(col_chan,0),col_chan,kit.postChan());
	  if(index == 0) results.push_back(res);
	  else{
	    for(int k = 0; k < res.size(); k++) results[index2].push_back(res[k]);
	  }
	  delete estimate;
	}
      }
      else return 1;
    }
    else{
      if(intype != 0){
	boost::char_separator<char> sep(",");
	boost::tokenizer< boost::char_separator<char> > tokens(line,sep);
	boost::tokenizer< boost::char_separator<char> >::iterator it = tokens.begin();
	fnames.push_back(basedir + *it);
	it++;
	fnames2.push_back(basedir + *it);
      }
      else{
	fnames.push_back(basedir + line);
	fnames2.push_back(basedir + line);
      }
    }
  }
  fin.close();

  std::vector<Result> totals;
  for(int i = 0; i < results.size(); i++){
    Result tot = results[i][0];
    for(int j = 1; j < results[i].size(); j++) tot = add(tot, results[i][j]);
    totals.push_back(tot);
  }
  std::ofstream fout(argv[5]);
  for(int i = 0; i < totals.size(); i++){
    double n_actual = totals[i].n_true_positive + totals[i].n_merge + totals[i].n_false_negative;
    //std::cout << n_actual << std::endl;
    fout << totals[i].n_true_positive/n_actual << ",";
    fout << totals[i].n_false_positive/(totals[i].n_false_positive + n_actual) << ",";
    fout << totals[i].n_merge/(n_actual) << ",";
    fout << totals[i].n_split/(n_actual) << "\n";
  }
  fout.close();
}
