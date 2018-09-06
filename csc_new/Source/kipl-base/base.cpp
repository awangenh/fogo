#include <kipl-base/base.h>


namespace kipl {

	void Base::printTo(std::ostream& str) const {
		str << (void*)this;
	}

	Properties::Properties() {
	}


	Properties::Properties(const Properties& properties) {
		copyPropertyMap(properties.propertyMap);			
	}

	void Properties::assign(const Base* obj) {
		const Properties* prop = dynamic_cast<const Properties*>(obj);
		if (prop) copyPropertyMap(prop->propertyMap);
	}

	void Properties::copyPropertyMap(const std::map<std::string,Base*>& _propertyMap) {
		clearProperties();

		std::map<std::string,Base*>::const_iterator it = _propertyMap.begin();		
		std::map<std::string,Base*>::const_iterator end = _propertyMap.end();

		while (it!=end) {
			if (it->second)
				propertyMap[it->first]=it->second->clone();
			else propertyMap[it->first]=NULL;
			it++;
		}
	}

	Base* Properties::clone() const {
		return new Properties(*this);
	}

	Properties::~Properties() {
		clearProperties();
	}
		
	Base* Properties::getProperty(const std::string& key) const {
		std::map<std::string,Base*>::const_iterator it = propertyMap.find(key);
		if (it==propertyMap.end()) return NULL;

		return it->second;
	}

	void Properties::getAllProperties(std::vector<std::pair<std::string,Base*> >& propout){
		propout.clear();
		std::map<std::string,Base*>::const_iterator it = propertyMap.begin();
		while (it!=propertyMap.end()){
			propout.push_back(std::pair<std::string,Base*>(it->first,it->second));
			++it;
		}
	}
	
	void Properties::removeProperty(const std::string& key) {
		std::map<std::string,Base*>::iterator it = propertyMap.find(key);
		if (it!=propertyMap.end()) {
			if (it->second) delete it->second;
			propertyMap.erase(it);
		}
	}

	void Properties::moveProperty(const std::string& key, Properties& properties) {
		std::map<std::string,Base*>::iterator it = propertyMap.find(key);
		if (it!=propertyMap.end()) {
			properties.setProperty(key,it->second);
			propertyMap.erase(it);
		}
	}


	void Properties::moveProperties(Properties& properties) {
		std::map<std::string,Base*>::iterator old,it = propertyMap.begin();
		std::map<std::string,Base*>::iterator itEnd = propertyMap.end();

		while (it!=itEnd) {
			properties.setProperty(it->first,it->second);
			old=it;
			++it;
			propertyMap.erase(old);
		}
	}

	void Properties::setProperty(const std::string& key, Base* content) {
		removeProperty(key);
		propertyMap[key]=content;
	}

	void Properties::setProperty(const std::string& key, const Base& content) {
		setProperty(key,content.clone());
	}

	void Properties::clearProperties() {
		//std::cout << "clearProperties deactivated" << std::endl; return;
		std::map<std::string,Base*>::const_iterator it = propertyMap.begin();		
		std::map<std::string,Base*>::const_iterator end = propertyMap.end();

		while (it!=end) {
			if (it->second) delete it->second;
			it++;
		}
		propertyMap.clear();
	}

	void Properties::printTo(std::ostream& str) const {
		std::map<std::string,Base*>::const_iterator it = propertyMap.begin();		
		std::map<std::string,Base*>::const_iterator end = propertyMap.end();

		while (it!=end) {
			str << "[--" << it->first << "------" << std::endl;
			if (it->second)	it->second->printTo(str);
			else str << "(null)";
			str << std::endl << "--------]" << std::endl;
			it++;
		}
	}


};


