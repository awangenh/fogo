// Include-Section
#include <kipl-base/types.h>
#include <kipl-base/error.h>
#include <kipl-base/image.h>
#include <kipl-base/color.h>
#include <kipl-base/loadsavemanager.h>
#include <string>
#include <memory>
#include <iostream>
#include <iomanip>

// Declaration Section
namespace kipl {
	const Image::ImageType lastValidType = Image::pfHSV16;

	const unsigned typeToBPP[9] = {0,1,2,4,4,3,6,4,6};

	ROI& ROI::moveTo(int x, int y, int z) {
		return move(x-(int)left,y-(int)top,z-(int)front);
	}

	ROI& ROI::move(int dx, int dy, int dz) {
		int _left = dx+(int)left;
		int _right = dx+(int)right;
		int _top = dy+(int)top;
		int _bottom = dy+(int)bottom;
		int _front = dz+(int)front;
		int _back = dz+(int)back;
		if (_left>=0) left=(unsigned)_left; else left=0;
		if (_top>=0) top=(unsigned)_top; else top=0;
		if (_front>=0) front=(unsigned)_front; else front=0;
		if (_right>=0) right=(unsigned)_right; else right=0;
		if (_bottom>=0) bottom=(unsigned)_bottom; else bottom=0;
		if (_back>=0) back=(unsigned)_back; else back=0;

		return *this;
	}

	ROI& ROI::clip(unsigned w, unsigned h, unsigned d) {
		if (left>w) left=w;
		if (right>w) right=w;
		if (top>h) top=h;
		if (bottom>h) bottom=h;
		if (back>d) back=d;
		if (front>d) front=d;

		return *this;
	}

	ROI& ROI::intersection(const ROI& roi) {
		if (left<roi.left) left=roi.left;
		if (top<roi.top) top=roi.top;
		if (front<roi.front) front=roi.front;
		if (roi.right<right) right=roi.right;
		if (roi.bottom<bottom) bottom=roi.bottom;
		if (roi.back<back) back=roi.back;
		return *this;
	}

	void ROI::printTo(std::ostream& str) const {
		str << "[(" << left << "," << right << ");("
			<< top << "," << bottom << ");("
			<< front << "," << back << ")]";
	}

	Base* ROI::clone() const {
		ROI* roi = new ROI;
		roi->assign(this);
		return roi;
	}

	void ROI::assign(const Base* obj) {
		const ROI* roi = dynamic_cast<const ROI*>(obj);
		if (roi) {
			front=roi->front;
			back=roi->back;
			left=roi->left;
			top=roi->top;
			right=roi->right;
			bottom=roi->bottom;
		}
		else throw EInvalidArgument();
	}

	bool ROI::equalDimension(const ROI& op2) const{
		return ((back-front==op2.back-op2.front) &&
						(right-left==op2.right-op2.left) &&
						(bottom-top==op2.bottom-op2.top));
	}
	
	bool ROI::operator == (const ROI& op2) const
	{
		return ((front == op2.front) &&
						(back == op2.back) &&
						(left == op2.left) &&
						(right == op2.right) &&
						(top == op2.top) &&
						(bottom == op2.bottom));
	}
	
	void Image::updateChannelInfos() {
		switch(type) {
			case pfGray: channels=1; channelType=pfGray; break;
			case pfGray16: channels=1; channelType=pfGray16; break;
			case pfGray32: channels=1; channelType=pfGray32; break;
			case pfGrayFloat: channels=1; channelType=pfGrayFloat; break;
			case pfRGB: channels=3; channelType=pfGray; break;
			case pfRGB16: channels=3; channelType=pfGray16; break;
			case pfHSV: channels=3; channelType=pfUndefined; break;
			case pfHSV16: channels=3; channelType=pfGray16; break;
			default: channels=0; channelType=pfUndefined;
		}
	}

	Image::Image(const Image& img): Properties(img) {
		subImagesEnBloc=false;
		width=height=numOfSubImages=0;
		channelType=type=pfUndefined;
		channels=0;
		subImageIndex=NULL;
		scanlineIndex=NULL;
		ownsPixelData=true;
		min=0.0f;
		max=-1.0f;
		roi.bottom=roi.front=roi.back=roi.left=roi.right=roi.top=0;
		copyPixelData(img);
	}

	Image::Image(unsigned _width, unsigned _height, const Image& image, unsigned _depth) {
		subImagesEnBloc=false;
		width=height=numOfSubImages=0;
		channelType=type=pfUndefined;
		channels=0;
		subImageIndex=NULL;
		scanlineIndex=NULL;
		ownsPixelData=true;
		min=0.0f;
		max=-1.0f;
		roi.bottom=roi.front=roi.back=roi.left=roi.right=roi.top=0;
		create(_width,_height,image,_depth);

	}

	Image::Image(unsigned _width, unsigned _height, ImageType _imgtype, UINT8* image, unsigned _numOfSubImages, bool owner, unsigned _channels) {
		subImagesEnBloc=false;
		width=height=numOfSubImages=0;
		channelType=type=pfUndefined;
		channels=0;
		subImageIndex=NULL;
		scanlineIndex=NULL;
		ownsPixelData=true;
		min=0.0f;
		max=-1.0f;
		roi.bottom=roi.front=roi.back=roi.left=roi.right=roi.top=0;
		if (_width>0 && _height>0 && _numOfSubImages>0 && _imgtype!=pfUndefined) {
			if (image) changeImageData(_width,_height,_numOfSubImages,_imgtype,image,owner,_channels);
			else create(_width,_height,_numOfSubImages,_imgtype,_channels);
		}
	}

	Image::~Image() {
		free();
	}

	void Image::copyPixelData(const Image& img) {
		if (img.scanlineIndex && img.subImageIndex) {
			const unsigned sl_size=img.width*img.getBytesPerPixel();
			
			const unsigned subImageSize = sl_size*img.height;

			if (width==img.width && height==img.height && numOfSubImages==img.numOfSubImages
														&& getBytesPerPixel()==img.getBytesPerPixel()) {
				for (unsigned i=0; i<img.numOfSubImages; ++i)
					memcpy(subImageIndex[i][0],img.subImageIndex[i][0],subImageSize);
			} 
			else {
				free();
				buildIndex(img.height,img.numOfSubImages);
				unsigned sl_idx=0;
				for (unsigned i=0; i<img.numOfSubImages; ++i) {
					UINT8* image = new UINT8[subImageSize];
					memcpy(image,img.subImageIndex[i][0],subImageSize);
					for (unsigned j=0; j<img.height; ++j)  {
						scanlineIndex[sl_idx++]=image;
						image+=sl_size;
					}
				}
				subImagesEnBloc=false;
				ownsPixelData=true;
			}
			min=img.min;
			max=img.max;
			width=img.width;
			height=img.height;
			numOfSubImages=img.numOfSubImages;
			type=img.type;
			channels=img.channels;
			channelType=img.channelType;
			roi=img.roi;
		}
		else free();
	}

	void Image::assign(const Base* obj) {
		const Image* img = dynamic_cast<const Image*>(obj);
		if (img) {
			copyPixelData(*img);
			Properties::assign(obj);
		}
		else throw EInvalidArgument();
	}

	Base* Image::clone() const {
		Base* newImage = new Image();
		newImage->assign(this);

		return newImage;
	}

	/*void Image::loadImage(const std::string& filename, const std::string& format) {
		unsigned char   *data = NULL;
		unsigned char **pdata = &data;
		unsigned int _width, _height;
		static LoadSaveManager *lsm = LoadSaveManager::Instance();
		
		lsm->loadImage(filename, _width, _height, pdata);
		if (data != NULL) {
			changeImageData(_width, _height, 1, Image::pfRGB, pdata, true);
		}
	}

	void Image::saveImage(const std::string& filename, const std::string& format) {
		static LoadSaveManager *lsm = LoadSaveManager::Instance();
		
		lsm->saveImage(filename, format, getWidth(), getHeight(), subImage(0));
		//throw ENotImplemented();
	}*/


	unsigned Image::getBytesPerPixelChannel() const {
		if (channels==0) return 0;

		return getBytesPerPixel()/channels;
	}

	unsigned Image::getBytesPerPixel() const {
		if (channelType==pfUndefined) 
			return typeToBPP[type];

		return typeToBPP[channelType]*channels;
	}

	void Image::freePixelData() {
		if (subImageIndex) {
			if (ownsPixelData) {
				if (subImagesEnBloc) {
					delete[] subImageIndex[0][0];
					subImagesEnBloc=false;
				}
				else {
					for (unsigned i=0; i<numOfSubImages; ++i)
						if (subImageIndex[i][0]) 
							delete[] subImageIndex[i][0];
				}
			}
			delete[] subImageIndex;
			subImageIndex=NULL;
		}
		if (scanlineIndex) 	{
			delete[] scanlineIndex;
			scanlineIndex=NULL;
		}
		width=height=numOfSubImages=0;
		channelType=type=pfUndefined;
		channels=0;
		resetROI();
	}

	void Image::free() {
		freePixelData();
		clearProperties();
	}

	void Image::buildIndex(unsigned _height, unsigned _numOfSubImages) {
		if (height!=_height || numOfSubImages!=_numOfSubImages) {
			freePixelData();
			subImageIndex=new UINT8**[_numOfSubImages];
			scanlineIndex=new UINT8*[_numOfSubImages*_height];
			UINT8** scanline=scanlineIndex;
			for (unsigned i=0;i<_numOfSubImages; ++i) {
				subImageIndex[i]=scanline;
				scanline+=_height;
			}
		}
	}

	void Image::setROI(unsigned firstImg, unsigned lastImg, unsigned left, unsigned top, unsigned right, unsigned bottom) {
		roi.left=(left<=width) ? left : width;
		roi.top=(top<=height) ? top : height;
		roi.front=(firstImg<=numOfSubImages) ? firstImg : numOfSubImages;
		roi.right=(right<=width) ? right : width;
		roi.bottom=(bottom<=height) ? bottom : height;
		roi.back=(lastImg<=numOfSubImages) ? lastImg : numOfSubImages;
	}

	void Image::setROI(const ROI& newROI) {
		setROI(newROI.front,newROI.back,newROI.left,newROI.top,newROI.right,newROI.bottom);
	}

	void Image::setROI(unsigned left, unsigned top, unsigned right, unsigned bottom) {
		setROI(0,numOfSubImages,left,top,right,bottom);
	}

	void Image::resetROI() {
		setROI(0,numOfSubImages,0,0,width,height);
	}

	void Image::changeImageData(unsigned _width, unsigned _height, unsigned _numOfSubImages, ImageType _type, UINT8** images, bool owner, unsigned _channels) {
		if (_type<pfGray || _type>lastValidType|| images==NULL)
			throw EInvalidArgument();
		else{
			freePixelData();
			if (_width>0 && _height>0 && _numOfSubImages>0) {
				buildIndex(_height,_numOfSubImages);
				width=_width;
				height=_height;
				numOfSubImages=_numOfSubImages;
				type=_type;
				updateChannelInfos();
				if (type==channelType && channels==1 && _channels>1) {
					type=pfUndefined;
					channels=_channels;
				}
				
				unsigned sl_size=width*getBytesPerPixel();
				unsigned sl_idx=0;
				
				for (unsigned i=0; i<numOfSubImages; ++i) {
					UINT8* image = images[i];
					for (unsigned j=0; j<height; ++j)  {
						scanlineIndex[sl_idx++]=image;
						image+=sl_size;
					}
				}
				subImagesEnBloc=false;
				ownsPixelData=owner;
				resetROI();
			}
		}
	}

	void Image::changeImageData(unsigned _width, unsigned _height, ImageType _type, const std::vector<UINT8*>& images, bool owner, unsigned _channels) {
		unsigned n = (unsigned)images.size();
		if (_type<pfGray || _type>lastValidType)
			throw EInvalidArgument();
		else {
			freePixelData();
			if (_width>0 && _height>0 && n>0) {
				buildIndex(_height,n);
				width=_width;
				height=_height;
				numOfSubImages=n;
				type=_type;
				updateChannelInfos();
				if (type==channelType && channels==1 && _channels>1) {
					type=pfUndefined;
					channels=_channels;
				}

				unsigned sl_size=width*getBytesPerPixel();
				unsigned sl_idx=0;

				for (unsigned i=0; i<n; ++i) {
					UINT8* image = images[i];
					for (unsigned j=0; j<height; ++j)  {
						scanlineIndex[sl_idx++]=image;
						image+=sl_size;
					}
				}
				ownsPixelData=owner;
				subImagesEnBloc=false;
				resetROI();
			}
		}
	}

	/*void Image::enforceTypeAndDimensionOf(const Image& image)
		create(image.getWidth(),image.getHeight(),image,image.getDepth());
	}*/


	void Image::create(const ROI& roi, ImageType _type, unsigned _channels) {
		create(roi.width(),roi.height(),roi.depth(),_type,_channels);
	}

	void Image::create(const ROI& roi, const Image& image) {
		if (image.getImageType()==pfUndefined) 
			create(roi.width(),roi.height(),roi.depth(),image.getChannelType(),image.getNumberOfChannels());
		else create(roi.width(),roi.height(),roi.depth(),image.getImageType());
	}

	void Image::create(unsigned _width, unsigned _height, const Image& image, unsigned _numOfSubImages) {
		if (image.getImageType()==pfUndefined) 
			create(_width,_height,_numOfSubImages,image.getChannelType(),image.getNumberOfChannels());
		else create(_width,_height,_numOfSubImages,image.getImageType());
	}


	void Image::create(unsigned _width, unsigned _height, unsigned _numOfSubImages, ImageType _type, unsigned _channels) {
		if (_width==0 || _height==0 || _numOfSubImages==0) return;
		if (width==_width && height==_height && numOfSubImages==_numOfSubImages && type==_type) return;
		if (_type<pfGray || _type>pfHSV16) 
			throw EInvalidArgument();
		
		buildIndex(_height,_numOfSubImages);
		width=_width;
		height=_height;
		numOfSubImages=_numOfSubImages;
		type=_type;
		updateChannelInfos();
		if (type==channelType && channels==1 && _channels>1) {
			type=pfUndefined;
			channels=_channels;
		}

		unsigned sl_size=width*getBytesPerPixel();
		unsigned sl_idx=0;

		for (unsigned i=0; i<numOfSubImages; ++i) {
			UINT8* image=new UINT8[height*sl_size];
			for (unsigned j=0; j<height; ++j)  {
				scanlineIndex[sl_idx++]=image;
				image+=sl_size;
			}
		}
		subImagesEnBloc=false;
		ownsPixelData=true;
		resetROI();
	}


	void Image::changeImageData(unsigned _width, unsigned _height, unsigned _numOfSubImages, ImageType _type, UINT8* image, bool owner, unsigned _channels) {
		if (_type<pfGray || _type>pfHSV16) 
			throw EInvalidArgument();
		else {
			freePixelData();
			if (_width>0 && _height>0 && _numOfSubImages>0) {
				buildIndex(_height,_numOfSubImages);
				width=_width;
				height=_height;
				numOfSubImages=_numOfSubImages;
				type=_type;
				updateChannelInfos();
				if (type==channelType && channels==1 && _channels>1) {
					type=pfUndefined;
					channels=_channels;
				}
			
				unsigned sl_size=width*getBytesPerPixel();
				unsigned sl_idx=0;
			
				for (unsigned i=0; i<numOfSubImages; ++i) {
					for (unsigned j=0; j<height; ++j)  {
						scanlineIndex[sl_idx++]=image;
						image+=sl_size;
					}
				}
				ownsPixelData=owner;
				resetROI();
				subImagesEnBloc=true;
			}
		}
	}

	template <typename el_type>
	void computeMinMax(el_type** arr, SIZE rows, SIZE cols, FLOAT& min, FLOAT& max, SIZE off=0, SIZE step=1) {
		if (rows>0 && cols>0 && off<cols) {
			min=max=(FLOAT)arr[0][off];
			for (unsigned j=0; j<rows; ++j) {
				const el_type* row = arr[j];
				for (unsigned i=0; i<cols; i+=step) {
					if (min>row[i]) min=(FLOAT)row[i];
					else if (max<row[i]) max=(FLOAT)row[i];
				}
			}
		}
		else {
			min=0.0f;
			max=-1.0f;
		}
	}


	void Image::updateLimits() {
		if (subImageIndex && scanlineIndex) {
			if (type==pfHSV) 
				computeMinMax<UINT8>((UINT8**)scanlineIndex,height*numOfSubImages,3*width,min,max,2,3);
			else if (type==pfHSV16) 
				computeMinMax<UINT16>((UINT16**)scanlineIndex,height*numOfSubImages,3*width,min,max,2,3);
			else if (channelType==pfGray) 
				computeMinMax<GRAY>((GRAY**)scanlineIndex,height*numOfSubImages,channels*width,min,max,0,1);
			else if (channelType==pfGray16) 
				computeMinMax<GRAY16>((GRAY16**)scanlineIndex,height*numOfSubImages,channels*width,min,max,0,1);
			else if (channelType==pfGray32) 
				computeMinMax<GRAY32>((GRAY32**)scanlineIndex,height*numOfSubImages,channels*width,min,max,0,1);
			else if (channelType==pfGrayFloat) 
				computeMinMax<GRAYFLOAT>((GRAYFLOAT**)scanlineIndex,height*numOfSubImages,channels*width,min,max,0,1);
		}
	}

	void Image::cut(kipl::Image& img) {
		unsigned w = roi.width();
		unsigned h = roi.height();
		unsigned numOfSubImages = roi.depth();

		const unsigned source_offset = (roi.left+roi.top*width)*getBytesPerPixel();
		const unsigned source_step = width*getBytesPerPixel();
		const unsigned target_step = w*getBytesPerPixel();

		img.create(w,h,*this,numOfSubImages);
		for (unsigned subImg=0; subImg<numOfSubImages; subImg++) {
			kipl::UINT8* source = subImage(subImg+roi.front)+source_offset;
			kipl::UINT8* target = img.subImage(subImg);
			for (unsigned y=0; y<h; ++y) {
				memcpy(target,source,target_step);
				source+=source_step;
				target+=target_step;
			}
		}
	}

	void Image::scaleDown(kipl::Image& img, unsigned factor){
		const unsigned neww = width/factor;
		const unsigned newh = height/factor;
		const unsigned newd = numOfSubImages/factor;
		img.create(neww,newh,*this,newd);
		
		const unsigned source_step = factor*getBytesPerPixel();
		const unsigned target_step = getBytesPerPixel();

		for (unsigned z=0; z<newd; ++z) {
			for (unsigned y=0; y<newh; ++y) {
				kipl::UINT8* source = scanline(z*factor,y*factor);
				kipl::UINT8* target = img.scanline(z,y);
				for (unsigned x=0; x<neww; ++x) {
					memcpy(target,source,getBytesPerPixel());
					source+=source_step;
					target+=target_step;
				}
			}
		}
	}

	void Image::scaleUp(kipl::Image& img, unsigned factor){
		const unsigned neww = width*factor;
		const unsigned newh = height*factor;
		const unsigned newd = numOfSubImages*factor;
		img.create(neww,newh,*this,newd);
		const unsigned bpp=getBytesPerPixel();

		for (unsigned z=0; z<numOfSubImages; z++) {
			for (unsigned y=0; y<height; y++) {
				kipl::UINT8* source = scanline(z,y);
				kipl::UINT8* target = img.scanline(z*factor,y*factor);
				for (unsigned x=0; x<width; x++) {
					for (unsigned i=0; i<factor; ++i){
						memcpy(target,source,bpp);
						target+=bpp;
					}
					source+=bpp;
				}
				source = img.scanline(z*factor,y*factor);
				for (unsigned i=1; i<factor; ++i){
					target=img.scanline(z*factor,y*factor+i);
					memcpy(target,source,neww*bpp);
				}
			}
			kipl::UINT8* source = img.subImage(z*factor);
			for (unsigned i=1; i<factor; ++i){
				kipl::UINT8* target=img.subImage(z*factor+i);
				memcpy(target,source,neww*newh*bpp);
			}
		}
	}
	
	void Image::scale(kipl::Image& result, int factor) {
		if (factor==0) return;
		if (factor<0) scaleDown(result, std::abs(factor)+1);
		else scaleUp(result, factor+1);
	}

	
	/** Enhances the image dimension at the border */
	void Image::enhance(unsigned left, unsigned right, unsigned top, unsigned bottom) {
		if (left>0 && right>0 && top>0 && bottom>0) {
			Image* copy = dynamic_cast<Image*>(clone());
			unsigned _width = width;
			unsigned _height = height;
			unsigned _off = getBytesPerPixel()*left;
			unsigned _slSize = getBytesPerPixel()*_width;

			create(left+width+right,top+height+bottom,numOfSubImages,type);

			unsigned slSize = getBytesPerPixel()*width;

			for (unsigned z=0; z<numOfSubImages; ++z) {
				for (unsigned y=0; y<top; ++y)
					memset(scanline(z,y),0,slSize);
				for (unsigned y=0; y<_height; ++y) {
					UINT8* slSrc = copy->scanline(z,y);
					UINT8* slTrg = scanline(z,y+top);
					memset(slTrg,0,slSize);
					memcpy(slTrg+_off,slSrc,_slSize);
				}
				for (unsigned y=top+_height; y<height; ++y)
					memset(scanline(z,y),0,slSize);
			}

			delete copy;
		}
	}


	/** Enhances the image dimension equally at the border */
	void Image::enhance(unsigned d) {
		enhance(d,d,d,d);
	}


	template <typename ChannelTypeSrc, typename ChannelTypeTrg>
	void __cast_image(Image& src, Image& trg) {
		unsigned size = src.getWidth()*src.getHeight()*src.getNumberOfChannels();
		for (unsigned z=0; z<src.getDepth(); ++z) {
			ChannelTypeSrc* srcData = (ChannelTypeSrc*)src.subImage(z);
			ChannelTypeTrg* trgData = (ChannelTypeTrg*)trg.subImage(z);
			ChannelTypeSrc* endData = srcData+size;
			while (srcData<endData)
				(*trgData++)=(ChannelTypeTrg)(*srcData++);
		}
	}


	template <typename ChannelTypeTrg>
	void _cast_image(Image& src, Image& trg) {
		switch (src.getChannelType()) {
			case Image::pfGray: 
				__cast_image<GRAY,ChannelTypeTrg>(src,trg);
				break;
			case Image::pfGray16: 
				__cast_image<GRAY16,ChannelTypeTrg>(src,trg);
				break;
			case Image::pfGray32: 
				__cast_image<GRAY32,ChannelTypeTrg>(src,trg);
				break;
			case Image::pfGrayFloat: 
				__cast_image<GRAYFLOAT,ChannelTypeTrg>(src,trg);
				break;
			default:
				kiplAssert(false); // get rid of warning "enumeration value not handled"
				break;
		}
	}


	void Image::changeChannelType(ImageType channelType) {
		if (channelType!=getChannelType()) {
			Image tmp(getWidth(),getHeight(),channelType,NULL,getDepth(),true,getNumberOfChannels());
			switch (channelType) {
				case Image::pfGray: 
					_cast_image<GRAY>(*this,tmp);
					break;
				case Image::pfGray16: 
					_cast_image<GRAY16>(*this,tmp);
					break;
				case Image::pfGray32: 
					_cast_image<GRAY32>(*this,tmp);
					break;
				case Image::pfGrayFloat: 
					_cast_image<GRAYFLOAT>(*this,tmp);
					break;
				default: throw EInvalidArgument();
			}
			stealImageData(&tmp);
		}
	}


	void Image::write(int x, int y, Image& src, int z) {
		if (src.getNumberOfChannels()==getNumberOfChannels()) {
			if (src.getImageType()==getImageType() && src.getChannelType()==getChannelType()) {
				ROI _srcROI = src.getROI();
				const int dx = x-(int)_srcROI.left;
				const int dy = y-(int)_srcROI.top;
				const int dz = z-(int)_srcROI.front;
				_srcROI.move(dx,dy,dz);
				_srcROI.intersection(roi);
				ROI _roi = _srcROI;
				_srcROI.move(-dx,-dy,-dz);

				if (_roi.isValid() && _srcROI.isValid()) {
					const unsigned w=_roi.width();
					const unsigned h=_roi.height();
					const unsigned d=_roi.depth();
					const unsigned srcStep = src.getWidth()*src.getBytesPerPixel();
					const unsigned trgStep = getWidth()*getBytesPerPixel();
					const unsigned slSize = w*getBytesPerPixel();

					for (unsigned z=0; z<d; ++z) {
						UINT8* srcData = src.scanline(z+_srcROI.front,_srcROI.top,_srcROI.left);
						UINT8* trgData = scanline(z+_roi.front,_roi.top,_roi.left);
						for (unsigned y=0; y<h; ++y) {
							memcpy(trgData,srcData,slSize);
							srcData+=srcStep;
							trgData+=trgStep;
						}
					}
				}
			}
		}
	}

	void Image::clear() {
		unsigned slLength = roi.width()*getBytesPerPixel();
		unsigned lineLength = getWidth()*getBytesPerPixel();
		for (unsigned z=roi.front; z<roi.back; ++z) {
			UINT8* sl=scanline(z,roi.top,roi.left);
			UINT8* slEnd=sl+roi.height()*lineLength;
			while (sl<slEnd) {
				memset(sl,0,slLength);
				sl+=lineLength;
			}
		}
	}



	void Image::printTo(std::ostream& str) const {
		unsigned bpp = getBytesPerPixel();
		str << "Width      = " << width << std::endl
			<< "Height     = " << height << std::endl
			<< "#Subimages = " << numOfSubImages << std::endl
			<< "#Channels  = " << channels << std::endl
			<< "Type       = " << type << std::endl
			<< "BPP        = " << bpp << std::endl;

		unsigned scanlines = numOfSubImages*height;
		for (unsigned i = 0; i<scanlines; ++i) {
			str << std::setw(4) << i << ": ";
			for (unsigned x = 0; x<width*bpp; ++x) {
				if (x>0) str << ", ";
				str << std::setw(3) << (int)scanlineIndex[i][x];
			}
			str << std::endl;
		}
		//std::cout << std::endl << "Properties:" << std::endl;
		Properties::printTo(str);
	}

	void Image::stealImageData(Image* victim){
		free();
		if (victim->getSubImagesEnBloc()){
			changeImageData(victim->getWidth(),victim->getHeight(),victim->getNumOfSubImages(),victim->getImageType(),
											victim->subImage(0),victim->isPixelDataOwner(),victim->getNumberOfChannels());
		}
		else{
			std::vector<UINT8*> subim(victim->getNumOfSubImages());
			for (unsigned i=0; i<victim->getNumOfSubImages(); ++i){
				subim[i]=victim->subImage(i);
			}
			changeImageData(victim->getWidth(),victim->getHeight(),victim->getImageType(),
											subim,victim->isPixelDataOwner(),victim->getNumberOfChannels());
		}
		victim->setPixelDataOwner(false);
		setSimpleProperty<std::string>(*this,"filename",victim->getFileName());
	}

	template <typename ChannelType>
	void _channelSplit(Image& img, std::vector<Image*>& channels, Image::ImageType channelType) {
		const ROI roi = img.getROI();

		const unsigned w = roi.width();
		const unsigned h = roi.height();
		const unsigned d = roi.depth();
		const unsigned size = w*h;
		const unsigned numOfChannels = img.getNumberOfChannels();

		for (unsigned channel=0; channel<channels.size(); ++channel)
			if (channels[channel]) delete channels[channel];
		channels.resize(numOfChannels);

		unsigned out_step = w;
		unsigned in_step = numOfChannels*w;
		for (unsigned channel=0; channel<numOfChannels ; ++channel) {
			channels[channel]=new Image(w,h,channelType,NULL,d,true,1);

			for (unsigned z=0; z<d; ++z) {
				ChannelType* out = (ChannelType*)channels[channel]->subImage(z);
				ChannelType* in = (ChannelType*)img.scanline(roi.front+z,roi.top,roi.left)+channel;
				ChannelType* outEnd = out+size;
				while (out<outEnd)
				{
					ChannelType* x_out = out;
					ChannelType* x_end = out+w;
					ChannelType* x_in = in;
					while (x_out<x_end) {
						(*x_out++)=*x_in;
						x_in+=numOfChannels;
					}
					out+=out_step;
					in+=in_step;
				}
			}
		}
	}

	/* Channel Split */
	void Image::channelSplit(std::vector<Image*>& channels) {
		switch(getChannelType()) 
		{
		case Image::pfGray: _channelSplit<GRAY>(*this,channels,Image::pfGray); break;
		case Image::pfGray16: _channelSplit<GRAY16>(*this,channels,Image::pfGray16); break;
		case Image::pfGray32: _channelSplit<GRAY32>(*this,channels,Image::pfGray32); break;
		case Image::pfGrayFloat: _channelSplit<GRAYFLOAT>(*this,channels,Image::pfGrayFloat); break;
		default: throw EInvalidArgument();
		}
	}


	template <typename ChannelType>
	void _channelMerge(Image& img, const std::vector<Image*>& channels) {
		ROI roi = channels[0]->getROI();
		unsigned channelImgs = (unsigned)channels.size();
		unsigned w = roi.width();
		unsigned h = roi.height();
		unsigned d = roi.depth();
		unsigned currChannel=0;

		unsigned trgChannels = img.getNumberOfChannels();
		unsigned stepTrg=trgChannels*img.getWidth();
		for (unsigned channelImg = 0; channelImg<channelImgs; ++channelImg) {
			unsigned srcChannels = channels[channelImg]->getNumberOfChannels();
			unsigned stepSrc=srcChannels*img.getWidth();
			for (unsigned channel=0; channel<srcChannels; ++channel) {
				for (unsigned z=0; z<d; ++z) {
					ChannelType* slSrc = (ChannelType*)channels[channelImg]->scanline(roi.front+z,roi.top,roi.left)+channel;
					ChannelType* slTrg = (ChannelType*)img.scanline(z,0,0)+currChannel;
					for (unsigned y=0; y<h; ++y) {
						ChannelType* _slSrc = slSrc;
						ChannelType* _slEnd = slSrc+w*srcChannels;
						ChannelType* _slTrg = slTrg;
						while (_slSrc<_slEnd) {
							(*_slTrg)=(*_slSrc);
							_slSrc+=srcChannels;
							_slTrg+=trgChannels;
						}
						slSrc+=stepSrc;
						slTrg+=stepTrg;
					}
				}
				++currChannel;
			}
		}
	}


	/* Channel Merge */
	void Image::channelMerge(const std::vector<Image*>& channels) {
		if (channels.size()==0) throw EInvalidArgument();

		unsigned numOfChannels=channels[0]->getNumberOfChannels();
		unsigned w = channels[0]->getWidth();
		unsigned h = channels[0]->getHeight();
		unsigned d = channels[0]->getDepth();
		Image::ImageType channelType = channels[0]->getChannelType();

		for (unsigned channel=1; channel<channels.size(); ++channel) {
			numOfChannels+=channels[channel]->getNumberOfChannels();
			if (w!=channels[channel]->getWidth() || h!=channels[channel]->getHeight() || 
				d!=channels[channel]->getDepth() || channelType!=channels[channel]->getChannelType())
				throw EInvalidArgument();
		}

		ROI roi = channels[0]->getROI();

		create(roi.width(),roi.height(),roi.depth(),channelType,numOfChannels);

		switch(getChannelType())
		{
		case Image::pfGray: _channelMerge<GRAY>(*this,channels); break;
		case Image::pfGray16: _channelMerge<GRAY16>(*this,channels); break;
		case Image::pfGray32: _channelMerge<GRAY32>(*this,channels); break;
		case Image::pfGrayFloat: _channelMerge<GRAYFLOAT>(*this,channels); break;
		default:	std::cerr << "kipl::Image::channelMerge not implemented for this channel type" << std::endl; 
		}
	}


	template <typename GrayType>
	void _addOffset(GrayType* values, unsigned n, float offset, float scale) {
		for (unsigned i=0; i<n; ++i)
			values[i]=(GrayType)(((float)values[i]+offset)*scale);
	}

	void Image::addOffset(float offset, float scale) {
		if (offset!=0.0f && scale!=1.0f) {
			unsigned n = width*height*channels;
			switch(getChannelType())
			{
			case Image::pfGray:
				for (unsigned z=0; z<numOfSubImages; ++z)
					_addOffset((GRAY*)subImageIndex[z][0],n,offset,scale);
				break;
			case Image::pfGray16:
				for (unsigned z=0; z<numOfSubImages; ++z)
					_addOffset((GRAY16*)subImageIndex[z][0],n,offset,scale);
				break;
			case Image::pfGray32:
				for (unsigned z=0; z<numOfSubImages; ++z)
					_addOffset((GRAY32*)subImageIndex[z][0],n,offset,scale);
				break;
			case Image::pfGrayFloat:
				for (unsigned z=0; z<numOfSubImages; ++z)
					_addOffset((GRAYFLOAT*)subImageIndex[z][0],n,offset,scale);
				break;
			default:	std::cerr << "kipl::Image::channelMerge not implemented for this channel type" << std::endl; 
			}
		}
	}

	float getPixelWidth(const Image& img){
		return getSimpleProperty<float>(img,"pixelwidth");
	}

	float getPixelHeight(const Image& img){
		return getSimpleProperty<float>(img,"pixelheight");
	}

	float getPixelDepth(const Image& img){
		return getSimpleProperty<float>(img,"pixeldepth");
	}

}
