//$Id: FixedScrollBox.cpp,v 1.1 2010/09/12 22:51:10 darseq Exp $
#include <vcl.h>
#pragma hdrstop
#include "FixedScrollBox.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
static inline void ValidCtrCheck(TFixedScrollBox *) {
	new TFixedScrollBox(NULL);
}
//---------------------------------------------------------------------------

__fastcall TFixedScrollBox::TFixedScrollBox(TComponent* Owner)
	: TScrollBox(Owner) {
}
//---------------------------------------------------------------------------

namespace Fixedscrollbox {
	void __fastcall PACKAGE Register() {
		TComponentClass classes[1] = {__classid(TFixedScrollBox)};
		RegisterComponents("PostWare", classes, 0);
	}
}
//---------------------------------------------------------------------------

