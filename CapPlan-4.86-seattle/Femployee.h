//$Id: Femployee.h,v 1.3 2013/12/26 15:11:33 darseq Exp $
#ifndef FemployeeH
#define FemployeeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <Graphics.hpp>
#include "Fbase.h"
#include "FixedScrollBox.h"
//---------------------------------------------------------------------------
class TFemployee1 : public TFbase1
{
__published:	// IDE-managed Components
	TFixedScrollBox *ScrollBox;
	TImage *Image;
	void __fastcall FormMouseWheelUp(TObject *Sender, TShiftState Shift,
		  TPoint &MousePos, bool &Handled);
	void __fastcall FormMouseWheelDown(TObject *Sender, TShiftState Shift,
		  TPoint &MousePos, bool &Handled);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
		  TShiftState Shift);
private:	// User declarations
public:		// User declarations
	__fastcall TFemployee1(TComponent* Owner);
	TModalResult __fastcall view(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TFemployee1 *Femployee1;
//---------------------------------------------------------------------------
#endif

