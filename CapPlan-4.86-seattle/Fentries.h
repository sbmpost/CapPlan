//$Id: Fentries.h,v 1.14 2013/02/16 11:41:37 darseq Exp $
#ifndef FentriesH
#define FentriesH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <Menus.hpp>
#include "Fmdetail.h"
#include "DataRows.h"
#include "RowControl.h"
//---------------------------------------------------------------------------

class TFentries1 : public TFmdetail1
{
__published:	// IDE-managed Components
	TTabSheet *STARTING;
	TDataRows *StartingRows;
	TMainMenu *WorkMenu;
	TMenuItem *archive1;
	TMenuItem *print1;
	TTabSheet *ENDING;
	TDataRows *EndingRows;
	TTabSheet *ACTIVE;
	TDataRows *CurrentRows;
	TPopupMenu *PopupMenu1;
	TMenuItem *selectAll2;
	TMenuItem *list1;
	TMenuItem *selectAll1;
	TMenuItem *popupPrint1;
	TMenuItem *export1;
	TMenuItem *popupExport1;
	TMenuItem *N1;
	TMenuItem *refresh1;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall DataRowsDrawCell(TObject *Sender, int ACol, int ARow,
		  const TRect &Rect, TGridDrawState State);
	void __fastcall archive1Click(TObject *Sender);
	void __fastcall DataRowsDblClick(TObject *Sender);
	void __fastcall DataRowsDataToCell(TObject *Sender, int meta,
		  AnsiString &data);
	void __fastcall DataRowsCellToData(TObject *Sender, int meta,
		  AnsiString &cell);
	void __fastcall print1Click(TObject *Sender);
	void __fastcall DataRowsBeforeRefresh(TObject *Sender, QueryPtr &ResultSet,
		  id_t &Id);
	void __fastcall DataRowsNewPage(TObject *Sender, TRect &Rect);
	void __fastcall selectAll1Click(TObject *Sender);
	void __fastcall export1Click(TObject *Sender);
	void __fastcall refresh1Click(TObject *Sender);
private:	// User declarations
	enum {active=0,starting,ending,PAGECOUNT};
	Entries entries; AnsiString DateLabel;
	void __fastcall autosave(void);
	void __fastcall setMetaIndices(void);
	int APP_ID,CANCELLED,PERSONAL_ID;
public:		// User declarations
	__fastcall TFentries1(TComponent* Owner);
	TModalResult __fastcall view(TTabSheet * show_sheet);
};
//---------------------------------------------------------------------------
extern PACKAGE TFentries1 *Fentries1;
//---------------------------------------------------------------------------
#endif

