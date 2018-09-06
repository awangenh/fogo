#ifndef _kipl_base_h_
#define _kipl_base_h_

// Include-Section
#include <iostream>
#include <map>
#include <vector>
#include <string>

// Declaration-Section
namespace kipl {

	class Base {
	public:
		virtual void assign(const Base* obj) = 0;
		virtual Base* clone() const = 0;
		virtual ~Base() {};

		virtual void printTo(std::ostream& str) const;
	};

	class Properties: public Base {
	private:
		std::map<std::string,Base*> propertyMap;

		void copyPropertyMap(const std::map<std::string,Base*>& _propertyMap);
	public:
		Properties();
		Properties(const Properties& properties);

		virtual void assign(const Base* obj);
		virtual Base* clone() const;
		virtual ~Properties();

		//return vector of all properties
		void getAllProperties(std::vector<std::pair<std::string,Base*> >& propout);

		Base* getProperty(const std::string& key) const;

		/*! Adds a content to the property table with the given key. 
		The life of content ends when the property table is destroyed.
		*/
		void setProperty(const std::string& key, Base* content);

		/*! A copy of content will be added with the given key*/
		void setProperty(const std::string& key, const Base& content);

		void moveProperty(const std::string& key, Properties& properties);
		void moveProperties(Properties& properties);
		void clearProperties();
		void removeProperty(const std::string& key);
		virtual void printTo(std::ostream& str) const;
	};

	template <typename Type>
	class SimpleProperty: public Base {
	protected:
		Type data;
	public:
		const Type& getData() const { return data; };
		void setData(const Type& _data) { data=_data; };

		SimpleProperty() {}
		SimpleProperty(const Type& _data) { data=_data; };
		void assign(const Base* obj) {
			const SimpleProperty<Type>* prop = dynamic_cast<const SimpleProperty<Type>*>(obj);
			if (prop) setData(prop->data);
		}

		Base* clone() const {
			SimpleProperty<Type>* obj = new SimpleProperty<Type>;
			obj->assign(this);
			return obj;
		}

		virtual void printTo(std::ostream& str) const {
			str << data;
		}
	};

	template <typename ContainerType>
	class ContainerProperty: public Base {
	protected:
		ContainerType data;
	public:
		const ContainerType& getData() const { return data; };
		void setData(const ContainerType& _data) { data=_data; };

		ContainerProperty() {}
		ContainerProperty(const ContainerType& _data) { data=_data; };
		void assign(const Base* obj) {
			const ContainerProperty<ContainerType>* prop = dynamic_cast<const ContainerProperty<ContainerType>*>(obj);
			if (prop) setData(prop->data);
		}

		Base* clone() const {
			ContainerProperty<ContainerType>* obj = new ContainerProperty<ContainerType>;
			obj->assign(this);
			return obj;
		}

		virtual void printTo(std::ostream& str) const{
			for (typename ContainerType::const_iterator it = data.begin(); it != data.end(); ++it){
				str << *it;
			}
		}
	};

	class StringListProperty: public Base {
	protected:
		std::vector<std::string> data;
	public:
		const std::vector<std::string>& getData() const { return data; };
		void setData(const std::vector<std::string>& _data) { data=_data; };

		StringListProperty() {}
		StringListProperty(const std::vector<std::string>& _data) { data=_data; };
		void assign(const Base* obj) {
			const StringListProperty* prop = dynamic_cast<const StringListProperty*>(obj);
			if (prop) setData(prop->data);
		}

		Base* clone() const {
			StringListProperty* obj = new StringListProperty;
			obj->assign(this);
			return obj;
		}

		virtual void printTo(std::ostream& str) const{
			for (unsigned i = 0; i<data.size(); ++i){
				std::string s = data[i];
				for (std::string::size_type pos = s.find("_"); pos!=std::string::npos; pos = s.find("_",pos+2)){
					s.replace(pos, 1, "__");
				}
				for (std::string::size_type pos = s.find(";"); pos!=std::string::npos; pos = s.find(";",pos+2)){
					s.replace(pos, 1, "_;");
				}
				if (i==0)
					str << s;
				else
					str << ";" << s;
			}
		}
	};
	
	typedef SimpleProperty<std::string> StringProperty;
	typedef SimpleProperty<bool> BoolProperty;
	typedef SimpleProperty<char> CharProperty;
	typedef SimpleProperty<int> IntProperty;
	typedef SimpleProperty<float> FloatProperty;
	typedef SimpleProperty<double> DoubleProperty;
	typedef SimpleProperty<unsigned> UnsignedProperty;
	
	template <typename ValueType>
	void setSimpleProperty(Properties& prop, const std::string& key, const ValueType& val) {
		SimpleProperty<ValueType>* obj = new SimpleProperty<ValueType>(val);
		prop.setProperty(key,obj);
	}

	template <typename ValueType>
	ValueType getSimpleProperty(const Properties& prop, const std::string& key) {
		SimpleProperty<ValueType>* val = dynamic_cast<SimpleProperty<ValueType>*>(prop.getProperty(key)); 
		if (val) return val->getData();
		return ValueType();
	}
	
}

#endif
