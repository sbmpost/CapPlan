//$Id: config.cpp,v 1.25 1998/01/26 00:26:24 darseq Exp $
#include <vcl.h>
#include <registry.hpp>
#include <stdio.h>
#pragma hdrstop

#include "config.h"
#include "selectdir.h"
#include "finish.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "setup"
#pragma resource "*.dfm"
TSetupConfig *SetupConfig;
//---------------------------------------------------------------------------
__fastcall TSetupConfig::TSetupConfig(TComponent* Owner) : TSetupBase(Owner) {
	TRegistry * registry = new TRegistry();
	registry->Access = KEY_READ;
	registry->RootKey = HKEY_CURRENT_USER;
	AnsiString current_key = "\\Software\\PostWare\\" PRODUCT;
	INSTALLROOT->Text = ExtractFileDrive(Application->ExeName);
	INSTALLROOT->Text = INSTALLROOT->Text+"\\" PRODUCT;
	if(registry->OpenKey(current_key,false)) {
		INSTALLROOT->Text = registry->ReadString("installroot");
		registry->CloseKey();
	} else {INSTALLROOT->Text = getPath("USERPROFILE")+"\\" PRODUCT;}
/*
		registry->RootKey = HKEY_LOCAL_MACHINE;
		current_key = "\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion";
		if(registry->OpenKey(current_key,false)) {
			INSTALLROOT->Text = registry->ReadString("ProgramFilesDir");
			registry->CloseKey();
			AnsiString filename(INSTALLROOT->Text+"\\"+String(tmpnam(NULL)));
			int handle = FileCreate(filename);
			if(handle == -1) {INSTALLROOT->Text = getPath("USERPROFILE");}
			else {FileClose(handle); DeleteFile(filename);}
		} else {INSTALLROOT->Text = getPath("USERPROFILE");}
		INSTALLROOT->Text = INSTALLROOT->Text+"\\"PRODUCT;
*/
	registry->RootKey = HKEY_CURRENT_USER;
#ifdef MS_SQL
	current_key = "\\Software\\ODBC\\ODBC.INI\\MSSQLsource";
#elif defined INNODB
	current_key = "\\Software\\ODBC\\ODBC.INI\\MYSQLsource";
#endif
	if(registry->OpenKey(current_key,false)) {
		AnsiString host = registry->ReadString("SERVER");
		if(host.Length()) {DATABASEHOST->Text = host;}
		registry->CloseKey();
	}
	delete registry;

#if !defined DEMO && !defined FREEWARE
//	Next->Enabled = false;
//	GroupBox2->Visible = true;
//	GroupBox2->TabOrder = 0;
#endif
}
//---------------------------------------------------------------------------

void __fastcall TSetupConfig::Button2Click(TObject *Sender) {
	try {Fselect->DirectoryListBox1->Directory = INSTALLROOT->Text;}
	catch (...) {Fselect->DirectoryListBox1->Directory = "";}

	Fselect->ShowModal();
	INSTALLROOT->Text = Fselect->DirectoryListBox1->Directory;
}
//---------------------------------------------------------------------------

void __fastcall TSetupConfig::NextClick(TObject *Sender) {
	INSTALLROOT->Text = normalizePath(INSTALLROOT->Text);
	TSetupBase::NextClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TSetupConfig::UserInfoChange(TObject *Sender) {
	Next->Enabled = USERNAME->Text.Length() && PASSWORD->Text.Length();
}
//---------------------------------------------------------------------------

void __fastcall TSetupConfig::EditKeyPress(TObject *Sender,char &Key) {
	if(Key == VK_RETURN && Next->Enabled) {Next->Click(); Key = '\0';}
}
//---------------------------------------------------------------------------

