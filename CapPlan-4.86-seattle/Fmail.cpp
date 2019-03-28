//$Id$
#include <vcl.h>
#pragma hdrstop
#include "Fmail.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Frange"
#pragma link "Fprint"
#pragma link "FixedCheckBox"
#pragma resource "*.dfm"
TFmail1 *Fmail1;
//---------------------------------------------------------------------------
__fastcall TFmail1::TFmail1(TComponent* Owner): TFprint1(Owner) {
}
//---------------------------------------------------------------------------

void __fastcall TFmail1::FormClose(TObject *Sender, TCloseAction &Action) {
	LOGFUNC();
	if(ModalResult == mrOk) {
		setAndCheckInput();
		AnsiString message;
		message = AnsiString::LoadStr(FMAIL_NUMBEROFMAILSTOSEND)+
			IntToStr((int) selected.size())+", "+
			AnsiString::LoadStr(CONTINUEQUESTION);
		if(MSGYN(message.c_str()) == ID_NO) {
			ModalResult = mrNone;
		}
	}
	TFbase1::FormClose(Sender,Action);
}
//---------------------------------------------------------------------------

void __fastcall TFmail1::AttachIcalBoxClick(TObject *Sender) {
	CheckBox->Enabled = !AttachIcalBox->Checked;
	CheckBox->Checked = true;
}
//---------------------------------------------------------------------------

