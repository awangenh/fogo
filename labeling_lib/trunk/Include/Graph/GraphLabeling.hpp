#ifndef GRAPHLABELING_HPP_
#define GRAPHLABELING_HPP_

// -------------------------------------------------------------------------- //
// INCLUDE's                                                                  //
// -------------------------------------------------------------------------- //

//std stuff
#include "Labeling/Labeling.hpp"
//#include "Graph/Edge.hpp"

// INCLUDE's ---------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// NAMESPACE Labeling                                                         //
// -------------------------------------------------------------------------- //

namespace Labeling
{
  typedef std::pair<Region*,Region*> Edge;
  typedef std::set<Edge> Edges;
  typedef std::pair<Labeling::Vertices,Labeling::Edges> Graph;
  
  class GraphProvider : public Provider
  {
  public:
    
    static Labeling::Graph AsGraph(Pixel_value* image,
				   unsigned int width,
				   unsigned int height,
				   unsigned int channels);
  private:
    static Labeling::Edges LookForNeighborhood(std::map<Neighbors_Pair,unsigned>& neighbor_lbls,
					       Labeling::Vertices& regs,
					       std::vector<int>& replace,
					       std::set<Label> erased);
  };
};

#endif /*GRAPHLABELING_HPP_*/
