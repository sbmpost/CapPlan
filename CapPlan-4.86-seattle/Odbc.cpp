// $Id: Odbc.cpp,v 1.34 2013/02/16 12:18:36 darseq Exp $
//---------------------------------------------------------------------------
#pragma hdrstop
#include "Odbc.h"
#include <algorithm>
#if defined _DEBUG || !defined NO_EXHANDLER
#include "Exceptions.h"
#endif
//---------------------------------------------------------------------------
#pragma package(smart_init)

__fastcall Odbc::Odbc(const ODBCparams & params) {
	odbc_params = params; connected = false;
	connstr[0] = dbmsname[0] = username[0] = database[0] = '\0';
	env_h = SQL_NULL_HENV; dbc_h = SQL_NULL_HDBC;

	if(odbc_params.pooling) {
		r = SQLSetEnvAttr(SQL_NULL_HANDLE,SQL_ATTR_CONNECTION_POOLING,
			(SQLPOINTER) SQL_CP_ONE_PER_HENV,SQL_IS_UINTEGER);
		Query::chkErr(r,SQL_HANDLE_ENV,SQL_NULL_HANDLE);
	}

	// Allocate environment handle
	r = SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&env_h);
	Query::chkErr(r,SQL_HANDLE_ENV,SQL_NULL_HANDLE);
	try {
		r = SQLSetEnvAttr(env_h,SQL_ATTR_ODBC_VERSION,
			(SQLPOINTER) odbc_params.odbc_version,SQL_IS_UINTEGER);
		Query::chkErr(r,SQL_HANDLE_ENV,env_h);
	} catch(...) {
		r = SQLFreeHandle(SQL_HANDLE_ENV,env_h);
		env_h = SQL_NULL_HENV;
		throw;
	}
}
//---------------------------------------------------------------------------

__fastcall Odbc::~Odbc(void) {
	// destructors should never throw exceptions
	try {if(connected) {rollback();}} // a transaction may be in progress.
#if defined _DEBUG || !defined NO_EXHANDLER
	catch(DBException & e) {Exceptions::handleException(NULL,&e);}
#else
	catch(DBException & e) {}
#endif
	try { // try: network may be down
		disconnect();
		// Free environment handle (can only succeed if disconnect succeeds)
		r = SQLFreeHandle(SQL_HANDLE_ENV,env_h);
		Query::chkErr(r,SQL_HANDLE_ENV,env_h);
		env_h = SQL_NULL_HENV;
#if defined _DEBUG || !defined NO_EXHANDLER
	} catch(DBException & e) {Exceptions::handleException(NULL,&e);}
#else
	} catch(DBException & e) {}
#endif
	// do not unlink (cannot be called twice without
	// error, and queries is destroyed anyway)
}
//---------------------------------------------------------------------------

void __fastcall Odbc::connect(AnsiString connect_string) {
	disconnect();
	// allocate connection handle
	if(dbc_h == SQL_NULL_HDBC) {
		r = SQLAllocHandle(SQL_HANDLE_DBC,env_h,&dbc_h);
		Query::chkErr(r,SQL_HANDLE_ENV,env_h);
	}

	if(connected == false) {
#ifdef _DEBUG
		Exceptions::writeLog("CONNECT");
#endif
		// connect database
		// sqlncli seems to mess things up on the second connection whenever the
		// first connection attempt fails. This happens for instance when trying
		// to connect checking the trusted connection box but the account needs
		// a password. A second connection attempt using a valid password will
		// open succesfully but when multiple active result sets are generated,
		// the message: connection is busy waiting for results from another
		// command appears.
		SQLSMALLINT connstr_length;
		r = SQLDriverConnect(dbc_h,odbc_params.hwnd,
			(SQLCHAR *) connect_string.c_str(),
			(SQLSMALLINT) connect_string.Length(),connstr,sizeof(connstr),
			&connstr_length,odbc_params.driver_completion);
		Query::chkErr(r,SQL_HANDLE_DBC,dbc_h);
		if(r == SQL_NO_DATA) {return;} // user should call isConnected()
		connected = true; connstr[connstr_length] = '\0';

		// SQLUSMALLINT user_length; SQLSMALLINT length;
		// r = SQLGetInfo(dbc_h,SQL_MAX_USER_NAME_LEN,&user_length,
		//		sizeof(user_length),&length);
		// user_length = (user_length) ? user_length : 256;
		// char uname[user_length];

		SQLSMALLINT len; // should we check len for truncated data?
		r = SQLGetInfo(dbc_h,SQL_DBMS_NAME,&dbmsname,sizeof(dbmsname),&len);
		Query::chkErr(r,SQL_HANDLE_DBC,dbc_h);
		r = SQLGetInfo(dbc_h,SQL_USER_NAME,&username,sizeof(username),&len);
		Query::chkErr(r,SQL_HANDLE_DBC,dbc_h);
		r = SQLGetInfo(dbc_h,SQL_DATABASE_NAME,&database,sizeof(database),&len);
		Query::chkErr(r,SQL_HANDLE_DBC,dbc_h);

		queries_t::const_iterator it = queries.begin();
		for(;it != queries.end();it++) {(*it)->setdbch(dbc_h);}
#ifdef _DEBUG
		// MessageBox(odbc_params.hwnd,(char *) connstr,"CONNECT OK",MB_OK);
		Exceptions::writeLog("CONNECT OK");
		Exceptions::writeLog(getConnstr());
#endif
	}
}
//---------------------------------------------------------------------------

void __fastcall Odbc::connect(Odbc * odbc) {
	connect(odbc->getConnstr());
	if(connected) {
		setIsolation(odbc->getIsolation());
		setAutocommit(odbc->getAutocommit());
	}
}
//---------------------------------------------------------------------------

void __fastcall Odbc::disconnect() {
	if(connected) {
#ifdef _DEBUG
		Exceptions::writeLog("DISCONNECT");
#endif
		// stmt handles need to be freed here because they were allocated
		// within the context of dbc handle. freeSQL also frees variables.
		queries_t::const_iterator it = queries.begin();
		// for(;it != queries.end();it++) {(*it)->freeSQL(connected);}
		// sqlncli fix, open a document, and let the connection drop.

		// disconnect database (fails if transaction is executing)
		r = SQLDisconnect(dbc_h);
		Query::chkErr(r,SQL_HANDLE_DBC,dbc_h);
		connected = false;

		it = queries.begin();
		for(;it != queries.end();it++) {(*it)->setdbch(SQL_NULL_HDBC);}
#ifdef _DEBUG
		// MessageBox(odbc_params.hwnd,"DISCONNECT OK","NOTICE",MB_OK);
		Exceptions::writeLog("DISCONNECT OK");
#endif
	}

	if(dbc_h != SQL_NULL_HDBC) {
		// free connection handle
		r = SQLFreeHandle(SQL_HANDLE_DBC,dbc_h);
		Query::chkErr(r,SQL_HANDLE_DBC,dbc_h);
		dbc_h = SQL_NULL_HDBC;
	}
}
//---------------------------------------------------------------------------

void __fastcall Odbc::link(Query * query) {
	queries_t::const_iterator it = find(queries.begin(),queries.end(),query);
	if(it != queries.end()) {
		throw DBException("link - Query object was already linked");
	}
	query->logwindow = odbc_params.hwnd; // window handle to log to
	query->setdbch(dbc_h); // could be NULL if not yet connected
	queries.push_back(query);
}
//---------------------------------------------------------------------------

void __fastcall Odbc::unlink(Query * query) {
	queries_t::iterator it = find(queries.begin(),queries.end(),query);
	if(it == queries.end()) {
		throw DBException("unlink - Query object was never linked");
	}
	queries.erase(it); // erase even when freeSQL fails
	query->setdbch(SQL_NULL_HDBC); // iterator it is now invalidated
	// free stmt handle as well, since it was allocated by means of
	// the dbc handle. if reconnect & relink, query->dbc_h changes
	query->freeSQL(connected);
}
//---------------------------------------------------------------------------

void __fastcall Odbc::setAutocommit(bool value) {
	if(dbc_h == SQL_NULL_HDBC) {
		throw DBException("setAutocommit - no connection");
	}
	SQLUINTEGER sqlvalue = value ? SQL_AUTOCOMMIT_ON : SQL_AUTOCOMMIT_OFF;
	r = SQLSetConnectAttr(dbc_h,SQL_ATTR_AUTOCOMMIT,
		(SQLPOINTER) sqlvalue,SQL_IS_UINTEGER);
	Query::chkErr(r,SQL_HANDLE_DBC,dbc_h);
}
//---------------------------------------------------------------------------

bool __fastcall Odbc::getAutocommit(void) {
	if(dbc_h == SQL_NULL_HDBC) {
		throw DBException("getAutocommit - no connection");
	}
	SQLUINTEGER sqlvalue;
	r = SQLGetConnectAttr(dbc_h,SQL_ATTR_AUTOCOMMIT,
		(SQLPOINTER) &sqlvalue,SQL_IS_UINTEGER,NULL);
	Query::chkErr(r,SQL_HANDLE_DBC,dbc_h);
	return sqlvalue == SQL_AUTOCOMMIT_ON;
}
//---------------------------------------------------------------------------

isolation_t __fastcall Odbc::getIsolation(void) {
	if(dbc_h == SQL_NULL_HDBC) {
		throw DBException("getIsolation - no connection");
	}
	SQLUINTEGER sqlvalue; isolation_t value;
	r = SQLGetConnectAttr(dbc_h,SQL_ATTR_TXN_ISOLATION,
		(SQLPOINTER) &sqlvalue,SQL_IS_INTEGER,NULL);
	Query::chkErr(r,SQL_HANDLE_DBC,dbc_h);
	switch(sqlvalue) {
		case SQL_TXN_READ_UNCOMMITTED: value = READ_UNCOMMITTED; break;
		case SQL_TXN_READ_COMMITTED: value = READ_COMMITTED; break;
		case SQL_TXN_REPEATABLE_READ: value = REPEATABLE_READ; break;
		case SQL_TXN_SERIALIZABLE: value = SERIALIZABLE; break;
	}
	return value;
}
//---------------------------------------------------------------------------

void __fastcall Odbc::setIsolation(isolation_t value) {
	if(dbc_h == SQL_NULL_HDBC) {
		throw DBException("setIsolation - no connection");
	}
	SQLINTEGER sqlvalue;
	switch(value) {
		case READ_UNCOMMITTED: sqlvalue = SQL_TXN_READ_UNCOMMITTED; break;
		case READ_COMMITTED: sqlvalue = SQL_TXN_READ_COMMITTED; break;
		case REPEATABLE_READ: sqlvalue = SQL_TXN_REPEATABLE_READ; break;
		case SERIALIZABLE: sqlvalue = SQL_TXN_SERIALIZABLE; break;
	}
	r = SQLSetConnectAttr(dbc_h,SQL_ATTR_TXN_ISOLATION,
		(SQLPOINTER) sqlvalue,SQL_IS_INTEGER);
	Query::chkErr(r,SQL_HANDLE_DBC,dbc_h);
}
//---------------------------------------------------------------------------

void __fastcall Odbc::commit(void) {
#ifdef _DEBUG
	Exceptions::writeLog("COMMIT");
#endif
	if(dbc_h == SQL_NULL_HDBC) {throw DBException("commit - no connection");}
	r = SQLEndTran(SQL_HANDLE_DBC,dbc_h,SQL_COMMIT);
	Query::chkErr(r,SQL_HANDLE_DBC,dbc_h);
}
//---------------------------------------------------------------------------

void __fastcall Odbc::rollback(void) {
#ifdef _DEBUG
	Exceptions::writeLog("ROLLBACK");
#endif
	if(dbc_h == SQL_NULL_HDBC) {throw DBException("rollback - no connection");}
	r = SQLEndTran(SQL_HANDLE_DBC,dbc_h,SQL_ROLLBACK);
	Query::chkErr(r,SQL_HANDLE_DBC,dbc_h);
}
//---------------------------------------------------------------------------

