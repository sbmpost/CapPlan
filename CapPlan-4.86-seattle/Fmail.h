//$Id$
#ifndef FmailH
#define FmailH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <CheckLst.hpp>
#include <Mask.hpp>
#include "Frange.h"
#include "Fprint.h"
#include "FixedCheckBox.h"
//---------------------------------------------------------------------------
class TFmail1 : public TFprint1
{
__published:	// IDE-managed Components
	TFixedCheckBox *AttachIcalBox;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall AttachIcalBoxClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TFmail1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFmail1 *Fmail1;
//---------------------------------------------------------------------------
#endif

