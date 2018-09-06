/**
 * \file image.hpp
 *
 * Provides a generic image class and some auxiliar functions, as read and write in files.
 *
 * $Date 08/13/2008$
 * $Author Fogo (Rafael F. Bertoldi)$
 * $Author Guilherme Bertuol$
 */

#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include <boost/numeric/ublas/storage.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <imgman/ImageManipulator.hpp>
#include <ndiffusion/colors.hpp>
#include <iostream>

namespace ublas = boost::numeric::ublas;
using namespace color;

//----------------------------------------------------------------------------------------------------------------------------------//
// IMAGE
//----------------------------------------------------------------------------------------------------------------------------------//

/**
 * \brief Generic class offering a simple representation of 2D images.
 * \class Image
 *
 * \tparam T The type of the pixel data.
 */
template<typename T>
class Image {
public:
  //-------------------------------------------------------------------------------------//
  // public functions
  //-------------------------------------------------------------------------------------//

  /*
   * \brief Iterator for the image pixel data.
   * \typedef iterator
   */
  typedef typename ublas::unbounded_array<T>::iterator        iterator;
  /*
   * \brief Const iterator for the image pixel data.
   * \typedef const_iterator
   */
  typedef typename ublas::unbounded_array<T>::const_iterator  const_iterator;

  /**
   * \brief Empty constructor of an image.
   */
  Image() : data(0), m_width(0), m_height(0) { }  
  /**
   * \brief Constructs an image of w*h size.
   *
   * \param w Image width.
   * \param h Image height.
   */
  Image(const size_t w, const size_t h) : data(w*h), m_width(w), m_height(h) { }  
  /**
   * \brief Copy constructor for an image.
   *
   * \tparam T2 The other image type of pixel data.
   *
   * \param other Other image.
   */
  template<typename T2>
  Image(const Image<T2>& other) { 
    m_width = other.width();
    m_height = other.height();
    data = ublas::unbounded_array<T>(m_width*m_height);

    typedef typename ublas::unbounded_array<T2>::const_iterator otheriterator;
    otheriterator otherit = other.begin();
    iterator it = begin();

    for (;it != end(); it++, otherit++)
      *it = static_cast<T>(*otherit);
  }

  //-------------------------------------------------------------------------------------//

  /**
   * \brief Set the value of a pixel in the given position.
   *
   * \param pos A position in the image.
   * \param value The value to be set.
   */
  void set(const size_t pos, const T& value) {
    data[pos] = value;
  }

  //-------------------------------------------------------------------------------------//

  /**
   * \brief Returns the width of the image.
   *
   * \return The image width.
   */
  size_t width() const {
    return m_width;
  }

  //-------------------------------------------------------------------------------------//
  
  /**
   * \brief Returns the height of the image.
   *
   * \return The image height.
   */
  size_t height() const {
    return m_height;
  }

  //-------------------------------------------------------------------------------------//

  /**
   * \brief Operator that gives access to the references for pixel data values.
   *
   * \param i A position in the image.
   *
   * \return A reference to the value in the give position.
   */
  T& operator[](const size_t i) {
    return data[i];
  }

  //-------------------------------------------------------------------------------------//

  /**
   * \brief An iterator located in the first position of the pixel data.
   *
   * \return An iterator located in the first position of the pixel data.
   */
  iterator begin()
  {
    return data.begin();
  }

  //-------------------------------------------------------------------------------------//
  
  /**
   * \brief A constant iterator located in the first position of the pixel data.
   *
   * \return A constant iterator located in the first position of the pixel data.
   */
  const_iterator begin() const
  {
    return data.begin();
  }

  //-------------------------------------------------------------------------------------//

  /**
   * \brief An iterator located after the last position of the pixel data.
   *
   * \return An iterator located after the last position of the pixel data.
   */
  iterator end()
  {
    return data.end();
  }

  //-------------------------------------------------------------------------------------//
  
  /**
   * \brief A constant iterator located after the last position of the pixel data.
   *
   * \return A constant iterator located after the last position of the pixel data.
   */
  const_iterator end() const
  {
    return data.end();
  }  

  //-------------------------------------------------------------------------------------//

protected:
  ublas::unbounded_array<T> data;

  size_t m_width, m_height;  
};

//----------------------------------------------------------------------------------------------------------------------------------//

/**
 * \brief Generic function to read an image from a file.
 *
 * \tparam T The type of the pixel data.
 *
 * \param filepath The path to where the file is located in the file system.
 *
 * \return An image read from a file.
 *
 * Observation: it has no pratical purpose, it is supposed only to define the structure to be followed by generalizations.
 */
template <typename T>
static Image<T>& read(const std::string& filepath) {
  Image<T> p;
  return p;
}

//----------------------------------------------------------------------------------------------------------------------------------//

/**
 * \brief Generalization that reads an image of Pixel8 from a file.
 *
 * \param filepath The path to where the file is located in the file system.
 *
 * \return An image of Pixel8 read from the file.
 */
template<>
static Image<Pixel8>& read(const std::string& filepath)
{
  imgman::ImageManipulator imgman;
  try {
    boost::shared_ptr<imgman::Image> image_p = imgman.OpenImage(filepath);   
      
    size_t h = image_p->GetHeight();
    size_t w = image_p->GetWidth();
    static Image<Pixel8> img(w,h);
    imgman::Intensity* readdata = image_p->GetPixelData().get();    

    Image<Pixel8>::iterator imgit = img.begin();
    for (; imgit != img.end(); imgit++) {
      *imgit = *readdata++;
    }

    return img;
  } catch(imgman::Exception &ex){
    std::cout << ex.what() << std::endl;

    static Image<Pixel8> err;
    return err;
  } //end try
}

//----------------------------------------------------------------------------------------------------------------------------------//

/**
 * \brief Generalization that reads an image of RGB<Color8> from a file.
 *
 * \param filepath The path to where the file is located in the file system.
 *
 * \return An image of RGB<Color8> read from the file.
 */
template<>
static Image<RGB<Color8> >& read(const std::string& filepath)
{
  imgman::ImageManipulator imgman;
  try {
    boost::shared_ptr<imgman::Image> image_p = imgman.OpenImage(filepath);   
      
    size_t h  = image_p->GetHeight();
    size_t w  = image_p->GetWidth();
    static Image<RGB<Color8> > img(w,h);
    imgman::Intensity* readdata = image_p->GetPixelData().get();    

    Image<RGB<Color8> >::iterator imgit = img.begin();
    for (; imgit != img.end(); imgit++) {
      imgit->red = *readdata++;
      imgit->green = *readdata++;
      imgit->blue = *readdata++;
    }

    return img;
  } catch(imgman::Exception &ex){
    std::cout << ex.what() << std::endl;

    static Image<RGB<Color8> > err;
    return err;
  } //end try
}

//----------------------------------------------------------------------------------------------------------------------------------//

/**
 * \brief Generic function to write an image to a file.
 *
 * \tparam T The type of the pixel data.
 *
 * \param img The image to be written.
 * \param filepath The path to where the file will be located in the file system.
 *
 * \return An integer value indicating sucess or failure.
 *
 * Observation: it has no pratical purpose, it is supposed only to define the structure to be followed by generalizations.
 */
template <typename T>
static int write(const Image<T>& img, const std::string& filepath) 
{
  return 1;
}

//----------------------------------------------------------------------------------------------------------------------------------//

/**
 * \brief Generalization that writes an image of Pixel8 to a file.
 *
 * \param img The image of Pixel8 to be written.
 * \param filepath The path to where the file will be located in the file system.
 *
 * \return An integer value indicating sucess or failure.
 */
template <>
static int write(const Image<Pixel8>& img, const std::string& filepath) 
{
  imgman::ImageManipulator imgman;
  
  try {    
    size_t h = img.width();
    size_t w = img.height();

    imgman::Image imgfile;
    imgfile.New(1,w,h,255);
    imgman::Intensity* data = imgfile.GetPixelData().get();
    
    for(Image<Pixel8>::const_iterator it = img.begin(); it != img.end(); ++it) {
      *data++ = *it;
    }

    imgman.SaveImage(imgfile, filepath);

    return 0;
  } catch(imgman::Exception &ex){
    std::cout << ex.what() << std::endl;

    return 1;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------//

/**
 * \brief Generalization that writes an image of RGB<Color8> to a file.
 *
 * \param img The image of RGB<Color8> to be written.
 * \param filepath The path to where the file will be located in the file system.
 *
 * \return An integer value indicating sucess or failure.
 */
template <>
static int write(const Image<RGB<Color8> >& img, const std::string& filepath) 
{
  imgman::ImageManipulator imgman;
  
  try {    
    size_t w = img.width();
    size_t h = img.height();

    imgman::Image imgfile;
    imgfile.New(3,w,h,255);
    imgman::Intensity* data = imgfile.GetPixelData().get();

    for(Image<RGB<Color8> >::const_iterator it = img.begin(); it != img.end(); ++it) {
      *data++ = it->red;
      *data++ = it->green;
      *data++ = it->blue;
    }

    imgman.SaveImage(imgfile, filepath);

    return 0;
  } catch(imgman::Exception &ex){
    std::cout << ex.what() << std::endl;

    return 1;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------//

#endif
