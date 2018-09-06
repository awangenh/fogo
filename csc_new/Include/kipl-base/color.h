#ifndef _kipl_color_h_
#define _kipl_color_h_

// Include-Section
#include <kipl-base/types.h>

// Delclaration-Section
namespace kipl {

	typedef UINT8 GRAY;
	typedef UINT16 GRAY16;
	typedef UINT32 GRAY32;
	typedef float GRAYFLOAT;

	struct RGB {
		UINT8 red,green,blue;
	};

	struct RGB16 {
		UINT16 red,green,blue;
	};

	struct HSV {
		UINT16 hue;
		UINT8 sat,val;
	};

	struct HSV16 {
		UINT16 hue,sat,val;
	};

} // end of namespace

#endif
