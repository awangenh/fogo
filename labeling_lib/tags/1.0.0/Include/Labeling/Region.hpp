/**
 * \file Region.hpp
 *
 * Header defining regions of pixels.
 *
 * $Rev $
 * $Date 11/13/2007$
 * $Author Fogo!$
 */

// -------------------------------------------------------------------------- //
// DEFINE's                                                                   //
// -------------------------------------------------------------------------- //

#ifndef REGION_HPP_
#define REGION_HPP_

// DEFINE's ----------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// INCLUDE's                                                                  //
// -------------------------------------------------------------------------- //

//std stuff
#include <set>
#include <map>

// INCLUDE's ---------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// TYPEDEF's                                                                  //
// -------------------------------------------------------------------------- //

typedef unsigned int Label;
typedef unsigned int Index;

// TYPEDEF's ---------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// NAMESPACE Labeling                                                         //
// -------------------------------------------------------------------------- //

namespace Labeling
{
	// -------------------------------------------------------------------------- //
	// STRUCT Region				                                                      //
	// -------------------------------------------------------------------------- //
	
	/*
	 * \brief Regions of pixels, also linking to its neighbor regions.
	 * \struct Region
	 */
  struct Region
  {
    Region();
    Region(const Region& other_region);
    ~Region();
  
     std::set<Index> positions;
     std::set<Region*> neighbors;
  };
  
  // STRUCT Region ------------------------------------------------------------ //  
};

// NAMESPACE Labeling --------------------------------------------------------//

#endif /*REGION_HPP_*/