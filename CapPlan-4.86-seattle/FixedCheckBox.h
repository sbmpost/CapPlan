//$Id: FixedCheckBox.h,v 1.1 2005/05/25 20:15:18 darseq Exp $
#ifndef FixedCheckBoxH
#define FixedCheckBoxH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------
class PACKAGE TFixedCheckBox : public TCheckBox
{
private:
	bool internal_call;
protected:
	DYNAMIC void __fastcall Click(void) {
		if(internal_call == false && OnClick) {OnClick(this);}
	}
	virtual void __fastcall SetChecked(bool Value) {
		if(Value) {SetState(cbChecked);}
		else {SetState(cbUnchecked);}
	}
	virtual void __fastcall SetState(TCheckBoxState NewState) {
		internal_call = true;
		TCheckBox::State = NewState;
		internal_call = false;
	}
public:
	__fastcall TFixedCheckBox(TComponent* Owner);
__published:
};
//---------------------------------------------------------------------------
#endif

