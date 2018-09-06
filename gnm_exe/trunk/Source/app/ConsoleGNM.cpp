//TODO: arrumar a sele��o de verbose

//wx stuff
#include <wx/wx.h>
#include <wx/image.h>

//stdlib stuff
#include <time.h>
#include <fstream>
#include <iostream>
#include <string>

//my stuff
#include <app/ConsoleGNM.hpp>
#include <Labeling/Labeling.hpp>
#include <argtable2/argtable2.h>

IMPLEMENT_APP(ConsoleGNM)

// -------------------------------------------------------------------------- //

gnm::Configuration ConsoleGNM::LoadConfigurationFromFile(const std::string& config_file)
{  
  std::ifstream cfg_file(config_file.c_str());
 
  if (cfg_file) {
    gnm::Configuration cfg;

    int count_param = 0;  
    std::string line;
    while ( std::getline(cfg_file, line,'\n') ) {
      cfg.params[count_param++] = atof(line.c_str());

      if (count_param >= GNM_TOTAL_PARAMS) break;
    } //end while

    return cfg;
  } else { 
    return gnm::Configuration();
  } //end if
}

// -------------------------------------------------------------------------- //

FileSettings ConsoleGNM::LoadFileSettingsFromFile(const std::string& settings_path)
{
  std::ifstream settings_file(settings_path.c_str());
  
  FileSettings settings;
  
  if (settings_file)
  {
    std::string line;
    bool waiting = true;
    int count = -1;
    while ( getline(settings_file, line,'\n') ) {
      

      if (waiting) {
	++count;
      } else {
	
	switch (count) {
	case 0 : {
	  int type = atoi(line.c_str());
	  switch (type) {
	  case 1 : {
	    settings.output_file_type = wxBITMAP_TYPE_PNM;
	    break;
	  } //end case
	  case 2 : {
	    settings.output_file_type = wxBITMAP_TYPE_PNG;
	    break;
	  } //end case
	  case 3 : {
	    settings.output_file_type = wxBITMAP_TYPE_BMP;
	    break;
	  } //end case
	  default : settings.output_file_type = wxBITMAP_TYPE_PNM;
	  } //end switch
	  
	  break;
	} //end case

	case 1 : {
	  settings.size_threshold_min = atoi(line.c_str());
	  break;
	} //end case
	case 2 : {
	  settings.size_threshold_max = atoi(line.c_str());
	  break;
	} //end case
	case 3 : {
	  settings.color_threshold = atof(line.c_str());
	  break;
	} //end case
	default : break; 
	} //end switch		
      } //end if

      waiting = !waiting;
    } //end while  

  } //end if
  
  return settings;
}

// -------------------------------------------------------------------------- //

bool ConsoleGNM::OnInit()
{
  struct arg_file *p_in_img  = arg_file1("i","input","<image_file>",	"input image filename, preferrably a previously segmented one (PPM or BMP or JPEG)");
  struct arg_file *p_src_img = arg_file1("s","source","<image_file>",	"original image filename, the image used for the initial segmentation (PPM or BMP or JPEG)");
  struct arg_file *p_out_img = arg_file1("o","output","<image_file>",	"output image filename (format specified in the gnm.settings file, if not save as PPM)");
  struct arg_file *p_border_img = arg_file0("b","border","<image_file>","image only showing the segmentated regions borders");
  struct arg_file *p_cfg_file = arg_file0("c","config","<cfg_file>",	"configuration filename (refer to README file to learn about configuration)");
  struct arg_lit  *p_no_post = arg_lit0("n",NULL,"disable the post-processing step");
  struct arg_lit  *p_stat_dump = arg_lit0("d",NULL,"dump segments statistics and labeled segments files");
  struct arg_lit  *p_help    	= arg_lit0(NULL,"help",                   "print this help and exit");
  struct arg_lit  *p_version 	= arg_lit0(NULL,"version",                "print version information and exit");
  struct arg_end  *p_end     	= arg_end(20);
  void* argtable[] = {p_in_img,p_src_img,p_out_img,p_border_img,p_cfg_file,p_no_post,p_stat_dump,p_help,p_version,p_end};
  
  const char* progname = "gnm";
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
    std::cout << "Usage: " << progname;
    arg_print_syntax(stdout,argtable,"\n");
    std::cout << "LAPIX - Laboratory for Image Processing and Graphic Computing \n";
    std::cout << "Cyclops Project \n";
    std::cout << "Federal University of Santa Catarina - Brazil \n";
    std::cout << "\n";
    std::cout << "GNM - Gradient Network Method Segmentation \n";
    arg_print_glossary(stdout,argtable,"  %-25s %s\n");
    
    return 0;
  }
  
  /* special case: '--version' takes precedence error reporting */
  if (p_version->count > 0) {
    std::cout << progname << " 1.0\n";
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
				      p_src_img->filename[0],
				      p_out_img->filename[0],
				      p_border_img->count, p_border_img->filename[0],
				      p_cfg_file->count, p_cfg_file->filename[0],
				      p_no_post->count,
				      p_stat_dump->count
				      );
		
  /* deallocate each non-null entry in argtable[] */
  arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
	
  return exitcode;
}

// -------------------------------------------------------------------------- //

bool ConsoleGNM::Process_commandline( const char *in_img,
				      const char *src_img,
				      const char *out_img,
				      int border_count, const char *border_img,
				      int cfg_count, const char *cfg_file,
				      int nopost_count,
				      int dump_count)
{
  //-----------------------------------------//
  //  1. Load images                         //
  //-----------------------------------------//
	
  //add expected image kinds handlers
  wxImage::AddHandler( new wxPNMHandler );
  wxImage::AddHandler( new wxBMPHandler );
  wxImage::AddHandler( new wxJPEGHandler );
  wxImage::AddHandler( new wxPNGHandler );
  
  //reads input image
  wxImage in_image = wxImage(wxBitmap(in_img, wxBITMAP_TYPE_ANY).ConvertToImage());  
  //reads source image
  wxImage src_image = wxImage(wxBitmap(src_img, wxBITMAP_TYPE_ANY).ConvertToImage());  
  
  unsigned char* input_data = in_image.GetData();
  unsigned char* src_data   = src_image.GetData();
  int height  = in_image.GetHeight();
  int width   = in_image.GetWidth();
  
  //-----------------------------------------//
  //  2. Labeling                            //
  //-----------------------------------------//
  
  std::cout << "Process started with " << in_img << "...\n";  
  clock_t initial_time = clock();
  
  Labeling::Graph graph = Labeling::GraphProvider::AsGraph(input_data,width,height,3);
  
  clock_t label_time = clock();  
  std::cout << "Labeling time (seconds): " << (label_time - initial_time) / (double)CLOCKS_PER_SEC << "\n";
  
  //-----------------------------------------//
  //  3. Load configuration file             //
  //-----------------------------------------//
  
  gnm::Configuration cfg = (cfg_count > 0) ? LoadConfigurationFromFile(cfg_file) : LoadConfigurationFromFile("");  
  
  FileSettings settings  = LoadFileSettingsFromFile("gnm.settings");

  //-----------------------------------------//
  //  4. Executing GNM                       //
  //-----------------------------------------//
   
  gnm::Algorithm gnm(settings.size_threshold_min,settings.size_threshold_max,settings.color_threshold);
  
  gnm::Results results = gnm.Execute( input_data, src_data, width, height, 
				      graph.first, graph.second,
				      cfg,border_count,dump_count,!nopost_count);


  if (dump_count) {
    std::ofstream segdump("segdump.csv");
    if (segdump) segdump << results.stats_dump;
  }

  if (border_count) {
    wxImage border = wxImage(width,height,results.border_image,true);
    border.SaveFile(border_img,settings.output_file_type);
  }

  clock_t final_time = clock();
  
  //std::cout << "Postprocessing time (seconds): " << (final_time - post_start) / (double)CLOCKS_PER_SEC << "\n";
  std::cout << "GNM time (seconds): " << (final_time - initial_time) / (double)CLOCKS_PER_SEC << "\n";
  
  //-----------------------------------------//
  //  5. Saving output                       //
  //-----------------------------------------//
  
  wxImage out = wxImage(width,height,results.output_image,true);
  out.SaveFile(out_img,settings.output_file_type);

  return 0;
}

// -------------------------------------------------------------------------- //
