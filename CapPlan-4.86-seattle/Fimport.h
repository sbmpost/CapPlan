//$Id: Fimport.h,v 1.2 2016/08/17 21:47:43 darseq Exp $
#ifndef FimportH
#define FimportH
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
class TFimport1 : public TFrange1
{
__published:	// IDE-managed Components
private:	// User declarations
public:		// User declarations
	__fastcall TFimport1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFimport1 *Fimport1;
//---------------------------------------------------------------------------
#endif

