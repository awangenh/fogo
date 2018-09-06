#ifndef APP_HPP_
#define APP_HPP_

#include <wx/wx.h>

#include <kipl-csc/csc.h>
#include <kipl-base/color.h>
#include <kipl-transform/color-transform.h>
#include <kipl-segmentation/segm-stat.h>
#include <kipl-csc/csctypes.h>
//#include <kipl-csc/stdgraylinkcontrol.h>
#include <kipl-csc/stdrgblinkcontrol.h>

class CscApp : public wxApp {
	public:
		virtual bool OnInit();
		
	private:
		unsigned callHSVCSC(kipl::csc::CSCParameters& p, kipl::Image* original, kipl::Image* region, kipl::Image* label);
		unsigned callRGBCSC(kipl::csc::CSCParameters& p, kipl::Image* original, kipl::Image* region, kipl::Image* label);
		kipl::Image* CSCImage(const wxString& path);
		bool ToFile(kipl::Image* image, const wxString& path);
};


#endif /*APP_HPP_*/
