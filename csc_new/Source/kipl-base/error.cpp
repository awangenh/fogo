
// Include-Section
#include <kipl-base/error.h>
#include <iostream>
#include <sstream>


kipl::Exception::Exception(const std::string& msg) : runtime_error(msg){
		message=msg;
}

kipl::Exception::~Exception() throw() {};


void kipl::Exception::setMessage(const std::string& msg) {
	message=msg;
}

const char* kipl::Exception::getMessage() const { 
	return message.c_str(); 
}

const char* kipl::Exception::what() const throw() {
	return message.c_str();
}

kipl::Assertion::Assertion(const std::string& cond, const std::string& file, int line) {
	std::ostringstream stream;

	stream << "Assertion (Cond[" << cond << "], File[" << file << "], Line [" << line << "])";
	setMessage(stream.str().c_str());
}


void kipl::_assert(const std::string& __cond, const std::string& __file, int __line) {
	throw kipl::Assertion(__cond,__file,__line);
}
