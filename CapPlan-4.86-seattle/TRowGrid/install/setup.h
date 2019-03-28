//$Id: setup.h,v 1.5 2005/03/28 00:56:48 darseq Exp $
#ifndef setupH
#define setupH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <list.h>
//---------------------------------------------------------------------------
class TSetupBase : public TForm
{
__published:	// IDE-managed Components
	TShape *Shape1;
	TButton *Back;
	TButton *Next;
	TButton *Cancel;
	TImage *Image1;
	void __fastcall NextClick(TObject *Sender);
	void __fastcall BackClick(TObject *Sender);
	void __fastcall CancelClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormKeyPress(TObject *Sender, char &Key);
private:	// User declarations
	static list<TSetupBase *> TSetupBase::form_list;
public:		// User declarations
	static AnsiString __fastcall normalizePath(AnsiString path);
	__fastcall TSetupBase(TComponent* Owner);
};
//---------------------------------------------------------------------------
list<TSetupBase *> TSetupBase::form_list;
extern PACKAGE TSetupBase *SetupBase;
//---------------------------------------------------------------------------
#endif
