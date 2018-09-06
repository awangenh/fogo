/* 

This "SOFTWARE" is a free software.

You are allowed to download, use, modify and redistribute this software.
The software is provided "AS IS" without warranty of any kind.

Copyright: University of Koblenz, Lutz Priese, Volker Rehrman, Patrick Sturm

*/


#ifndef _codebase_h_
#define _codebase_h_

/***********************************************
iMAge pROcessing cLAsses Version 1.0

Copyright 2002, Patrick Sturm
************************************************/

// Include-Files
#include <kipl-base/error.h>
#include <kipl-base/image.h>
#include <kipl-csc/islhierarchy.h>
#include <math.h>
#include <memory.h>
#include <map>
#include <list>
#include <iostream>

namespace kipl {
	namespace csc {
		// Codeelement-Flags
		enum CEFlags
		{
			CEUsed          =   1,
			CEUsed2         =   2,
			CERoot          =   4,
			CEVisited       =   8,
			CEInQueue       =  16,
			CEDisintegrated =  32,
			CEUser1         =  64,
			CEUser2         = 128
		};


		// Forward-Declarations
		template <class FeatureType> struct Codeelement;

		//! A subcodeelement contains a codeelement and a its relativ position.
		template <class FeatureType>
		struct SubCodeelement
		{
			int pos;
			//short freeFields;
			Codeelement<FeatureType>* ce;
			SubCodeelement<FeatureType>* next;
		};

		template <class FeatureType>
		struct CodeelementRef
		{
			Island island;
			Codeelement<FeatureType>* ce;
		};


		/*! A codeelement... (TODO: better documentation) */
		template <class FeatureType>
		struct Codeelement
		{
			unsigned char form,numOfSubCEs,flags;
			FeatureType data;
#ifdef _CSC_STORE_ISLAND
			Island island;
#endif
#ifdef _CSC_STORE_RUNNING_NUMBER
			int runningNumber;
#endif
#ifdef _CSC_STORE_BOUNDING_BOX
			short bbLeft,bbRight,bbTop,bbBottom;
#endif
			Codeelement<FeatureType>* root;
			Codeelement<FeatureType>* parents[2];
			SubCodeelement<FeatureType>* subCEs;
			Codeelement<FeatureType>* next;
		};


		/*! \brief A index entry stores a number of codeelements belonging to a comman island I.*/
		template <class FeatureType>
		struct IndexEntry
		{
			//! Stores the number of codeelements of a the island I
			int numOfCEs;

			//! Points to the first codeelement of the island I.
			Codeelement<FeatureType>* firstCE;
			Codeelement<FeatureType>* lastCE;
		};


		// Forward-Declaration

		/*! \brief The codeelement base stores the segmentation result of the color structure code. 

		The class has several methods to access comfortably the codeelement graph. Further one can 
		simply transform the codeelement graph into another representation, for example, a region
		image. Everytime a programmer wants to access the codeelements directly he has to use the codeelement
		base. 

		\b Warning: Codeelements should not directly insert by the programmer.

		The class \c CodeelementBase is a template class that expect one template argument: \c FeatureType.
		\c FeatureType describes the structure of the data that a codeelement is able to store. \c FeatureType
		can be every type that allows a copy operation:
		\verbatim
		FeatureType a,b;
		a=b; // <--- Copy operation
		\endverbatim
		@author Patrick Sturm
		*/

		template <class FeatureType>
		struct CSCSegment
		{
			Island island;
			Codeelement<FeatureType>* ce;
			CSCSegment* next;
		};


		template <class FeatureType>
		class CodeelementBase
		{
		private:
			const IslandHierarchy* hierarchy;
			IndexEntry<FeatureType>* islandToCEs;
			Codeelement<FeatureType>* ces;
			SubCodeelement<FeatureType>* subCEs;
			int numOfCEs,numOfSubCEs,currentIsland;
			int maxNumOfCEs,maxNumOfSubCEs;
			double avgCEs;
			CSCSegment<FeatureType>* firstSegment;

			void drawRegionRecursive(const Island& island, Codeelement<FeatureType>* ce, const FeatureType& feature, int x, int y, int w, int h, FeatureType** image);
			void drawLabelRecursive(const Island& island, Codeelement<FeatureType>* ce, int label, int x, int y, int w, int h, int** image);

		public:
			CodeelementBase(const IslandHierarchy& islandHierarchy, double avgCEsPerIsland = 2.0/*, double overheadPerCE = 4.0*/);
			virtual ~CodeelementBase();

			inline double getAVGCapacityPerIsland() {return avgCEs; };

			inline void addNewSegment(const Island& island, Codeelement<FeatureType>* ce);
			inline void removeSegment(Codeelement<FeatureType>* ce);
			inline CSCSegment<FeatureType>* getFirstSegment() const { return firstSegment; };
			void freeSegmentList();
			void clearState(Codeelement<FeatureType>* ce);
			void reinit(double avgCEsPerIsland);
			void clearContent();
			inline void newIsland(const Island& island);
			inline void reopenIsland(const Island& island);
			inline void removeCodeelement(const Island& island, Codeelement<FeatureType>* ce);
			inline Codeelement<FeatureType>* getCurrentCE() const;
			inline SubCodeelement<FeatureType>* getCurrentSubCEs() const;
			inline void storeCurrentCE(bool storeSubCEs = true);
			inline void storeCurrentSubCEs(int n);
			inline void removeSubCE(Codeelement<FeatureType>* ce, Codeelement<FeatureType>* subCE);
			inline SubCodeelement<FeatureType>* findSubCE(Codeelement<FeatureType>* parent, Codeelement<FeatureType>* subCE);
			inline Codeelement<FeatureType>* findRootCE(int x, int y, Island& rootIsland);

			Codeelement<FeatureType>* getCodeelements(const Island& island, int& count) const;
			void getCodeelements(const Island* islands, int numOfIslands, Codeelement<FeatureType>** ces, int* ceCounts) const;

			const IslandHierarchy& getHierarchy() const;

			void drawRegionImage(int x, int y, int w, int h, FeatureType** image, int minLevel=0/*, bool topDown = true*/);
			void drawRegion(const Island& island, Codeelement<FeatureType>* ce, int x, int y, int w, int h, FeatureType** image);
			void drawLabelImage(int x, int y, int w, int h, int** image, std::map<int,CodeelementRef<FeatureType> >& ceMap, int minLevel=0);
			void drawLabelImage(int x, int y, int w, int h, int** image, std::map<int,FeatureType>& featureMap, int minLevel=0);
			void drawRegion(const Island& island, Codeelement<FeatureType>* ce, int label, int x, int y, int w, int h, int** image);
			void drawBinaryRecursive(const Island& island, Codeelement<FeatureType>* ce, int x, int y, int w, int h, unsigned char** image);
			void drawBinaryImage(int x, int y, int w, int h, CSCSegment<FeatureType>* segment, unsigned char** image, int minLevel=0);

			void drawRegionImage(Image& img, int minLevel=0);
			void drawLabelImage(Image& img, std::map<int,CodeelementRef<FeatureType> >& ceMap, int minLevel=0);
			void drawLabelImage(Image& img, std::map<int,FeatureType>& featureMap, int minLevel=0);
			void drawBinaryImage(Image& img, CSCSegment<FeatureType>* segment, unsigned char** image, int minLevel=0);

			inline void findStartPoint(Island& island, Codeelement<FeatureType>* ce, Island& startIsl,
					Codeelement<FeatureType>*& startCE, int& startX, int& startY, int& startPos);
			inline int computeFreemanCode(CSCSegment<FeatureType>* segment, int& startX, int& startY, unsigned char* chainCodes);
		};


		// Implementation of CodeelementBase
		template <class FeatureType>
		CodeelementBase<FeatureType>::CodeelementBase(const IslandHierarchy &islandHierarchy, double avgCEsPerIsland/*, double overheadPerCE*/)
		{
			kiplAssert(avgCEsPerIsland>=1.0);

			firstSegment=NULL;
			hierarchy=&islandHierarchy;
			avgCEs=avgCEsPerIsland;
			maxNumOfCEs=(int)(avgCEsPerIsland*hierarchy->numberOfIslands());
			maxNumOfSubCEs=2*maxNumOfCEs;
			numOfCEs=0;
			numOfSubCEs=0;
			currentIsland=0;
			islandToCEs=new IndexEntry<FeatureType>[hierarchy->maxIslandKey()+1];
			ces=new Codeelement<FeatureType>[maxNumOfCEs];
			subCEs=new SubCodeelement<FeatureType>[maxNumOfSubCEs];
			kiplAssert(islandToCEs!=NULL && ces!=NULL && subCEs!=NULL);
		}


		template <class FeatureType>
		void CodeelementBase<FeatureType>::reinit(double avgCEsPerIsland)
		{
			kiplAssert(avgCEsPerIsland>=1.0);
			avgCEs=avgCEsPerIsland;
			maxNumOfCEs=(int)(avgCEsPerIsland*hierarchy->numberOfIslands());
			maxNumOfSubCEs=2*maxNumOfCEs;
			numOfCEs=0;
			numOfSubCEs=0;
			currentIsland=0;
			if (subCEs!=NULL) delete[] subCEs;
			if (ces!=NULL) delete[] ces;
			if (islandToCEs) delete[] islandToCEs;
			ces=new Codeelement<FeatureType>[maxNumOfCEs];
			subCEs=new SubCodeelement<FeatureType>[maxNumOfSubCEs];
			kiplAssert(ces!=NULL && subCEs!=NULL);
			islandToCEs=new IndexEntry<FeatureType>[hierarchy->maxIslandKey()+1];
			freeSegmentList();
		}


		template <class FeatureType>
		void CodeelementBase<FeatureType>::freeSegmentList()
		{
			CSCSegment<FeatureType>* delSeg;
			while (firstSegment!=NULL)
			{
				delSeg=firstSegment;
				firstSegment=firstSegment->next;
				delete delSeg;
			}
		}


		template <class FeatureType>
		CodeelementBase<FeatureType>::~CodeelementBase()
		{
			freeSegmentList();
			if (subCEs!=NULL) delete[] subCEs;
			if (ces!=NULL) delete[] ces;
			if (islandToCEs!=NULL) delete[] islandToCEs;
		}


		template <class FeatureType>
		void CodeelementBase<FeatureType>::clearContent()
		{
			numOfCEs=0;
			numOfSubCEs=0;
			currentIsland=0;
			freeSegmentList();
		}


		template <class FeatureType>
		inline void CodeelementBase<FeatureType>::addNewSegment(const Island& island, Codeelement<FeatureType>* ce)
		{
			CSCSegment<FeatureType>* newSegment = new CSCSegment<FeatureType>();

			kiplAssert(newSegment!=NULL);
			newSegment->island=island;
			newSegment->ce=ce;
			newSegment->next=firstSegment;
			firstSegment=newSegment;
			/*if (firstSegment==NULL)
			{
			firstSegment=newSegment;
			firstSegment->next=NULL;
			}
			else
			{
			if (island.level>=firstSegment->island.level)
			{
			newSegment->next=firstSegment;
			firstSegment=newSegment;
			}
			else
			{ 
			CSCSegment<FeatureType>* tmp = firstSegment;
			while (tmp->next!=NULL && tmp->next->island.level>island.level)
			tmp=tmp->next;  
			newSegment->next=tmp->next;
			tmp->next=newSegment;
			}
			}*/
		}


		template <class FeatureType>
		inline void CodeelementBase<FeatureType>::removeSegment(Codeelement<FeatureType>* ce)
		{
			CSCSegment<FeatureType>* delSeg;

			if (firstSegment!=NULL)
			{
				if (firstSegment->ce==ce) 
				{
					delSeg=firstSegment;
					firstSegment=firstSegment->next;
					delete delSeg;
				}
				else
				{
					CSCSegment<FeatureType>* tmp = firstSegment;
					while (tmp->next!=NULL && tmp->next->ce!=ce)
						tmp=tmp->next;
					if (tmp->next!=NULL) 
					{
						CSCSegment<FeatureType>* delSeg = tmp->next;
						tmp->next=tmp->next->next;
						delete delSeg;
					}
				}
			}
		}


		template <class FeatureType>
		inline void CodeelementBase<FeatureType>::newIsland(const Island& island)
		{
			currentIsland=hierarchy->getIslandKey(island);
			islandToCEs[currentIsland].numOfCEs=0;
			islandToCEs[currentIsland].firstCE=NULL;
			islandToCEs[currentIsland].lastCE=NULL;
		}


		template <class FeatureType>
		inline Codeelement<FeatureType>* CodeelementBase<FeatureType>::getCurrentCE() const
		{
			if (numOfCEs<maxNumOfCEs && numOfSubCEs<maxNumOfSubCEs)
			{
				Codeelement<FeatureType>* nextCE = &(ces[numOfCEs]);
				nextCE->numOfSubCEs=(maxNumOfSubCEs-numOfSubCEs>255) ? 255 : maxNumOfSubCEs-numOfSubCEs;
				nextCE->subCEs=&(subCEs[numOfSubCEs]);
				nextCE->next=NULL;

				return nextCE;
			}
			return NULL;
		}


		template <class FeatureType>
		inline void CodeelementBase<FeatureType>::reopenIsland(const Island& island)
		{
			currentIsland=hierarchy->getIslandKey(island);
		}


		template <class FeatureType>
		inline void CodeelementBase<FeatureType>::storeCurrentCE(bool storeSubCEs)
		{
			Codeelement<FeatureType>* currentCE = &(ces[numOfCEs]);

			//std::cout << "Store " << " " << currentIsland << " " << numOfCEs << "(" << maxNumOfCEs << ")" << std::endl;
			kiplAssert(currentIsland>=0);
			kiplAssert(numOfCEs<maxNumOfCEs);
			if (islandToCEs[currentIsland].firstCE==NULL)
			{
				islandToCEs[currentIsland].firstCE=currentCE;
				islandToCEs[currentIsland].lastCE=currentCE;
			}
			else
			{
				islandToCEs[currentIsland].lastCE->next=currentCE;
				islandToCEs[currentIsland].lastCE=currentCE;
			}
			currentCE->next=NULL;
#ifdef _CSC_STORE_RUNNING_NUMBER
			currentCE->runningNumber=numOfCEs;
#endif
			islandToCEs[currentIsland].numOfCEs++;
			numOfCEs++;

			if (storeSubCEs)
			{
				int n=currentCE->numOfSubCEs;
				kiplAssert(numOfSubCEs+n<=maxNumOfSubCEs);
				for (int i=1; i<n; i++)
					currentCE->subCEs[i-1].next=&(currentCE->subCEs[i]);
				currentCE->subCEs[n-1].next=NULL;
				numOfSubCEs+=n;
			}
		}


		template <class FeatureType>
		inline SubCodeelement<FeatureType>* CodeelementBase<FeatureType>::getCurrentSubCEs() const
		{
			return &(subCEs[numOfSubCEs]);
		}


		template <class FeatureType>
		inline void CodeelementBase<FeatureType>::storeCurrentSubCEs(int n)
		{
			kiplAssert(numOfSubCEs+n<=maxNumOfSubCEs);
			numOfSubCEs+=n;
		}

		template <class FeatureType>
		inline void CodeelementBase<FeatureType>::removeSubCE(Codeelement<FeatureType>* ce, Codeelement<FeatureType>* subCE)
		{
			SubCodeelement<FeatureType>* current = ce->subCEs;

			if (current->ce==subCE)
			{
				ce->subCEs=ce->subCEs->next;
				ce->numOfSubCEs--;
				if (subCE->parents[0]==ce) subCE->parents[0]=NULL;
				else if (subCE->parents[1]==ce) subCE->parents[1]=NULL;
			}
			else 
			{
				while (current->next!=NULL && current->next->ce!=subCE)
					current=current->next;
				if (current->next!=NULL) 
				{
					current->next=current->next->next;
					ce->numOfSubCEs--;
					if (subCE->parents[0]==ce) subCE->parents[0]=NULL;
					else if (subCE->parents[1]==ce) subCE->parents[1]=NULL;
				}
			}
		}


		template <class FeatureType>
		inline void CodeelementBase<FeatureType>::removeCodeelement(const Island& island, Codeelement<FeatureType>* ce)
		{
			int islandKey = hierarchy->getIslandKey(island);
			Codeelement<FeatureType>* searchCE = islandToCEs[islandKey].firstCE;

			if (ce==searchCE)
			{
				islandToCEs[islandKey].firstCE=searchCE->next;
				if (islandToCEs[islandKey].firstCE==NULL) 
					islandToCEs[islandKey].lastCE=NULL;
				islandToCEs[islandKey].numOfCEs--;
			}
			else
			{
				while (searchCE->next!=ce)
				{
					searchCE=searchCE->next;
					kiplAssert(searchCE!=NULL);
				}
				searchCE->next=ce->next;
				ce->next=NULL;
				if (ce==islandToCEs[islandKey].lastCE) 
					islandToCEs[islandKey].lastCE=searchCE;
				islandToCEs[islandKey].numOfCEs--;
			}
		}


		template <class FeatureType>
		inline SubCodeelement<FeatureType>* CodeelementBase<FeatureType>::findSubCE(Codeelement<FeatureType>* parent, Codeelement<FeatureType>* subCE)
		{
			SubCodeelement<FeatureType>* current = parent->subCEs;

			while (current!=NULL)
			{
				if (current->ce==subCE) return current;
				current=current->next;
			}
			return NULL;
		}


		template <class FeatureType>
		inline Codeelement<FeatureType>* CodeelementBase<FeatureType>::findRootCE(int x, int y, Island& rootIsland)
		{
			Island parentIslands[2];
			int subPos[2];
			int numOfParents = hierarchy->getParentIslands(x,y,subPos,parentIslands);
			Codeelement<FeatureType>* ce;
			int count,pos;

			for (int i=0; i<numOfParents; i++)
			{
				ce=getCodeelements(parentIslands[i],count);
				pos=1<<subPos[i];
				while (ce!=NULL)
				{
					if (ce->form & pos)
					{
						rootIsland=parentIslands[i];
						while (ce->parents[0] || ce->parents[1])
						{
							SubCodeelement<FeatureType>* subCE;

							if (ce->parents[0])
							{
								subCE=findSubCE(ce->parents[0],ce);
								kiplAssert(subCE!=NULL);
								hierarchy->getParentIsland(rootIsland,subCE->pos,rootIsland);
								ce=ce->parents[0];
							}
							else
							{
								subCE=findSubCE(ce->parents[1],ce);
								kiplAssert(subCE!=NULL);
								hierarchy->getParentIsland(rootIsland,subCE->pos,rootIsland);
								ce=ce->parents[1];
							}
						}
						return ce;
					}
					ce=ce->next;
				}
			}
			return NULL;
		}


		template <class FeatureType>
		void CodeelementBase<FeatureType>::getCodeelements(const Island* islands, int numOfIslands, Codeelement<FeatureType>** ces, int* ceCounts) const
		{
			for (int i=0; i<numOfIslands; i++)
			{
				int islandKey = hierarchy->getIslandKey(islands[i]);

				if (islandKey>=0)
				{
					ceCounts[i]=islandToCEs[islandKey].numOfCEs;
					ces[i]=islandToCEs[islandKey].firstCE;
				}
				else
				{
					ceCounts[i]=0;
					ces[i]=NULL;
				}
			}
		}


		template <class FeatureType>
		Codeelement<FeatureType>* CodeelementBase<FeatureType>::getCodeelements(const Island& island, int& count) const
		{
			int islandKey = hierarchy->getIslandKey(island);

			if (islandKey>=0)
			{
				count=islandToCEs[islandKey].numOfCEs;
				return islandToCEs[islandKey].firstCE;
			}
			count=0;
			return NULL;
		}


		template <class FeatureType>
		const IslandHierarchy& CodeelementBase<FeatureType>::getHierarchy() const
		{
			return *hierarchy;
		}


		template <class FeatureType>
		void CodeelementBase<FeatureType>::clearState(Codeelement<FeatureType>* ce)
		{
			if (ce->flags & CEVisited)
			{
				ce->flags&=~CEVisited;
				SubCodeelement<FeatureType>* subCE = ce->subCEs;
				while (subCE!=NULL)
				{
					clearState(subCE->ce);
					subCE=subCE->next;
				}
			}
		}


		template <class FeatureType>
		void CodeelementBase<FeatureType>::drawBinaryRecursive(const Island& island, Codeelement<FeatureType>* ce, int x, int y, int w, int h, unsigned char** image)
		{
			ce->flags|=CEVisited;

			if (island.level>0)
			{
				Island subIslands[7];
				SubCodeelement<FeatureType>* subCE;

				hierarchy->getSubIslands(island,subIslands);
				subCE=ce->subCEs;
				while (subCE!=NULL)
				{
					if ((subCE->ce->flags & CEVisited)==0)
						drawBinaryRecursive(subIslands[subCE->pos],subCE->ce,x,y,w,h,image);
					subCE=subCE->next;
				}
			}
			else
			{
				int xcoord[7],ycoord[7];

				hierarchy->getSubPixels(island,xcoord,ycoord);
				for (int i=0; i<7; i++)
					if ((ce->form & (1<<i)) &&
						xcoord[i]>=x && xcoord[i]<x+w &&
						ycoord[i]>=y && ycoord[i]<y+h)
						image[ycoord[i]][xcoord[i]]=255;
			}
		}


		template <class FeatureType>
		void CodeelementBase<FeatureType>::drawRegionRecursive(const Island& island, Codeelement<FeatureType>* ce, const FeatureType& feature, int x, int y, int w, int h, FeatureType** image)
		{
			ce->flags|=CEVisited;

			if (island.level>0)
			{
				Island subIslands[7];
				SubCodeelement<FeatureType>* subCE;

				hierarchy->getSubIslands(island,subIslands);
				subCE=ce->subCEs;
				while (subCE!=NULL)
				{
					if ((subCE->ce->flags & CEVisited)==0)
						drawRegionRecursive(subIslands[subCE->pos],subCE->ce,feature,x,y,w,h,image);
					subCE=subCE->next;
				}
			}
			else
			{
				int xcoord[7],ycoord[7];

				hierarchy->getSubPixels(island,xcoord,ycoord);
				for (int i=0; i<7; i++)
					if ((ce->form & (1<<i)) &&
						xcoord[i]>=x && xcoord[i]<x+w &&
						ycoord[i]>=y && ycoord[i]<y+h)
						image[ycoord[i]][xcoord[i]]=feature;
			}
		}


		template <class FeatureType>
		void CodeelementBase<FeatureType>::drawLabelRecursive(const Island& island, Codeelement<FeatureType>* ce, int label, int x, int y, int w, int h, int** image)
		{
			ce->flags|=CEVisited;

			if (island.level>0)
			{
				Island subIslands[7];
				SubCodeelement<FeatureType>* subCE;

				hierarchy->getSubIslands(island,subIslands);
				subCE=ce->subCEs;
				while (subCE!=NULL)
				{
					if ((subCE->ce->flags & CEVisited)==0)
						drawLabelRecursive(subIslands[subCE->pos],subCE->ce,label,x,y,w,h,image);
					subCE=subCE->next;
				}
			}
			else
			{
				int xcoord[7],ycoord[7];

				hierarchy->getSubPixels(island,xcoord,ycoord);
				for (int i=0; i<7; i++)
					if ((ce->form & (1<<i)) &&
						xcoord[i]>=x && xcoord[i]<x+w &&
						ycoord[i]>=y && ycoord[i]<y+h)
						image[ycoord[i]][xcoord[i]]=label;
			}
		}


		template <class FeatureType>
		void CodeelementBase<FeatureType>::drawBinaryImage(int x, int y, int w, int h, CSCSegment<FeatureType>* currSegment, unsigned char** image, int minLevel)
		{
			if (image!=NULL && currSegment)
			{
				int maxLevel=hierarchy->numberOfLevels();

				if (minLevel<=currSegment->island.level && currSegment->island.level<=maxLevel)
				{
					if (currSegment->ce->flags & CERoot)
					{
						drawBinaryRecursive(currSegment->island,currSegment->ce,x,y,w,h,image);
						clearState(currSegment->ce);
					}
					else
						currSegment=currSegment;
				}
			}
		}


		template <class FeatureType>
		void CodeelementBase<FeatureType>::drawRegionImage(int x, int y, int w, int h, FeatureType** image, int minLevel/*, bool topDown*/)
		{
			if (image!=NULL)
			{
				CSCSegment<FeatureType>* currSegment = firstSegment;

				int maxLevel=hierarchy->numberOfLevels();

				while (currSegment!=NULL)
				{
					if (minLevel<=currSegment->island.level && currSegment->island.level<=maxLevel)
					{
						if (currSegment->ce->flags & CERoot)
						{
							drawRegionRecursive(currSegment->island,currSegment->ce,currSegment->ce->data,x,y,w,h,image);
							clearState(currSegment->ce);
						}
					}
					currSegment=currSegment->next;
				}
			}
		}


		template <class FeatureType>
		void CodeelementBase<FeatureType>::drawRegion(const Island& island, Codeelement<FeatureType>* ce, int x, int y, int w, int h, FeatureType** image)
		{
			if (image!=NULL && ce!=NULL)
			{
				drawRegionRecursive(island,ce,ce->data,x,y,w,h,image);
				clearState(ce);
			}
		}

		template <class FeatureType>
		void CodeelementBase<FeatureType>::drawRegion(const Island& island, Codeelement<FeatureType>* ce, int label, int x, int y, int w, int h, int** image)
		{
			if (image!=NULL && ce!=NULL)
			{
				drawLabelRecursive(island,ce,label,x,y,w,h,image);
				clearState(ce);
			}
		}


		template <class FeatureType>
		void CodeelementBase<FeatureType>::drawLabelImage(int x, int y, int w, int h, int** image, std::map<int,CodeelementRef<FeatureType> >& ceMap, int minLevel)
		{
			if (image!=NULL)
			{
				CSCSegment<FeatureType>* currSegment;
				int nextLabel=1;
				CodeelementRef<FeatureType> ceRef;

				ceMap.clear();
				int maxLevel=hierarchy->numberOfLevels();

				currSegment=firstSegment;
				while (currSegment!=NULL)
				{
					if (minLevel<=currSegment->island.level && currSegment->island.level<=maxLevel && (currSegment->ce->flags & CERoot))
					{
						drawLabelRecursive(currSegment->island,currSegment->ce,nextLabel,x,y,w,h,image);
						ceRef.island=currSegment->island;
						ceRef.ce=currSegment->ce;
						ceMap[nextLabel]=ceRef;
						nextLabel++;
						clearState(currSegment->ce);
					}
					currSegment=currSegment->next;
				}
			}
		}


		template <class FeatureType>
		void CodeelementBase<FeatureType>::drawLabelImage(int x, int y, int w, int h, int** image, std::map<int,FeatureType>& featureMap, int minLevel)
		{
			std::map<int,CodeelementRef<FeatureType> > ceMap;

			featureMap.clear();
			drawLabelImage(x,y,w,h,image,ceMap,minLevel);
			typename std::map<int,CodeelementRef<FeatureType> >::iterator end = ceMap.end();

			for (typename std::map<int,CodeelementRef<FeatureType> >::iterator current = ceMap.begin(); current!=end; current++)
				featureMap[(*current).first]=(*current).second.ce->data;
		}


		template <class FeatureType>
		void CodeelementBase<FeatureType>::drawRegionImage(Image& img, int minLevel) {
			const ROI& roi = img.getROI();
			if (roi.isValid()) {
				FeatureType** idx = (FeatureType**)(img.getSubImageIndex()[0]);
				int x = (int)roi.left;
				int y = (int)roi.top;
				int w = (int)roi.width();
				int h = (int)roi.height();
				img.clear();
				drawRegionImage(x,y,w,h,idx,minLevel);
			}
		}


		template <class FeatureType>
		void CodeelementBase<FeatureType>::drawLabelImage(Image& img, std::map<int,CodeelementRef<FeatureType> >& ceMap, int minLevel) {
			const ROI& roi = img.getROI();
			if (roi.isValid()) {
				int** idx = (int**)(img.getSubImageIndex()[0]);
				int x = (int)roi.left;
				int y = (int)roi.top;
				int w = (int)roi.width();
				int h = (int)roi.height();
				img.clear();
				drawLabelImage(x,y,w,h,idx,ceMap,minLevel);
			}
		}

		template <class FeatureType>
		void CodeelementBase<FeatureType>::drawLabelImage(Image& img, std::map<int,FeatureType>& featureMap, int minLevel) {
			const ROI& roi = img.getROI();
			if (roi.isValid()) {
				int** idx = (int**)(img.getSubImageIndex()[0]);
				int x = (int)roi.left;
				int y = (int)roi.top;
				int w = (int)roi.width();
				int h = (int)roi.height();
				img.clear();
				drawLabelImage(x,y,w,h,idx,featureMap,minLevel);
			}
		}

		template <class FeatureType>
		void CodeelementBase<FeatureType>::drawBinaryImage(Image& img, CSCSegment<FeatureType>* segment, unsigned char** image, int minLevel) {
			const ROI& roi = img.getROI();
			if (roi.isValid()) {
				unsigned char** idx = (unsigned char**)(img.getSubImageIndex()[0]);
				int x = (int)roi.left;
				int y = (int)roi.top;
				int w = (int)roi.width();
				int h = (int)roi.height();
				img.clear();
				drawBinaryImage(x,y,w,h,segment,idx,minLevel);
			}
		}


		template <class FeatureType>
		inline void CodeelementBase<FeatureType>::findStartPoint(Island& island, Codeelement<FeatureType>* ce,
			Island& startIsl,
			Codeelement<FeatureType>*& startCE,
			int& startX, int& startY,
			int& startPos)
		{
			ce->flags|=CEVisited;

			if (island.level>0)
			{
				Island subIslands[7];
				SubCodeelement<FeatureType>* subCE;

				hierarchy->getSubIslands(island,subIslands);
				subCE=ce->subCEs;
				while (subCE!=NULL)
				{
					if ((subCE->ce->flags & CEVisited)==0)
						findStartPoint(subIslands[subCE->pos],subCE->ce,startIsl,startCE,startX,startY,startPos);
					subCE=subCE->next;
				}
			}
			else
			{
				int x,y,pos;

				hierarchy->getMinCoord(island,ce->form,x,y,pos);
				if (startCE==NULL || y<startY || (y==startY && x<startX))
				{
					startIsl=island;
					startX=x;
					startY=y;
					startPos=pos;
					startCE=ce;
				}
			}
		}


		template <class FeatureType>
		inline int CodeelementBase<FeatureType>::computeFreemanCode(CSCSegment<FeatureType>* segment, int& startX, int& startY, unsigned char* chainCodes)
		{
			int numOfChainCodes = 0;
			static const int code6ToCode8[7][7] = { 
				{-1,0,5,6,3,2,4}, // Even
				{-1,0,5,6,3,2,4}, // Even
				{-1,0,6,7,2,1,4}, // Odd
				{-1,0,6,7,2,1,4}, // Odd
				{-1,0,6,7,2,1,4}, // Odd
				{-1,0,6,7,2,1,4}, // Odd
				{-1,0,5,6,3,2,4}  // Even 
			};

			static const int neighborPixel[7][7] = 
			{
				{ 0, 1, 2, 3, 4, 5, 6}, // Direction 0
				{ 1,-1, 3,-1, 5,-1, 0}, // Direction 1
				{ 2, 3,-1,-1, 6, 0,-1}, // Direction 2
				{ 3,-1,-1,-1, 0, 1, 2}, // Direction 3
				{ 4, 5, 6, 0,-1,-1,-1}, // Direction 4
				{ 5,-1, 0, 1,-1,-1, 4}, // Direction 5
				{ 6, 0,-1, 2,-1, 4,-1}  // Direction 6
			};

			static const int level0DeltaX[7] = { 0,2,-1,1,-1, 1,-2 };
			static const int level0DeltaY[7] = { 0,0, 2,2,-2,-2, 0 };
			static const int level0Pos[7] = {0,6,5,4,3,2,1};

			static const int nextDirection[7]  = {1,3,6,2,5,1,4};
			static const int startDirection[7] = {1,5,3,1,6,4,2};

			Island island,newIsland,startIsl;
			Codeelement<FeatureType> *ce,*startCE=NULL;
			unsigned char form,newForm;
			int pos,newPos,currDirection,count,startPos;

			startCE=NULL;
			findStartPoint(segment->island,segment->ce,startIsl,startCE,startX,startY,startPos);
			if (startPos==1 || startPos==3 || startPos==5)
			{
				startIsl.x+=level0DeltaX[startPos];
				startIsl.y+=level0DeltaY[startPos];
				startPos=level0Pos[startPos];
			}

			form=0;
			ce=getCodeelements(startIsl,count);
			while (ce!=NULL)
			{
				if (ce->flags & CEVisited) form|=ce->form;
				ce=ce->next;
			}

			pos=startPos;
			island=startIsl;
			currDirection=startDirection[0];
			do
			{
				newPos=neighborPixel[currDirection][pos];
				if (newPos>=0)
				{
					if (form & (1<<newPos))
					{
						chainCodes[numOfChainCodes]=code6ToCode8[pos][currDirection];
						numOfChainCodes++;
						pos=newPos;
						currDirection=startDirection[currDirection];
					}
					else
					{
						if (level0Pos[newPos]>=0)
						{
							newIsland.level=0;
							newIsland.x=island.x+level0DeltaX[newPos];
							newIsland.y=island.y+level0DeltaY[newPos];
							newPos=level0Pos[newPos];
							ce=getCodeelements(newIsland,count);
							newForm=0;
							while (ce!=NULL)
							{
								if (ce->flags & CEVisited) newForm|=ce->form;
								ce=ce->next;
							}
							if (newForm & (1<<newPos)) 
							{
								chainCodes[numOfChainCodes]=code6ToCode8[pos][currDirection];
								numOfChainCodes++;
								form=newForm;
								pos=newPos;
								island=newIsland;
								currDirection=startDirection[currDirection];
							}
							else currDirection=nextDirection[currDirection];
						}
						else currDirection=nextDirection[currDirection];
					}
				}
				else
				{
					island.level=0;
					island.x+=level0DeltaX[pos];
					island.y+=level0DeltaY[pos];
					pos=level0Pos[pos];
					ce=getCodeelements(island,count);

					form=1<<pos;
					while (ce!=NULL)
					{
						if (ce->flags & CEVisited) form|=ce->form;
						ce=ce->next;
					}
				}
			} while (startIsl.x!=island.x || startIsl.y!=island.y || startPos!=pos || currDirection!=startDirection[0]);
			return numOfChainCodes;
		}


	}
}
#endif
