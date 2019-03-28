//$Id: FixedScrollBox.h,v 1.1 2010/09/12 22:51:10 darseq Exp $
#ifndef FixedScrollBoxH
#define FixedScrollBoxH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class PACKAGE TFixedScrollBox : public TScrollBox
{
private:
protected:
	void __fastcall AutoScrollInView(TControl* AControl) {}
public:
	__fastcall TFixedScrollBox(TComponent* Owner);
__published:
};
//---------------------------------------------------------------------------
#endif

