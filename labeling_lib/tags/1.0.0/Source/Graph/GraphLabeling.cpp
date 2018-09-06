// -------------------------------------------------------------------------- //
// INCLUDE's                                                                  //
// -------------------------------------------------------------------------- //

#include "Graph/GraphLabeling.hpp"

// INCLUDE's ---------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// CLASS GraphProvider                                                        //
// -------------------------------------------------------------------------- //

Labeling::Graph Labeling::GraphProvider::AsGraph(Pixel_value* image,
                                       unsigned int width,
                                       unsigned int height,
                                       unsigned int channels)
{
  #ifdef CHECK_TIME   
    std::cout << "STEP1\n";
    clock_t step1_initial = clock();
  #endif
  
  /*
   * Label the image
   */
  LabelingData data = DoLabeling(image,width,height,channels); 
    
  /*
   * Looks for the neighborhood relationships and creates edges to represent them.
   */ 
  Edges regs_edges = LookForNeighborhood(data.neighbor_lbls,data.regs,data.replace,data.erased);
  for (std::set<Label>::reverse_iterator it = data.erased.rbegin(); it != data.erased.rend(); it++)
  {
    Label l = *it;
    Labeling::Region *reg = data.regs[l];
    delete reg;
    data.regs.erase(data.regs.begin() + l);
  } //end for
  
	#ifdef CHECK_TIME   
    clock_t step1_final = clock();  
    std::cout << "END: " << (step1_final - step1_initial) / (double)CLOCKS_PER_SEC << "seconds\n";
  #endif  

  /*
   * Return the labeled regions as a graph.
   */
  return Labeling::Graph(data.regs,regs_edges);
}

// -------------------------------------------------------------------------- //

Labeling::Edges Labeling::GraphProvider::LookForNeighborhood(std::set<Neighbors_Pair>& neighbor_lbls,
                                   Labeling::Vertices& regs,
                                   std::vector<int>& replace,
                                  std::set<Label> erased)
{
  #ifdef CHECK_TIME   
    std::cout << "STEP3\n";  
    clock_t step3_initial = clock();
  #endif
  
  Edges edges;
  
  for (std::set<Neighbors_Pair>::iterator it = neighbor_lbls.begin(); it != neighbor_lbls.end(); it++)
  {
    Neighbors_Pair neighbors = *it;
    
    Label s = neighbors.smaller;
    Label b = neighbors.bigger;
    
    /*
     * Checks if any of these labels were replaced.
     */
    Label replaced_s = (replace[s] == -1) ? s : replace[s];
    Label replaced_b = (replace[b] == -1) ? b : replace[b];
    
    /*
     * If labels not equal, it means there is neighborhood between the regions 
     * identified by these two labels.
     */
    if (replaced_s != replaced_b)
    {
      regs[replaced_s]->neighbors.insert(regs[replaced_b]);
      regs[replaced_b]->neighbors.insert(regs[replaced_s]);
      
      Label ss = std::min(replaced_s,replaced_b);
      Label bb = (ss == replaced_s) ? replaced_b : replaced_s;
      
      edges.insert(Labeling::Edge(regs[ss],regs[bb]));
    } //end if    
  } //end for
  
  #ifdef CHECK_TIME
    clock_t step3_final = clock();    
    std::cout << "END: " << (step3_final - step3_initial) / (double)CLOCKS_PER_SEC << "seconds\n";
  #endif
  
  return edges;
}

// CLASS GraphProvider ------------------------------------------------------ //  

