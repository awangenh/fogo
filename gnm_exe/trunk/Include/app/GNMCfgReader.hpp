#ifndef GNMCFGREADER_HPP_
#define GNMCFGREADER_HPP_

//std stuff
#include <string>

//my stuff
#include <Verbose/VerboseUtil.hpp>

//TEST stuff
#include <iostream>

class GNMCfgReader : public VerboseUnit
{
public:
  GNMCfgReader(double* cfg, unsigned int total, bool optional = false) : cfg_params(cfg),
									 cfg_total(total),
									 count(0) {m_optional = optional;};
    
 bool Process(const std::string& str)
  {
    cfg_params[count++] = atof(str.c_str());
      
    //std::cout << "PARAM: " << str << "\n";   
    
    return (count >= cfg_total);
  }
private:
  double*      cfg_params;
  unsigned int cfg_total;
  unsigned int count;
};

#endif /*GNMCFGREADER_HPP_*/
