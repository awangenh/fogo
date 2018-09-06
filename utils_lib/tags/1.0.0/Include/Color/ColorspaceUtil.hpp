#ifndef COLORSPACEUTIL_HPP
#define COLORSPACEUTIL_HPP

// ----------- namespace UTILS ------------ //

namespace Utils
{
  bool RGBtoHSI(const unsigned char& R, const unsigned char& G, const unsigned char& B,
                double* H, double* S, double* I);
  bool HSItoRGB(const double& H, const double& S, const double& I,
                unsigned char* R, unsigned char* G, unsigned char* B);
  double* RBGImageToHSLImage(const unsigned char* original,
                             const int width,
                             const int height );
};

// ---------------------------------------- //

#endif /*COLORSPACEUTIL_HPP*/
