//$Id: Ffind.h,v 1.8 2012/02/14 17:54:48 darseq Exp $
#ifndef FfindH
#define FfindH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <CheckLst.hpp>
#include <Mask.hpp>
#include "FixedCheckBox.h"
#include "Frange.h"
//---------------------------------------------------------------------------

class TFfind1 : public TFrange1
{
__published:	// IDE-managed Components
private:	// User declarations
	void __fastcall buildIdList(id_t select_id);
public:		// User declarations
	__fastcall TFfind1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFfind1 *Ffind1;
//---------------------------------------------------------------------------
#endif

