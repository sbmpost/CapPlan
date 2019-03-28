//$Id: Codes.cpp,v 1.19 2013/02/16 12:18:36 darseq Exp $
#pragma hdrstop
#include "Codes.h"
#include "Exceptions.h"
#include "Foptions.h"
#include <algorithm>
//---------------------------------------------------------------------------
#pragma package(smart_init)
str2code_t Codes::value2code;

void __fastcall Codes::insert(id_t code_name,int color) {
	PROG_IF(code_name == UNDEFINED,"Invalid id, cannot insert code");
	execSQL("insert into codes "+Policy::lock+"values("+
		QuotedStr(code_name)+","+IntToStr(color)+",0)");
}
//---------------------------------------------------------------------------

void __fastcall Codes::remove(id_t code_name) {
	PROG_IF(code_name == UNDEFINED,"Invalid id, cannot remove code");
	execSQL("delete from codes "+Policy::lock+
		"where code_name="+QuotedStr(code_name));
}
//---------------------------------------------------------------------------

void __fastcall Codes::refresh(void) {
	execSQL("select * from codes order by CODE_NAME");
	value2code.clear(); Code code;
	while(next()) {
		code.get(*this);
		pair<AnsiString,Code> value2code_pair;
		value2code_pair.first = code.color;
		value2code_pair.second = code;
		value2code.insert(value2code_pair);
		// Note that a unique key is guaranteed
		// by an unique db index on CODE_COLOR.
	}
}
//---------------------------------------------------------------------------

void __fastcall Codes::getCodeValues(TGetCodeProc Proc) {
	vector<Code> code_list;
	str2code_t::const_iterator pair_it = value2code.begin();
	for(;pair_it != value2code.end();pair_it++) {
		code_list.push_back(pair_it->second);
	}
	sort(code_list.begin(),code_list.end());
	vector<Code>::const_iterator code_it = code_list.begin();
	for(;code_it != code_list.end();code_it++) {Proc(*code_it);}
}
//---------------------------------------------------------------------------

Code __fastcall Codes::color2code(AnsiString color_value) {
	Code code;
	code.color = IntToStr(TFoptions1::o.insert_color);
	if(color_value.Length()) {
		code.color = color_value;
		code.name = IntToHex(StrToInt(color_value),6);
		str2code_t::const_iterator pair_it = value2code.find(color_value);
		if(pair_it != value2code.end()) {code = pair_it->second;}
	}
	return code;
}
//---------------------------------------------------------------------------

Code __fastcall Codes::name2code(AnsiString code_name) {
	Code code;
	code.color = IntToStr(TFoptions1::o.insert_color);
	if(code_name.Length()) {
		str2code_t::const_iterator pair_it = value2code.begin();
		for(;pair_it != value2code.end();pair_it++) {
			if(pair_it->second.name == code_name) {break;}
		}
		if(pair_it != value2code.end()) {code = pair_it->second;}
	}
	return code;
}
//---------------------------------------------------------------------------

