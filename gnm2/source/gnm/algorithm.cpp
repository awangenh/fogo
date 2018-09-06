//INCLUDE
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <queue>

#include <gnm/algorithm.hpp>

// ------------------------------------------------------------------------------------------- //

gnm::algorithm::algorithm(image_data source_image, image_data input_image, params_data input_params)
{ 
  param_initial = input_params.initial;  
  inc_param = input_params.inc;
  n_iterations = input_params.iterations;
  slimit = input_params.slimit;
  dlimit = input_params.dlimit;

  height  = input_image.height;
  width   = input_image.width;
  data = input_image.data;
  source = source_image.data;

  unmerged = NULL;
  dist = NULL;
  neighbors = NULL;
  equivalence = NULL;
  eq_set = NULL;
}

// ------------------------------------------------------------------------------------------- //

void gnm::algorithm::reset()
{
  index.clear();
  lab.clear();

  if (unmerged != NULL) delete[] unmerged;
  if (dist != NULL) delete[] dist;
  if (neighbors != NULL) delete[] neighbors;
  if (equivalence != NULL) delete[] equivalence;
  //if (eq_set != NULL) delete[] eq_set;

  angle_map.clear();
  energy_map.clear();
  remaining.clear();
}

// ------------------------------------------------------------------------------------------- //

void gnm::algorithm::mean_values()
{
  const unsigned rsize = regions.size();

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

    for (std::set<Index>::iterator it=pos.begin(); it!=pos.end(); it++){      
      Index p = *it * 3;
	
      data[p] = r;
      data[p + 1] = g;
      data[p + 2] = b;
    } //end for
  } //end for 
}

// ------------------------------------------------------------------------------------------- //

gnm::output_data gnm::algorithm::run()
{
  time_data times;
  clock_t initial_time = clock();

  Labeling::Graph graph = Labeling::GraphProvider::AsGraph(data,width,height,3);
  
  mean_values();
  
  regions = graph.first;
  edges = graph.second;

  current_param = param_initial;

  for (unsigned iterations=1; iterations<=n_iterations; iterations++) {
    reset();

    unmerged = new bool[regions.size()];
    for (unsigned i=0; i<regions.size(); i++) unmerged[i]=true;
    
#ifdef _DEBUG_
    std::cout << "CONVERTING TO L*AB" << std::endl;
#endif
    to_lab();
    
#ifdef _DEBUG_
    std::cout << std::endl;
    std::cout << "CALCULATING DISTANCES" << std::endl;
#endif
    calc_dists();
    
#ifdef _DEBUG_
    std::cout << std::endl;
    std::cout << "CALCULATING ANGLES" << std::endl;
#endif
    calc_angles();
    
#ifdef _DEBUG_
    std::cout << std::endl;
    std::cout << "LOOKING FOR SIMILAR PATHS" << std::endl;
#endif
  
    const unsigned rsize = regions.size();
    equivalence = new std::set<unsigned>[rsize];
    eq_set = new std::set<eq_pair>[rsize];
#ifdef _DEBUG_
    std::cout << "NEIGHBORS: " << std::endl;
#endif

    for (unsigned i=0; i<rsize; i++) {
      equivalence[i].insert(i);
#ifdef _DEBUG_
      std::cout << i << " = ";
      for (std::set<unsigned>::iterator it=neighbors[i].begin(); it!=neighbors[i].end(); it++) std::cout << *it << " ";
      std::cout << std::endl;
#endif
    } //end for
    
    seek_equivalences();

#ifdef _DEBUG_
    std::cout << "MERGE REGIONS" << std::endl;    
#endif    
    merge_regions();

#ifdef _DEBUG_
    std::cout << std::endl;
#endif

    current_param += inc_param;

  } //end for

  clock_t s1_time = clock();
  times.s1_time = (s1_time - initial_time) / (double)CLOCKS_PER_SEC;

  post_sizerelevance();  
  merge_regions();

  post_similaritycheck();  
  merge_regions();

  clock_t s2_time = clock();
  times.s2_time = (s2_time - initial_time) / (double)CLOCKS_PER_SEC;

#ifdef _DEBUG_
  std::cout << "DONE!" << std::endl;
#endif

  reset();
  
  output_data out(image_data(width,height,data),times);

  return out;
}

// ------------------------------------------------------------------------------------------- //

void gnm::algorithm::merge_regions()
{
  const unsigned rsize = regions.size();

  std::set<unsigned> final;
  std::set<unsigned> unmerged;
  for (unsigned i=0; i<rsize; i++) {
    if (equivalence[i].empty())
      unmerged.insert(i);
    else
      final.insert( *(equivalence[i].begin()) );
  } //end for
  
  unsigned count = 0;  
  unsigned rsum[unmerged.size()+final.size()];
  unsigned gsum[unmerged.size()+final.size()];
  unsigned bsum[unmerged.size()+final.size()];
  unsigned psum[unmerged.size()+final.size()];
  for (unsigned i=0; i<unmerged.size()+final.size(); i++) {
    rsum[i] = 0;
    gsum[i] = 0;
    bsum[i] = 0;
    psum[i] = 0;
  } //end for  

  unsigned *seg = new unsigned[width * height];

  for (std::set<unsigned>::iterator f=final.begin(); f!=final.end(); f++) {
    for (std::set<unsigned>::iterator e=equivalence[*f].begin(); e!=equivalence[*f].end(); e++) {
      Labeling::Region *r = regions[*e];
      std::set<Index> pos = r->positions;
	
      for (std::set<Index>::iterator it=pos.begin(); it!=pos.end(); it++){      
	Index p = *it * 3;
	
	rsum[count] += data[p];
	gsum[count] += data[p+1];
	bsum[count] += data[p+2];
	psum[count] += 1;

	seg[*it] = count;
      } //end for

      delete r;

    } //end for
    ++count;
  } //end for

  for (std::set<unsigned>::iterator u=unmerged.begin(); u!=unmerged.end(); u++) {      
    Labeling::Region *r = regions[*u];
    std::set<Index> pos = r->positions;
    
    for (std::set<Index>::iterator it=pos.begin(); it!=pos.end(); it++){      
      Index p = *it * 3;
      
      rsum[count] += data[p];
      gsum[count] += data[p+1];
      bsum[count] += data[p+2];
      psum[count] += 1;

      seg[*it] = count;
    } //end for

    delete r;

    ++count;
  } //end for

  for (unsigned y=0; y<height; y++) {
    unsigned aux = y * width;
    for (unsigned x=0; x<width; x++) {
      unsigned p = aux + x;

      data[(p*3)] = (double)rsum[seg[p]] / psum[seg[p]];
      data[(p*3) + 1] = (double)gsum[seg[p]] / psum[seg[p]];      
       data[(p*3) + 2] = (double)bsum[seg[p]] / psum[seg[p]];
    } //end for
  } //end for
 
  Labeling::Graph graph = Labeling::GraphProvider::AsGraph(data,width,height,3);

  regions = graph.first;
  edges = graph.second;
}

// ------------------------------------------------------------------------------------------- //

void gnm::algorithm::seek_equivalences()
{
  for (std::set<evaluated_av>::iterator it=remaining.begin(); it!=remaining.end(); it++) {
    angle_vertices av = it->av;

    if (equivalence[av.p0].find(av.p1) != equivalence[av.p0].end() &&
	equivalence[av.p0].find(av.p2) != equivalence[av.p0].end()) {
      continue;
    } //end if
    
    std::queue<evaluated_av> next;
    next.push(*it);

    double energy_sum = 0.0;
    std::set<unsigned> eq;
    std::set<eq_pair> ep;
      

    while (!next.empty()) {      
      angle_vertices current = next.front().av;
      double e_n = next.front().e;          
      next.pop();

#ifdef _DEBUG_
      std::cout << "CURRENT: " << current.p0 << " " << current.p1 << " " << current.p2 << " ---> " << e_n << std::endl;      
#endif
      
      if (e_n < current_param) {

#ifdef _DEBUG_
	std::cout << "SUCCESS!" << std::endl;
#endif

	//unsigned a, b;
	//a = std::min(current.p0,current.p1);
	//b = (a == current.p0) ? current.p1 : current.p0;
	//eq_pair q1(a,b);
	//a = std::min(current.p1,current.p2);
	//b = (a == current.p2) ? current.p1 : current.p2;
	//eq_pair q2(a,b);

	//ep.insert(q1);
	//ep.insert(q2);
	//ep.insert(eq_set[current.p0].begin(),eq_set[current.p0].end());
	//ep.insert(eq_set[current.p1].begin(),eq_set[current.p1].end());
	//ep.insert(eq_set[current.p2].begin(),eq_set[current.p2].end());

	//for (std::set<eq_pair>::iterator e=ep.begin(); e!=ep.end(); e++) {
	//  eq_set[e->first] = ep;
	//  eq_set[e->second] = ep;
	//} //end for

	eq.insert(equivalence[current.p0].begin(),equivalence[current.p0].end());
	eq.insert(equivalence[current.p1].begin(),equivalence[current.p1].end());
	eq.insert(equivalence[current.p2].begin(),equivalence[current.p2].end());	
	
	for (std::set<unsigned>::iterator e=eq.begin(); e!=eq.end(); e++) equivalence[*e] = eq;
      } else {
#ifdef _DEBUG_
	std::cout << "FAIL..." << std::endl;
#endif
	continue;
      } //end if   

      unsigned ps[3] = {current.p0, current.p1, current.p2};
      
      unsigned index1 = 0;
      unsigned index2 = 1;
      bool has_next = false;    

      evaluated_av min_eav(angle_vertices(0,0,0),99999999.0);

#ifdef _DEBUG_
      std::cout << "( TESTING CANDIDATES )" << std::endl;
#endif
      for (unsigned c=1; c<=2; c++) {
	unsigned i1 = ps[index1];
	unsigned i2 = ps[index2];

	for (unsigned s=1; s<=2; s++) {
	  for (std::set<unsigned>::iterator it=neighbors[i2].begin(); it!=neighbors[i2].end(); it++) {
	    unsigned i3 = *it;

	    if (i1 == i3) continue;
	    if (equivalence[i1].find(i3) != equivalence[i1].end()) continue;	    

	    angle_vertices av(i1,i2,i3);
	    double e = energy_map[av];

#ifdef _DEBUG_
	    std::cout << "CANDIDATE: " << i1 << " " << i2 << " " << i3 << " ---> " << e << std::endl;
#endif

	    if (e < min_eav.e) {
	      has_next = true;
	      min_eav.av = av;
	      min_eav.e = e;
	    } //end if
	  } //end for

	  i1 = ps[index2];
	  i2 = ps[index1];
	} //end for

	++index1;
	++index2;
      } //end for      
      
      if (has_next) next.push(min_eav);     

#ifdef _DEBUG_
      std::cout << std::endl;
#endif
    } //end while
  } //end while
}

// ------------------------------------------------------------------------------------------- //

void gnm::algorithm::to_lab()
{
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

#ifdef _DEBUG_
    std::cout << "[" << k << "] = " << (unsigned)r << "," << (unsigned)g << "," << (unsigned)b << std::endl;
#endif

    double _l, _a, _b;      
    colorspace::rgbtolab(r,g,b,_l,_a,_b);
    
    double *aux = new double[3];
    aux[0]=_l;aux[1]=_a;aux[2]=_b;
      
    index[current] = k;
    lab[k] = aux;
  } //end for

#ifdef _DEBUG_
  std::cout << std::endl;
#endif
}

// ------------------------------------------------------------------------------------------- //

void gnm::algorithm::calc_dists()
{
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

    double s1 = (double)r1->positions.size() / (r1->positions.size() + r2->positions.size());
    double s2 = (double)r2->positions.size() / (r1->positions.size() + r2->positions.size());
    
    double nl1[3] = { l1[0]/100.0, (l1[1]+128.0)/255.0, (l1[2]+128.0)/255.0 };
    double nl2[3] = { l2[0]/100.0, (l2[1]+128.0)/255.0, (l2[2]+128.0)/255.0 };

    double aux1 = (nl1[0] - nl2[0]) * (nl1[0] - nl2[0]);
    double aux2 = (nl1[1] - nl2[1]) * (nl1[1] - nl2[1]);
    double aux3 = (nl1[2] - nl2[2]) * (nl1[2] - nl2[2]);

    // distance using angle between the p1-p2 vector and the L axis to find weights 
    // for the equation
    double aux = nl1[0] - nl2[0];
    aux /= sqrt(aux1 + aux2 + aux3);
    if (aux > 1) aux = 1.0;
    else if (aux < -1) aux = -1.0;
    double cos_a = aux;
    double sin_a = sin(acos(aux));    

    unsigned n1 = r1->neighbors[r2];
    unsigned n2 = std::min( r1->border_size, r2->border_size );
    double nn = (double)n1 / n2;

    double c1 = 3.0;
    double c2 = 1.0;
    double d = sqrt( (c1*sin_a*sin_a)*(aux2 + aux3) + (c2*cos_a*cos_a)*(aux1) );
    d = pow(d,1+nn);
    
    unsigned irow = std::min(k1,k2);
    unsigned icol = (irow == k1) ? k2 : k1;

    dist[(irow * rsize) + icol] = d;
    
    neighbors[irow].insert(icol);
    neighbors[icol].insert(irow);
  } //end for
}

// ------------------------------------------------------------------------------------------- //

void gnm::algorithm::calc_angles()
{
  const unsigned rsize = regions.size();
         
  std::queue<unsigned> next;
  next.push(0);

  for (unsigned p0=0; p0<rsize; p0++) {    
    for (std::set<unsigned>::iterator it1=neighbors[p0].begin(); it1!=neighbors[p0].end(); it1++) {
      unsigned p1 = *it1;      
      for (std::set<unsigned>::iterator it2=neighbors[p1].begin(); it2!=neighbors[p1].end(); it2++) {
	unsigned p2 = *it2;
	
	if (p2 == p0) continue;

	unsigned pmin = (p0 <= p2) ? p0 : p2;
	unsigned pmax = (pmin == p2) ? p0 : p2;
		
	// angle between p0-p1 and p2-p0 vectors	  
	double alpha = angle(p0,p1,p2);
	
	angle_vertices av(pmin,p1,pmax);

	double e = energy_function(pmin,p1,pmax);

	if (angle_map.find(av) != angle_map.end()) {
	  if (alpha > angle_map[av]) {
	    evaluated_av eav(av,energy_map[av]);
	    std::set<evaluated_av>::iterator it = remaining.find(eav);
	    remaining.erase(*it);
	    remaining.insert(evaluated_av(av,e));
	    angle_map[av] = alpha;
	    energy_map[av] = e;
	  } //end if
	} else {	  
	  evaluated_av eav(av,e);
	  remaining.insert(eav);
	  angle_map[av] = alpha;
	  energy_map[av] = e;
	} //end if	
	
      } //end for	
    } //end for
  } //end while
}

// ------------------------------------------------------------------------------------------- //

double* gnm::algorithm::vector(unsigned p0,
		    unsigned p1)
{
  double nl0[3] = { lab[p0][0]/100.0, (lab[p0][1]+128.0)/255.0, (lab[p0][2]+128.0)/255.0 };
  double nl1[3] = { lab[p1][0]/100.0, (lab[p1][1]+128.0)/255.0, (lab[p1][2]+128.0)/255.0 };
  double *v = new double[3];
  v[0] = nl1[0] - nl0[0];
  v[1] = nl1[1] - nl0[1];
  v[2] = nl1[2] - nl0[2];

  return v;
}

// ------------------------------------------------------------------------------------------- //

double gnm::algorithm::energy_function(unsigned p0,
			    unsigned p1,
			    unsigned p2)
{
  const unsigned rsize = regions.size();

  double a = angle(p0,p1,p2);
  double *v = vector(p0,p2);
  double d1 = dist[std::min( (p0*rsize)+p1,(p1*rsize)+p0)];   
  double d2 = dist[std::min( (p2*rsize)+p1,(p1*rsize)+p2)];  
  double d3 = sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );

  double c1 = 1.0;
  double c2 = 1.0;
#ifdef _DEBUG_
  std::cout << "( " << p0 << " " << p1 << " " << p2 << " )" << std::endl; 
  std::cout << "[ D1 = " << d1 << "]" << std::endl;
  std::cout << "[ D2 = " << d2 << "]" << std::endl;
  std::cout << "[ D1 + D2 = " << (d1 + d2) << "]" << std::endl;
  std::cout << "[ D3 = " << d3 << "]" << std::endl;
  std::cout << "[ ALPHA = " << sin(a) << "]" << std::endl;
#endif

  double e_np1 = (d1 + d2);
#ifdef _DEBUG_
  std::cout << "[ ENERGY = " << e_np1 << "]" << std::endl;
#endif

  return e_np1;
}

// ------------------------------------------------------------------------------------------- //

double gnm::algorithm::angle(unsigned p0,
		  unsigned p1,
		  unsigned p2)
{
  const double PI = 3.1415926535897932384626433832795;

  double *v1 = vector(p1,p0);
  double *v2 = vector(p1,p2);

  double aux = (v1[0]*v2[0]) + (v1[1]*v2[1]) + (v1[2]*v2[2]);
  aux /= sqrt(v1[0]*v1[0] + v1[1]*v1[1] + v1[2]*v1[2]);
  aux /= sqrt(v2[0]*v2[0] + v2[1]*v2[1] + v2[2]*v2[2]);

  if (aux > 1) aux = 1.0;
  else if (aux < -1) aux = -1.0;
   
  return acos(aux) / PI; 
}

// ------------------------------------------------------------------------------------------- //

void gnm::algorithm::post_similaritycheck()
{
  reset();

  unmerged = new bool[regions.size()];
  for (unsigned i=0; i<regions.size(); i++) unmerged[i]=true;

  to_lab();
  
  calc_dists();
  
  const unsigned rsize = regions.size();
  equivalence = new std::set<unsigned>[rsize];
  //eq_set = new std::set<eq_pair>[rsize];

  for (unsigned i=0; i<rsize; i++) {
    equivalence[i].insert(i);
  } //end for
  
  unsigned size_limit = 20;
  for (unsigned i=0; i<regions.size(); i++) {
    Labeling::Region *r1 = regions[i];
    unsigned s1 = r1->positions.size();

    int selected = -1;
    double min_d = 9999999.9;
    unsigned biggest = 0;

    for( std::map<Labeling::Region*, unsigned>::iterator it = r1->neighbors.begin(); it != r1->neighbors.end(); it++ ) {
      Labeling::Region *r2 = it->first;
      unsigned j = index[r2];
      unsigned s2 = r2->positions.size();

      double ratio = std::min( (double)s1/s2, (double)s2/s1 );

      unsigned k1 = index[r1];
      unsigned k2 = index[r2];
      
      double* l1 = lab[k1];
      double* l2 = lab[k2];
      
      double nl1[3] = { l1[0]/100.0, (l1[1]+128.0)/255.0, (l1[2]+128.0)/255.0 };
      double nl2[3] = { l2[0]/100.0, (l2[1]+128.0)/255.0, (l2[2]+128.0)/255.0 };
      
      double d = 9999999999.0;
      bool flag = false;
      double llimit = 0.0;
      double ulimit = 1.0;

      flag = true;
      
      d = sqrt( pow(nl1[1]-nl2[1],2) + pow(nl1[2]-nl2[2],2) + (pow(nl1[0]-nl2[0],2)/3) );
      
      unsigned n1 = r1->neighbors[r2];
      unsigned n2 = std::min( r1->border_size, r2->border_size );
      double nn = (double)n1 / n2;
      
      if (flag && d < min_d && d < dlimit) {	  
	selected = j;
	min_d = d;
      } //end if
    } //end for

    std::set<unsigned> eq;
    //std::set<eq_pair> ep;
    if (selected != -1) {
      //unsigned a, b;
      //a = std::min(i,(unsigned)selected);
      //b = (a == i) ? selected : i;
      //eq_pair q1(a,b);

      //ep.insert(q1);	
      //ep.insert(eq_set[i].begin(),eq_set[i].end());
      //ep.insert(eq_set[selected].begin(),eq_set[selected].end());
      //for (std::set<eq_pair>::iterator e=ep.begin(); e!=ep.end(); e++) {
      //eq_set[e->first] = ep;
      //eq_set[e->second] = ep;
      //} //end for

      eq.insert(equivalence[i].begin(),equivalence[i].end());
      eq.insert(equivalence[selected].begin(),equivalence[selected].end());	
      for (std::set<unsigned>::iterator e=eq.begin(); e!=eq.end(); e++) equivalence[*e] = eq;    
    } //end if
  } //end for
}

// ------------------------------------------------------------------------------------------- //

void gnm::algorithm::post_sizerelevance()
{
  reset();

  unmerged = new bool[regions.size()];
  for (unsigned i=0; i<regions.size(); i++) unmerged[i]=true;

  to_lab();
  
  calc_dists();
  
  const unsigned rsize = regions.size();
  equivalence = new std::set<unsigned>[rsize];
  eq_set = new std::set<eq_pair>[rsize];

  for (unsigned i=0; i<rsize; i++) {
    equivalence[i].insert(i);
  } //end for

  unsigned size_limit = slimit;
  for (unsigned i=0; i<regions.size(); i++) {
    Labeling::Region *r1 = regions[i];
    unsigned s1 = r1->positions.size();
    bool small = (s1 < size_limit);

    int selected = -1;
    double min_d = 9999999.9;
    unsigned biggest = 0;

    for( std::map<Labeling::Region*, unsigned>::iterator it = r1->neighbors.begin(); it != r1->neighbors.end(); it++ ) {
      Labeling::Region *r2 = it->first;
      unsigned j = index[r2];
      unsigned s2 = r2->positions.size();

      if (!small && s2 < size_limit) continue;

      double ratio = std::min( (double)s1/s2, (double)s2/s1 );
      
      if (small) {
	if (s2 > biggest) {
	  biggest = s2;
	  selected = j;
	} //end if
      }
    } //end for

    std::set<unsigned> eq;
    //std::set<eq_pair> ep;
    if (selected != -1) {
      //unsigned a, b;
      //a = std::min(i,(unsigned)selected);
      //b = (a == i) ? selected : i;
      //eq_pair q1(a,b);

      //ep.insert(q1);	
      //ep.insert(eq_set[i].begin(),eq_set[i].end());
      //ep.insert(eq_set[selected].begin(),eq_set[selected].end());
      //for (std::set<eq_pair>::iterator e=ep.begin(); e!=ep.end(); e++) {
      //eq_set[e->first] = ep;
      //eq_set[e->second] = ep;
      //} //end for

      eq.insert(equivalence[i].begin(),equivalence[i].end());
      eq.insert(equivalence[selected].begin(),equivalence[selected].end());	
      for (std::set<unsigned>::iterator e=eq.begin(); e!=eq.end(); e++) equivalence[*e] = eq;    
    } //end if
  } //end for
}

// ------------------------------------------------------------------------------------------- //
