//$Id: welcome.cpp,v 1.1 2005/03/14 23:23:52 darseq Exp $
#include <vcl.h>
#pragma hdrstop

#include "welcome.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "setup"
#pragma resource "*.dfm"
TSetupWelcome *SetupWelcome;
//---------------------------------------------------------------------------
__fastcall TSetupWelcome::TSetupWelcome(TComponent* Owner) : TSetupBase(Owner) {
	Back->Enabled = false;
}
//---------------------------------------------------------------------------

