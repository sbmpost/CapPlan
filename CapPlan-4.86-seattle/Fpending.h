//$Id: Fpending.h,v 1.85 2013/03/19 20:39:13 darseq Exp $
#ifndef FpendingH
#define FpendingH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <Menus.hpp>
#include "Fbase.h"
#include "Fmdetail.h"
#include "Tracks.h"
#include "DataRows.h"
#include "RowControl.h"
//---------------------------------------------------------------------------

class TFpending1 : public TFmdetail1
{
__published:	// IDE-managed Components
	TTabSheet *REPURCHASE;
	TDataRows *PurchaseRows;
	TTabSheet *SCHEDULE;
	TDataRows *ScheduleRows;
	TMainMenu *RepurchaseMenu;
	TMenuItem *repurchaseOrdered1;
	TMenuItem *repurchaseArchive1;
	TMainMenu *ScheduleMenu;
	TMenuItem *scheduleArchive1;
	TMenuItem *scheduleDocument1;
	TMenuItem *scheduleComplete1;
	TMenuItem *repurchaseRemove1;
	TTabSheet *INVOICE;
	TDataRows *InvoiceRows;
	TMainMenu *InvoiceMenu;
	TMenuItem *invoiceArchive1;
	TMenuItem *invoiceRemove1;
	TMenuItem *print2;
	TMenuItem *print3;
	TMenuItem *print4;
	TMenuItem *scheduleRemove1;
	TTabSheet *REVIEW;
	TDataRows *ReviewRows;
	TMainMenu *ReviewMenu;
	TMenuItem *reviewArchive1;
	TMenuItem *reviewSchedule1;
	TMenuItem *print1;
	TMenuItem *reviewRejected1;
	TMenuItem *macro4;
	TMenuItem *invoiceCharged1;
	TMenuItem *macro3;
	TMenuItem *macro2;
	TMenuItem *macro1;
	TPopupMenu *PopupMenu1;
	TMenuItem *popupRejected1;
	TMenuItem *popupSelectAll1;
	TMenuItem *popupSchedule1;
	TPopupMenu *PopupMenu2;
	TMenuItem *popupSelectAll2;
	TPopupMenu *PopupMenu3;
	TMenuItem *popupOrdered1;
	TMenuItem *popupSelectAll3;
	TPopupMenu *PopupMenu4;
	TMenuItem *popupCharged1;
	TMenuItem *popupSelectAll4;
	TMenuItem *reviewList1;
	TMenuItem *scheduleList1;
	TMenuItem *repurchaseList1;
	TMenuItem *invoiceList1;
	TMenuItem *popupPlanned1;
	TMenuItem *selectAll1;
	TMenuItem *SelectAll2;
	TMenuItem *selectAll3;
	TMenuItem *selectAll4;
	TMenuItem *popupMacro1;
	TMenuItem *popupMacro2;
	TMenuItem *popupPrint1;
	TMenuItem *popupPrint2;
	TMenuItem *popupPrint3;
	TMenuItem *popupPrint4;
	TMenuItem *popupMacro3;
	TMenuItem *popupMacro4;
	TMenuItem *N1;
	TMenuItem *N2;
	TMenuItem *N3;
	TMenuItem *N4;
	TMenuItem *popupRemove1;
	TMenuItem *popupRemove2;
	TMenuItem *popupRemove3;
	TMenuItem *open1;
	TMenuItem *open2;
	TMenuItem *open3;
	TMenuItem *open4;
	TMenuItem *N5;
	TMenuItem *comment1;
	TMenuItem *N6;
	TMenuItem *comment2;
	TMenuItem *N7;
	TMenuItem *comment3;
	TMenuItem *N8;
	TMenuItem *comment4;
	TMenuItem *scheduleRejected1;
	TMenuItem *popupRejected2;
	TMenuItem *popupSchedule2;
	TButton *ScheduleButton;
	TMenuItem *popupReset1;
	TMenuItem *popupReset2;
	TMenuItem *popupReset3;
	TMenuItem *popupReset4;
	TMenuItem *popupOpen1;
	TMenuItem *popupOpen2;
	TMenuItem *popupOpen3;
	TMenuItem *popupOpen4;
	TMenuItem *message1;
	TMenuItem *message2;
	TMenuItem *message3;
	TMenuItem *message4;
	TMenuItem *export1;
	TMenuItem *export2;
	TMenuItem *export3;
	TMenuItem *export4;
	TMenuItem *popupExport1;
	TMenuItem *ExportCsv1;
	TMenuItem *popupExport3;
	TMenuItem *popupExport4;
	TMenuItem *refresh1;
	TMenuItem *refresh2;
	TMenuItem *refresh3;
	TMenuItem *refresh4;
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall PurchaseRowsBeforeRefresh(TObject *Sender,
		  QueryPtr &ResultSet, id_t &Id);
	void __fastcall DataRowsDrawCell(TObject *Sender, int ACol, int ARow,
		  const TRect &Rect, TGridDrawState State);
	void __fastcall ScheduleRowsBeforeRefresh(TObject *Sender,
		  QueryPtr &ResultSet, id_t &Id);
	void __fastcall archive1Click(TObject *Sender);
	void __fastcall repurchaseOrdered1Click(TObject *Sender);
	void __fastcall scheduleComplete1Click(TObject *Sender);
	void __fastcall DataRowsCellToData(TObject *Sender, int meta,
		  AnsiString &cell);
	void __fastcall repurchaseRemove1Click(TObject *Sender);
	void __fastcall InvoiceRowsBeforeRefresh(TObject *Sender,
		  QueryPtr &ResultSet, id_t &Id);
	void __fastcall invoiceRemove1Click(TObject *Sender);
	void __fastcall printClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall DataRowsDblClick(TObject *Sender);
	void __fastcall DataRowsNewPage(TObject *Sender, TRect &Rect);
	void __fastcall ScheduleRowsBeforeReadRow(TObject *Sender,
		  QueryPtr &ResultSet, id_t Id, bool &Compare);
	void __fastcall PurchaseRowsBeforeReadRow(TObject *Sender,
		  QueryPtr &ResultSet, id_t Id, bool &Compare);
	void __fastcall InvoiceRowsBeforeReadRow(TObject *Sender,
		  QueryPtr &ResultSet, id_t Id, bool &Compare);
	void __fastcall scheduleRemove1Click(TObject *Sender);
	void __fastcall ReviewRowsBeforeReadRow(TObject *Sender,
		  QueryPtr &ResultSet, id_t Id, bool &Compare);
	void __fastcall ReviewRowsBeforeRefresh(TObject *Sender,
		  QueryPtr &ResultSet, id_t &Id);
	void __fastcall reviewSchedule1Click(TObject *Sender);
	void __fastcall reviewRejected1Click(TObject *Sender);
	void __fastcall invoiceCharged1Click(TObject *Sender);
	void __fastcall DataRowsAfterRefresh(TObject *Sender, int &SelectRow,
		  bool IsIdRow);
	void __fastcall macro1Click(TObject *Sender);
	void __fastcall selectAll1Click(TObject *Sender);
	void __fastcall open1Click(TObject *Sender);
	void __fastcall FormKeyPress(TObject *Sender, char &Key);
	void __fastcall modify1Click(TObject *Sender);
	void __fastcall DataRowsMouseDown(TObject *Sender,
		  TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall DataRowsSelectCell(TObject *Sender, int ACol,
		  int ARow, bool &CanSelect);
	void __fastcall DataRowsAfterReadRow(TObject *Sender, id_t Id,
		  bool Compare);
	void __fastcall REVIEWShow(TObject *Sender);
	void __fastcall SCHEDULEShow(TObject *Sender);
	void __fastcall REPURCHASEShow(TObject *Sender);
	void __fastcall INVOICEShow(TObject *Sender);
	void __fastcall scheduleDocument1Click(TObject *Sender);
	void __fastcall popupReset1Click(TObject *Sender);
	void __fastcall popupReset2Click(TObject *Sender);
	void __fastcall popupReset3Click(TObject *Sender);
	void __fastcall popupReset4Click(TObject *Sender);
	void __fastcall DataRowsContextPopup(TObject *Sender,
		  const TPoint &MousePos, bool &Handled);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormContextPopup(TObject *Sender, TPoint &MousePos,
		  bool &Handled);
	void __fastcall sendMail1Click(TObject *Sender);
	void __fastcall export1Click(TObject *Sender);
	void __fastcall ReviewRowsStoreRow(TObject *Sender, id_t Id);
	void __fastcall ScheduleRowsStoreRow(TObject *Sender, id_t Id);
	void __fastcall PurchaseRowsStoreRow(TObject *Sender, id_t Id);
	void __fastcall InvoiceRowsStoreRow(TObject *Sender, id_t Id);
	void __fastcall DataRowsGetEditText(TObject *Sender, int ACol,
          int ARow, UnicodeString &Value);
	void __fastcall refresh1Click(TObject *Sender);
private:	// User declarations
	enum {rev=0,doc,pur,inv,PAGECOUNT};
	Docs docs; TColor clfix;
	bool rev_visible,sch_visible;
	bool pur_visible,inv_visible;
	void __fastcall autosave(void);
	void __fastcall setDocData(Document & document);
	void __fastcall setBefore(id_t Id,AnsiString before);
	void __fastcall setHandled(AnsiString handled);
	void __fastcall resetHandled(AnsiString handled);
	void __fastcall removeItems(AnsiString pending);
	void __fastcall setMetaIndices(void);
	int DOC_NAME,LASTNAME,COMMENT,HANDLE_BEFORE,HANDLED_DATE;
	int LUPDATE_UTC,SIGNED,URGENT,PRIORITY,MODIFIED,AVAILABLE;
	int DOC_PLANNED_DATE,DOC_REJECTED_DATE,DOC_ORDERED_DATE;
	int DOC_CHARGED_DATE,PENDING_REPURCHASE,PENDING_INVOICE,DOC_DATA;
public:		// User declarations
	THintWindow * hint_window;
	__fastcall TFpending1(TComponent* Owner);
	TModalResult __fastcall view(TTabSheet * show_sheet,id_t & document_id);
};
//---------------------------------------------------------------------------
extern PACKAGE TFpending1 *Fpending1;
//---------------------------------------------------------------------------
#endif

