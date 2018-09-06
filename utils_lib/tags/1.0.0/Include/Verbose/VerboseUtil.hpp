#ifndef VERBOSEUTIL_HPP_
#define VERBOSEUTIL_HPP_

// --------------- INCLUDE's -------------- //

//std stuff
#include <string>
#include <istream>
#include <map>
#include <vector>

// ----------- class VERBOSEUNIT ---------- //

class VerboseUnit
{
  public:  
    virtual bool Process(const std::string& str) = 0;    
    bool IsOptional() {return m_optional;};
  protected:
  	bool m_optional;
};

// ----------- namespace UTILS ------------ //

namespace Utils
{  
  int VerboseReader(std::istream& input,
                    std::map<std::string,VerboseUnit* >& states_map,
                    char delimiter = '\n');
                    
  int VerboseReader(std::istream& input,
                    std::vector<VerboseUnit*>& states,
                    char delimiter = '\n');
                    
  /*int VerboseReader(int argc,
                    char** argv,
                    std::map<std::string,VerboseUnit* >& states_map,
                    char delimiter = '\n'); */                                       
};

// ---------------------------------------- //

#endif /*VERBOSEUTIL_HPP_*/
