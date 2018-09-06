#ifndef GNM_ANGLEVERTICES_HPP
#define GNM_ANGLEVERTICES_HPP

// ------------------------------------------------------------------------------------------- //

//INCLUDE

// ------------------------------------------------------------------------------------------- //

namespace gnm {
  struct angle_vertices {
    angle_vertices() {p0=0; p1=1; p2=2;};
    angle_vertices(unsigned _p0, unsigned _p1, unsigned _p2)
    {    
      unsigned pp[3] = {_p0, _p1, _p2};
      
      for (unsigned j=0; j<=1; j++) {
	unsigned min = j;
	for (unsigned i=j+1; i<=2; i++) {
	  if (pp[i] < pp[min]) min = i;
	} //end for
	if (min != j) {
	  unsigned tmp = pp[j];
	  pp[j] = pp[min];
	  pp[min] = tmp;
	} //end if
      } //end for
      
      o0 = pp[0]; o1 = pp[1]; o2 = pp[2];
      p0 = _p0; p1 = _p1; p2 = _p2;
    };
    
    unsigned p0, p1, p2;
    unsigned o0, o1, o2; //ordered
    
    bool operator==(angle_vertices av) const {
      return (o0 == av.o0) && (o1 == av.o1) && (o2 == av.o2);
    }
    
    bool operator<(angle_vertices av) const {
      if (o0 < av.o0) return true;
      else if (o0 == av.o0 && o1 < av.o1) return true;
      else if (o0 == av.o0 && o1 == av.o1 && o2 < av.o2) return true;
      
      return false;
    }
  };

  // ------------------------------------------------------------------------------------------- //
  
  struct evaluated_av {
    evaluated_av(angle_vertices _av, double _e) {av=_av; e=_e;};
    
    angle_vertices av;
    double e;
    
    bool operator<(evaluated_av eav) const {
      return e < eav.e;
    }
  };

  // ------------------------------------------------------------------------------------------- //
};

// ------------------------------------------------------------------------------------------- //

#endif /* GNM_ANGLEVERTICES_HPP */

// ------------------------------------------------------------------------------------------- //

