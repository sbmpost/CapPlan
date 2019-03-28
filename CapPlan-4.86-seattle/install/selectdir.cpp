//$Id: selectdir.cpp,v 1.3 2010/12/13 01:08:06 darseq Exp $
#include <vcl.h>
#pragma hdrstop
#include "selectdir.h"
#include "dir.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFselect *Fselect;
//---------------------------------------------------------------------------
__fastcall TFselect::TFselect(TComponent* Owner) : TForm(Owner) {
	DriveComboBox1->Drive = getdisk()+'a';
}
//---------------------------------------------------------------------------

void __fastcall TFselect::DriveComboBox1Change(TObject *Sender) {
	DirectoryListBox1->Drive = DriveComboBox1->Drive;
}
//---------------------------------------------------------------------------

