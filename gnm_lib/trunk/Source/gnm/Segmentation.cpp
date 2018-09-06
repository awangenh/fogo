/**
 * \file Segmentation.hpp
 *
 * Implementation for the \link Algorithm class, contained by \link gnm namespace.
 *
 * $Rev $
 * $Date 04/04/2007$
 * $Author Fogo!$
 */

// -------------------------------------------------------------------------- //
// TODO's                                                                     //
// -------------------------------------------------------------------------- //

//TODO: se livrar do WX_MESSED_RGB.
//TODO: existem alguns detalhes que podem ser melhorados.

// TODO's ------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// DEFINE's                                                                   //
// -------------------------------------------------------------------------- //

//#define WX_MESSED_RGB //used only because of a stupid thing that happens with wxImage

// DEFINE's ----------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// INCLUDE's                                                                  //
// -------------------------------------------------------------------------- //

//stdlib stuff
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <sstream>
#include <iostream>

//my stuff
#include <Color/ColorspaceUtil.hpp>
#include <gnm/Segmentation.hpp>

// INCLUDE's ---------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// IMPLEMENTATION CLASS gnm::Algorithm                                        //
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// PUBLIC METHODS                                                             //
// -------------------------------------------------------------------------- //

gnm::Results gnm::Algorithm::Execute( const Pixel_value *labelled, //RGB
				      const Pixel_value *original, //RGB
				      const int width,
				      const int height,
				      const Labeling::Vertices& regions,
				      const Labeling::Edges& edges,              
				      const Configuration& perception,
				      const bool borders, 
				      const bool dump,
				      const bool post_processing
				      )
{
  m_verbose     = false;
  m_log_stream  = NULL;
  m_dump        = dump;
  m_borders     = borders;
  m_post_processing = post_processing;
  
  return Start( labelled,
		original,
		width,
		height,
		regions, 
		edges,              
		perception);
}

// PUBLIC METHODS ----------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// PRIVATE METHODS                                                            //
// -------------------------------------------------------------------------- //

gnm::Results gnm::Algorithm::Start( const Pixel_value *labelled, //RGB
				    const Pixel_value *original, //RGB
				    const int width,
				    const int height,
				    const Labeling::Vertices& regions,
				    const Labeling::Edges& edges,              
				    const Configuration& perception
				    )
{
  //---------------------------------------------------------//
  // 1. INITIALIZATION                                       //
  //---------------------------------------------------------//
  
  /*
   * Setting the desired perception.
   */
  m_perception_config = perception;
  
  /*
   * Vector that will be used to track the meta-regions merging.
   * 1-to-1 relationship with labels.
   */ 
  //std::map<Label,MetaTrackSet*> meta_track;
  std::vector<MetaTrackSet*> meta_track;
  meta_track.resize(regions.size(),NULL);
  
  /*
   * Map used to track meta-regions small enough to require post-processing.
   */
  std::set<Metaregion*> post_track;

  std::pair<Meta_Vertices,HSI_Map> res_init = InitializeMetaregions( regions,
								     meta_track,
								     post_track,
								     original,
								     width,
								     height);
                                                            
  Meta_Vertices meta_regions = res_init.first;
  HSI_Map       hsi          = res_init.second;
  
  if (m_verbose) 
    (*m_log_stream) << "Performing gradient segmentation ...\n";  
  
  //---------------------------------------------------------//
  // 2. EXECUTION                                            //
  //---------------------------------------------------------//

#define SORTED
#ifdef UNSORTED
  for (Labeling::Edges::iterator i=edges.begin(); i != edges.end(); i++){
    Labeling::Edge current_edge = *i;
    Labeling::Region *r1 = current_edge.first;
    Labeling::Region *r2 = current_edge.second;    
    
    /*
     * Regions are not already in the same meta-region
     */
    if (meta_track.find(meta_regions[r1]->label_id) != 
        meta_track.find(meta_regions[r2]->label_id)){
      /*
       * If the gradient is acceptable, merge the meta-regions which contain both regions.
       */   	
      std::pair< std::pair<double,double> , bool> tmp = LookForGradient(hsi[r1],hsi[r2]);
      std::pair<double,double> gradient = tmp.first;
      if (gradient.first < gradient.second){    	
	MergeMetaRegionsOf(r1,r2,regions,meta_regions,meta_track);	    	
      } //end if
    } //end if    
  } //end for
  
#endif
  
#ifdef SORTED    
  std::set<EvalEdge> clearSet;
  std::set<EvalEdge> roughSet;
  for (Labeling::Edges::iterator i=edges.begin(); i != edges.end(); i++){
    Labeling::Edge current_edge = *i;
    Labeling::Region *r1 = current_edge.first;
    Labeling::Region *r2 = current_edge.second;    
    
    /*
     * Check if the gradient between these two regions is acceptable.
     */
    std::pair< std::pair<double,double> , bool> tmp = LookForGradient(hsi[r1],hsi[r2]);
    std::pair<double,double> gradient = tmp.first;
    if (gradient.first < gradient.second){
      EvalEdge ee;
      ee.first = current_edge;
      ee.second = gradient.first;
      
      (tmp.second) ? clearSet.insert(ee) : roughSet.insert(ee);
    } //end if
  } //end for
  
  
  for (std::set<EvalEdge>::iterator i=clearSet.begin(); i != clearSet.end(); i++){
    Labeling::Edge current_edge = i->first;
    Labeling::Region *r1 = current_edge.first;
    Labeling::Region *r2 = current_edge.second;
      
    /*
     * Regions are not already in the same meta-region
     */
    /*if (meta_track.find(meta_regions[r1]->label_id) != 
      meta_track.find(meta_regions[r2]->label_id)){*/
    if (meta_track[meta_regions[r1]->label_id] != meta_track[meta_regions[r2]->label_id]){
      /*
       * If the gradient is acceptable, merge the meta-regions which contain both regions.
       */   	
      
      //MergeMetaRegionsOf(r1,r2,regions,meta_regions,meta_track,post_track);
      
      /*
       * First recovers the meta-regions which contain regions r1 and r2.
       */
      Metaregion *meta1  = meta_regions[r1];
      Metaregion *meta2  = meta_regions[r2];
      	
      MergeMetaRegionsOf(meta1,meta2,regions,meta_regions,meta_track,post_track);
    } //end if            
  } //end for
  
  for (std::set<EvalEdge>::iterator i=roughSet.begin(); i != roughSet.end(); i++){
    Labeling::Edge current_edge = i->first;
    Labeling::Region *r1 = current_edge.first;
    Labeling::Region *r2 = current_edge.second;
    
    /*
     * Regions are not already in the same meta-region
     */
    /*if (meta_track.find(meta_regions[r1]->label_id) != 
        meta_track.find(meta_regions[r2]->label_id)){*/
    if (meta_track[meta_regions[r1]->label_id] != meta_track[meta_regions[r2]->label_id]){
      /*
       * If the gradient is acceptable, merge the meta-regions which contain both regions.
       */   	
      
      //MergeMetaRegionsOf(r1,r2,regions,meta_regions,meta_track,post_track);
      
      /*
       * First recovers the meta-regions which contain regions r1 and r2.
       */
      Metaregion *meta1  = meta_regions[r1];
      Metaregion *meta2  = meta_regions[r2];
      
      MergeMetaRegionsOf(meta1,meta2,regions,meta_regions,meta_track,post_track);
    } //end if            
  } //end for
  
#endif
  
  if (m_post_processing)
    DoPostProcessing(regions,meta_regions,meta_track,post_track);
  
  if (m_verbose) 
    (*m_log_stream) << "Building output image ...\n";  
  
  //---------------------------------------------------------//
  // 3. OUTPUT CREATION                                      //
  //---------------------------------------------------------//
  
  //if (segmap != NULL && segdump != NULL){ 
  //  BuildMapDump(regions,meta_regions,meta_track,width,height,segmap,segdump);
  //} //end if
  
  Results results = BuildOutput(regions,meta_regions,meta_track,width,height);
      
  if (m_verbose) 
    (*m_log_stream) << "Gradient segmentation finished!\n";
  
  //---------------------------------------------------------//
  // 4. GARBAGE CLEANING                                     //
  //---------------------------------------------------------//
  
  for(HSI_Map::iterator hit = hsi.begin(); hit != hsi.end(); hit++ ) 
    delete (*hit).second;
    
  return results;  
}
              
// -------------------------------------------------------------------------- //              

std::pair<Meta_Vertices,HSI_Map> gnm::Algorithm::InitializeMetaregions( const Labeling::Vertices& regions,
									//std::map<Label,MetaTrackSet*>& meta_track, 
									std::vector<MetaTrackSet*>& meta_track,
									std::set<Metaregion*>& post_track,
									const Pixel_value *original, //RGB
									const int width,
									const int height                 
									)
{
  Meta_Vertices meta_v;
  HSI_Map hsi_map; 
  
  for (unsigned int k=0; k<regions.size(); k++){
    Labeling::Region *current = regions[k];
    
    unsigned int sum_R = 0;
    unsigned int sum_G = 0;
    unsigned int sum_B = 0;
    std::set<Index> pos = current->positions;
    int total  = pos.size();
    
    for (std::set<Index>::iterator it=pos.begin(); it!=pos.end(); it++){      
      Index p = *it * 3;
      
      /*
       * These sums will be used to accumulate the total sum of the values of each channel
       * for each meta-region, making faster to get the mean color value later.
       */
      sum_R += original[p];
      sum_G += original[p + 1];
      sum_B += original[p + 2];
    } //end for
    
    /*
     * Converts the mean RGB value of this meta-region to HSI and maps it.
     */
    unsigned char r = (unsigned char)round((double)sum_R / total),
                  g = (unsigned char)round((double)sum_G / total),
                  b = (unsigned char)round((double)sum_B / total);
    double h = -1.0, s = -1.0, i = -1.0;
    Utils::RGBtoHSI(r,g,b,&h,&s,&i);
    double *hsi = new double[3];
    hsi[0] = h;
    hsi[1] = s;
    hsi[2] = i;
    hsi_map[current] = hsi;   
    
    /*
     * Initializing this meta-region, given it an unique label.
     */
    Metaregion *meta = new Metaregion(k);
    meta->regions.push_back(current);
    
    /*
     * The accumulated sum for each channel.
     */
    meta->rgb_sum[0] = sum_R;
    meta->rgb_sum[1] = sum_G;
    meta->rgb_sum[2] = sum_B;
    /*
     * The total number of positions currently contained by the meta-region.
     */
    meta->pos_sum = total;
    
    /*
     * Track meta-regions that are small enough that may require post-processing ahead.
     */
    if (total <= m_size_threshold_max){
      post_track.insert(meta);
    } //end if
    
    /*
     * Puts the created meta-region in the tracking map, used to later check merged 
     * meta-regions.
     */
    meta_v[current] = meta;
    MetaTrackSet *mt_set = new MetaTrackSet();
    mt_set->insert(k);  
    meta_track[k] = mt_set;
  } //end for
  
  return std::pair<Meta_Vertices,HSI_Map>(meta_v,hsi_map); 
}

// -------------------------------------------------------------------------- //

std::pair< std::pair<double,double> , bool> gnm::Algorithm::LookForGradient( const double *hsi_1, 
									     const double *hsi_2){
  double alpha      = 0;
  double beta       = 0;
  double gamma      = 0;
  double threshold  = 0;
  
  double h1 = hsi_1[0], s1 = hsi_1[1], i1 = hsi_1[2];
  double h2 = hsi_2[0], s2 = hsi_2[1], i2 = hsi_2[2];                               
  
  gnm::Configuration *cfg = &m_perception_config;
  bool clear_perc = true;
  
  /*
   * If any of these conditions is satisfied, the HSI values will be evaluated through
   * rough perception.
   */
  if (s1 < cfg->params[ROUGH_SAT]    
      || s2 < cfg->params[ROUGH_SAT] //poor perception of hue      
      || i1 > cfg->params[MAX_LUMINANCE] 
      || i2 > cfg->params[MAX_LUMINANCE] //excessive intensity
      || i1 < cfg->params[MIN_LUMINANCE] 
      || i2 < cfg->params[MIN_LUMINANCE] //lack of intensity
      ) {
    alpha     = cfg->params[ALPHA_RP];
    beta      = cfg->params[BETA_RP];
    gamma     = cfg->params[GAMMA_RP];
    threshold = cfg->params[THRESHOLD_RP];
    
    clear_perc = false;
  } else {
    /*
     * If not, the HSI values will be evaluated through clear perception.
     */
    alpha     = cfg->params[ALPHA_CP];
    beta      = cfg->params[BETA_CP];
    gamma     = cfg->params[GAMMA_CP];
    threshold = cfg->params[THRESHOLD_CP];
    
    clear_perc = true;
  } //end if
  
  double diffH = std::min( fabs(h1 - h2) , 1.0 - fabs(h1 - h2) );
  double diffS = fabs(s1 - s2);
  double diffI = fabs(i1 - i2);
  
  double energy = ( alpha * diffH ) + ( beta  * diffS ) + ( gamma * diffI );
    
  //return (energy < threshold);
  return std::pair< std::pair<double,double> , bool>( std::pair<double,double>(energy , threshold), clear_perc);
}

// -------------------------------------------------------------------------- //

void gnm::Algorithm::MergeMetaRegionsOf( //Labeling::Region *r1, 
                                         //Labeling::Region *r2,
					Metaregion *meta1,
					Metaregion *meta2,
                                        const Labeling::Vertices& regions, 
                                        Meta_Vertices& meta_regions,
                                        //std::map<Label,MetaTrackSet*>& meta_track){  
                                        std::vector<MetaTrackSet*>& meta_track,
                                        std::set<Metaregion*>& post_track)
{
  /*
   * First recovers the meta-regions which contain regions r1 and r2.
   */
  //Metaregion *meta1  = meta_regions[r1];
  //Metaregion *meta2  = meta_regions[r2];
  
  /*
   * Verifies which meta-region has a smaller label id.
   */
  Label small = std::min(meta1->label_id,meta2->label_id);
  Label big = (small == meta1->label_id) ? meta2->label_id : meta1->label_id;
  
  /*
   * Labels that will
   */
  Label from, to;
  
  /*
   * Gets the merge tracking set for both the small and big meta-regions, respectively.
   * If both meta-regions point to a same tracking set, there is nothing to be done. 
   */
  MetaTrackSet *mts = meta_track[small];
  MetaTrackSet *mtb = meta_track[big];
  if (mts != mtb) {   
    Label aux_s = *(mts->begin());
    Label aux_b = *(mtb->begin());
    
    /*
     * The meta-region with the smallest label in both tracking sets will be selected to:
     *    1. Receive all pixel positions from the regions from the other tracking set.
     *    2. Have its tracking set pointed to by the other tracking set meta-regions.
     * 
     * Always ONLY the pixel positions from the smallest label meta-region from the 'from'
     * tracking set will be copied to the 'to' smallest label meta-region. This ensures 
     * that a correct operation and a faster execution. 
     */
    to    = std::min(aux_s,aux_b);
    from  = (to == aux_s) ? aux_b : aux_s;
    
    MetaTrackSet *mt_from = meta_track[from];
    MetaTrackSet *mt_to   = meta_track[to];  
    for (MetaTrackSet::iterator t = mt_from->begin(); t != mt_from->end(); t++){
      /*
       * Inserting the 'from' tracking set labels in the 'to' tracking set. 
       */
      Label l = *t;
      mt_to->insert(l);
      /*
       * Pointing the tracking sets from 'from' now to 'to' tracking set. 
       */
      meta_track[l] = mt_to;
    } //end for
    
    Metaregion *meta_From = meta_regions[regions[from]];
    Metaregion *meta_To   = meta_regions[regions[to]];
    
    /*
     * Adding the RGB sum and total pixel positions from the smallest label meta-region 
     * from 'from' set to the values from the smallest label meta-region from 'to' set.
     */
    meta_To->rgb_sum[0] += meta_From->rgb_sum[0];
    meta_To->rgb_sum[1] += meta_From->rgb_sum[1];
    meta_To->rgb_sum[2] += meta_From->rgb_sum[2];
    meta_To->pos_sum    += meta_From->pos_sum;
    
    /*
     * Updating post-processing tracker.
     */
    
    //if meta_From is small enough to be tracked, erase it, because now it will 
    //be merged with meta_To 
    if (meta_From->pos_sum <= m_size_threshold_max){
      post_track.erase(meta_From);
    } //end if
    
    //if the merge with meta_From made meta_To too big to be post-processed,
    //erase it from the tracker
    if (meta_To->pos_sum > m_size_threshold_max){
      post_track.erase(meta_To);
    } //end if
    
    /*
     * Copying the pixel positions from the smallest label meta-region from 'from' set to 
     * the values from the smallest label meta-region from 'to' set.
     */
    meta_To->regions.insert( meta_To->regions.end(),
                             meta_From->regions.begin(),meta_From->regions.end());
    /*
     * Clears smallest label meta-region from 'from' set, because it is not responsible
     * anymore for any position. This meta-region will be deleted later.
     */                            
    meta_From->regions.clear(); 
  } //end if
}

// -------------------------------------------------------------------------- //

void gnm::Algorithm::MergeMetaRegionsOf( //Labeling::Region *r1, 
                                         //Labeling::Region *r2,
					 Metaregion *meta1,
					 Metaregion *meta2,
					 const Labeling::Vertices& regions, 
					 Meta_Vertices& meta_regions,
					 //std::map<Label,MetaTrackSet*>& meta_track){  
					 std::vector<MetaTrackSet*>& meta_track){
  /*
   * First recovers the meta-regions which contain regions r1 and r2.
   */
  //Metaregion *meta1  = meta_regions[r1];
  //Metaregion *meta2  = meta_regions[r2];
  
  /*
   * Verifies which meta-region has a smaller label id.
   */
  Label small = std::min(meta1->label_id,meta2->label_id);
  Label big = (small == meta1->label_id) ? meta2->label_id : meta1->label_id;
  
  /*
   * Labels that will
   */
  Label from, to;
  
  /*
   * Gets the merge tracking set for both the small and big meta-regions, respectively.
   * If both meta-regions point to a same tracking set, there is nothing to be done. 
   */
  MetaTrackSet *mts = meta_track[small];
  MetaTrackSet *mtb = meta_track[big];
  if (mts != mtb) {   
    Label aux_s = *(mts->begin());
    Label aux_b = *(mtb->begin());
    
    /*
     * The meta-region with the smallest label in both tracking sets will be selected to:
     *    1. Receive all pixel positions from the regions from the other tracking set.
     *    2. Have its tracking set pointed to by the other tracking set meta-regions.
     * 
     * Always ONLY the pixel positions from the smallest label meta-region from the 'from'
     * tracking set will be copied to the 'to' smallest label meta-region. This ensures 
     * that a correct operation and a faster execution. 
     */
    to    = std::min(aux_s,aux_b);
    from  = (to == aux_s) ? aux_b : aux_s;
    
    MetaTrackSet *mt_from = meta_track[from];
    MetaTrackSet *mt_to   = meta_track[to];  
    for (MetaTrackSet::iterator t = mt_from->begin(); t != mt_from->end(); t++){
      /*
       * Inserting the 'from' tracking set labels in the 'to' tracking set. 
       */
      Label l = *t;
      mt_to->insert(l);
      /*
       * Pointing the tracking sets from 'from' now to 'to' tracking set. 
       */
      meta_track[l] = mt_to;
    } //end for
    
    Metaregion *meta_From = meta_regions[regions[from]];
    Metaregion *meta_To   = meta_regions[regions[to]];
    
    /*
     * Adding the RGB sum and total pixel positions from the smallest label meta-region 
     * from 'from' set to the values from the smallest label meta-region from 'to' set.
     */
    meta_To->rgb_sum[0] += meta_From->rgb_sum[0];
    meta_To->rgb_sum[1] += meta_From->rgb_sum[1];
    meta_To->rgb_sum[2] += meta_From->rgb_sum[2];
    meta_To->pos_sum    += meta_From->pos_sum;
    
    /*
     * Copying the pixel positions from the smallest label meta-region from 'from' set to 
     * the values from the smallest label meta-region from 'to' set.
     */
    meta_To->regions.insert( meta_To->regions.end(),
			     meta_From->regions.begin(),meta_From->regions.end());
    /*
     * Clears smallest label meta-region from 'from' set, because it is not responsible
     * anymore for any position. This meta-region will be deleted later.
     */                            
    meta_From->regions.clear(); 
  } //end if
}

// -------------------------------------------------------------------------- //

void gnm::Algorithm::DoPostProcessing( const Labeling::Vertices& regions, 
				       Meta_Vertices& meta_regions,          									
				       std::vector<MetaTrackSet*>& meta_track,
				       std::set<Metaregion*>& post_track)
{
  
  for (std::set<Metaregion*>::iterator m=post_track.begin(); m != post_track.end(); m++){		
    Metaregion* meta_r = *m;
    
    Labeling::Vertices regions_r = meta_r->regions;
    
    Metaregion *chosen = NULL; //metaregion selected to merge to. maybe no region will.
    double min_diff = 1.0;
    
    Metaregion *largest = NULL; //biggest neighbor found.
    unsigned max_size = 0;
    
    unsigned int total_r = meta_r->pos_sum;
    unsigned char r_r = (unsigned char)round((double)meta_r->rgb_sum[0] / total_r),
      g_r = (unsigned char)round((double)meta_r->rgb_sum[1] / total_r),
      b_r = (unsigned char)round((double)meta_r->rgb_sum[2] / total_r);
    
    double h_r = -1.0, s_r = -1.0, i_r = -1.0;
    Utils::RGBtoHSI(r_r,g_r,b_r,&h_r,&s_r,&i_r);
    
    for (Labeling::Vertices::const_iterator i=regions_r.begin(); i != regions_r.end(); i++){
      Labeling::Region *r = *i;
      
      for (std::set<Labeling::Region*>::iterator j = r->neighbors.begin(); j != r->neighbors.end(); j++){
	Labeling::Region *q = *j;
	
	Metaregion *meta_q = meta_regions[q];
	
	if (meta_track[meta_r->label_id] != meta_track[meta_q->label_id]){
	  unsigned int total_q = meta_q->pos_sum;
	  unsigned char r_q = (unsigned char)round((double)meta_q->rgb_sum[0] / total_q),
	    g_q = (unsigned char)round((double)meta_q->rgb_sum[1] / total_q),
	    b_q = (unsigned char)round((double)meta_q->rgb_sum[2] / total_q);
	  
	  double h_q = -1.0, s_q = -1.0, i_q = -1.0;
	  Utils::RGBtoHSI(r_q,g_q,b_q,&h_q,&s_q,&i_q);
	  
	  if (total_q > max_size)
	    {
	      largest = meta_q;
	      max_size = total_q;
	    } //end if
	  
	  if (total_q > m_size_threshold_max)
	    {	    			
	      double diff = (s_r < 0.2) ? fabs(i_r - i_q) : fabs(h_r - h_q);
	      
	      if (diff < min_diff && diff <= m_color_threshold){
		chosen = meta_q;
		min_diff = diff;
	      } //end if		    		
	    } //end if
	} //end if

      } //end for			
    } //end for
    
    if (chosen != NULL){           
      MergeMetaRegionsOf(meta_r,chosen,regions,meta_regions,meta_track);
    } else if (total_r <= m_size_threshold_min && largest != NULL) {
      MergeMetaRegionsOf(meta_r,largest,regions,meta_regions,meta_track);
    } //end if
  } //end for
}

// -------------------------------------------------------------------------- //

gnm::Results gnm::Algorithm::BuildOutput( const Labeling::Vertices& regions, 
					  Meta_Vertices& meta_regions,
					  //std::map<Label,MetaTrackSet*>& meta_track,
					  std::vector<MetaTrackSet*>& meta_track,
					  const int width,
					  const int height
					  )
{
  Results results;
  Pixel_value *output = new Pixel_value[width * height * 3];
  
  /*
   * A variable that will be used to check if no more operations are necessary, as
   * all meta-regions might have been already checked.
   */
  int countdown = meta_track.size();
  
  /*
   * Auxiliar vector used to store if a meta-region was already checked.
   */
  std::vector<bool> done;
  done.reserve(countdown);
  done.resize(countdown,false);
 
  int count_uid = 0; //only necessary if stats dumped 
  std::ostringstream sdump; //only necessary if stats dumped
  int *uid_img = (m_borders) ? new int[width * height] : NULL;
 
  for (unsigned i = 0; i < meta_track.size() && countdown > 0; i++){
    Label l = i;
    
    if (!done[l]) {
      /*
       * The smallest meta-region from each tracking set will contain all pixel positions
       * and necessary information to calculate the mean color from all meta-regions contained
       * by this tracking set. 
       */    
      //MetaTrackSet *mt = (*i).second;
      MetaTrackSet *mt = meta_track[l];
      Label smallest = *(mt->begin());
      Metaregion *meta = meta_regions[regions[smallest]];
      
      Paint(output,meta);
      if (m_dump) DumpStep(&sdump,count_uid,width,height,meta);
      if (m_borders) BordersStep(uid_img,count_uid,meta);
      ++count_uid;
      
      /*
       * Sets as checked.
       */
      done[meta->label_id] = true;
      
      /*
       * Decreases countdown by the number of meta-regions that the current tracking set
       * cointains.
       */    
      countdown -= mt->size();
      
      /*
       * As there is nothing else to be done with this meta-region, erases it from the 
       * tracking set and deletes it.
       */
      mt->erase(smallest);
      delete meta;
      
      /*
       * Now, for the rest of the tracking set, check them all as done and delete them.
       */
      for (MetaTrackSet::iterator j = mt->begin(); j != mt->end(); j++){
        Label bigger = *j;
        
        done[bigger] = true;
        delete meta_regions[regions[bigger]];
      };
      /*
       * This tracking set has no use anymore, so it is deleted. 
       */
      delete mt;
    } //end if
  } //end for

  results.output_image = output;
  if (m_dump) {
    std::ostringstream dump;
    dump << "Total;" << (count_uid) << "\n";
    dump << "Label;N_Pixels;R;G;B\n";
    dump << sdump.str();

    results.stats_dump = dump.str();
  } //end if

  if (m_borders) {
    Pixel_value *border_img = new Pixel_value[width * height * 3];
    for (int i=0; i<width*height; i++) {
      int count_n = 0;

      int tmp_row = i / width;
      int tmp_col = i % width;
      

      if (tmp_col == 0 || uid_img[i] == uid_img[i-1]) ++count_n; //left
      if (tmp_row == 0 || uid_img[i] == uid_img[i-width]) ++count_n; //top
      if (tmp_col == width-1 || uid_img[i] == uid_img[i+1]) ++count_n; //right
      if (tmp_row == height-1 || uid_img[i] == uid_img[i+width]) ++count_n; //bottom
      
      Pixel_value color = (count_n == 4) ? 255 : 0;
      int pos = i * 3;
      border_img[pos]   = color;
      border_img[pos+1] = color;
      border_img[pos+2] = color;      
    } //end for

    results.border_image = border_img;
    delete[] uid_img;
  } //end if
  
  return results;
}

// -------------------------------------------------------------------------- //

void gnm::Algorithm::Paint(Pixel_value *image, Metaregion *meta_region)
{
  Labeling::Vertices regions = meta_region->regions;
  unsigned int total = meta_region->pos_sum;
  unsigned char r = (unsigned char)round((double)meta_region->rgb_sum[0] / total),
    g = (unsigned char)round((double)meta_region->rgb_sum[1] / total),
    b = (unsigned char)round((double)meta_region->rgb_sum[2] / total);  
  
  /*
   * Runs through the labeled regions this meta-region contains.
   */
  for (unsigned int i=0; i<regions.size(); i++){
    std::set<Index> pos = regions[i]->positions;
  
    /*
     * Runs through the pixel positions the current labeled region contains.
     */
    for (std::set<Index>::iterator it=pos.begin(); it!=pos.end(); it++){            
      Index p = *it * 3;
      
      /*
       * This define is only to avoid a problem with wxImage save method.
       */
#ifdef WX_MESSED_RGB
      image[ p ]      = g;
      image[ p + 1 ]  = b;
      image[ p + 2 ]  = r;
#else
      image[ p ]      = r;
      image[ p + 1 ]  = g;
      image[ p + 2 ]  = b;
#endif
    } //end for    
  } //end for
}

// -------------------------------------------------------------------------- //

void gnm::Algorithm::DumpStep( std::ostream *datadump, int uid, int width, int height, Metaregion *meta_region)
{	
  Labeling::Vertices regions = meta_region->regions;
  unsigned int total = meta_region->pos_sum;
  unsigned char r = (unsigned char)round((double)meta_region->rgb_sum[0] / total),
    g = (unsigned char)round((double)meta_region->rgb_sum[1] / total),
    b = (unsigned char)round((double)meta_region->rgb_sum[2] / total);  
  
  (*datadump) << uid << ";" << meta_region->pos_sum << ";" << (unsigned)r << ";" << (unsigned)g << ";" << (unsigned)b << "\n";
}

// -------------------------------------------------------------------------- //

void gnm::Algorithm::BordersStep( int *uid_image, int uid, Metaregion *meta_region)
{
  Labeling::Vertices regions = meta_region->regions;
  
  /*
   * Runs through the labeled regions this meta-region contains.
   */
  for (unsigned int i=0; i<regions.size(); i++){
    std::set<Index> pos = regions[i]->positions;
  
    /*
     * Runs through the pixel positions the current labeled region contains.
     */
    for (std::set<Index>::iterator it=pos.begin(); it!=pos.end(); it++){            
      Index p = *it;
      
      uid_image[p] = uid;
    } //end for    
  } //end for
}

// PRIVATE METHODS ---------------------------------------------------------- //

// IMPLEMENTATION CLASS gnm::Algorithm -------------------------------------- //
