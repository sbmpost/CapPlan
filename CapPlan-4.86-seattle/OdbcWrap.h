// $Id: OdbcWrap.h,v 1.2 2014/01/10 16:36:21 darseq Exp $
//---------------------------------------------------------------------------
#ifndef OdbcH
#define OdbcH
//---------------------------------------------------------------------------
#include <System.hpp>
#include <SysUtils.hpp>
#include <list>
//#include <windows.h>
//#include <sql.h>
//#include <sqlext.h>
#include "Query.h"

typedef enum {
	READ_UNCOMMITTED,
	READ_COMMITTED,
	REPEATABLE_READ,
	SERIALIZABLE
} isolation_t;

class ODBCparams {
public:
	HWND hwnd; bool pooling; // hwnd: window to log to
/*
	SQLINTEGER odbc_version; // SQL_OV_ODBC2 or SQL_OV_ODBC3
	// SQL_DRIVER_PROMPT or SQL_DRIVER_COMPLETE or
	// SQL_DRIVER_COMPLETE_REQUIRED or SQL_DRIVER_NOPROMPT
	SQLUSMALLINT driver_completion;
*/
	__fastcall ODBCparams(void) {
		hwnd = NULL; pooling = false;
//		odbc_version = SQL_OV_ODBC3;
//		driver_completion = SQL_DRIVER_COMPLETE_REQUIRED;
	}
};

typedef list<Query *> queries_t;
class Odbc {
private:
	ODBCparams odbc_params;
	bool connected;
	SQLCHAR connstr[1024];
	// 1024 from ODBC API docs
	SQLCHAR dbmsname[MAX_NAME];
	SQLCHAR username[MAX_NAME];
	SQLCHAR database[MAX_NAME];

	SQLHENV env_h; SQLHDBC dbc_h;
	queries_t queries;
	SQLRETURN r;

	Odbc(const Odbc & odbc); // invalid to copy an Odbc object
public:
	__fastcall Odbc(const ODBCparams & params = ODBCparams());
	__fastcall ~Odbc(void);

	ODBCparams getParams(void) {return odbc_params;}
	void __fastcall setAutocommit(bool value);
	bool __fastcall getAutocommit(void);
	void __fastcall setIsolation(isolation_t value);
	isolation_t __fastcall getIsolation(void);
	void __fastcall commit(void); // ODBC implicitely starts a new transaction
	void __fastcall rollback(void);
	void __fastcall link(Query * query);
	void __fastcall unlink(Query * query);
	void __fastcall connect(AnsiString connect_string);
	void __fastcall connect(Odbc * odbc);
	void __fastcall disconnect(void);
	bool __fastcall isConnected(void) {return connected;}
	// todo: use DBMS information for dynamic INNODB/MS_SQL support?
	AnsiString getConnstr(void) {return String((char *) connstr);}
	AnsiString getDBMSname(void) {return String((char *) dbmsname);}
	AnsiString getUsername(void) {return String((char *) username);}
	AnsiString getDatabase(void) {return String((char *) database);}
protected:
};
#endif

