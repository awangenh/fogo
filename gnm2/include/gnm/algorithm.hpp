#ifndef GNM_ALGORITHM_HPP
#define GNM_ALGORITHM_HPP

// ------------------------------------------------------------------------------------------- //

//INCLUDE
#include <set>
#include <map>
#include <vector>

#include <colorspace.hpp>
#include <Graph/GraphLabeling.hpp>
#include <gnm/anglevertices.hpp>

// ------------------------------------------------------------------------------------------- //

namespace gnm {
  struct image_data {
    image_data() {
      data = NULL;
      width = 0;
      height = 0;
    }

    image_data(unsigned _width, unsigned _height, pixelvalue *_data) {
      data = _data;
      width = _width;
      height = _height;
    }

    //image
    pixelvalue *data;
    unsigned width;
    unsigned height;
  };

  struct time_data {
    time_data() {
      s1_time = 0;
      s2_time = 0;
    }

    time_data(double _s1, double _s2) {
      s1_time = _s1;
      s2_time = _s2;
    }

    double s1_time;
    double s2_time;
  };

  struct params_data {
    params_data(double _initial, double _inc, unsigned _iterations, unsigned _slimit, double _dlimit) {
      initial = _initial;
      inc = _inc;
      iterations = _iterations;
      slimit = _slimit;
      dlimit = _dlimit;
    }

    double initial;
    double inc;
    unsigned iterations;
    unsigned slimit;
    double dlimit;
  };

  struct output_data {
    output_data(image_data _image, time_data _times) {
      image = _image;
      times = _times;
    }

    image_data image;
    time_data times;
  };


  class algorithm {

  public:
    algorithm(image_data source_image, image_data input_image, params_data input_params);
    
    output_data run();
  protected:
    typedef std::pair<unsigned,unsigned> eq_pair;
    
    void to_lab();
    void calc_dists();
    void calc_angles();
    void seek_equivalences();
    void mean_values();
    void merge_regions();
    void post_similaritycheck();
    void post_sizerelevance();
    
    inline double* vector(unsigned p0,
			  unsigned p1);
    
    double energy_function(unsigned p0,
			   unsigned p1,
			   unsigned p2);
    
    double angle(unsigned p0,
		 unsigned p1,
		 unsigned p2);

    void reset();
    
    double param_initial;
    double current_param;
    double inc_param;
    unsigned n_iterations;

    unsigned slimit;
    double dlimit;
    
    unsigned height;
    unsigned width;
    pixelvalue *data;
    pixelvalue *source;
    
    std::map<Labeling::Region*,unsigned> index;
    std::vector<double*> lab;
    
    Labeling::Vertices regions;
    Labeling::Edges edges;
    
    bool *unmerged;
    
    double *dist;
    std::set<unsigned> *neighbors;
    
    std::set<unsigned> *equivalence;
    std::set<eq_pair> *eq_set;
    
    std::map<angle_vertices, double> angle_map;
    std::map<angle_vertices, double> energy_map;
    std::set<evaluated_av> remaining;
  };
};

// ------------------------------------------------------------------------------------------- //

#endif /* GNM_ALGORITHM_HPP */

// ------------------------------------------------------------------------------------------- //
