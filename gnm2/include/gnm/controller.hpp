#ifndef GNM_CONTROLLER_HPP
#define GNM_CONTROLLER_HPP

// ------------------------------------------------------------------------------------------- //

#include <set>

#include <colorspace.hpp>
#include <gnm/algorithm.hpp>

// ------------------------------------------------------------------------------------------- //

namespace gnm {
  class controller {
  public:
    controller();
    
    //int runGNM_console(char *_input, char *_output, double _initial, double _inc, unsigned _iterations, unsigned _slimit, double _dlimit);
    int runGNM_console(int argn, char** argv);
  private:
    int start(const char *_source, const char *_input, const char *_output, double _initial, double _inc, unsigned _iterations, unsigned _slimit, double _dlimit);

    int saveimage(const char *_filename, image_data output);

    bool quiet_mode;
  };
};

// ------------------------------------------------------------------------------------------- //

#endif

// ------------------------------------------------------------------------------------------- //
