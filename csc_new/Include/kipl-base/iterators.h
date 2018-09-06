#ifndef _kipl_iterators_h_
#define _kipl_iterators_h_

// Include-Section
#include <kipl-base/image.h>
#include <kipl-base/error.h>


// Declaration-Section
namespace kipl {

	template <typename ChannelType>
	class PixelIterator
	{
	protected:
		Image* src;
		ChannelType* curr;
		unsigned cx,cy,cz,stepNextLine,channels,w,h,d;
		ROI roi;

	public:
		PixelIterator(Image& img) {
			src=&img;
			roi=img.getROI();
			w=roi.width();
			h=roi.height();
			d=roi.depth();

			if (img.getBytesPerPixel()==sizeof(ChannelType)) channels=1;
			else if (img.getBytesPerPixelChannel()==sizeof(ChannelType)) 
				channels=img.getNumberOfChannels();
			else throw EInvalidArgument();

			stepNextLine=(img.getWidth()-w)*channels;
			reset();
		}

		void reset() {
			curr=(ChannelType*)src->scanline(roi.front,roi.top,roi.left);
			cx=cy=cz=0;
		}

		inline bool nextColumn() {
			curr+=channels;
			return (++cx)<w;
		}


		inline bool nextLine() {
			if ((++cy)>=h) 
				return false;

			curr+=stepNextLine+(w-cx)*channels;
			cx=0;

			return true;
		}

		inline bool nextLayer() {
			if ((++cz)>=d) 
				return false;

			cx=cy=0;
			curr=(ChannelType*)src->scanline(roi.front+cz,roi.top,roi.left);

			return true;
		}


		inline unsigned posX() const
		{
			return cx;
		}

		inline unsigned posY() const
		{
			return cy;
		}

		inline unsigned posZ() const
		{
			return cz;
		}

		inline bool operator++() 
		{
			if (nextColumn() || nextLine() || nextLayer()) return true;

			return false;
		}

		inline ChannelType* operator*() 
		{
			return curr;
		}
	};


}
#endif
