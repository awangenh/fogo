#ifndef ONEPARAMREADER_HPP_
#define ONEPARAMREADER_HPP_

//std stuff
#include <string>

//my stuff
#include "Verbose/VerboseUtil.hpp"

//TEST stuff
#include <iostream>

class OneParamReader : public VerboseUnit
{
  public:
    OneParamReader(std::string* p) : param(p) {};
    
    bool Process(const std::string& str)
    {
      *param = str;   
            
      return true;      
    }
  private:
    std::string* param;
};

#endif /*ONEPARAMREADER_HPP_*/
