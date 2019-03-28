//$Id: Fcreated.h,v 1.14 2013/02/16 11:41:37 darseq Exp $
#ifndef FcreatedH
#define FcreatedH
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

class TFcreated1 : public TFmdetail1
{
__published:	// IDE-managed Components
	TTabSheet *APPS;
	TDataRows *AppRows;
	TTabSheet *DOCUMENTS;
	TMainMenu *DocumentMenu;
	TMenuItem *archive2;
	TMenuItem *list2;
	TMenuItem *selectAll2;
	TMenuItem *print2;
	TDataRows *DocumentRows;
	TPopupMenu *PopupMenu1;
	TMenuItem *popupSelectAll1;
	TMenuItem *popupPrint1;
	TMenuItem *macro1;
	TMainMenu *AppointmentMenu;
	TMenuItem *archive1;
	TMenuItem *list1;
	TMenuItem *selectAll1;
	TMenuItem *print1;
	TPopupMenu *PopupMenu2;
	TMenuItem *popupSelectAll2;
	TMenuItem *popupMacro1;
	TMenuItem *popupPrint2;
	TMenuItem *export1;
	TMenuItem *export2;
	TMenuItem *popupExport1;
	TMenuItem *popupExport2;
	TMenuItem *refresh1;
	TMenuItem *refresh2;
	TMenuItem *N1;
	TMenuItem *N2;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall DataRowsDblClick(TObject *Sender);
	void __fastcall archive1Click(TObject *Sender);
	void __fastcall AppRowsDataToCell(TObject *Sender, int meta,
		  AnsiString &data);
	void __fastcall DocumentRowsDataToCell(TObject *Sender, int meta,
		  AnsiString &data);
	void __fastcall DocumentRowsCellToData(TObject *Sender, int meta,
		  AnsiString &cell);
	void __fastcall AppRowsCellToData(TObject *Sender, int meta,
		  AnsiString &dcell);
	void __fastcall AppRowsBeforeRefresh(TObject *Sender,
		  QueryPtr &ResultSet, id_t &Id);
	void __fastcall DocumentRowsAfterRefresh(TObject *Sender, int &SelectRow,
		  bool IsIdRow);
	void __fastcall DataRowsDrawCell(TObject *Sender, int ACol, int ARow,
		  const TRect &Rect, TGridDrawState State);
	void __fastcall DataRowsPrintCell(TObject *Sender, long ACol,
		  long ARow, TRect &Rect);
	void __fastcall DataRowsNewPage(TObject *Sender, TRect &Rect);
	void __fastcall print1Click(TObject *Sender);
	void __fastcall selectAll1Click(TObject *Sender);
	void __fastcall DocumentRowsBeforeRefresh(TObject *Sender,
		  QueryPtr &ResultSet, id_t &Id);
	void __fastcall DocumentRowsBeforeReadRow(TObject *Sender,
          QueryPtr &ResultSet, id_t Id, bool &Compare);
	void __fastcall macro1Click(TObject *Sender);
	void __fastcall export1Click(TObject *Sender);
	void __fastcall refresh1Click(TObject *Sender);
private:	// User declarations
	enum {app=0,doc,PAGECOUNT};
	Docs docs; AnsiString DateLabel;
	void __fastcall autosave(void);
	void __fastcall setMetaIndices(void);
	int PAID,CREATED_DATE,CANCELLED;
	int DOC_NAME,SCHEDULE,PERSONAL_ID;
	int LUPDATE_UTC,AVAILABLE,DOC_DATA;
public:		// User declarations
	__fastcall TFcreated1(TComponent* Owner);
	TModalResult __fastcall view(TTabSheet * show_sheet);
};
//---------------------------------------------------------------------------
extern PACKAGE TFcreated1 *Fcreated1;
//---------------------------------------------------------------------------
#endif

