//$Id: Clients.cpp,v 1.63 2013/02/16 11:41:37 darseq Exp $
#include <vcl.h>
#pragma hdrstop
#include "Clients.h"
#include "Exceptions.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

AnsiString __fastcall Persons::getCaption(id_t per_id) {
	// PROG_IF(per_id == UNDEFINED,"Invalid id, cannot get caption");
	// schedules can be shown without having a valid person id
	if(per_id == UNDEFINED) {return UNDEFINED;}
	execSQL("select LASTNAME,TELEPHONE,TELEPHONE2 from personal "+
		Policy::lock+"where PERSONAL_ID="+per_id+Policy::lock_end);
	if(next() == false) {throw UserException(CL_PERSONNOTEXISTS);}
	return fieldByName("LASTNAME").toString()+" ("+
		fieldByName("TELEPHONE").toString()+"/"+
		fieldByName("TELEPHONE2").toString()+")";
}
//---------------------------------------------------------------------------

AnsiString __fastcall Persons::getText(id_t per_id,AnsiString field_name) {
	PROG_IF(per_id == UNDEFINED,"Invalid id, cannot get "+field_name);
	execSQL("select "+field_name+" from personal "+Policy::lock+
		"where PERSONAL_ID="+per_id+Policy::lock_end);
	if(next() == false) {throw UserException(CL_PERSONNOTEXISTS);}
	return(fieldByName(field_name).toString());
}
//---------------------------------------------------------------------------

void __fastcall Persons::insert(Person & person) {
	AnsiString hidden = person.hidden ? "1" : "0";
	// insert default information
	execSQL("insert into personal "+Policy::lock+"(LASTNAME,FIRSTNAME,"
		"PREFIX,INITIALS,ADDRESS,POSTCODE,CITY,TELEPHONE,TELEPHONE2,"
		"BIRTHDATE,HIDDEN,CREATED_DATE,CREATED_BY,MEMOTEXT) values("+
		QuotedStr(person.lastname)+",'','','',"+QuotedStr(person.address)+
		","+QuotedStr(person.postcode)+","+QuotedStr(person.city)+
		","+QuotedStr(person.telephone)+","+QuotedStr(person.telephone2)+
		"," DS+YMD(TDateTime(0,0,0,0))+DS ","+hidden+"," CURRENT_TIMESTAMP ","+
		QuotedStr(person.created_by)+","+QuotedStr(person.memo)+")");
	execSQL("select " LAST_ID " as LAST_ID");
	PROG_IF(next() == false,"Could not get unique id");
	person.id = fieldByName("LAST_ID").toString();
	PROG_IF(person.id == UNDEFINED,"Could not get unique id");
}
//---------------------------------------------------------------------------

void __fastcall Persons::remove(Person & person) {
	PROG_IF(person.id == UNDEFINED,"Invalid id, cannot remove person");
	execSQL("delete from personal "+Policy::lock+
		"where PERSONAL_ID="+person.id);
}
//---------------------------------------------------------------------------

