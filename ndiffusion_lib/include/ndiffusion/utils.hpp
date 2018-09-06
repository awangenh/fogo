/**
 * \file utils.hpp
 *
 * Contains auxiliar containers and algorithms related loosely to the nonlinear diffusion algorithm.
 *
 * $Date 08/13/2008$
 * $Author Fogo (Rafael F. Bertoldi)$
 * $Author Guilherme Bertuol$
 */

#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <boost/numeric/ublas/storage.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/array.hpp>

namespace ublas = boost::numeric::ublas;
using namespace color;

//----------------------------------------------------------------------------------------------------------------------------------//
// CONTAINERS AUXILIAR TO AOS ALGORITHM
//----------------------------------------------------------------------------------------------------------------------------------//

/**
 * \brief Class declaring a container of type T information of D dimension, which functions support the AOS algorithm.
 * \class aosstorage
 *
 * \tparam T The type of the data contained.
 * \tparam D The dimensionality of this container.
 */
template <typename T, size_t D>
class aosstorage {  
public:
  //-------------------------------------------------------------------------------------//
  // public functions
  //-------------------------------------------------------------------------------------//

  /**
   * \brief Converts the container data to a 1D vector format.
   * 
   * \return The vector containing the converted data.       
   */
  virtual ublas::vector<T> tovector() = 0;
  /**
   * \brief Converts the data from a 1D vector to the container data.
   * 
   * \param v The vector from which the data will be converted.
   */
  //virtual void fromvector(const ublas::vector<T>& v, const boost::array<size_t,D> &size) = 0;
  virtual void fromvector(const ublas::vector<T>& v) = 0;
  /**
   * \brief Converts the data from an image to the container data.
   * 
   * \param img The image to be converted.
   */
  virtual void fromimage(Image<T>& img) = 0;
  /**
   * \brief Converts the container data to an image format.
   * 
   * \return The image represent the converted data from the container.       
   */
  virtual Image<T> toimage() = 0;
  
};

//----------------------------------------------------------------------------------------------------------------------------------//
// CONTAINER AUXILIAR TO 2D VERSION OF AOS ALGORITHM
//----------------------------------------------------------------------------------------------------------------------------------//

/**
 * \brief Class defining aosstorage functions for a 2D matrix format container.
 * \class my2Dmatrix
 *
 * \tparam T The type of the data contained by the matrix.
 */
template <typename T>
class my2Dmatrix : public ublas::matrix<T>,public aosstorage<T,2> {
public:
  //-------------------------------------------------------------------------------------//
  // public functions
  //-------------------------------------------------------------------------------------//

  /**
   * \brief Constructs a 2D matrix with i rows and j columns.
   * 
   * \param i Number of rows.
   * \param j Number of columns.
   */
  my2Dmatrix(const size_t i, const size_t j) : ublas::matrix<T>(i,j)
  {
  }

  //-------------------------------------------------------------------------------------//

  /**
   * \brief Constructs a 2D matrix taking a BOOST uBLAS matrix as input.
   * 
   * \param m A BOOST uBLAS matrix.
   */
  my2Dmatrix(const ublas::matrix<T>& m) : ublas::matrix<T>(m.size1(),m.size2())
  {      
    typedef typename ublas::matrix<T>::const_iterator1 cit1;
    typedef typename ublas::matrix<T>::const_iterator2 cit2;

    typedef typename ublas::matrix<T>::iterator1 it1;
    typedef typename ublas::matrix<T>::iterator2 it2;

    it1 i1 = this->begin1();
    cit1 mi1 = m.begin1();

    for (; i1 != this->end1(); i1++, mi1++) {
      it2 i2 = i1.begin();
      cit2 mi2 = mi1.begin();
      for (; i2 != i1.end(); i2++, mi2++) {
	*i2 = *mi2;
      }
    }
  }

  //-------------------------------------------------------------------------------------//

  /**
   * \brief Converts the 2D matrix to a 1D vector format.
   * 
   * \return The vector containing the converted matrix data.       
   *
   * The 1D vector size will be the matrix width*height.
   */
  ublas::vector<T> tovector()
  {
    size_t s1 = this->size1();
    size_t s2 = this->size2();
    size_t mins = std::min(s1,s2);

    size_t n = s1 * s2;
    ublas::vector<T> v(n);
      
    typedef typename ublas::matrix<T>::const_iterator1 it1;
    typedef typename ublas::matrix<T>::const_iterator2 it2;

    size_t c = 0;
    for (it1 mit1 = this->begin1();mit1 != this->end1(); mit1++)
      for (it2 mit2 = mit1.begin();mit2 != mit1.end(); mit2++)
	v(c++) = *mit2;

    return v;
  }

  //-------------------------------------------------------------------------------------//
  
  /**
   * \brief Converts the data from a 1D vector to the 2D matrix data.
   * 
   * \param v The vector from which the data will be converted.
   *
   * It is expected that the vector size is equal to the matrix width*height.
   */
  //void fromvector(const ublas::vector<T>& v, const boost::array<size_t,2> &size)
  void fromvector(const ublas::vector<T>& v)
  {
    typedef typename ublas::matrix<T>::iterator1 it1;
    typedef typename ublas::matrix<T>::iterator2 it2;

    size_t c = 0;
    for (it1 mit1 = this->begin1();mit1 != this->end1(); mit1++)
      for (it2 mit2 = mit1.begin();mit2 != mit1.end(); mit2++)
	*mit2 = v(c++);
  }

  //-------------------------------------------------------------------------------------//

  /**
   * \brief Converts the data from an image to the 2D matrix data.
   * 
   * \param img The image to be converted.
   *
   * It is expected that the image dimensions are equivalent to the matrix dimensions.
   */
  void fromimage(Image<T>& img)
  {    
    typedef typename ublas::matrix<T>::iterator1 it1;
    typedef typename ublas::matrix<T>::iterator2 it2;
  
    size_t c = 0;
    for (it1 i = this->begin1(); i != this->end1(); i++) {     
      for (it2 j = i.begin(); j != i.end(); j++){
	*j = img[c++];
      }
    }
  }

  //-------------------------------------------------------------------------------------//

  /**
   * \brief Converts the 2D matrix data to an image format.
   * 
   * \return The image represent the 2D matrix data from the container.
   *
   * The image dimensions will be the same as the matrix ones.
   */
  Image<T> toimage()
  {
    size_t w = this->size2();
    size_t h = this->size1();

    Image<T> img(w,h);

    typedef typename ublas::matrix<T>::iterator1 it1;
    typedef typename ublas::matrix<T>::iterator2 it2;

    size_t c = 0;
    for (it1 i = this->begin1(); i != this->end1(); i++) {     
      for (it2 j = i.begin(); j != i.end(); j++){
	img[c++] = *j;
      }
    }
    
    return img;
  }

  //-------------------------------------------------------------------------------------//
    
};

//----------------------------------------------------------------------------------------------------------------------------------//
// THOMAS/TRIDIAGONAL MATRIX ALGORITHM
//----------------------------------------------------------------------------------------------------------------------------------//

/**
 * \brief Implementation of the Thomas algorithm, a.k.a. tridiagonal matrix algorithm.
 * 
 * \tparam T The type of the data contained by the vectors and matrix.
 *
 * \param a A vector containing the lower diagonal of a matrix M.
 * \param b A vector containing the main diagonal of a matrix M.
 * \param c A vector containing the upper diagonal of a matrix M.
 * \param d A known vector equal to the result of M * x.
 *
 * \return A vector corresponding to the variables vector x.
 *
 * This algorithm can be used to solve tridiagonal system of equations. Systems of equations like M * x = d, where M is a tridiagonal 
 * matrix, x is a vector of variables and d is the resulting vector from M * x, can be solved with a complexity of O(n). For more
 * information: http://www.cfd-online.com/Wiki/Tridiagonal_matrix_algorithm_-_TDMA_(Thomas_algorithm)
 */
template <typename T>
ublas::vector<T> thomasalg(const ublas::vector<T> &a, const ublas::vector<T> &b, const ublas::vector<T> &c, const ublas::vector<T> &d)
{  
  size_t n = d.size();
  ublas::vector<T> u(n);

  ublas::vector<T> bb = ublas::vector<T>(b);
  ublas::vector<T> dd = ublas::vector<T>(d);
  for (size_t k=1; k<n; k++) {
    T m = a(k-1) / ( bb(k-1) + 0.001);
    T tmpb = bb(k) - (m * c(k-1));
    bb(k) = tmpb;
    T tmpd = dd(k) - (m * dd(k-1)); 
    dd(k) = tmpd;
  }

  u(n-1) = dd(n-1) / bb(n-1);
  for (int k=n-2; k>=0; k--) {
    u(k) = (dd(k) - c(k)*u(k+1)) / ( bb(k) + 0.001 );
  }

  return u;
}

//----------------------------------------------------------------------------------------------------------------------------------//

#endif
