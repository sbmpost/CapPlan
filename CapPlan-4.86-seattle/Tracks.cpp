//$Id: Tracks.cpp,v 1.111 2013/02/16 12:18:36 darseq Exp $
#include <vcl.h>
#pragma hdrstop
#include "Tracks.h"
#include "defines.h"
#include "Exceptions.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

AnsiString __fastcall Apps::getText(id_t app_id,AnsiString field_name) {
	PROG_IF(app_id == UNDEFINED,"Invalid id, cannot get "+field_name);
	execSQL("select "+field_name+" from apps "+Policy::lock+
		"where APP_ID="+app_id+Policy::lock_end);
	if(next() == false) {throw UserException(TR_APPOINTMENTNOTEXISTS);}
	return fieldByName(field_name).toString();
}
//---------------------------------------------------------------------------

void __fastcall Apps::validate(id_t app_id,AnsiString username) {
	PROG_IF(app_id == UNDEFINED,"Invalid id, cannot validate");
	execSQL("select EXCLUSIVE,CANCELLED,CREATED_BY from apps "+
		Policy::lock+"where APP_ID="+app_id+Policy::lock_end);
	if(next()) {
		if(StrToInt(fieldByName("EXCLUSIVE").toString()) &&
			fieldByName("CREATED_BY").toString() != username) {
			throw UserException(ITEMSACCESSDENIED);
		}
		if(StrToInt(fieldByName("CANCELLED").toString())) {
			// In the event that an entry is added to a cancelled
			// appointment, the user can always fix it by selecting
			// the entry with the appointment list and deleting it.
			throw UserException(SELECTEDCANCELLED);
		}
	}
}
//---------------------------------------------------------------------------

id_t __fastcall Apps::insert(id_t per_id,bool exclusive,bool paid,
	int color,AnsiString charge,AnsiString info,AnsiString created_by) {
	PROG_IF(per_id == UNDEFINED,"Invalid id, cannot insert appointment");
	execSQL("insert into apps "+Policy::lock+"(PERSONAL_ID,EXCLUSIVE,"
		"CANCELLED,PAID,COLOR,CHARGE,CREATED_DATE,CREATED_BY,INFOTEXT) "
		"values("+per_id+","+IntToStr((int) exclusive)+",0,"+IntToStr(
		(int) paid)+","+IntToStr(color)+","+charge+"," CURRENT_TIMESTAMP ","+
		QuotedStr(created_by)+","+QuotedStr(info)+")");
	execSQL("select " LAST_ID " as LAST_ID");
	PROG_IF(next() == false,"Could not get unique id");
	id_t new_id = fieldByName("LAST_ID").toString();
	PROG_IF(new_id == UNDEFINED,"Could not get unique id");
	return new_id;
}
//---------------------------------------------------------------------------

void __fastcall Apps::remove(id_t app_id) {
	PROG_IF(app_id == UNDEFINED,"Invalid id, cannot remove appointment");
	execSQL("delete from entries "+Policy::lock+"where APP_ID="+app_id);
	execSQL("delete from apps "+Policy::lock+"where APP_ID="+app_id);
}
//---------------------------------------------------------------------------

void __fastcall Apps::erase(id_t per_id,AnsiString username,bool root_user) {
	PROG_IF(per_id == UNDEFINED,"Invalid id, cannot remove appointments");
	vector<id_t> app_ids; vector<id_t>::const_iterator it;
	// if enclosed in TRYLOCK, locks app_ids
	execSQL("select APP_ID,EXCLUSIVE,CREATED_BY from apps "+
		Policy::lock+"where PERSONAL_ID="+per_id+Policy::lock_end);
	bool access_denied = false;
	while(next()) {
		if(root_user == false &&
			StrToInt(fieldByName("EXCLUSIVE").toString()) &&
			fieldByName("CREATED_BY").toString() != username) {
			access_denied = true; // complete query next()
		}
		app_ids.push_back(fieldByName("APP_ID").toString());
	} if(access_denied) {throw UserException(ITEMSACCESSDENIED);}
	// buffer ids because we cannot delete
	// while iterating over a result set.
	for(it=app_ids.begin();it != app_ids.end();it++) {
		execSQL("delete from entries "+Policy::lock+
			"where APP_ID="+*it);
	}
	execSQL("delete from apps "+Policy::lock+
		"where PERSONAL_ID="+per_id);
	// when person is or was an employee, do
	// not forget to delete its entries
	execSQL("delete from entries "+Policy::lock+
		"where EMPLOYEE_ID="+per_id);
}
//---------------------------------------------------------------------------

AnsiString __fastcall Docs::getText(id_t doc_id,AnsiString field_name) {
	AnsiString per_id,doc_name; getId(doc_id,per_id,doc_name);
	if(per_id == UNDEFINED) {return UNDEFINED;} // necessary for check()
	// PROG_IF(per_id == UNDEFINED,"Invalid id, cannot get "+field_name);
	execSQL("select "+field_name+" from docs "+Policy::lock+
		"where PERSONAL_ID="+per_id+" and DOC_NAME="+
		QuotedStr(doc_name)+Policy::lock_end,1); // 1: SQLGetData
	if(next() == false) {throw UserException(TR_DOCUMENTNOTEXISTS);}
	return fieldByName(field_name).toString();
}
//---------------------------------------------------------------------------

AnsiString __fastcall Docs::EscapeStr(AnsiString str) {
#if defined MS_SQL || defined ACCESS
	str = AnsiReplaceStr(str,"[","[[]");
	str = AnsiReplaceStr(str,"%","[%]");
	str = AnsiReplaceStr(str,"_","[_]");
#elif defined INNODB
	str = AnsiReplaceStr(str,"%","\\%");
	str = AnsiReplaceStr(str,"_","\\_");
#endif
	return str;
}
//---------------------------------------------------------------------------

int __fastcall Docs::getDocNumber(id_t per_id,AnsiString name,AnsiString ext) {
	// It is not useful to use locks here. If the name is invalid,
	// the database will complain when it does integrity checking.
	AnsiString escaped = EscapeStr(name);
	execSQL("select DOC_NAME from docs where DOC_NAME like "+
		QuotedStr(escaped+"_"+ext)+" and PERSONAL_ID="+
		per_id+" order by DOC_NAME");
	int i=0;
	for(;next();i++) {
		if(fieldByName("DOC_NAME").toString().LowerCase() !=
			(name+IntToStr(i)+ext).LowerCase()) {break;}
		else if((i+1) % 10 == 0) {
			// maximum is 100 documents (not counting copies)
			execSQL("select DOC_NAME from docs where DOC_NAME like "+
				QuotedStr(escaped+(i+1)/10+"_"+ext)+" and PERSONAL_ID="+
				per_id+" order by DOC_NAME");
		}
	} return i;
}
//---------------------------------------------------------------------------

bool __fastcall Docs::getId(id_t doc_id,id_t & per_id,AnsiString & doc_name) {
	if(doc_id == UNDEFINED) {per_id = doc_name = UNDEFINED; return false;}
	TStringList * id = new TStringList();
	try {
		id->QuoteChar = '\''; id->DelimitedText = doc_id;
		PROG_IF(id->Count != 2,"Invalid id cannot get document id");
		per_id = id->Strings[0]; doc_name = id->Strings[1];
	} __finally {delete id;}
	return true;
}
//---------------------------------------------------------------------------

/*
void __fastcall Docs::checkSize(int doc_size) {
	if(doc_size > MAXFILEDATA) {
		AnsiString size = IntToStr(MAXFILEDATA/1024); size += " Kb";
		throw UserException(AnsiString::LoadStr(TR_DOCUMENTTOOLARGE)+size);
	}
}
//---------------------------------------------------------------------------

void __fastcall Docs::setData(id_t doc_id,TStream * Stream) {
	AnsiString per_id,doc_name; getId(doc_id,per_id,doc_name);
	PROG_IF(per_id == UNDEFINED,"Invalid id, cannot update document");
	Field field(1);
	field.c_type = SQL_C_BINARY;
	field.data = (SQLPOINTER) field.col;
	field.length = SQL_LEN_DATA_AT_EXEC(Stream->Size);
	bindParameter(&field);
	prepSQL("update docs "+Policy::lock+"set DOC_DATA=? where PERSONAL_ID="+
		per_id+" and doc_name="+QuotedStr(doc_name));
	if(execSQL()) {
		int buffers = 1+field.length/sizeof(io_buffer); int data_read;
		progressbar->Position = 0; progressbar->Visible = true;
		try {
			while(nextParameter()) { // needs to finish (called twice)
				for(int bufnr=2;(data_read = Stream->Read(io_buffer,
					sizeof(io_buffer))) != 0;bufnr++) {
					write(io_buffer,data_read);
					progressbar->Position = (progressbar->Max*bufnr)/buffers;
				}
			}
		} __finally {progressbar->Visible = false;}
	}
}
//---------------------------------------------------------------------------

int __fastcall Docs::getData(id_t doc_id,int doc_size,TStream * stream) {
	getText(doc_id,"DOC_DATA"); Field field = fieldByName("DOC_DATA");
	PROG_IF(field.col_isbound,"Document data column should not be bound");

	long data_read = sizeof(io_buffer); // see return statement
	int buffers = 1+doc_size/sizeof(io_buffer); int bufnr = 2;
	progressbar->Position = 0; progressbar->Visible = true;
	try {
		for(;read(field.col,io_buffer,sizeof(io_buffer),&data_read);bufnr++) {
			stream->Write(io_buffer,sizeof(io_buffer));
			// some odbc drivers can only set the value of data_read
			// on the last read. getData returns the actual data size
			progressbar->Position = (progressbar->Max*bufnr)/buffers;
		}
	} __finally {progressbar->Visible = false;}
	return (bufnr-3)*sizeof(io_buffer)+data_read;
}
//---------------------------------------------------------------------------
*/

AnsiString __fastcall Scheds::getText(id_t schedule_id,AnsiString field_name) {
	PROG_IF(schedule_id == UNDEFINED,"Invalid id, cannot get "+field_name);
	execSQL("select "+field_name+" from schedules "+Policy::lock+
		"where SCHEDULE_ID="+schedule_id+Policy::lock_end);
	if(next() == false) {throw UserException(TR_SCHEDULENOTEXISTS);}
	return fieldByName(field_name).toString();
}
//---------------------------------------------------------------------------

void __fastcall Scheds::insert(Schedule & schedule) {
	AnsiString hidden = schedule.hidden ? "1" : "0";
	execSQL("insert into schedules "+Policy::lock+"(SCHEDULE_NAME,"
		"WEEK_START,WEEK_STOP,DAY_START,DAY_STOP,DAY_UNITS,HIDDEN,"
		"CREATED_DATE,CREATED_BY,NOTETEXT) values("+QuotedStr(schedule.name)+
		","+IntToStr(schedule.wstart)+","+IntToStr(schedule.wstop)+
		","+IntToStr(schedule.dstart)+","+IntToStr(schedule.dstop)+
		","+IntToStr(schedule.tunits)+","+hidden+"," CURRENT_TIMESTAMP ","+
		QuotedStr(schedule.created_by)+","+QuotedStr(schedule.note)+")");
	execSQL("select " LAST_ID " as LAST_ID");
	PROG_IF(next() == false,"Could not get unique id");
	schedule.id = fieldByName("LAST_ID").toString();
	PROG_IF(schedule.id == UNDEFINED,"Could not get unique id");
}
//---------------------------------------------------------------------------

void __fastcall Scheds::remove(Schedule & schedule) {
	PROG_IF(schedule.id == UNDEFINED,"Invalid id, cannot remove schedule");
	execSQL("delete from employees "+Policy::lock+
		"where SCHEDULE_ID="+schedule.id);
	execSQL("delete from schedules "+Policy::lock+
		"where SCHEDULE_ID="+schedule.id);
}
//---------------------------------------------------------------------------

