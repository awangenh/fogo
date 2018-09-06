#include <iostream>
#include <sstream>
#include <queue>
#include <map>
#include <vector>

#include <colorspace.hpp>
#include <Graph/GraphLabeling.hpp>
#include <imgman/ImageManipulator.hpp>

// ------------------------------------------------------------------------------------------- //

struct cmp {
  bool operator()(Labeling::Edge e1, Labeling::Edge e2) const {
    return (e1.first == e2.first) && (e1.second == e2.second);
  }
};

struct seed {
  seed(double _d, unsigned _r1, unsigned _r2) {d=_d; regs=std::pair<unsigned,unsigned>(_r1,_r2);};

  bool operator<(seed s) const {
    return d < s.d;
  }

  bool operator==(seed s) const {
    return d == s.d;
  }

  double d;
  std::pair<unsigned,unsigned> regs;
};

// ------------------------------------------------------------------------------------------- //

class teste {

public:
  teste(char *_input, double _angparam);

  void run();

  double angle(double *l1,
	       double *l2,
	       double *l3);

protected:
  void to_lab();
  void calc_dists();
  void calc_angles();
  void create_result();

  void search_seeds();

  inline double energy_function(double e_n,
				unsigned p0,
				unsigned p1,
				unsigned p2);

  void clear();

  double angparam;

  unsigned height;
  unsigned width;
  pixelvalue *data;

  std::map<Labeling::Region*,unsigned> index;
  std::vector<double*> lab;

  Labeling::Vertices regions;
  Labeling::Edges edges;

  bool *unmerged;

  double *dist;
  std::set<unsigned> *neighbors;

  std::set<unsigned> *equivalence;
};

// ------------------------------------------------------------------------------------------- //

teste::teste(char *_input, double _angparam) 
{ 
  angparam = _angparam;

  std::cout << "OPENING IMAGE" << std::endl;

  imgman::ImageManipulator imgman;
  try {
    boost::shared_ptr<imgman::Image> image_p = imgman.OpenImage(_input);   
    
    height  = image_p->GetHeight();
    width   = image_p->GetWidth();
    data = new pixelvalue[width*height*3];
    
    for (unsigned i=0; i<width*height*3; i++) {
      data[i] = image_p->GetPixelData()[i];      
    } //end for

    std::cout << "BUILDING GRAPH" << std::endl;
    
    Labeling::Graph graph = Labeling::GraphProvider::AsGraph(data,width,height,3);
  
    regions = graph.first;
    edges = graph.second;

    unmerged = new bool[regions.size()];
    for (unsigned i=0; i<regions.size(); i++) unmerged[i]=true;
  } catch(imgman::Exception &ex){
    std::cout << ex.what() << std::endl;
  } //end try
}

// ------------------------------------------------------------------------------------------- //

void teste::run()
{
  to_lab();
  calc_dists();
  calc_angles();
  search_seeds();
  create_result();

  //clear();
}

// ------------------------------------------------------------------------------------------- //

void teste::to_lab()
{
  std::cout << "CONVERTING TO L*AB" << std::endl;

  std::cout << std::endl;

  const unsigned rsize = regions.size();
  
  lab.reserve(rsize);
  for (unsigned int k=0; k<rsize; k++){
    Labeling::Region *current = regions[k];
    
    unsigned sum_R = 0;
    unsigned sum_G = 0;
    unsigned sum_B = 0;
    std::set<Index> pos = current->positions;
    unsigned total  = pos.size();
    
    for (std::set<Index>::iterator it=pos.begin(); it!=pos.end(); it++){      
      Index p = *it * 3;
	
      sum_R += data[p];
      sum_G += data[p + 1];
      sum_B += data[p + 2];
    } //end for
      
    pixelvalue r = (pixelvalue)round((double)sum_R / total),
      g = (pixelvalue)round((double)sum_G / total),
      b = (pixelvalue)round((double)sum_B / total);
    double _l, _a, _b;      
    colorspace::rgbtolab(r,g,b,_l,_a,_b);
    
    std::cout << "[" << k << "] = " << (unsigned)r << "," << (unsigned)g << "," << (unsigned)b << std::endl;
    
    double *aux = new double[3];
    aux[0]=_l;aux[1]=_a;aux[2]=_b;
      
    index[current] = k;
    lab[k] = aux;
  } //end for
  
  std::cout << std::endl;
}

// ------------------------------------------------------------------------------------------- //

void teste::calc_dists()
{
  std::cout << "CALCULATING DISTANCES" << std::endl;

  const unsigned rsize = regions.size();

  dist = new double[rsize * rsize];
  neighbors = new std::set<unsigned>[rsize];
  for (int d=0; d<rsize*rsize; d++) dist[d] = 0.0;
   
  for (Labeling::Edges::iterator i=edges.begin(); i != edges.end(); i++){
    Labeling::Edge current_edge = *i;
    Labeling::Region *r1 = current_edge.first;
    unsigned k1 = index[r1];
    
    Labeling::Region *r2 = current_edge.second;
    unsigned k2 = index[r2];
      
    double* l1 = lab[k1];
    double* l2 = lab[k2];
    
    double nl1[3] = { l1[0]/100.0, (l1[1]+128.0)/255.0, (l1[2]+128.0)/255.0 };
    double nl2[3] = { l2[0]/100.0, (l2[1]+128.0)/255.0, (l2[2]+128.0)/255.0 };

    double aux1 = (nl1[0] - nl2[0]) * (nl1[0] - nl2[0]);
    double aux2 = (nl1[1] - nl2[1]) * (nl1[1] - nl2[1]);
    double aux3 = (nl1[2] - nl2[2]) * (nl1[2] - nl2[2]);
    //double d = sqrt( aux3 + aux2 + aux1 );

    double aux = (1.0*(nl1[0] - nl2[0]));
    aux /= sqrt(aux1 + aux2 + aux3);
    if (aux > 1) aux = 1.0;
    else if (aux < -1) aux = -1.0;
    double cos_a = aux;
    double sin_a = sin(acos(aux));

    double d = sqrt( (sin_a*sin_a)*(aux2 + aux3) + (cos_a*cos_a)*(aux1) );
    
    unsigned irow = std::min(k1,k2);
    unsigned icol = (irow == k1) ? k2 : k1;

    dist[(irow * rsize) + icol] = d;
    
    neighbors[irow].insert(icol);
    neighbors[icol].insert(irow);
  } //end for
}

// ------------------------------------------------------------------------------------------- //

void teste::calc_angles()
{
  std::cout << "CALCULATING ANGLES" << std::endl;

  const unsigned rsize = regions.size();
         
  std::queue<unsigned> next;
  next.push(0);
  std::set<std::string> angset;
  bool done[rsize];    

  equivalence = new std::set<unsigned>[rsize];
  for (unsigned i=0; i<rsize; i++) {
    equivalence[i].insert(i);
    done[i]=false;
  } //end for

  while (!next.empty()) {
    unsigned p0 = next.front();
    next.pop();
      
    if (done[p0]) continue;    
    done[p0] = true;
    
    for (std::set<unsigned>::iterator it1=neighbors[p0].begin(); it1!=neighbors[p0].end(); it1++) {
      unsigned p1 = *it1;
      
      next.push(p1);
      
      for (std::set<unsigned>::iterator it2=neighbors[p1].begin(); it2!=neighbors[p1].end(); it2++) {
	unsigned p2 = *it2;
	
	if (p2 == p0) continue;
	
	  std::ostringstream aux;
	  unsigned pmin = (p0 <= p2) ? p0 : p2;
	  unsigned pmax = (pmin == p2) ? p0 : p2;
	  aux << pmin << ";" << p1 << ";" << pmax; 
	  
	  if (angset.find(aux.str()) != angset.end()) continue;
	  
	  //angle p0-p1-p2	  
	  double alpha = angle(lab[p0],lab[p1],lab[p2]);
	  std::cout << "POINTS: " << p0 << "," << p1 << "," << p2 << std::endl;
	  std::cout << "ANGLE: " << alpha << std::endl;
	  
	  unsigned d1 = std::min( (p0*rsize)+p1,(p1*rsize)+p0);   
	  unsigned d2 = std::min( (p2*rsize)+p1,(p1*rsize)+p2);   
	  
	  std::cout << "DIST1: " << dist[d1] << std::endl;
	  std::cout << "DIST2: " << dist[d2] << std::endl;

	  double energy = energy_function(0,p0,p1,p2); 
	  std::cout << "ENERGY: " << energy << std::endl;

	  std::cout << std::endl;
	  
	  angset.insert(aux.str());
	  
	  //if (alpha >= angparam) {
	  if (energy <= angparam) {
	    std::set<unsigned> eq;
	    eq.insert(equivalence[p0].begin(),equivalence[p0].end());
	    eq.insert(equivalence[p1].begin(),equivalence[p1].end());
	    eq.insert(equivalence[p2].begin(),equivalence[p2].end());
	    
	    for (std::set<unsigned>::iterator e=eq.begin(); e!=eq.end(); e++)
	      equivalence[*e] = eq;
	    
	  } //end if
	  
	} //end for	
    } //end for
  } //end while
}

// ------------------------------------------------------------------------------------------- //

void teste::create_result()
{
  const unsigned rsize = regions.size();

  std::set<unsigned> final;
  for (unsigned i=0; i<rsize; i++) 
    final.insert(*(equivalence[i].begin()));

  imgman::ImageManipulator imgman;
  try {
    imgman::Image output;
    output.New(3,width,height,255);    
    unsigned inc = 255.0/final.size();    
    unsigned count = 0;
    
    for (std::set<unsigned>::iterator f=final.begin(); f!=final.end(); f++) {
      for (std::set<unsigned>::iterator e=equivalence[*f].begin(); e!=equivalence[*f].end(); e++) {
	std::cout << *e  << " ";
	
	Labeling::Region *reg = regions[*e];
	std::set<Index> pos = reg->positions;
	
	for (std::set<Index>::iterator it=pos.begin(); it!=pos.end(); it++){      
	  Index p = *it * 3;
	
	  output.GetPixelData()[p] = count * inc;
	  output.GetPixelData()[p + 1] = count * inc;
	  output.GetPixelData()[p + 2] = count * inc;
	} //end for
	
      } //end for
      ++count;
      std::cout << std::endl;
    } //end for
    
    imgman.SaveImage(output, "/home/fogo/tmp/out.png");
  } catch(imgman::Exception &ex){
    std::cout << ex.what() << std::endl;
  } //end try
  
  std::cout << "DONE!" << std::endl;
}  

// ------------------------------------------------------------------------------------------- //

void teste::clear()
{ 
  for (unsigned l=0; l<lab.size(); l++) delete[] lab[l];

  for (unsigned r=0; r<regions.size(); r++) delete regions[r];
  
  delete[] data;
  delete[] dist;
  delete neighbors;
  delete equivalence;
}

// ------------------------------------------------------------------------------------------- //

double teste::angle(double *l1,
		    double *l2,
		    double *l3)
{
  const double PI = 3.1415926535897932384626433832795;

  double nl1[3] = { l1[0]/100.0, (l1[1]+128.0)/255.0, (l1[2]+128.0)/255.0 };
  double nl2[3] = { l2[0]/100.0, (l2[1]+128.0)/255.0, (l2[2]+128.0)/255.0 };
  double nl3[3] = { l3[0]/100.0, (l3[1]+128.0)/255.0, (l3[2]+128.0)/255.0 };

  double v1[3] = { nl1[0] - nl2[0], nl1[1] - nl2[1], nl1[2] - nl2[2] };
  double v2[3] = { nl3[0] - nl2[0], nl3[1] - nl2[1], nl3[2] - nl2[2] };

  double aux = (v1[0]*v2[0]) + (v1[1]*v2[1]) + (v1[2]*v2[2]);
  aux /= sqrt(v1[0]*v1[0] + v1[1]*v1[1] + v1[2]*v1[2]);
  aux /= sqrt(v2[0]*v2[0] + v2[1]*v2[1] + v2[2]*v2[2]);

  if (aux > 1) aux = 1.0;
  else if (aux < -1) aux = -1.0;
   
  return acos(aux) * 180 / PI; 
}

// ------------------------------------------------------------------------------------------- //

void teste::search_seeds()
{
  const unsigned rsize = regions.size();

  double dparam = 0.05;

  std::set<seed> seeds;

  for (unsigned i=0; i<rsize; i++) {
    double min = 999.0;
    int index = -1;
    for (unsigned j=i+1; j<rsize; j++) {
      if (i!=j && unmerged[i] && unmerged[j]) {
	double d = dist[i*rsize + j];
      	if (d && d<=dparam && d < min) {
	  min = d;
	  index = j;
	} //end if
      } //end if
    } //end for

    if (index >= 0) seeds.insert(seed(min,i,index));
  } //end for

  std::cout << std::endl;
  std::cout << "SEEDS" << std::endl;
  for (std::set<seed>::iterator it=seeds.begin(); it!=seeds.end(); it++) {
    std::cout << it->regs.first << " --- " << it->regs.second << " " << it->d << std::endl;
  } //end for
  std::cout << std::endl;
}

// ------------------------------------------------------------------------------------------- //

double teste::energy_function(double e_n,
			      unsigned p0,
			      unsigned p1,
			      unsigned p2)
{
  const double PI = 3.1415926535897932384626433832795;  
  const unsigned rsize = regions.size();

  double a = angle(lab[p0],lab[p1],lab[p2]) * PI / 180;
  double sin_a = a / PI;
  const double offset = 2.0 - (a/PI); 

  double d1 = dist[std::min( (p0*rsize)+p1,(p1*rsize)+p0)];   
  double d2 = dist[std::min( (p2*rsize)+p1,(p1*rsize)+p2)];
  
  double nl1[3] = { lab[p0][0]/100.0, (lab[p0][1]+128.0)/255.0, (lab[p0][2]+128.0)/255.0 };
  double nl2[3] = { lab[p2][0]/100.0, (lab[p2][1]+128.0)/255.0, (lab[p2][2]+128.0)/255.0 };
  double v[3] = { nl2[0] - nl1[0], nl2[1] - nl1[1], nl2[2] - nl1[2] };
  double d3 = sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );

  std::cout << "PO-P2 NORM: " << d3 << std::endl;

  double e_np1 = (d1 + d2) + (d3 * (1.0 - a/PI));

  return e_np1;
}

// ------------------------------------------------------------------------------------------- //
