#ifndef IMFRAME_HPP
#define IMFRAME_HPP

#include <iostream>
#include <fstream>
#include <vector>

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
  uint16_t getPixel(int x, int y){ return m_pixels[x][y]; }
  uint32_t width(){ return m_width; }
  uint32_t height(){ return m_height; }

  static uint16_t convertToShort(char c0, char c1);
  static uint32_t convertToInt(char c0, char c1, char c2, char c3);

};

#endif
