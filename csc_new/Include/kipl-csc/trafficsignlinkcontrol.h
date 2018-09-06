/* 

This "SOFTWARE" is a free software.

You are allowed to download, use, modify and redistribute this software.
The software is provided "AS IS" without warranty of any kind.

Copyright: University of Koblenz, Lutz Priese, Volker Rehrman, Patrick Sturm

*/


#ifndef _TRAFFICSIGNLINKCONTROL_H_
#define _TRAFFICSIGNLINKCONTROL_H_

/***********************************************
iMAge pROcessing cLAsses Version 1.0

Copyright 2002, Patrick Sturm
************************************************/

// Include-Files
#include <kipl-base/color.h>
#include <kipl-csc/hsvcolsim.h>
#include <kipl-csc/trafficsigncolsim.h>
#include <kipl-csc/codebase.h>
#include <cmath>

namespace kipl {
	namespace csc {
		// TrafficSignLinkControl
		class TrafficSignLinkControl: public TrafficSignHSVSimilarityFunction
		{
		private:
			int count,hueCount,redLowCount,redHighCount,currLevel;
			int hueSum,satSum,valSum;
			HSV avgHSV;

		public:
			TrafficSignLinkControl();

			inline void firstFeature(const HSV& hsv, int weight);
			inline bool addFeature(const HSV& hsv, int weight);

			inline void begin(int level, const HSV& hsv);
			inline void add(const HSV& hsv);
			inline bool add(const HSV& hsv1, const HSV& hsv2, bool justTest=false);

			inline void begin(int level, const Codeelement<HSV>* ce);
			inline void add(const Codeelement<HSV>* ce);
			inline bool add(const Codeelement<HSV>* ce1, const Codeelement<HSV>* ce2, bool justTest=false);

			inline void setFeature(HSV& hsv);
			inline int compareFeatures(const HSV& hsvCE1, const HSV& hsvCE2, const HSV& hsvSubCE);

			inline bool colorSimilar(const HSV& hsv1, const HSV& hsv2) { return colorSimilarLink(hsv1.hue,hsv1.sat,hsv1.val,hsv2.hue,hsv2.sat,hsv2.val); }
		};


		// TrafficSignLinkControlSingleLinkageOnRGB
		class TrafficSignLinkControlSingleLinkageOnRGB: public TrafficSignHSVSimilarityFunction
		{
		private:
			int count,currLevel;
			int redSum,greenSum,blueSum;
			int avgRed,avgBlue,avgGreen;
			//int HSVDISTANCE[16][16][3];
			HSV rgbToHSV[64][64][64];
			HSV avgHSV;

		public:
			TrafficSignLinkControlSingleLinkageOnRGB();

			inline void firstFeature(const RGB& rgb, int weight);
			inline bool addFeature(const RGB& rgb, int weight);

			inline void begin(int level, const RGB& rgb);
			inline void add(const RGB& rgb);
			inline bool add(const RGB& rgb1, const RGB& rgb2, bool justTest=false);

			inline void begin(int level, const Codeelement<RGB>* ce);
			inline void add(const Codeelement<RGB>* ce);
			inline bool add(const Codeelement<RGB>* ce1, const Codeelement<RGB>* ce2, bool justTest=false);

			inline void setFeature(RGB& rgb);
			inline int compareFeatures(const RGB& rgbCE1, const RGB& rgbCE2, const RGB& rgbSubCE);
			inline bool colorSimilar(const RGB& rgb1, const RGB& rgb2);
		};


		// TrafficSignLinkControlSingleLinkageOnRGB
		class TrafficSignLinkControlOnRGB: public TrafficSignHSVSimilarityFunction
		{
		private:
			int count,currLevel;
			int redSum,greenSum,blueSum;
			int avgRed,avgBlue,avgGreen;
			//int HSVDISTANCE[16][16][3];
			HSV rgbToHSV[64][64][64];
			HSV avgHSV;

		public:
			TrafficSignLinkControlOnRGB();

			inline void firstFeature(const RGB& rgb, int weight);
			inline bool addFeature(const RGB& rgb, int weight);

			inline void begin(int level, const RGB& rgb);
			inline void add(const RGB& rgb);
			inline bool add(const RGB& rgb1, const RGB& rgb2, bool justTest=false);

			inline void begin(int level, const Codeelement<RGB>* ce);
			inline void add(const Codeelement<RGB>* ce);
			inline bool add(const Codeelement<RGB>* ce1, const Codeelement<RGB>* ce2, bool justTest=false);

			inline void setFeature(RGB& rgb);
			inline int compareFeatures(const RGB& rgbCE1, const RGB& rgbCE2, const RGB& rgbSubCE);
			inline bool colorSimilar(const RGB& rgb1, const RGB& rgb2);
		};

		// TrafficSignLinkControl
		class TrafficSignLinkControlSingleLinkage: public TrafficSignHSVSimilarityFunction
		{
		private:
			int count,hueCount,redLowCount,redHighCount,currLevel;
			int hueSum,satSum,valSum;
			HSV avgHSV;

		public:
			TrafficSignLinkControlSingleLinkage();

			inline void firstFeature(const HSV& hsv, int weight);
			inline bool addFeature(const HSV& hsv, int weight);

			inline void begin(int level, const HSV& hsv);
			inline void add(const HSV& hsv);
			inline bool add(const HSV& hsv1, const HSV& hsv2, bool justTest=false);

			inline void begin(int level, const Codeelement<HSV>* ce);
			inline void add(const Codeelement<HSV>* ce);
			inline bool add(const Codeelement<HSV>* ce1, const Codeelement<HSV>* ce2, bool justTest=false);

			inline void setFeature(HSV& hsv);
			inline int compareFeatures(const HSV& hsvCE1, const HSV& hsvCE2, const HSV& hsvSubCE);

			inline bool colorSimilar(const HSV& hsv1, const HSV& hsv2) { return colorSimilarLink(hsv1.hue,hsv1.sat,hsv1.val,hsv2.hue,hsv2.sat,hsv2.val); }
		};


		// StdTrafficSignLinkControlSingleLinkageOnRGB
		class StdTrafficSignLinkControlSingleLinkageOnRGB: public TrafficSignHSVSimilarityFunction
		{
		private:
			int count,currLevel;
			int redSum,greenSum,blueSum;
			//int HSVDISTANCE[16][16][3];
			HSV rgbToHSV[64][64][64];

		public:
			StdTrafficSignLinkControlSingleLinkageOnRGB();

			inline void firstFeature(const RGB& rgb, int weight);
			inline bool addFeature(const RGB& rgb, int weight);

			inline void begin(int level, const RGB& rgb);
			inline void add(const RGB& rgb);
			inline bool add(const RGB& rgb1, const RGB& rgb2, bool justTest=false);

			inline void begin(int level, const Codeelement<RGB>* ce);
			inline void add(const Codeelement<RGB>* ce);
			inline bool add(const Codeelement<RGB>* ce1, const Codeelement<RGB>* ce2, bool justTest=false);

			inline void setFeature(RGB& rgb);
			inline int compareFeatures(const RGB& rgbCE1, const RGB& rgbCE2, const RGB& rgbSubCE);
			inline bool colorSimilar(const RGB& rgb1, const RGB& rgb2);
		};

		// TrafficSignLinkControl
		inline void TrafficSignLinkControl::firstFeature(const HSV& hsv, int weight)
		{
			currLevel=-1;
			redLowCount=0;
			redHighCount=0;
			if (hsv.sat>0)
			{
				hueSum=weight*hsv.hue;
				if (hsv.hue<=60) redLowCount=weight*360;
				else if (hsv.hue>=300) redHighCount=1;
				hueCount=weight;
			}
			else 
			{
				hueCount=0;
				hueSum=0;
			}
			satSum=weight*hsv.sat;
			valSum=weight*hsv.val;
			avgHSV=hsv;
			count=weight;
		}


		inline bool TrafficSignLinkControl::addFeature(const HSV& hsv, int weight)
		{
			if (colorSimilarLink(avgHSV.hue,avgHSV.sat,avgHSV.val,hsv.hue,hsv.sat,hsv.val))
			{
				if (hsv.sat>0)
				{
					if (hsv.hue<=60) redLowCount+=weight*360;
					else if (hsv.hue>=300) redHighCount++;
					hueSum+=weight*hsv.hue;
					hueCount+=weight;
				}
				satSum+=weight*hsv.sat;
				valSum+=weight*hsv.val;
				count+=weight;

				if (hueCount>0)
				{
					if (redHighCount>0)
						avgHSV.hue=(hueSum+redLowCount+(hueCount>>1))/hueCount;
					else avgHSV.hue=(hueSum+(hueCount>>1))/hueCount;
					if (avgHSV.hue>=360) avgHSV.hue=avgHSV.hue%360;
				}
				else avgHSV.hue=0;
				avgHSV.sat=satSum/count;
				avgHSV.val=valSum/count;
				//hueSum=avgHSV.hue*hueCount;
				//red=(avgHSV.hue>300);

				return true;
			}
			return false;
		}


		void TrafficSignLinkControl::begin(int level, const HSV& hsv)
		{
			currLevel=level;
			redLowCount=0;
			redHighCount=0;
			if (hsv.sat>0)
			{
				hueSum=hsv.hue;
				if (hsv.hue<=60) redLowCount=360;
				else if (hsv.hue>=300) redHighCount=1;
				hueCount=1;
			}
			else 
			{
				hueCount=0;
				hueSum=0;
			}
			satSum=hsv.sat;
			valSum=hsv.val;
			count=1;
		}


		void TrafficSignLinkControl::add(const HSV& hsv)
		{
			if (hsv.sat>0)
			{
				if (hsv.hue<=60) redLowCount+=360;
				else if (hsv.hue>=300) redHighCount++;
				hueSum+=hsv.hue;
				hueCount++;
			}
			satSum+=hsv.sat;
			valSum+=hsv.val;
			count++;
		}


		bool TrafficSignLinkControl::add(const HSV& hsv1, const HSV& hsv2, bool justTest)
		{
			if (colorSimilarDetect(hsv1.hue,hsv1.sat,hsv1.val,hsv2.hue,hsv2.sat,hsv2.val))
			{
				if (justTest==false)
				{
					if (hsv2.sat>0)
					{
						if (hsv2.hue<=60) redLowCount+=360;
						else if (hsv2.hue>=300) redHighCount++;
						hueSum+=hsv2.hue;
						hueCount++;
					}
					satSum+=hsv2.sat;
					valSum+=hsv2.val;
					count++;
				}
				return true;
			}
			return false;
		}


		void TrafficSignLinkControl::begin(int level, const Codeelement<HSV>* ce)
		{
			currLevel=level;
			redLowCount=0;
			redHighCount=0;
			if (ce->data.sat>0)
			{
				hueSum=ce->data.hue;
				if (ce->data.hue<=60) redLowCount=360;
				else if (ce->data.hue>=300) redHighCount=1;
				hueCount=1;
			}
			else 
			{
				hueCount=0;
				hueSum=0;
			}
			satSum=ce->data.sat;
			valSum=ce->data.val;
			avgHSV.hue=hueSum;
			avgHSV.sat=satSum;
			avgHSV.val=valSum;
			count=1;
		}


		void TrafficSignLinkControl::add(const Codeelement<HSV>* ce)
		{
			if (ce->data.sat>0)
			{
				if (ce->data.hue<=60) redLowCount+=360;
				else if (ce->data.hue>=300) redHighCount++;
				hueSum+=ce->data.hue;
				hueCount++;
			}
			satSum+=ce->data.sat;
			valSum+=ce->data.val;
			count++;

			if (hueCount>0) 
			{
				if (redHighCount>0)
					avgHSV.hue=(hueSum+redLowCount+(hueCount>>1))/hueCount;
				else avgHSV.hue=(hueSum+(hueCount>>1))/hueCount;
				if (avgHSV.hue>=360) avgHSV.hue=avgHSV.hue%360;
			}
			else avgHSV.hue=0;
			avgHSV.sat=satSum/count;
			avgHSV.val=valSum/count;
		}


		bool TrafficSignLinkControl::add(const Codeelement<HSV>* ce1, const Codeelement<HSV>* ce2, bool justTest)
		{
			if (colorSimilarLink(avgHSV.hue,avgHSV.sat,avgHSV.val,ce2->data.hue,ce2->data.sat,ce2->data.val))
				//&& colorSimilarLink(ce1->data.hue,ce1->data.sat,ce1->data.val,ce2->data.hue,ce2->data.sat,ce2->data.val))
			{
				if (justTest==false)
				{
					if (ce2->data.sat>0)
					{
						if (ce2->data.hue<=60) redLowCount+=360;
						else if (ce2->data.hue>=300) redHighCount++;
						hueSum+=ce2->data.hue;
						hueCount++;
					}
					satSum+=ce2->data.sat;
					valSum+=ce2->data.val;
					count++;

					if (hueCount>0) 
					{
						if (redHighCount>0)
							avgHSV.hue=(hueSum+redLowCount+(hueCount>>1))/hueCount;
						else avgHSV.hue=(hueSum+(hueCount>>1))/hueCount;
						if (avgHSV.hue>=360) avgHSV.hue=avgHSV.hue%360;
					}
					else avgHSV.hue=0;
					avgHSV.sat=satSum/count;
					avgHSV.val=valSum/count;
					//hueSum=avgHSV.hue*hueCount;
					//red=(avgHSV.hue>300);
				}	
				return true;
			}
			return false;
		}


		void TrafficSignLinkControl::setFeature(HSV& hsv)
		{
			if (currLevel==0)
			{
				if (hueCount>0) 
				{
					if (redHighCount>0)
						avgHSV.hue=(hueSum+redLowCount+(hueCount>>1))/hueCount;
					else avgHSV.hue=(hueSum+(hueCount>>1))/hueCount;
					if (avgHSV.hue>=360) avgHSV.hue=avgHSV.hue%360;
				}
				else avgHSV.hue=0;

				avgHSV.sat=satSum/count;
				avgHSV.val=valSum/count;
			}
			hsv.hue=avgHSV.hue;
			//hsv.sat=(255*((int)(avgHSV.sat)>>4))/15;
			//hsv.val=(255*((int)(avgHSV.val)>>4))/15;
			hsv.sat=avgHSV.sat;
			hsv.val=avgHSV.val;
		}


		int TrafficSignLinkControl::compareFeatures(const HSV& hsvCE1, const HSV& hsvCE2, const HSV& hsvSubCE)
		{
			return compareColors(hsvSubCE.hue,hsvSubCE.sat,hsvSubCE.val,
				hsvCE1.hue,hsvCE1.sat,hsvCE1.val,
				hsvCE2.hue,hsvCE2.sat,hsvCE2.val);
		}


		// TrafficSignLinkControlSingleLinkage
		inline void TrafficSignLinkControlSingleLinkage::firstFeature(const HSV& hsv, int weight)
		{
			currLevel=-1;
			redLowCount=0;
			redHighCount=0;
			if (hsv.sat>0)
			{
				hueSum=weight*hsv.hue;
				if (hsv.hue<=60) redLowCount=weight*360;
				else if (hsv.hue>=300) redHighCount=1;
				hueCount=weight;
			}
			else 
			{
				hueCount=0;
				hueSum=0;
			}
			satSum=weight*hsv.sat;
			valSum=weight*hsv.val;
			avgHSV=hsv;
			count=weight;
		}


		inline bool TrafficSignLinkControlSingleLinkage::addFeature(const HSV& hsv, int weight)
		{
			if (colorSimilarLink(avgHSV.hue,avgHSV.sat,avgHSV.val,hsv.hue,hsv.sat,hsv.val))
			{
				if (hsv.sat>0)
				{
					if (hsv.hue<=60) redLowCount+=weight*360;
					else if (hsv.hue>=300) redHighCount++;
					hueSum+=weight*hsv.hue;
					hueCount+=weight;
				}
				satSum+=weight*hsv.sat;
				valSum+=weight*hsv.val;
				count+=weight;

				if (hueCount>0)
				{
					if (redHighCount>0)
						avgHSV.hue=(hueSum+redLowCount+(hueCount>>1))/hueCount;
					else avgHSV.hue=(hueSum+(hueCount>>1))/hueCount;
					if (avgHSV.hue>=360) avgHSV.hue=avgHSV.hue%360;
				}
				else avgHSV.hue=0;
				avgHSV.sat=satSum/count;
				avgHSV.val=valSum/count;
				//hueSum=avgHSV.hue*hueCount;
				//red=(avgHSV.hue>300);

				return true;
			}
			return false;
		}


		void TrafficSignLinkControlSingleLinkage::begin(int level, const HSV& hsv)
		{
			currLevel=level;
			redLowCount=0;
			redHighCount=0;
			if (hsv.sat>0)
			{
				hueSum=hsv.hue;
				if (hsv.hue<=60) redLowCount=360;
				else if (hsv.hue>=300) redHighCount=1;
				hueCount=1;
			}
			else 
			{
				hueCount=0;
				hueSum=0;
			}
			satSum=hsv.sat;
			valSum=hsv.val;
			count=1;
		}


		void TrafficSignLinkControlSingleLinkage::add(const HSV& hsv)
		{
			if (hsv.sat>0)
			{
				if (hsv.hue<=60) redLowCount+=360;
				else if (hsv.hue>=300) redHighCount++;
				hueSum+=hsv.hue;
				hueCount++;
			}
			satSum+=hsv.sat;
			valSum+=hsv.val;
			count++;
		}


		bool TrafficSignLinkControlSingleLinkage::add(const HSV& hsv1, const HSV& hsv2, bool justTest)
		{
			if (colorSimilarDetect(hsv1.hue,hsv1.sat,hsv1.val,hsv2.hue,hsv2.sat,hsv2.val))
			{
				if (justTest==false)
				{
					if (hsv2.sat>0)
					{
						if (hsv2.hue<=60) redLowCount+=360;
						else if (hsv2.hue>=300) redHighCount++;
						hueSum+=hsv2.hue;
						hueCount++;
					}
					satSum+=hsv2.sat;
					valSum+=hsv2.val;
					count++;
				}
				return true;
			}
			return false;
		}


		void TrafficSignLinkControlSingleLinkage::begin(int level, const Codeelement<HSV>* ce)
		{
			currLevel=level;
			redLowCount=0;
			redHighCount=0;
			if (ce->data.sat>0)
			{
				hueSum=ce->data.hue;
				if (ce->data.hue<=60) redLowCount=360;
				else if (ce->data.hue>=300) redHighCount=1;
				hueCount=1;
			}
			else 
			{
				hueCount=0;
				hueSum=0;
			}
			satSum=ce->data.sat;
			valSum=ce->data.val;
			avgHSV.hue=hueSum;
			avgHSV.sat=satSum;
			avgHSV.val=valSum;
			count=1;
		}


		void TrafficSignLinkControlSingleLinkage::add(const Codeelement<HSV>* ce)
		{
			if (ce->data.sat>0)
			{
				if (ce->data.hue<=60) redLowCount+=360;
				else if (ce->data.hue>=300) redHighCount++;
				hueSum+=ce->data.hue;
				hueCount++;
			}
			satSum+=ce->data.sat;
			valSum+=ce->data.val;
			count++;

			if (hueCount>0) 
			{
				if (redHighCount>0)
					avgHSV.hue=(hueSum+redLowCount+(hueCount>>1))/hueCount;
				else avgHSV.hue=(hueSum+(hueCount>>1))/hueCount;
				if (avgHSV.hue>=360) avgHSV.hue=avgHSV.hue%360;
			}
			else avgHSV.hue=0;
			avgHSV.sat=satSum/count;
			avgHSV.val=valSum/count;
		}


		bool TrafficSignLinkControlSingleLinkage::add(const Codeelement<HSV>* ce1, const Codeelement<HSV>* ce2, bool justTest)
		{
			if (colorSimilarLink(ce1->data.hue,ce1->data.sat,ce1->data.val,ce2->data.hue,ce2->data.sat,ce2->data.val))
				//&& colorSimilarLink(ce1->data.hue,ce1->data.sat,ce1->data.val,ce2->data.hue,ce2->data.sat,ce2->data.val))
			{
				if (justTest==false)
				{
					if (ce2->data.sat>0)
					{
						if (ce2->data.hue<=60) redLowCount+=360;
						else if (ce2->data.hue>=300) redHighCount++;
						hueSum+=ce2->data.hue;
						hueCount++;
					}
					satSum+=ce2->data.sat;
					valSum+=ce2->data.val;
					count++;

					if (hueCount>0) 
					{
						if (redHighCount>0)
							avgHSV.hue=(hueSum+redLowCount+(hueCount>>1))/hueCount;
						else avgHSV.hue=(hueSum+(hueCount>>1))/hueCount;
						if (avgHSV.hue>=360) avgHSV.hue=avgHSV.hue%360;
					}
					else avgHSV.hue=0;
					avgHSV.sat=satSum/count;
					avgHSV.val=valSum/count;
					//hueSum=avgHSV.hue*hueCount;
					//red=(avgHSV.hue>300);
				}	
				return true;
			}
			return false;
		}


		void TrafficSignLinkControlSingleLinkage::setFeature(HSV& hsv)
		{
			if (currLevel==0)
			{
				if (hueCount>0) 
				{
					if (redHighCount>0)
						avgHSV.hue=(hueSum+redLowCount+(hueCount>>1))/hueCount;
					else avgHSV.hue=(hueSum+(hueCount>>1))/hueCount;
					if (avgHSV.hue>=360) avgHSV.hue=avgHSV.hue%360;
				}
				else avgHSV.hue=0;

				avgHSV.sat=satSum/count;
				avgHSV.val=valSum/count;
			}
			hsv.hue=avgHSV.hue;
			//hsv.sat=(255*((int)(avgHSV.sat)>>4))/15;
			//hsv.val=(255*((int)(avgHSV.val)>>4))/15;
			hsv.sat=avgHSV.sat;
			hsv.val=avgHSV.val;
		}


		int TrafficSignLinkControlSingleLinkage::compareFeatures(const HSV& hsvCE1, const HSV& hsvCE2, const HSV& hsvSubCE)
		{
			return compareColors(hsvSubCE.hue,hsvSubCE.sat,hsvSubCE.val,
				hsvCE1.hue,hsvCE1.sat,hsvCE1.val,
				hsvCE2.hue,hsvCE2.sat,hsvCE2.val);
		}

		// TrafficSignLinkControlSingleLinkageOnRGB
		inline void TrafficSignLinkControlSingleLinkageOnRGB::firstFeature(const RGB &rgb, int weight)
		{
			currLevel=-1;
			count=weight;
			redSum=(int)weight*rgb.red;
			greenSum=(int)weight*rgb.green;
			blueSum=(int)weight*rgb.blue;
			avgRed=rgb.red;
			avgGreen=rgb.green;
			avgBlue=rgb.blue;
			avgHSV=rgbToHSV[avgRed>>2][avgGreen>>2][avgBlue>>2];
		}


		inline bool TrafficSignLinkControlSingleLinkageOnRGB::addFeature(const RGB &rgb, int weight)
		{
			const HSV& hsv = rgbToHSV[rgb.red>>2][rgb.green>>2][rgb.blue>>2];
			if (TrafficSignHSVSimilarityFunction::colorSimilarLink(avgHSV.hue,avgHSV.sat,avgHSV.val,hsv.hue,hsv.sat,hsv.val))
			{
				redSum+=weight*(int)rgb.red;
				greenSum+=weight*(int)rgb.green;
				blueSum+=weight*(int)rgb.blue;
				count+=weight;
				avgRed=(redSum+(redSum>>1))/count;
				avgGreen=(greenSum+(greenSum>>1))/count;
				avgBlue=(blueSum+(blueSum>>1))/count;
				avgHSV=rgbToHSV[avgRed>>2][avgGreen>>2][avgBlue>>2];
				return true; 
			}
			return false;
		}


		inline bool TrafficSignLinkControlSingleLinkageOnRGB::colorSimilar(const RGB& rgb1, const RGB& rgb2)
		{
			const HSV& hsv1 = rgbToHSV[rgb1.red>>2][rgb1.green>>2][rgb1.blue>>2];
			const HSV& hsv2 = rgbToHSV[rgb2.red>>2][rgb2.green>>2][rgb2.blue>>2];

			return TrafficSignHSVSimilarityFunction::colorSimilarDetect(hsv1.hue,hsv1.sat,hsv1.val,hsv2.hue,hsv2.sat,hsv2.val);
		}


		inline void TrafficSignLinkControlSingleLinkageOnRGB::begin(int level, const RGB& rgb)
		{
			currLevel=level;
			count=1;
			redSum=rgb.red;
			greenSum=rgb.green;
			blueSum=rgb.blue;
		}


		inline void TrafficSignLinkControlSingleLinkageOnRGB::add(const RGB& rgb)
		{
			redSum+=rgb.red;
			greenSum+=rgb.green;
			blueSum+=rgb.blue;
			count++;
		}


		inline bool TrafficSignLinkControlSingleLinkageOnRGB::add(const RGB& rgb1, const RGB& rgb2, bool justTest)
		{
			const HSV& hsv1 = rgbToHSV[rgb1.red>>2][rgb1.green>>2][rgb1.blue>>2];
			const HSV& hsv2 = rgbToHSV[rgb2.red>>2][rgb2.green>>2][rgb2.blue>>2];

			if (TrafficSignHSVSimilarityFunction::colorSimilarLink(hsv1.hue,hsv1.sat,hsv1.val,hsv2.hue,hsv2.sat,hsv2.val))
			{
				if (justTest==false)
				{
					redSum+=rgb2.red;
					greenSum+=rgb2.green;
					blueSum+=rgb2.blue;
					count++;
				}
				return true;
			}
			return false;
		}


		inline void TrafficSignLinkControlSingleLinkageOnRGB::begin(int level, const Codeelement<RGB>* ce)
		{
			currLevel=level;
			count=1;
			avgRed=redSum=ce->data.red;
			avgGreen=greenSum=ce->data.green;
			avgBlue=blueSum=ce->data.blue;
			avgHSV=rgbToHSV[avgRed>>2][avgGreen>>2][avgBlue>>2];
		}


		inline void TrafficSignLinkControlSingleLinkageOnRGB::add(const Codeelement<RGB>* ce)
		{
			int weight=(ce->numOfSubCEs>0) ? ce->numOfSubCEs : 1;

			redSum+=weight*(int)ce->data.red;
			greenSum+=weight*(int)ce->data.green;
			blueSum+=weight*(int)ce->data.blue;
			count+=weight;
			avgRed=(redSum+(redSum>>1))/count;
			avgGreen=(greenSum+(greenSum>>1))/count;
			avgBlue=(blueSum+(blueSum>>1))/count;
			avgHSV=rgbToHSV[avgRed>>2][avgGreen>>2][avgBlue>>2];
		}


		inline bool TrafficSignLinkControlSingleLinkageOnRGB::add(const Codeelement<RGB>* ce1, const Codeelement<RGB>* ce2, bool justTest)
		{
			const HSV& hsv1 = rgbToHSV[ce1->data.red>>2][ce1->data.green>>2][ce1->data.blue>>2];
			const HSV& hsv2 = rgbToHSV[ce2->data.red>>2][ce2->data.green>>2][ce2->data.blue>>2];
			if (TrafficSignHSVSimilarityFunction::colorSimilarLink(hsv1.hue,hsv1.sat,hsv1.val,hsv2.hue,hsv2.sat,hsv2.val))
			{
				if (justTest==false)
				{
					int weight=(ce2->numOfSubCEs>0) ? ce2->numOfSubCEs : 1;

					redSum+=weight*(int)ce2->data.red;
					greenSum+=weight*(int)ce2->data.green;
					blueSum+=weight*(int)ce2->data.blue;
					count+=weight;
					avgRed=(redSum+(redSum>>1))/count;
					avgGreen=(greenSum+(greenSum>>1))/count;
					avgBlue=(blueSum+(blueSum>>1))/count;
					avgHSV=rgbToHSV[avgRed>>2][avgGreen>>2][avgBlue>>2];
				}
				return true; 
			}
			return false;
		}


		inline void TrafficSignLinkControlSingleLinkageOnRGB::setFeature(RGB& rgb)
		{
			if (currLevel==0)
			{
				rgb.red=(redSum+(redSum>>1))/count;
				rgb.green=(greenSum+(greenSum>>1))/count;
				rgb.blue=(blueSum+(blueSum>>1))/count;
			}
			else
			{
				rgb.red=avgRed;
				rgb.green=avgGreen;
				rgb.blue=avgBlue;
			}
		}


		inline int TrafficSignLinkControlSingleLinkageOnRGB::compareFeatures(const RGB& rgbCE1, const RGB& rgbCE2, const RGB& rgbSubCE)
		{
			const HSV& hsvCE1   = rgbToHSV[rgbCE1.red>>2][rgbCE1.green>>2][rgbCE1.blue>>2];
			const HSV& hsvCE2   = rgbToHSV[rgbCE2.red>>2][rgbCE2.green>>2][rgbCE2.blue>>2];
			const HSV& hsvSubCE = rgbToHSV[rgbSubCE.red>>2][rgbSubCE.green>>2][rgbSubCE.blue>>2];

			return compareColors(hsvSubCE.hue,hsvSubCE.sat,hsvSubCE.val,
				hsvCE1.hue,hsvCE1.sat,hsvCE1.val,
				hsvCE2.hue,hsvCE2.sat,hsvCE2.val);
		}

		// TrafficSignLinkControlOnRGB
		inline void TrafficSignLinkControlOnRGB::firstFeature(const RGB &rgb, int weight)
		{
			currLevel=-1;
			count=weight;
			redSum=(int)weight*rgb.red;
			greenSum=(int)weight*rgb.green;
			blueSum=(int)weight*rgb.blue;
			avgRed=rgb.red;
			avgGreen=rgb.green;
			avgBlue=rgb.blue;
			avgHSV=rgbToHSV[avgRed>>2][avgGreen>>2][avgBlue>>2];
		}


		inline bool TrafficSignLinkControlOnRGB::addFeature(const RGB &rgb, int weight)
		{
			const HSV& hsv = rgbToHSV[rgb.red>>2][rgb.green>>2][rgb.blue>>2];
			if (TrafficSignHSVSimilarityFunction::colorSimilarLink(avgHSV.hue,avgHSV.sat,avgHSV.val,hsv.hue,hsv.sat,hsv.val))
			{
				redSum+=weight*(int)rgb.red;
				greenSum+=weight*(int)rgb.green;
				blueSum+=weight*(int)rgb.blue;
				count+=weight;
				avgRed=(redSum+(redSum>>1))/count;
				avgGreen=(greenSum+(greenSum>>1))/count;
				avgBlue=(blueSum+(blueSum>>1))/count;
				avgHSV=rgbToHSV[avgRed>>2][avgGreen>>2][avgBlue>>2];
				return true; 
			}
			return false;
		}


		inline bool TrafficSignLinkControlOnRGB::colorSimilar(const RGB& rgb1, const RGB& rgb2)
		{
			const HSV& hsv1 = rgbToHSV[rgb1.red>>2][rgb1.green>>2][rgb1.blue>>2];
			const HSV& hsv2 = rgbToHSV[rgb2.red>>2][rgb2.green>>2][rgb2.blue>>2];

			return TrafficSignHSVSimilarityFunction::colorSimilarDetect(hsv1.hue,hsv1.sat,hsv1.val,hsv2.hue,hsv2.sat,hsv2.val);
		}


		inline void TrafficSignLinkControlOnRGB::begin(int level, const RGB& rgb)
		{
			currLevel=level;
			count=1;
			redSum=rgb.red;
			greenSum=rgb.green;
			blueSum=rgb.blue;
			avgRed=redSum;
			avgGreen=greenSum;
			avgBlue=blueSum;
			avgHSV=rgbToHSV[avgRed>>2][avgGreen>>2][avgBlue>>2];
		}


		inline void TrafficSignLinkControlOnRGB::add(const RGB& rgb)
		{
			redSum+=rgb.red;
			greenSum+=rgb.green;
			blueSum+=rgb.blue;
			count++;
			avgRed=(redSum+(redSum>>1))/count;
			avgGreen=(greenSum+(greenSum>>1))/count;
			avgBlue=(blueSum+(blueSum>>1))/count;
			avgHSV=rgbToHSV[avgRed>>2][avgGreen>>2][avgBlue>>2];
		}


		inline bool TrafficSignLinkControlOnRGB::add(const RGB& rgb1, const RGB& rgb2, bool justTest)
		{
			//const HSV& hsv1 = rgbToHSV[rgb1.red>>2][rgb1.green>>2][rgb1.blue>>2];
			const HSV& hsv2 = rgbToHSV[rgb2.red>>2][rgb2.green>>2][rgb2.blue>>2];

			if (TrafficSignHSVSimilarityFunction::colorSimilarLink(avgHSV.hue,avgHSV.sat,avgHSV.val,hsv2.hue,hsv2.sat,hsv2.val))
			{
				if (justTest==false)
				{
					redSum+=rgb2.red;
					greenSum+=rgb2.green;
					blueSum+=rgb2.blue;
					count++;
					avgRed=(redSum+(redSum>>1))/count;
					avgGreen=(greenSum+(greenSum>>1))/count;
					avgBlue=(blueSum+(blueSum>>1))/count;
					avgHSV=rgbToHSV[avgRed>>2][avgGreen>>2][avgBlue>>2];
				}
				return true;
			}
			return false;
		}


		inline void TrafficSignLinkControlOnRGB::begin(int level, const Codeelement<RGB>* ce)
		{
			currLevel=level;
			count=1;
			avgRed=redSum=ce->data.red;
			avgGreen=greenSum=ce->data.green;
			avgBlue=blueSum=ce->data.blue;
			avgHSV=rgbToHSV[avgRed>>2][avgGreen>>2][avgBlue>>2];
		}


		inline void TrafficSignLinkControlOnRGB::add(const Codeelement<RGB>* ce)
		{
			int weight=(ce->numOfSubCEs>0) ? ce->numOfSubCEs : 1;

			redSum+=weight*(int)ce->data.red;
			greenSum+=weight*(int)ce->data.green;
			blueSum+=weight*(int)ce->data.blue;
			count+=weight;
			avgRed=(redSum+(redSum>>1))/count;
			avgGreen=(greenSum+(greenSum>>1))/count;
			avgBlue=(blueSum+(blueSum>>1))/count;
			avgHSV=rgbToHSV[avgRed>>2][avgGreen>>2][avgBlue>>2];
		}


		inline bool TrafficSignLinkControlOnRGB::add(const Codeelement<RGB>* ce1, const Codeelement<RGB>* ce2, bool justTest)
		{
			//const HSV& hsv1 = rgbToHSV[ce1->data.red>>2][ce1->data.green>>2][ce1->data.blue>>2];
			const HSV& hsv2 = rgbToHSV[ce2->data.red>>2][ce2->data.green>>2][ce2->data.blue>>2];
			if (TrafficSignHSVSimilarityFunction::colorSimilarLink(avgHSV.hue,avgHSV.sat,avgHSV.val,hsv2.hue,hsv2.sat,hsv2.val))
			{
				if (justTest==false)
				{
					int weight=(ce2->numOfSubCEs>0) ? ce2->numOfSubCEs : 1;

					redSum+=weight*(int)ce2->data.red;
					greenSum+=weight*(int)ce2->data.green;
					blueSum+=weight*(int)ce2->data.blue;
					count+=weight;
					avgRed=(redSum+(redSum>>1))/count;
					avgGreen=(greenSum+(greenSum>>1))/count;
					avgBlue=(blueSum+(blueSum>>1))/count;
					avgHSV=rgbToHSV[avgRed>>2][avgGreen>>2][avgBlue>>2];
				}
				return true; 
			}
			return false;
		}


		inline void TrafficSignLinkControlOnRGB::setFeature(RGB& rgb)
		{
			rgb.red=(redSum+(redSum>>1))/count;
			rgb.green=(greenSum+(greenSum>>1))/count;
			rgb.blue=(blueSum+(blueSum>>1))/count;
		}


		inline int TrafficSignLinkControlOnRGB::compareFeatures(const RGB& rgbCE1, const RGB& rgbCE2, const RGB& rgbSubCE)
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
			/*
			const HSV& hsvCE1   = rgbToHSV[rgbCE1.red>>2][rgbCE1.green>>2][rgbCE1.blue>>2];
			const HSV& hsvCE2   = rgbToHSV[rgbCE2.red>>2][rgbCE2.green>>2][rgbCE2.blue>>2];
			const HSV& hsvSubCE = rgbToHSV[rgbSubCE.red>>2][rgbSubCE.green>>2][rgbSubCE.blue>>2];

			return compareColors(hsvSubCE.hue,hsvSubCE.sat,hsvSubCE.val,
			hsvCE1.hue,hsvCE1.sat,hsvCE1.val,
			hsvCE2.hue,hsvCE2.sat,hsvCE2.val);*/
		}
	}
}
#endif
