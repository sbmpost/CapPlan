//$Id: Fcodes.h,v 1.17 2012/02/14 17:54:48 darseq Exp $
#ifndef FcodesH
#define FcodesH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
#include "Fbase.h"
#include "Codes.h"
#include "DataRows.h"
//---------------------------------------------------------------------------

class TFcodes1 : public TFbase1
{
__published:	// IDE-managed Components
	TDataRows *CodeRows;
	TMainMenu *MainMenu1;
	TMenuItem *new1;
	TMenuItem *delete1;
	TMenuItem *colors1;
	TColorDialog *ColorDialog1;
	TMenuItem *edit1;
	TMenuItem *modify1;
	TMenuItem *N1;
	TMenuItem *selectAll1;
	TMenuItem *export1;
	TMenuItem *print1;
	void __fastcall CodeRowsBeforeRefresh(TObject *Sender,
		  QueryPtr &ResultSet, id_t &Id);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall new1Click(TObject *Sender);
	void __fastcall delete1Click(TObject *Sender);
	void __fastcall CodeRowsDrawCell(TObject *Sender, int ACol, int ARow,
		  const TRect &Rect, TGridDrawState State);
	void __fastcall FormKeyPress(TObject *Sender, char &Key);
	void __fastcall colors1Click(TObject *Sender);
	void __fastcall CodeRowsDataToCell(TObject *Sender, int meta,
		  AnsiString &data);
	void __fastcall CodeRowsCellToData(TObject *Sender, int meta,
		  AnsiString &cell);
	void __fastcall CodeRowsBeforeReadRow(TObject *Sender,
          QueryPtr &ResultSet, id_t Id, bool &Compare);
	void __fastcall CodeRowsStoreRow(TObject *Sender, id_t Id);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall modify1Click(TObject *Sender);
	void __fastcall CodeRowsAfterReadRow(TObject *Sender, id_t Id,
          bool Compare);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall print1Click(TObject *Sender);
	void __fastcall export1Click(TObject *Sender);
	void __fastcall selectAll1Click(TObject *Sender);
	void __fastcall CodeRowsPrintCell(TObject *Sender, long ACol, long ARow,
          TRect &Rect);
	void __fastcall CodeRowsDblClick(TObject *Sender);
private:	// User declarations
	Codes codes;
	void __fastcall autosave(void);
	void __fastcall rollback(void);
	int CODE_NAME,APP_CHARGE,CODE_COLOR;
public:		// User declarations
	__fastcall TFcodes1(TComponent* Owner);
	TModalResult __fastcall view(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TFcodes1 *Fcodes1;
//---------------------------------------------------------------------------
#endif
