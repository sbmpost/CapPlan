//$Id: config.h,v 1.5 2005/03/22 16:48:58 darseq Exp $
#ifndef configH
#define configH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "setup.h"
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TSetupConfig : public TSetupBase
{
__published:	// IDE-managed Components
	TGroupBox *GroupBox1;
	TLabeledEdit *DATABASEHOST;
	TLabeledEdit *INSTALLROOT;
	TButton *Button2;
	TGroupBox *GroupBox2;
	TLabeledEdit *USERNAME;
	TLabeledEdit *PASSWORD;
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall NextClick(TObject *Sender);
	void __fastcall UserInfoChange(TObject *Sender);
	void __fastcall EditKeyPress(TObject *Sender, char &Key);
private:	// User declarations
public:		// User declarations
	__fastcall TSetupConfig(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSetupConfig *SetupConfig;
//---------------------------------------------------------------------------
#endif
