/* 

This "SOFTWARE" is a free software.

You are allowed to download, use, modify and redistribute this software.
The software is provided "AS IS" without warranty of any kind.

Copyright: University of Koblenz, Lutz Priese, Volker Rehrman, Patrick Sturm

*/


#ifndef _csc_h_
#define _csc_h_

/***********************************************
iMAge pROcessing cLAsses Version 1.0

Copyright 2002, Patrick Sturm
************************************************/

// Include-Files
#include <kipl-csc/islhierarchy.h>
#include <kipl-csc/codebase.h>
#include <kipl-csc/cscengine.h>
#include <map>

namespace kipl {
	namespace csc {
		/*! \brief The class CdSC is a compoment that encapsulate the csc algorithm.

		It consists of the 3 subcomponents: island hierarchy, codeelement base and csc engine.
		The class CSC is a template class that expect three template arguments: PixelType, FeatureType and LinkControl.

		\li \c PixelType describes the pixel format of the input image
		\li \c FeatureType describes the data format of codeelements. Normally \c PixelType and \c FeatureType are equivalent types.
		\li \c LinkControl is a module that controls how the csc should link codeelements together. Therefore
		a link control class encapsulates a color similarity function and a averaging function. 
		The standard link control classes are the StdRGBLinkControl and the StdHSVLinkControl class.
		On can change the behaviour of the csc algorithm by implementing an own LinkControlControl class.


		\b Important \b restrictions:

		\li The types PixelType and FeatureType must allow a copy operation:
		\verbatim
		PixelType p1,p2;
		FeatureType f1,f2;

		p1=p2; // <--- must be valid
		f1=f2; // <--- must be valid
		\endverbatim


		\li A \c LinkControl class has to satisfy the following interface:
		\verbatim
		class SampleLinkControl
		{
		SampleLinkControl();
		inline void begin(int level, const PixelType& pixel);
		inline bool add(const PixelType& pixel1, const PixelType& pixel2);
		inline void begin(int level, const Codeelement<FeatureType>* ce);
		inline bool add(const Codeelement<FeatureType>* ce1, const Codeelement<FeatureType>* ce2);
		inline void setFeature(PixelType& avgColor);
		inline int compareFeatures(const FeatureType& ce1, const FeatureType& ce2, const FeatureType& subCE);
		}
		\endverbatim
		*/
		template <class PixelType, class FeatureType, class LinkControl>
		class CSC
		{
		private:
			LinkControl* linkControl;
			IslandHierarchy* hierarchy;
			CodeelementBase<FeatureType>* ceBase;
			CSCEngine<PixelType,FeatureType,LinkControl>* cscEngine;
			double avgCEsPerIsland;
			SplittingMethod splittingMethod;
			RepairMethod repairMethod;
			bool updateAfterSplitting,bestNeighbor;
			int startPosition;
			int maxLinkLevel;

			void init(int width, int height);

		public:
			/*! The standard constructor tries to find the smallest island hierarchy that covers a "width x height"-raster 
			and initializes the three subcomponents (island hierarchy, codeelement base and csc engine).
			\param avgCEsPerIsland parameterized the size of the codeelement base
			*/
			CSC(int width = 0, int height = 0, double avgCEsPerIsland = 2.0);

			void freeAll();

			//! The destructor frees all subcomponents
			virtual ~CSC();

			void enableUpdateAfterSplitting(bool enabled);
			void enableBestNeighbor(bool enabled);
			void changeSplittingMethod(SplittingMethod newSplittingMethod);
			void changeRepairMethod(RepairMethod newRepairMethod);
			void changeStartPosition(int newStartPosition);
			void setMaxLinkLevel(int level=-1);


			/*! \brief Applies the csc segmentation on the given image.
			\param img Input Image
			*/
			void segmentation(Image& img);

			/*! \brief Applies the csc segmentation on the given image.

			\param img Input Image
			\param regImage Region Image
			*/
			void segmentation(Image& img, Image& regImage, int minLevel=0);

			/*! \brief Applies the csc segmentation on the given source image.

			\param srcImage source image. The source image has to be represented as
			an array with w*h fields of the type \c PixelType.
			\param labeledImage Resulting Labeled Image
			\param ceMap codeelement map. The method maps an integer label to a codeelement and stores this association in the codeelement map.
			*/    
			void segmentation(Image& srcImage, Image& labeledImage, std::map<int,CodeelementRef<FeatureType> >& ceMap, int minLevel=0);


			/*! \brief Applies the csc segmentation on the given source image.

			\param srcImage source image. The source image has to be represented as
			an array with w*h fields of the type \c PixelType.
			\param labeledImage Resulting Labeled Image. Every pixel is mapped
			to an integer label of the region he belongs to.
			\param featureMap feature map. The method maps an integer label to a feature vector and stores this association in the feature map.
			*/    
			void segmentation(Image& srcImage, Image& labeledImage, std::map<int,FeatureType>& featureMap, int minLevel=0);

			//! \returns the current island hierarchy.
			inline const IslandHierarchy* getIslandHierarchy() const;

			//! \returns the current codeelement base.
			inline CodeelementBase<FeatureType>* getCEBase() const;

			//! \returns the current link control object
			inline LinkControl* getLinkControl() const;

		};


		// Implementation of CSC
		template <class PixelType, class FeatureType, class LinkControl>
		void CSC<PixelType,FeatureType,LinkControl>::init(int width, int height)
		{
			//std::cout << "Init... (Width=" << width << "; Height=" << height << ")" << std::endl;
			if (width<=0 || height<=0)
			{
				//std::cout << "Clear CSC..." << std::endl;
				if (cscEngine!=NULL) delete cscEngine;
				if (ceBase!=NULL) delete ceBase;
				if (hierarchy!=NULL) delete hierarchy;
				cscEngine=NULL;
				ceBase=NULL;
				hierarchy=NULL;
			}
			else
			{
				if (hierarchy==NULL)
				{
					//std::cout << "Create Hierarchy..." << std::endl;
					hierarchy=new IslandHierarchy(width,height);
					//std::cout << "Create CEBase (" << avgCEsPerIsland << ")..." << std::endl;
					ceBase=new CodeelementBase<FeatureType>(*hierarchy,avgCEsPerIsland);
					//std::cout << "Create Engine..." << std::endl;
					cscEngine=new CSCEngine<PixelType,FeatureType,LinkControl>(*ceBase,linkControl);
				}
				else if (hierarchy->numberOfLevels()!=IslandHierarchy::numberOfLevels(width,height))
				{
					//std::cout << "Recreate CSC-Components..." << std::endl;
					hierarchy->init(width,height);
					ceBase->reinit(avgCEsPerIsland);
					/*if (cscEngine!=NULL) delete cscEngine;
					if (ceBase!=NULL) delete ceBase;
					if (hierarchy!=NULL) delete hierarchy;
					hierarchy=new IslandHierarchy(width,height);
					ceBase=new CodeelementBase<FeatureType>(*hierarchy,avgCEsPerIsland);
					cscEngine=new CSCEngine<PixelType,FeatureType,LinkControl>(*ceBase,linkControl);*/
				}
				if (cscEngine!=NULL)
				{
					//std::cout << "Initialize Engine..." << std::endl;
					cscEngine->setMaxLinkLevel(maxLinkLevel);
					cscEngine->changeRepairMethod(repairMethod);
					cscEngine->changeSplittingMethod(splittingMethod);
					cscEngine->enableUpdateAfterSplitting(updateAfterSplitting);
					cscEngine->enableBestNeighbor(bestNeighbor);
					cscEngine->changeStartPosition(startPosition);
				}
			}
		}


		template <class PixelType, class FeatureType, class LinkControl>
		void CSC<PixelType,FeatureType,LinkControl>::freeAll()
		{
			init(0,0);
		}

		template <class PixelType, class FeatureType, class LinkControl>
		CSC<PixelType,FeatureType,LinkControl>::CSC(int width, int height, double avgCEsPerIsland)
		{
			linkControl=new LinkControl();
			hierarchy=NULL;
			ceBase=NULL;
			cscEngine=NULL;
			repairMethod=PreciseRepair;
			splittingMethod=DirectSplitting;
			updateAfterSplitting=false;
			bestNeighbor=false;
			this->avgCEsPerIsland=avgCEsPerIsland;
			startPosition=0;
			init(width,height);
			maxLinkLevel=-1;
		}

		template <class PixelType, class FeatureType, class LinkControl>
		void CSC<PixelType,FeatureType,LinkControl>::setMaxLinkLevel(int level)
		{
			maxLinkLevel=level;
			if (cscEngine!=NULL) cscEngine->setMaxLinkLevel(level);
		}

		template <class PixelType, class FeatureType, class LinkControl>
		void CSC<PixelType,FeatureType,LinkControl>::changeStartPosition(int newStartPosition)
		{
			startPosition=newStartPosition;
			if (cscEngine!=NULL) cscEngine->changeStartPosition(newStartPosition);
		}


		template <class PixelType, class FeatureType, class LinkControl>
		void CSC<PixelType,FeatureType,LinkControl>::enableUpdateAfterSplitting(bool enabled)
		{
			updateAfterSplitting=enabled;
			if (cscEngine!=NULL) cscEngine->enableUpdateAfterSplitting(enabled);
		}


		template <class PixelType, class FeatureType, class LinkControl>
		void CSC<PixelType,FeatureType,LinkControl>::enableBestNeighbor(bool enabled)
		{
			bestNeighbor=enabled;
			if (cscEngine!=NULL) cscEngine->enableBestNeighbor(enabled);
		}


		template <class PixelType, class FeatureType, class LinkControl>
		void CSC<PixelType,FeatureType,LinkControl>::changeSplittingMethod(SplittingMethod newSplittingMethod)
		{
			splittingMethod=newSplittingMethod;
			if (cscEngine!=NULL) cscEngine->changeSplittingMethod(splittingMethod);
		}


		template <class PixelType, class FeatureType, class LinkControl>
		void CSC<PixelType,FeatureType,LinkControl>::changeRepairMethod(RepairMethod newRepairMethod)
		{
			repairMethod=newRepairMethod;
			if (cscEngine!=NULL) cscEngine->changeRepairMethod(repairMethod);
		}


		template <class PixelType, class FeatureType, class LinkControl>
		CSC<PixelType,FeatureType,LinkControl>::~CSC()
		{
			if (cscEngine!=NULL) delete cscEngine;
			if (ceBase!=NULL) delete ceBase;
			if (hierarchy!=NULL) delete hierarchy;
			if (linkControl!=NULL) delete linkControl;
		}


		template <class PixelType, class FeatureType, class LinkControl>
		void CSC<PixelType,FeatureType,LinkControl>::segmentation(Image& srcImage)
		{
			bool repeat;

			do
			{ 
				repeat = false;
				try
				{
					ROI roi = srcImage.getROI();
					init(roi.width(),roi.height());
					kiplAssert(cscEngine!=NULL);
					cscEngine->startSegmentation(srcImage);
				} 
				catch (ECodeelementBaseOverrun& /*exc*/)
				{
					avgCEsPerIsland*=2.0;
					ceBase->reinit(avgCEsPerIsland);
					repeat=true;
				}
			} while (repeat);
		}


		template <class PixelType, class FeatureType, class LinkControl>
		void CSC<PixelType,FeatureType,LinkControl>::segmentation(Image& srcImage, Image& regionImage, int minLevel)
		{
			segmentation(srcImage);
			const ROI& roi = srcImage.getROI();
			regionImage.create(roi.width(),roi.height(),srcImage,1);
			ceBase->drawRegionImage(regionImage,minLevel);
		}


		template <class PixelType, class FeatureType, class LinkControl>
		void CSC<PixelType,FeatureType,LinkControl>::segmentation(Image& srcImage, Image& labeledImage, std::map<int,CodeelementRef<FeatureType> >& ceMap, int minLevel)
		{
			segmentation(srcImage);
			const ROI& roi = srcImage.getROI();
			labeledImage.create(roi.width(),roi.height(),1,Image::pfGray32);
			ceBase->drawLabelImage(labeledImage,ceMap,minLevel);
		}


		template <class PixelType, class FeatureType, class LinkControl>
		void CSC<PixelType,FeatureType,LinkControl>::segmentation(Image& srcImage, Image& labeledImage, std::map<int,FeatureType>& featureMap, int minLevel)
		{
			segmentation(srcImage);
			const ROI& roi = srcImage.getROI();
			labeledImage.create(roi.width(),roi.height(),1,Image::pfGray32);
			ceBase->drawLabelImage(labeledImage,featureMap,minLevel);
		}


		template <class PixelType, class FeatureType, class LinkControl>
		inline const IslandHierarchy* CSC<PixelType,FeatureType,LinkControl>::getIslandHierarchy() const
		{
			return hierarchy;
		}


		template <class PixelType, class FeatureType, class LinkControl>
		inline CodeelementBase<FeatureType>* CSC<PixelType,FeatureType,LinkControl>::getCEBase() const
		{
			return ceBase;
		}


		template <class PixelType, class FeatureType, class LinkControl>
		inline LinkControl* CSC<PixelType,FeatureType,LinkControl>::getLinkControl() const
		{
			return linkControl;
		}
	}
}
#endif
