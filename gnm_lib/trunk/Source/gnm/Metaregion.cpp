// --------------- TODOs -------------- //

//TODO: permitir parâmetros opcionais.

// --------------- INCLUDE's -------------- //

//std stuff
#include <math.h>

//my stuff
#include <gnm/Metaregion.hpp>

// ----------- class VERBOSEUNIT ---------- //

Metaregion::Metaregion(const Label l)
{  
  rgb_sum[0] = 0;
  rgb_sum[1] = 0;
  rgb_sum[2] = 0;
  
  pos_sum = 0;
  
  label_id = l;
}

// ---------------------------------------- //

Metaregion::~Metaregion()
{
  for (Labeling::Vertices::iterator it = regions.begin(); it != regions.end(); it++)
    delete *it;
}

// ---------------------------------------- //
