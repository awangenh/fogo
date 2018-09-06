// --------------- TODOs -------------- //

//TODO: permitir parâmetros opcionais.

// --------------- INCLUDE's -------------- //

//std stuff
#include <set>

//my stuff
#include <Verbose/VerboseUtil.hpp>

//TEST stuff
#include <iostream>

// ----------- namespace UTILS ------------ //

int Utils::VerboseReader(std::istream& input,
                         std::map<std::string,VerboseUnit* >& states_map,
                         char delimiter)
{
  bool waiting = false;
  std::string current_state = "";
  VerboseUnit *current_unit;
  
  /*
   * Check how many parameters are expected.
   */
  std::set<VerboseUnit*> expected;
  for( std::map<std::string,VerboseUnit* >::iterator it = states_map.begin(); 
  		 it != states_map.end(); it++ ) {
  	VerboseUnit* v = it->second;
  	
  	if (!v->IsOptional()) expected.insert(v);
  } //end for
  
  std::string line;
  while ( getline(input,line, delimiter) )
  {    
    if (!waiting) //expecting VerboseUnit to finish
    {      
      if (states_map.find(line) != states_map.end()) //exists
      {
        waiting = true;
        
        current_state = line;
        current_unit  = states_map[current_state];
        
        if (!current_unit->IsOptional()) expected.erase(current_unit);
      } //end if
    }
    else
    {
      if (current_unit->Process(line)) //finished
      {
        waiting = false;
        
        current_state = ""; 
      } //end if
    } //end if
    
  } //end while
  
  return (!expected.empty());
}                          

// ----------------------------------------------------- //

int Utils::VerboseReader(std::istream& input,
                    std::vector<VerboseUnit*>& states,
                    char delimiter)
{
	int count = 0;
  bool waiting = false;
  VerboseUnit *current_unit;
  
  /*
   * Check how many parameters are expected.
   */
  std::set<VerboseUnit*> expected;
  for( int i = 0; i < states.size(); i++ ) {
  	VerboseUnit* v = states[i];
  	
  	if (!v->IsOptional()) expected.insert(v);
  } //end for
  
  std::string line;
  while ( getline(input,line, delimiter) )
  {    
    if (!waiting) //expecting VerboseUnit to finish
    {
      waiting = true;
      
      current_unit  = states[count];
      
      if (!current_unit->IsOptional()) expected.erase(current_unit);      
    } //end if
    
    if (current_unit->Process(line)) //finished
    {
      waiting = false;
      ++count;
    } //end if
    
  } //end while
  
  return (!expected.empty());
}                    

// ----------------------------------------------------- //

/*int Utils::VerboseReader(int argc,
                    char** argv,
                    std::map<std::string,VerboseUnit* >& states_map,
                    char delimiter)
{
  int count = 0;
  int total = states_map.size();
  bool waiting = false;
  std::string current_state = "";
  VerboseUnit *current_unit;
  
  while ( count < total )
  {
    std::string line;
    
    getline( input, line, delimiter );
    
    if (!waiting) //expecting VerboseUnit to finish
    {      
      if (states_map.find(line) != states_map.end()) //exists
      {
        waiting = true;
        
        current_state = line;
        current_unit  = states_map[current_state];
      } //end if
    }
    else
    {
      if (current_unit->Process(line)) //finished
      {
        waiting = false;
        
        current_state = ""; 
        
        ++count; 
      } //end if
    } //end if
    
  } //end while
  
  return 0;
}*/

// ----------------------------------------------------- //
                     
