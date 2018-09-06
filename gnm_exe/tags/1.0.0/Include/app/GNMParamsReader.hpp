#ifndef GNMPARAMSREADER_HPP_
#define GNMPARAMSREADER_HPP_

//std stuff
#include <string>

//my stuff
#include <Verbose/VerboseUtil.hpp>

//TEST stuff
#include <iostream>

class GNMParamsReader : public VerboseUnit
{
public:
  GNMParamsReader(std::string* path, bool optional = false) : file_path(path) {m_optional = optional;};
  
  bool Process(const std::string& str)
  {
    *file_path = str;
      
    //std::cout << "PATH: " << *file_path << "\n";   
    
    return true;      
  }
private:
  std::string* file_path;
};

#endif /*GNMPARAMSREADER_HPP_*/
