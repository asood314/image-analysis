#ifndef IMFRAME_HPP
#define IMFRAME_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <boost/thread.hpp>
#include "NiaUtils.hpp"
#include "Mask.hpp"

class ImFrame
{

protected:
  std::vector< std::vector<int> > m_pixels;
  int m_width;
  int m_height;

public:
  ImFrame();
  virtual ~ImFrame();
  ImFrame(int width, int height);
  static std::vector<ImFrame*> load(const char* fname);
  static std::vector<ImFrame*> loadLittle(char* buf, std::ifstream &fin);
  static std::vector<ImFrame*> loadBig(char* buf, std::ifstream &fin);
  static void loadMT(std::vector<ImFrame*>* frames, const char* fname);
  static void loadLittleMT(std::vector<ImFrame*>* frames, char* buf, const char* fname);
  static void loadBigMT(std::vector<ImFrame*>* frames, char* buf, const char* fname);
  void readLittle(char* buf, std::ifstream &fin, uint32_t offset);
  void readBig(char* buf, std::ifstream &fin, uint32_t offset);
  void readLittleMT(char* buf, const char* fname, uint32_t offset);
  void readBigMT(char* buf, const char* fname, uint32_t offset);
  void divide(int d);
  int getPixel(int x, int y){ return m_pixels[x][y]; }
  void setPixel(int x, int y, int value){ m_pixels[x][y] = value; }
  int width(){ return m_width; }
  int height(){ return m_height; }
  ImFrame* derivative();
  ImFrame* d2EigenvalueMax();

  int min(int x1, int x2, int y1, int y2);
  double mean(int x1, int x2, int y1, int y2);
  double mean(int x1, int x2, int y1, int y2, Mask* m);
  double median(int x1, int x2, int y1, int y2);
  double median(int x1, int x2, int y1, int y2, Mask* m);
  double median(int x1, int x2, int y1, int y2, int offset);
  double median(int x1, int x2, int y1, int y2, int offset, Mask* m);
  double mode(int x1, int x2, int y1, int y2);
  double mode(int x1, int x2, int y1, int y2, Mask* m);
  double mode(int x1, int x2, int y1, int y2, int offset);
  double mode(int x1, int x2, int y1, int y2, int offset, Mask* m);
  double std(int x1, int x2, int y1, int y2);
  double std(int x1, int x2, int y1, int y2, Mask* m);
  void getMedianStd(int x1, int x2, int y1, int y2, Mask* m, int nVals, double& med, double& std);

  int min(){ return min(0,m_width,0,m_height); }
  double mean(){ return mean(0,m_width,0,m_height); }
  double mean(Mask* m){ return mean(0,m_width,0,m_height,m); }
  double median(){ return median(0,m_width,0,m_height); }
  double median(Mask* m){ return median(0,m_width,0,m_height,m); }
  double median(int offset){ return median(0,m_width,0,m_height,offset); }
  double median(int offset, Mask* m){ return median(0,m_width,0,m_height,offset,m); }
  double mode(){ return mode(0,m_width,0,m_height); }
  double mode(Mask* m){ return mode(0,m_width,0,m_height,m); }
  double mode(int offset){ return mode(0,m_width,0,m_height,offset); }
  double mode(int offset, Mask* m){ return mode(0,m_width,0,m_height,offset,m); }
  double std(){ return std(0,m_width,0,m_height); }
  double std(Mask* m){ return std(0,m_width,0,m_height,m); }
  
};

#endif
