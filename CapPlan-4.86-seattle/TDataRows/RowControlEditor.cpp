//$Id: RowControlEditor.cpp,v 1.3 2013/12/21 20:00:16 darseq Exp $
#include <basepch.h>
#pragma hdrstop
#include <ComCtrls.hpp>
#include "RowControlEditor.h"
#include "RowControl.h"
#include "DataRows.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
static inline void ValidCtrCheck(TRowControlEditor *) {
}
//---------------------------------------------------------------------------

namespace Rowcontroleditor { // Rowcontroledit would not work?!
	void __fastcall PACKAGE Register() {
		RegisterComponentEditor(__classid(TRowControl),
			__classid(TRowControlEditor));
	}
}
//---------------------------------------------------------------------------

UnicodeString __fastcall TRowControlEditor::GetVerb(int Index) {
	if(Index == 0) {return "New page";}
	else if(Index == 1) {return "Next page";}
	else if(Index == 2) {return "Previous page";}
	else if(Index == 3) {return "Delete page";}
	else {return "";}
}
//---------------------------------------------------------------------------

void __fastcall TRowControlEditor::PrepareItem(int Index,
	const _di_IMenuItem AItem) {
	if(Index == 3) {AItem->Enabled = false;} // delete by TTabsheet itself
}
//---------------------------------------------------------------------------

void __fastcall TRowControlEditor::ExecuteVerb(int Index) {
	TPageControl * page_control = (TPageControl *) Component;
	if(Index == 0) {
		TTabSheet * tabsheet = (TTabSheet *)
			Designer->CreateComponent(__classid(TTabSheet),page_control,
			0,0,0,0); // tabsheet size is set when its PageControl is set
		tabsheet->PageControl = page_control;
		// TDataRows * datarows = (TDataRows *)
			Designer->CreateComponent(__classid(TDataRows),tabsheet,0,8,
			tabsheet->ClientWidth,tabsheet->ClientHeight - 8 + 1);
		// datarows->SetSubComponent(tabsheet); (not necessary?)
	}
	else if(Index == 1) {page_control->SelectNextPage(true,false);}
	else if(Index == 2) {page_control->SelectNextPage(false,false);}
}
//---------------------------------------------------------------------------

