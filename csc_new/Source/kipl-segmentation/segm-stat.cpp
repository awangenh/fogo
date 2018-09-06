// Include
#include <kipl-segmentation/segm-stat.h>
#include <kipl-base/color.h>
#include <kipl-base/error.h>


namespace kipl {

	template <typename ChannelType>
	unsigned _estimateSegmentInfo(Image& label, Image& orig, std::map<GRAY32,SegmentInfo>& info, GRAY32 offset) {
		unsigned maxLabel=0;
		const unsigned channels = orig.getNumberOfChannels();
		const ROI& roi = label.getROI();
		const unsigned w=roi.width();
		const unsigned stepOrg = orig.getWidth()*channels;
		const unsigned stepLab = label.getWidth();

		for (unsigned z=roi.front; z<roi.back; ++z) {
			ChannelType* slOrg = ((ChannelType*)orig.scanline(z,roi.top,roi.left));
			GRAY32* slLab = (GRAY32*)label.scanline(z,roi.top,roi.left);
			GRAY32* slLabEnd = slLab+roi.height()*stepLab;
			while (slLab<slLabEnd) {
				for (unsigned xLab=0, xOrg=0; xLab<w; ++xLab) {
					GRAY32 lab = slLab[xLab];
					if (lab>0) {
						lab+=offset;
						slLab[xLab]=lab;
						if (lab>maxLabel) maxLabel=lab;
						std::map<GRAY32,SegmentInfo>::iterator it = info.find(lab);
						if (it==info.end()) {
							SegmentInfo& newInfo = info[lab];
							newInfo.area=1.0;
							newInfo.graySum.resize(channels);
							for (unsigned i=0; i<channels; ++i) {
								newInfo.graySum[i]=(double)slOrg[xOrg++];
								newInfo.used=false;
								newInfo.newLabel=lab;
							}
						}
						else
						{
							it->second.area+=1.0;
							for (unsigned i=0; i<channels; ++i) {
								it->second.graySum[i]+=(double)slOrg[xOrg++];
							}
						}
					}
				}
				slOrg+=stepOrg;
				slLab+=stepLab;
			}
		}
		return maxLabel;
	}


	unsigned estimateSegmentInfo(Image& label, Image& orig, std::map<GRAY32,SegmentInfo>& info, GRAY32 offset) {
		switch (orig.getChannelType()) 
		{
		case Image::pfGray:
			return _estimateSegmentInfo<GRAY>(label,orig,info,offset);
			break;
		case Image::pfGray16:
			return _estimateSegmentInfo<GRAY16>(label,orig,info,offset);
			break;
		default: throw EInvalidArgument();
		}
		return 0;
	}

	template <typename ChannelType, typename LabelType>
	void __generateRegionImage(Image& labImage, Image& regImage, std::map<unsigned,SegmentInfo>& labelInfo) {
		const unsigned numOfChannels = regImage.getNumberOfChannels();
		for (unsigned z=0; z<labImage.getDepth(); ++z) {
			for (unsigned y=0; y<labImage.getHeight(); ++y) {
				LabelType* slLab = (LabelType*)labImage.scanline(z,y);
				LabelType* slLabEnd = slLab+labImage.getWidth();
				ChannelType* slReg = (ChannelType*)regImage.scanline(z,y);
				while (slLab<slLabEnd) {
					if (*slLab>0) {
						*slLab=labelInfo[*slLab].newLabel;
						SegmentInfo& info = labelInfo[*slLab];
						for (unsigned c=0; c<numOfChannels; ++c) {
							(*slReg++)=(ChannelType)(info.graySum[c]/info.area);
						}
					}
					else {
						for (unsigned c=0; c<numOfChannels; ++c) {
							(*slReg++)=(ChannelType)0;
						}
					}
					slLab++;
				}
			}
		}
	}

	template <typename LabelType>
	void _generateRegionImage(Image& labImage, Image& regImage, std::map<unsigned,SegmentInfo>& labelInfo) {
		switch (regImage.getChannelType()) 
		{
		case Image::pfGray:
			__generateRegionImage<GRAY,LabelType>(labImage,regImage,labelInfo);
			break;
		case Image::pfGray16:
			__generateRegionImage<GRAY16,LabelType>(labImage,regImage,labelInfo);
			break;
		default: throw EInvalidArgument();
		}
	}


	void generateRegionImage(Image& labImage, Image& regImage, std::map<unsigned,SegmentInfo>& labelInfo) {
		switch (labImage.getChannelType()) 
		{
		case Image::pfGray32:
			_generateRegionImage<GRAY32>(labImage,regImage,labelInfo);
			break;
		default: throw EInvalidArgument();
		}
	}

	template <typename ChannelType, typename LabelType>
	void __generateRegionImage(Image& orig, Image& labels, Image& region){
		kipl::ROI roi=orig.getROI();
		labels.updateLimits();
		unsigned max=(unsigned)labels.getMax();
		unsigned channels=orig.getNumberOfChannels();
		std::vector<std::vector<double> > sums(channels);
		std::vector<unsigned>  counts(max+1);
		for (unsigned i=0; i<channels; ++i){
			sums[i].resize(max+1);
		}
		for (unsigned z=0; z<labels.getDepth();++z){
			for (unsigned y=0; y<labels.getHeight(); ++y){
				ChannelType* scanline = (ChannelType*)orig.scanline(z+roi.front,y+roi.top,roi.left);
				LabelType* lscanline = (LabelType*)labels.scanline(z,y);
				for (unsigned x=0; x<labels.getWidth(); ++x){
					counts[*lscanline]++;
					for (unsigned i=0; i<channels; ++i){
						sums[i][*lscanline]+=scanline[i];
					}
					++lscanline;
					scanline+=channels;
				}
			}
		}
		for (unsigned i=0; i<max+1; ++i){
			for (unsigned c=0; c<channels; ++c){
				if (counts[i]>0){
					sums[c][i]/=counts[i];
				}
				else{
					sums[c][i]=0;
				}
			}
		}
		for (unsigned z=0; z<labels.getDepth();++z){
			for (unsigned y=0; y<labels.getHeight(); ++y){
				LabelType* lscanline = (LabelType*)labels.scanline(z,y);
				ChannelType* scanline = (ChannelType*)region.scanline(z,y);
				for (unsigned x=0; x<labels.getWidth(); ++x){
					for (unsigned i=0; i<channels; ++i){
						scanline[i]=(ChannelType)sums[i][*lscanline];
					}
					++lscanline;
					scanline+=channels;
				}
			}
		}
	}

	template <typename LabelType>
	void _generateRegionImage(Image& orig, Image& labels, Image& region){
		switch (region.getChannelType()){
		case Image::pfGray: __generateRegionImage<UINT8,LabelType>(orig,labels,region); break;
		case Image::pfGray16: __generateRegionImage<UINT16,LabelType>(orig,labels,region); break;
		case Image::pfGray32: __generateRegionImage<UINT32,LabelType>(orig,labels,region); break;
		default: throw Exception("Channel type of input image has to be pfGray, pfGray16 or pfGray32 in generateRegionImage");
		}
	}
	
	void generateRegionImage(Image& orig, Image& labels, Image& region){
		kipl::ROI roi=orig.getROI();
		if (roi.width()!=labels.getWidth()
				|| roi.height()!=labels.getHeight()
				|| roi.depth()!=labels.getDepth()){
			throw Exception("Roi of orig has to have the same dimensions as whole label image in generateRegionImage");
		}
		if (roi.width()!=region.getWidth()
				|| roi.height()!=region.getHeight()
				|| roi.depth()!=region.getDepth()
				|| orig.getNumberOfChannels()!=region.getNumberOfChannels()
				|| orig.getChannelType()!=region.getChannelType()){
			region.create(roi.width(), roi.height(), orig, roi.depth());
		}
		switch (labels.getImageType()){
		case Image::pfGray: _generateRegionImage<UINT8>(orig,labels,region); break;
		case Image::pfGray16: _generateRegionImage<UINT16>(orig,labels,region); break;
		case Image::pfGray32: _generateRegionImage<UINT32>(orig,labels,region); break;
		default: throw Exception("Label image has to be pfGray, pfGray16 or pfGray32 in generateRegionImage");
		}
	}
} // end of namespace

