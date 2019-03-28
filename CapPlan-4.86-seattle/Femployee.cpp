//$Id: Femployee.cpp,v 1.5 2013/12/26 15:11:33 darseq Exp $
#include <vcl.h>
#pragma hdrstop
#include "Femployee.h"
#include "Fmain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Fbase"
#pragma link "FixedScrollBox"
#pragma resource "*.dfm"
TFemployee1 *Femployee1;
//---------------------------------------------------------------------------
__fastcall TFemployee1::TFemployee1(TComponent* Owner) : TFbase1(Owner) {
	Image->Width = ScrollBox->ClientWidth;
	TCanvas * canvas = Image->Canvas;
	canvas->Brush->Color = clWhite;
	canvas->Pen->Color = clBlack;
	canvas->Pen->Width = 1;
	canvas->Pen->Style = psSolid;
	canvas->Font = Font;
}
//---------------------------------------------------------------------------

TModalResult __fastcall TFemployee1::view(void) {
	LOGFUNC();
	ScrollBox->VertScrollBar->Position = 0;
	return ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TFemployee1::FormKeyDown(TObject *Sender, WORD &Key,
	TShiftState Shift) {
	if(Key == VK_NEXT) {
		ScrollBox->VertScrollBar->Position =
			ScrollBox->VertScrollBar->Position +
			ScrollBox->VertScrollBar->Increment;
	} else if(Key == VK_PRIOR) {
		ScrollBox->VertScrollBar->Position =
			ScrollBox->VertScrollBar->Position -
			ScrollBox->VertScrollBar->Increment;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFemployee1::FormMouseWheelDown(TObject *Sender,
	  TShiftState Shift, TPoint &MousePos, bool &Handled) {
	Word Key = VK_NEXT; KeyDown(Key,TShiftState());
	Handled = true;
}
//---------------------------------------------------------------------------

void __fastcall TFemployee1::FormMouseWheelUp(TObject *Sender,
	  TShiftState Shift, TPoint &MousePos, bool &Handled) {
	Word Key = VK_PRIOR; KeyDown(Key,TShiftState());
	Handled = true;
}
//---------------------------------------------------------------------------

