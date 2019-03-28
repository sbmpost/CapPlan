//$Id: welcome.h,v 1.2 2005/02/20 02:27:36 darseq Exp $
#ifndef welcomeH
#define welcomeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "setup.h"
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TSetupWelcome : public TSetupBase
{
__published:	// IDE-managed Components
	TMemo *welcome;
private:	// User declarations
public:		// User declarations
	__fastcall TSetupWelcome(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSetupWelcome *SetupWelcome;
//---------------------------------------------------------------------------
#endif
