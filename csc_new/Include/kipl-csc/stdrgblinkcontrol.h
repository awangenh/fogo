/* 

This "SOFTWARE" is a free software.

You are allowed to download, use, modify and redistribute this software.
The software is provided "AS IS" without warranty of any kind.

Copyright: University of Koblenz, Lutz Priese, Volker Rehrman, Patrick Sturm

*/


#ifndef _STDRGBLINKCONTROL_H_
#define _STDRGBLINKCONTROL_H_

// Include-Files
#include <kipl-base/color.h>
#include <kipl-csc/codebase.h>

namespace kipl {
	namespace csc {

		// StdRGBLinkControl
		class StdRGBLinkControl
		{
		private:
			int count,redSum,greenSum,blueSum;
			int avgRed,avgGreen,avgBlue;
			int currLevel,thresholdQuad;

		public:
			inline StdRGBLinkControl();
			inline void setThreshold(int threshold);
			inline void firstFeature(const RGB& rgb, int weight);
			inline bool addFeature(const RGB& rgb, int weight);
			inline void begin(int level, const RGB& rgb);
			inline void add(const RGB& rgb);
			inline bool add(const RGB& rgb1, const RGB& rgb2, bool justTest=false);
			inline void begin(int level, const Codeelement<RGB>* ce);
			inline void add(const Codeelement<RGB>* ce);
			inline bool add(const Codeelement<RGB>* ce1, const Codeelement<RGB>* ce2, bool justTest=false);
			inline void setFeature(RGB& avgRGB);
			inline int compareFeatures(const RGB& rgbCE1, const RGB& rgbCE2, const RGB& rgbSubCEs);
			inline bool colorSimilar(const RGB& rgb1, const RGB& rgb2);
		};


		// StdRGBLinkControl
		inline StdRGBLinkControl::StdRGBLinkControl() 
		{ 
			count=0; 
			redSum=0; 
			greenSum=0; 
			blueSum=0; 
			avgRed=0;
			avgGreen=0;
			avgBlue=0;
			currLevel=0;
			thresholdQuad=400;
		}


		inline void StdRGBLinkControl::firstFeature(const RGB& rgb, int weight)
		{
			currLevel=-1;
			count=weight;
			redSum=weight*rgb.red;
			greenSum=weight*rgb.green;
			blueSum=weight*rgb.blue;
			avgRed=rgb.red;
			avgGreen=rgb.green;
			avgBlue=rgb.blue;
		}


		inline bool StdRGBLinkControl::addFeature(const RGB& rgb, int weight)
		{
			int deltaRed,deltaGreen,deltaBlue,delta;

			deltaRed = avgRed-rgb.red;
			deltaGreen = avgGreen-rgb.green;
			deltaBlue = avgBlue-rgb.blue;
			delta = deltaRed*deltaRed+deltaGreen*deltaGreen+deltaBlue*deltaBlue;

			if (delta<thresholdQuad)
			{
				redSum+=weight*rgb.red;
				greenSum+=weight*rgb.green;
				blueSum+=weight*rgb.blue;
				count+=weight;
				avgRed=redSum/count;
				avgGreen=greenSum/count;
				avgBlue=blueSum/count;

				return true;
			}
			return false;
		}

		inline void StdRGBLinkControl::setThreshold(int threshold)
		{
			thresholdQuad=threshold*threshold;
		}


		inline bool StdRGBLinkControl::colorSimilar(const RGB& rgb1, const RGB& rgb2)
		{
			int deltaRed = (int)rgb1.red-rgb2.red;
			int deltaGreen = (int)rgb1.green-rgb2.green;
			int deltaBlue = (int)rgb1.blue-rgb2.blue;
			int delta = deltaRed*deltaRed+deltaGreen*deltaGreen+deltaBlue*deltaBlue;

			return (delta<thresholdQuad);
		}


		inline void StdRGBLinkControl::begin(int level, const RGB& rgb) 
		{ 
			avgRed=redSum=rgb.red; 
			avgGreen=greenSum=rgb.green; 
			avgBlue=blueSum=rgb.blue; 
			count=1; 
			currLevel=level; 
		}


		inline void StdRGBLinkControl::add(const RGB& rgb)
		{
			redSum+=rgb.red;
			greenSum+=rgb.green;
			blueSum+=rgb.blue;
			count++;
			if (currLevel>0)
			{
				avgRed=redSum/count;
				avgGreen=greenSum/count;
				avgBlue=blueSum/count;
			}
		}


		inline bool StdRGBLinkControl::add(const RGB& rgb1, const RGB& rgb2, bool justTest)
		{
			int deltaRed,deltaGreen,deltaBlue,delta,delta2;

			deltaRed = (int)rgb1.red-rgb2.red;
			deltaGreen = (int)rgb1.green-rgb2.green;
			deltaBlue = (int)rgb1.blue-rgb2.blue;
			delta = deltaRed*deltaRed+deltaGreen*deltaGreen+deltaBlue*deltaBlue;

			if (currLevel>0)
			{
				deltaRed = avgRed-rgb2.red;
				deltaGreen = avgGreen-rgb2.green;
				deltaBlue = avgBlue-rgb2.blue;
				delta2 = deltaRed*deltaRed+deltaGreen*deltaGreen+deltaBlue*deltaBlue;
				if (delta2>delta) delta=delta2;
			}

			if (delta<thresholdQuad)
			{
				if (justTest==false)
				{
					redSum+=rgb2.red;
					greenSum+=rgb2.green;
					blueSum+=rgb2.blue;
					count++;
					if (currLevel>0)
					{
						avgRed=redSum/count;
						avgGreen=greenSum/count;
						avgBlue=blueSum/count;
					}
				}
				return true;
			}
			return false;
		}


		inline void StdRGBLinkControl::begin(int level, const Codeelement<RGB>* ce)
		{
			begin(level,ce->data);
		}


		inline void StdRGBLinkControl::add(const Codeelement<RGB>* ce)
		{
			add(ce->data);
		}


		inline bool StdRGBLinkControl::add(const Codeelement<RGB>* ce1, const Codeelement<RGB>* ce2, bool justTest)
		{
			return add(ce1->data,ce2->data,justTest);
		}


		inline void StdRGBLinkControl::setFeature(RGB& avgRGB)
		{
			if (currLevel==0)
			{
				avgRGB.red=redSum/count;
				avgRGB.green=greenSum/count;
				avgRGB.blue=blueSum/count;
			}
			else
			{
				avgRGB.red=avgRed;
				avgRGB.green=avgGreen;
				avgRGB.blue=avgBlue;
			}
		}


		inline int StdRGBLinkControl::compareFeatures(const RGB& rgbCE1, const RGB& rgbCE2, const RGB& rgbSubCE)
		{
			int deltaRed1 = (int)rgbCE1.red-(int)rgbSubCE.red;
			int deltaGreen1 = (int)rgbCE1.green-(int)rgbSubCE.green;
			int deltaBlue1 = (int)rgbCE1.blue-(int)rgbSubCE.blue;
			int delta1 = deltaRed1*deltaRed1+deltaGreen1*deltaGreen1+deltaBlue1*deltaBlue1;
			int deltaRed2 = (int)rgbCE2.red-(int)rgbSubCE.red;
			int deltaGreen2 = (int)rgbCE2.green-(int)rgbSubCE.green;
			int deltaBlue2 = (int)rgbCE2.blue-(int)rgbSubCE.blue;
			int delta2 = deltaRed2*deltaRed2+deltaGreen2*deltaGreen2+deltaBlue2*deltaBlue2;

			if (delta1<=delta2) return 1;

			return 2;
		}
	}
}

#endif
