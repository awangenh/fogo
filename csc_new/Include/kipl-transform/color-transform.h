#ifndef _kipl_color_transform_h_
#define _kipl_color_transform_h_

// Include-Section
// Declaration-Section
#include <kipl-base/color.h>

namespace kipl {

	void convertRGBToHSV(double r, double g, double b, double& h, double& s, double& v);
	void convertHSVToRGB(double h, double s, double v, double& r, double& g, double& b);
	void convertRGBToHSV(const RGB& rgb, HSV& hsv, bool fast);
	void convertRGBToHSV(const RGB16& rgb, HSV16& hsv);
	void convertHSVToRGB(const HSV& hsv, RGB& rgb);
	void convertHSVToRGB(const HSV16& hsv, RGB16& rgb);

	void toRGB(Image& src, Image& trg);
	void toRGB(Image& img);
	void toHSV(Image& src, Image& trg, bool fast);
	void toHSV(Image& img, bool fast);
} // end of namespace

#endif
