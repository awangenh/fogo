/**
 * \file Labeling.hpp
 *
 * Header defining a group of functions for a labeling algorithm.
 *
 * $Rev $
 * $Date 11/13/2007$
 * $Author Fogo!$
 */

// -------------------------------------------------------------------------- //
// DEFINE's                                                                   //
// -------------------------------------------------------------------------- //

#ifndef LABELING_HPP_
#define LABELING_HPP_

// DEFINE's ----------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// INCLUDE's                                                                  //
// -------------------------------------------------------------------------- //

//std stuff
#include <string>
#include <vector>
#include <set>

//my stuff
#include "Labeling/Region.hpp"

// INCLUDE's ---------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// TYPEDEF's                                                                  //
// -------------------------------------------------------------------------- //

/*
 * \typedef Pixel_value
 * Unit value found in images.
 */
typedef unsigned char Pixel_value;
/*
 * \typedef TrackSet
 * Set used to store and sort a group of equivalent labels.
 */
typedef std::set<Label>   TrackSet;

// TYPEDEF's ---------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// STRUCT Neighbors_Pair                                                      //
// -------------------------------------------------------------------------- //

/*
 * \brief A pair of labeled regions that share a neighborhood relationship.
 * \struct Neighbors_Pair
 * 
 * Auxiliar structure used in storage and sorting in sets.
 */
struct Neighbors_Pair
{
  Neighbors_Pair(Label lbl1, Label lbl2);
  
  bool operator==(const Neighbors_Pair& other_eq) const;
  bool operator<(const Neighbors_Pair& other_eq) const;
  
  Label smaller, bigger;  
};

// STRUCT Neighbors_Pair ----------------------------------------------------- //

// -------------------------------------------------------------------------- //
// NAMESPACE Labeling                                                         //
// -------------------------------------------------------------------------- //

namespace Labeling
{	
	/*
	 * \typedef Vertices
	 * A vector of labeled regions, linked through \link Region pointers.
	 */
	typedef std::vector<Labeling::Region*> Vertices;

	// -------------------------------------------------------------------------- //
	// CLASS Provider                                                             //
	// -------------------------------------------------------------------------- //
	
	/*
	 * \brief Responsible for providing the labeled data of an image.
	 * \class Provider
	 */
	class Provider
	{		  
		protected:
			
			// -------------------------------------------------------------------------- //
			// STRUCT LabelingData                                                        //
			// -------------------------------------------------------------------------- //
			
			/*
			 * \brief Stores several data resulting from a labeling.
			 * \struct LabelingData
			 */
			struct LabelingData
			{
				Labeling::Vertices 				regs;						//all regions labeled, including redundants
				std::set<Neighbors_Pair> 	neighbor_lbls;	//all neighborhood relationships
				std::vector<int> 				replace;				//replaced labels during equivalence elimination
				std::set<Label> 					erased;					//labels to be eliminated
			};
			
			// STRUCT LabelingData ------------------------------------------------------ //
			
			/**
	     * \brief Performs the labeling over an image.
	     *
	     * \param image			Image to be labeled.
	     * \param width			Image width.
	     * \param height		Image height.
	     * \param channels	Number of channels of the image.
	     * 
	     * \return A \link LabelingData containing the resulting labeling data.      
	     */ 
			static LabelingData DoLabeling(Pixel_value* image,
															          unsigned int width,
															          unsigned int height,
															          unsigned int channels);                                                                                                    
		private:
			/**
	     * \brief Lists labels that can be eliminated as there are other labels equivalent.
	     *
	     * \param regs			All the labeled regions.
	     * \param track			A map linking labels to a \link TrackSet pointer.
	     * \param replace		Replaced labels.
	     * 
	     * \return A set of labels identifying regions to be eliminated.      
	     */
			static inline std::set<Label> SolveEquivalences(Labeling::Vertices& regs,
		                                std::map<Label,TrackSet*>& track,
		                                std::vector<int>& replace);
			/*
			 * \brief Stores data identifying two labels as equivalent.
	     *
	     * \param l1				A label.
	     * \param l2				Other label.
	     * \param track			A map linking labels to a \link TrackSet pointer.
	     * \param storage		Container of \link TrackSet pointer. 
			 */
			static inline void CreateEquivalence(Label l1,
		                                Label l2,
		                                std::map<Label,TrackSet*>& track,
		                                std::vector<TrackSet*>& storage); 
			
			/*
			 * \brief Auxiliar test method. Shows the labeling result as a string.
	     *
	     * \param regs			All the labeled regions.
	     * \param width			Image width.
	     * \param height		Image height.
	     * 
	     * \return A string with the labeling result.   
			 */
			static std::string AsString(const Labeling::Vertices& regs,
		                  unsigned int width,
		                  unsigned int height);
	};
	
	// CLASS Provider ------------------------------------------------------------//	
	
};

// NAMESPACE Labeling --------------------------------------------------------//

#endif /*LABELING_HPP_*/
