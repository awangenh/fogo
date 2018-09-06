// --------------- TODOs -------------- //

//TODO: permitir parâmetros opcionais.

// --------------- INCLUDE's -------------- //

//stdlib sutff
#include <algorithm>
#include <math.h>

//my stuff
#include <Color/ColorspaceUtil.hpp>

// ----------- namespace UTILS ------------ //

bool Utils::RGBtoHSI(const unsigned char& R, const unsigned char& G, const unsigned char& B,
                                 double* H, double* S, double* I)
{
  double _R = R / 255.0;
  double _G = G / 255.0;
  double _B = B / 255.0;

  double minRGB = std::min(_R, std::min(_G, _B));
  double maxRGB = std::max(_R, std::max(_G, _B));

  //calculates I component from HSI
  *I = (minRGB + maxRGB) / 2.0;

  //calculates S component from HSI
  if (maxRGB == minRGB)
  {
    *S = 0;
  }
  else
  if (*I > 0.5)
  {
    *S = (maxRGB - minRGB) / (2 - (maxRGB + minRGB));
  }
  else
  {
    *S = (maxRGB - minRGB) / (maxRGB + minRGB);
  }

  //calculates H component from HSI
  if (maxRGB == minRGB)
  {
    *H = 0.0; //undefined
  }
  else
  if (maxRGB == _R && _G >= _B)
  {
    *H = 60.0 * ((_G - _B) / (maxRGB - minRGB));
  }
  else
  if (maxRGB == _R && _G < _B)
  {
    *H = 360.0 + (60.0 * ((_G - _B) / (maxRGB - minRGB)));
  }
  else
  if (maxRGB == _G)
  {
    *H = 120.0 + (60.0 * ((_B - _R) / (maxRGB - minRGB)));
  }
  else
  {
    *H = 240.0 + (60.0 * ((_R - _G) / (maxRGB - minRGB)));
  }
  *H /= 360.0;

  return true; 
}

// ---------------------------------------- //

bool Utils::HSItoRGB(const double& H, const double& S, const double& I,
                                 unsigned char* R, unsigned char* G, unsigned char* B)
{
  unsigned char *rgb_value = new unsigned char[3];

  double temp2 = (I < 0.5 ) ? (I * (1+S)) : (I + S - (I * S));
  double temp1 = (2 * I) - temp2;

  double temp3_C[3];
  temp3_C[0] = H + (1.0/3);
  temp3_C[1] = H;
  temp3_C[2] = H - (1.0/3);
  for (int i=0; i<3; i++) if (temp3_C[i] < 0) temp3_C[i] += 1;
  for (int i=0; i<3; i++) if (temp3_C[i] > 1) temp3_C[i] -= 1;

  for (int i=0; i<3; i++)
  {
    double aux = 0;
    if (temp3_C[i] < (1.0/6))
    {
      aux = temp1 + ((temp2 - temp1) * 6 * temp3_C[i]);
    }
    else
    if (temp3_C[i] < (1.0/2))
    {
      aux = temp2;
    }
    else
    if (temp3_C[i] < (2.0/3))
    {
      aux = temp1 + ((temp2 - temp1) * ((2.0/3) - temp3_C[i]) * 6);
    }
    else
    {
      aux = temp1;
    }
    rgb_value[i] = round(aux * 255); //i = (0 = RED) ; (1 = GREEN) ; (2 = BLUE)
  }
  
  *R = rgb_value[0];
  *G = rgb_value[1];
  *B = rgb_value[2];
  
  return true;
}

// ---------------------------------------- //

double* Utils::RBGImageToHSLImage(const unsigned char* original,
                   const int width,
                   const int height )
{
  double* result = new double[width * height * 3];
  
  int offsetRGB = 0;
  int offsetHSI = 0;
  for (int r=0; r<height; r++)
  {    
    for (int c=0; c<width; c++)
    {
      const unsigned char R = original[offsetRGB++];
      const unsigned char G = original[offsetRGB++];
      const unsigned char B = original[offsetRGB++];
      
      double H = 0, S = 0, I = 0;
      RGBtoHSI(R,G,B,&H,&S,&I);
      
      result[offsetHSI++] = H;
      result[offsetHSI++] = S;
      result[offsetHSI++] = I;
    } //end for    
  } //end for
  
  return result;
}
                     
// ---------------------------------------- //
