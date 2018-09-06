/**
 * \file App.hpp
 *
 * Header for a program capable of converting image files to the segmentation 
 * file format provided by the The Berkeley Segmentation Dataset and Benchmark.
 *
 * $Rev $
 * $Date 11/13/2007$
 * $Author Fogo!$
 */

//TODO: falta implementar parâmetros FORMAT, GRAY, INVERT e FLIPFLOP.

// -------------------------------------------------------------------------- //
// DEFINE's                                                                   //
// -------------------------------------------------------------------------- //

#ifndef SEGAPP_HPP_
#define SEGAPP_HPP_

// DEFINE's ----------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// INCLUDE's                                                                  //
// -------------------------------------------------------------------------- //

//wx stuff
#include <wx/wx.h>

//std stuff
#include <string>

//my stuff
#include "SegFormat/SegFormatLabeling.hpp"

// INCLUDE's ---------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// CLASS SegApp                                                               //
// -------------------------------------------------------------------------- //

/**
 * \brief The image to seg format file converter program.
 * \class SegApp
 *
 * This program is a console application and uses wxWidgets to load image files.
 */
class SegApp : public wxApp {
public:
  /**
   * \brief Corresponds to the main of a program extending wxApp.       
   * 
   * \return The condition of the program exit.       
   */ 
  virtual bool OnInit();
private:
  
  /**
   * \brief Process the parameters given by the command line.
   *
   * \param in_img	The input image filename.
   * \param out_count	Counts the number of ocurrences of the out_seg parameter.
   * \param out_seg	The output segmentation file filename.
   * \param uid_count	Counts the number of ocurrences of the uid parameter.
   * \param uid		The user id.
   * \param iid_count	Counts the number of ocurrences of the iid parameter.
   * \param iid		The image id.
   * \param date_count	Counts the number of ocurrences of the date parameter.
   * \param date	A date.
   * \param format	Date format. See std function strptime for details about it.
   * 
   * \return True if it was possible to use all given parameters correctly, false if not.       
   */ 
  bool Process_commandline( const char *in_img,
			    int out_count, const char *out_seg,
			    int uid_count, int uid,
			    int iid_count, int iid,
			    int date_count, struct tm *date, const char *format);
		
  /**
   * \brief Stores the segmentation data in a file.
   *
   * \param path		The output file path.
   * \param seg    	The segmentation data.
   * \param uid			The user id. Optional.
   * \param iid			The image id. Optional.
   * \param date		A date string. Optional.
   * 
   * \return True if the data was stored correctly, false if not.       
   */ 
  int ToFile( const std::string& path,
	      const Labeling::SegFormatProvider::SegFormat& seg,									
	      const std::string& uid = "",
	      const std::string& iid = "",
	      const std::string& date = "");
		
  /**
   * \brief Puts the segmentation data in the console.
   *
   * \param seg    	The segmentation data.
   * \param uid			The user id. Optional.
   * \param iid			The image id. Optional.
   * \param date		A date string. Optional.
   * 
   * \return True if the data was output correctly, false if not.       
   */ 
  int ToConsole( const Labeling::SegFormatProvider::SegFormat& seg,
		 const std::string& uid = "",
		 const std::string& iid = "",
		 const std::string& date = "");

  /**
   * \brief Puts the segmentation data in the desired container.
   *
   * \param seg    	The segmentation data.
   * \param uid			The user id. Optional.
   * \param iid			The image id. Optional.
   * \param date		A date string. Optional.
   * 
   * \return True if the data was output correctly, false if not.       
   */
  int Save( std::ostream *out,
	    const Labeling::SegFormatProvider::SegFormat& seg,
	    const std::string& uid = "",
	    const std::string& iid = "",
	    const std::string& date = "");
};

// CLASS SegApp  ------------------------------------------------------------ //

#endif /*SEGAPP_HPP_*/
