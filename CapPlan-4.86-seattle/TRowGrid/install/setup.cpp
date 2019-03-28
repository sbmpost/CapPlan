//$Id: setup.cpp,v 1.6 2005/03/28 00:56:48 darseq Exp $
#include <vcl.h>
#pragma hdrstop

#include "setup.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSetupBase *SetupBase;
//---------------------------------------------------------------------------
__fastcall TSetupBase::TSetupBase(TComponent* Owner) : TForm(Owner) {
	form_list.push_back(this);
	Caption = Application->Title;
}
//---------------------------------------------------------------------------

void __fastcall TSetupBase::NextClick(TObject *Sender) {
	list<TSetupBase *>::iterator it = form_list.begin();
	for(;it != form_list.end();++it) {if(*it == this) {break;}}
	if(it == form_list.end()) {throw Exception("Invalid form");}
	if(++it != form_list.end()) {Visible = false; (*it)->Visible = true;}
}
//---------------------------------------------------------------------------

void __fastcall TSetupBase::BackClick(TObject *Sender) {
	list<TSetupBase *>::reverse_iterator rit = form_list.rbegin();
	for(;rit != form_list.rend();++rit) {if(*rit == this) {break;}}
	if(rit == form_list.rend()) {throw Exception("Invalid form");}
	if(++rit != form_list.rend()) {Visible = false; (*rit)->Visible = true;}
}
//---------------------------------------------------------------------------

void __fastcall TSetupBase::CancelClick(TObject *Sender) {Close();}
//---------------------------------------------------------------------------

AnsiString __fastcall TSetupBase::normalizePath(AnsiString path) {
	//  foo -> foo, \\foo\\\bar\\ -> \foo\bar
	AnsiString norm; int i=1;
	while(i <= path.Length()) {
		for(;i <= path.Length() && path[i] != '\\';i++) {norm += path[i];}
		for(;i <= path.Length() && path[i] == '\\';i++);
		if(i <= path.Length()) {norm += '\\';}
	}
	return norm;
}
//---------------------------------------------------------------------------

void __fastcall TSetupBase::FormClose(TObject *Sender,TCloseAction &Action) {
	AnsiString message = "Exit "+Application->Title+" setup?";
	if(MessageDlg(message.c_str(),mtConfirmation,
		TMsgDlgButtons() << mbYes << mbNo,0) == mrYes) {
		exit(0); // an immediate exit is requested
	} else {Action = caNone;}
}
//---------------------------------------------------------------------------

void __fastcall TSetupBase::FormShow(TObject *Sender) {
	if(Next->Enabled) {ActiveControl = Next;}
}
//---------------------------------------------------------------------------

void __fastcall TSetupBase::FormKeyPress(TObject *Sender, char &Key) {
	if(Key == VK_ESCAPE) {Close();}
}
//---------------------------------------------------------------------------

