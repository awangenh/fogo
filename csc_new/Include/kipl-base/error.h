#ifndef _kipl_error_h_
#define _kipl_error_h_

// Include-Section
#include <exception>
#include <string>
#include <stdexcept>

// DEFINES
#define KIPL_NEW_STD_EXCEPTION(CLASSNAME,DEFAULTMSG) KIPL_NEW_SUB_EXCEPTION(CLASSNAME,Exception,DEFAULTMSG)
#define KIPL_NEW_SUB_EXCEPTION(CLASSNAME,INHERITFROM,DEFAULTMSG) class CLASSNAME: public INHERITFROM { public: CLASSNAME(const char* msg): INHERITFROM(msg) {}; CLASSNAME(): INHERITFROM(DEFAULTMSG) {}; };

namespace kipl {

	//! Base exception class
	/*! This class is the base class for all kipl exceptions. The class saves a message std::string
	containing a error message.

	\author Patrick Sturm
	*/
	class Exception: public std::runtime_error
	{
	private:
		std::string message;

	public:
		//! The constructor creates a new exception with message msg
		/*!
		\param msg message text
		*/
		Exception(const std::string& msg = "");

		//! Destructor
		virtual ~Exception() throw();

		//! The method changes the message text of the exception
		/*!
		\param msg new message text
		*/
		void setMessage(const std::string& msg);

		//! The method returns the message text of the exception
		const char* getMessage() const;

		virtual const char* what() const throw();
	};


	//! Exception of this type are thrown by kiplAssert.
	/*! kiplAssert(cond) throws a exception of type kiplAssertion if condition cond is false.
	\author Patrick Sturm
	*/
	class Assertion: public Exception
	{
	public:
		Assertion(const std::string& cond, const std::string& file, int line);
	};


	extern void _assert(const std::string&  __cond, const std::string&  __file, int __line);


#ifdef kiplAssert
#undef kiplAssert
#endif

#ifdef NDEBUG
#define kiplAssert(p)   ((void)0)
#else
#define kiplAssert(p)   ((p) ? (void)0 : kipl::_assert(#p, __FILE__, __LINE__))
#endif


// Standard-Error-Exceptions
KIPL_NEW_STD_EXCEPTION(EFileNotFound,"File not found...")
KIPL_NEW_STD_EXCEPTION(EOutOfMemory, "Out of memory...")
KIPL_NEW_STD_EXCEPTION(ENotImplemented, "Not implemented yet...")
KIPL_NEW_STD_EXCEPTION(EInvalidArgument, "Invalid argument...")

}

#endif
