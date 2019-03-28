//$Id: Fcomplete.h,v 1.17 2012/02/14 17:54:48 darseq Exp $
#ifndef FcompleteH
#define FcompleteH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include "Fbase.h"
#include "FixedCheckBox.h"
//---------------------------------------------------------------------------

class TFcomplete1 : public TFbase1
{
__published:	// IDE-managed Components
	TButton *CancelButton;
	TButton *OkButton;
	TGroupBox *GroupBox1;
	TFixedCheckBox *RepurchaseBox;
	TFixedCheckBox *InvoiceBox;
	TFixedCheckBox *RemoveItemBox;
	void __fastcall FormKeyPress(TObject *Sender, char &Key);
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
		  TShiftState Shift);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TFcomplete1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFcomplete1 *Fcomplete1;
//---------------------------------------------------------------------------
#endif

