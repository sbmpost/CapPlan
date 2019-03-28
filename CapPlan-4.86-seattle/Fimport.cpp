//$Id: Fimport.cpp,v 1.2 2016/08/17 21:47:43 darseq Exp $
#include <vcl.h>
#pragma hdrstop

#include "Fimport.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FixedCheckBox"
#pragma link "Frange"
#pragma resource "*.dfm"
TFimport1 *Fimport1;
//---------------------------------------------------------------------------
__fastcall TFimport1::TFimport1(TComponent* Owner) : TFrange1(Owner) {
}
//---------------------------------------------------------------------------

