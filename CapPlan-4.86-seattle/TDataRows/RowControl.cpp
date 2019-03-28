//$Id: RowControl.cpp,v 1.3 2007/11/25 02:08:21 darseq Exp $
#include <vcl.h>
#pragma hdrstop
//#pragma resource "*.res"
#include "RowControl.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
static inline void ValidCtrCheck(TRowControl *) {
	new TRowControl(NULL);
}
//---------------------------------------------------------------------------

__fastcall TRowControl::TRowControl(TComponent* Owner) : TPageControl(Owner) {
	OnChanging = NULL;
}
//---------------------------------------------------------------------------

namespace Rowcontrol {
	void __fastcall PACKAGE Register() {
		TComponentClass classes[1] = {__classid(TRowControl)};
		RegisterComponents("PostWare", classes, 0);
	}
}
//---------------------------------------------------------------------------

