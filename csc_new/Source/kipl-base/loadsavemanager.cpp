#include <kipl-base/loadsavemanager.h>
#include <iostream>
#include <cstring>
#include <kipl-base/error.h>

#ifdef WIN32

#else
	#include <dirent.h>
	#include <dlfcn.h> // dlopen dlclose dlsym
#endif //_WIN32

using namespace std;

namespace kipl {

	LSPlugIn::LSPlugIn(const std::string &fileName) {
		_isValide  = false;
		loadImage = NULL;
		saveImage = NULL;
#ifdef WIN32
#else
		handle = dlopen(fileName.c_str(), RTLD_LAZY);
		if (!handle) {
			cerr << fileName << " has no handle" << endl;
			return;
		}
		char *error;
		error = dlerror();
		
		suppf supportedFormatsF = (suppf) dlsym(handle, "supportedFormats");
		error = dlerror();
		if (error) {
			cerr << fileName << "has no supportedFormats()" << endl;
			dlclose(handle);
			return;
		}
		
		loadImage = (loadf) dlsym(handle, "loadImage");
		error = dlerror();
		if (error) {
			cerr << fileName << " has no loadImage()" << endl;
			loadImage = NULL;
		} else {
			supportedLoadFormats = supportedFormatsF(0);
			strupper(supportedLoadFormats);
		}
		
		saveImage = (savef) dlsym(handle, "saveImage");
		error = dlerror();
		if (error) {
			cerr << fileName << " has no saveImage()" << endl;
			saveImage = NULL;
		} else {
			supportedSaveFormats = supportedFormatsF(1);
			strupper(supportedSaveFormats);
		}
		_isValide = true;
		
#endif //WIN32
	}
	
	LSPlugIn::~LSPlugIn() {
#ifdef WIN32
#else
		if (isValide()) {
			dlclose(handle);
		}
#endif //WIN32
	}
	
	
	bool LSPlugIn::canLoad(const std::string &extension) {
		return canHandle(extension, supportedLoadFormats);
	}
	
	bool LSPlugIn::canSave(const std::string &extension) {
		std::cout << "OI\n";
		std::cout << supportedSaveFormats << "\n";
		
		return canHandle(extension, supportedSaveFormats);
	}
	
	bool LSPlugIn::canHandle(const std::string &extension, const std::string &formats) {
		string ext = extension;
		strupper(ext);
		string::size_type i = formats.find(ext);
		if (i != string::npos) return true;
		return false;
	}
	
	void LSPlugIn::strupper(string &str) {
		for (unsigned i = 0; i < str.length(); ++i)
			str[i] = (char) toupper(str[i]);
	}


	LoadSaveManager* LoadSaveManager::_instance = 0;

	LoadSaveManager* LoadSaveManager::Instance(void) {
		if (_instance == 0) {
			_instance = new LoadSaveManager();
		}
		return _instance;
	}

	
	LoadSaveManager::LoadSaveManager(void) {
		// TODO: use Dir from toolsfiles as soon as it's an own lib
#ifdef WIN32
#else
		//cout << "LoadSaveManager()" << endl;
		cout << "SHIT!" << endl;
		DIR *dir;
		struct dirent *dirzeiger;
		if((dir=opendir(".")) != NULL) {
			while((dirzeiger = readdir(dir)) != NULL) {
				//cout << "LoadSaveManager: filename: " << (*dirzeiger).d_name << endl;
				if (hasExtension((*dirzeiger).d_name, "so") && ((*dirzeiger).d_name != "imageLoaderSaver.so")) {
					LSPlugIn *p = new LSPlugIn( (*dirzeiger).d_name );
					if ( p->isValide() ) {
						plugins.push_back(p);
					} else {
						delete p;
					}
				}
			}
		}
		closedir(dir);
#endif //WIN32
	}
	
	LoadSaveManager::~LoadSaveManager(void) {
		for (unsigned int i = 0; i < plugins.size(); ++i) {
			delete plugins[i];
		}
	}
	
	void LoadSaveManager::loadImage(const std::string &fileName, unsigned &width, unsigned &height, UINT8 **data) {
		//cout << "LoadSaveManager::loadImage()" << endl;
		string ext = getExtension(fileName);
		
		//data = NULL;
		for (unsigned int i = 0; i < plugins.size(); ++i) {
			if (plugins[i]->canLoad(ext)) {
				//cout << "LoadSaveManager: this plugin could work..." << endl;
				plugins[i]->loadImage(fileName.c_str(), &width, &height, data);
				//if (data != NULL) return;
				return;
			}
		}
		cout << "LoadSaveManager: couldn't load " << fileName << endl;
		throw EInvalidArgument(); //TODO: better error
	}
	
	void LoadSaveManager::saveImage(const std::string &fileName, const std::string &format, unsigned width, unsigned height, UINT8 *data) {
		cout << plugins.size() << "\n";
		for (unsigned int i = 0; i < plugins.size(); ++i) {			
			if (plugins[i]->canSave(format)) {
				plugins[i]->saveImage(fileName.c_str(), format.c_str(), width, height, data);
				return;
			}
		}
		cout << "LoadSaveManager: couldn't save " << fileName << endl;
		throw EInvalidArgument(); //TODO: better error
	}
	
	bool LoadSaveManager::hasExtension(const std::string& name, const std::string& ext) {
		//if (name.length() <= ext.length()) return false;
		//return (name.substr(name.length()-ext.length(),ext.length()) == ext);
		//cout << "is " << getExtension(name) << " == " << ext << " ?" << endl;
		return (getExtension(name) == ext);
	}
	
	string LoadSaveManager::getExtension(const std::string& fileName) {
		string::size_type i = fileName.rfind(".");
		if (i == string::npos) return "";
		return fileName.substr(i+1, fileName.size());
	}
	
}

