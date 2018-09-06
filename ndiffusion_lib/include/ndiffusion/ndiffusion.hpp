/**
 * \file ndiffusion.hpp
 *
 * Implements a generic framework for nonlinear diffusion, using an additive operator splitting scheme to perform it efficiently.
 * A generic way for declaring diffusivity functions and some example diffusivity functions are also present.
 *
 * IMPORTANT NOTE: this framework was based in the PHD thesis of Thomas Brox [BROX2005], "From Pixels to Regions: Partial Differential Equations
 * in Image Analysis", namely the section 2.1 of this thesis. The paper "Efficient and Reliable Schemes for Nonlinear Diffusion Filtering" [WEICKERT1998] was also
 * used.
 *
 * $Date 08/13/2008$
 * $Author Fogo (Rafael F. Bertoldi)$
 * $Author Guilherme Bertuol$
 */

#ifndef __NDIFFUSION_HPP__
#define __NDIFFUSION_HPP__

#include <ndiffusion/image.hpp>
#include <ndiffusion/utils.hpp>
#include <boost/array.hpp>

namespace ublas = boost::numeric::ublas;
using namespace color;

//----------------------------------------------------------------------------------------------------------------------------------//

/**
 * \brief Namespace containing functions and classes related to nonlinear diffusion algorithms.
 * \namespace ndiffusion
 */
namespace ndiffusion
{
  /*
   * \brief Index for a given dimension in a D dimension space.
   * \typedef axisdirection
   */
  typedef size_t axisdirection;

  //-------------------------------------------------------------------------------------//
  // DIFFUSIVITY FUNCTION
  //-------------------------------------------------------------------------------------//

  /**
   * \brief Generic class declaring an abstract function expected from a diffusivity function class.
   * \class diffusivity
   *
   * \tparam result_type The type to be returned by the diffusivity function.
   * \tparam param_type The type of the gradient parameter.
   *
   * To see more about diffusivity functions, we refer to pages 6 through 14 of [BROX2005].
   */
  template <typename result_type, typename param_type>
  class diffusivity
  {
  public:
    /**
     * \brief Abstract function declaring how to perform diffusivity according to a given gradient.
     * 
     * \param gradient A given gradient.
     * 
     * \return The diffusivity for the given gradient.       
     */
    virtual result_type calculate(param_type& gradient) = 0;
  };

  //-------------------------------------------------------------------------------------//
  // ADDITIVE OPERATOR SPLITTING (AOS)
  //-------------------------------------------------------------------------------------//
    
  /**
   * \brief Generic class declaring functions to implement an Additive Operator Splitting (AOS) scheme.
   * \class aos
   *
   * \tparam storage_type The type of container for the data.   
   * \tparam T The type of data contained.   
   * \tparam D The dimensionality of the algorithm.
   *
   * We refer to page 21 of [BROX2005] and [WEICKERT1998] for more details.
   */
  template <typename storage_type, typename T, size_t D>
  class aos
  {
  public:
    //-------------------------------------------------------------------------------------//
    // public functions
    //-------------------------------------------------------------------------------------//

    /**
     * \brief Performs AOS over a given image, using a diffusivity function for a number of iterations with a given tal parameter.
     *     
     * \param img The input image.
     * \param func A pointer to a diffusivity function.
     * \param niterations The number of iterations.
     * \param tal The tal parameter expected by the diffusion.
     * 
     * \tparam color_type The type of the colorspace for the input and output image.
     * \tparam result_type The result type for the diffusivity function.
     * \tparam gradient_type The gradient type given to the diffusivity function.
     * \tparam tal_type The precision expected for tal parameter.
     * 
     * \return The resulting image after the diffusion process realized through AOS.       
     */
    template <typename color_type, typename result_type, typename gradient_type, typename tal_type>
    Image<color_type> execute(Image<color_type> &img, diffusivity<result_type, gradient_type> *func, size_t niterations, tal_type tal) { return img;};

  protected:
    //-------------------------------------------------------------------------------------//
    // protected functions
    //-------------------------------------------------------------------------------------//

    /**
     * \brief Abstract function how to build the tridiagonal matrix A expected by the AOS.
     * 
     * \param gij A container with the diffusivity values for every unit contained by the image.
     * \param a A pointer to a vector where the lower diagonal of A is stored.
     * \param b A pointer to a vector where the main diagonal of A is stored.
     * \param c A pointer to a vector where the upper diagonal of A is stored.
     * \param d The desired direction when building the matrix.
     *
     * See page 20 of [BROX2005] and page 3 of [WEICKERT1998] for details about this tridiagonal matrix.
     */
    virtual void matrixA(const storage_type& gij, ublas::vector<T> *a, ublas::vector<T> *b, ublas::vector<T> *c, const axisdirection d) = 0;

    /**
     * \brief Abstract function how to build the container with the diffusivity values for the image.
     *      
     * \param u A container whose data is equivalent to the image data.
     * \param func Pointer to the diffusivity function to be used.
     *
     * \result A container with the diffusivity values for every unit contained by the image.
     */
    virtual storage_type matrixG(const storage_type& u, diffusivity<T, T> *func) = 0;
    /**
     * \brief Abstract function how to calculate the gradient vector for a given position.
     *      
     * \param u A container whose data is equivalent to the image data.
     * \param coords The position to be evaluated.
     *
     * \result A vector containing the gradient values for the given position.
     *
     * See page 15 of [BROX2005] for details.
     */
    virtual ublas::vector<T> gradient(const storage_type& u, const boost::array<size_t,D> &coords) = 0;
  };

  //-------------------------------------------------------------------------------------//
  // AOS 2D
  //-------------------------------------------------------------------------------------//

  /**
   * \brief Class defining functions to implement an Additive Operator Splitting (AOS) scheme in 2 dimensions using matrices.
   * \class aos2d
   *   
   * \tparam T The type of data contained.
   *
   * We refer to page 21 of [BROX2005] and [WEICKERT1998] for more details. The implementation for 2D dimensions was heavily
   * based on these two sources.
   */
  template <typename T>
  class aos2d : public aos<my2Dmatrix<T>, T, 2>
  {    
  public:
    /**
     * \brief Enumerates the directions for the 2D matrix
     * \enum MatrixDirection
     */
    enum MatrixDirection
      {
	mdByRow=0, /**< Direction preferred is running by the rows */
	mdByColumn=1 /**< Direction preferred is running by the columns */
      };
    
    //-------------------------------------------------------------------------------------//
    // public functions
    //-------------------------------------------------------------------------------------//
    
    /**
     * \brief Performs AOS in 2D over a given image, using a diffusivity function for a number of iterations with a given tal parameter.
     * 
     * \tparam color_type The type of the colorspace for the input and output image.
     * \tparam result_type The result type for the diffusivity function.
     * \tparam gradient_type The gradient type given to the diffusivity function.
     * \tparam tal_type The precision expected for tal parameter.
     *     
     * \param img The input image.
     * \param func A pointer to a diffusivity function.
     * \param niterations The number of iterations.
     * \param tal The tal parameter expected by the diffusion.
     * 
     * \return The resulting image after the diffusion process realized through AOS in 2D.       
     */
    template <typename color_type, typename result_type, typename gradient_type, typename tal_type>
    Image<color_type> execute(Image<color_type> &img, diffusivity<result_type, gradient_type> *func, size_t niterations, tal_type tal)
    {
      size_t width = img.width();
      size_t height = img.height();  
      size_t total = width * height;  
      const size_t D = 2;  
      
      const gradient_type ID = identity<gradient_type>() * static_cast<double>(D);    
      ublas::matrix<gradient_type> zerom = ublas::zero_matrix<gradient_type>(height,width);
      gradient_type zerog = zero<gradient_type>();

      my2Dmatrix<gradient_type> u(height,width);
      Image<gradient_type> cimg = static_cast<Image<gradient_type> >(img);      
      u.fromimage(cimg);

      ublas::vector<gradient_type> ah(total-1), bh(total), ch(total-1);
      ublas::vector<gradient_type> av(total-1), bv(total), cv(total-1);
      ublas::vector<gradient_type> d = u.tovector();
      
      typedef typename ublas::matrix<gradient_type>::iterator1 it1;
      typedef typename ublas::matrix<gradient_type>::iterator2 it2;

      my2Dmatrix<gradient_type> sumu(zerom);
      my2Dmatrix<gradient_type> gij = matrixG(u,func);
      my2Dmatrix<gradient_type> vh(height,width);
      my2Dmatrix<gradient_type> vv(height,width);    

      // k=0,...,niterations-1 iterations
      for (size_t k=0; k<niterations; ++k) {
	matrixA(gij,&ah,&bh,&ch,mdByRow);
	matrixA(gij,&av,&bv,&cv,mdByColumn);

	gradient_type multiplier = tal * D * D;
	for (size_t i=0; i<total-1; i++) {
	  gradient_type aa = (ah(i) * multiplier * -1.);
	  ah(i) = aa;
	  gradient_type bb = ID - (bh(i) * multiplier);
	  bh(i) = bb;
	  gradient_type cc = (ch(i) * multiplier * -1.);
	  ch(i) = cc;
      
	  aa = (av(i) * multiplier * -1.);
	  av(i) = aa;
	  bb = ID - (bv(i) * multiplier);
	  bv(i) = bb;
	  cc = (cv(i) * multiplier * -1.);
	  cv(i) = cc;
	}
	bh(total-1) = ID - (bh(total-1) * multiplier);
	bv(total-1) = ID - (bv(total-1) * multiplier);
	vh.fromvector(thomasalg(ah,bh,ch,d));
	vv.fromvector(thomasalg(av,bv,cv,d));

	sumu += vv + vh;
      } //end iterations

      my2Dmatrix<gradient_type> result( sumu / static_cast<double>(niterations) );
      Image<color_type> out = result.toimage();
  
      return out;
  }
    
  //-------------------------------------------------------------------------------------//
    
  protected:

    //-------------------------------------------------------------------------------------//
    // protected functions
    //-------------------------------------------------------------------------------------//

    /**
     * \brief Function that builds the tridiagonal matrix A expected by the AOS from a 2D matrix container.
     * 
     * \param gij A 2D matrix with the diffusivity values for every unit contained by the image.
     * \param a A pointer to a vector where the lower diagonal of A is stored.
     * \param b A pointer to a vector where the main diagonal of A is stored.
     * \param c A pointer to a vector where the upper diagonal of A is stored.
     * \param d The desired direction when building the matrix.
     *
     * See page 20 of [BROX2005] and page 3 of [WEICKERT1998] for details about this tridiagonal matrix.
     */
    void matrixA(const my2Dmatrix<T>& gij, ublas::vector<T> *a, ublas::vector<T> *b, ublas::vector<T> *c, const axisdirection d)
    {
      size_t width = gij.size2();
      size_t height = gij.size1();

      size_t n = width*height;
      
      size_t dim = (d == mdByRow) ? width : height;
      size_t tmp11, tmp12;
      size_t tmp21, tmp22;
      size_t *r1 = (d == mdByRow) ? &tmp11 : &tmp12;
      size_t *c1 = (d == mdByRow) ? &tmp12 : &tmp11;
      size_t *r2 = (d == mdByRow) ? &tmp21 : &tmp22;
      size_t *c2 = (d == mdByRow) ? &tmp22 : &tmp21;


      for (size_t i=0; i<n-1; i++) {
	tmp11 = i / dim;
	tmp12 = i % dim;	
	
	tmp21 = (i+1) / dim;
	tmp22 = (i+1) % dim;
	
	T gc = ( gij(*r1,*c1) + gij(*r2,*c2) ) / 2.;
	T ga = ( gij(*r1,*c1) + gij(*r2,*c2) ) / 2.;

	(*a)(i) = ga;
	(*c)(i) = gc;
      }

      (*b)(0) = -1. * (*c)(0);
      for (size_t i=1; i<n-1; i++) {
	(*b)(i) = -1. * ((*c)(i) + (*a)(i-1));
      }
      (*b)(n-1) = -1. * (*a)(n-2);
    }

    //-------------------------------------------------------------------------------------//
    
    /**
     * \brief Function that builds a 2D matrix with the diffusivity values for the image.
     *      
     * \param u A 2D matrix whose data is equivalent to the image data.
     * \param func Pointer to the diffusivity function to be used.
     *
     * \result A 2D matrix with the diffusivity values for every unit contained by the image.
     */
    my2Dmatrix<T> matrixG(const my2Dmatrix<T>& u, diffusivity<T, T> *func)
    {
      size_t h = u.size1();
      size_t w = u.size2();

      ublas::matrix<T> tmp = ublas::zero_matrix<T>(h,w);
      my2Dmatrix<T> gij(tmp);

      const T tzero = zero<T>();

      size_t row = 1;
      size_t col = 1;

      typedef typename ublas::matrix<T>::const_iterator1 it1;
      typedef typename ublas::matrix<T>::const_iterator2 it2;

      for (it1 ii = u.begin1()+1;ii != u.end1()-1; ii++) {
	size_t cnt = 0;
    
	col = 1;
	gij(row,0) = tzero;
	gij(row,w-1) = tzero;
	for (it2 jj = ii.begin()+1; jj != ii.end()-1; jj++){
	  boost::array<size_t,2> coords = {row, col};
	  ublas::vector<T> grad = gradient(u,coords);
	  //ublas::vector<T> grad = const_cast<my2Dmatrix<T>& >(u).gradient(coords);

	  T uij = u(row,col);
	  T utop = u(row-1,col);
	  T uleft = u(row,col-1);
	  T uright = u(row,col+1);
	  T ubottom = u(row+1,col);

	  T diff1 = ubottom - uij;
	  T diff2 = uij - utop;
	  T diff3 = uright - uij;
	  T diff4 = uij - uleft;

	  T grad1 = grad(0);
	  T grad2 = grad(1);
	  T grad3 = grad(2);
	  T grad4 = grad(3);

	  T g1 = func->calculate(grad1);
	  T g2 = func->calculate(grad2);
	  T g3 = func->calculate(grad3);
	  T g4 = func->calculate(grad4);

	  T pt1 = g1 * diff1;
	  T pt2 = g2 * diff2;
	  T pt3 = g3 * diff3;
	  T pt4 = g4 * diff4;

	  T g = pt1 - pt2 + pt3 - pt4;
	  gij(row,col) = color::abs(g);

	  ++col;
	} //end secondary dimension
	++row;
      } //end primary dimension

      return gij;
    }

    //-------------------------------------------------------------------------------------//
    
    /**
     * \brief Function that calculates the gradient vector for a given position in the 2D matrix.
     *      
     * \param u A 2D matrix whose data is equivalent to the image data.
     * \param coords The position (i,j) to be evaluated.
     *
     * \result A vector containing the gradient values for the position (i,j).
     *
     * See page 15 of [BROX2005] for details.
     */    
    ublas::vector<T> gradient(const my2Dmatrix<T>& u, const boost::array<size_t,2> &coords)
    {      
      size_t i = coords[0];
      size_t j = coords[1];

      T uij = u(i,j);
      T u1 = u(i-1,j-1);
      T u2 = u(i-1,j);
      T u3 = u(i-1,j+1);
      T u4 = u(i,j-1);
      T u5 = u(i,j+1);
      T u6 = u(i+1,j-1);
      T u7 = u(i+1,j);
      T u8 = u(i+1,j+1);

      T tmp1 = u7 - uij;
      T tmp2 = uij - u2;
      T tmp3 = u5 - uij;
      T tmp4 = uij - u4;
      T tmp5 = 0.5*((u8-u6) * 0.5  + (u5-u4) * 0.5);
      T tmp6 = 0.5*(0.5*(u3-u1) + 0.5*(u5-u4));
      T tmp7 = 0.5*(0.5*(u8-u3) + 0.5*(u7-u2));
      T tmp8 = 0.5*(0.5*(u6-u1) + 0.5*(u7-u2));  

      T part1 = (tmp1 * tmp1) + (tmp5 * tmp5);
      T part2 = (tmp2 * tmp2) + (tmp6 * tmp6);
      T part3 = (tmp3 * tmp3) + (tmp7 * tmp7);
      T part4 = (tmp4 * tmp4) + (tmp8 * tmp8);

      T sq1 = color::sqrt(part1);
      T sq2 = color::sqrt(part2);
      T sq3 = color::sqrt(part3);
      T sq4 = color::sqrt(part4);

      ublas::vector<T> v(4);
      v(0) = sq1;
      v(1) = sq2;
      v(2) = sq3;
      v(3) = sq4;

      return v;
    }    

    //-------------------------------------------------------------------------------------//

  };
  

  //-------------------------------------------------------------------------------------//

};

//----------------------------------------------------------------------------------------------------------------------------------//
// DIFFUSIVITY FUNCTIONS
//----------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------//
// Perona-Malik diffusivity function
//-------------------------------------------------------------------------------------//

/**
 * \brief Class defining a Perona-Malik diffusivity function.
 * \class perona_malik
 *
 * \tparam result_type The type to be returned by the diffusivity function.
 * \tparam param_type The type of the gradient parameter.
 * \tparam lambda_type The precision expected for the lambda parameter.
 *
 * Based on the equation 2.4 in page 7 of [BROX2005].
 */
template <typename result_type, typename param_type, typename lambda_type=double>
class perona_malik : public ndiffusion::diffusivity<result_type, param_type>
{
public:
  /**
   * \brief Constructs a Perona-Malik diffusivity function with lambda parameter.
   *         
   * \param lambda The parameter lambda expected by this function.
   */
  perona_malik(lambda_type l) : m_lambda(l), ndiffusion::diffusivity<result_type, param_type>() {};

  /**
   * \brief Performs the Perona-Malik diffusivity function over a given gradient.    
   * 
   * \param gradient A given gradient.
   * 
   * \return The diffusivity for the given gradient.
   */
  result_type calculate(param_type& gradient);
protected:
  lambda_type m_lambda;
};

//----------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------//
// Stabilized TV flow diffusivity function
//-------------------------------------------------------------------------------------//

/**
 * \brief Class defining a stabilized version of total variation(TV) diffusivity function.
 * \class stabilized_tvflow
 *
 * \tparam result_type The type to be returned by the diffusivity function.
 * \tparam param_type The type of the gradient parameter.
 * \tparam e_type The precision expected for the e parameter.
 * \tparam p_type The precision expected for the p parameter.
 *
 * Based on the equation 2.18 in page 18 of [BROX2005].
 */
template <typename result_type, typename param_type, typename e_type=double, typename p_type=double>
class stabilized_tvflow : public ndiffusion::diffusivity<result_type, param_type>
{
public:
  /**
   * \brief Constructs a stabilized TV flow diffusivity function with e and p parameters.
   *         
   * \param e Parameter used to avoid division by zero, usually receives very small values.
   * \param p A parameter that control the diffusivity performed.   
   */
  stabilized_tvflow(e_type e, p_type p) : m_e(e), m_p(p), ndiffusion::diffusivity<result_type, param_type>() {};
    
  /**
   * \brief Performs the stabilized TV flow diffusivity function over a given gradient.    
   * 
   * \param gradient A given gradient.
   * 
   * \return The diffusivity for the given gradient.
   */
  result_type calculate(param_type& gradient);
protected:
  e_type m_e;
  p_type m_p;
};

//----------------------------------------------------------------------------------------------------------------------------------//


#endif
