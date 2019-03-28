//$Id: FixedCheckBox.cpp,v 1.2 2010/09/12 22:48:39 darseq Exp $
#include <vcl.h>
#pragma hdrstop
#include "FixedCheckBox.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
static inline void ValidCtrCheck(TFixedCheckBox *) {
	new TFixedCheckBox(NULL);
}
//---------------------------------------------------------------------------

__fastcall TFixedCheckBox::TFixedCheckBox(TComponent* Owner): TCheckBox(Owner) {
	internal_call = false;
}
//---------------------------------------------------------------------------

namespace Fixedcheckbox {
	void __fastcall PACKAGE Register() {
		TComponentClass classes[1] = {__classid(TFixedCheckBox)};
		RegisterComponents("PostWare", classes, 0);
	}
}
//---------------------------------------------------------------------------

