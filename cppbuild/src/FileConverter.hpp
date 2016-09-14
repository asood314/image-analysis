#ifndef FILECONVERTER_HPP
#define FILECONVERTER_HPP

#include "FileManager.hpp"
#include "ImageAnalysisToolkit.hpp"

class FileConverter
{

protected:
static FileManager::input_file readV00(FileManager* fm, ImageAnalysisToolkit* kit, std::vector<ImRecord*>* recs, std::string filename);
static void writeV00(FileManager* fm, ImageAnalysisToolkit* kit, std::vector<ImRecord*>* recs, std::string filename, int seriesID);
static FileManager::input_file readV01(FileManager* fm, ImageAnalysisToolkit* kit, std::vector<ImRecord*>* recs, std::string filename);
static void writeV01(FileManager* fm, ImageAnalysisToolkit* kit, std::vector<ImRecord*>* recs, std::string filename, int seriesID);
//static void convert(int oldVersion, int newVersion, std::string filename);

public:
FileConverter(){}
virtual ~FileConverter(){}
static FileManager::input_file read(FileManager* fm, ImageAnalysisToolkit* kit, std::vector<ImRecord*>* recs, std::string filename, int version);
static void write(FileManager* fm, ImageAnalysisToolkit* kit, std::vector<ImRecord*>* recs, std::string filename, int seriesID, int version);

};

#endif
