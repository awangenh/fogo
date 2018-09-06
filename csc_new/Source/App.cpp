#include "App.hpp"

#include <Verbose/VerboseUtil.hpp>
#include <kipl-base/loadsavemanager.h>
#include "OneParamReader.hpp"
#include <sstream>
#include <iostream>

IMPLEMENT_APP(CscApp)

// ------------------------------------------------------------ //

kipl::Image* CscApp::CSCImage(const wxString& path) {
	//add expected image kinds handlers
  wxImage::AddHandler( new wxPNMHandler );
  wxImage::AddHandler( new wxBMPHandler );
  wxImage::AddHandler( new wxJPEGHandler );
  wxImage::AddHandler( new wxPNGHandler );
	
	wxImage bmp(path.c_str(), wxBITMAP_TYPE_ANY);
	
	unsigned char* data = bmp.GetData();
	int height  = bmp.GetHeight();
	int width   = bmp.GetWidth();
	
	kipl::Image* cscImage = new kipl::Image(width, height, kipl::Image::pfRGB, data, 1, true, 3);
	
	return cscImage;
}

// ------------------------------------------------------------ //

bool CscApp::ToFile(kipl::Image* image, const wxString& path)
{
	wxImage img(image->getWidth(),image->getHeight(),image->subImage(0),true);
	
	return img.SaveFile(path, wxBITMAP_TYPE_PNG);
}

// ------------------------------------------------------------ //

unsigned CscApp::callRGBCSC(kipl::csc::CSCParameters& p, kipl::Image* original, kipl::Image* region, kipl::Image* label) {
	using namespace kipl;
	using namespace kipl::csc;	
	
	const ROI& roi = original->getROI();
	Image *tmpOrig,*tmpReg;

	if (original->getImageType()==Image::pfHSV) {
		tmpOrig=new Image;
		toRGB(*original,*tmpOrig);
		if (region) {
			tmpReg=new Image(roi.width(),roi.height(),*tmpOrig,1);
			tmpReg->clear();
		}
		else tmpReg=NULL;
	}
	else {
		tmpOrig=original;
		tmpReg=region;
	}

	CSC<RGB,RGB,StdRGBLinkControl> csc;
	csc.getLinkControl()->setThreshold((int)p.threshold);
	csc.enableBestNeighbor(p.bestNeighbor);
	csc.changeStartPosition(p.startPosition);
	csc.changeRepairMethod((RepairMethod)p.repair);
	csc.setMaxLinkLevel(p.maxLinkLevel);
	csc.changeSplittingMethod((SplittingMethod)p.splitting);
	csc.segmentation(*tmpOrig);

	if (label) {
		std::map<int,CodeelementRef<RGB> > ceMap;
		csc.getCEBase()->drawLabelImage(*label,ceMap);
	}
	if (tmpReg) 
	{
		csc.getCEBase()->drawRegionImage(*tmpReg);
		if (tmpReg!=region) toHSV(*tmpReg,*region,true);
	}

	if (original!=tmpOrig) delete tmpOrig;
	if (region!=tmpReg) delete tmpReg;

	return 0;
}

// ------------------------------------------------------------ //

bool CscApp::OnInit()
{
	if (argc > 1)
  {
    std::string image_input_file 	= wxT("");
    std::string image_output_file = wxT("");
    std::string csc_threshold 		= wxT("");
    double threshold;
    
    std::map<std::string,VerboseUnit*> states_map;
    states_map["-i"] = new OneParamReader(&image_input_file);
    states_map["-o"] = new OneParamReader(&image_output_file);
    states_map["-t"] = new OneParamReader(&csc_threshold);
    
    //std::cout << "1\n";
    
    std::stringstream args;
    for (int i=1; i<argc; i++) args << argv[i] << ' ';
    
    int res = Utils::VerboseReader(args,states_map, ' ');
    if (res) return -66;
    
    //std::cout << "2\n";

    if (image_input_file.size() == 0)
    {
      std::cerr << "ERROR: No input image specified.";
      return -111;  
    } //end if
    if (image_output_file.size() == 0)
    {
      std::cerr << "ERROR: No output image specified.";
      return -222;  
    } //end if
    wxString wxtmp = csc_threshold;    
    if (!wxtmp.ToDouble(&threshold))
    {
      std::cerr << "ERROR: Invalid threshold.";
      return -333;  
    } //end if
    
    //std::cout << "3\n";

    kipl::csc::CSCParameters params = kipl::csc::CSCParameters();
    params.csf = 2;
    params.startPosition = 0;
    params.threshold = threshold;
    //std::cout << "THRESHOLD: " << threshold << "\n";
    params.exact = 1;
    params.bestNeighbor = 0;
    params.splitting = 1;
    params.repair = 2;
    params.maxLinkLevel = -1;
    
    //kipl::Image* original = CSCImage(image_input_file.c_str());
    
    //add expected image kinds handlers
      wxImage::AddHandler( new wxPNMHandler );
      wxImage::AddHandler( new wxBMPHandler );
      wxImage::AddHandler( new wxJPEGHandler );
      wxImage::AddHandler( new wxPNGHandler );
    	
    	wxImage bmp(image_input_file.c_str(), wxBITMAP_TYPE_ANY);
    	
    	unsigned char* data = bmp.GetData();
    	int height  = bmp.GetHeight();
    	int width   = bmp.GetWidth();
    	
    	kipl::Image* original = new kipl::Image(width, height, kipl::Image::pfRGB, data, 1, true, 3);
      
      kipl::Image* regions = new kipl::Image(width,height,*original,1);
      kipl::Image* label	 = NULL;
    
    //std::cout << "4\n";
    
    callRGBCSC(params,original,regions,label);
    
    //std::cout << "5\n";
    
    //bool sucess = ToFile(regions,image_output_file.c_str());
    
    wxImage img(regions->getWidth(),regions->getHeight(),regions->subImage(0));
    	
    img.SaveFile(image_output_file.c_str(), wxBITMAP_TYPE_PNG);
    //kipl::LoadSaveManager::Instance()->saveImage(image_output_file.c_str(),"jpeg",width,height,regions->subImage(0));
    
    return 0;
    
    //std::cout << "6\n";

    /*if (!output){
      std::cerr << "ERROR: Problems during execution.";
      return -444;
    } //end if
    
    delete[] output;*/
    //delete states_map["-i"];
    //delete states_map["-o"];
    //delete states_map["-t"];
    
    //return true;
  } //end if
  else
  {         
    //std::cout << "OPTIONS: -i <FILEPATH> -o <FILEPATH> -c <FILEPATH> -s <FILEPATH> [-v] \n";
    std::cout << "Adaptação do CSC provido pela Universidade de Koblenz \n";
    std::cout << "\n";
    std::cout << "CSC -i <FILEPATH> -o <FILEPATH> -t <INTEGER> \n";
    std::cout << "-i: input image path \n";
    std::cout << "-o: output image path (PNG) \n";    
    std::cout << "-t: threshold value, must be integer \n";
    //std::cout << "-b: border image path, PROPERTY STILL IN TEST. [OPTIONAL] \n";
    //std::cout << "-v: show verbose. [OPTIONAL] \n";
    //std::cout << "-sb: show borders, associated with the border image provided with -b option. [OPTIONAL] \n";
    return 0;
  } //end if
	
	return 0;
}

// ------------------------------------------------------------ //

