/*

This "SOFTWARE" is a free software.

You are allowed to download, use, modify and redistribute this software.
The software is provided "AS IS" without warranty of any kind.

Copyright: University of Koblenz, Lutz Priese, Volker Rehrman, Patrick Sturm

*/


/***********************************************
iMAge pROcessing cLAsses Version 1.0

Copyright 2002, Patrick Sturm
************************************************/

// Include-Files
#include <kipl-csc/islhierarchy.h>
#include <kipl-base/error.h>

using namespace std;
namespace kipl {
	namespace csc {
		// Tables
		const int deltaX[7] = {0,1,-1,0,-1, 0,-1};
		const int deltaY[7] = {0,0, 1,1,-1,-1, 0};

		//const int level0DeltaX[7] = {0,1,-1,0,-1, 0,-1};
		//const int level0DeltaY[7] = {0,0, 1,1,-1,-1, 0};

		//const int level0Pos[7] = {0,6,5,4,3,2,1};

		const NeighborTable neighborTable =
		{
			{1,2,3,4,5,6,-1},    // Neighbors of 0
			{3,5,0,-1,-1,-1,-1}, // Neighbors of 1
			{3,6,0,-1,-1,-1,-1},  // Neighbors of 2
			{0,1,2,-1,-1,-1,-1}, // Neighbors of 3
			{5,6,0,-1,-1,-1,-1},   // Neighbors of 4
			{0,1,4,-1,-1,-1,-1}, // Neighbors of 5
			{0,2,4,-1,-1,-1,-1}   // Neighbors of 6
		};

		const CommonIslTable commonIslTable =
		{
			{-1, 1, 2, 3, 4, 5, 6}, // Pos1=0
			{ 6,-1,-1, 2,-1, 4,-1}, // Pos1=1
			{ 5,-1,-1, 1,-1,-1, 4}, // Pos1=2,
			{ 4, 5, 6,-1,-1,-1,-1}, // Pos1=3,
			{ 3,-1,-1,-1,-1, 1, 2}, // Pos1=4
			{ 2, 3,-1,-1, 6,-1,-1}, // Pos1=5
			{ 1,-1, 3,-1, 5,-1,-1}  // Pos1=6
		};


		const ConnectedWithTable connectedWithTable =
		{
			{-1, 1, 2, 3, 4, 5, 6}, // Pos1=0
			{-1,-1, 3,-1, 5,-1, 0}, // Pos1=1
			{-1, 3,-1,-1, 6, 0,-1}, // Pos1=2
			{-1,-1,-1,-1, 0, 1, 2}, // Pos1=3
			{-1, 5, 6, 0,-1,-1,-1}, // Pos1=4
			{-1,-1, 0, 1,-1,-1, 4}, // Pos1=5
			{-1, 0,-1, 2,-1, 4,-1}  // Pos1=6
		};


		/*const int parentPositions[2][4][3]=
		{
		{ {0,0,1}, {2,5,2}, {1,6,2}, {3,4,2} },
		{ {1,6,2}, {3,4,2}, {0,0,1}, {2,5,2} }
		};*/
		const int parentPositions[2][4][3]=
		{
			{ {0,0,1}, {4,3,2}, {1,6,2}, {5,2,2} },
			{ {1,6,2}, {5,2,2}, {0,0,1}, {4,3,2} }
		};

		// Methods: IslandHierarchy
		int IslandHierarchy::numberOfLevels(int w, int h)
		{
			int initSize = (w>=h) ? w : h;
			int size=1;
			int levelCount=0;
			do
			{
				levelCount++;
				size*=2;
			} while (size<initSize || levelCount<2);

			return levelCount;
		}


		IslandHierarchy::IslandHierarchy(int w, int h)
		{
			levelCount=0;
			evenWidth=NULL;
			numOfIslands=NULL;
			levelDeltaX=NULL;
			levelDeltaY=NULL;
			init(w,h);  
		}


		void IslandHierarchy::init(int w, int h)
		{
			int initSize = (w>=h) ? w : h;
			int oldLevelCount = levelCount;

			size=1;
			levelCount=0;
			do
			{
				levelCount++;
				size*=2;
			} while (size<initSize || levelCount<2);
			// size = 2^n: Virtual Rastersize

			shiftX=(size-w)>>1;
			shiftY=(size-h)>>1;
			if (shiftX%2) shiftX-=1;
			if (shiftY%2) shiftY-=1;

			if (oldLevelCount!=levelCount)
			{
				int level,pos;

				if (evenWidth!=NULL) delete[] evenWidth;
				if (numOfIslands!=NULL) delete[] numOfIslands;
				if (levelDeltaX!=NULL)
				{
					for (level=0; level<oldLevelCount; level++)
						delete[] levelDeltaX[level];
					delete[] levelDeltaX;
				}
				if (levelDeltaY!=NULL)
				{
					for (level=0; level<oldLevelCount; level++)
						delete[] levelDeltaY[level];
					delete[] levelDeltaY;
				}
				//if (correctionNum!=NULL) delete[] correctionNum;

				levelDeltaX=new int*[levelCount];
				levelDeltaY=new int*[levelCount];
				for (level=0; level<levelCount; level++)
				{
					levelDeltaX[level]=new int[7];
					levelDeltaY[level]=new int[7];
				}
				for (pos=0; pos<7; pos++)
				{
					if (deltaY[pos]==0)
						levelDeltaX[0][pos]=2*deltaX[pos];
					else levelDeltaX[0][pos]=2*deltaX[pos]+1;
					levelDeltaY[0][pos]=2*deltaY[pos];
				}
				for (level=1; level<levelCount; level++)
					for (pos=0; pos<7; pos++)
					{
						levelDeltaX[level][pos]=2*levelDeltaX[level-1][pos];
						levelDeltaY[level][pos]=2*levelDeltaY[level-1][pos];
					}

					evenWidth=new int[levelCount];
					numOfIslands=new int[levelCount+1];
					evenWidth[0]=(size>>1)+1; // Virtual RasterSize div 2 +1
					numOfIslands[0]=0;
					for (level=1; level+1<levelCount; level++)
					{
						evenWidth[level]=(evenWidth[level-1]>>1)+1;
						numOfIslands[level]=evenWidth[level-1]*evenWidth[level]
						+(evenWidth[level-1]-1)*(evenWidth[level]-1)
							+numOfIslands[level-1];
					}
					evenWidth[levelCount-1]=1;
					numOfIslands[levelCount-1]=numOfIslands[levelCount-2]+7;
					numOfIslands[levelCount]=numOfIslands[levelCount-1]+1;

					for (int form=0; form<128; form++)
					{
						int minY=2;
						int minX=2;
						for (int pos=0; pos<7; pos++)
							if ((form & (1<<pos)) && (deltaY[pos]<minY || (deltaY[pos]==minY && deltaX[pos]<minX)))
							{
								minY=deltaY[pos];
								minX=deltaX[pos];
								minPos[form]=pos;
							}
					}
			}
		}


		IslandHierarchy::~IslandHierarchy()
		{
			if (evenWidth!=NULL) delete[] evenWidth;
			if (numOfIslands!=NULL) delete[] numOfIslands;

			int level;
			if (levelDeltaX!=NULL)
			{
				for (level=0; level<levelCount; level++)
					delete[] levelDeltaX[level];
				delete[] levelDeltaX;
			}
			if (levelDeltaY!=NULL)
			{
				for (level=0; level<levelCount; level++)
					delete[] levelDeltaY[level];
				delete[] levelDeltaY;
			}
			//if (correctionNum!=NULL) delete[] correctionNum;
		}


		void IslandHierarchy::getSubIslands(const Island& island, Island* subIslands) const
		{
			kiplAssert(island.level>0 && island.level<levelCount);

			int level=island.level-1;
			subIslands[0]=island;
			subIslands[0].level=level;

			subIslands[1].level=level;
			subIslands[1].y=subIslands[0].y+levelDeltaY[level][1];
			subIslands[1].x=subIslands[0].x+levelDeltaX[level][1];

			subIslands[2].level=level;
			subIslands[2].y=subIslands[0].y+levelDeltaY[level][2];
			subIslands[2].x=subIslands[0].x+levelDeltaX[level][2];

			subIslands[3].level=level;
			subIslands[3].y=subIslands[0].y+levelDeltaY[level][3];
			subIslands[3].x=subIslands[0].x+levelDeltaX[level][3];

			subIslands[4].level=level;
			subIslands[4].y=subIslands[0].y+levelDeltaY[level][4];
			subIslands[4].x=subIslands[0].x+levelDeltaX[level][4];

			subIslands[5].level=level;
			subIslands[5].y=subIslands[0].y+levelDeltaY[level][5];
			subIslands[5].x=subIslands[0].x+levelDeltaX[level][5];

			subIslands[6].level=level;
			subIslands[6].y=subIslands[0].y+levelDeltaY[level][6];
			subIslands[6].x=subIslands[0].x+levelDeltaX[level][6];
			/*  kiplAssert(island.level>0 && island.level<levelCount);

			if (island.level+2<levelCount)
			{
			subIslands[0].y=2*island.y;
			subIslands[0].x=2*island.x+(island.y & 1);
			}
			else if (island.level+2==levelCount)
			{
			subIslands[0].y=2*island.y;
			subIslands[0].x=2*island.x+1-(island.y & 1);
			}
			else if (island.level==levelCount-1)
			{
			subIslands[0].y=1;
			subIslands[0].x=1;
			}
			subIslands[0].level=island.level-1;

			subIslands[1].level=subIslands[0].level;
			subIslands[1].y=subIslands[0].y+deltaY[1];
			subIslands[1].x=subIslands[0].x+deltaX[1];

			subIslands[2].level=subIslands[0].level;
			subIslands[2].y=subIslands[0].y+deltaY[2];
			subIslands[2].x=subIslands[0].x+deltaX[2];

			subIslands[3].level=subIslands[0].level;
			subIslands[3].y=subIslands[0].y+deltaY[3];
			subIslands[3].x=subIslands[0].x+deltaX[3];

			subIslands[4].level=subIslands[0].level;
			subIslands[4].y=subIslands[0].y+deltaY[4];
			subIslands[4].x=subIslands[0].x+deltaX[4];

			subIslands[5].level=subIslands[0].level;
			subIslands[5].y=subIslands[0].y+deltaY[5];
			subIslands[5].x=subIslands[0].x+deltaX[5];

			subIslands[6].level=subIslands[0].level;
			subIslands[6].y=subIslands[0].y+deltaY[6];
			subIslands[6].x=subIslands[0].x+deltaX[6];*/
		}


		void IslandHierarchy::getMinCoord(const Island& island, unsigned char form, int& x, int& y, int& pos) const
		{
			pos=minPos[form];
			if (island.level==0)
			{
				x=island.x+deltaX[pos];
				y=island.y+deltaY[pos];
			}
			else
			{
				x=island.x+levelDeltaX[island.level-1][pos];
				y=island.y+levelDeltaY[island.level-1][pos];
			}

			/*  if (island.level+2<levelCount)
			{
			y=2*island.y;
			x=2*island.x+(island.y & 1);
			}
			else if (island.level+2==levelCount)
			{
			y=2*island.y;
			x=2*island.x+1-(island.y & 1);
			}
			else if (island.level==levelCount-1)
			{
			y=1;
			x=1;
			}
			y+=deltaY[pos];
			x+=deltaX[pos];*/
		}


		void IslandHierarchy::getSubIsland(const Island& island, int pos, Island& subIsland) const
		{
			kiplAssert(island.level>0 && island.level<levelCount);

			int level=island.level-1;
			subIsland=island;
			subIsland.level=level;
			subIsland.x+=levelDeltaX[level][pos];
			subIsland.y+=levelDeltaY[level][pos];
			/*  kiplAssert(island.level>0 && island.level<levelCount);

			if (island.level+2<levelCount)
			{
			subIsland.y=2*island.y+deltaY[pos];
			subIsland.x=2*island.x+(island.y & 1)+deltaX[pos];
			}
			else if (island.level+2==levelCount)
			{
			subIsland.y=2*island.y+deltaY[pos];
			subIsland.x=2*island.x+1-(island.y & 1)+deltaX[pos];
			}
			else if (island.level==levelCount-1)
			{
			subIsland.y=1+deltaY[pos];
			subIsland.x=1+deltaX[pos];
			}
			subIsland.level=island.level-1;*/
		}


		void IslandHierarchy::getParentIsland(const Island& island, int relPos, Island& parentIsland) const
		{
			parentIsland.level=island.level+1;
			parentIsland.x=island.x-levelDeltaX[island.level][relPos];
			parentIsland.y=island.y-levelDeltaY[island.level][relPos];

			/*int cx=island.x-deltaX[relPos];

			parentIsland.level=island.level+1;
			if (island.level+3<levelCount)
			{
			parentIsland.y=(island.y-deltaY[relPos])/2;
			parentIsland.x=(cx-(cx & 1))/2; // Short for: island.x=(cx%2==0)?cx:cx-1;
			}
			else if (island.level+2<levelCount)
			{
			parentIsland.y=(island.y-deltaY[relPos])/2;
			parentIsland.x=(cx+1-(cx & 1))/2;
			}
			else
			{
			parentIsland.x=size>>1;
			parentIsland.y=size>>1;
			}*/
		}


		int IslandHierarchy::getParentIslands(int x, int y, int subPos[2], Island parents[2]) const
		{
			x+=shiftX;
			y+=shiftY;

			subPos[0]=parentPositions[x%2][y%4][0];
			subPos[1]=parentPositions[x%2][y%4][1];
			//cout << subPos[0] << "|" << subPos[1] << endl;

			parents[0].level=0;
			parents[0].x=x-deltaX[subPos[0]];
			parents[0].y=y-deltaY[subPos[0]];

			parents[1].level=0;
			parents[1].x=x-deltaX[subPos[1]];
			parents[1].y=y-deltaY[subPos[1]];

			/*  kiplAssert(false);
			Island current;

			current.level=-1;
			current.x=x;
			current.y=y;
			subPos[0]=parentPositions[x%2][y%4][0];
			subPos[1]=parentPositions[x%2][y%4][1];
			getParentIsland(current,subPos[0],parents[0]);
			getParentIsland(current,subPos[1],parents[1]);*/

			return parentPositions[x%2][y%4][2];
		}


		void IslandHierarchy::getSubPixels(const Island& island, int* xcoord, int* ycoord) const
		{
			kiplAssert(island.level==0);
			ycoord[0]=island.y-shiftY;
			xcoord[0]=island.x-shiftX;

			ycoord[1]=ycoord[0]+deltaY[1];
			xcoord[1]=xcoord[0]+deltaX[1];

			ycoord[2]=ycoord[0]+deltaY[2];
			xcoord[2]=xcoord[0]+deltaX[2];

			ycoord[3]=ycoord[0]+deltaY[3];
			xcoord[3]=xcoord[0]+deltaX[3];

			ycoord[4]=ycoord[0]+deltaY[4];
			xcoord[4]=xcoord[0]+deltaX[4];

			ycoord[5]=ycoord[0]+deltaY[5];
			xcoord[5]=xcoord[0]+deltaX[5];

			ycoord[6]=ycoord[0]+deltaY[6];
			xcoord[6]=xcoord[0]+deltaX[6];

			/*  kiplAssert(island.level==0);
			if (island.level+2<levelCount)
			{
			ycoord[0]=2*island.y;
			xcoord[0]=2*island.x+(island.y & 1);
			}
			else if (island.level+2==levelCount)
			{
			ycoord[0]=2*island.y;
			xcoord[0]=2*island.x+1-(island.y & 1);
			}
			else if (island.level==levelCount-1)
			{
			ycoord[0]=1;
			xcoord[0]=1;
			}

			ycoord[1]=ycoord[0]+deltaY[1];
			xcoord[1]=xcoord[0]+deltaX[1];

			ycoord[2]=ycoord[0]+deltaY[2];
			xcoord[2]=xcoord[0]+deltaX[2];

			ycoord[3]=ycoord[0]+deltaY[3];
			xcoord[3]=xcoord[0]+deltaX[3];

			ycoord[4]=ycoord[0]+deltaY[4];
			xcoord[4]=xcoord[0]+deltaX[4];

			ycoord[5]=ycoord[0]+deltaY[5];
			xcoord[5]=xcoord[0]+deltaX[5];

			ycoord[6]=ycoord[0]+deltaY[6];
			xcoord[6]=xcoord[0]+deltaX[6];
			*/
		}


		int IslandHierarchy::maxIslandKey() const
		{
			Island topIsland;

			topIsland.level=levelCount-1;
			topIsland.x=size >> 1;
			topIsland.y=size >> 1;

			return getIslandKey(topIsland);
		}


		int IslandHierarchy::numberOfIslands(int level) const
		{
			kiplAssert(level>=0 && level<levelCount);
			return numOfIslands[level+1]-numOfIslands[level]/*-correctionNum[level]*/;
		}


		int IslandHierarchy::numberOfIslands() const
		{
			return numOfIslands[levelCount];//numOfIslands[levelCount]-2;
		}


		int IslandHierarchy::numberOfLevels() const
		{
			return levelCount;
		}


		BottomUpIslandIterator* IslandHierarchy::createIteratorBU() const
		{
			return new BottomUpIslandIterator(*this);
		}


		TopDownIslandIterator* IslandHierarchy::createIteratorTD() const
		{
			return new TopDownIslandIterator(*this);
		}


		const CommonIslTable& IslandHierarchy::getCommonIslandTable() const
		{
			return commonIslTable;
		}

		const NeighborTable& IslandHierarchy::getNeighborTable() const
		{
			return neighborTable;
		}


		const ConnectedWithTable& IslandHierarchy::getConnectedWithTable() const
		{
			return connectedWithTable;
		}


		int IslandHierarchy::getSize() const
		{
			return size;
		}

		// Methods: BottomUpIslandIterator
		BottomUpIslandIterator::BottomUpIslandIterator(const IslandHierarchy& initHierarchy)
		{
			hierarchy=&initHierarchy;
			begin();
		}


		bool BottomUpIslandIterator::begin()
		{
			currIsland.level=0;

			width=hierarchy->size+1;
			levelDeltaX=hierarchy->levelDeltaX[0];
			levelDeltaY=hierarchy->levelDeltaY[0];
			if (currIsland.level+1==hierarchy->levelCount)
			{ 
				currIsland.x=hierarchy->size >> 1;
				currIsland.y=hierarchy->size >> 1;
			}
			else if (currIsland.level+2==hierarchy->levelCount)
			{
				currIsland.x=levelDeltaX[3];
				currIsland.y=0;
			}
			else
			{
				currIsland.x=0;
				currIsland.y=0;
			}

			levelWidth=hierarchy->evenWidth[0];
			firstLine=0;

			return true;
		}


		bool BottomUpIslandIterator::nextIsland()
		{
			currIsland.x+=levelDeltaX[1];
			if (currIsland.level+1==hierarchy->levelCount) return false;
			else if (currIsland.level+2==hierarchy->levelCount)
			{
				if (currIsland.x>=width)
				{
					currIsland.y+=levelDeltaY[3]+levelDeltaY[2];
					if (currIsland.y+firstLine>width && firstLine==0) 
					{
						firstLine=1;
						currIsland.y=levelDeltaY[3];
					}
					currIsland.x=(firstLine==0) ? levelDeltaX[3] : 0;
				}
			}
			else
			{
				if (currIsland.x>=width)
				{
					currIsland.y+=levelDeltaY[3]+levelDeltaY[2];
					if (currIsland.y>width && firstLine==0)
					{
						firstLine=1;
						currIsland.y=levelDeltaY[3];
					}
					currIsland.x=(firstLine==0) ? 0 : levelDeltaX[3];
				}
			}
			if (currIsland.y<=width) return true;
			return false;
			/*  currIsland.x++;
			if (currIsland.level+2==hierarchy->levelCount)
			{
			if (currIsland.x+1>=levelWidth+firstLine)
			{
			currIsland.y+=2;
			if (currIsland.y>=levelWidth && firstLine==0) 
			{
			firstLine=1;
			currIsland.y=1;
			}
			currIsland.x=0;
			}
			}
			else
			{
			if (currIsland.x+firstLine>=levelWidth)
			{
			currIsland.y+=2;
			if (currIsland.y>=levelWidth && firstLine==0) 
			{
			firstLine=1;
			currIsland.y=1;
			}
			currIsland.x=0;
			}
			}
			if (currIsland.y<levelWidth) return true;
			return false;*/
			/*  int modifier = (currIsland.y & 1);
			currIsland.x++;
			if (currIsland.level+2==hierarchy->levelCount)
			{
			if (currIsland.x+1>=levelWidth+modifier)
			{
			currIsland.y++;
			currIsland.x=0;
			}
			}
			else
			{
			if (currIsland.x+modifier>=levelWidth)
			{
			currIsland.y++;
			currIsland.x=0;
			}
			}
			if (currIsland.y<levelWidth) return true;
			return false;*/
		}


		bool BottomUpIslandIterator::nextLevel()
		{
			currIsland.level++;
			if (currIsland.level==hierarchy->levelCount) return false;
			levelDeltaX=hierarchy->levelDeltaX[currIsland.level];
			levelDeltaY=hierarchy->levelDeltaY[currIsland.level];
			if (currIsland.level+1==hierarchy->levelCount)
			{ 
				currIsland.x=hierarchy->size >> 1;
				currIsland.y=hierarchy->size >> 1;
			}
			else if (currIsland.level+2==hierarchy->levelCount)
			{
				currIsland.x=levelDeltaX[3];
				currIsland.y=0;
			}
			else
			{
				currIsland.x=0;
				currIsland.y=0;
			}
			firstLine=0;

			/*  currIsland.level++;
			currIsland.x=0;
			currIsland.y=0;
			levelWidth=hierarchy->evenWidth[currIsland.level];
			firstLine=0;*/

			return (currIsland.level<hierarchy->levelCount);
		}


		// Methods: TopDownIslandIterator
		TopDownIslandIterator::TopDownIslandIterator(const IslandHierarchy& initHierarchy)
		{
			hierarchy=&initHierarchy;
			begin();
		}


		bool TopDownIslandIterator::begin()
		{
			currIsland.level=hierarchy->levelCount-1;

			width=hierarchy->size+1;
			levelDeltaX=hierarchy->levelDeltaX[currIsland.level];
			levelDeltaY=hierarchy->levelDeltaY[currIsland.level];
			if (currIsland.level+1==hierarchy->levelCount)
			{ 
				currIsland.x=hierarchy->size >> 1;
				currIsland.y=hierarchy->size >> 1;
			}
			else if (currIsland.level+2==hierarchy->levelCount)
			{
				currIsland.x=levelDeltaX[3];
				currIsland.y=0;
			}
			else
			{
				currIsland.x=0;
				currIsland.y=0;
			}

			levelWidth=hierarchy->evenWidth[0];
			firstLine=0;

			return true;

			/*  firstLine=0;
			currIsland.level=hierarchy->levelCount-1;
			currIsland.x=0;
			currIsland.y=0;
			levelWidth=hierarchy->evenWidth[currIsland.level];

			return true;*/
		}


		bool TopDownIslandIterator::nextIsland()
		{
			currIsland.x+=levelDeltaX[1];
			if (currIsland.level+1==hierarchy->levelCount) return false;
			else if (currIsland.level+2==hierarchy->levelCount)
			{
				if (currIsland.x>=width)
				{
					currIsland.y+=levelDeltaY[3]+levelDeltaY[2];
					if (currIsland.y>width && firstLine==0) 
					{
						firstLine=1;
						currIsland.y=levelDeltaY[3];
					}
					currIsland.x=(firstLine==0) ? levelDeltaX[3] : 0;
				}
			}
			else
			{
				if (currIsland.x>=width)
				{
					currIsland.y+=levelDeltaY[3]+levelDeltaY[2];
					if (currIsland.y>width && firstLine==0)
					{
						firstLine=1;
						currIsland.y=levelDeltaY[3];
					}
					currIsland.x=(firstLine==0) ? 0 : levelDeltaX[3];
				}
			}
			if (currIsland.y<=width) return true;
			return false;
			/*  currIsland.x++;
			if (currIsland.level+2==hierarchy->levelCount)
			{
			if (currIsland.x+1>=levelWidth+firstLine)
			{
			currIsland.y+=2;
			if (currIsland.y>=levelWidth && firstLine==0) 
			{
			firstLine=1;
			currIsland.y=1;
			}
			currIsland.x=0;
			}
			}
			else
			{
			if (currIsland.x+firstLine>=levelWidth)
			{
			currIsland.y+=2;
			if (currIsland.y>=levelWidth && firstLine==0) 
			{
			firstLine=1;
			currIsland.y=1;
			}
			currIsland.x=0;
			}
			}
			if (currIsland.y<levelWidth) return true;
			return false;*/
			/*  int modifier = (currIsland.y & 1);
			currIsland.x++;
			if (currIsland.level+2==hierarchy->levelCount)
			{
			if (currIsland.x+1>=levelWidth+modifier)
			{
			currIsland.y++;
			currIsland.x=0;
			}
			}
			else
			{
			if (currIsland.x+modifier>=levelWidth)
			{
			currIsland.y++;
			currIsland.x=0;
			}
			}
			if (currIsland.y<levelWidth) return true;
			return false;*/
		}


		bool TopDownIslandIterator::nextLevel()
		{
			currIsland.level--;
			if (currIsland.level<0) return false;
			levelDeltaX=hierarchy->levelDeltaX[currIsland.level];
			levelDeltaY=hierarchy->levelDeltaY[currIsland.level];
			if (currIsland.level+2==hierarchy->levelCount)
			{
				currIsland.x=levelDeltaX[3];
				currIsland.y=0;
			}
			else
			{
				currIsland.x=0;
				currIsland.y=0;
			}
			firstLine=0;

			/*  currIsland.level++;
			currIsland.x=0;
			currIsland.y=0;
			levelWidth=hierarchy->evenWidth[currIsland.level];
			firstLine=0;*/

			return (currIsland.level>=0);
		}
	}
}

ostream& operator << (ostream& out, const kipl::csc::Island& island)
{
	out << "[Level:" << island.level << ",x=" << island.x << ",y=" << island.y << "]";

	return out;
}
