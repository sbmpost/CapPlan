//$Id: selectdir.cpp,v 1.2 2005/02/16 04:13:24 darseq Exp $
#include <vcl.h>
#pragma hdrstop

#include "selectdir.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFselect *Fselect;
//---------------------------------------------------------------------------
__fastcall TFselect::TFselect(TComponent* Owner)
	: TForm(Owner) {
}
//---------------------------------------------------------------------------

void __fastcall TFselect::DriveComboBox1Change(TObject *Sender) {
	DirectoryListBox1->Drive = DriveComboBox1->Drive;
}
//---------------------------------------------------------------------------
