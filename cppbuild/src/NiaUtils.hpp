#ifndef NIA_UTILS_HPP
#define NIA_UTILS_HPP

#include <iostream>

/*--------------------------------------------------
Methods for converting integers/floating point numbers to character arrays and vice-versa
--------------------------------------------------*/
class NiaUtils
{

public:
  static uint16_t convertToShort(char c0, char c1);
  static uint32_t convertToInt(char c0, char c1, char c2, char c3);
  static double convertToDouble(char c0, char c1, char c2, char c3);
  static void writeShortToBuffer(char* buf, uint64_t offset, int data);
  static void writeIntToBuffer(char* buf, uint64_t offset, int data);
  static void writeDoubleToBuffer(char* buf, uint64_t offset, double data);

};

#endif
