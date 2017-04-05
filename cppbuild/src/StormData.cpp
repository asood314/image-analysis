#include "StormData.hpp"

void StormData::read(std::string infile)
{
  std::ifstream fin(infile);
  if(fin.is_open()){
    std::string line;
    getline(fin,line);
    Blink b;
    b.clusterID = -1;
    while(getline(fin,line)){
      boost::char_separator<char> sep(",");
      boost::tokenizer< boost::char_separator<char> > tokens(line,sep);
      boost::tokenizer< boost::char_separator<char> >::iterator tit = tokens.begin();
      b.frame = (int)(boost::lexical_cast<double>(*tit));
      tit++;
      b.x = boost::lexical_cast<double>(*tit);
      tit++;
      b.y = boost::lexical_cast<double>(*tit);
      tit++;
      b.z = boost::lexical_cast<double>(*tit);
      tit++;
      b.sigma1 = boost::lexical_cast<double>(*tit);
      tit++;
      b.sigma2 = boost::lexical_cast<double>(*tit);
      tit++;
      b.intensity = boost::lexical_cast<double>(*tit);
      m_molecules.push_back(b);
    }
    fin.close();
  }
  else std::cout << "Error: Couldn't open file" << std::endl;
}
