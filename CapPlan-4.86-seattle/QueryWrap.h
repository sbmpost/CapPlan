//$Id: QueryWrap.h,v 1.3 2013/12/09 23:29:50 darseq Exp $
#ifndef QueryH
#define QueryH
#include <SqlExpr.hpp>
#include <StdCtrls.hpp>
#include <sql.h>
#include <vector>
#ifdef _DEBUG
#include "Exceptions.h"
#endif
using namespace std;

#define UNDEFINED ""
#define MAX_NAME 256 // for columns and SQLGetInfo
#define YMD(date) (FormatDateTime("yyyy'-'mm'-'dd",date))
#define HMS(time) (FormatDateTime("hh':'mm':'ss",time))
#define YMDHMS(date) (FormatDateTime("yyyy'-'mm'-'dd' 'hh':'mm':'ss",date))

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

/*
typedef struct database_error {
	char sql_state[255+1];
	char message[255];
	int native_err;
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
*/

class Field {
	private:
		AnsiString value;
	public:
		unsigned short col;
		_fastcall Field(int field_number,
			AnsiString field_value) {
			col = field_number;
			value = field_value;
		}
		AnsiString _fastcall toString(void) {
			return value;
		}
};

class Query {
	private:
		TSQLQuery * sql_query;
		bool first;
	public:
		__fastcall Query(void) {
			sql_query = new TSQLQuery(NULL);
		}
		__fastcall ~Query() {
			delete sql_query;
		}
		void __fastcall execSQL(AnsiString query,
			unsigned rowset_size = 25) {
#ifdef _DEBUG
			Exceptions::writeLog(query);
#endif
			sql_query->Close();
			sql_query->SQL->Clear();
			sql_query->SQL->Add(query);
			SQLCALL(sql_query->ExecSQL(true););
			first = true;
		}
		bool _fastcall next() {
			if(sql_query->Active) {
				if(first) {first = false; sql_query->First();}
				else {sql_query->Next();}
				if(sql_query->Eof) {sql_query->Close();}
			}
			return sql_query->Active;
		}
		Field _fastcall fieldByName(AnsiString field_name) {
			TField * field = NULL;
			int field_number = 0;
			AnsiString value = UNDEFINED;
			try {
				field = sql_query->FieldByName(field_name);
				field_number = field->FieldNo;
			} catch(EDatabaseError & e) {}
			if(field && field->IsNull == false) {
				value = field->Value;
			}
			return Field(field_number,value);
		}
};
//---------------------------------------------------------------------------
#endif

