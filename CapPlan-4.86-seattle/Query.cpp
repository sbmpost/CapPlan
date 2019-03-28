// $Id: Query.cpp,v 1.47 2014/01/09 15:01:13 darseq Exp $
//---------------------------------------------------------------------------
#pragma hdrstop
#include "Query.h"
#ifdef _DEBUG
#include "Exceptions.h"
#endif
//---------------------------------------------------------------------------
#pragma package(smart_init)
AnsiString Query::last_message;
SQLRETURN Query::last_retvalue;

#ifdef _WIN64
#pragma link "Odbc32.a"
#else
#pragma link "Odbc32.lib"
#endif

#define SQLCALL(sqlcode) \
do { \
	TCursor cursor = Screen->Cursor; \
	Screen->Cursor = crSQLWait; \
	try {sqlcode} \
	__finally { \
		/* necessary when in menu */ \
		Screen->Cursor = crArrow; \
		Screen->Cursor = cursor; \
	} \
} while(0)

__fastcall Query::Query(void) {
	dbc_h = SQL_NULL_HDBC;
	stmt_h = SQL_NULL_HSTMT;
	nosign = NULL; cols = 0; col_names = NULL;
	c_datatypes = NULL; col_lengths = NULL;
	buf_offsets = NULL; data_buffer = NULL;
	array_size = 1; row_index = 0; rows = 1;
}
//---------------------------------------------------------------------------

void __fastcall Query::setdbch(SQLHDBC connection) {
	dbc_h = connection;
	if(dbc_h != SQL_NULL_HDBC) {setupSQL();}
}
//---------------------------------------------------------------------------

void __fastcall Query::setupSQL(void) {
#ifdef _DEBUG
	Exceptions::writeLog("setupSQL()");
#endif
	if(dbc_h == SQL_NULL_HDBC) {
		throw DBException("setupSQL - No connection");
	}

	// if necessary, allocate sql statement handle
	if(stmt_h == SQL_NULL_HSTMT) {
		r = SQLAllocHandle(SQL_HANDLE_STMT,dbc_h,&stmt_h);
		chkErr(r,SQL_HANDLE_DBC,dbc_h);

		// supply a pointer to store the number of rows into
		r = SQLSetStmtAttr(stmt_h,SQL_ATTR_ROWS_FETCHED_PTR,
			&rows,SQL_IS_POINTER);
		chkErr(r,SQL_HANDLE_STMT,stmt_h);
	}

	// release variable bindings
	r = SQLFreeStmt(stmt_h,SQL_UNBIND);
	chkErr(r,SQL_HANDLE_STMT,stmt_h);

	// release variables
	freeVariables();

	// release cursor and pending results
	r = SQLFreeStmt(stmt_h,SQL_CLOSE);
	chkErr(r,SQL_HANDLE_STMT,stmt_h);

	// get forward, read only, rowset size = 1 cursor
	r = SQLSetStmtAttr(stmt_h,SQL_ATTR_ROW_ARRAY_SIZE,
		(SQLPOINTER) 1,SQL_IS_UINTEGER);
	chkErr(r,SQL_HANDLE_STMT,stmt_h);
}
//---------------------------------------------------------------------------

void __fastcall Query::freeSQL(bool connected) {
	if(stmt_h != SQL_NULL_HSTMT) {
#ifdef _DEBUG
		Exceptions::writeLog("freeSQL()");
#endif
		if(connected) {
			// free statement handle (also releases variable
			// and parameter bindings and closes the cursor)
			r = SQLFreeHandle(SQL_HANDLE_STMT,stmt_h);
			chkErr(r,SQL_HANDLE_STMT,stmt_h);
		}
		stmt_h = SQL_NULL_HSTMT;
		freeVariables();
	}
}
//---------------------------------------------------------------------------

void __fastcall Query::execSQL(AnsiString query,SQLULEN rowset_size) {
#ifdef MS_SQL_LOCK_TIMEOUT
	// sqlncli quirk: it constantly resets lock_timeout
	query = "set lock_timeout "MS_SQL_LOCK_TIMEOUT";"+query;
#endif
#ifdef _DEBUG
	Exceptions::writeLog("execSQL(): "+query);
#endif
	setupSQL(); // resets SQL_ATTR_ROW_ARRAY_SIZE to 1 (ms_sql speedup)
	SQLCALL(r = SQLExecDirect(stmt_h,(SQLCHAR *) query.c_str(),SQL_NTS););
	chkErr(r,SQL_HANDLE_STMT,stmt_h);
	if(r == SQL_NEED_DATA) {
		throw DBException("execSQL - To set parameter data "
			"at execution time, prepSQL() should be used");
	}
	bindVariables(rowset_size);
}
//---------------------------------------------------------------------------

void __fastcall Query::prepSQL(AnsiString query) {
#ifdef MS_SQL_LOCK_TIMEOUT
	// sqlncli quirk: it constantly resets lock_timeout
	query = "set lock_timeout "MS_SQL_LOCK_TIMEOUT";"+query;
#endif
#ifdef _DEBUG
	Exceptions::writeLog("prepSQL(): "+query);
#endif
	setupSQL(); // resets SQL_ATTR_ROW_ARRAY_SIZE to 1
	SQLCALL(r = SQLPrepare(stmt_h,(SQLCHAR *) query.c_str(),SQL_NTS););
	chkErr(r,SQL_HANDLE_STMT,stmt_h);
}
//---------------------------------------------------------------------------

bool __fastcall Query::execSQL(SQLULEN rowset_size) {
#ifdef _DEBUG
	Exceptions::writeLog("execSQL()");
#endif
	if(stmt_h == SQL_NULL_HSTMT) {
		throw DBException("execSQL - No query to process");
	}
	// If execSQL ran before, reset row_index and rows
	if(cols != 0) {row_index = array_size-1; rows = 1;}
	else {bindVariables(rowset_size);} // after binding parameters
	// before SQLExecute to support parameter data at execution.
	SQLCALL(r = SQLExecute(stmt_h););
	chkErr(r,SQL_HANDLE_STMT,stmt_h);
	return r == SQL_NEED_DATA;
}
//---------------------------------------------------------------------------

void __fastcall Query::freeVariables(void) {
	delete[] nosign; nosign = NULL;
	delete[] c_datatypes; c_datatypes = NULL;
	delete[] col_lengths; col_lengths = NULL;
	delete[] buf_offsets; buf_offsets = NULL;
	delete[] data_buffer; data_buffer = NULL;
	for(int i=0;i < cols;i++) {delete[] col_names[i];}
	delete[] col_names; col_names = NULL; cols = 0;
	array_size = 1; row_index = 0; rows = 1;
}
//---------------------------------------------------------------------------

void __fastcall Query::bindVariables(SQLULEN rowset_size) {
#ifdef _DEBUG
	Exceptions::writeLog("bindVariables()");
#endif
	SQLCALL(r = SQLNumResultCols(stmt_h,&cols););
	chkErr(r,SQL_HANDLE_STMT,stmt_h);
	if(cols == 0) {return;}
	// return if no result set.
	// insert/update statement.
	c_datatypes = new SQLSMALLINT[cols];
	col_lengths = new SQLLEN[cols];
	buf_offsets = new SQLLEN[cols];
	col_names = new SQLCHAR*[cols];
	nosign = new SQLLEN[cols];

#ifndef ACCESS // otherwise keep SQL_ATTR_ROW_ARRAY_SIZE set by setupSQL()
	// When multiple rows are fetched, older access drivers store TINYINTS
	// like: 11 XX,22 XX,33 XX. Avoid this bug by fetching only one row at
	// a time (fieldByColumn then retrieves 11,22,33 instead of 11,XX,22).
	r = SQLSetStmtAttr(stmt_h,SQL_ATTR_ROW_ARRAY_SIZE,
		(SQLPOINTER) rowset_size,SQL_IS_UINTEGER);
	chkErr(r,SQL_HANDLE_STMT,stmt_h);
#endif
	SQLINTEGER length;
	r = SQLGetStmtAttr(stmt_h,SQL_ATTR_ROW_ARRAY_SIZE,
		&array_size,SQL_IS_UINTEGER,&length);
	chkErr(r,SQL_HANDLE_STMT,stmt_h);
	row_index = array_size-1;

	buf_offsets[0] = 0;
	for(SQLSMALLINT i=0;i < cols;i++) {
		SQLULEN chars; // the number of character positions
		SQLSMALLINT name_length,s_type,decimal_digits,nullable;

		// SQLGetInfo(dbc_h,SQL_MAX_COLUMN_NAME_LEN...)
		col_names[i] = new SQLCHAR[MAX_NAME];
		// use SQLDescribeCol for SQL_C_CHAR - SQL_C_BINARY length. ODBC2 does
		// not support SQLColAttribute SQL_DESC_LENGTH/SQL_DESC_OCTET_LENGTH.
		r = SQLDescribeCol(stmt_h,i+1,col_names[i],MAX_NAME,&name_length,
			&s_type,&chars,&decimal_digits,&nullable);
		chkErr(r,SQL_HANDLE_STMT,stmt_h);
		if(name_length >= MAX_NAME) {
			throw DBException("bindVariables - Column name too long");
		}
		c_datatypes[i] = getCtype(s_type);
		col_lengths[i] = getCsize(s_type,chars);
		r = SQLColAttribute(stmt_h,((SQLUSMALLINT) i)+1,
			SQL_DESC_UNSIGNED,NULL,0,NULL,&nosign[i]);
		chkErr(r,SQL_HANDLE_STMT,stmt_h);

		// get start offset within data_buffer for the current column
		if(i) {
			buf_offsets[i] = buf_offsets[i-1] +
				array_size * col_lengths[i-1] + !!col_lengths[i-1] *
				array_size * sizeof(SQLLEN); // length/indicator buffer
		}
	}

	// Allocate memory for the rows and columns
	data_buffer = new SQLCHAR[buf_offsets[cols-1] +
		array_size * col_lengths[cols-1] + !!col_lengths[cols-1] *
		array_size * sizeof(SQLLEN)]; // length/indicator buffer

	// Bind the allocated memory locations to the columns so SQLFetch knows
	// where to store the data (the actual column data and the length/indicator)
	for(SQLSMALLINT i=0;i < cols;i++) {
		if(col_lengths[i] == 0) {continue;} // do not bind SQL_BINARY columns
		r = SQLBindCol(stmt_h,((SQLUSMALLINT) i)+1,c_datatypes[i],
			(SQLPOINTER) (data_buffer + buf_offsets[i]), // data start offset
			col_lengths[i], // number of bytes reserved for one column field
			(SQLLEN *) (data_buffer + buf_offsets[i] +
			array_size * col_lengths[i])); // length/indicator buffer start
		chkErr(r,SQL_HANDLE_STMT,stmt_h);
	}
}
//---------------------------------------------------------------------------

bool __fastcall Query::next(void) {
	bool data_available = true;
	if(stmt_h == SQL_NULL_HSTMT) {
		throw DBException("next - No query to process");
	} else if((row_index = (row_index+1) % array_size) == 0) {
		SQLCALL(r = SQLFetch(stmt_h);); // odbc 3.x: sets global rows
		if(r == SQL_NO_DATA) {data_available = false;}
		else {chkErr(r,SQL_HANDLE_STMT,stmt_h);}
	}
	if(data_available) {
		if(row_index >= rows) {
			SQLCALL(r = SQLFetch(stmt_h););
			// always complete the call to SQLFetch
			// to ensure there is no data_available
			if(r == SQL_NO_DATA) {data_available = false;}
			else {chkErr(r,SQL_HANDLE_STMT,stmt_h);}
		}
	}
	if(data_available == false) {
		// keep variables and parameters
		r = SQLFreeStmt(stmt_h,SQL_CLOSE);
		chkErr(r,SQL_HANDLE_STMT,stmt_h);
	}
	return data_available;
}
//---------------------------------------------------------------------------

Field __fastcall Query::fieldByColumn(unsigned int column) {
	if(stmt_h == SQL_NULL_HSTMT || cols == 0) {
		throw DBException("fieldByColumn - No query to process");
	}
	if(column > (unsigned int) cols) {column = 0;}
	Field field(column);
	if(field.col != 0) {
		SQLUSMALLINT field_index = field.col-1;
		field.c_type = c_datatypes[field_index];
		field.nosign = nosign[field_index];
		if((field.col_isbound = col_lengths[field_index]) == true) {
			// data_buffer[buf_offsets[i]+array_size*col_lengths[i])+row_index];
			// data_buffer is an unsigned char pointer. When assigning the char
			// to field.length, it is sign extended which is wrong.
			field.length = *((SQLLEN *) (data_buffer + buf_offsets[field_index] +
				array_size * col_lengths[field_index]) + row_index);
			field.data = data_buffer + buf_offsets[field_index] +
				row_index * col_lengths[field_index];
		}
	}
	return field;
}
//---------------------------------------------------------------------------

Field __fastcall Query::fieldByName(AnsiString field_name) {
	if(stmt_h == SQL_NULL_HSTMT || cols == 0) {
		throw DBException("fieldByName - No query to process");
	}
	return fieldByColumn(findColumn(field_name));
}
//---------------------------------------------------------------------------

void __fastcall Query::bindParameter(Field * field) {
	if(stmt_h == SQL_NULL_HSTMT) {
		throw DBException("bindParameter - No query to process");
	}
	// number of character positions
	SQLLEN chars = field->length;
	if(chars == SQL_NULL_DATA ||
		chars == SQL_DATA_AT_EXEC ||
		chars == SQL_NTS) {chars = 1;}
	else if(chars < 0) {chars = SQL_LEN_DATA_AT_EXEC(chars);}
	if(chars == 0) {chars = 1;} // field->length sets the number of bytes
	r = SQLBindParameter(stmt_h,field->col,SQL_PARAM_INPUT,field->c_type,
		getStype(field->c_type),chars,0,field->data,0,&field->length);
	chkErr(r,SQL_HANDLE_STMT,stmt_h);
}
//---------------------------------------------------------------------------

void * __fastcall Query::nextParameter(void) {
	if(stmt_h == SQL_NULL_HSTMT) {
		throw DBException("nextParameter - No query to process");
	}
	SQLPOINTER token;
	r = SQLParamData(stmt_h,&token);
	if(r == SQL_NEED_DATA) {return token;}
	chkErr(r,SQL_HANDLE_STMT,stmt_h);
	return NULL;
}
//---------------------------------------------------------------------------

void __fastcall Query::write(void * data,SQLLEN length) {
	if(stmt_h == SQL_NULL_HSTMT) {
		throw DBException("write - No query to process");
	}
	SQLCALL(r = SQLPutData(stmt_h,data,length););
	chkErr(r,SQL_HANDLE_STMT,stmt_h);
}
//---------------------------------------------------------------------------

bool __fastcall Query::read(unsigned int column,void * data,
	SQLLEN length,SQLLEN * data_read) {
	if(stmt_h == SQL_NULL_HSTMT || cols == 0) {
		throw DBException("read - No query to process");
	}
	if(column == 0 || column > (unsigned int) cols) {
		throw DBException("read - Invalid column number");
	}
	SQLLEN temp_read;
	SQLCALL(r = SQLGetData(stmt_h,column,c_datatypes[column-1],data,
		length,&temp_read);); // first call never returns SQL_NO_DATA
	if(r == SQL_NO_DATA) {return false;}
	chkErr(r,SQL_HANDLE_STMT,stmt_h);
	// some odbc drivers can only set the value of data_read
	// on the last read. some others set SQL_NULL_DATA even
	// when there is no data (SQL_NO_DATA)
	if(temp_read == SQL_NULL_DATA) {*data_read = 0;}
	else {*data_read = temp_read;}
	return true;
}
//---------------------------------------------------------------------------

void __fastcall Query::fieldNames(AnsiString table_name) {
#ifdef _DEBUG
	Exceptions::writeLog("fieldNames()");
#endif
	setupSQL();
	SQLCALL(r = SQLColumns(stmt_h,NULL,0,
		NULL,0,(SQLCHAR *) table_name.c_str(),
		table_name.Length(),NULL,0););
	chkErr(r,SQL_HANDLE_STMT,stmt_h);
	bindVariables(1);
}
//---------------------------------------------------------------------------

void __fastcall Query::tableNames(void) {
#ifdef _DEBUG
	Exceptions::writeLog("tableNames()");
#endif
	setupSQL();
	SQLCALL(r = SQLTables(stmt_h,NULL,0,
		NULL,0,NULL,0,NULL,0););
	chkErr(r,SQL_HANDLE_STMT,stmt_h);
	bindVariables(1);
}
//---------------------------------------------------------------------------

AnsiString __fastcall Query::chkErr(SQLRETURN ret_val,SQLSMALLINT handle_type,
	SQLHANDLE handle) {
	last_message.SetLength(0);
	last_retvalue = ret_val;
	switch(ret_val) {
		// disabled for performance
		// case SQL_SUCCESS_WITH_INFO:
		case SQL_ERROR: {
			// note that last_message is static so we add to it.
			last_message += ("\n"); SQLSMALLINT message_length = 0;
			SQLRETURN ret; vector<DBError> dberrors; DBError error;
			for(SQLSMALLINT i=1;(ret = SQLGetDiagRec(handle_type,handle,i,
				error.sql_state,&error.native_err,error.message,
				sizeof(error.message),&message_length)) != SQL_NO_DATA;i++) {
				if(ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
					last_message += "ODBC Diagnostics retrieval failed\n";
					break;
				}

				last_message += String("[") + i + "]:: SQL state:" +
					String((char *) error.sql_state) +
					", Native error code:" + error.native_err +
					", Message:" + String((char *) error.message) + "\n";
				dberrors.push_back(error);
			}
			// if(ret_val == SQL_ERROR) {
				if(dberrors.empty()) {last_message += "Unknown ODBC error\n";}
				throw DBException(last_message,dberrors);
			// } else {break;}
		}
		case SQL_INVALID_HANDLE: throw DBException("Invalid handle");
		case SQL_STILL_EXECUTING: throw DBException("Function still executing");
		case SQL_SUCCESS_WITH_INFO: case SQL_SUCCESS:
		case SQL_NO_DATA: case SQL_NEED_DATA: break;
		default: throw DBException("Unexpected return value");
	}
	return last_message;
}
//---------------------------------------------------------------------------

SQLUSMALLINT __fastcall Query::findColumn(AnsiString field_name) {
	SQLUSMALLINT column = 0; // set column not present
	for(SQLUSMALLINT i=0;i < cols;i++) {
		AnsiString col_name((char *) col_names[i]);
		if(col_name.LowerCase() == field_name.LowerCase()) {
			column = i+1; break;
		}
	}
#ifdef _DEBUG
	if(column == 0) {
		Exceptions::writeLog("findColumn(), column not found: "+field_name);
	}
#endif
	return column;
}
//---------------------------------------------------------------------------

SQLULEN __fastcall Query::getCsize(SQLSMALLINT s_type,SQLULEN chars) {
	// Convert the character count to the number of bytes to be allocated.
	// Note that this only makes sense for character types. For fixed-length
	// data, such as an integer or a timestamp structure, SQLBindCol ignores
	// BufferLength and assumes the buffer is large enough to hold the data.
	// It is still important however that the number of bytes returned exactly
	// matches the size of the data stored by SQLFetch. Consider for example a
	// SMALLINT column with three rows being fetched simultaniously. In this
	// situation memory is laid out like: 11 11,22 22,33 33. If the number of
	// bytes would be 1 instead of 2, values 11 11,11 22,22 22 are retrieved.
	switch(s_type) {
		case SQL_LONGVARCHAR: case SQL_WLONGVARCHAR:
			chars = LONG_BUFFER;
		break;
		case SQL_GUID:
		case SQL_CHAR: case SQL_VARCHAR:
			chars++; // include terminating '\0'
		break;
		case SQL_WCHAR: case SQL_WVARCHAR:
			chars *= 2; // wide chars are 2 bytes
			chars++; // include terminating '\0'
		break;
		case SQL_BINARY: case SQL_VARBINARY: case SQL_LONGVARBINARY:
			chars = 0; // we do not bind binary types
		break;
		case SQL_DECIMAL: case SQL_NUMERIC:
			chars += 2; // sign symbol and decimal symbol
			// depends on ANSI or UNICODE character set?
		break;

		case SQL_BIT: chars = sizeof(unsigned char); break;
		case SQL_TINYINT: chars = sizeof(char); break;
		case SQL_SMALLINT: chars = sizeof(short); break;
		case SQL_INTEGER: chars = sizeof(int); break;
		case SQL_BIGINT: chars = sizeof(__int64); break;
		case SQL_REAL: chars = sizeof(float); break;
		case SQL_FLOAT: case SQL_DOUBLE: chars = sizeof(double); break;

		// odbc 2.x
		case SQL_DATE: chars = sizeof(DATE_STRUCT); break;
		case SQL_TIME: chars = sizeof(TIME_STRUCT); break;
		case SQL_TIMESTAMP: chars = sizeof(TIMESTAMP_STRUCT); break;

		// odbc 3.x
		case SQL_TYPE_DATE: chars = sizeof(DATE_STRUCT); break;
		// MyOdbc 3.51.09 contains a bug that prevents times to be returned?!
		case SQL_TYPE_TIME: chars = sizeof(TIME_STRUCT); break;
		case SQL_TYPE_TIMESTAMP: chars = sizeof(TIMESTAMP_STRUCT); break;

		case SQL_INTERVAL_YEAR: case SQL_INTERVAL_MONTH:
		case SQL_INTERVAL_YEAR_TO_MONTH: case SQL_INTERVAL_DAY:
		case SQL_INTERVAL_HOUR: case SQL_INTERVAL_MINUTE:
		case SQL_INTERVAL_SECOND: case SQL_INTERVAL_DAY_TO_HOUR:
		case SQL_INTERVAL_DAY_TO_MINUTE: case SQL_INTERVAL_DAY_TO_SECOND:
		case SQL_INTERVAL_HOUR_TO_MINUTE: case SQL_INTERVAL_HOUR_TO_SECOND:
		case SQL_INTERVAL_MINUTE_TO_SECOND: chars = sizeof(SQL_INTERVAL_STRUCT);
		break;
		default: chars = 0; break; // SQL_UNKNOWN_TYPE or driver specific type
	}
	return chars;
}
//---------------------------------------------------------------------------

SQLSMALLINT __fastcall Query::getCtype(SQLSMALLINT s_type) {
	SQLSMALLINT c_type;
	switch(s_type) {
		case SQL_GUID:
		case SQL_CHAR: case SQL_VARCHAR: case SQL_LONGVARCHAR:
			c_type = SQL_C_CHAR;
		break;
		case SQL_WCHAR: case SQL_WVARCHAR: case SQL_WLONGVARCHAR:
			c_type = SQL_C_WCHAR;
		break;
		case SQL_BINARY: case SQL_VARBINARY: case SQL_LONGVARBINARY:
			c_type = SQL_C_BINARY;
		break;
		case SQL_DECIMAL: case SQL_NUMERIC:
			c_type = SQL_C_CHAR;
		break;

		case SQL_BIT: c_type = SQL_C_BIT; break;
		case SQL_TINYINT: c_type = SQL_C_TINYINT; break;
		case SQL_SMALLINT: c_type = SQL_C_SHORT; break;
		case SQL_INTEGER: c_type = SQL_C_LONG; break;
		case SQL_BIGINT: c_type = SQL_C_SBIGINT; break;
		case SQL_REAL: c_type = SQL_C_FLOAT; break;
		case SQL_FLOAT: case SQL_DOUBLE: c_type = SQL_C_DOUBLE; break;

		// odbc 2.x
		case SQL_DATE: c_type = SQL_C_DATE; break;
		case SQL_TIME: c_type = SQL_C_TIME; break;
		case SQL_TIMESTAMP: c_type = SQL_C_TIMESTAMP; break;

		// odbc 3.x
		case SQL_TYPE_DATE: c_type = SQL_C_TYPE_DATE; break;
		case SQL_TYPE_TIME: c_type = SQL_C_TYPE_TIME; break;
		case SQL_TYPE_TIMESTAMP: c_type = SQL_C_TYPE_TIMESTAMP; break;

		case SQL_INTERVAL_YEAR: c_type = SQL_C_INTERVAL_YEAR; break;
		case SQL_INTERVAL_MONTH: c_type = SQL_C_INTERVAL_MONTH; break;
		case SQL_INTERVAL_YEAR_TO_MONTH:
			c_type = SQL_C_INTERVAL_YEAR_TO_MONTH;
		break;
		case SQL_INTERVAL_DAY: c_type = SQL_C_INTERVAL_DAY; break;
		case SQL_INTERVAL_HOUR: c_type = SQL_C_INTERVAL_HOUR; break;
		case SQL_INTERVAL_MINUTE: c_type = SQL_C_INTERVAL_MINUTE; break;
		case SQL_INTERVAL_SECOND: c_type = SQL_C_INTERVAL_SECOND; break;
		case SQL_INTERVAL_DAY_TO_HOUR:
			c_type = SQL_C_INTERVAL_DAY_TO_HOUR;
		break;
		case SQL_INTERVAL_DAY_TO_MINUTE:
			c_type = SQL_C_INTERVAL_DAY_TO_MINUTE;
		break;
		case SQL_INTERVAL_DAY_TO_SECOND:
			c_type = SQL_C_INTERVAL_DAY_TO_SECOND;
		break;
		case SQL_INTERVAL_HOUR_TO_MINUTE:
			c_type = SQL_C_INTERVAL_HOUR_TO_MINUTE;
		break;
		case SQL_INTERVAL_HOUR_TO_SECOND:
			c_type = SQL_C_INTERVAL_HOUR_TO_SECOND;
		break;
		case SQL_INTERVAL_MINUTE_TO_SECOND:
			c_type = SQL_C_INTERVAL_MINUTE_TO_SECOND;
		break;
		default: c_type = SQL_C_BINARY; break;
		// SQL_UNKNOWN_TYPE or driver specific type
	}
	return c_type;
}
//---------------------------------------------------------------------------

SQLSMALLINT __fastcall Query::getStype(SQLSMALLINT c_type) {
	SQLSMALLINT s_type;
	switch(c_type) {
		case SQL_C_CHAR: s_type = SQL_LONGVARCHAR; break;
		case SQL_C_WCHAR: s_type = SQL_WLONGVARCHAR; break;
		case SQL_C_BIT: s_type = SQL_BIT; break;
		case SQL_C_TINYINT: case SQL_C_STINYINT: s_type = SQL_TINYINT; break;
		case SQL_C_SHORT: case SQL_C_SSHORT: s_type = SQL_SMALLINT; break;
		case SQL_C_LONG: case SQL_C_SLONG: s_type = SQL_INTEGER; break;
		case SQL_C_SBIGINT: s_type = SQL_BIGINT; break;
		case SQL_C_FLOAT: s_type = SQL_REAL; break;
		case SQL_C_DOUBLE: s_type = SQL_DOUBLE; break;
		case SQL_C_BINARY: s_type = SQL_LONGVARBINARY; break;
		case SQL_C_DATE: case SQL_C_TYPE_DATE: s_type = SQL_TYPE_DATE; break;
		case SQL_C_TIME: case SQL_C_TYPE_TIME: s_type = SQL_TYPE_TIME; break;
		case SQL_C_TIMESTAMP: case SQL_C_TYPE_TIMESTAMP:
			s_type = SQL_TYPE_TIMESTAMP;
		break;
		case SQL_C_INTERVAL_YEAR: s_type = SQL_INTERVAL_YEAR; break;
		case SQL_C_INTERVAL_MONTH: s_type = SQL_INTERVAL_MONTH; break;
		case SQL_C_INTERVAL_YEAR_TO_MONTH:
			s_type = SQL_INTERVAL_YEAR_TO_MONTH;
		break;
		case SQL_C_INTERVAL_DAY: s_type = SQL_INTERVAL_DAY; break;
		case SQL_C_INTERVAL_HOUR: s_type = SQL_INTERVAL_HOUR; break;
		case SQL_C_INTERVAL_MINUTE: s_type = SQL_INTERVAL_MINUTE; break;
		case SQL_C_INTERVAL_SECOND: s_type = SQL_INTERVAL_SECOND; break;
		case SQL_C_INTERVAL_DAY_TO_HOUR:
			s_type = SQL_INTERVAL_DAY_TO_HOUR;
		break;
		case SQL_C_INTERVAL_DAY_TO_MINUTE:
			s_type = SQL_INTERVAL_DAY_TO_MINUTE;
		break;
		case SQL_C_INTERVAL_DAY_TO_SECOND:
			s_type = SQL_INTERVAL_DAY_TO_SECOND;
		break;
		case SQL_C_INTERVAL_HOUR_TO_MINUTE:
			s_type = SQL_INTERVAL_HOUR_TO_MINUTE;
		break;
		case SQL_C_INTERVAL_HOUR_TO_SECOND:
			s_type = SQL_INTERVAL_HOUR_TO_SECOND;
		break;
		case SQL_C_INTERVAL_MINUTE_TO_SECOND:
			s_type = SQL_INTERVAL_MINUTE_TO_SECOND;
		break;
		default: s_type = SQL_LONGVARBINARY; break;
		// SQL_UNKNOWN_TYPE or driver specific type
	}
	return s_type;
}
//---------------------------------------------------------------------------

