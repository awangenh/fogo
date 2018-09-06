#ifndef CONSOLEGNM_HPP_
#define CONSOLEGNM_HPP_

//wx stuff
#include <wx/wx.h>

//stdlib stuff
#include <string>

//my stuff
#include <gnm/Segmentation.hpp>

// -------------------------------------------------------------------------- //

struct FileSettings
{
  FileSettings() : output_file_type(wxBITMAP_TYPE_PNM),size_threshold_min(10), size_threshold_max(50), color_threshold(0.5) {};
  FileSettings(const long type, const unsigned size_t_min, const unsigned size_t_max, const double color_t) 
    : output_file_type(type), size_threshold_min(size_t_min), size_threshold_max(size_t_max), color_threshold(color_t) {};
  
  long output_file_type;
  
  unsigned size_threshold_min;
  unsigned size_threshold_max;
  double color_threshold;
};

// -------------------------------------------------------------------------- //

class ConsoleGNM : public wxApp
{
public:
  virtual bool OnInit();
private:  
  bool Process_commandline( const char *in_img,
			    const char *src_img,
			    const char *out_img,
			    int border_count, const char *border_img,
			    int cfg_count, const char *cfg_file,
			    int nopost_count,
			    int dump_count);
  
  gnm::Configuration LoadConfigurationFromFile(const std::string& config_file);
  
  FileSettings       LoadFileSettingsFromFile(const std::string& settings_path);
};

#endif /*CONSOLEGNM_HPP_*/

// -------------------------------------------------------------------------- //
