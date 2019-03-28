//$Id: Fprint.h,v 1.15 2012/04/17 21:13:46 darseq Exp $
#ifndef FprintH
#define FprintH
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
#include "FixedCheckBox.h"
//---------------------------------------------------------------------------
class TFprint1 : public TFrange1
{
__published:	// IDE-managed Components
	TButton *PreviewButton;
	void __fastcall PreviewButtonClick(TObject *Sender);
	void __fastcall FormKeyPress(TObject *Sender, char &Key);
private:	// User declarations
public:		// User declarations
	__fastcall TFprint1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFprint1 *Fprint1;
//---------------------------------------------------------------------------
#endif

