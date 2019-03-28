// $Id: Query.h,v 1.44 2013/12/21 20:00:16 darseq Exp $
//---------------------------------------------------------------------------
#ifndef QueryH
#define QueryH
//---------------------------------------------------------------------------
#include <StdCtrls.hpp>
#include <sql.h>
#include <sqlext.h>
#include <sqlucode.h>
#include <vector>
using namespace std;

#define UNDEFINED ""
#define MAX_NAME 256 // for columns and SQLGetInfo
#define LONG_BUFFER 4096 // for SQL_LONGVARCHAR types
#define YMD(date) (FormatDateTime("yyyy'-'mm'-'dd",date))
#define HMS(time) (FormatDateTime("hh':'mm':'ss",time))
#define YMDHMS(date) (FormatDateTime("yyyy'-'mm'-'dd' 'hh':'mm':'ss",date))

typedef struct database_error {
	SQLCHAR sql_state[SQL_SQLSTATE_SIZE+1];
	SQLCHAR message[SQL_MAX_MESSAGE_LENGTH];
	SQLINTEGER native_err;
} DBError;

class DBException : public Exception {
	public:
		vector<DBError> errors;
		__fastcall DBException(AnsiString message) : Exception(message) {}
		__fastcall DBException(AnsiString message,
			vector<DBError> & dberrors) : Exception(message) {
			errors = dberrors; dberrors.clear();
		}
};

class Field {
	public:
		SQLUSMALLINT col; // col = 0 when not present
		bool col_isbound; // set by fieldByXX methods
		// following fields are valid when col present
		SQLSMALLINT c_type; // specifies SQL_C_XX type
		SQLLEN nosign; // true when unsigned
		SQLLEN length; // length of data buffer
		SQLPOINTER data; // pointer to data buffer
		__fastcall Field(int column) {
			col = column;
			col_isbound = false;
			c_type = SQL_C_DEFAULT;
			nosign = false;
			length = 0;
			data = NULL;
		}
		bool __fastcall isNull(void) {
			if(col == 0) {return false;}
			return length == SQL_NULL_DATA;
		}
		AnsiString __fastcall toString(void) {
			AnsiString string = UNDEFINED;
			if(col == 0 || length == 0 ||
				length == SQL_NULL_DATA) {
				return string;
			}
			switch(c_type) {
				case SQL_C_CHAR:
					string = AnsiString((char *) data);
				break;
				case SQL_C_WCHAR:
					string = AnsiString((wchar_t *) data);
				break;
				case SQL_C_BIT:
					string = AnsiString(*(unsigned char *) data);
				break;
// #pragma message "Query.h: BCB IntToStr does not support unsigned types"
				case SQL_C_TINYINT:
					if(nosign) {string = IntToStr(*((unsigned char *) data));}
					else {string = IntToStr(*((char *) data));}
				break;
				case SQL_C_SHORT:
					if(nosign) {string = IntToStr(*((unsigned short *) data));}
					else {string = IntToStr(*((short *) data));}
				break;
				case SQL_C_LONG:
					// if(nosign) {string = IntToStr(*((unsigned int *) data));}
					// else
					{string = IntToStr(*((int *) data));}
				break;
				case SQL_C_SBIGINT:
					// if(nosign) {string = IntToStr(*((unsigned __int64 *)data));}
					// else
					{string = IntToStr(*((__int64 *) data));}
				break;
				case SQL_C_FLOAT:
					string = FloatToStr(*((float *) data));
				break;
				case SQL_C_DOUBLE:
					string = FloatToStr(*((double *) data));
				break;
				// case SQL_C_BINARY: break;
				case SQL_C_DATE: case SQL_C_TYPE_DATE: {
					DATE_STRUCT date = *((DATE_STRUCT *) data);
					string =YMD(TDateTime(date.year,date.month,date.day));
				} break;
				case SQL_C_TIME: case SQL_C_TYPE_TIME: {
					TIME_STRUCT time = *((TIME_STRUCT *) data);
					string =HMS(TDateTime(time.hour,time.minute,time.second,0));
				} break;
				case SQL_C_TIMESTAMP: case SQL_C_TYPE_TIMESTAMP: {
					TIMESTAMP_STRUCT stamp = *((TIMESTAMP_STRUCT *) data);
					TDateTime date(stamp.year,stamp.month,stamp.day);
					TDateTime time(stamp.hour,stamp.minute,stamp.second,0);
					if(double(time)) {string = YMDHMS(date+time);}
					else {string = YMD(date);}
				} break;
				default: string = "FIELD TYPE NOT SUPPORTED"; break;
				// todo: interval types not supported yet
/*
				case SQL_C_INTERVAL_YEAR:
					SQL_INTERVAL_STRUCT ival = *((SQL_INTERVAL_STRUCT *) data);
					int year = ival.intval.year_month.year;
					string = YMD(TDateTime(year,0,0));
				break;
				case SQL_C_INTERVAL_MONTH:
					SQL_INTERVAL_STRUCT ival = *((SQL_INTERVAL_STRUCT *) data);
					int month = ival.intval.year_month.month;
					string = YMD(TDateTime(0,month,0));
				break;
				case SQL_C_INTERVAL_YEAR_TO_MONTH:
					SQL_INTERVAL_STRUCT ival = *((SQL_INTERVAL_STRUCT *) data);
					int year = ival.intval.year_month.year;
					int month = ival.intval.year_month.month;
					string = YMD(TDateTime(year,0,0));
					string = string + " - " + YMD(TDateTime(0,month,0));
				break;
				case SQL_C_INTERVAL_DAY: break;
				case SQL_C_INTERVAL_HOUR: break;
				case SQL_C_INTERVAL_MINUTE: break;
				case SQL_C_INTERVAL_SECOND: break;
				case SQL_C_INTERVAL_DAY_TO_HOUR:
				case SQL_C_INTERVAL_DAY_TO_MINUTE:
				case SQL_C_INTERVAL_DAY_TO_SECOND:
				case SQL_C_INTERVAL_HOUR_TO_MINUTE:
				case SQL_C_INTERVAL_HOUR_TO_SECOND:
				case SQL_C_INTERVAL_MINUTE_TO_SECOND:
*/
			}
			return string;
		}
};

class Query {
	private:
		SQLHDBC dbc_h;
		SQLHSTMT stmt_h;
		SQLRETURN r;

		// meta data for the result set
		SQLSMALLINT cols;
		SQLCHAR ** col_names;
		SQLSMALLINT * c_datatypes;
		SQLLEN * col_lengths;
		SQLLEN * buf_offsets;
		SQLLEN * nosign;

		// data that applies to a rowset
		SQLCHAR * data_buffer;
		SQLULEN array_size;
		SQLULEN row_index;
		SQLULEN rows;

		void __fastcall setupSQL(void);
		void __fastcall freeVariables(void);
		void __fastcall bindVariables(SQLULEN rowset_size);
		SQLUSMALLINT __fastcall findColumn(AnsiString field_name);
		Query(const Query & q); // invalid to copy a Query object
	public:
		HWND logwindow;
		static AnsiString last_message;
		static SQLRETURN last_retvalue;
		__fastcall Query(void);
		void __fastcall setdbch(SQLHDBC connection);
		void __fastcall freeSQL(bool connected);
		// freeSQL should be called when the Query
		// object is no longer needed or parameter
		// bindings are to be released.
		void __fastcall execSQL(AnsiString query,
			SQLULEN rowset_size = 25);
		void __fastcall prepSQL(AnsiString query);
		bool __fastcall execSQL(SQLULEN rowset_size = 25);
		// This version of execSQL may be called multiple times.
		// Before doing so, it must be initialised with prepSQL.
		// execSQL returns true if parameter data is required.
		int __fastcall getResultSetCols(void) {return cols;}
		// cols is set after the query executes and is zero
		// if there was no result set (i.e: insert/update)
		// note that an -empty- result set always has cols
		bool __fastcall next(void);
		// usage: while(next()) {field = fieldBy...;}
		Field __fastcall fieldByColumn(unsigned int column);
		Field __fastcall fieldByName(AnsiString field_name);
		void __fastcall bindParameter(Field * field);
		// Binds a field as input parameter. If the parameter data is long it
		// can be split into blocks and supplied to execSQL in several passes.
		// To do so, set field.length to either: SQL_LEN_DATA_AT_EXEC(length)
		// or SQL_DATA_AT_EXEC. SQL_LEN_DATA_AT_EXEC(length) should be used if
		// the length of the long data for this parameter is known in advance.
		// If the data is supplied at execution time, field.data can be set to
		// the parameter number or a callback function (see nextParameter)
		void * __fastcall nextParameter(void);
		// nextParameter should be called if the second version of execSQL
		// requests parameter data (returning true). nextParameter returns
		// the token given to bindParameter which can be used to identify
		// the parameter. Call write as often as needed to supply the data.
		void __fastcall write(void * data,SQLLEN length);
		// read() can be used to read binary (long) data. It
		// returns true if there is still more data to read.
		bool __fastcall read(unsigned int column,void * data,
			SQLLEN length,SQLLEN * data_read);
		void __fastcall fieldNames(AnsiString table_name);
		// usage: fieldByName(COLUMN_NAME/TABLE_NAME)
		void __fastcall tableNames(void);

		// SQLHSTMT __fastcall getstmt(void) {return stmt_h;}
		static SQLULEN __fastcall getCsize(SQLSMALLINT s_type,SQLULEN chars);
		static SQLSMALLINT __fastcall getCtype(SQLSMALLINT s_type);
		static SQLSMALLINT __fastcall getStype(SQLSMALLINT c_type);
		static AnsiString __fastcall chkErr(SQLRETURN ret_val,
			SQLSMALLINT handle_type,SQLHANDLE handle);
};
//---------------------------------------------------------------------------
#endif

