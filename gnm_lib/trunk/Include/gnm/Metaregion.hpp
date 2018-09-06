#ifndef METAREGION_HPP
#define METAREGION_HPP

// --------------- INCLUDE's -------------- //

//stdlib stuff
#include <set>
#include <map>

//my stuff
#include <Labeling/Labeling.hpp>

// ----------- class VERBOSEUNIT ---------- //

struct Metaregion
{
  Metaregion(const Label l);
  ~Metaregion();
  
  Labeling::Vertices regions;
  unsigned int rgb_sum[3];
  unsigned int pos_sum;
  
  Label label_id;  
};

// --------------- TYPEDEF's -------------- //

typedef std::pair<Metaregion*,Metaregion*>        Meta_Edge;
typedef std::set<Meta_Edge>                       Meta_Edges;
typedef std::map<Labeling::Region*,Metaregion* >  Meta_Vertices;
typedef std::set<Metaregion*>                     Meta_List;

// ---------------------------------------- //

#endif /*METAREGION_HPP*/
