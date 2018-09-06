/* 

This "SOFTWARE" is a free software.

You are allowed to download, use, modify and redistribute this software.
The software is provided "AS IS" without warranty of any kind.

Copyright: University of Koblenz, Lutz Priese, Volker Rehrman, Patrick Sturm

*/


#ifndef _cscengine_h_
#define _cscengine_h_

/***********************************************
iMAge pROcessing cLAsses  Version 1.0

Copyright 2002, Patrick Sturm
************************************************/

// Include-Files
#include <kipl-base/error.h>
#include <kipl-csc/codebase.h>
#include <kipl-util/queue.h>
#include <kipl-base/image.h>
#include <iostream>


// Exceptions

namespace kipl {
	namespace csc {
		KIPL_NEW_STD_EXCEPTION(ECSCException,"CSC failed...")
			KIPL_NEW_SUB_EXCEPTION(ECodeelementBaseOverrun,ECSCException,"Codeelement overrun...")

			//! Symbolic names for different splitting algorithms
			enum SplittingMethod {None=0, DirectSplitting=1, DeferredSplitting=2/*, DirectSplittingRecursive*/};

		//! Symbolic names for different repair alhorithms
		enum RepairMethod {NoRepair=0, FastRepair=1, PreciseRepair=2};

		template <class FeatureType>
		struct UpdateFeature
		{
			int level;
			Codeelement<FeatureType>* ce;
		};


		template <class FeatureType>
		struct BestNeighborItem
		{
			int memberPos,neighborPos;
			Codeelement<FeatureType>* member;
			Codeelement<FeatureType>* neighbor;
		};


		template <class FeatureType>
		struct SplittingPair
		{
			Codeelement<FeatureType> *root1;
			Codeelement<FeatureType> *root2;
			Codeelement<FeatureType> *ce1,*ce2;
			Island island;
			int relPos1,relPos2;
		};


		template <class FeatureType>
		struct RepairItem
		{
			Island island;
			Codeelement<FeatureType> *ce;
		};


		template <class PixelType, class FeatureType, class LinkControl>
		class CSCEngine
		{
		private:
			static const int startPositions[2][7];

		private:
			const IslandHierarchy* hierarchy;
			CodeelementBase<FeatureType>* ceBase;
			BottomUpIslandIterator* islandIterator;
			LinkControl* linkControl;
			int roiX1,roiY1,roiX2,roiY2;
			SplittingMethod splittingMethod;
			Queue<SplittingPair<FeatureType> > splittingQueue;
			Queue<RepairItem<FeatureType> > repairQueue;
			int maxCEQueue,maxCEQueueBestNeighbor,disintegratedCEs;
			SubCodeelement<FeatureType>* ceQueue;
			BestNeighborItem<FeatureType>* ceQueueBestNeighbor;
			RepairMethod repairMethod;
			bool updateAfterSplitting,bestNeighbor;
			UpdateFeature<FeatureType>* updateQueue;
			int maxUpdateQueue,startPosition,maxLinkLevel;

			inline void updateFeature(int level, Codeelement<FeatureType>* ce);
			void coding(Image& img);
			void level0Linking();
			void level0LinkingBestNeighbor();
			void levelNLinking();
			void levelNLinkingBestNeighbor();
			void deferredSplitting();
			void directSplitting();
			void storeSegments();

			bool testConnectivityIf(Codeelement<FeatureType>* rootCE, Codeelement<FeatureType>* removedCE);
			bool testLevel1ConnectivityIf(Codeelement<FeatureType>* rootCE, Codeelement<FeatureType>* removedCE);
			void repair(Codeelement<FeatureType>* rootCE, SubCodeelement<FeatureType>* subCEList, int &numOfConnected, SubCodeelement<FeatureType>* &connectedList, unsigned char& connectedForm, int &numOfRest, SubCodeelement<FeatureType>* &restList);
			void level1Repair(Codeelement<FeatureType>* rootCE, SubCodeelement<FeatureType>* subCEList, int &numOfConnected, SubCodeelement<FeatureType>* &connectedList, unsigned char& connectedForm, int &numOfRest, SubCodeelement<FeatureType>* &restList);
			int repair(const Island& island, Codeelement<FeatureType>* rootCE);
#ifdef DEBUG
			bool testConnectivity(Codeelement<FeatureType>* rootCE);
			bool testLevel1Connectivity(Codeelement<FeatureType>* rootCE);
			void testConnectivity();
#endif

		public:
			CSCEngine(CodeelementBase<FeatureType>& newCEBase, LinkControl* newLinkControl);
			virtual ~CSCEngine();

			void enableUpdateAfterSplitting(bool enable = true) { updateAfterSplitting=enable; };
			void enableBestNeighbor(bool enable = true);
			void changeSplittingMethod(SplittingMethod newSplittingMethod) { splittingMethod=newSplittingMethod; };
			void changeRepairMethod(RepairMethod newRepairMethod) {repairMethod=newRepairMethod;};
			void changeCEBase(CodeelementBase<FeatureType>& newCEBase);
			void changeStartPosition(int newStartPosition);
			void startSegmentation(Image& img);
			LinkControl& getLinkControl() { return (*linkControl); };

			inline int getNumOfDisintegratedCEs() const { return disintegratedCEs; }
			inline void setMaxLinkLevel(int linkLevel=-1) { maxLinkLevel=linkLevel; }
		};


		template <class PixelType, class FeatureType, class LinkControl>
		const int CSCEngine<PixelType,FeatureType,LinkControl>::startPositions[2][7] = {{0,1,2,3,4,5,6},{1,2,3,4,5,6,0}};


		// Implementation
		template <class PixelType, class FeatureType, class LinkControl>
		CSCEngine<PixelType,FeatureType,LinkControl>::CSCEngine(CodeelementBase<FeatureType>& newCEBase, LinkControl* newLinkControl): linkControl(newLinkControl), splittingQueue(2000,2000), repairQueue(2000,2000)
		{
			kiplAssert(linkControl!=NULL);
			islandIterator=NULL;
			changeCEBase(newCEBase);
			maxCEQueue=2000;
			maxCEQueueBestNeighbor=2000;
			ceQueue=new SubCodeelement<FeatureType>[maxCEQueue];
			ceQueueBestNeighbor=new BestNeighborItem<FeatureType>[maxCEQueueBestNeighbor];
			//ceQueueBestNeighbor=NULL;
			disintegratedCEs=0;
			splittingMethod=DirectSplitting;
			repairMethod=PreciseRepair;
			updateAfterSplitting=false;
			bestNeighbor=false;
			maxUpdateQueue=2000;
			updateQueue=new UpdateFeature<FeatureType>[maxUpdateQueue];
			startPosition=0;
			maxLinkLevel=-1;
		}


		template <class PixelType, class FeatureType, class LinkControl>
		void CSCEngine<PixelType,FeatureType,LinkControl>::changeStartPosition(int newStartPosition)
		{
			if (newStartPosition<0) startPosition=0;
			else if (newStartPosition>1) startPosition=1;
			else startPosition=newStartPosition;
		}


		template <class PixelType, class FeatureType, class LinkControl>
		void CSCEngine<PixelType,FeatureType,LinkControl>::enableBestNeighbor(bool enable) 
		{ 
			if (enable)
			{
				bestNeighbor=true;
				/*if (ceQueue!=NULL)
				{
				delete[] ceQueue;
				ceQueue=NULL;
				}*/
				if (ceQueueBestNeighbor==NULL)  
					ceQueueBestNeighbor=new BestNeighborItem<FeatureType>[maxCEQueueBestNeighbor];
			}
			else
			{
				bestNeighbor=false;
				/*if (ceQueueBestNeighbor!=NULL)
				{
				delete[] ceQueueBestNeighbor;
				ceQueueBestNeighbor=NULL;
				}*/
				if (ceQueue==NULL) 
					ceQueue=new SubCodeelement<FeatureType>[maxCEQueue];
			}
		}


		template <class PixelType, class FeatureType, class LinkControl>
		CSCEngine<PixelType,FeatureType,LinkControl>::~CSCEngine()
		{
			if (islandIterator!=NULL) delete islandIterator;
			if (ceQueue!=NULL) delete[] ceQueue;
			if (ceQueueBestNeighbor!=NULL) delete[] ceQueueBestNeighbor;
			if (updateQueue!=NULL) delete[] updateQueue;
		}


		template <class PixelType, class FeatureType, class LinkControl>
		inline void CSCEngine<PixelType,FeatureType,LinkControl>::updateFeature(int level, Codeelement<FeatureType>* ce)
		{
			kiplAssert(level>0);
			if ((ce->flags & CEVisited)==0)
			{
				int nextUpdate,lastUpdate;
				SubCodeelement<FeatureType>* subCE;

				updateQueue[0].level=0;
				updateQueue[0].ce=ce;
				ce->flags|=CEVisited;
				nextUpdate=0;
				lastUpdate=1;
				while (nextUpdate<lastUpdate)
				{
					UpdateFeature<FeatureType>& updateFeature = updateQueue[nextUpdate];
					updateFeature.ce->flags&=~CEVisited;

					subCE=updateFeature.ce->subCEs;
					kiplAssert(subCE!=NULL);
					if (subCE!=NULL)
					{
						linkControl->begin(level,subCE->ce);
						subCE=subCE->next;
						while (subCE!=NULL)
						{
							linkControl->add(subCE->ce);
							subCE=subCE->next;
						}
					}
					linkControl->setFeature(updateFeature.ce->data);

					if (updateFeature.ce->parents[0] && (updateFeature.ce->flags & CEVisited)==0)
					{
						if (lastUpdate>=maxUpdateQueue)
						{
							maxUpdateQueue*=2;
							UpdateFeature<FeatureType>* newUpdateQueue=new UpdateFeature<FeatureType>[maxUpdateQueue];
							for (int j=0; j<lastUpdate; j++) newUpdateQueue[j]=updateQueue[j];
							delete[] updateQueue;
							updateQueue=newUpdateQueue;
						}
						updateQueue[lastUpdate].level=level+1;
						updateQueue[lastUpdate].ce=updateFeature.ce->parents[0];
						updateFeature.ce->parents[0]->flags|=CEVisited;
						lastUpdate++;
					}

					if (updateFeature.ce->parents[1] && (updateFeature.ce->flags & CEVisited)==0)
					{
						if (lastUpdate>=maxUpdateQueue)
						{
							maxUpdateQueue*=2;
							UpdateFeature<FeatureType>* newUpdateQueue=new UpdateFeature<FeatureType>[maxUpdateQueue];
							for (int j=0; j<lastUpdate; j++) newUpdateQueue[j]=updateQueue[j];
							delete[] updateQueue;
							updateQueue=newUpdateQueue;
						}
						updateQueue[lastUpdate].level=level+1;
						updateQueue[lastUpdate].ce=updateFeature.ce->parents[1];
						updateFeature.ce->parents[1]->flags|=CEVisited;
						lastUpdate++;
					}
					nextUpdate++;
				}
			}
		}            


		template <class PixelType, class FeatureType, class LinkControl>
		void CSCEngine<PixelType,FeatureType,LinkControl>::changeCEBase(CodeelementBase<FeatureType>& newCEBase)
		{
			hierarchy=&(newCEBase.getHierarchy());
			ceBase=&newCEBase;
			if (islandIterator!=NULL) delete islandIterator;
			islandIterator=hierarchy->createIteratorBU();
		}


		template <class PixelType, class FeatureType, class LinkControl>
		void CSCEngine<PixelType,FeatureType,LinkControl>::startSegmentation(Image& img)
		{
			roiX1=img.getROI().left;
			roiX2=img.getROI().right;
			roiY1=img.getROI().top;
			roiY2=img.getROI().bottom;

			int tryAgain = 4;

			// Correct region of interest
			if (roiX2>(int)img.getWidth()) roiX2=(int)img.getWidth();
			if (roiY2>(int)img.getHeight()) roiY2=(int)img.getHeight();

			// Reset the codebase
			ceBase->clearContent();

			disintegratedCEs=0;
			do
			{
				try
				{
					//std::cout << "Max Link Level = " << maxLinkLevel << std::endl;
					//std::cout << "Coding..." << std::endl;

					coding(img);
					if (maxLinkLevel==-1 || maxLinkLevel>0) 
					{
						if (bestNeighbor)
						{ 
							//std::cout << "Linking Level 0 (Best Neighbor)..." << std::endl;
							level0LinkingBestNeighbor();
							//std::cout << "Linking Level >0 (Best Neighbor)..." << std::endl;
							levelNLinkingBestNeighbor();
						}
						else 
						{
							//std::cout << "Linking Level 0..." << std::endl;
							level0Linking();
							//std::cout << "Linking Level >0..." << std::endl;
							levelNLinking();
						}
					}

					// Splitting
					if (splittingMethod==DeferredSplitting)
					{
						//std::cout << "Deferred Splitting..." << std::endl;
						deferredSplitting();
					}

					//std::cout << "Store Segments..." << std::endl;
					storeSegments();
					tryAgain=0;
#ifdef _DEBUG_DISINTEGRATION
					//std::cout << "Test Connectivity..." << std::endl;
					testConnectivity();
#endif
				}  
				catch (ECodeelementBaseOverrun&)
				{
					//std::cout << "CSC failed: reinit..." << std::endl;
					ceBase->reinit(2*ceBase->getAVGCapacityPerIsland());
					tryAgain--;
				}
			} while (tryAgain>0);
			//std::cout << "Segmentation done..." << std::endl;
		}


		template <class PixelType, class FeatureType, class LinkControl>
		void CSCEngine<PixelType,FeatureType,LinkControl>::coding(Image& img)
		{
			const NeighborTable& neighborTable = hierarchy->getNeighborTable();
			int xcoords[7],ycoords[7],pixQueue[225],pixelCount;
			const PixelType* values[7];
			bool used[7];
			int startPos,currPos,neighPos,i,j,inQueue,nextElem,remainingPixels;
			PixelType** idx = (PixelType**)(img.getSubImageIndex()[img.getROI().front]);


			if (islandIterator->begin())
				do
				{
					const Island& currIsland = islandIterator->currentIsland(); // Island to code...

					ceBase->newIsland(currIsland);

					// Get all pixels of the current island...
					hierarchy->getSubPixels(currIsland,xcoords,ycoords);

					remainingPixels=0;
					//if (xcoords[0]<=roiX2 || ycoords[0]<=roiY2)

					for (i=0; i<7; i++)
					{
						xcoords[i]+=roiX1;
						ycoords[i]+=roiY1;

						if (roiX1<=xcoords[i] && xcoords[i]<roiX2 && roiY1<=ycoords[i] && ycoords[i]<roiY2)
						{
							used[i]=false;
							//values[i]=(PixelType*)img.scanline(0,ycoords[i],xcoords[i]);
							values[i]=&idx[ycoords[i]][xcoords[i]];
							remainingPixels++;
						}
						else used[i]=true;
					}

					//for (startPos=0; (startPos<7) && (remainingPixels>=1); startPos++)
					for (j=0; j<7 && (remainingPixels>=1); j++)
					{
						startPos=startPositions[startPosition][j];
						if (!used[startPos])
						{
							Codeelement<FeatureType>* newCE = ceBase->getCurrentCE();

							if (newCE==NULL) throw ECodeelementBaseOverrun();

							newCE->subCEs=NULL;
							newCE->parents[0]=NULL;
							newCE->parents[1]=NULL;
							newCE->root=NULL;
							newCE->numOfSubCEs=0;
							newCE->flags=CERoot;
							newCE->form=(1<<startPos);
#ifdef _CSC_STORE_ISLAND
							newCE->island=currIsland;
#endif
							pixelCount=1;

							linkControl->begin(0,*values[startPos]);

							used[startPos]=true;
							pixQueue[0]=startPos;
							inQueue=1;
							nextElem=0;
							remainingPixels--;

							while (nextElem<inQueue)
							{
								currPos=pixQueue[nextElem]; 
								nextElem++;  // Extract next position form queue
								for (i=0; neighborTable[currPos][i]!=-1; i++) // Extract all neighbors from queue
								{
									neighPos=neighborTable[currPos][i];
									if (!used[neighPos])
									{
										if (linkControl->add(*values[currPos],*values[neighPos]))
										{
											used[neighPos]=true;
											pixQueue[inQueue]=neighPos;
											inQueue++;
											newCE->form|=(1<<neighPos);
											pixelCount++;
											remainingPixels--;
											if (remainingPixels==0) break;
										}
									}
								}
								if (remainingPixels==0) break;
							}
							if (pixelCount>1)
							{
								linkControl->setFeature(newCE->data);
								ceBase->storeCurrentCE(false);
							}
							else remainingPixels++;
						}
					}
				} while (islandIterator->nextIsland());
		}


		template <class PixelType, class FeatureType, class LinkControl>
		void CSCEngine<PixelType,FeatureType,LinkControl>::level0Linking()
		{
			const NeighborTable& neighborTable = hierarchy->getNeighborTable();
			const CommonIslTable& commonIslTable = hierarchy->getCommonIslandTable(); 
			//SubCodeelement<FeatureType> ceQueue[11025]; // 7 (Islands) * 7 (Max. Codeelements per Island) = 49
			Island subIslands[7];
			Codeelement<FeatureType>* subCEs[7];
			Codeelement<FeatureType>* startCE;
			Codeelement<FeatureType>* temp;
			int i,j,inQueue,nextElem,startPos;
			int numOfCEs[7];
			int currPos,neighPos,remainingCEs;

			if (islandIterator->nextLevel())
				do
				{
					const Island& currIsland = islandIterator->currentIsland();

					ceBase->newIsland(currIsland);

					hierarchy->getSubIslands(currIsland,subIslands);
					ceBase->getCodeelements(subIslands,7,subCEs,numOfCEs);

					remainingCEs=numOfCEs[0]+numOfCEs[1]+numOfCEs[2]+numOfCEs[3]+numOfCEs[4]+numOfCEs[5]+numOfCEs[6];
					//for (startPos=0; (startPos<7) && (remainingCEs>1); startPos++)
					for (j=0; (j<7) && (remainingCEs>1); j++)
					{ 
						startPos=startPositions[startPosition][j];
						startCE=subCEs[startPos];
						while (startCE!=NULL)
						{
							if ((startCE->flags & CEUsed) == 0)
							{
								Codeelement<FeatureType>* newCE = ceBase->getCurrentCE();

								if (newCE==NULL) throw ECodeelementBaseOverrun();

								newCE->parents[0]=NULL;
								newCE->parents[1]=NULL;
								newCE->root=NULL;
								newCE->flags=CERoot;
								newCE->form=(1<<startPos);
								newCE->numOfSubCEs=1;
								newCE->subCEs[0].pos=startPos;
								newCE->subCEs[0].ce=startCE;
#ifdef _CSC_STORE_ISLAND
								newCE->island=currIsland;
#endif
#ifdef _CSC_STORE_BOUNDING_BOX
								newCE->bbLeft=startCE->bbLeft;
								newCE->bbRight=startCE->bbRight;
								newCE->bbTop=startCE->bbTop;
								newCE->bbBottom=startCE->bbBottom;
#endif
								linkControl->begin(1,startCE);
								ceQueue[0].ce=startCE;
								ceQueue[0].pos=startPos;
								startCE->flags|=CEUsed;
								remainingCEs--;

								inQueue=1;
								nextElem=0;

								while (nextElem<inQueue)
								{
									kiplAssert(inQueue<=11025);
									Codeelement<FeatureType>* currCE=ceQueue[nextElem].ce;
									currPos=ceQueue[nextElem].pos;
									nextElem++;  // Extract next position form queue

									for (i=0; neighborTable[currPos][i]!=-1; i++) // Extract all neighbors from queue
									{
										neighPos=neighborTable[currPos][i];
										kiplAssert(commonIslTable[currPos][neighPos]>=0);
										Codeelement<FeatureType>* neighCE=subCEs[neighPos];
										while (neighCE!=NULL)
										{
											if (((neighCE->flags & CEUsed) == 0)
												&& ((currCE->form & (1<<commonIslTable[currPos][neighPos]))!=0)
												&& ((neighCE->form & (1<<commonIslTable[neighPos][currPos]))!=0))
											{
												if (linkControl->add(currCE,neighCE))
												{
													ceQueue[inQueue].ce=neighCE;
													ceQueue[inQueue].pos=neighPos;
													inQueue++;

													neighCE->flags|=CEUsed;

													newCE->form|=(1<<neighPos);
													newCE->subCEs[newCE->numOfSubCEs].pos=neighPos;
													newCE->subCEs[newCE->numOfSubCEs].ce=neighCE;
													newCE->numOfSubCEs++;

													kiplAssert(neighCE->parents[0]==NULL || neighCE->parents[1]==NULL);
													if (neighCE->parents[0]==NULL) neighCE->parents[0]=newCE;
													else neighCE->parents[1]=newCE;
													neighCE->flags&=~CERoot;
													remainingCEs--;
													if (remainingCEs==0) break;
												}
											}
											neighCE=neighCE->next;
										}
										if (remainingCEs==0) break;
									}
									if (remainingCEs==0) break;
								}
								if (newCE->numOfSubCEs>1)
								{
									linkControl->setFeature(newCE->data);
									kiplAssert(startCE->parents[0]==NULL || startCE->parents[1]==NULL);
									if (startCE->parents[0]==NULL) startCE->parents[0]=newCE;
									else startCE->parents[1]=newCE;
									startCE->flags&=~(CERoot);
									ceBase->storeCurrentCE(true);
								}
								else remainingCEs++;
								if (remainingCEs<=1) break;
							}
							startCE=startCE->next;
						}
					}
					for (i=0; i<7; i++)
					{
						temp=subCEs[i];
						while (temp!=NULL)
						{
							temp->flags&=~CEUsed;
							temp=temp->next;
						}
					}
				} while (islandIterator->nextIsland());
		}


		template <class PixelType, class FeatureType, class LinkControl>
		void CSCEngine<PixelType,FeatureType,LinkControl>::level0LinkingBestNeighbor()
		{
			const NeighborTable& neighborTable = hierarchy->getNeighborTable();
			const CommonIslTable& commonIslTable = hierarchy->getCommonIslandTable(); 
			//ceQueueBestNeighborItem<FeatureType> ceQueueBestNeighbor[11025]; // 7 (Islands) * 7 (Max. Codeelements per Island) = 105

			Island subIslands[7];
			Codeelement<FeatureType>* subCEs[7];
			Codeelement<FeatureType>* startCE;
			Codeelement<FeatureType>* currCE;
			int bestNeighbor;
			FeatureType avgColor;
			Codeelement<FeatureType>* temp;
			int i,j,inQueue,nextElem,startPos,changes;
			int numOfCEs[7];
			int currPos,neighPos,remainingCEs;

			if (islandIterator->nextLevel())
				do
				{
					const Island& currIsland = islandIterator->currentIsland();

					ceBase->newIsland(currIsland);

					hierarchy->getSubIslands(currIsland,subIslands);
					ceBase->getCodeelements(subIslands,7,subCEs,numOfCEs);

					remainingCEs=numOfCEs[0]+numOfCEs[1]+numOfCEs[2]+numOfCEs[3]+numOfCEs[4]+numOfCEs[5]+numOfCEs[6];

					//for (startPos=0; (startPos<7) && (remainingCEs>1); startPos++)
					for (j=0; (j<7) && (remainingCEs>1); j++)
					{ 
						startPos=startPositions[startPosition][j];
						startCE=subCEs[startPos];
						while (startCE!=NULL)
						{
							if ((startCE->flags & CEUsed) == 0)
							{
								Codeelement<FeatureType>* newCE = ceBase->getCurrentCE();

								if (newCE==NULL) throw ECodeelementBaseOverrun();

								newCE->parents[0]=NULL;
								newCE->parents[1]=NULL;
								newCE->root=NULL;
								newCE->flags=CERoot;
								newCE->form=(1<<startPos);
								newCE->numOfSubCEs=1;
								newCE->subCEs[0].pos=startPos;
								newCE->subCEs[0].ce=startCE;
#ifdef _CSC_STORE_ISLAND
								newCE->island=currIsland;
#endif
#ifdef _CSC_STORE_BOUNDING_BOX
								newCE->bbLeft=startCE->bbLeft;
								newCE->bbRight=startCE->bbRight;
								newCE->bbTop=startCE->bbTop;
								newCE->bbBottom=startCE->bbBottom;
#endif
								linkControl->begin(1,startCE);
								startCE->flags|=CEUsed;
								remainingCEs--;

								inQueue=0;
								currCE=startCE;
								currPos=startPos;

								do
								{
									for (i=0; neighborTable[currPos][i]!=-1; i++) // Extract all neighbors from queue
									{
										neighPos=neighborTable[currPos][i];
										kiplAssert(commonIslTable[currPos][neighPos]>=0);
										Codeelement<FeatureType>* neighCE=subCEs[neighPos];

										while (neighCE!=NULL)
										{
											kiplAssert(inQueue<11025);
											if (((neighCE->flags & CEUsed) == 0)
												&& ((currCE->form & (1<<commonIslTable[currPos][neighPos]))!=0)
												&& ((neighCE->form & (1<<commonIslTable[neighPos][currPos]))!=0))
											{
												if (inQueue>=maxCEQueueBestNeighbor)
												{
													maxCEQueueBestNeighbor*=2;
													BestNeighborItem<FeatureType>* newQueue=new BestNeighborItem<FeatureType>[maxCEQueueBestNeighbor];
													for (int k=0; k<inQueue; k++) newQueue[k]=ceQueueBestNeighbor[k];
													delete[] ceQueueBestNeighbor;
													ceQueueBestNeighbor=newQueue;
												}

												ceQueueBestNeighbor[inQueue].member=currCE;
												ceQueueBestNeighbor[inQueue].memberPos=currPos;
												ceQueueBestNeighbor[inQueue].neighbor=neighCE;
												ceQueueBestNeighbor[inQueue].neighborPos=neighPos;
												inQueue++;
											}
											neighCE=neighCE->next;
										}
									}

									linkControl->setFeature(avgColor);
									changes=0;
									nextElem=0;
									bestNeighbor=-1;
									while (nextElem<inQueue)
									{
										// If a neighbor within the queue is alread part of the new segment remove it from the queue
										if (ceQueueBestNeighbor[nextElem].neighbor->flags & CEUsed)
										{
											inQueue--;
											ceQueueBestNeighbor[nextElem]=ceQueueBestNeighbor[inQueue];
										}
										else if (linkControl->add(ceQueueBestNeighbor[nextElem].member,ceQueueBestNeighbor[nextElem].neighbor,true))
										{
											if (bestNeighbor<0 || linkControl->compareFeatures(ceQueueBestNeighbor[bestNeighbor].neighbor->data,ceQueueBestNeighbor[nextElem].neighbor->data,avgColor)==2)
												bestNeighbor=nextElem;
											nextElem++;
										}
										else nextElem++;
									}
									if (bestNeighbor>=0)
									{
										currCE=ceQueueBestNeighbor[bestNeighbor].neighbor;
										currPos=ceQueueBestNeighbor[bestNeighbor].neighborPos;
										linkControl->add(currCE);
										currCE->flags|=CEUsed;

										newCE->form|=(1<<currPos);
										newCE->subCEs[newCE->numOfSubCEs].pos=currPos;
										newCE->subCEs[newCE->numOfSubCEs].ce=currCE;
										newCE->numOfSubCEs++;

										kiplAssert(currCE->parents[0]==NULL || currCE->parents[1]==NULL);
										if (currCE->parents[0]==NULL) currCE->parents[0]=newCE;
										else currCE->parents[1]=newCE;
										currCE->flags&=~CERoot;
										remainingCEs--;
										if (remainingCEs==0) break;
										changes++;
									}
									if (remainingCEs==0) break;
								} while (changes>0);

								if (newCE->numOfSubCEs>1)
								{
									linkControl->setFeature(newCE->data);
									kiplAssert(startCE->parents[0]==NULL || startCE->parents[1]==NULL);
									if (startCE->parents[0]==NULL) startCE->parents[0]=newCE;
									else startCE->parents[1]=newCE;
									startCE->flags&=~(CERoot);
									ceBase->storeCurrentCE(true);
								}
								else remainingCEs++;
							}
							if (remainingCEs<=1) break;
							startCE=startCE->next;
						}
					}
					for (i=0; i<7; i++)
					{
						temp=subCEs[i];
						while (temp!=NULL)
						{
							temp->flags&=~CEUsed;
							temp=temp->next;
						}
					}
				} while (islandIterator->nextIsland());
		}


		template <class PixelType, class FeatureType, class LinkControl>
		void CSCEngine<PixelType,FeatureType,LinkControl>::levelNLinking()
		{
			const ConnectedWithTable& connectedWithTable = hierarchy->getConnectedWithTable();
			//const CommonIslTable& commonIslTable = hierarchy->getCommonIslandTable(); 
			int inQueue,nextElem;
			Island subIslands[7];
			Codeelement<FeatureType>* subCEs[7];
			Codeelement<FeatureType>* startCE;
			Codeelement<FeatureType>* temp;
			SubCodeelement<FeatureType>* connectCE;
			int numOfCEs[7],startPos/*,ceNr*/;
			int currPos,neighPos,connectPos,i,j,k;
			SplittingPair<FeatureType> splittingInfo;

			while (islandIterator->nextLevel())
			{
				if (maxLinkLevel>=0 && islandIterator->currentIsland().level>maxLinkLevel) break;
				do
				{
					const Island& currIsland = islandIterator->currentIsland();

					ceBase->newIsland(currIsland);
					hierarchy->getSubIslands(currIsland,subIslands);
					ceBase->getCodeelements(subIslands,7,subCEs,numOfCEs);

					//for (startPos=0; startPos<7; startPos++)
					for (k=0; k<7; k++)
					{
						startPos=startPositions[startPosition][k];
						startCE=subCEs[startPos];
						while (startCE!=NULL)
						{
							if ((startCE->flags & CEUsed) == 0)
							{
								Codeelement<FeatureType>* newCE = ceBase->getCurrentCE();

								if (newCE==NULL)
								{
									if (ceQueue!=NULL) delete[] ceQueue;
									throw ECodeelementBaseOverrun();
								}

								newCE->parents[0]=NULL;
								newCE->parents[1]=NULL;
								newCE->root=NULL;
								newCE->flags=CERoot;
								newCE->form=(1<<startPos);
								newCE->numOfSubCEs=1;
								newCE->subCEs[0].pos=startPos;
								newCE->subCEs[0].ce=startCE;
#ifdef _CSC_STORE_ISLAND
								newCE->island=currIsland;
#endif
								linkControl->begin(currIsland.level,startCE);
								ceQueue[0].ce=startCE;
								ceQueue[0].pos=startPos;
								ceQueue[0].ce->flags|=CEUsed;
								inQueue=1;
								nextElem=0;

								while (nextElem<inQueue)
								{
									SubCodeelement<FeatureType>& currSubCE=ceQueue[nextElem];
									Codeelement<FeatureType>* currCE=currSubCE.ce;
									currPos=currSubCE.pos;
									nextElem++;

									//for (int subCENr=0; subCENr<currCE->numOfSubCEs; subCENr++)
									connectCE=currCE->subCEs;
									while (connectCE!=NULL)
									{
										connectPos = connectCE->pos;
										neighPos = connectedWithTable[currPos][connectPos];

										if (neighPos>=0)
										{
											Codeelement<FeatureType>* neighCE;

											kiplAssert(connectCE->ce->parents[0]==currCE || connectCE->ce->parents[1]==currCE);
											neighCE=(connectCE->ce->parents[0]==currCE) ? connectCE->ce->parents[1] : connectCE->ce->parents[0];
											kiplAssert(neighCE!=currCE);
											if (neighCE!=NULL && ((neighCE->flags & CEUsed) == 0))
											{
												if (linkControl->add(currCE,neighCE))
												{ 
													if (inQueue>=maxCEQueue)
													{
														maxCEQueue*=2;
														SubCodeelement<FeatureType>* newQueue=new SubCodeelement<FeatureType>[maxCEQueue];
														for (j=0; j<inQueue; j++) newQueue[j]=ceQueue[j];
														delete[] ceQueue;
														ceQueue=newQueue;
													}
													ceQueue[inQueue].ce=neighCE;
													ceQueue[inQueue].pos=neighPos;
													inQueue++;

													neighCE->flags|=CEUsed;

													newCE->form|=(1<<neighPos);
													newCE->subCEs[newCE->numOfSubCEs].pos=neighPos;
													newCE->subCEs[newCE->numOfSubCEs].ce=neighCE;
													newCE->numOfSubCEs++;
													kiplAssert(neighCE->parents[0]==NULL || neighCE->parents[1]==NULL);
													if (neighCE->parents[0]==NULL) neighCE->parents[0]=newCE;
													else neighCE->parents[1]=newCE;
													neighCE->flags&=~CERoot;
#ifdef _CSC_STORE_BOUNDING_BOX
													if (neighCE->bbLeft<newCE->bbLeft) newCE->bbLeft=neighCE->bbLeft;
													if (neighCE->bbRight>newCE->bbRight) newCE->bbRight=neighCE->bbRight;
													if (neighCE->bbBottom>newCE->bbBottom) newCE->bbBottom=neighCE->bbBottom;
													if (neighCE->bbTop<newCE->bbTop) newCE->bbTop=neighCE->bbTop;
#endif
												}
												else if (splittingMethod==DirectSplitting)
												{
													splittingInfo.root1=currCE;
													splittingInfo.root2=neighCE;
													splittingInfo.island=currIsland;
													splittingInfo.relPos1=currPos;
													splittingInfo.relPos2=neighPos;
													splittingInfo.ce1=currCE;
													splittingInfo.ce2=neighCE;
													splittingQueue.enqueue(splittingInfo);
												}
											}
										}
										connectCE=connectCE->next;
									}
								}
								if (newCE->numOfSubCEs>1)
								{
									linkControl->setFeature(newCE->data);
									kiplAssert(startCE->parents[0]==NULL || startCE->parents[1]==NULL);
									if (startCE->parents[0]==NULL) startCE->parents[0]=newCE;
									else startCE->parents[1]=newCE;
									startCE->flags&=~CERoot;
									ceBase->storeCurrentCE(true);
								}
							}
							startCE=startCE->next;
						}
					}
					for (i=0; i<7; i++)
					{
						temp=subCEs[i];
						while (temp!=NULL)
						{
							temp->flags&=~CEUsed;
							temp=temp->next;
						}
					}

				} while (islandIterator->nextIsland());
				if (splittingMethod==DirectSplitting) directSplitting();
			}
		}


		template <class PixelType, class FeatureType, class LinkControl>
		void CSCEngine<PixelType,FeatureType,LinkControl>::levelNLinkingBestNeighbor()
		{
			const ConnectedWithTable& connectedWithTable = hierarchy->getConnectedWithTable();
			//const CommonIslTable& commonIslTable = hierarchy->getCommonIslandTable(); 
			int inQueue,nextElem;
			Island subIslands[7];
			Codeelement<FeatureType>* subCEs[7];
			Codeelement<FeatureType>* startCE;
			Codeelement<FeatureType>* currCE;
			Codeelement<FeatureType>* neighCE;
			Codeelement<FeatureType>* temp;
			SubCodeelement<FeatureType>* connectCE;
			int numOfCEs[7],startPos/*,ceNr*/;
			int currPos,neighPos,connectPos,i,k,j;
			SplittingPair<FeatureType> splittingInfo;
			int bestNeighbor,changes;
			FeatureType avgColor;

			while (islandIterator->nextLevel())
			{
				do
				{
					const Island& currIsland = islandIterator->currentIsland();

					ceBase->newIsland(currIsland);
					hierarchy->getSubIslands(currIsland,subIslands);
					ceBase->getCodeelements(subIslands,7,subCEs,numOfCEs);

					//for (startPos=0; startPos<7; startPos++)
					for (k=0; k<7; k++)
					{ 
						startPos=startPositions[startPosition][k];
						startCE=subCEs[startPos];
						while (startCE!=NULL)
						{
							if ((startCE->flags & CEUsed) == 0)
							{
								Codeelement<FeatureType>* newCE = ceBase->getCurrentCE();

								if (newCE==NULL) throw ECodeelementBaseOverrun();

								newCE->parents[0]=NULL;
								newCE->parents[1]=NULL;
								newCE->root=NULL;
								newCE->flags=CERoot;
								newCE->form=(1<<startPos);
								newCE->numOfSubCEs=1;
								newCE->subCEs[0].pos=startPos;
								newCE->subCEs[0].ce=startCE;
#ifdef _CSC_STORE_ISLAND
								newCE->island=currIsland;
#endif
#ifdef _CSC_STORE_BOUNDING_BOX
								newCE->bbLeft=startCE->bbLeft;
								newCE->bbRight=startCE->bbRight;
								newCE->bbTop=startCE->bbTop;
								newCE->bbBottom=startCE->bbBottom;
#endif
								linkControl->begin(currIsland.level,startCE);
								startCE->flags|=CEUsed;

								inQueue=0;
								currCE=startCE;
								currPos=startPos;

								do
								{
									// Find all neighbors of currCE in put them into a queue
									connectCE=currCE->subCEs;
									while (connectCE!=NULL)
									{
										connectPos = connectCE->pos;
										neighPos = connectedWithTable[currPos][connectPos];

										if (neighPos>=0)
										{
											kiplAssert(connectCE->ce->parents[0]==currCE || connectCE->ce->parents[1]==currCE);
											neighCE=(connectCE->ce->parents[0]==currCE) ? connectCE->ce->parents[1] : connectCE->ce->parents[0];
											kiplAssert(neighCE!=currCE);
											if (neighCE!=NULL && ((neighCE->flags & CEUsed) == 0))
											{
												if (inQueue>=maxCEQueueBestNeighbor)
												{
													maxCEQueueBestNeighbor*=2;
													BestNeighborItem<FeatureType>* newQueue=new BestNeighborItem<FeatureType>[maxCEQueueBestNeighbor];
													for (j=0; j<inQueue; j++) newQueue[j]=ceQueueBestNeighbor[j];
													delete[] ceQueueBestNeighbor;
													ceQueueBestNeighbor=newQueue;
												}

												ceQueueBestNeighbor[inQueue].member=currCE;
												ceQueueBestNeighbor[inQueue].memberPos=currPos;
												ceQueueBestNeighbor[inQueue].neighbor=neighCE;
												ceQueueBestNeighbor[inQueue].neighborPos=neighPos;
												inQueue++;
											}
										}
										connectCE=connectCE->next;
									}

									linkControl->setFeature(avgColor);
									changes=0;
									nextElem=0;
									bestNeighbor=-1;
									while (nextElem<inQueue)
									{
										// If a neighbor within the queue is alread part of the new segment remove it from the queue
										if (ceQueueBestNeighbor[nextElem].neighbor->flags & CEUsed)
										{
											inQueue--;
											ceQueueBestNeighbor[nextElem]=ceQueueBestNeighbor[inQueue];
										}
										else if (linkControl->add(ceQueueBestNeighbor[nextElem].member,ceQueueBestNeighbor[nextElem].neighbor,true))
										{
											if (bestNeighbor<0 || linkControl->compareFeatures(ceQueueBestNeighbor[bestNeighbor].neighbor->data,ceQueueBestNeighbor[nextElem].neighbor->data,avgColor)==2)
												bestNeighbor=nextElem;
											nextElem++;
										}
										else nextElem++;
									}
									if (bestNeighbor>=0)
									{
										currCE=ceQueueBestNeighbor[bestNeighbor].neighbor;
										currPos=ceQueueBestNeighbor[bestNeighbor].neighborPos;
										linkControl->add(currCE);
										currCE->flags|=CEUsed;

										newCE->form|=(1<<currPos);
										newCE->subCEs[newCE->numOfSubCEs].pos=currPos;
										newCE->subCEs[newCE->numOfSubCEs].ce=currCE;
										newCE->numOfSubCEs++;

										kiplAssert(currCE->parents[0]==NULL || currCE->parents[1]==NULL);
										if (currCE->parents[0]==NULL) currCE->parents[0]=newCE;
										else currCE->parents[1]=newCE;
										currCE->flags&=~CERoot;
										changes++;
									}
								} while (changes>0);

								if (splittingMethod==DirectSplitting)
									for (nextElem=0; nextElem<inQueue; nextElem++)
									{
										splittingInfo.root1=ceQueueBestNeighbor[nextElem].member;
										splittingInfo.root2=ceQueueBestNeighbor[nextElem].neighbor;
										splittingInfo.island=currIsland;
										splittingInfo.relPos1=ceQueueBestNeighbor[nextElem].memberPos;
										splittingInfo.relPos2=ceQueueBestNeighbor[nextElem].neighborPos;
										splittingInfo.ce1=ceQueueBestNeighbor[nextElem].member;
										splittingInfo.ce2=ceQueueBestNeighbor[nextElem].neighbor;
										splittingQueue.enqueue(splittingInfo);
									}

									if (newCE->numOfSubCEs>1)
									{
										linkControl->setFeature(newCE->data);
										kiplAssert(startCE->parents[0]==NULL || startCE->parents[1]==NULL);
										if (startCE->parents[0]==NULL) startCE->parents[0]=newCE;
										else startCE->parents[1]=newCE;
										startCE->flags&=~(CERoot);
										ceBase->storeCurrentCE(true);
									}
							}
							startCE=startCE->next;
						}
					}
					for (i=0; i<7; i++)
					{
						temp=subCEs[i];
						while (temp!=NULL)
						{
							temp->flags&=~CEUsed;
							temp=temp->next;
						}
					}
				} while (islandIterator->nextIsland());
				if (splittingMethod==DirectSplitting) directSplitting();
			}
		}


#ifdef _DEBUG_DISINTEGRATION
		template <class PixelType, class FeatureType, class LinkControl>
		bool CSCEngine<PixelType,FeatureType,LinkControl>::testConnectivity(Codeelement<FeatureType>* rootCE)
		{
			int i,j,inQueue,neighPos;
			const ConnectedWithTable& connectedWithTable = hierarchy->getConnectedWithTable();
			const CommonIslTable& commonIslTable = hierarchy->getCommonIslandTable(); 
			SubCodeelement<FeatureType> current;
			SubCodeelement<FeatureType>* subCE;
			Codeelement<FeatureType>* neighCE;

			i=0;
			j=0;
			if (j<rootCE->numOfSubCEs)
			{
				inQueue=1;
				ceQueue[0]=rootCE->subCEs[j];
				ceQueue[0].ce->flags|=CEInQueue;
			}
			else inQueue=0;

			while (i<inQueue)
			{
				current=ceQueue[i]; i++;
				subCE=current.ce->subCEs;
				while (subCE!=NULL)
				{
					neighPos=connectedWithTable[current.pos][subCE->pos];
					if (neighPos!=-1 && subCE->ce->parents[0]!=NULL && subCE->ce->parents[1]!=NULL)
					{
						if (inQueue>=maxCEQueue)
						{
							maxCEQueue*=2;
							SubCodeelement<FeatureType>* newQueue=new SubCodeelement<FeatureType>[maxCEQueue];
							for (j=0; j<inQueue; j++) newQueue[j]=ceQueue[j];
							delete[] ceQueue;
							ceQueue=newQueue;
						}  
						if (subCE->ce->parents[0]==current.ce)
						{
							neighCE=subCE->ce->parents[1];
							if (((neighCE->flags & CEInQueue)==0) && (neighCE->parents[0]==rootCE || neighCE->parents[1]==rootCE))
							{
								ceQueue[inQueue].pos=neighPos;
								ceQueue[inQueue].ce=neighCE;
								ceQueue[inQueue].ce->flags|=CEInQueue;
								inQueue++;
							}
						}
						else 
						{
							neighCE=subCE->ce->parents[0];
							if (((neighCE->flags & CEInQueue)==0) && (neighCE->parents[0]==rootCE || neighCE->parents[1]==rootCE))
							{
								ceQueue[inQueue].pos=neighPos;
								ceQueue[inQueue].ce=neighCE;
								ceQueue[inQueue].ce->flags|=CEInQueue;
								inQueue++;
							}
						}
					}
					subCE=subCE->next;
				}
			}
			kiplAssert(inQueue<=rootCE->numOfSubCEs);
			for (i=0; i<inQueue; i++)
				ceQueue[i].ce->flags&=~CEInQueue;

			return (inQueue==rootCE->numOfSubCEs);
		}


		template <class PixelType, class FeatureType, class LinkControl>
		bool CSCEngine<PixelType,FeatureType,LinkControl>::testLevel1Connectivity(Codeelement<FeatureType>* rootCE)
		{
			int i,j,inQueue;
			const ConnectedWithTable& connectedWithTable = hierarchy->getConnectedWithTable();
			const CommonIslTable& commonIslTable = hierarchy->getCommonIslandTable(); 
			SubCodeelement<FeatureType> current;
			SubCodeelement<FeatureType>* searchCE;

			i=0;
			searchCE=rootCE->subCEs;
			while (searchCE!=NULL && searchCE->ce==removedCE) searchCE=searchCE->next;
			if (searchCE!=NULL)
			{
				inQueue=1;
				ceQueue[0]=(*searchCE);
				ceQueue[0].ce->flags|=CEInQueue;
			}
			else inQueue=0;

			while (i<inQueue)
			{
				current=ceQueue[i]; i++;
				searchCE=rootCE->subCEs;
				while (searchCE!=NULL)
				{
					if ((searchCE->ce->flags & CEInQueue)==0 && searchCE->ce!=removedCE 
						&& ((current.ce->form & (1<<commonIslTable[current.pos][searchCE->pos]))!=0)
						&& ((searchCE->ce->form & (1<<commonIslTable[searchCE->pos][current.pos]))!=0))
					{
						if (inQueue>=maxCEQueue)
						{
							maxCEQueue*=2;
							SubCodeelement<FeatureType>* newQueue=new SubCodeelement<FeatureType>[maxQueue];
							for (j=0; j<inQueue; j++) newQueue[j]=ceQueue[j];
							delete[] ceQueue;
							ceQueue=newQueue;
						}  
						ceQueue[inQueue]=*searchCE;
						searchCE->ce->flags|=CEInQueue;
						inQueue++;
					}
					searchCE=searchCE->next;
				}
			}
			kiplAssert(inQueue<=rootCE->numOfSubCEs);
			for (i=0; i<inQueue; i++)
				ceQueue[i].ce->flags&=~CEInQueue;

			return (inQueue==rootCE->numOfSubCEs);
		}


		template <class PixelType, class FeatureType, class LinkControl>
		void CSCEngine<PixelType,FeatureType,LinkControl>::testConnectivity()
		{
			Codeelement<FeatureType>* current;
			int /*i,*/ceCount;
			TopDownIslandIterator* islIterator = hierarchy->createIteratorTD();

			islIterator->begin();
			do
			{
				do
				{
					current=ceBase->getCodeelements(islIterator->currentIsland(),ceCount);
					while (current)
					{
						if (!testConnectivity(current))
							current->flags|=CEDisintegrated;
						current=current->next;
					}
				} while (islIterator->nextIsland());
			} while (islIterator->nextLevel() && islIterator->currentIsland().level>1);
		}
#endif


		template <class PixelType, class FeatureType, class LinkControl>
		void CSCEngine<PixelType,FeatureType,LinkControl>::directSplitting()
		{
			const CommonIslTable& commonIslTable = hierarchy->getCommonIslandTable();
			const ConnectedWithTable& connectedWithTable = hierarchy->getConnectedWithTable();
			SplittingPair<FeatureType> currentPair;
			SplittingPair<FeatureType> newPair;
			RepairItem<FeatureType> repairItem;
			int commonPos,masterCE;
			SubCodeelement<FeatureType>* commonCE;
			SubCodeelement<FeatureType>* subCE;

			int number=splittingQueue.numOfElements();
			int count=number;
			while (!splittingQueue.isEmpty())
			{
				currentPair=splittingQueue.current();
				count--;
				commonCE=currentPair.ce1->subCEs;
				while (commonCE!=NULL)
				{
					if (commonCE->ce->parents[0]==currentPair.ce2 || commonCE->ce->parents[1]==currentPair.ce2)
					{
						masterCE=linkControl->compareFeatures(currentPair.root1->data,currentPair.root2->data,commonCE->ce->data);
						if (repairMethod==FastRepair)
						{
							if (masterCE==1)
							{
								if (commonCE->ce->numOfSubCEs>0 && !testConnectivityIf(currentPair.ce2,commonCE->ce)) 
								{
									if ((currentPair.island.level>1 && testConnectivityIf(currentPair.ce1,commonCE->ce)) 
										|| (currentPair.island.level==1 && testLevel1ConnectivityIf(currentPair.ce1,commonCE->ce))) 
										masterCE=2;
									else
										currentPair.ce2->flags|=CEDisintegrated;
								}
							}
							else
							{
								if (commonCE->ce->numOfSubCEs>0 && !testConnectivityIf(currentPair.ce1,commonCE->ce)) 
								{
									if ((currentPair.island.level>1 && testConnectivityIf(currentPair.ce2,commonCE->ce))
										|| (currentPair.island.level==1 && testLevel1ConnectivityIf(currentPair.ce1,commonCE->ce))) 
										masterCE=1;
									else
										currentPair.ce1->flags|=CEDisintegrated;
								}
							}
						}

						if (masterCE==1)
						{
							Island subIsland;

							hierarchy->getSubIsland(currentPair.island,currentPair.relPos2,subIsland);
							ceBase->removeSubCE(currentPair.ce2,commonCE->ce);
							if (updateAfterSplitting) updateFeature(currentPair.island.level,currentPair.ce2);
							commonPos=commonIslTable[currentPair.relPos2][currentPair.relPos1];
							subCE=commonCE->ce->subCEs;
							while (subCE!=NULL)
							{
								if (connectedWithTable[commonPos][subCE->pos]>=0 && subCE->ce->parents[0]!=NULL && subCE->ce->parents[1]!=NULL)
								{
									newPair.root1=currentPair.root1;
									newPair.root2=currentPair.root2;
									newPair.relPos1=commonPos;
									newPair.relPos2=connectedWithTable[commonPos][subCE->pos];
									newPair.island=subIsland;
									newPair.ce1=commonCE->ce;
									if (subCE->ce->parents[0]==commonCE->ce) newPair.ce2=subCE->ce->parents[1];
									else newPair.ce2=subCE->ce->parents[0];
									splittingQueue.enqueue(newPair);
								}
								subCE=subCE->next;
							}
							if (((currentPair.ce2->flags & CEDisintegrated) || repairMethod==PreciseRepair)  && (currentPair.ce2->flags & CEUsed) == 0)
							{
								currentPair.ce2->flags&=~CEDisintegrated;
								currentPair.ce2->flags|=CEUsed;
								repairItem.ce=currentPair.ce2;
								repairItem.island=subIsland;
								repairQueue.enqueue(repairItem);
								//repair(subIsland,currentPair.ce2);
							}
						}
						else if (masterCE==2)
						{
							Island subIsland;

							hierarchy->getSubIsland(currentPair.island,currentPair.relPos1,subIsland);
							ceBase->removeSubCE(currentPair.ce1,commonCE->ce);
							if (updateAfterSplitting) updateFeature(currentPair.island.level,currentPair.ce2);
							commonPos=commonIslTable[currentPair.relPos1][currentPair.relPos2];
							subCE=commonCE->ce->subCEs;
							while (subCE!=NULL)
							{
								if (connectedWithTable[commonPos][subCE->pos]>=0 && subCE->ce->parents[0]!=NULL && subCE->ce->parents[1]!=NULL)
								{
									newPair.root1=currentPair.root1;
									newPair.root2=currentPair.root2;
									newPair.relPos1=connectedWithTable[commonPos][subCE->pos];
									newPair.relPos2=commonPos;
									newPair.island=subIsland;
									if (subCE->ce->parents[0]==commonCE->ce) newPair.ce1=subCE->ce->parents[1];
									else newPair.ce1=subCE->ce->parents[0];
									newPair.ce2=commonCE->ce;
									splittingQueue.enqueue(newPair);
								}
								subCE=subCE->next;
							}
							if (((currentPair.ce1->flags & CEDisintegrated) || repairMethod==PreciseRepair) && (currentPair.ce1->flags & CEUsed) == 0)
							{
								currentPair.ce1->flags&=~CEDisintegrated;
								currentPair.ce1->flags|=CEUsed;
								repairItem.ce=currentPair.ce1;
								repairItem.island=subIsland;
								repairQueue.enqueue(repairItem);
								//repair(subIsland,currentPair.ce1);
							}
						}
					}
					commonCE=commonCE->next;
				}
				splittingQueue.dequeue();
			}
			while (!repairQueue.isEmpty())
			{
				repairItem=repairQueue.current();
				repairItem.ce->flags&=~CEUsed;
				repair(repairItem.island,repairItem.ce);
				repairQueue.dequeue();
			}
		}


		template <class PixelType, class FeatureType, class LinkControl>
		bool CSCEngine<PixelType,FeatureType,LinkControl>::testConnectivityIf(Codeelement<FeatureType>* rootCE, Codeelement<FeatureType>* removedCE)
		{
			int i,j,inQueue,neighPos;
			const ConnectedWithTable& connectedWithTable = hierarchy->getConnectedWithTable();
			//const CommonIslTable& commonIslTable = hierarchy->getCommonIslandTable(); 
			SubCodeelement<FeatureType> current;
			SubCodeelement<FeatureType>* subCE;
			Codeelement<FeatureType>* neighCE;

			i=0;
			subCE=rootCE->subCEs;
			while (subCE!=NULL && subCE->ce==removedCE) subCE=subCE->next;
			if (subCE!=NULL)
			{
				inQueue=1;
				ceQueue[0]=*subCE;
				subCE->ce->flags|=CEInQueue;
			}
			else inQueue=0;

			while (i<inQueue)
			{
				current=ceQueue[i]; i++;
				subCE=current.ce->subCEs;
				while (subCE!=NULL)
				{
					neighPos=connectedWithTable[current.pos][subCE->pos];
					if (neighPos!=-1 && subCE->ce->parents[0]!=NULL && subCE->ce->parents[1]!=NULL && subCE->ce->parents[0]!=removedCE && subCE->ce->parents[1]!=removedCE)
					{
						if (inQueue>=maxCEQueue)
						{
							maxCEQueue*=2;
							SubCodeelement<FeatureType>* newQueue=new SubCodeelement<FeatureType>[maxCEQueue];
							for (j=0; j<inQueue; j++) newQueue[j]=ceQueue[j];
							delete[] ceQueue;
							ceQueue=newQueue;
						}  
						if (subCE->ce->parents[0]==current.ce)
						{
							neighCE=subCE->ce->parents[1];
							if (((neighCE->flags & CEInQueue)==0) && neighCE!=removedCE && (neighCE->parents[0]==rootCE || neighCE->parents[1]==rootCE))
							{
								ceQueue[inQueue].pos=neighPos;
								ceQueue[inQueue].ce=neighCE;
								ceQueue[inQueue].ce->flags|=CEInQueue;
								inQueue++;
							}
						}
						else 
						{
							neighCE=subCE->ce->parents[0];
							if (((neighCE->flags & CEInQueue)==0) && neighCE!=removedCE && (neighCE->parents[0]==rootCE || neighCE->parents[1]==rootCE))
							{
								ceQueue[inQueue].pos=neighPos;
								ceQueue[inQueue].ce=neighCE;
								ceQueue[inQueue].ce->flags|=CEInQueue;
								inQueue++;
							}
						}
					}
					subCE=subCE->next;
				}
			}
			kiplAssert(inQueue+1<=rootCE->numOfSubCEs);
			for (i=0; i<inQueue; i++)
				ceQueue[i].ce->flags&=~CEInQueue;

			return (inQueue+1==rootCE->numOfSubCEs);
		}


		template <class PixelType, class FeatureType, class LinkControl>
		bool CSCEngine<PixelType,FeatureType,LinkControl>::testLevel1ConnectivityIf(Codeelement<FeatureType>* rootCE, Codeelement<FeatureType>* removedCE)
		{
			int i,j,inQueue;
			//const ConnectedWithTable& connectedWithTable = hierarchy->getConnectedWithTable();
			const CommonIslTable& commonIslTable = hierarchy->getCommonIslandTable(); 
			SubCodeelement<FeatureType> current;
			SubCodeelement<FeatureType>* searchCE;

			i=0;
			searchCE=rootCE->subCEs;
			while (searchCE!=NULL && searchCE->ce==removedCE) searchCE=searchCE->next;
			if (searchCE!=NULL)
			{
				inQueue=1;
				ceQueue[0]=(*searchCE);
				ceQueue[0].ce->flags|=CEInQueue;
			}
			else inQueue=0;

			while (i<inQueue)
			{
				current=ceQueue[i]; i++;
				searchCE=rootCE->subCEs;
				while (searchCE!=NULL)
				{
					if ((searchCE->ce->flags & CEInQueue)==0 && searchCE->ce!=removedCE 
						&& ((current.ce->form & (1<<commonIslTable[current.pos][searchCE->pos]))!=0)
						&& ((searchCE->ce->form & (1<<commonIslTable[searchCE->pos][current.pos]))!=0))
					{
						if (inQueue>=maxCEQueue)
						{
							maxCEQueue*=2;
							SubCodeelement<FeatureType>* newQueue=new SubCodeelement<FeatureType>[maxCEQueue];
							for (j=0; j<inQueue; j++) newQueue[j]=ceQueue[j];
							delete[] ceQueue;
							ceQueue=newQueue;
						}  
						ceQueue[inQueue]=*searchCE;
						searchCE->ce->flags|=CEInQueue;
						inQueue++;
					}
					searchCE=searchCE->next;
				}
			}
			kiplAssert(inQueue+1<=rootCE->numOfSubCEs);
			for (i=0; i<inQueue; i++)
				ceQueue[i].ce->flags&=~CEInQueue;

			return (inQueue+1==rootCE->numOfSubCEs);
		}


		template <class PixelType, class FeatureType, class LinkControl>
		void CSCEngine<PixelType,FeatureType,LinkControl>::repair(Codeelement<FeatureType>* rootCE, SubCodeelement<FeatureType>* subCEList, int &numOfConnected, SubCodeelement<FeatureType>* &connectedList, unsigned char &connectedForm, int &numOfRest, SubCodeelement<FeatureType>* &restList)
		{
			int i,j,inQueue,neighPos;
			const ConnectedWithTable& connectedWithTable = hierarchy->getConnectedWithTable();
			//const CommonIslTable& commonIslTable = hierarchy->getCommonIslandTable(); 
			SubCodeelement<FeatureType> current;
			SubCodeelement<FeatureType>* subCE;
			Codeelement<FeatureType>* neighCE;

			i=0;
			inQueue=1;
			ceQueue[0]=rootCE->subCEs[0];
			ceQueue[0].ce->flags|=CEInQueue;
			while (i<inQueue)
			{
				current=ceQueue[i]; i++;
				subCE=current.ce->subCEs;
				while (subCE!=NULL)
				{
					neighPos=connectedWithTable[current.pos][subCE->pos];
					if (neighPos!=-1 && subCE->ce->parents[0]!=NULL && subCE->ce->parents[1]!=NULL)
					{
						if (inQueue>=maxCEQueue)
						{
							maxCEQueue*=2;
							SubCodeelement<FeatureType>* newQueue=new SubCodeelement<FeatureType>[maxCEQueue];
							for (j=0; j<inQueue; j++) newQueue[j]=ceQueue[j];
							delete[] ceQueue;
							ceQueue=newQueue;
						}  
						if (subCE->ce->parents[0]==current.ce)
						{
							neighCE=subCE->ce->parents[1];
							if (((neighCE->flags & CEInQueue)==0) && (neighCE->parents[0]==rootCE || neighCE->parents[1]==rootCE))
							{
								ceQueue[inQueue].pos=neighPos;
								ceQueue[inQueue].ce=neighCE;
								ceQueue[inQueue].ce->flags|=CEInQueue;
								inQueue++;
							}
						}
						else 
						{
							neighCE=subCE->ce->parents[0];
							if (((neighCE->flags & CEInQueue)==0) && (neighCE->parents[0]==rootCE || neighCE->parents[1]==rootCE))
							{
								ceQueue[inQueue].pos=neighPos;
								ceQueue[inQueue].ce=neighCE;
								ceQueue[inQueue].ce->flags|=CEInQueue;
								inQueue++;
							}
						}
					}
					subCE=subCE->next;
				}
			}

			kiplAssert(inQueue<=rootCE->numOfSubCEs);

			numOfConnected=0;
			numOfRest=0;
			connectedList=NULL;
			restList=NULL;
			SubCodeelement<FeatureType>* lastOfRestList=NULL;
			SubCodeelement<FeatureType>* lastOfConnectedList=NULL;
			connectedForm=0;

			while (subCEList!=NULL)
			{
				if ((subCEList->ce->flags & CEInQueue)!=0)
				{
					if (lastOfConnectedList==NULL) 
					{
						numOfConnected=1;
						lastOfConnectedList=connectedList=subCEList;
						connectedForm|=(1<<subCEList->pos);
					}
					else 
					{
						numOfConnected++;
						lastOfConnectedList->next=subCEList;
						lastOfConnectedList=lastOfConnectedList->next;
						connectedForm|=(1<<subCEList->pos);
					}
				}
				else
				{
					if (lastOfRestList==NULL) 
					{
						numOfRest=1;
						lastOfRestList=restList=subCEList;
					}
					else 
					{
						numOfRest++;
						lastOfRestList->next=subCEList;
						lastOfRestList=lastOfRestList->next;
					}
				}
				subCEList=subCEList->next;
			}
			if (lastOfConnectedList!=NULL) lastOfConnectedList->next=NULL;
			if (lastOfRestList!=NULL) lastOfRestList->next=NULL;

			for (i=0; i<inQueue; i++)
				ceQueue[i].ce->flags&=~CEInQueue;
		}


		template <class PixelType, class FeatureType, class LinkControl>
		void CSCEngine<PixelType,FeatureType,LinkControl>::level1Repair(Codeelement<FeatureType>* rootCE, SubCodeelement<FeatureType>* subCEList, int &numOfConnected, SubCodeelement<FeatureType>* &connectedList, unsigned char &connectedForm, int &numOfRest, SubCodeelement<FeatureType>* &restList)
		{
			int i,j,inQueue;
			//const ConnectedWithTable& connectedWithTable = hierarchy->getConnectedWithTable();
			const CommonIslTable& commonIslTable = hierarchy->getCommonIslandTable(); 
			SubCodeelement<FeatureType> current;
			SubCodeelement<FeatureType>* searchCE;

			i=0;
			inQueue=1;
			ceQueue[0]=rootCE->subCEs[0];
			ceQueue[0].ce->flags|=CEInQueue;
			while (i<inQueue)
			{
				current=ceQueue[i]; i++;
				searchCE=rootCE->subCEs;
				while (searchCE!=NULL)
				{
					if ((searchCE->ce->flags & CEInQueue)==0
						&& ((current.ce->form & (1<<commonIslTable[current.pos][searchCE->pos]))!=0)
						&& ((searchCE->ce->form & (1<<commonIslTable[searchCE->pos][current.pos]))!=0))
					{
						if (inQueue>=maxCEQueue)
						{
							maxCEQueue*=2;
							SubCodeelement<FeatureType>* newQueue=new SubCodeelement<FeatureType>[maxCEQueue];
							for (j=0; j<inQueue; j++) newQueue[j]=ceQueue[j];
							delete[] ceQueue;
							ceQueue=newQueue;
						}  
						ceQueue[inQueue]=*searchCE;
						searchCE->ce->flags|=CEInQueue;
						inQueue++;
					}
					searchCE=searchCE->next;
				}
			}

			kiplAssert(inQueue<=rootCE->numOfSubCEs);

			numOfConnected=0;
			numOfRest=0;
			connectedList=NULL;
			restList=NULL;
			SubCodeelement<FeatureType>* lastOfRestList=NULL;
			SubCodeelement<FeatureType>* lastOfConnectedList=NULL;
			connectedForm=0;

			while (subCEList!=NULL)
			{
				if ((subCEList->ce->flags & CEInQueue)!=0)
				{
					if (lastOfConnectedList==NULL) 
					{
						numOfConnected=1;
						lastOfConnectedList=connectedList=subCEList;
						connectedForm|=(1<<subCEList->pos);
					}
					else 
					{
						numOfConnected++;
						lastOfConnectedList->next=subCEList;
						lastOfConnectedList=lastOfConnectedList->next;
						connectedForm|=(1<<subCEList->pos);
					}
				}
				else
				{
					if (lastOfRestList==NULL) 
					{
						numOfRest=1;
						lastOfRestList=restList=subCEList;
					}
					else 
					{
						numOfRest++;
						lastOfRestList->next=subCEList;
						lastOfRestList=lastOfRestList->next;
					}
				}
				subCEList=subCEList->next;
			}
			if (lastOfConnectedList!=NULL) lastOfConnectedList->next=NULL;
			if (lastOfRestList!=NULL) lastOfRestList->next=NULL;

			for (i=0; i<inQueue; i++)
				ceQueue[i].ce->flags&=~CEInQueue;
		}


		template <class PixelType, class FeatureType, class LinkControl>
		int CSCEngine<PixelType,FeatureType,LinkControl>::repair(const Island& island, Codeelement<FeatureType>* rootCE)
		{
			Codeelement<FeatureType>* newCE = rootCE;
			Codeelement<FeatureType>* oldCE = rootCE;
			Codeelement<FeatureType>* parent1 = rootCE->parents[0];
			Codeelement<FeatureType>* parent2 = rootCE->parents[1];
			//int parent1Pos = 0;
			//int parent2Pos = 0;
			SubCodeelement<FeatureType>* connectedList;
			SubCodeelement<FeatureType>* restList;
			SubCodeelement<FeatureType>* subCE;
			SubCodeelement<FeatureType>* subCEInParent1 = NULL;
			SubCodeelement<FeatureType>* subCEInParent2 = NULL;
			int numOfConnected,numOfRest,degreeOfDisintegration=1;

			if (rootCE==NULL || rootCE->subCEs==NULL) return 0;
			subCEInParent1=(parent1) ? ceBase->findSubCE(parent1,rootCE) : NULL;
			subCEInParent2=(parent2) ? ceBase->findSubCE(parent2,rootCE) : NULL;
			ceBase->reopenIsland(island);
			do
			{
				if (island.level>1)
					repair(newCE,newCE->subCEs,numOfConnected,connectedList,newCE->form,numOfRest,restList);
				else level1Repair(newCE,newCE->subCEs,numOfConnected,connectedList,newCE->form,numOfRest,restList);
				if (numOfConnected>1)
				{
					newCE->subCEs=connectedList;
					newCE->numOfSubCEs=numOfConnected;
					if (newCE!=rootCE) 
					{
						newCE->parents[0]=parent1;
						newCE->parents[1]=parent2;
						if (parent1!=NULL)
						{
							subCE=ceBase->getCurrentSubCEs();
							subCE->pos=subCEInParent1->pos;
							subCE->ce=newCE;
							subCE->next=parent1->subCEs;
							parent1->subCEs=subCE;;
							ceBase->storeCurrentSubCEs(1);
							parent1->numOfSubCEs++;
						}
						if (parent2!=NULL)
						{
							subCE=ceBase->getCurrentSubCEs();
							subCE->pos=subCEInParent2->pos;
							subCE->ce=newCE;
							subCE->next=parent2->subCEs;
							parent2->subCEs=subCE;;
							ceBase->storeCurrentSubCEs(1);
							parent2->numOfSubCEs++;
						}
						ceBase->storeCurrentCE(false);
						degreeOfDisintegration++;
					}
					if (newCE->parents[0]==NULL && newCE->parents[1]==NULL) 
						newCE->flags|=CERoot;

					oldCE=newCE;
					newCE=ceBase->getCurrentCE();
					newCE->data=rootCE->data;
					newCE->form=rootCE->form;
					newCE->flags=rootCE->flags;
					newCE->numOfSubCEs=numOfRest;
					newCE->subCEs=restList;
					newCE->parents[0]=NULL;
					newCE->parents[1]=NULL;
					subCE=newCE->subCEs;
					while (subCE!=NULL)
					{
						kiplAssert(subCE->ce->parents[0]==oldCE || subCE->ce->parents[1]==oldCE);
						if (subCE->ce->parents[0]==oldCE) subCE->ce->parents[0]=newCE;
						else subCE->ce->parents[1]=newCE;
						subCE=subCE->next;
					}
				}
				else if (connectedList!=NULL)
				{
					newCE->subCEs=restList;
					newCE->numOfSubCEs=numOfRest;

					kiplAssert(connectedList->ce->parents[0]==newCE || connectedList->ce->parents[1]==newCE);
					if (connectedList->ce->parents[0]==newCE) connectedList->ce->parents[0]=NULL;
					else connectedList->ce->parents[1]=NULL;
					if (connectedList->ce->parents[0]==NULL && connectedList->ce->parents[1]==NULL) 
						connectedList->ce->flags|=CERoot;
					degreeOfDisintegration++;
				}
			} while (restList!=NULL && newCE->subCEs==restList);
			if (rootCE->numOfSubCEs<=1)
			{
				if (parent1!=NULL) ceBase->removeSubCE(parent1,rootCE);
				if (parent2!=NULL) ceBase->removeSubCE(parent2,rootCE);
				ceBase->removeCodeelement(island,rootCE);
			}
			if (degreeOfDisintegration>1)
			{ 
				Island parentIsland;

				if (subCEInParent1!=NULL)
				{
					hierarchy->getParentIsland(island,subCEInParent1->pos,parentIsland);
					repair(parentIsland,parent1);
				}
				if (subCEInParent2!=NULL)
				{
					hierarchy->getParentIsland(island,subCEInParent2->pos,parentIsland);
					repair(parentIsland,parent2);
				}
			}

			return degreeOfDisintegration;
		}


		template <class PixelType, class FeatureType, class LinkControl>
		void CSCEngine<PixelType,FeatureType,LinkControl>::deferredSplitting()
		{
			Codeelement<FeatureType>* ces;
			Codeelement<FeatureType>* root1;
			Codeelement<FeatureType>* root2;
			int ceCount;
			TopDownIslandIterator* islIterator = hierarchy->createIteratorTD();

			islIterator->begin();
			do
			{
				do
				{
					const Island& currentIsland = islIterator->currentIsland();
					ces=ceBase->getCodeelements(currentIsland,ceCount);
					while (ces!=NULL)
					{
						root1=(ces->parents[0]!=NULL) ? ces->parents[0]->root : NULL;
						root2=(ces->parents[1]!=NULL) ? ces->parents[1]->root : NULL;

						if (root1==NULL && root2!=NULL) ces->root=root2;
						else if (root2==NULL && root1!=NULL) ces->root=root1;
						else if (root1==NULL && root2==NULL) 
							ces->root=ces;
						else if (root1==root2) ces->root=root1;
						else
						{
							switch (linkControl->compareFeatures(root1->data,root2->data,ces->data))
							{
							case 1: 
								ceBase->removeSubCE(ces->parents[1],ces);
								ces->root=ces->parents[0]->root;
								break;
							case 2: 
								ceBase->removeSubCE(ces->parents[0],ces);
								ces->root=ces->parents[1]->root;
								break;
							}
						}
						ces=ces->next;
					}
				} while (islIterator->nextIsland());
			} while (islIterator->nextLevel());
		}


		template <class PixelType, class FeatureType, class LinkControl>
		void CSCEngine<PixelType,FeatureType,LinkControl>::storeSegments()
		{
			Codeelement<FeatureType>* ce;
			int count;

			if (islandIterator->begin())
			{
				do
				{
					if (maxLinkLevel>=0 && islandIterator->currentIsland().level>maxLinkLevel) break;
					do
					{
						const Island& currIsland = islandIterator->currentIsland();
						ce=ceBase->getCodeelements(currIsland,count);
						while (ce!=NULL)
						{ 
							if (ce->flags & CERoot){
								ceBase->addNewSegment(currIsland,ce);
							}
							ce=ce->next;
						}
					} while (islandIterator->nextIsland());
				} while (islandIterator->nextLevel());
			}
		}
	}
}

#endif
