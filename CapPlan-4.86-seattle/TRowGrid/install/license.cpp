//$Id: license.cpp,v 1.4 2005/03/25 02:24:57 darseq Exp $
#include <vcl.h>
#pragma hdrstop

#include "license.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "setup"
#pragma resource "*.dfm"
TSetupLicense *SetupLicense;
//---------------------------------------------------------------------------
__fastcall TSetupLicense::TSetupLicense(TComponent* Owner) : TSetupBase(Owner) {
	Next->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TSetupLicense::CheckBox1Click(TObject *Sender) {
	Next->Enabled = CheckBox1->Checked;
	if(Next->Enabled) {ActiveControl = Next;}
}
//---------------------------------------------------------------------------
