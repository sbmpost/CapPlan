//$Id: Fprint.cpp,v 1.12 2010/11/28 00:36:31 darseq Exp $
#include <vcl.h>
#pragma hdrstop
#include "Fprint.h"
#include "Fmail.h"
#include "Fmain.h"
#include "Femployee.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Frange"
#pragma link "FixedCheckBox"
#pragma resource "*.dfm"
TFprint1 *Fprint1;
//---------------------------------------------------------------------------
__fastcall TFprint1::TFprint1(TComponent* Owner) : TFrange1(Owner) {
}
//---------------------------------------------------------------------------

void __fastcall TFprint1::PreviewButtonClick(TObject *Sender) {
	LOGFUNC();
	setAndCheckInput();
	Fmain1->printEntries(NULL, this);
	Femployee1->view();
}
//---------------------------------------------------------------------------

void __fastcall TFprint1::FormKeyPress(TObject *Sender, char &Key) {
	if(tolower(Key) == tolower(PreviewButton->Caption[2])) {
		PreviewButton->Click(); Key = '\0';
	}
	TFrange1::FormKeyPress(Sender,Key);
}
//---------------------------------------------------------------------------

