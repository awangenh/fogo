#ifndef _kipl_csctypes_h_
#define _kipl_csctypes_h_

#include <string>
#include <iostream>

namespace kipl {
	namespace csc {
		// struct CPluginCSCState
		struct CSCParameters
		{
			int csf,startPosition;
			float threshold;
			bool exact,bestNeighbor;
			int splitting,repair;
			int maxLinkLevel;
		};

	}
}

#endif
