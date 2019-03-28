//$Id: Fcomplete.cpp,v 1.24 2011/09/27 15:12:26 darseq Exp $
#include <vcl.h>
#pragma hdrstop
#include "Fcomplete.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Fbase"
#pragma link "FixedCheckBox"
#pragma resource "*.dfm"
TFcomplete1 *Fcomplete1;
//---------------------------------------------------------------------------
__fastcall TFcomplete1::TFcomplete1(TComponent* Owner) : TFbase1(Owner) {
	Color = clBtnFace;
}
//---------------------------------------------------------------------------

void __fastcall TFcomplete1::FormShow(TObject *Sender) {
	ActiveControl = RemoveItemBox;
}
//---------------------------------------------------------------------------

void __fastcall TFcomplete1::CancelButtonClick(TObject *Sender) {
	ModalResult = mrIgnore;
}
//---------------------------------------------------------------------------

void __fastcall TFcomplete1::FormKeyPress(TObject *Sender, char &Key) {
	if(tolower(Key) == tolower(OkButton->Caption[2])) {
		OkButton->Click(); Key = '\0';
	} else if(Key == VK_ESCAPE ||
		tolower(Key) == tolower(CancelButton->Caption[2])) {
		CancelButton->Click(); Key = '\0';
	} else if(Key == VK_RETURN) {
		OkButton->Click(); Key = '\0';
	}
	TFbase1::FormKeyPress(Sender,Key);
}
//---------------------------------------------------------------------------

void __fastcall TFcomplete1::FormKeyDown(TObject *Sender, WORD &Key,
	  TShiftState Shift) {
	if(Key == VK_DOWN || Key == VK_UP ||
		Key == VK_RIGHT || Key == VK_LEFT) {
		ActiveControl = FindNextControl(ActiveControl,
			Key == VK_DOWN || Key == VK_RIGHT,true,false);
		Key = '\0';
	}
}
//---------------------------------------------------------------------------

