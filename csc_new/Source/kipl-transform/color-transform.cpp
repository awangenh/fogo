#ifndef _kipl_color_transform_
#define _kipl_color_transform_

// Include-Files
#include <kipl-base/error.h>
#include <kipl-base/types.h>
#include <kipl-base/color.h>
#include <kipl-base/image.h>
#include <kipl-base/iterators.h>
#include <kipl-transform/color-transform.h>
#include <cmath>

namespace kipl {
	// Lookup-Table
	static bool lutRGB2HSV_init = false;
	static bool lutHSV2RGB_init = false;
	static RGB lutHS2RGB[360][256];
	static HSV lutRGB2HSV[64][64][64];
	static UINT8 lutVal[256][256];

	inline void createLUT_RGB2HSV()
	{
		if (!lutRGB2HSV_init) {
			for (int r=0; r<64; r++) 
			{
				for (int g=0; g<64; g++) 
				{
					for (int b=0; b<64; b++) 
					{
						const RGB rgb = {r<<2, g<<2, b<<2};
						convertRGBToHSV(rgb,lutRGB2HSV[r][g][b],false);
					}
				}
			}
			lutRGB2HSV_init=true;
		}
	}


	void createLUT_HSV2RGB()
	{
		if (!lutHSV2RGB_init)
		{
			int i,j;

			for (i=0; i<360; i++) {
				for (j=0; j<256; j++)
				{
					HSV hsv = {i,j,255};
					const double norm = 1.0/255.0;
					double r,g,b;
					convertHSVToRGB((double)hsv.hue,(double)hsv.sat*norm,(double)hsv.val*norm,r,g,b);
					lutHS2RGB[i][j].red=(GRAY)(255.0*r);
					lutHS2RGB[i][j].green=(GRAY)(255.0*g);
					lutHS2RGB[i][j].blue=(GRAY)(255.0*b);
				}
			}
			for (i=0; i<256; i++)
				for (j=0; j<256; j++)
					lutVal[i][j]=(i*j)/255;
			lutHSV2RGB_init=true;
		}
	}



	void convertRGBToHSV(double r, double g, double b, double& h, double& s, double& v)
	{
		double delta,min,max;

		if (r<=g)
		{
			min=r;
			max=g;
		}
		else
		{
			min=g;
			max=r;
		}
		if (b<min) min=b;
		if (b>max) max=b;

		v=max;
		delta=max-min;

		if (max!=0.0) s=delta/max;
		else s=0.0;

		if (s==0.0) h=0.0;
		else
		{
			if (r==max) h=(g-b)/delta;
			else if (g==max) h=2.0+(b-r)/delta;
			else if (b==max) h=4.0+(r-g)/delta;
			h=h*60.0;
			if (h<0) h=h+360.0;
		}
	}


	void convertHSVToRGB(double h, double s, double v, double& r, double& g, double& b)
	{
		double f,p,q,t;
		int i;

		if (s==0.0)
		{
			r=v;
			g=v;
			b=v;
		}
		else
		{
			if (h<0.0) h=360.0+h;
			h=h/60.0;
			i=(int)floor(h);
			f=h-i;
			p=v*(1.0-s);
			q=v*(1.0-(s*f));
			t=v*(1.0-(s*(1.0-f)));
			switch (i)
			{
			case 0: r=v; g=t; b=p; break;
			case 1: r=q; g=v; b=p; break;
			case 2: r=p; g=v; b=t; break;
			case 3: r=p; g=q; b=v; break;
			case 4: r=t; g=p; b=v; break;
			case 5: r=v; g=p; b=q; break;
			case 6: r=v; g=t; b=p; break;
			}
		}
	}

	void convertRGBToHSV(const RGB& rgb, HSV& hsv, bool fast)
	{
		if (fast) {
			createLUT_RGB2HSV();
			hsv=lutRGB2HSV[rgb.red>>2][rgb.green>>2][rgb.blue>>2];
		}
		else {
			double h,s,v;
			convertRGBToHSV((double)rgb.red/255.0,(double)rgb.green/255.0,(double)rgb.blue/255.0,h,s,v);
			hsv.hue=(GRAY16)(h+0.5);
			if (hsv.hue>359) hsv.hue=0;
			hsv.sat=(UINT8)(s*255.0);
			hsv.val=(UINT8)(v*255.0);
		}
	}


	void convertRGBToHSV(const RGB16& rgb, HSV16& hsv)
	{
		double h,s,v;
		convertRGBToHSV((double)rgb.red/65535.0,(double)rgb.green/65535.0,(double)rgb.blue/65535.0,h,s,v);
		h=floor(hsv.hue+0.5);
		if (h>359.0) hsv.hue=(UINT16)359.0;
		else hsv.hue=(UINT8)h;
		hsv.sat=(UINT8)(s*65535.0+0.5);
		hsv.val=(UINT8)(v*65535.0+0.5);
	}

	void convertHSVToRGB(const HSV& hsv, RGB& rgb)
	{
		createLUT_HSV2RGB();
		rgb=lutHS2RGB[hsv.hue][hsv.sat];
		rgb.red=lutVal[rgb.red][hsv.val];
		rgb.green=lutVal[rgb.green][hsv.val];
		rgb.blue=lutVal[rgb.blue][hsv.val];
	}


	void convertHSVToRGB(const HSV16& hsv, RGB16& rgb)
	{
		double r,g,b;
		const double norm = 1.0/65535.0;
		convertHSVToRGB((double)hsv.hue,(double)hsv.sat*norm,(double)hsv.val*norm,r,g,b);
		rgb.red=(GRAY16)(65535.0*r);
		rgb.green=(GRAY16)(65535.0*g);
		rgb.blue=(GRAY16)(65535.0*b);
	}


	void toHSV(Image& src, Image& trg, bool fast) {
		const ROI& roi = src.getROI();
		if (roi.isValid()) {
			if (src.getImageType()==Image::pfRGB) {
				PixelIterator<RGB> src_it(src);
				trg.create(roi,Image::pfHSV);
				PixelIterator<HSV> trg_it(trg);
				do {
					convertRGBToHSV(**src_it,**trg_it,fast);
					++trg_it;
				} while (++src_it);
			}
			if (src.getImageType()==Image::pfRGB16) {
				PixelIterator<RGB16> src_it(src);
				trg.create(roi,Image::pfHSV16);
				PixelIterator<HSV16> trg_it(trg);
				do {
					convertRGBToHSV(**src_it,**trg_it);
					++trg_it;
				} while (++src_it);
			}
		}
		else throw EInvalidArgument();
	}


	void toRGB(Image& src, Image& trg) {
		const ROI& roi = src.getROI();
		if (roi.isValid()) {
			if (src.getImageType()==Image::pfHSV) {
				PixelIterator<HSV> src_it(src);
				trg.create(roi,Image::pfRGB);
				PixelIterator<RGB> trg_it(trg);
				do {
					convertHSVToRGB(**src_it,**trg_it);
					++trg_it;
				} while (++src_it);
			}
			if (src.getImageType()==Image::pfHSV16) {
				PixelIterator<HSV16> src_it(src);
				trg.create(roi,Image::pfHSV16);
				PixelIterator<RGB16> trg_it(trg);
				do {
					convertHSVToRGB(**src_it,**trg_it);
					++trg_it;
				} while (++src_it);
			}
		}
		else throw EInvalidArgument();
	}


	void toHSV(Image& img, bool fast) {
		Image hsv;
		toHSV(img,hsv,fast);
		img.stealImageData(&hsv);
	}


	void toRGB(Image& img) {
		Image rgb;
		toRGB(img,rgb);
		img.stealImageData(&rgb);
	}



} // end of namespace

#endif
