//std stuff
#include <iostream>
#include <sstream>
#include <time.h>

//edison stuff
#include <segm/msImageProcessor.h>
#include <prompt/defs.h>
#include <segm/tdef.h>

//my stuff
#include "App.hpp"
#include "OneParamReader.hpp"

IMPLEMENT_APP(EdisonApp)

// ------------------------------------------------------------ //

bool EdisonApp::OnInit()
{
	if (argc > 1)
  {
		/*
		 * Initializing parameters
		 */
		
    std::string input_image_path 	= "";
    std::string output_image_path	= "";
		
    int sigmaS				= 7;	std::string str_ss 		= "7";
    float sigmaR			= 6.5;	std::string str_sr 		= "6.5";    
    int minRegion		= 20;		std::string str_minr 	= "20";    
		
		/*
		 * Reading parameters
		 */
		
		std::map<std::string,VerboseUnit*> states_map;
		states_map["-i"]		= new OneParamReader(&input_image_path);
		states_map["-o"]		= new OneParamReader(&output_image_path);
		states_map["-ss"]		= new OneParamReader(&str_ss,true);
		states_map["-sr"] 	= new OneParamReader(&str_sr,true);
		states_map["-minr"] = new OneParamReader(&str_minr,true);
		
		std::stringstream args;
		for (int i=1; i<argc; i++) args << argv[i] << ' ';
		
		int res = Utils::VerboseReader(args,states_map, ' ');
		if (res) return 1;
				
		sigmaS 			= atoi(str_ss.c_str());
		sigmaR 			= atof(str_sr.c_str());
		minRegion 	= atoi(str_minr.c_str());
		
		/*
		 * Reading image
		 */
		
		//add expected image kinds handlers
	  wxImage::AddHandler( new wxPNMHandler );
	  wxImage::AddHandler( new wxBMPHandler );
	  wxImage::AddHandler( new wxJPEGHandler );
	  wxImage::AddHandler( new wxPNGHandler );
		
	  wxImage bmp(input_image_path.c_str(), wxBITMAP_TYPE_ANY);
	  	
  	byte* data	= (byte*)bmp.GetData();
  	int height	= bmp.GetHeight();
  	int width	= bmp.GetWidth();
	  
		/*
		 * EDISON
		 */		
		msImageProcessor edison;
		
		edison.DefineImage(data, COLOR, height, width);
		
		std::cout << "EDISON with " << input_image_path.c_str() << "\n";
		std::cout << "sigmaS = " << str_ss.c_str() << "\n";
		std::cout << "sigmaR = " << str_sr.c_str() << "\n";
		std::cout << "minRegion = " << str_minr.c_str() << "\n";
		clock_t initial_time = clock();
		
		edison.Segment(sigmaS, sigmaR, minRegion, NO_SPEEDUP);
		
		clock_t final_time = clock();
		
		double total_time = (final_time - initial_time) / (double)CLOCKS_PER_SEC;
		int hours 		= total_time / 3600;
		int minutes 	= (total_time - (3600 * hours)) / 60;
		double seconds	= total_time - (3600 * hours) - (60 * minutes);		
		if (hours > 0)
		{
			std::cout << "Time: " << hours << "h" << minutes << "m" << seconds << "s \n"; 
		} else if (minutes > 0)
		{
			std::cout << "Time: " << minutes << "m" << seconds << "s \n";
		} else
		{
			std::cout << "Time: " << seconds << "s \n";
		} //end if
		
		byte* out_data = new byte[height * width * 3];
		edison.GetResults(out_data);
		
		wxImage out(width,height,out_data);
		
		out.SaveFile(output_image_path.c_str(),wxBITMAP_TYPE_PNG);
    
  } //end if
  else
  { 
    std::cout << "EDISON \n";
    std::cout << "\n";
    std::cout << "edison -i <FILEPATH> -o <FILEPATH> [-ss <INTEGER>] [-sr <FLOAT>] [-minr <INTEGER>] \n";
    std::cout << "-i: input image path (PNG,JPEG,BMP,PNM)\n";
    std::cout << "-o: output image path (PNG) \n";    
    std::cout << "-ss: the spatial radius of the mean shift window [OPTIONAL, default=7] \n";
    std::cout << "-sr: The range radius of the mean shift window [OPTIONAL, default=6.5] \n";
    std::cout << "-minr: The minimum density a region may have in the resulting segmented image [OPTIONAL, default=20] \n";
           
    return 0;
  } //end if
	
	return 0;
	    
  
	
	
}

// ------------------------------------------------------------ //

