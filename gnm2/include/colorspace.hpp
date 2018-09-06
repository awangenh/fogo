/*
 * Refer to http://www.easyrgb.com/math.php for more information about the formulas.
 * 
 * To find a conversion function search for <colorspace1>to<colorspace2>. Example:
 * rgbtohsv.
 */

#ifndef COLORSPACE_HPP
#define COLORSPACE_HPP

//---------------------------------------------------------------------------------//

#include <cmath>
#include <algorithm>

//---------------------------------------------------------------------------------//

/*
 * Types and definitions
 */

typedef unsigned char pixelvalue;

struct illuminant {
  illuminant(double _x,double _y,double _z) {x=_x; y=_y; z=_z;};

  double x, y, z;
};

namespace observer2 {
  const illuminant A(109.850,100,35.585);
  const illuminant C(98.074,100,118.232);
  const illuminant D50(96.422,100,82.521);
  const illuminant D55(95.682,100,92.149);
  const illuminant D65(95.047,100,108.883);
  const illuminant D75(94.972,100,122.638);
  const illuminant F2(99.187,100,67.395);
  const illuminant F7(95.044,100,108.755);
  const illuminant F11(100.966,100,64.370);
};

namespace observer10 {
  const illuminant A(111.144,100,35.200);
  const illuminant C(97.285,100,116.145);
  const illuminant D50(96.720,100,81.427);
  const illuminant D55(95.799,100,90.926);
  const illuminant D65(94.811,100,107.304);
  const illuminant D75(94.416,100,120.641);
  const illuminant F2(103.280,100,69.026);
  const illuminant F7(95.792,100,107.687);
  const illuminant F11(103.866,100,65.627);
};

//---------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------//

namespace colorspace {  
  
  // XYZ
  inline int rgbtoxyz(const pixelvalue r, const pixelvalue g, const pixelvalue b,
	       double& x, double& y, double& z);
  inline int xyztorgb(const double x, const double y, const double z,
	       pixelvalue& r, pixelvalue& g, pixelvalue& b);
  // HSL
  inline int rgbtohsl(const pixelvalue r, const pixelvalue g, const pixelvalue b,
	       double& h, double& s, double& l);
  inline int hsltorgb(const double h, const double s, const double l,
	       pixelvalue& r, pixelvalue& g, pixelvalue& b);
  // HSV
  inline int rgbtohsv(const pixelvalue r, const pixelvalue g, const pixelvalue b,
	       double& h, double& s, double& v);
  inline int hsvtorgb(const double h, const double s, const double v,
	       pixelvalue& r, pixelvalue& g, pixelvalue& b);
  // CIE L*AB
  inline int xyztolab(const double x, const double y, const double z,
	       double& l, double& a, double& b);
  inline int labtoxyz(const double l, const double a, const double b,
	       double& x, double& y, double& z);
  inline int rgbtolab(const pixelvalue r, const pixelvalue g, const pixelvalue b,
	       double& l, double& a, double& _b);
  inline int labtorgb(const double l, const double a, const double _b,
	       pixelvalue& r, pixelvalue& g, pixelvalue& b);
  // CIE L*CH
  inline int labtolch(const double _l, const double a, const double b,
	       double& l, double& c, double& h);
  inline int lchtolab(const double l, const double c, const double h,
	       double& _l, double& a, double& b);
  inline int rgbtolch(const pixelvalue r, const pixelvalue g, const pixelvalue b,
	       double& l, double& c, double& h);
  inline int lchtorgb(const double l, const double c, const double h,
	       pixelvalue& r, pixelvalue& g, pixelvalue& b);
  // CIE L*UV
  inline int xyztoluv(const double x, const double y, const double z,
	       double& l, double& u, double& v);
  inline int luvtoxyz(const double l, const double u, const double v,
	       double& x, double& y, double& z);
  inline int rgbtoluv(const pixelvalue r, const pixelvalue g, const pixelvalue b,
	       double& l, double& u, double& v);
  inline int luvtorgb(const double l, const double u, const double v,
	       pixelvalue& r, pixelvalue& g, pixelvalue& b);
  
};
  

//---------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------//

int colorspace::rgbtoxyz(const pixelvalue r, const pixelvalue g, const pixelvalue b,
			 double& x, double& y, double& z)
{
  double var_R = r/255.0;        //Where R = 0 ÷ 255
  double var_G = g/255.0;        //Where G = 0 ÷ 255
  double var_B = b/255.0;        //Where B = 0 ÷ 255
    
  if ( var_R > 0.04045 ) 
    var_R = pow( ( var_R+0.055 )/1.055, 2.4);
  else      
    var_R = var_R/12.92;

  if ( var_G > 0.04045 ) 
    var_G = pow( ( var_G+0.055 )/1.055, 2.4);
  else
    var_G = var_G/12.92;

  if ( var_B > 0.04045 ) 
    var_B = pow( ( var_B + 0.055 )/1.055, 2.4);
  else
    var_B = var_B/12.92;

  var_R = var_R * 100;
  var_G = var_G * 100;
  var_B = var_B * 100;

  //Observer. = 2°, Illuminant = D65
  x = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
  y = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
  z = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;

  return 0;
}

//---------------------------------------------------------------------------------//

int colorspace::xyztorgb(const double x, const double y, const double z,
			 pixelvalue& r, pixelvalue& g, pixelvalue& b)
{
                         //Observer = 2°, Illuminant = D65
  double var_X = x/100.0;  //Where X = 0 ÷  95.047
  double var_Y = y/100.0;  //Where Y = 0 ÷ 100.000
  double var_Z = z/100.0;  //Where Z = 0 ÷ 108.883

  double var_R = var_X *  3.2406 + var_Y * -1.5372 + var_Z * -0.4986;
  double var_G = var_X * -0.9689 + var_Y *  1.8758 + var_Z *  0.0415;
  double var_B = var_X *  0.0557 + var_Y * -0.2040 + var_Z *  1.0570;

  if ( var_R > 0.0031308 ) 
    var_R = 1.055 * pow( var_R,1.0/2.4) - 0.055;
  else
    var_R = 12.92 * var_R;

  if ( var_G > 0.0031308 ) 
    var_G = 1.055 * pow( var_G,1.0/2.4) - 0.055;
  else                     
    var_G = 12.92 * var_G;
  
  if ( var_B > 0.0031308 ) 
    var_B = 1.055 * pow( var_B,1.0/2.4) - 0.055;
  else                     
    var_B = 12.92 * var_B;

  r = round(var_R * 255.0);
  g = round(var_G * 255.0);
  b = round(var_B * 255.0);

  return 0;
}

//---------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------//

int colorspace::rgbtohsl(const pixelvalue r, const pixelvalue g, const pixelvalue b,
			 double& h, double& s, double& l)
{
  double var_R = ( r / 255.0 );                     //Where RGB values = 0 ÷ 255
  double var_G = ( g / 255.0 );
  double var_B = ( b / 255.0 );

  double var_Min = std::min( var_R, std::min(var_G, var_B) );    //Min. value of RGB
  double var_Max = std::max( var_R, std::max(var_G, var_B) );    //Max. value of RGB
  double del_Max = var_Max - var_Min;                            //Delta RGB value

  l = ( var_Max + var_Min ) / 2.0;

  if ( del_Max == 0 ) {                 //This is a gray, no chroma...    
    h = 0;                              //HSL results = 0 ÷ 1
    s = 0;
  } else {                              //Chromatic data...
    if ( l < 0.5 ) 
      s = del_Max / ( var_Max + var_Min );
    else           
      s = del_Max / ( 2.0 - var_Max - var_Min );

    double del_R = ( ( ( var_Max - var_R ) / 6.0 ) + ( del_Max / 2.0 ) ) / del_Max;
    double del_G = ( ( ( var_Max - var_G ) / 6.0 ) + ( del_Max / 2.0 ) ) / del_Max;
    double del_B = ( ( ( var_Max - var_B ) / 6.0 ) + ( del_Max / 2.0 ) ) / del_Max;

    if ( var_R == var_Max ) 
      h = del_B - del_G;
    else if ( var_G == var_Max ) 
      h = ( 1.0 / 3.0 ) + del_R - del_B;
    else if ( var_B == var_Max ) 
      h = ( 2.0 / 3.0 ) + del_G - del_R;

    if ( h < 0 ) h += 1;
    if ( h > 1 ) h -= 1;
  } //end if

  return 0;
}

//---------------------------------------------------------------------------------//

inline double huetorgb(double v1, double v2, double vH )
{
  if ( vH < 0 ) vH += 1;
  if ( vH > 1 ) vH -= 1;

  if ( ( 6 * vH ) < 1 ) 
    return v1 + ( v2 - v1 ) * 6 * vH;
  if ( ( 2 * vH ) < 1 ) 
    return v2;
  if ( ( 3 * vH ) < 2 ) 
    return v1 + ( v2 - v1 ) * ( ( 2.0 / 3.0 ) - vH ) * 6;

  return v1;
}

int colorspace::hsltorgb(const double h, const double s, const double l,
			 pixelvalue& r, pixelvalue& g, pixelvalue& b)
{
  if ( s == 0 ) {                    //HSL values = 0 ÷ 1
    r = l * 255.0;                      //RGB results = 0 ÷ 255
    g = l * 255.0;
    b = l * 255.0;
  } else {
    double var_2;
    if ( l < 0.5 ) 
      var_2 = l * ( 1 + s );
    else
      var_2 = ( l + s ) - ( s * l );
    
    double var_1 = 2.0 * l - var_2;

    r = round(255.0 * huetorgb( var_1, var_2, h + ( 1.0 / 3.0 ) ));
    g = round(255.0 * huetorgb( var_1, var_2, h ));
    b = round(255.0 * huetorgb( var_1, var_2, h - ( 1.0 / 3.0 ) ));
  } //end if

  return 0;
}

//---------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------//

int colorspace::rgbtohsv(const pixelvalue r, const pixelvalue g, const pixelvalue b,
			 double& h, double& s, double& v)
{
  double var_R = r / 255.0;                     //RGB values = 0 ÷ 255
  double var_G = g / 255.0;
  double var_B = b / 255.0;

  double var_Min = std::min( var_R, std::min(var_G, var_B) );    //Min. value of RGB
  double var_Max = std::max( var_R, std::max(var_G, var_B) );    //Max. value of RGB
  double del_Max = var_Max - var_Min;             //Delta RGB value

  v = var_Max;

  if ( del_Max == 0 ) {                   //This is a gray, no chroma...
    h = 0;                                //HSV results = 0 ÷ 1
    s = 0;
  } else {                                //Chromatic data...
    s = del_Max / var_Max;

    double del_R = ( ( ( var_Max - var_R ) / 6.0 ) + ( del_Max / 2.0 ) ) / del_Max;
    double del_G = ( ( ( var_Max - var_G ) / 6.0 ) + ( del_Max / 2.0 ) ) / del_Max;
    double del_B = ( ( ( var_Max - var_B ) / 6.0 ) + ( del_Max / 2.0 ) ) / del_Max;

    if ( var_R == var_Max ) 
      h = del_B - del_G;
    else if ( var_G == var_Max ) 
      h = ( 1.0 / 3.0 ) + del_R - del_B;
    else if ( var_B == var_Max ) 
      h = ( 2.0 / 3.0 ) + del_G - del_R;

   if ( h < 0 ) 
     h += 1;
   if ( h > 1 ) 
     h -= 1;
  } //end if

  return 0;
}

//---------------------------------------------------------------------------------//

int colorspace::hsvtorgb(const double h, const double s, const double v,
			 pixelvalue& r, pixelvalue& g, pixelvalue& b)
{
  if ( s == 0 ) {                      //HSV values = 0 ÷ 1
    r = round(v * 255.0);
    g = round(v * 255.0);
    b = round(v * 255.0);
  } else {
    double var_h = h * 6;
    if ( var_h == 6 ) 
      var_h = 0;      //H must be < 1

    int var_i = int( var_h );             //Or ... var_i = floor( var_h )
    double var_1 = v * ( 1.0 - s );
    double var_2 = v * ( 1.0 - s * ( var_h - var_i ) );
    double var_3 = v * ( 1.0 - s * ( 1.0 - ( var_h - var_i ) ) );

    double var_r, var_g, var_b;
    if ( var_i == 0 ) 
      { var_r = v; var_g = var_3; var_b = var_1; }
    else if ( var_i == 1 ) 
      { var_r = var_2; var_g = v; var_b = var_1; }
    else if ( var_i == 2 ) 
      { var_r = var_1; var_g = v; var_b = var_3; }
    else if ( var_i == 3 ) 
      { var_r = var_1; var_g = var_2; var_b = v; }
    else if ( var_i == 4 ) 
      { var_r = var_3; var_g = var_1; var_b = v; }
    else
      { var_r = v; var_g = var_1; var_b = var_2; }

    r = round(var_r * 255);                  //RGB results = 0 ÷ 255
    g = round(var_g * 255);
    b = round(var_b * 255);
  } //end if

  return 0;
}

//---------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------//

int colorspace::xyztolab(const double x, const double y, const double z,
			 double& l, double& a, double& b)
{
  double var_X = x / observer2::D65.x;          //ref_X =  95.047  Observer= 2°, Illuminant= D65
  double var_Y = y / observer2::D65.y;          //ref_Y = 100.000
  double var_Z = z / observer2::D65.z;          //ref_Z = 108.883

  if ( var_X > 0.008856 ) 
    var_X = pow(var_X, 1.0/3.0 );
  else
    var_X = ( 7.787 * var_X ) + ( 16.0 / 116.0 );

  if ( var_Y > 0.008856 ) 
    var_Y = pow(var_Y, 1.0/3.0 );
  else            
    var_Y = ( 7.787 * var_Y ) + ( 16.0 / 116.0 );

  if ( var_Z > 0.008856 ) 
    var_Z = pow(var_Z, 1.0/3.0 );
  else
    var_Z = ( 7.787 * var_Z ) + ( 16.0 / 116.0 );

  l = ( 116.0 * var_Y ) - 16.0;
  a = 500.0 * ( var_X - var_Y );
  b = 200.0 * ( var_Y - var_Z );

  return 0;
}

//---------------------------------------------------------------------------------//
 
int colorspace::labtoxyz(const double l, const double a, const double b,
			 double& x, double& y, double& z)
{
  double var_Y = ( l + 16.0 ) / 116.0;
  double var_X = a / 500.0 + var_Y;
  double var_Z = var_Y - b / 200.0;

  double aux_Y = pow(var_Y,3);
  if ( aux_Y  > 0.008856 ) 
    var_Y = aux_Y;
  else              
    var_Y = ( var_Y - 16.0 / 116.0 ) / 7.787;

  double aux_X = pow(var_X,3);
  if ( aux_X > 0.008856 ) 
    var_X = aux_X;
  else                     
    var_X = ( var_X - 16.0 / 116.0 ) / 7.787;

  double aux_Z = pow(var_Z,3);
  if ( aux_Z > 0.008856 ) 
    var_Z = aux_Z;
  else
    var_Z = ( var_Z - 16.0 / 116.0 ) / 7.787;

  x = observer2::D65.x * var_X;     //ref_X =  95.047  Observer= 2°, Illuminant= D65
  y = observer2::D65.y * var_Y;     //ref_Y = 100.000
  z = observer2::D65.z * var_Z;     //ref_Z = 108.883

  return 0;
}

//---------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------//

int colorspace::labtolch(const double _l, const double a, const double b,
			 double& l, double& c, double& h)
{
  const double PI = 3.1415926535897932384626433832795;

  double var_H = atan2(b,a);  //Quadrant by signs

  if ( var_H > 0 ) 
    var_H = ( var_H / PI ) * 180.0;
  else
    var_H = 360.0 - ( fabs( var_H ) / PI ) * 180.0;

  l = _l;
  c = sqrt(pow(a,2) + pow(b,2));
  h = var_H;

  return 0;
}

//---------------------------------------------------------------------------------//

int colorspace::lchtolab(const double l, const double c, const double h,
			 double& _l, double& a, double& b)
{
  const double PI = 3.1415926535897932384626433832795;
  
  //Where CIE-H° = 0 ÷ 360°
  double rad = h*PI/180.0;
    
  _l = l;
  a = cos( rad ) * c;
  b = sin( rad ) * c;
}

//---------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------//

int colorspace::xyztoluv(const double x, const double y, const double z,
			 double& l, double& u, double& v)
{
  double var_U = ( 4.0 * x ) / ( x + ( 15.0 * y ) + ( 3.0 * z ) );
  double var_V = ( 9.0 * y ) / ( x + ( 15.0 * y ) + ( 3.0 * z ) );

  double var_Y = y / 100.0;
  if ( var_Y > 0.008856 ) 
    var_Y = pow(var_Y, 1.0/3.0 );
  else
    var_Y = ( 7.787 * var_Y ) + ( 16.0 / 116.0 );

  double ref_X = observer2::D65.x;        //Observer= 2°, Illuminant= D65
  double ref_Y = observer2::D65.y;
  double ref_Z = observer2::D65.z;

  double ref_U = ( 4.0 * ref_X ) / ( ref_X + ( 15.0 * ref_Y ) + ( 3.0 * ref_Z ) );
  double ref_V = ( 9.0 * ref_Y ) / ( ref_X + ( 15.0 * ref_Y ) + ( 3.0 * ref_Z ) );

  l = ( 116.0 * var_Y ) - 16.0;
  u = 13.0 * l * ( var_U - ref_U );
  v = 13.0 * l * ( var_V - ref_V );

  return 0;
}

//---------------------------------------------------------------------------------//
 
int colorspace::luvtoxyz(const double l, const double u, const double v,
			 double& x, double& y, double& z)
{
  double var_Y = ( l + 16.0 ) / 116.0;
  if ( pow(var_Y,3) > 0.008856 ) 
    var_Y = pow(var_Y,3);
  else
    var_Y = ( var_Y - 16.0 / 116.0 ) / 7.787;

  double ref_X = observer2::D65.x;        //Observer= 2°, Illuminant= D65
  double ref_Y = observer2::D65.y;
  double ref_Z = observer2::D65.z;

  double ref_U = ( 4.0 * ref_X ) / ( ref_X + ( 15.0 * ref_Y ) + ( 3.0 * ref_Z ) );
  double ref_V = ( 9.0 * ref_Y ) / ( ref_X + ( 15.0 * ref_Y ) + ( 3.0 * ref_Z ) );

  double var_U = u / ( 13.0 * l ) + ref_U;
  double var_V = v / ( 13.0 * l ) + ref_V;

  y = var_Y * 100.0;
  x = - ( 9.0 * y * var_U ) / ( ( var_U - 4.0 ) * var_V  - var_U * var_V );
  z = ( 9.0 * y - ( 15.0 * var_V * y ) - ( var_V * x ) ) / ( 3.0 * var_V );

  return 0;
}

//---------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------//

int colorspace::rgbtolab(const pixelvalue r, const pixelvalue g, const pixelvalue b,
			 double& l, double& a, double& _b)
{
  double x, y, z;
  if (!rgbtoxyz(r,g,b,x,y,z)) return xyztolab(x,y,z,l,a,_b);
  
  return 1;
}

//---------------------------------------------------------------------------------//

int colorspace::labtorgb(const double l, const double a, const double _b,
			 pixelvalue& r, pixelvalue& g, pixelvalue& b)
{
  double x, y, z;
  if (!labtoxyz(l,a,_b,x,y,z)) return xyztorgb(x,y,z,r,g,b);
  
  return 1;
}

//---------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------//

int colorspace::rgbtoluv(const pixelvalue r, const pixelvalue g, const pixelvalue b,
			 double& l, double& u, double& v)
{
  double x, y, z;
  if (!rgbtoxyz(r,g,b,x,y,z)) return xyztoluv(x,y,z,l,u,v);
  
  return 1;
}

//---------------------------------------------------------------------------------//

int colorspace::luvtorgb(const double l, const double u, const double v,
			 pixelvalue& r, pixelvalue& g, pixelvalue& b)
{
  double x, y, z;
  if (!luvtoxyz(l,u,v,x,y,z)) return xyztorgb(x,y,z,r,g,b);
  
  return 1;
}

//---------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------//

int colorspace::rgbtolch(const pixelvalue r, const pixelvalue g, const pixelvalue b,
			 double& l, double& c, double& h)
{
  double _l, a, _b;
  if (!rgbtolab(r,g,b,_l,a,_b)) return labtolch(_l,a,_b,l,c,h);
  
  return 1;
}

//---------------------------------------------------------------------------------//

int colorspace::lchtorgb(const double l, const double c, const double h,
			 pixelvalue& r, pixelvalue& g, pixelvalue& b)
{
  double _l, a, _b;
  if (!lchtolab(l,c,h,_l,a,_b)) return labtorgb(_l,a,_b,r,g,b);
  
  return 1;
}

//---------------------------------------------------------------------------------//

#endif /* COLORSPACE_HPP */

//---------------------------------------------------------------------------------//
