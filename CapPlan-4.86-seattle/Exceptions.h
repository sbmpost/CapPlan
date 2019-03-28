//$Id: Exceptions.h,v 1.35 2013/12/21 20:00:16 darseq Exp $
#ifndef ExceptionsH
#define ExceptionsH
#include <Registry.hpp>
#include <stdio.h>
//---------------------------------------------------------------------------
//#ifdef _DEBUG
#define LOGFUNC()\
do {\
	if(Exceptions::debug_mode) {\
		Exceptions::writeLog(String("----------->")+__FUNC__);\
	}\
} while(0)
//#else
//#define LOGFUNC()
//#endif

#define LOGEXCEPTION()\
do {\
	writeLog(version);\
	AnsiString log("Sender: ");\
	log += (Sender != NULL) ? String(Sender->ClassName()):String("NO_SENDER");\
	log += " ## exception: " + class_name; log += " ## message: " + message;\
	writeLog(log);\
} while(0)

#define PROG_IF(condition,mymesg)\
do {\
	AnsiString line = __LINE__;\
	AnsiString file = __FILE__;\
	if(((bool) (condition)) == true) {\
		throw ProgException(file+":"+line+") "+(mymesg));\
	}\
} while(0)

class Exceptions {
private:
	static TFileStream * logfile;
	static TRegistry * registry;
	static int __fastcall openLog(void);
	static void __fastcall closeLog(void);
public:
	static bool clearlog;
	static bool debug_mode;
	static char * log_path;
	static AnsiString version;
	static void __fastcall initLog(AnsiString app_version);
	static AnsiString __fastcall copyLog(void);
	static void __fastcall writeLog(AnsiString string);
	static void __fastcall cleanup(void);
	static TRegistry * __fastcall getRegistry(void);
	static void __fastcall handleException(TObject *Sender, Exception * e);
};

class ProgException : public Exception {
	public:
		__fastcall ProgException(AnsiString mymesg) :
			Exception(mymesg) {}
};

class UserException : public Exception {
	public:
		__fastcall UserException(AnsiString mymesg) :
			Exception(mymesg) {}
		__fastcall UserException(int msgindex) :
			Exception(AnsiString::LoadStr(msgindex)) {}
};
#endif

