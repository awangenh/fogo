/**
 * \file colors.hpp
 *
 * Defines colorspaces such as RGB and others.
 *
 * $Date 08/13/2008$
 * $Author Fogo (Rafael F. Bertoldi)$
 * $Author Guilherme Bertuol$
 */

#ifndef __COLORS_HPP__
#define __COLORS_HPP__

//----------------------------------------------------------------------------------------------------------------------------------//

/**
 * \brief Namespace containing types and functions related to colorspaces.
 * \namespace color
 */
namespace color
{
  /*
   * \brief A 1D colorspace, 8 bytes alocated a pixel.
   * \typedef Pixel8
   */
  typedef unsigned char Pixel8;

  //----------------------------------------------------------------------------------------------------------------------------------//

  /*
   * \brief A tuple of N-D colorspaces with 8 bytes alocated a tuple.
   * \typedef Color8
   */
  typedef unsigned char Color8;

  //----------------------------------------------------------------------------------------------------------------------------------//
  // RGB
  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Generic class defining the RGB colors.
   * \class RGB
   *
   * \tparam T1 The type of the RGB tuples.
   */
  template <typename T1>
  class RGB 
  {
  public:
    template <typename T>
    friend RGB<T> operator+(const RGB<T>& src, const RGB<T>& other);  
    template <typename T>
    friend RGB<T> operator*(const RGB<T>& src, const RGB<T>& other);
    template <typename T>
    friend RGB<T> operator*(const RGB<T>& src, const T& other);
    template <typename T>
    friend RGB<T> operator-(const RGB<T>& src, const RGB<T>& other);
  
    /**
     * \brief The red component of RGB
     */
    T1 red;
    /**
     * \brief The green component of RGB
     */
    T1 green;
    /**
     * \brief The blue component of RGB
     */
    T1 blue;

    //-------------------------------------------------------------------------------------//

    /**
     * \brief Copy constructor of RGB.
     *
     * \tparam T2 The other RGB tuples type.
     *
     * \param src Other RGB color.
     */
    template<typename T2>
    RGB(const RGB<T2>& src)
    {
      red = static_cast<T1>(src.red);
      green = static_cast<T1>(src.green);
      blue = static_cast<T1>(src.blue);
    }

    /**
     * \brief Empty constructor of RGB.
     */
    RGB()
    {
      red = 0;
      green = 0;
      blue = 0;
    };

    /**
     * \brief Constructs RGB with all tuples receiving a same value.
     *
     * \param value The value to be given to the tuples.
     */
    RGB(const T1& value) : red(value), green(value), blue(value)
    {
    
    };

    /*
      RGB(const RGB<T1>& other) : red(other.red), green(other.green), blue(other.blue)
      {
    
      };
    */
  
    /**
     * \brief Constructs RGB with given values for every tuple.
     *
     * \param r The value given for the red component.
     * \param g The value given for the green component.
     * \param b The value given for the blue component.
     */
    RGB(const T1& r, const T1& g, const T1& b) : red(r), green(g), blue(b)
    {
    };
    
    //-------------------------------------------------------------------------------------//

    /**
     * \brief Operator of equality.
     *
     * \param other Other RGB color.
     *
     * \return True if equal, false if not.
     */
    bool operator==(const RGB<T1>& other)
    {
      return (red == other.red && green == other.green && blue == other.blue);
    }

    //-------------------------------------------------------------------------------------//

    /**
     * \brief Operator of inequality.
     *
     * \param other Other RGB color.
     *
     * \return True if false, false if true.
     */
    bool operator!=(const RGB<T1>& other)
    {
      return !operator==(other);
    }

    //-------------------------------------------------------------------------------------//
  
    /**
     * \brief Operator of sum and assignment.
     *
     * \param other Other RGB color.
     *
     * \return A reference to this color summed with an other color.
     */
    RGB<T1>& operator+=(const RGB<T1>& other)
    {
      red += other.red;
      green += other.green;
      blue += other.blue;
    
      return *this;
    }

    //-------------------------------------------------------------------------------------//

    /**
     * \brief Operator of multiplication and assignment.
     *
     * \param other Other RGB color.
     *
     * \return A reference to this color multiplicated by an other color.
     */
    RGB<T1>& operator*=(const RGB<T1>& other)
    {
      red *= other.red;
      green *= other.green;
      blue *= other.blue;
    
      return *this;
    }

    //-------------------------------------------------------------------------------------//

  };

  //----------------------------------------------------------------------------------------------------------------------------------//
  // RGB NON-ASSIGNMENT/BOOLEAN OPERATORS
  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Operator of sum of colors.
   *
   * \tparam T1 The type of the RGB tuples.
   *
   * \param src A RGB color.
   * \param other A RGB color.
   *
   * \return src + other.
   */
  template <typename T1>
  RGB<T1> operator+(const RGB<T1>& src, const RGB<T1>& other)
  {
    RGB<T1> res(src.red + other.red, src.green + other.green, src.blue + other.blue);
  
    return res;
  }

  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Operator of sum of color with a scalar.
   *
   * \tparam T1 The type of the RGB tuples.
   *
   * \param src A RGB color.
   * \param other A scalar.
   *
   * \return src.red + other,src.green + other,src.blue + other.
   */
  template<typename T1>
  RGB<T1> operator+(const RGB<T1>& src, const T1& other)
  {
    RGB<T1> res(src.red + other, src.green + other, src.blue + other);

    return res;
  }

  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Operator of sum of a scalar with a color.
   *
   * \tparam T1 The type of the RGB tuples.
   *
   * \param other A scalar.
   * \param src A RGB color.
   *
   * \return src.red + other,src.green + other,src.blue + other.
   */
  template<typename T1>
  RGB<T1> operator+(const T1& other, const RGB<T1>& src)
  {
    return operator+(src,other);
  }

  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Operator of difference of colors.
   *
   * \tparam T1 The type of the RGB tuples.
   *
   * \param src A RGB color.
   * \param other A RGB color.
   *
   * \return src - other.
   */
  template <typename T1>
  RGB<T1> operator-(const RGB<T1>& src, const RGB<T1>& other)
  {
    RGB<T1> res(src.red - other.red, src.green - other.green, src.blue - other.blue);
    return res;
  }

  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Operator of multiplication of colors.
   *
   * \tparam T1 The type of the RGB tuples.
   *
   * \param src A RGB color.
   * \param other A RGB color.
   *
   * \return src * other.
   */
  template<typename T1>
  RGB<T1> operator*(const RGB<T1>& src, const RGB<T1>& other)
  {
    RGB<T1> res(src.red * other.red, src.green * other.green, src.blue * other.blue);
  
    return res;
  }

  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Operator of multiplication of a color with a scalar.
   *
   * \tparam T1 The type of the RGB tuples.
   *
   * \param src A RGB color.
   * \param other A scalar.
   *
   * \return src.red * other,src.green * other,src.blue * other.
   */
  template<typename T1>
  RGB<T1> operator*(const RGB<T1>& src, const T1& other)
  {
    RGB<T1> res(src.red * other, src.green * other, src.blue * other);

    return res;
  }

  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Operator of multiplication of a scalar with a color.
   *
   * \tparam T1 The type of the RGB tuples.
   *
   * \param src A RGB color.
   * \param other A scalar.
   *
   * \return src.red * other,src.green * other,src.blue * other.
   */
  template<typename T1>
  RGB<T1> operator*(const T1& other, const RGB<T1>& src)
  {
    //RGB<T1> res(src.red * other, src.green * other, src.blue * other);

    //return res;
    return operator*(src,other);
  }

  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Operator of division of colors.
   *
   * \tparam T1 The type of the RGB tuples.
   *
   * \param src A RGB color.
   * \param other A RGB color.
   *
   * \return src / other.
   */
  template<typename T1>
  RGB<T1> operator/(const RGB<T1>& src, const RGB<T1>& other)
  {
    RGB<T1> res(src.red / other.red, src.green / other.green, src.blue / other.blue);
  
    return res;
  }

  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Operator of division of a color with a scalar.
   *
   * \tparam T1 The type of the RGB tuples.
   *
   * \param src A RGB color.
   * \param other A scalar.
   *
   * \return src.red / other,src.green / other,src.blue / other.
   */
  template<typename T1>
  RGB<T1> operator/(const RGB<T1>& src, const T1& other)
  {
    RGB<T1> res(src.red / other, src.green / other, src.blue / other);

    return res;
  }

  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Operator of division of a scalar with a color.
   *
   * \tparam T1 The type of the RGB tuples.
   *
   * \param other A scalar.
   * \param src A RGB color.
   *
   * \return other / src.red,other / src.green,other / src.blue.
   */
  template<typename T1>
  RGB<T1> operator/(const T1& other, const RGB<T1>& src)
  {
    RGB<T1> res(other / src.red,other / src.green,other / src.blue);

    return res;
  }

  //----------------------------------------------------------------------------------------------------------------------------------//
  // VECTOR FUNCTIONS
  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Generic function for obtaining the square root of a color.
   *
   * \tparam T1 The type of the input color.
   * \tparam T2 The type of the output color.
   *
   * \param v A color vector.
   *
   * \return The square root of a color.
   *
   * Observation: it has no pratical purpose, it is supposed only to define the structure to be followed by generalizations.
   */
  template <typename T1, typename T2>
  inline T2 sqrt(const T1& v)
  {

  }

  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Function for obtaining the square root of a Pixel8 color.
   *
   * \param v A Pixel8 color vector.
   *
   * \return The square root of a Pixel8 color.
   */
  template <>
  inline double sqrt(const Pixel8& v)
  {
    return std::sqrt(v);
  }

  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Function for obtaining the square root of a RGB color.
   *
   * \tparam T The type of the RGB color tuples.
   *
   * \param v A RGB color vector.
   *
   * \return The square root of a RGB color.
   */
  template <typename T>
  inline RGB<double> sqrt(const RGB<T>& v)
  {
    return RGB<double>(std::sqrt(v.red),std::sqrt(v.green),std::sqrt(v.blue));
  }

  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Generic function for obtaining the absolute value of a color.
   *
   * \tparam T1 The type of the input color.
   * \tparam T2 The type of the output color.
   *
   * \param v A color vector.
   *
   * \return The absolute value of a color.
   *
   * Observation: it has no pratical purpose, it is supposed only to define the structure to be followed by generalizations.
   */
  template <typename T1, typename T2>
  inline T2 abs(const T1& v)
  {

  }

  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Function for obtaining the absolute value of a Pixel8 color.
   *
   * \param v A Pixel8 color vector.
   *
   * \return The absolute value of a Pixel8 color.
   */
  template <>
  inline double abs(const Pixel8& v)
  {
    return fabs(v);
  }

  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Function for obtaining the absolute value of a RGB color.
   *
   * \tparam T The type of the RGB color tuples
   *
   * \param v A RGB color vector.
   *
   * \return The absolute value of a RGB color.
   */
  template <typename T>
  inline RGB<double> abs(const RGB<T>& v)
  {
    return RGB<double>(fabs(v.red),fabs(v.green),fabs(v.blue));
  }

  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Generic function for obtaining the a color raised by a value.
   *
   * \tparam T1 The type of the input color.
   * \tparam T2 The type of the output color.
   *
   * \param v A color vector.
   * \param p Value to raise the color with.
   *
   * \return The color raised by a value.
   *
   * Observation: it has no pratical purpose, it is supposed only to define the structure to be followed by generalizations.
   */
  //template <typename T>
  template <typename T1, typename T2>
  inline T2 pow(const T1& v, const double p)
  {
  
  }

  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Generic function for obtaining the a Pixel8 color raised by a value.
   *
   * \param v A Pixel8 color vector.
   * \param p Value to raise the color with.
   *
   * \return The Pixel8 color raised by a value.
   */
  template <>
  inline double pow(const Pixel8& v, const double p)
  {
    return std::pow(v,p);
  }

  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Generic function for obtaining the a RGB color raised by a value.
   *
   * \tparam T The type of the RGB color tuples.
   *
   * \param v A RGB color vector.
   * \param p Value to raise the color with.
   *
   * \return The RGB color raised by a value.
   */
  template <typename T>
  inline RGB<T> pow(const RGB<T>& v, const double p)
  {
    return RGB<T>(std::pow(v.red,p),std::pow(v.green,p),std::pow(v.blue,p));
  }

  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Generic function for obtaining the zero value of a colorspace.
   *
   * \tparam T The type of the colorspace.
   *
   * \return The zero value of the colorspace.
   *
   * Observation: it has no pratical purpose, it is supposed only to define the structure to be followed by generalizations.
   */
  template <typename T>
  inline T zero()
  {
  
  }

  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Function for obtaining the zero value of Pixel8 colorspace.
   *
   * \return The zero value of Pixel8 colorspace.
   */
  template <>
  inline Pixel8 zero()
  {
    return 0;
  }

  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Function for obtaining the zero value of RGB<Color8> colorspace.
   *
   * \return The zero value of RGB<Color8> colorspace.
   */
  template <>
  inline RGB<Color8> zero()
  {
    return RGB<Color8>(0,0,0);
  }

  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Function for obtaining the zero value of RGB<double> colorspace.
   *
   * \return The zero value of RGB<double> colorspace.
   */
  template <>
  inline RGB<double> zero()
  {
    return RGB<double>(0.,0.,0.);
  }

  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Generic function for obtaining the identity value of a colorspace.
   *
   * \tparam T The type of the colorspace.
   *
   * \return The identity value of the colorspace.
   *
   * Observation: it has no pratical purpose, it is supposed only to define the structure to be followed by generalizations.
   */
  template <typename T>
  inline T identity()
  {

  }

  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Function for obtaining the identity value of Pixel8 colorspace.
   *
   * \return The identity value of Pixel8 colorspace.
   */
  template <>
  inline Pixel8 identity()
  {
    return 1;
  }

  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Function for obtaining the identity value of RGB<Color8> colorspace.
   *
   * \return The identity value of RGB<Color8> colorspace.
   */
  template <>
  inline RGB<Color8> identity()
  {
    return RGB<Color8>(1,1,1);
  }

  //----------------------------------------------------------------------------------------------------------------------------------//

  /**
   * \brief Function for obtaining the identity value of RGB<double> colorspace.
   *
   * \return The identity value of RGB<double> colorspace.
   */
  template <>
  inline RGB<double> identity()
  {
    return RGB<double>(1.,1.,1.);
  }

  //----------------------------------------------------------------------------------------------------------------------------------//

};

#endif
