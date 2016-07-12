#ifndef IMFRAME_HPP
#define IMFRAME_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include "NiaUtils.hpp"
#include "Mask.hpp"

class ImFrame
{

protected:
  uint16_t** m_pixels;
  uint32_t m_width;
  uint32_t m_height;

public:
  ImFrame();
  virtual ~ImFrame();
  ImFrame(int width, int height);
  static std::vector<ImFrame*> load(const char* fname);
  static std::vector<ImFrame*> loadLittle(char* buf, std::ifstream &fin);
  static std::vector<ImFrame*> loadBig(char* buf, std::ifstream &fin);
  void readLittle(char* buf, std::ifstream &fin, uint32_t offset);
  void readBig(char* buf, std::ifstream &fin, uint32_t offset);
  void divide(int d);
  uint16_t getPixel(int x, int y){ return m_pixels[x][y]; }
  void setPixel(int x, int y, uint16_t value){ m_pixels[x][y] = value; }
  uint32_t width(){ return m_width; }
  uint32_t height(){ return m_height; }

  double mean(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2);
  double mean(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, Mask* m);
  double median(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2);
  double median(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, Mask* m);
  double mode(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2);
  double mode(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, Mask* m);
  double std(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2);
  double std(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, Mask* m);
  void getMedianStd(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, Mask* m, double& med, double& std);

  double mean(){ return mean(0,m_width,0,m_height); }
  double mean(Mask* m){ return mean(0,m_width,0,m_height,m); }
  double median(){ return median(0,m_width,0,m_height); }
  double median(Mask* m){ return median(0,m_width,0,m_height,m); }
  double mode(){ return mode(0,m_width,0,m_height); }
  double mode(Mask* m){ return mode(0,m_width,0,m_height,m); }
  double std(){ return std(0,m_width,0,m_height); }
  double std(Mask* m){ return std(0,m_width,0,m_height,m); }
  
};

#endif
