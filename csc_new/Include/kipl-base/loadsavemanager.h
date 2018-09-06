#ifndef _kipl_loadSaveManager_h_
#define _kipl_loadSaveManager_h_

#include <string>
#include <vector>
#include <kipl-base/types.h>
#include <kipl-base/error.h>

namespace kipl {

// function to load an image
//                   filename       width          height         data 
// data is an array of imagelayers in kipl, but only one layer is used yet, so return a pointer to
// an one-dimensional imagearray here!
typedef int(*loadf)(const char*, unsigned int*, unsigned int*, unsigned char**);

// function to save an image
//                    filename   imageformat      width        height      data to save (in RGB)
typedef int(*savef)(const char*, const char*, unsigned int, unsigned int, unsigned char*);

// function which returns, which formats are supported
// if 0 is passed, it will return the loadable formats,
// if 1 is passed, it returns the saveable formats (as an "jpeg,tiff,bmp" like string)
typedef char*(*suppf)(int);

	class LSPlugIn {
	public:
		LSPlugIn(const std::string &fileName);
		~LSPlugIn();
		bool isValide(void) {return _isValide; }
		bool canLoad(const std::string &extension);
		bool canSave(const std::string &extension);
		bool canHandle(const std::string &extension, const std::string &formats);
		
		loadf loadImage;
		savef saveImage;
	private:
		bool _isValide;
		void *handle;
		std::string supportedLoadFormats;
		std::string supportedSaveFormats;
		
		void strupper(std::string &str);
	};

	
	class LoadSaveManager {
	public:
		~LoadSaveManager();
		static LoadSaveManager* Instance(void);
		void loadImage(const std::string &fileName,                            unsigned &width, unsigned &height, UINT8 **data);
		void saveImage(const std::string &fileName, const std::string &format, unsigned  width, unsigned  height, UINT8 *data);
	private:
		static LoadSaveManager *_instance;
		LoadSaveManager(void);
		
		std::string getExtension(const std::string& fileName);
		bool hasExtension(const std::string& name, const std::string& ext);
		std::vector<LSPlugIn*> plugins;
	};
}

#endif
