#include <gnm/controller.hpp>
#include <imgman/ImageManipulator.hpp>
#include <argtable2/argtable2.h>

// ------------------------------------------------------------------------------------------- //

gnm::controller::controller()
{
  quiet_mode = false;
}

// ------------------------------------------------------------------------------------------- //

int gnm::controller::runGNM_console(int argc, char** argv)
{
  struct arg_file *p_source 	= arg_file1("s","source","<image_file>","source image filename (PNG,JPEG,BMP,PNM)");
  struct arg_file *p_input 	= arg_file1("i","input","<image_file>","input image filename (PNG,JPEG,BMP,PNM)");
  struct arg_file *p_output 	= arg_file1("o","output","<image_file>","output image filename (PNG,JPEG,BMP,PNM)");
  //struct arg_dbl  *p_step1param = arg_dbln("1",NULL,"<initial increment iterations>",3,3,"initial barrier(DOUBLE), the increment to be used(DOUBLE) and the number of iterations(INTEGER)");
  //struct arg_dbl  *p_step2param = arg_dbln("2",NULL,"<size similarity>",2,2,"size threshold(INTEGER) and similarity value(DOUBLE)");
  //struct arg_dbl  *p_s1in       = arg_dbl1(NULL,"in","<initial>","");
  struct arg_str  *p_s1in       = arg_str1(NULL,"in","<initial>","initial barrier(DOUBLE)");
  //struct arg_dbl  *p_s1inc      = arg_dbl1(NULL,"inc","<increment>","");
  struct arg_str  *p_s1inc      = arg_str1(NULL,"inc","<increment>","increment to be used(DOUBLE)");
  struct arg_int  *p_s1it       = arg_int1(NULL,"it","<iterations>","number of iterations(INTEGER)");
  
  struct arg_int  *p_s2size     = arg_int1(NULL,"ms","<size>","size threshold(INTEGER)");
  //struct arg_dbl  *p_s2similar  = arg_dbl1(NULL,"mc","<similarity>","");
  struct arg_str  *p_s2similar  = arg_str1(NULL,"mc","<similarity>","color similarity value(DOUBLE)");

  struct arg_lit  *p_quiet    	= arg_lit0("q","quiet","quiet mode, no output to console");
  struct arg_lit  *p_help    	= arg_lit0("h","help","print this help and exit");
  struct arg_lit  *p_version 	= arg_lit0("v","version","print version information and exit");
  struct arg_end  *p_end     	= arg_end(20);
  //void* argtable[] = {p_input,p_output,p_step1param,p_step2param,p_help,p_version,p_end};
  void* argtable[] = {p_source,p_input,p_output,p_s1in,p_s1inc,p_s1it,p_s2size,p_s2similar,p_quiet,p_help,p_version,p_end};
  
  const char* progname = "iGNM";
  int nerrors;	
	
  /* verify the argtable[] entries were allocated sucessfully */
  if (arg_nullcheck(argtable) != 0) {
    /* NULL entries were detected, some allocations must have failed */
    std::cout << progname << ": insufficient memory" << std::endl;
    
    return 1;
  }
		
  /* Parse the command line as defined by argtable[] */
  nerrors = arg_parse(argc,argv,argtable);
	
  /* special case: '--help' takes precedence over error reporting */
  if (p_help->count > 0) {    
    std::cout << "iGNM - iterative Gradient Network Method" << std::endl;
    std::cout << "A color image segmentation algorithm" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: " << progname;
    arg_print_syntax(stdout,argtable,"\n");
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

  quiet_mode = p_quiet->count > 0;

  int exitcode = start( p_source->filename[0], 
			p_input->filename[0],
			p_output->filename[0],
			//p_s1in->dval[0],
			atof(p_s1in->sval[0]),
			//p_s1inc->dval[0],
			atof(p_s1inc->sval[0]),
			p_s1it->ival[0],	
			p_s2size->ival[0],
			//p_s2similar->dval[0]);
			atof(p_s2similar->sval[0]));
		
  /* deallocate each non-null entry in argtable[] */
  arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
	
  return exitcode;
}

// ------------------------------------------------------------------------------------------- //

int gnm::controller::start(const char *_source, const char *_input, const char *_output, double _initial, double _inc, unsigned _iterations, unsigned _slimit, double _dlimit)
//int runGNMconsole(char *_input, char *_output, double _initial, double _inc, unsigned _iterations, unsigned _slimit, double _dlimit) 
{
  /*
  std::cout << _input << std::endl;
  std::cout << _output << std::endl;
  std::cout << _initial << std::endl;
  std::cout << _inc << std::endl;
  std::cout << _iterations << std::endl;
  std::cout << _slimit << std::endl;
  std::cout << _dlimit << std::endl;
  */

  imgman::ImageManipulator imgman;
  try {
    boost::shared_ptr<imgman::Image> image_p = imgman.OpenImage(_input);   
    
    unsigned _height  = image_p->GetHeight();
    unsigned _width   = image_p->GetWidth();
    pixelvalue *_data = new pixelvalue[_width*_height*3];
    pixelvalue *_source = new pixelvalue[_width*_height*3];
    
    for (unsigned i=0; i<_width*_height*3; i++) {
      _data[i] = image_p->GetPixelData()[i];
      _source[i] = image_p->GetPixelData()[i];      
    } //end for

    gnm::params_data params(_initial,_inc,_iterations,_slimit,_dlimit);    
    gnm::image_data input(_width,_height,_data);
    gnm::image_data source(_width,_height,_source);

    gnm::algorithm::algorithm gnm(source,input,params);
    gnm::output_data out = gnm.run();

    if (!quiet_mode) {
      char *date = new char[99];
      time_t t = time(NULL);
      strftime( date, 99, "%H:%M", localtime(&t) );

      std::cout << "segmentation started at " << date << std::endl;
      std::cout << "continuity step finished in " << out.times.s1_time << " seconds" << std::endl;
      std::cout << "similarity step finished in " << out.times.s2_time << " seconds" << std::endl;
      std::cout << "done." << std::endl;
    } //end if

    return saveimage(_output,out.image);    
  } catch(imgman::Exception &ex){
    std::cout << ex.what() << std::endl;

    return 1;
  } //end try  
}

// ------------------------------------------------------------------------------------------- //

int gnm::controller::saveimage(const char *_filename, gnm::image_data output)
{
  imgman::ImageManipulator imgman;
  
  try {
    imgman::Image meanimg;
    meanimg.New(3,output.width,output.height,255);
    for (unsigned y=0; y<output.height; y++) {
      unsigned aux = y * output.width;
      for (unsigned x=0; x<output.width; x++) {
	unsigned p = aux + x;

	meanimg.GetPixelData()[(p*3)] = output.data[p*3];	
	meanimg.GetPixelData()[(p*3) + 1] = output.data[(p*3)+1];
	meanimg.GetPixelData()[(p*3) + 2] = output.data[(p*3)+2];
      } //end for
    } //end for

    imgman.SaveImage(meanimg, _filename);

    return 0;
  } catch(imgman::Exception &ex){
    std::cout << ex.what() << std::endl;

    return 1;
  } //end try
}  

// ------------------------------------------------------------------------------------------- //
