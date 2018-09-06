// -------------------------------------------------------------------------- //
// INCLUDE's                                                                  //
// -------------------------------------------------------------------------- //

//std stuff
#include <iostream>
#include <sstream>
#include <fstream>

//my stuff
#include "argtable2/argtable2.h"
#include "App.hpp"

// -------------------------------------------------------------------------- //
// INCLUDE's                                                                  //
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// CLASS SegApp IMPLEMENTATION                                                //
// -------------------------------------------------------------------------- //

IMPLEMENT_APP(SegApp)

// -------------------------------------------------------------------------- //

bool SegApp::OnInit()
{
  struct arg_file *p_in_img 	= arg_file1("i","input","<image_file>",	"input image filename (PNG,JPEG,BMP,PNM)");
  struct arg_file *p_out_seg = arg_file0("o","output","<seg_file>",		"output seg file filename. If no name is given, output is put in the console");
  struct arg_int  *p_uid  		= arg_int0(NULL,"uid","<id_number>",			"user id number");
  struct arg_int  *p_iid  		= arg_int0(NULL,"iid","<id_number>",			"image id number");
  struct arg_date *p_date			= arg_date0("d", "date", "%D", "<date>", "date as MM/DD/YY");
  struct arg_lit  *p_help    	= arg_lit0("h","help",                   "print this help and exit");
  struct arg_lit  *p_version 	= arg_lit0("v","version",                "print version information and exit");
  struct arg_end  *p_end     	= arg_end(20);
  void* argtable[] = {p_in_img,p_out_seg,p_uid,p_iid,p_date,p_help,p_version,p_end};
  
  const char* progname = "imgtoseg";
  int nerrors;	
	
  /* verify the argtable[] entries were allocated sucessfully */
  if (arg_nullcheck(argtable) != 0) {
    /* NULL entries were detected, some allocations must have failed */
    std::cout << progname << ": insufficient memory\n";
    
    return 1;
  }
		
  /* Parse the command line as defined by argtable[] */
  nerrors = arg_parse(argc,argv,argtable);
	
  /* special case: '--help' takes precedence over error reporting */
  if (p_help->count > 0) {
    std::cout << "Usage: " << progname << "\n";
    arg_print_syntax(stdout,argtable,"\n");
    std::cout << "This program converts an image file to a segmentation file\n";
    std::cout << "following the format proposed by the Berkeley Segmentation\n";
    std::cout << "Dataset and Benchmark.\n";
    arg_print_glossary(stdout,argtable,"  %-25s %s\n");
    
    return 0;
  }
  
  /* special case: '--version' takes precedence error reporting */
  if (p_version->count > 0) {
    std::cout << progname << " 1.0\n";
    std::cout << "This is free software: you are free to change and redistribute it.\n";
    std::cout << "There is NO WARRANTY, to the extent permitted by law.\n";
    std::cout << "\n";
    std::cout << "Written by Fogo.\n";
 
    return 0;
  }
	
  /* If the parser returned any errors then display them and exit */
  if (nerrors > 0) {
    /* Display the error details contained in the arg_end struct.*/
    arg_print_errors(stdout,p_end,progname);    
    std::cout << "Try '" << progname << " --help' for more information.\n";
    
    return 1;
  }
	
  /* special case: uname with no command line options induces brief help */
  if (argc==1) {
    std::cout << "Try '" << progname << " --help' for more information.\n";
    
    return 0;
  }
	
  int exitcode = Process_commandline( p_in_img->filename[0],
				      p_out_seg->count, p_out_seg->filename[0],
				      p_uid->count, p_uid->ival[0],
				      p_iid->count, p_iid->ival[0],
				      p_date->count, p_date->tmval, p_date->format);
		
  /* deallocate each non-null entry in argtable[] */
  arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
	
  return exitcode;		
}

// -------------------------------------------------------------------------- //

bool SegApp::Process_commandline( const char *in_img,
				  int out_count, const char *out_seg,
				  int uid_count, int uid,
				  int iid_count, int iid,
				  int date_count, struct tm *date, const char *format)
{	
  wxString str_uid = "";
  if (uid_count > 0) str_uid << uid;
  wxString str_iid = "";
  if (iid_count > 0) str_iid << iid;
  std::string str_date = "";

  if (date_count > 0) {
    char str[60];
    strftime(str,60,format,date);
    str_date = str;
  }
	
  //add expected image kinds handlers
  wxImage::AddHandler( new wxPNMHandler );
  wxImage::AddHandler( new wxBMPHandler );
  wxImage::AddHandler( new wxJPEGHandler );
  wxImage::AddHandler( new wxPNGHandler );
  
  wxImage bmp(in_img, wxBITMAP_TYPE_ANY);
  
  unsigned char* data	= bmp.GetData();
  int height						= bmp.GetHeight();
  int width							= bmp.GetWidth();
  
  Labeling::SegFormatProvider::SegFormat seg =
    Labeling::SegFormatProvider::AsSegFormat(data,width,height,3);
  
  return (out_count == 0) ? ToConsole(seg,str_uid.c_str(),str_iid.c_str(),str_date) : 
    ToFile(out_seg,seg,str_uid.c_str(),str_iid.c_str(),str_date);
}

// -------------------------------------------------------------------------- //

int SegApp::ToFile( const std::string& path,
		    const Labeling::SegFormatProvider::SegFormat& seg,
		    const std::string& uid,
		    const std::string& iid,
		    const std::string& date)
{
  std::ofstream out(path.c_str());
  
  return Save(&out,seg,uid,iid,date);
}

// -------------------------------------------------------------------------- //

int SegApp::ToConsole( const Labeling::SegFormatProvider::SegFormat& seg,
		       const std::string& uid,	
		       const std::string& iid,
		       const std::string& date)
{
  return Save(&std::cout,seg,uid,iid,date);
}

// -------------------------------------------------------------------------- //

int SegApp::Save( std::ostream *out,
		  const Labeling::SegFormatProvider::SegFormat& seg, 
		  const std::string& uid,
		  const std::string& iid,
		  const std::string& date)
{		
  if (!(*out)) {
    std::cout << "Error: Could not create the output.\n";
    
    return 1;
  } //end if
		
  (*out) << "format ascii cr" << "\n";
  if (date.compare("") != 0) (*out) << "date " << date << "\n";
  if (iid.compare("") != 0) (*out) << "image " << iid << "\n";
  if (uid.compare("") != 0) (*out) << "user " << uid << "\n";	
  (*out) << "width " << seg.width << "\n";
  (*out) << "height " << seg.height << "\n";
  (*out) << "segments " << seg.segments << "\n";
  (*out) << "gray 0" << "\n";
  (*out) << "invert 0" << "\n";
  (*out) << "flipflop 0" << "\n";
  (*out) << "data" << "\n";
	
  std::vector<Labeling::SegFormatProvider::SegLine> data = seg.data;	
  for (unsigned l= 0; l < data.size(); l++) {
    Labeling::SegFormatProvider::SegLine sl = data[l];
    
    (*out) << sl.label	<< " " << sl.row << " " << sl.start << " " << sl.end << "\n";
  } //end for 
	
  return 0;
}

// CLASS SegApp  ------------------------------------------------------------ //
