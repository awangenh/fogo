//TODO: talvez otimizar a remo��o das regi�es redundantes
//TODO: talvez otimizar a parte onde detecta o label correspondente qdo pixels iguais

// -------------------------------------------------------------------------- //
// INCLUDE's                                                                  //
// -------------------------------------------------------------------------- //

//CHECK_TIME stuff
//#define CHECK_TIME
#ifdef CHECK_TIME
#include <iostream>
#include <time.h>
#endif

//std stuff
#include <sstream>

//my stuff
#include <Labeling/Labeling.hpp>

// INCLUDE's ---------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// STRUCT Neighbors_Pair                                                      //
// -------------------------------------------------------------------------- //

Neighbors_Pair::Neighbors_Pair(Label lbl1, Label lbl2)
{
  smaller = std::min(lbl1,lbl2), 
  bigger  = (smaller == lbl1) ? lbl2 : lbl1;
}

// -------------------------------------------------------------------------- //
  
bool Neighbors_Pair::operator==(const Neighbors_Pair& other_eq) const
{  
  return (smaller == other_eq.smaller && bigger == other_eq.bigger);
}

// -------------------------------------------------------------------------- //

bool Neighbors_Pair::operator<(const Neighbors_Pair& other_eq) const
{  
  return (bigger < other_eq.bigger || 
          (bigger == other_eq.bigger && smaller < other_eq.smaller) );          
}

// STRUCT Neighbors_Pair ----------------------------------------------------- //

// -------------------------------------------------------------------------- //
// NAMESPACE Labeling                                                         //
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// CLASS Provider                                                             //
// -------------------------------------------------------------------------- //

Labeling::Provider::LabelingData Labeling::Provider::DoLabeling(Pixel_value* image,
															          unsigned int width,
															          unsigned int height,
															          unsigned int channels)
{
#ifdef CHECK_TIME   
    std::cout << "STEP1\n";
    clock_t step1_initial = clock();
  #endif
  
  Label count_lbl = 0;
  
  /*
   * Builds the mask used to select only the useful values.
   */
  int size_pix = sizeof(Pixel_value);  
  long mask = 0;
  long aux_mask = 0xFF;  
  for (unsigned int i=0; i< size_pix * channels; i++)
  {
    mask ^= aux_mask;
    aux_mask = aux_mask << 8;
  } //end for
  
  /*
   * Initialize the vector that will store the resulting regions, reserving already
   * a certain ammount of memory. Also creates the auxiliar vector that will store
   * the label replacements caused by equivalences. These both vectors have a 1x1 
   * relationship.
   */ 
  Vertices regs;
  std::vector<int> replace;
  int min_size = (width * height) / 2;
  regs.reserve(min_size);
  replace.reserve(min_size);
  
  /*
   * This variable will be used to stored the sets that will track the equivalence between
   * labels, also keeps a mapping between these sets and the labels.
   */
  std::map<Label,TrackSet*> track;
  std::vector<TrackSet*> storage;
  
  /*
   * The set that will store the neighborhood pairs.
   */
  std::map<Neighbors_Pair,unsigned> neighbor_lbls;
  
  /*
   * Initialization of auxiliar arrays of labels, to speed up the access to the
   * label information.
   */
  Label       *lbls_1 = new Label[width];
  Label       *lbls_2 = new Label[width];  
  Label       *lbl_current = lbls_1;
  Label       *lbl_top     = lbls_2;
  bool        swap_flag    = true;
  
  /*
   * Some auxiliar variables.
   */
  unsigned int aux_width = width * channels;
  
  /*
   * Run through the image.
   */
  for (unsigned int j=0; j<height; j++)
  {    
    /*
     * Some auxiliar variables.
     */
    unsigned int current_j = (j * aux_width);
    unsigned int top_j = ((j-1) * aux_width);
    
    /*
     * Checks the current position vertical boundaries.
     */
    bool not_top    = j > 0;
        
    for (unsigned int i=0; i<width; i++)
    {                
      unsigned int row_pos = i * channels;
      unsigned int img_pos = row_pos + current_j;
      unsigned int img_pos_top = row_pos + top_j;
      
      /*
       * Checks the current position horizontal boundaries.
       */
      bool not_left   = i > 0;
      bool not_right  = (i+1) < width;      
      
      /*
       * Get only the necessary pixel values for the comparison with the current pixel
       * value.
       */
      long current  = *(reinterpret_cast<long*>(&image[img_pos])) & mask;
      
      /*
       * Bool array that will say if there is any pixel equivalent to the current one in
       * the left, top, top left or top right positions.
       */
      bool eq_boundary[4], eq_pixel[4]; //LEFT, TOP, TOP_LEFT, TOP_RIGHT
      eq_boundary[0] = not_left;
      eq_boundary[1] = not_top;
      eq_boundary[2] = not_left && not_top;
      eq_boundary[3] = not_right && not_top;
      
      eq_pixel[0] = eq_boundary[0] && 
                    current == (*(reinterpret_cast<long*>(&image[img_pos - channels])) & mask);
      eq_pixel[1] = eq_boundary[1] &&
                    current == (*(reinterpret_cast<long*>(&image[img_pos_top])) & mask);
      eq_pixel[2] = eq_boundary[2] &&
                    current == (*(reinterpret_cast<long*>(&image[img_pos_top - channels])) & mask);
      eq_pixel[3] = eq_boundary[3] &&
                    current == (*(reinterpret_cast<long*>(&image[img_pos_top + channels])) & mask);
      
      /*
       * Finding out the label for the current position.
       */
      Label n_lbl[4];
      n_lbl[0] = (eq_boundary[0]) ? lbl_current[i-1] : 99999;
      n_lbl[1] = (eq_boundary[1]) ? lbl_top[i]       : 99999;
      n_lbl[2] = (eq_boundary[2]) ? lbl_top[i-1]     : 99999;
      n_lbl[3] = (eq_boundary[3]) ? lbl_top[i+1]     : 99999;
       
      /*
       * Auxiliar array to store the neighbors found for the current position.
       */ 
      bool neighbors[4]; 
       
      Label current_lbl;
      bool labeled = false;      
      for (int n=0; n<4; n++)
      { 
        neighbors[n] = false;
               
        //if neighbors inside the boundaries of the image       
        if (eq_boundary[n]) 
        {
          //if neighbors have the same pixel value 
          if (eq_pixel[n])
          {
            //and not labeled receives the neighbor label
            if (!labeled)
            {
              current_lbl = n_lbl[n];
              lbl_current[i] = current_lbl;
                          
              //adds position to existent region
              Region *reg = regs[current_lbl];              
              reg->positions.insert(img_pos/channels);
              replace.push_back(-1);
              
              labeled = true;
            } 
            else 
            if (current_lbl != n_lbl[n])//if labeled creates a equivalence
            {
              CreateEquivalence(current_lbl,n_lbl[n],track,storage);            
            } //end if
          }
          else //if different pixel value, not same region but neighbors. 
          {
            neighbors[n] = true; 
          } //end if
        } //end if
        
      } //end for
      if (!labeled) //if did not receive the label of any neighbor, creates one
      {
        current_lbl = count_lbl++;
        lbl_current[i] = current_lbl;
        
        //creates new region
        Region *reg = new Region();        
        reg->positions.insert(img_pos/channels);
        regs.push_back(reg);
        replace.push_back(-1);       
           
      } //end if
      
      for (unsigned int n=0; n<4; n++) {
	if (neighbors[n] && current_lbl != n_lbl[n]) {
	  Neighbors_Pair nn(current_lbl,n_lbl[n]);

	  std::map<Neighbors_Pair,unsigned>::iterator iter = neighbor_lbls.find(nn);
	  ( iter != neighbor_lbls.end() ) ? 	    
	    ++(neighbor_lbls[nn]) :
	    neighbor_lbls[nn] = 1;
	  //neighbor_lbls.insert(nn);  
	} //end if
      } //end for
            
    } //end for
    
    /*
     * Swaps the current and top label arrays.
     */
    lbl_current = (swap_flag) ? lbls_2 : lbls_1; 
    lbl_top     = (swap_flag) ? lbls_1 : lbls_2;
    swap_flag = !swap_flag;
  } //end for
  
  #ifdef CHECK_TIME   
    clock_t step1_final = clock();  
    std::cout << "END: " << (step1_final - step1_initial) / (double)CLOCKS_PER_SEC << "seconds\n";
  #endif  
  
  /*
   * Gets rid of the redundant regions.
   */  
  std::set<Label> erased = SolveEquivalences(regs, track, replace);
    
  /*
   * Some house cleaning before we finish this.
   */
  delete lbls_1;
  delete lbls_2;
  for (unsigned int i=0;i<storage.size();i++) delete storage[i];
  
  /*
   * Return data related to the labeling realized.
   */
  
  LabelingData data;
  data.regs = regs;
  data.neighbor_lbls = neighbor_lbls;
  data.replace = replace;
  data.erased = erased;
  
  return data;
}

// -------------------------------------------------------------------------- //

void Labeling::Provider::CreateEquivalence(Label l1,
                                 Label l2,
                                 std::map<Label,TrackSet*>& track,
                                 std::vector<TrackSet*>& storage)
{ 
  /*
   * Verifies which of the 2 labels is the smaller.
   */
  Label small = std::min(l1,l2);
  Label big = (small == l1) ? l2 : l1;
  
  /*
   * Check if either one of these labels is already contained by any tracking set.
   */
  bool find_s = track.find(small) != track.end();
  bool find_b = track.find(big) != track.end();
  
  /*
   * If no one is contained, created a new set and add both to it. Also make sure that mapping
   * between these labels and the new tracking set is set.
   */
  if (!find_s && !find_b)
  {
    TrackSet* t = new TrackSet();
    t->insert(big);
    t->insert(small);
    track[small] = t;
    track[big] = t;
  }
  else
  /*
   * If only the bigger label is contained by a set, create a new set for the smaller label
   * and add all labels involved in the bigger' set to it. Also make sure that all these 
   * labels form the bigger' set point to this new set now.
   */
  if (!find_s)
  {
    TrackSet* ts = new TrackSet();
    TrackSet* tb = track[big];
    ts->insert(tb->begin(),tb->end());
    for (TrackSet::iterator t = tb->begin(); t != tb->end(); t++)
    {
      Label b = *t;
      track[b] = ts;
    } //end for
    ts->insert(small);
    track[small] = ts;
  } 
  else
  /*
   * If only the smaller label is contained by a set, add the bigger label to the set 
   * associated to the smaller label.
   */
  if (!find_b)
  {
    TrackSet* t = track[small];
    t->insert(big);
    track[big] = t;
  }
  /*
   * If both are contained by a set, add all labels involved in the bigger' set to the
   * existing set associated with the smaller label. Also make sure that all the 
   * labels form the bigger' set point to the smaller label set now.
   */
  else
  {
    TrackSet* ts = track[small];
    TrackSet* tb = track[big];
    if (ts == tb) return;
    ts->insert(tb->begin(),tb->end());
    for (TrackSet::iterator t = tb->begin(); t != tb->end(); t++)
    {
      Label b = *t;
      track[b] = ts;
    } //end for
      
  } //end if
}                                
                                
// -------------------------------------------------------------------------- //                            

std::set<Label> Labeling::Provider::SolveEquivalences(Labeling::Vertices& regs,
                                            std::map<Label,TrackSet*>& track,
                                            std::vector<int>& replace)
{
  #ifdef CHECK_TIME   
    std::cout << "STEP2\n";  
    clock_t step2_initial = clock();
  #endif
  
  std::set<Label> erased;
  
  /*
   * Run through the equivalence tracking sets, looking to get rid of the redundant regions.
   * The countdown will be used to speed up the process, if all necessary regions are checked,
   * the process is stopped immediatelly. 
   */
  unsigned int countdown = track.size();
  for(std::map<Label,TrackSet*>::iterator it = track.begin(); countdown > 0 && it != track.end(); it++)
  {
    /*
     * If the current has not yet been replaced, it means its tracking set still has redundant
     * regions.
     */
    Label l = (*it).first;
    if (replace[l] == -1)
    {
      --countdown;
      /*
       * Takes out the first label of the tracking set as that is the smallest label. This
       * label will be used to replace all other labels in this tracking set.
       */
      TrackSet* t = (*it).second;
      Label smallest = *(t->begin());
      t->erase(t->begin());
      
      Region* reg_s = regs[smallest];            
      for (TrackSet::iterator tt = t->begin(); tt != t->end(); tt++)
      {
        Label bigger = *tt;
        replace[bigger] = smallest;
        erased.insert(bigger);
        --countdown;
        
        /*
         * Gets all the positions from the region related to the bigger label added to the
         * region with the smaller label.
         */
        Region* reg_b = regs[bigger];
        reg_s->positions.insert(reg_b->positions.begin(),reg_b->positions.end());
      } //end for
    } //end if
  } //end for

  #ifdef CHECK_TIME 
    clock_t step2_final = clock();  
    std::cout << "END: " << (step2_final - step2_initial) / (double)CLOCKS_PER_SEC << "seconds\n";
  #endif
  
  return erased;
}                         
                                    
// -------------------------------------------------------------------------- //

std::string Labeling::Provider::AsString(const Labeling::Vertices& regs,
                  unsigned int width,
                  unsigned int height)
{
  Label* label_img = new Label[width * height];
  for (Label l = 0; l < regs.size(); l++)
  {
    Region* reg = regs[l];
    
    for (std::set<Index>::const_iterator it = reg->positions.begin(); 
         it != reg->positions.end();
         it++)
    {
      Index pos = *it;
      label_img[pos] = l;
    } //end for
  } //end for
  
  std::ostringstream str_img;
  for (unsigned int j = 0; j < height; j++)
  {
    unsigned int aux = j * width;
    for (unsigned int i = 0; i < width; i++)
    {
      str_img << label_img[aux + i] << " ";
    } //end for
    str_img << "\n";
  } //end for
  
  return str_img.str();
}                                                                               

// CLASS Provider ------------------------------------------------------------//

// NAMESPACE Labeling --------------------------------------------------------//


