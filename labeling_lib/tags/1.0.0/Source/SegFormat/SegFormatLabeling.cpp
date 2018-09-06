// -------------------------------------------------------------------------- //
// INCLUDE's                                                                  //
// -------------------------------------------------------------------------- //

//my stuff
#include "SegFormat/SegFormatLabeling.hpp"

// INCLUDE's ---------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// CLASS SegFormatProvider                                                    //
// -------------------------------------------------------------------------- //

Labeling::SegFormatProvider::SegFormat Labeling::SegFormatProvider::AsSegFormat(Pixel_value* image,
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
		
	return Convert(data.regs,width,height);
}

// -------------------------------------------------------------------------- //

Labeling::SegFormatProvider::SegFormat Labeling::SegFormatProvider::Convert(const Labeling::Vertices& regs,
																																								unsigned int width,
																																								unsigned int height)
{
  SegFormat seg(width,height,regs.size());

  for (Label l = 0; l < regs.size(); l++){  	
  	std::set<Index> *pos = &(regs[l]->positions);
  	  	
  	int former	= -1;
  	int start_col	= -1;
  	for (std::set<Index>::iterator it = pos->begin(); it != pos->end(); it++){
  		Index current = *it;
  		
  		if (start_col == -1) start_col = current % width;
  		
  		if (former > 0)
  		{
  			unsigned row_current	= current / width;
  			unsigned row_former		= former / width;
  			unsigned col_current	= current % width;
  			unsigned col_former		= former % width;
  			
	  		if ( (row_current > row_former) ||
	  				 (col_current - col_former > 1) ){
	  			/*
	  			 * If there is a difference of more than one row between both positions
	  			 */  		
	  			/*
					 * if col_current - col_former > 1 means there is other region between these two 
					 * positions
					 */	
	  			
	  			SegLine sl(start_col,col_former,row_former,l);  			
	  			seg.data.push_back(sl);
	  				  			
	  			start_col = col_current;	  				  			
	  		} //end if  			
	  		
  		} //end if
  		
  		former = current;
  	} //end for
  	
  	SegLine sl(start_col,former % width,former / width,l);  			
		seg.data.push_back(sl);
  } //end for
  
  return seg;
}

// -------------------------------------------------------------------------- //



