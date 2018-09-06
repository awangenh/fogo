/**
 * \file Segmentation.hpp
 *
 * Header for the \link Algorithm class and \link Configuration struct, both contained by
 * \link gnm namespace.
 *
 * $Rev $
 * $Date 04/03/2007$
 * $Author Fogo!$
 */

// -------------------------------------------------------------------------- //
// DEFINE's                                                                   //
// -------------------------------------------------------------------------- //

#ifndef SEGMENTATION
#define SEGMENTATION

#define GNM_TOTAL_PARAMS 11

// DEFINE's ----------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// INCLUDE's                                                                  //
// -------------------------------------------------------------------------- //

//stdlib stuff
#include <sstream>
#include <map>

//my stuff
#include <Labeling/Region.hpp>
#include <Graph/GraphLabeling.hpp>
#include "Metaregion.hpp"

// INCLUDE's ---------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// ENUMERATION's                                                              //
// -------------------------------------------------------------------------- //

/* \enum GNMIndex
 * An enumaration of the GNM configuration parameters. 
 *
 * \var GNMIndex GNMIndex::ALPHA_CP
 * The parameter that defines the relevance of the hue in clear perception.
 * 
 * \var GNMIndex GNMIndex::BETA_CP
 * The parameter that defines the relevance of the saturation in clear perception.
 * 
 * \var GNMIndex GNMIndex::GAMMA_CP
 * The parameter that defines the relevance of the intensity in clear perception.
 * 
 * \var GNMIndex GNMIndex::THRESHOLD_CP
 * The parameter that defines the threshold used for evaluation in clear perception.
 * 
 * \var GNMIndex GNMIndex::ALPHA_RP
 * The parameter that defines the relevance of the hue in rough perception.
 * 
 * \var GNMIndex GNMIndex::BETA_RP
 * The parameter that defines the relevance of the saturation in rough perception.
 * 
 * \var GNMIndex GNMIndex::GAMMA_RP
 * The parameter that defines the relevance of the intensity in rough perception.
 * 
 * \var GNMIndex GNMIndex::THRESHOLD_RP
 * The parameter that defines the threshold used for evaluation in rough perception.
 * 
 * \var GNMIndex GNMIndex::ROUGH_SAT
 * The parameter that defines the threshold used to evaluate saturation and define perception.
 * 
 * \var GNMIndex GNMIndex::MIN_LUMINANCE
 * The parameter that defines the threshold used to define lesser values as rough perception.
 * 
 * \var GNMIndex GNMIndex::MAX_LUMINANCE
 * The parameter that defines the threshold used to define bigger values as rough perception.
 */
 
enum GNMIndex { ALPHA_CP = 0,BETA_CP = 1,GAMMA_CP = 2, THRESHOLD_CP = 3,
                ALPHA_RP = 4,BETA_RP = 5,GAMMA_RP = 6, THRESHOLD_RP = 7,   
                ROUGH_SAT = 8,MIN_LUMINANCE = 9,MAX_LUMINANCE = 10 };
                
// ENUMERATION's ------------------------------------------------------------ //

// -------------------------------------------------------------------------- //
// TYPEDEF's                                                                  //
// -------------------------------------------------------------------------- //

/*
 * \typedef RGB_Map
 * A mapping among \link Labeling::Region pointers and RGB pixel values.
 */
typedef std::map<Labeling::Region*,Pixel_value*> RGB_Map;
/*
 * \typedef HSI_Map
 * A mapping among \link Labeling::Region pointers and HSI pixel values.
 */
typedef std::map<Labeling::Region*,double*>      HSI_Map;
/*
 * \typedef MetaTrackSet
 * A set used to track labels related to metaregions.
 */
typedef std::set<Label>                          MetaTrackSet;

// TYPEDEF's ---------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// NAMESPACE gnm                                                              //
// -------------------------------------------------------------------------- //

/*
 * \namespace gnm
 * The namespace that will store the functions, classes and other related stuff to
 * the GNM framework.
 */
namespace gnm {

  // ------------------------------------------------------------------------ //
  // STRUCT gnm::Configuration                                                //
  // ------------------------------------------------------------------------ //
  
  /**
   * \brief A struct used to store the configuration for GNM algorithm parameters.
   * \struct Configuration
   *
   * Contains an array of the size of the total number of parameters of GNM algorithm,
   * easily accessed by the \link GNMIndex enumeration.
   */
  struct Configuration{
    Configuration(){
      params[ALPHA_CP]      = 0.7; 
      params[BETA_CP]       = 0.2;
      params[GAMMA_CP]      = 0.1;
      params[ALPHA_RP]      = 0.02;
      params[BETA_RP]       = 0.0;
      params[GAMMA_RP]      = 0.2;
      params[THRESHOLD_CP]  = 0.8;
      params[THRESHOLD_RP]  = 0.08;
      params[ROUGH_SAT]     = 0.1;
      params[MIN_LUMINANCE] = 0.2;
      params[MAX_LUMINANCE] = 0.95;
    };
    
    Configuration( const int& uid,
		   const double& a_cp, const double& b_cp, const double& g_cp,
		   const double& a_rp, const double& b_rp, const double& g_rp,
		   const double& t_cp, const double& t_rp,
		   const double& r_s, const double& r_minL, const double& r_maxL){
      params[ALPHA_CP]      = a_cp; //coefficient related to HUE - CLEAR PERCEPTION 
      params[BETA_CP]       = b_cp; //coefficient related to SATURATION - CLEAR PERCEPTION
      params[GAMMA_CP]      = g_cp; //coefficient related to INTENSITY - CLEAR PERCEPTION
      params[ALPHA_RP]      = a_rp; //coefficient related to HUE - ROUGH PERCEPTION
      params[BETA_RP]       = b_rp; //coefficient related to SATURATION - ROUGH PERCEPTION
      params[GAMMA_RP]      = g_rp; //coefficient related to INTENSITY - ROUGH PERCEPTION
      params[THRESHOLD_CP]  = t_cp; //threshold CLEAR PERCEPTION
      params[THRESHOLD_RP]  = t_rp; //threshold ROUGH PERCEPTION
      params[ROUGH_SAT]     = r_s;    //threshold to rough perception for saturation
      params[MIN_LUMINANCE] = r_minL; //minimum threshold to rough perception for intensity
      params[MAX_LUMINANCE] = r_maxL; //maximum threshold to rough perception for intensity
    };
        
    double params[GNM_TOTAL_PARAMS];
  };
  
  // STRUCT gnm::Configuration ---------------------------------------------- //
  
  // ------------------------------------------------------------------------ //
  // CLASS gnm::EvalEdge                                                      //
  // ------------------------------------------------------------------------ //
  
  class EvalEdge : public std::pair<Labeling::Edge,double>{
  public:
    bool operator==(const EvalEdge& ee) const {
      return second == ee.second;
    }
    
    bool operator<(const EvalEdge& ee) const {
      return second < ee.second;
    }
  };
  
  // CLASS EvalEdge --------------------------------------------------------- //

  // ------------------------------------------------------------------------ //
  // STRUCT gnm::Results                                                      //
  // ------------------------------------------------------------------------ //
  
  struct Results {
    Results() : output_image(NULL), border_image(NULL), stats_dump("") {};
    ~Results() {
      if (output_image != NULL) delete[] output_image;
      if (border_image != NULL) delete[] border_image;
    };

    Pixel_value  *output_image;
    Pixel_value  *border_image;
    std::string  stats_dump;
  };
  
  // STRUCT gnm::Results ---------------------------------------------------- //
  
  // ------------------------------------------------------------------------ //
  // CLASS gnm::Algorithm                                                     //
  // ------------------------------------------------------------------------ //
  
  /**
   * \brief The class responsible for the GNM algorithm execution.
   * \class Algorithm
   */
  class Algorithm{
  public:
    Algorithm() : m_size_threshold_min(10), m_size_threshold_max(50), m_color_threshold(0.5) {};
    Algorithm( const unsigned size_threshold_min, const unsigned size_threshold_max, double color_threshold)
      : m_size_threshold_min(size_threshold_min), m_size_threshold_max(size_threshold_max), m_color_threshold(color_threshold) {};
       
    /**
     * \brief Executes the GNM, without support to a log stream.
     *
     * \param labelled    The labeling result of the original RGB image.
     * \param original    An RGB image.
     * \param width       The image width.
     * \param height      The image height.
     * \param regions     The similarity regions identified in the labeling process.
     * \param edges       The neighborhood between the similarity regions identified in 
     *                    the labeling process.
     * \param perception  The perception parameters to be used in the process.
     * 
     * \return The output image of the algorithm.       
     */
    Results Execute( const Pixel_value *labelled, //RGB
		     const Pixel_value *original, //RGB
		     const int width,
		     const int height,
		     const Labeling::Vertices& regions,
		     const Labeling::Edges& edges,              
		     const Configuration& perception,
		     const bool borders = false, 
		     const bool dump = false,
		     const bool post_processing = true
		     );
  private:
    /**
     * \brief Starts the process of GNM algorithm.
     *
     * \param labelled    The labeling result of the original RGB image.
     * \param original    An RGB image.
     * \param width       The image width.
     * \param height      The image height.
     * \param regions     The similarity regions identified in the labeling process.
     * \param edges       The neighborhood between the similarity regions identified in 
     *                    the labeling process.
     * 
     * \return The output image of the algorithm.       
     */
    Results Start( const Pixel_value *labelled, //RGB
		   const Pixel_value *original, //RGB
		   const int width,
		   const int height,
		   const Labeling::Vertices& regions,
		   const Labeling::Edges& edges,              
		   const Configuration& perception
		   );
      
    /**
     * \brief Configures and initializes some stuff that will be used during the process,
     *        like mappings and HSI converted pixel values. 
     *
     * \param regions     The similarity regions identified in the labeling process.
     * \param meta_track  A map used to track the merging of meta-regions.
     * \param post_track  A set that stores pointers to the metaregions selected for
     *                    post-processing steps.
     * \param labelled    The labeling result of the original RGB image.
     * \param original    An RGB image.
     * \param width       The image width.
     * \param height      The image height.
     * 
     * \return A pair composed of a \link Meta_Vertices and a \link HSI_Map, respectively.
     */
    std::pair<Meta_Vertices,HSI_Map> InitializeMetaregions( const Labeling::Vertices& regions,
							    //std::map<Label,MetaTrackSet*>& meta_track,
							    std::vector<MetaTrackSet*>& meta_track,
							    std::set<Metaregion*>& post_track,
							    const Pixel_value* original, //RGB
							    const int width,
							    const int height                 
							    );
    /**
     * \brief Check if the gradient between this two HSI values is acceptable according to
     *        the perception. 
     *
     * \param hsi_1 A HSI value.
     * \param hsi_2 A HSI value.
     * 
     * \return A pair with, respectively, the result of the perception function and the proper threshold.
     */                                 
    inline std::pair< std::pair<double,double> , bool> LookForGradient( const double *hsi_1, 
									const double *hsi_2
									);
    /**
     * \brief Merge two metaregions in one. 
     *
     * \param meta1 A metaregion.
     * \param meta2 Another metaregion.
     * \param regions The vertices of the graph.
     * \param meta_regions A vector of special regions which group regions 
     *                     found in the labeling..
     * \param meta_track A map used to track the merging of meta-regions.
     * \param post_track  A set that stores pointers to the metaregions selected for
     *                    post-processing steps.
     */                                  
    inline void MergeMetaRegionsOf( //Labeling::Region *r1, 
				   //Labeling::Region *r2,
				   Metaregion *meta1,
				   Metaregion *meta2,
				   const Labeling::Vertices& regions, 
				   Meta_Vertices& meta_regions,
				   //std::map<Label,MetaTrackSet*>& meta_track);
				   std::vector<MetaTrackSet*>& meta_track,
				   std::set<Metaregion*>& post_track
				    );
    
    /**
     * \brief Merge two metaregions in one. 
     *
     * \param meta1 A metaregion.
     * \param meta2 Another metaregion.
     * \param regions The vertices of the graph.
     * \param meta_regions A vector of special regions which group regions 
     *                     found in the labeling..
     * \param meta_track A map used to track the merging of meta-regions. 
     */  
    inline void MergeMetaRegionsOf( Metaregion *meta1,
				    Metaregion *meta2,
                                    const Labeling::Vertices& regions, 
                                    Meta_Vertices& meta_regions,                                    
                                    std::vector<MetaTrackSet*>& meta_track
				    );
    
    /**
     * \brief Performs the post-processing, eliminating unnecessary regions. 
     *
     * \param regions The vertices of the graph.
     * \param meta_regions A vector of special regions which group regions 
     *                     found in the labeling..
     * \param meta_track A map used to track the merging of meta-regions.
     * \param post_track  A set that stores pointers to the metaregions selected for
     *                    post-processing steps.
     */
    void DoPostProcessing( const Labeling::Vertices& regions, 
			   Meta_Vertices& meta_regions,          									
			   std::vector<MetaTrackSet*>& meta_track,
			   std::set<Metaregion*>& post_track
			   );
      
    /**
     * \brief Builds the image that will returned in the end of the process. 
     *
     * \param regions      The similarity regions identified in the labeling process.
     * \param meta_regions A vector of special regions which group regions 
     *                     found in the labeling.
     * \param meta_track   A map used to track the merging of meta-regions.
     * \param width        The image width.
     * \param height       The image height.
     * \param segdump	   Pointer to the segmentation dump stream.
     * 
     * \return True if acceptable, false if not.
     */      
    Results BuildOutput( const Labeling::Vertices& regions, 
			 Meta_Vertices& meta_regions,
			 //std::map<Label,MetaTrackSet*>& meta_track,
			 std::vector<MetaTrackSet*>& meta_track,
			 const int width,
			 const int height
			 );                         
    /**
     * \brief Paint the positions contained by the meta-region in the image. 
     *
     * The color used to paint the image is the mean color of the regions contained
     * by the meta-region.
     * 
     * \param image       An image.
     * \param meta_region A special region which group regions found in the labeling.
     */                               
    inline void Paint( Pixel_value *image, Metaregion *meta_region); 
      
    /**
     * \brief Step to generate a segmentation map and dumps data about every segment.     
     * 
     * \param datadump    A data dump.
     * \param uid					An unique identifier.
     * \param width				Image width.
     * \param height			Image height.
     * \param meta_region A special region which group regions found in the labeling.
     */                               
    inline void DumpStep( std::ostream *datadump, int uid, int width, int height, Metaregion *meta_region);

    inline void BordersStep( int *uid_image, int uid, Metaregion *meta_region);
      
    Configuration m_perception_config;
    bool          m_verbose;
    bool	  m_post_processing;
    bool          m_dump;
    bool          m_borders;
    std::ostream *m_log_stream;
      
    unsigned m_size_threshold_min;
    unsigned m_size_threshold_max;
    double 	 m_color_threshold;
  };

  // CLASS gnm::Algorithm --------------------------------------------------- //
  
};

// NAMESPACE gnm ------------------------------------------------------------ //

#endif
