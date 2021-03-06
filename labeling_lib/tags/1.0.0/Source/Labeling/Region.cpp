// -------------------------------------------------------------------------- //
// INCLUDE's                                                                  //
// -------------------------------------------------------------------------- //

//my stuff
#include <Labeling/Region.hpp>

// INCLUDE's ---------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// STRUCT Region				                                                      //
// -------------------------------------------------------------------------- //

Labeling::Region::Region()
{
  
}

// -------------------------------------------------------------------------- //

Labeling::Region::Region(const Region& other_region)
{
  positions = std::set<Index>(other_region.positions);
  neighbors = std::set<Region*>(other_region.neighbors);
}

// -------------------------------------------------------------------------- //

Labeling::Region::~Region()
{
  
}

// STRUCT Region ------------------------------------------------------------ //
