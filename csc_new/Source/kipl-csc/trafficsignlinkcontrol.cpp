/* 

This "SOFTWARE" is a free software.

You are allowed to download, use, modify and redistribute this software.
The software is provided "AS IS" without warranty of any kind.

Copyright: University of Koblenz, Lutz Priese, Volker Rehrman, Patrick Sturm

*/

// Include-Files
#include <kipl-csc/trafficsignlinkcontrol.h>
#include <kipl-transform/color-transform.h>

namespace kipl {
	namespace csc {

		TrafficSignLinkControl::TrafficSignLinkControl()
		{  
			currLevel=0;
			hueSum=0; 
			satSum=0;
			valSum=0;
			avgHSV.hue=0;
			avgHSV.val=0;
			avgHSV.sat=0;
			count=0;
			hueCount=0;
		}


		TrafficSignLinkControlSingleLinkage::TrafficSignLinkControlSingleLinkage()
		{  
			currLevel=0;
			hueSum=0; 
			satSum=0;
			valSum=0;
			avgHSV.hue=0;
			avgHSV.val=0;
			avgHSV.sat=0;
			count=0;
			hueCount=0;
		}


		TrafficSignLinkControlSingleLinkageOnRGB::TrafficSignLinkControlSingleLinkageOnRGB(): TrafficSignHSVSimilarityFunction()
		{
			currLevel=0;
			count=0;
			redSum=greenSum=blueSum=0;
			avgRed=avgBlue=avgGreen=0;
			for (int r=0; r<64; r++)
				for (int g=0; g<64; g++)
					for (int b=0; b<64; b++)
					{
						RGB rgb;

						rgb.red=(r*255)/63;
						rgb.green=(g*255)/63;
						rgb.blue=(b*255)/63;
						convertRGBToHSV(rgb,rgbToHSV[r][g][b],false);
					}
		}


		TrafficSignLinkControlOnRGB::TrafficSignLinkControlOnRGB(): TrafficSignHSVSimilarityFunction()
		{
			currLevel=0;
			count=0;
			redSum=greenSum=blueSum=0;
			avgRed=avgBlue=avgGreen=0;
			for (int r=0; r<64; r++)
				for (int g=0; g<64; g++)
					for (int b=0; b<64; b++)
					{
						RGB rgb;

						rgb.red=(r*255)/63;
						rgb.green=(g*255)/63;
						rgb.blue=(b*255)/63;
						convertRGBToHSV(rgb,rgbToHSV[r][g][b],false);
					}
		}
	}
}
