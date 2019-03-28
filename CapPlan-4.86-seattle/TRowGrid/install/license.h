//$Id: license.h,v 1.2 2005/02/20 02:27:36 darseq Exp $
#ifndef licenseH
#define licenseH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "setup.h"
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TSetupLicense : public TSetupBase
{
__published:	// IDE-managed Components
	TMemo *LICENSE;
	TCheckBox *CheckBox1;
	void __fastcall CheckBox1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TSetupLicense(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSetupLicense *SetupLicense;
//---------------------------------------------------------------------------
#endif
