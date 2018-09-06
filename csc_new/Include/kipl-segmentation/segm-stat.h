#ifndef _segm_stat_h_
#define _segm_stat_h_

// Include
#include <kipl-base/error.h>
#include <kipl-base/color.h>
#include <kipl-base/image.h>
#include <map>
#include <vector>

namespace kipl {


	struct SegmentInfo
	{
		std::vector<double> graySum;
		double area;
		GRAY32 newLabel;
		bool used;
	};


	unsigned estimateSegmentInfo(Image& label, Image& orig, std::map<GRAY32,SegmentInfo>& info, GRAY32 offset=0);
	void generateRegionImage(Image& labImage, Image& regImage, std::map<unsigned,SegmentInfo>& labelInfo);
	void generateRegionImage(Image& orig, Image& labels, Image& region);
} // End of namespace
#endif
