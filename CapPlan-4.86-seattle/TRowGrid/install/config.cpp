//$Id: config.cpp,v 1.10 2006/09/08 01:15:29 darseq Exp $
#include <vcl.h>
#include <registry.hpp>
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
__fastcall TSetupConfig::TSetupConfig(TComponent* Owner)
	: TSetupBase(Owner) {
	TRegistry * registry = new TRegistry();
	registry->RootKey = HKEY_LOCAL_MACHINE;
	AnsiString current_key = "\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion";
	if(registry->OpenKey(current_key,false)) {
		INSTALLROOT->Text = registry->ReadString("ProgramFilesDir");
		registry->CloseKey();
	}
	INSTALLROOT->Text = INSTALLROOT->Text+"\\"+Application->Title;

	// use HKEY_CURRENT_USER entry?
	current_key = "\\SOFTWARE\\Borland\\C++Builder\\6.0";
	if(registry->OpenKey(current_key,false)) {
		BCB6ROOT->Text = registry->ReadString("RootDir");
		registry->CloseKey();
	} else {
		BCB6ROOT->Text = INSTALLROOT->Text;
		// MessageBeep(MB_ICONEXCLAMATION);
		MessageDlg("Could not find a Borland C++ builder 6.0 installation. "
			"Therefore\nthe borland directory has been set to the installation "
			"directory.\nAfter the installer has finished copying files, "
			"please refer to the\nreadme.txt file to complete the ActiveX "
			"installation.",mtInformation,TMsgDlgButtons() << mbOK,0);
	}
	delete registry;

#ifndef DEMO
	// Next->Enabled = false;
	// GroupBox2->Visible = true;
	// GroupBox2->TabOrder = 0;
#endif
}
//---------------------------------------------------------------------------

void __fastcall TSetupConfig::Button1Click(TObject *Sender) {
	try {Fselect->DirectoryListBox1->Directory = BCB6ROOT->Text;}
	catch (...) {Fselect->DirectoryListBox1->Directory = "";}

	Fselect->ShowModal();
	BCB6ROOT->Text = Fselect->DirectoryListBox1->Directory;
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
	BCB6ROOT->Text = normalizePath(BCB6ROOT->Text);
	INSTALLROOT->Text = normalizePath(INSTALLROOT->Text);
	TSetupBase::NextClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TSetupConfig::UserInfoChange(TObject *Sender) {
	Next->Enabled = USERNAME->Text.Length() && PASSWORD->Text.Length();
}
//---------------------------------------------------------------------------

void __fastcall TSetupConfig::EditKeyPress(TObject *Sender, char &Key) {
	if(Key == VK_RETURN && Next->Enabled) {Next->Click(); Key = '\0';}
}
//---------------------------------------------------------------------------
