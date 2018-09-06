#ifndef _kipl_image_h_
#define _kipl_image_h_

// Include-Section
#include <kipl-base/types.h>
#include <kipl-base/base.h>
#include <string>
#include <vector>

/** \file image.h
 * \brief Definition of the central image class of KIPL. 
 */
namespace kipl {
	/** \brief ROI is a structure that defines a 3d region of interest.
	 * 
	 * A more extensive description of MyClass.
	 */
	struct ROI: public Base {
		virtual void printTo(std::ostream& str) const;
		virtual void assign(const Base* obj);
		virtual Base* clone() const;
		
		ROI() {
			front=back=left=top=right=bottom=0;
		}

		ROI(const ROI& roi) {
			front=roi.front;
			back=roi.back;
			left=roi.left;
			top=roi.top;
			right=roi.right;
			bottom=roi.bottom;
		}

		/** First sub image within this region of interest */
		unsigned front;

		/** First sub image after front that is NOT in this region of interest*/
		unsigned back;

		/** Left side of this region of interest*/
		unsigned left;
		
		/** Top side of this region of interest*/
		unsigned top;
		
		/** Right side of this region of interest. BEWARE: This side does not belong to the ROI.*/
		unsigned right;
		
		/** Bottom side of this region of interesr. BEWARE: This side does not belong to the ROI.*/
		unsigned bottom;

		/** Returns true if this ROI is valid ROI */
		inline bool isValid() const {
			return (left<right && top<bottom && front<back);
		}

		inline unsigned width() const { return right-left; }
		inline unsigned height() const { return bottom-top; }
		inline unsigned depth() const { return back-front; }

		ROI& move(int dx, int dy, int dz=0);
		ROI& moveTo(int x, int y, int z=0);
		ROI& clip(unsigned w, unsigned h, unsigned d=1);
		ROI& intersection(const ROI& roi);

		bool equalDimension(const ROI& op2) const;
		bool operator == (const ROI& op2) const;
	};

	/** \brief Image is the central image class of KIPL
	 */
	class Image: public Properties {
	public:
		/** Supported image types */
		enum ImageType {pfUndefined,pfGray,pfGray16,pfGray32,pfGrayFloat,pfRGB,pfRGB16,pfHSV,pfHSV16};

		/** Constructor of image
		  * \param _width Width of image
		  * \param _height Height of image
		  * \param _imgtype Type of image
		  * \param image Pointer to the pixel data. If image is NULL a new image will be allocated
		  * \param numOfSubImages Defines of how many sub images the new image should be consist. This is useful for 3d images or sequences.
		  * \param owner Defines if the image object is allowed to delete the pointer to (parameter) image.
		  * \param Number of channels. If channels>0 _imgtype denotes the channel type. For a channel type only gray types are allowed
		*/
		Image(unsigned _width=0, unsigned _height=0, ImageType _imgtype=pfRGB, UINT8* image=NULL, unsigned _numOfSubImages=1, bool owner=true, unsigned channels=0);

		/** Constructor creates an image of _width x _height x _depth voxels with a type of "image" */
		Image(unsigned _width, unsigned _height, const Image& image, unsigned _depth=0);

		Image(const Image& img);

		/** Destructor*/
		virtual ~Image();

		/** assign copies the content of obj to this object	*/
		virtual void assign(const Base* obj);

		/** clone creates a copy of this image*/
		virtual Base* clone() const;

		/** printTo prints this image to a stream. This is just for debugging purposes. */
		virtual void printTo(std::ostream& str) const;

		/** Loads an image from filename with given format
		void loadImage(const std::string& filename, const std::string& format="");*/

		/** Loads an image to filename with given format
		void saveImage(const std::string& filename, const std::string& format="");*/

		/** Returns the width of the image*/
		inline unsigned getWidth() const { return width; }

		/** Returns the height of the image*/
		inline unsigned getHeight() const { return height; }

		/** Returns the depth of the image*/
		inline unsigned getDepth() const { return numOfSubImages; }

		/** Returns true if the image isn't empty*/
		inline bool isFilled() const { return (width>0 && height>0 && numOfSubImages>0); }
		
		/** Returns the number of sub images*/
		inline unsigned getNumOfSubImages() const { return numOfSubImages; }

		/** Returns the number of bytes needed for one channel of a pixel */
		unsigned getBytesPerPixelChannel() const;

		/** Returns the number of bytes per pixel of this image*/
		unsigned getBytesPerPixel() const;

		/** Returns the number of channels */
		unsigned getNumberOfChannels() const { return channels; }

		/** Returns the image type of this image*/
		ImageType getImageType() const { return type; }

		/** Returns the image type of this image*/
		ImageType getChannelType() const { return channelType; }

		/** Creates an image showing the content of ROI */
		void cut(kipl::Image& img);

		/** Returns the filename of this image*/
		std::string getFileName() const { 
			return getSimpleProperty<std::string>(*this,"filename");
		}

		/** Sets the filename of this image*/
		void setFileName(const std::string& fn){ 
			setSimpleProperty<std::string>(*this,"filename",fn);
		}
		
		/** Creates a new image
		  * \param _width Width of image
		  * \param _height Height of image
		  * \param _imgtype Type of image
		*/
		void create(unsigned _width, unsigned _height, unsigned _numOfSubImages, ImageType _type, unsigned _channels=0);

		/** Creates a new image
		  * \param roi Dimension of image
		  * \param _imgtype Type of image
		*/
		void create(const ROI& roi, ImageType _type, unsigned _channels=0);

		/** Creates a new image with the type of "image"
		  * \param roi Dimension of new image
		  * \param Image Reference type of new image
		*/
		void create(const ROI& roi, const Image& image);

		/** Creates a new image with the type of "image"
		  * \param _width Width of image
		  * \param _height Height of image
		  * \param Image Reference type of new image
		*/
		void create(unsigned _width, unsigned _height, const Image& image, unsigned _numOfSubImages);

		//void enforceTypeAndDimensionOf(const Image& image);

		/** changeImageData fills the image with information
		  * \param _width The width of the image
		  * \param _height The height of the image
		  * \param _numOfSubImages The number of (2d) sub images
		  * \param _type The image type
		  * \param images Pointer to an array of 2d images
		  * \param owner Defines wether the param 'images' should be freed by this image object
		*/
		void changeImageData(unsigned _width, unsigned _height, unsigned _numOfSubImages, ImageType _type, UINT8** images, bool owner, unsigned _channels=0);

		/** changeImageData fills the image with information
		  * \param _width The width of the image
		  * \param _height The height of the image
		  * \param _type The image type
		  * \param images A vector with pointer to 2d images
		  * \param owner Defines wether the pointers in the vector 'images' should be freed by this image object
		*/
		void changeImageData(unsigned _width, unsigned _height, ImageType _type, const std::vector<UINT8*>& images, bool owner, unsigned _channels=0);

		/** changeImageData fills the image with information
		  * \param _width Width of image
		  * \param _height Height of image
		  * \param _imgtype Type of image
		  * \param image Pointer to the pixel data
		  * \param numOfSubImages Defines of how many sub images the new image should be consist. This is useful for 3d images or sequences.
		  * \param owner Defines if the image object is allowed to delete the pointer to (parameter) image.
		*/
		void changeImageData(unsigned _width, unsigned _height, unsigned _numOfSubImages, ImageType _type, UINT8* image, bool owner, unsigned _channels=0);

		/** Returns a pointer to sub image idx. Be careful. The internal pointer structure may be harmed*/
		UINT8*** getSubImageIndex() const { return subImageIndex; };

		/** stealImageData fills the image with information from an other image and ``steals´´ its pixel data
		  * \param _width The image to take information and data from
		*/
		void stealImageData(Image* victim);
		
		/** Returns a pointer to sub image 'subImg' */
		inline UINT8* subImage(unsigned subImg) { return subImageIndex[subImg][0]; };

		/** Returns a pointer to the scanline 'line' of the first sub image*/
		inline UINT8* scanline(unsigned line) { return scanlineIndex[line]; }

		/** Returns a pointer to the scanline 'line' of the sub image 'subImg'*/
		inline UINT8* scanline(unsigned subImg, unsigned line) { return subImageIndex[subImg][line]; }

		/** Returns a pointer to the scanline 'line' of the sub image 'subImg' at column col*/
		inline UINT8* scanline(unsigned subImg, unsigned line, unsigned col) { return &(subImageIndex[subImg][line][col*getBytesPerPixel()]); }

		/** Returns the minimum (channel) value stored in the image. The value is only valid if you call updateLimits before. */
		inline float getMin() const { return min; }

		inline void setMin(float val) { min=val; }
		inline void setMax(float val) { max=val; }

		/** Returns the maximum (channel) value stored in the image. The value is only valid if you call updateLimits before. */
		inline float getMax() const { return max; }

		/** Returns the region of interest of the image */
		inline const ROI& getROI() const { return roi; }

		/** Sets the region of interest of the image*/
		void setROI(const ROI& newROI);

		/** Sets the region of interest of the image*/
		void setROI(unsigned left, unsigned top, unsigned right, unsigned bottom);

		/** Sets the region of interest of the image*/
		void setROI(unsigned firstImg, unsigned lastImg, unsigned left, unsigned top, unsigned right, unsigned bottom);

		/** Resets the region of interest of the image*/
		void resetROI();

		/** This function clears the image and delete some memory. */
		void free();

		/** Calculates the minimum and maximum (channel) value of the image */
		void updateLimits();

		/** Sets the ownership of the pixel buffer */
		void setPixelDataOwner(bool owner) { ownsPixelData=owner; }

		/** Returns true if the image owns the pixel buffer */
		bool isPixelDataOwner() const { return ownsPixelData; }

		/** Sub images en bloc */
		bool getSubImagesEnBloc() const { return subImagesEnBloc || numOfSubImages<=1; };

		/** Enhances the image dimension at the border */
		void enhance(unsigned left, unsigned right, unsigned top, unsigned bottom);

		/** Enhances the image dimension equally at the border */
		void enhance(unsigned d);

		/** Copies the image content of src into this image. The operation takes care of the src.roi and this->roi. */
		void write(int x, int y, Image& src, int z=0);

		/** Sets all pixels within ROI to zero */
		void clear();

		/* Channel Split */
		void channelSplit(std::vector<Image*>& channels);

		/* Channel Merge */
		void channelMerge(const std::vector<Image*>& channels);

		void changeChannelType(ImageType newChannelType);

		void addOffset(float offset, float scale=1.0f);

		/** 
		 * Scales images up or down
		 * 
		 * @param img Resulting image
		 * @param factor "Zoomfactor",
		 * if < 0 reduce dimensions (width=width/(-factor+1), (height=height/(-factor+1) (depth=depth/(-factor+1)
		 * if > 0 increase dimensions (width=width*(factor+1), (height=height*(factor+1), (depth=depth*(factor+1)
		 */
		void scale(kipl::Image& result, int factor);
		
	protected:
		void freePixelData();
		void buildIndex(unsigned _height, unsigned _numOfSubImages);
		void scaleDown(kipl::Image& img, unsigned factor);
		void scaleUp(kipl::Image& img, unsigned factor);
		
	private:
		void updateChannelInfos();
		void copyPixelData(const Image& img);

		float min,max;
		unsigned  numOfSubImages;
		unsigned width,height,channels;
		ImageType type,channelType;
		UINT8*** subImageIndex;
		UINT8** scanlineIndex;
		bool ownsPixelData;
		bool subImagesEnBloc;
		ROI roi;
	};

	/** 
	 * Returns the width of one pixel (or voxel in 3d) in
	 * the image in mm
	 */
	float getPixelWidth(const Image& img);
	/** 
	 * Returns the height of one pixel (or voxel in 3d) in
	 * the image in mm
	 */
	float getPixelHeight(const Image& img);
	/** 
	 * Returns the depth of one pixel (or voxel in 3d) in
	 * the image in mm
	 */
	float getPixelDepth(const Image& img);
}

#endif
