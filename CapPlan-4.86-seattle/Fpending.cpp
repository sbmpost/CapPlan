//$Id: Fpending.cpp,v 1.164 2013/12/21 20:00:16 darseq Exp $
#include <vcl.h>
#pragma hdrstop
#include <strutils.hpp>
#include "Foptions.h"
#include "Fpending.h"
#include "Farchive.h"
#include "Fmain.h"
#include "Fcomplete.h"
#include "Fschedule.h"
#include "Femployee.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Fbase"
#pragma link "Fmdetail"
#pragma link "RowControl"
#pragma link "DataRows"
#pragma resource "*.dfm"
TFpending1 *Fpending1;
//---------------------------------------------------------------------------
__fastcall TFpending1::TFpending1(TComponent* Owner) : TFmdetail1(Owner) {
	db->link(&docs);
	RowControl->Width = ClientWidth - RowControl->Left + 1;
	RowControl->Height = ClientHeight - RowControl->Top + 1;
	AnsiString update = "update docs set ";
	AnsiString where = "where PERSONAL_ID=NULL";
	try {db_query.execSQL(update+"REJECTED_DATE=REJECTED_DATE "+where);}
	catch(DBException & e) {REVIEW->TabVisible = false;}
	try {db_query.execSQL(update+"PLANNED_DATE=PLANNED_DATE "+where);}
	catch(DBException & e) {SCHEDULE->TabVisible = false;}
	try {db_query.execSQL(update+"ORDERED_DATE=ORDERED_DATE "+where);}
	catch(DBException & e) {REPURCHASE->TabVisible = false;}
	try {db_query.execSQL(update+"CHARGED_DATE=CHARGED_DATE "+where);}
	catch(DBException & e) {INVOICE->TabVisible = false;}
	save(); // release the acquired locks on the docs table.
	rev_visible = REVIEW->TabVisible; sch_visible = SCHEDULE->TabVisible;
	pur_visible = REPURCHASE->TabVisible; inv_visible = INVOICE->TabVisible;
	Fmain1->MainMenu1->Items->Items[Fmain1->pending1->MenuIndex]->Enabled =
		rev_visible || sch_visible || pur_visible || inv_visible;
	setMetaIndices();

	ReviewRows->ColWidths[0] = 200;
	ReviewRows->ColWidths[1] = 252;
	ReviewRows->ColWidths[2] = 258;
	ReviewRows->ColWidths[3] = 90;
	ReviewRows->ColWidths[4] = 171;

	ScheduleRows->ColWidths[0] = 200;
	ScheduleRows->ColWidths[1] = 252;
	ScheduleRows->ColWidths[2] = 258;
	ScheduleRows->ColWidths[3] = 90;
	ScheduleRows->ColWidths[4] = 171;

	PurchaseRows->ColWidths[0] = 200;
	PurchaseRows->ColWidths[1] = 252;
	PurchaseRows->ColWidths[2] = 258;
	PurchaseRows->ColWidths[3] = 90;
	PurchaseRows->ColWidths[4] = 171;

	InvoiceRows->ColWidths[0] = 200;
	InvoiceRows->ColWidths[1] = 252;
	InvoiceRows->ColWidths[2] = 258;
	InvoiceRows->ColWidths[3] = 90;
	InvoiceRows->ColWidths[4] = 171;
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::FormCreate(TObject *Sender) {
	setMenu(ReviewMenu,rev,5); setMenu(ScheduleMenu,doc,6);
	setMenu(RepurchaseMenu,pur,4); setMenu(InvoiceMenu,inv,4);
	hint_window = new THintWindow(this);
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::FormDestroy(TObject *Sender) {
	db->unlink(&docs);
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::DataRowsDblClick(TObject *Sender) {
	open1Click(Sender); // archive1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::autosave(void) {
	if(TFoptions1::auto_save) {save();}
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::archive1Click(TObject *Sender) {
	LOGFUNC();
	if(editingRow()) {return;}
	ActiveControl = ActiveRows; // keep focus on rows for key search.
	autosave(); // release locks if possible.
	int C = 40; int top = Farchive1->Top; int left = Farchive1->Left;
	if(Top+Farchive1->Height+C<=Screen->WorkAreaHeight) {Farchive1->Top=Top+C;}
	if(Left+Farchive1->Width+C<=Screen->WorkAreaWidth) {Farchive1->Left=Left+C;}
	hint_window->ReleaseHandle();
	try {
		if(rowSelected() && GETDOCID(getId())) {
			// filter is reset by view_archive
			Fmain1->per_id = per_id;
			Fmain1->app_id = UNDEFINED;
			Fmain1->doc_id = ids[Page] = getId();
			Fmain1->view_archive(Farchive1->DOCS);
			// refreshRows(ids[Page]); // when revert clicked or updated
			// [disabled: performance, new documents should be added to
			// the pending list via the mainscreen -> archive]
			readRow(false); // compromise, only update the current row
		} else {Fmain1->archive1Click(Sender);/*refreshRows(ids[Page]);*/}
	} __finally {Farchive1->Top = top; Farchive1->Left = left;}
	// ActiveControl = ActiveRows; disabled: it is actually useful to
	// scroll the list while having the focus on the archive button.
}
//---------------------------------------------------------------------------

TModalResult __fastcall TFpending1::view(TTabSheet * show_sheet,
	id_t & document_id) {
	LOGFUNC();
	if(Fschedule1->Visible == false) {
		// currently Fschedule1 is never visible
		int page_index = TFoptions1::o.page_nr-1;
		if(page_index == -1) { // show all pages
			REVIEW->TabVisible = rev_visible &&
				TFoptions1::review_tab;
			SCHEDULE->TabVisible = sch_visible;
			REPURCHASE->TabVisible = pur_visible;
			INVOICE->TabVisible = inv_visible;
		} else { // show single page only
			REVIEW->TabVisible = rev_visible &&
				TFoptions1::review_tab &&
				REVIEW->PageIndex == page_index;
			SCHEDULE->TabVisible = sch_visible &&
				SCHEDULE->PageIndex == page_index;
			REPURCHASE->TabVisible = pur_visible &&
				REPURCHASE->PageIndex == page_index;;
			INVOICE->TabVisible = inv_visible &&
				INVOICE->PageIndex == page_index;
		}
	}
	scheduleRejected1->Visible = rev_visible &&
		TFoptions1::review_tab == false;
	popupRejected2->Visible = scheduleRejected1->Visible;
	if(Fschedule1->Visible) {ids[doc] = document_id;}
	TModalResult result;
	try {result = TFmdetail1::view(show_sheet);}
	__finally {hint_window->ReleaseHandle();}
	document_id = ids[doc];
	return result;
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::DataRowsCellToData(TObject *Sender, int meta,
	AnsiString &cell) {
	if(meta == DOC_NAME) {cell = QuotedStr(cell);}
	else if(meta == SIGNED && cell == UNDEFINED) {cell = "0";}
	else if(meta == URGENT && cell == UNDEFINED) {cell = "0";}
	else if(meta == PRIORITY && cell == UNDEFINED) {cell = "0";}
	else if(meta == MODIFIED && cell == UNDEFINED) {cell = "0";}
	else if(meta == AVAILABLE && cell == UNDEFINED) {cell = "0";}
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::ReviewRowsBeforeRefresh(TObject *Sender,
	  QueryPtr &ResultSet, id_t &Id) {
	LOGFUNC();
	docs.execSQL("select docs.PERSONAL_ID,DOC_NAME,LASTNAME,ADDRESS,"
		"COMMENT,REVIEW_BEFORE,REJECTED_DATE,LAST_UPDATE_UTC,SIGNED,URGENT,"
		IFELSE("REJECTED_DATE is NULL and REVIEW_BEFORE <= " DS+YMD(Date())+
		DS,"1","0")" as priority," IFELSE("REJECTED_DATE < LAST_UPDATE","1",
		"0")" as modified,STATETEXT,DOC_DATA from docs,personal where "
		"docs.PERSONAL_ID=personal.PERSONAL_ID and PENDING_REVIEW=1 "
		"order by REVIEW_BEFORE,docs.CREATED_DATE desc",ROWSET_SIZE);
	ResultSet = &docs;
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::ScheduleRowsBeforeRefresh(TObject *Sender,
	QueryPtr &ResultSet, id_t &Id) {
	LOGFUNC();
	// This query ignores the hidden value of a person
	docs.execSQL("select docs.PERSONAL_ID,DOC_NAME,LASTNAME,ADDRESS,"
		"COMMENT,PLAN_BEFORE,PLANNED_DATE,LAST_UPDATE_UTC,SIGNED,URGENT,"
		IFELSE("PLANNED_DATE is NULL and PLAN_BEFORE <= " DS+YMD(Date())+
		DS,"1","0")" as priority," IFELSE("PLANNED_DATE < LAST_UPDATE or "
		"REJECTED_DATE < LAST_UPDATE","1","0")" as modified,REJECTED_DATE,"
		"ORDERED_DATE,CHARGED_DATE,PENDING_REPURCHASE,PENDING_INVOICE,"
		"STATETEXT,DOC_DATA from docs,personal where docs.PERSONAL_ID="
		"personal.PERSONAL_ID and PENDING_SCHEDULE=1 order by PLAN_BEFORE,"
		"docs.CREATED_DATE desc",ROWSET_SIZE);
	ResultSet = &docs;
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::PurchaseRowsBeforeRefresh(TObject *Sender,
	QueryPtr &ResultSet, id_t &Id) {
	LOGFUNC();
	docs.execSQL("select docs.PERSONAL_ID,DOC_NAME,LASTNAME,ADDRESS,"
		"COMMENT,ORDER_BEFORE,ORDERED_DATE,LAST_UPDATE_UTC,SIGNED,URGENT,"
		IFELSE("ORDERED_DATE is NULL and ORDER_BEFORE <= " DS+YMD(Date())+
		DS,"1","0")" as priority," IFELSE("ORDERED_DATE < LAST_UPDATE","1",
		"0")" as modified,STATETEXT,DOC_DATA from docs,personal where "
		"docs.PERSONAL_ID=personal.PERSONAL_ID and PENDING_REPURCHASE=1 "
		"order by ORDER_BEFORE,docs.CREATED_DATE desc",ROWSET_SIZE);
	ResultSet = &docs;
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::InvoiceRowsBeforeRefresh(TObject *Sender,
	QueryPtr &ResultSet, id_t &Id) {
	LOGFUNC();
	docs.execSQL("select docs.PERSONAL_ID,DOC_NAME,LASTNAME,ADDRESS,"
		"COMMENT,CHARGE_BEFORE,CHARGED_DATE,LAST_UPDATE_UTC,SIGNED,URGENT,"
		IFELSE("CHARGED_DATE is NULL and CHARGE_BEFORE <= " DS+YMD(Date())+
		DS,"1","0")" as priority," IFELSE("CHARGED_DATE < LAST_UPDATE","1",
		"0")" as modified,STATETEXT,DOC_DATA from docs,personal where "
		"docs.PERSONAL_ID=personal.PERSONAL_ID and PENDING_INVOICE=1 "
		"order by CHARGE_BEFORE,docs.CREATED_DATE desc",ROWSET_SIZE);
	ResultSet = &docs;
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::DataRowsAfterRefresh(TObject *Sender,
	int &SelectRow, bool IsIdRow) {
	LOGFUNC();
	TDataRows * rows = ActiveRows;
	int col = rows->Meta->IndexOf("DOC_DATA");
	AnsiString doc_path(TFoptions1::o.documents_path);
	for(int row=1;row != rows->RowCount;row++) { // rowcount >= 1
		rows->setData(IntToStr((int) FileExists(doc_path+"\\"+
			rows->getData(col,row))),AVAILABLE,row);
	}
	hint_window->ReleaseHandle();
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::ReviewRowsBeforeReadRow(TObject *Sender,
	  QueryPtr &ResultSet, id_t Id, bool &Compare) {
	LOGFUNC(); if(GETDOCID(Id) == false) {per_id = "NULL";}
	if(Compare) {Policy::setULock();}
	AnsiString query = "select docs.PERSONAL_ID,DOC_NAME,COMMENT,"
		"REVIEW_BEFORE,REJECTED_DATE,LAST_UPDATE_UTC,SIGNED,URGENT,"
		IFELSE("REJECTED_DATE is NULL and REVIEW_BEFORE <= " DS+YMD(Date())+
		DS,"1","0")" as priority," IFELSE("REJECTED_DATE < LAST_UPDATE","1",
		"0")" as modified,STATETEXT,DOC_DATA from docs "+Policy::lock+
		"where PENDING_REVIEW=1 and docs.PERSONAL_ID="+per_id+
		" and DOC_NAME="+QuotedStr(doc_name)+Policy::lock_end;
	if(Compare) {TRYULOCK(docs.execSQL(query););}
	else {docs.execSQL(query);}
	ResultSet = &docs;
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::ScheduleRowsBeforeReadRow(TObject *Sender,
	  QueryPtr &ResultSet, id_t Id, bool &Compare) {
	LOGFUNC(); if(GETDOCID(Id) == false) {per_id = "NULL";}
	if(Compare) {Policy::setULock();}
	AnsiString query = "select docs.PERSONAL_ID,DOC_NAME,COMMENT,"
		"PLAN_BEFORE,PLANNED_DATE,LAST_UPDATE_UTC,SIGNED,URGENT,"
		IFELSE("PLANNED_DATE is NULL and PLAN_BEFORE <= " DS+YMD(Date())+
		DS,"1","0")" as priority," IFELSE("PLANNED_DATE < LAST_UPDATE or "
		"REJECTED_DATE < LAST_UPDATE","1","0")" as modified,"
		"REJECTED_DATE,ORDERED_DATE,CHARGED_DATE,PENDING_REPURCHASE,"
		"PENDING_INVOICE,STATETEXT,DOC_DATA from docs "+Policy::lock+
		"where PENDING_SCHEDULE=1 and docs.PERSONAL_ID="+per_id+
		" and DOC_NAME="+QuotedStr(doc_name)+Policy::lock_end;
	if(Compare) {TRYULOCK(docs.execSQL(query););}
	else {docs.execSQL(query);}
	ResultSet = &docs;
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::PurchaseRowsBeforeReadRow(TObject *Sender,
	  QueryPtr &ResultSet, id_t Id, bool &Compare) {
	LOGFUNC(); if(GETDOCID(Id) == false) {per_id = "NULL";}
	if(Compare) {Policy::setULock();}
	AnsiString query = "select docs.PERSONAL_ID,DOC_NAME,COMMENT,"
		"ORDER_BEFORE,ORDERED_DATE,LAST_UPDATE_UTC,SIGNED,URGENT,"
		IFELSE("ORDERED_DATE is NULL and ORDER_BEFORE <= " DS+YMD(Date())+
		DS,"1","0")" as priority," IFELSE("ORDERED_DATE < LAST_UPDATE","1",
		"0")" as modified,STATETEXT,DOC_DATA from docs "+Policy::lock+
		"where PENDING_REPURCHASE=1 and docs.PERSONAL_ID="+per_id+
		" and DOC_NAME="+QuotedStr(doc_name)+Policy::lock_end;
	if(Compare) {TRYULOCK(docs.execSQL(query););}
	else {docs.execSQL(query);}
	ResultSet = &docs;
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::InvoiceRowsBeforeReadRow(TObject *Sender,
	QueryPtr &ResultSet, id_t Id, bool &Compare) {
	LOGFUNC(); if(GETDOCID(Id) == false) {per_id = "NULL";}
	if(Compare) {Policy::setULock();}
	AnsiString query = "select docs.PERSONAL_ID,DOC_NAME,COMMENT,"
		"CHARGE_BEFORE,CHARGED_DATE,LAST_UPDATE_UTC,SIGNED,URGENT,"
		IFELSE("CHARGED_DATE is NULL and CHARGE_BEFORE <= " DS+YMD(Date())+
		DS,"1","0")" as priority," IFELSE("CHARGED_DATE < LAST_UPDATE","1",
		"0")" as modified,STATETEXT,DOC_DATA from docs "+Policy::lock+
		"where PENDING_INVOICE=1 and docs.PERSONAL_ID="+per_id+
		" and DOC_NAME="+QuotedStr(doc_name)+Policy::lock_end;
	if(Compare) {TRYULOCK(docs.execSQL(query););}
	else {docs.execSQL(query);}
	ResultSet = &docs;
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::DataRowsAfterReadRow(TObject *Sender, id_t Id,
	bool Compare) {
	LOGFUNC();
	if(Id != UNDEFINED) {checkChanged();}
	hint_window->ReleaseHandle();
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::DataRowsGetEditText(TObject *Sender,
	int ACol, int ARow, UnicodeString &Value) {
	if(ACol == HANDLE_BEFORE) {
		AnsiString handle_before(getData(HANDLE_BEFORE));
		if(handle_before.IsEmpty()) {
			unsigned short year,month,day;
			Date().DecodeDate(&year,&month,&day);
			handle_before = IntToStr(year)+DateSeparator;
		} Value = handle_before;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::setBefore(id_t Id,AnsiString before) {
	if(GETDOCID(Id) == false) {clearRow(); return;}
	AnsiString handle_before(getData(HANDLE_BEFORE));
	try {handle_before = DS+YMD(TDateTime(handle_before))+DS;}
	catch (EConvertError & e) {handle_before = "NULL";}
	AnsiString comment = getData(COMMENT).SubString(0,MAXCOMMENT);
	TRYSLOCK(
		docs.execSQL("update docs "+Policy::lock+"set COMMENT="+
			QuotedStr(comment)+","+before+"="+handle_before+
			" where PERSONAL_ID="+per_id+" and DOC_NAME="+
			QuotedStr(doc_name));
		readRow(false);
		modified = true;
	); autosave(); // release locks as soon as possible.
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::ReviewRowsStoreRow(TObject *Sender, id_t Id) {
	LOGFUNC(); setBefore(Id,"REVIEW_BEFORE");
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::ScheduleRowsStoreRow(TObject *Sender, id_t Id) {
	LOGFUNC(); setBefore(Id,"PLAN_BEFORE");
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::PurchaseRowsStoreRow(TObject *Sender, id_t Id) {
	LOGFUNC(); setBefore(Id,"ORDER_BEFORE");
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::InvoiceRowsStoreRow(TObject *Sender, id_t Id) {
	LOGFUNC(); setBefore(Id,"CHARGE_BEFORE");
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::reviewSchedule1Click(TObject *Sender) {
	LOGFUNC();
	ActiveControl = ReviewRows;
	if(rowSelected() == false || editingRow() ||
		GETDOCID(getId()) == false) {return;}
	AnsiString message = AnsiString::LoadStr(FPENDING_REALLYMOVETOSCHEDULE);
	if(MSGYN(message.c_str()) == ID_NO) {return;}
	readRow(true); // Inform the user when someone else rejected it.
	TRYSLOCK( // note that readRow also checks PENDING_REVIEW
		// If the document was first rejected, rejected_date must now be
		// reset so it doesn't show up as rejected in the schedule list.
		// Note that if the document is not planned but only removed from
		// the schedule list, it reappears in the review list if edited.
		docs.execSQL("update docs "+Policy::lock+"set REJECTED_DATE=NULL,"
			"PENDING_REVIEW=0,PENDING_SCHEDULE=1 where PERSONAL_ID="+
			per_id+" and DOC_NAME="+QuotedStr(doc_name));
		// clearRow() follows
		modified = true;
	); autosave();
	clearRow(); refreshNext();
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::reviewRejected1Click(TObject *Sender) {
	LOGFUNC();
	if(rowSelected() == false || editingRow() ||
		GETDOCID(getId()) == false) {return;}
	AnsiString message = AnsiString::LoadStr(FPENDING_REALLYMARKREJECTED);
	if(MSGYN(message.c_str()) == ID_NO) {return;}
	readRow(true); // Inform the user when someone else approved it.
	TRYSLOCK( // note that readRow also checks PENDING_REVIEW
		// if last_update < planned_date, the user should not be able to
		// reject. However to prevent confusion, just reset its state.
		docs.execSQL("update docs "+Policy::lock+"set PLANNED_DATE=NULL,"
			"REJECTED_DATE=" CURRENT_TIMESTAMP " where PERSONAL_ID="+
			per_id+" and DOC_NAME="+QuotedStr(doc_name));
		readRow(false); // get current_timestamp
		modified = true;
	); autosave();
	sendMail1Click(NULL);
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::setDocData(Document & document) {
	// These fields are checked/locked by readrow. The other
	// fields do not have to be set because restore = false.
	document.per_id = per_id;
	document.name = doc_name;
	document.lupdate_utc = getData(LUPDATE_UTC);
	document.is_signed = StrToInt(getData(SIGNED));
	document.is_urgent = StrToInt(getData(URGENT));
	document.schedule = true; document.planned_date.SetLength(1);
	// If restore = false and planned_date is set, Docs::update
	// will not update PENDING_REVIEW and PENDING_SCHEDULE.
	document.data = getData(ActiveRows->Meta->IndexOf("DOC_DATA"));
	document.initialize(false,TFoptions1::review_tab);
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::open1Click(TObject *Sender) {
	LOGFUNC(); hint_window->ReleaseHandle(); openDocument(docs);
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::scheduleComplete1Click(TObject *Sender) {
	LOGFUNC();
	if(rowSelected() == false || editingRow() ||
		GETDOCID(getId()) == false) {return;}

	bool planned = getData(DOC_PLANNED_DATE).Length();
	bool repurchase = StrToInt(getData(PENDING_REPURCHASE));
	bool ordered = repurchase || getData(DOC_ORDERED_DATE).Length();
	bool invoice = StrToInt(getData(PENDING_INVOICE));
	bool charged = invoice || getData(DOC_CHARGED_DATE).Length();
	AnsiString doc_path(TFoptions1::o.documents_path);
	doc_path = doc_path+"\\"+getData(DOC_DATA);
	TDateTime lutc = TDateTime(getData(LUPDATE_UTC));

	// To undo the repurchase/invoice check boxes, the items
	// should be removed from their respective pending lists.
	// After the items are ordered/charged, it is inconsistent
	// to undo these check boxes so even root cannot do this.
	Fcomplete1->RemoveItemBox->Checked = true;
	Fcomplete1->RepurchaseBox->Checked = ordered ||
		(!planned && TFoptions1::o.repurchase);
	Fcomplete1->RepurchaseBox->Enabled = !ordered;
	Fcomplete1->InvoiceBox->Checked = charged ||
		(!planned && TFoptions1::o.invoice);
	Fcomplete1->InvoiceBox->Enabled = !charged;
	if(Fcomplete1->ShowModal() == mrIgnore) {return;}

	bool schedule = !Fcomplete1->RemoveItemBox->Checked;
	// only make items pending if they weren't handled earlier
	if(!ordered) {repurchase = Fcomplete1->RepurchaseBox->Checked;}
	if(!charged) {invoice = Fcomplete1->InvoiceBox->Checked;}

	readRow(true); // get exclusive access and compare.
	TRYSLOCK( // note that readRow also checks PENDING_SCHEDULE
		AnsiString update = "update docs "+Policy::lock+"set ";
		update += "PENDING_SCHEDULE="+IntToStr((int) schedule)+",";
		update += "PENDING_REPURCHASE="+IntToStr((int) repurchase)+",";
		update += "PENDING_INVOICE="+IntToStr((int) invoice)+",";
		update += "REJECTED_DATE=NULL,PLANNED_DATE="
			CURRENT_TIMESTAMP" where PERSONAL_ID="+
			per_id+" and DOC_NAME="+QuotedStr(doc_name);
		docs.execSQL(update);
		if(schedule) {readRow(false);}
		else {clearRow(); refreshNext();}
		modified = true;
	); autosave();
	if(!ordered && repurchase) {macroExecute(doc_path,lutc,"ToRepurchase");}
	if(!charged && invoice) {macroExecute(doc_path,lutc,"ToInvoice");}
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::setHandled(AnsiString handled) {
	LOGFUNC();
	TDataRows * rows = ActiveRows;
	if(rows->Row == 0 || editingRow()) {return;}
	// When multiple rows are selected, rowSelected
	// deselects them. Instead check rows->Row == 0.
	int row = rows->Selection.Top;
	int end = rows->Selection.Bottom+1;
	rows->gotoRow(row);

	AnsiString doc_path(TFoptions1::o.documents_path);
	TGridRect sel = {rows->ColCount-1,row,0,row};
	try { // When a second readRow fails, release previous locks.
		for(rows->Selection = sel;row != end;row++) {
			rows->Row = row;
			if(GETDOCID(getId()) == false) {continue;}
			readRow(true);
			TRYSLOCK(
				docs.execSQL("update docs "+Policy::lock+"set "+
					handled+" where PERSONAL_ID="+per_id+
					" and DOC_NAME="+QuotedStr(doc_name));
				readRow(false);
				modified = true;
			);
		}
	} __finally {autosave();} // release readRow locks
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::repurchaseOrdered1Click(TObject *Sender) {
	LOGFUNC();
	if(rowSelected() == false || editingRow() ||
		GETDOCID(getId()) == false) {return;}
	//AnsiString message = AnsiString::LoadStr(FPENDING_REALLYMARKORDERED);
	//if(MSGYN(message.c_str()) == ID_NO) {return;}
	//readRow(true); // Make sure no one else has already ordered this document.
	//TRYSLOCK( // note that readRow also checks PENDING_REPURCHASE
	//	docs.execSQL("update docs "+Policy::lock+"set ORDERED_DATE="
	//		CURRENT_TIMESTAMP" where PERSONAL_ID="+per_id+
	//		" and DOC_NAME="+QuotedStr(doc_name));
	//	readRow(false);
	//	modified = true;
	//); autosave();
	setHandled("ORDERED_DATE=" CURRENT_TIMESTAMP);
	repurchaseRemove1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::invoiceCharged1Click(TObject *Sender) {
	LOGFUNC();
	if(rowSelected() == false || editingRow() ||
		GETDOCID(getId()) == false) {return;}
	//AnsiString message = AnsiString::LoadStr(FPENDING_REALLYMARKCHARGED);
	//if(MSGYN(message.c_str()) == ID_NO) {return;}
	//readRow(true); // Make sure no one else has already charged this document.
	//TRYSLOCK( // note that readRow also checks PENDING_INVOICE
	//	docs.execSQL("update docs "+Policy::lock+"set CHARGED_DATE="
	//		CURRENT_TIMESTAMP" where PERSONAL_ID="+per_id+
	//		" and DOC_NAME="+QuotedStr(doc_name));
	//	readRow(false);
	//	modified = true;
	//); autosave();
	setHandled("CHARGED_DATE=" CURRENT_TIMESTAMP);
	invoiceRemove1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::resetHandled(AnsiString handled) {
	if(rowSelected() == false || editingRow() ||
		GETDOCID(getId()) == false) {return;}
	AnsiString message = AnsiString::LoadStr(FPENDING_REALLYRESETSTATE);
	if(ATTYN(message.c_str()) == ID_NO) {return;}
	readRow(true); // Inform the user when someone else marked it.
	TRYSLOCK(
		docs.execSQL("update docs "+Policy::lock+"set "+
			handled+" where PERSONAL_ID="+per_id+
			" and DOC_NAME="+QuotedStr(doc_name));
		readRow(false); // reset date/charged value
		modified = true;
	); autosave();
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::popupReset1Click(TObject *Sender) {
	LOGFUNC(); resetHandled("REJECTED_DATE=NULL");
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::popupReset2Click(TObject *Sender) {
	LOGFUNC(); resetHandled("PLANNED_DATE=NULL,REJECTED_DATE=NULL");
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::popupReset3Click(TObject *Sender) {
	LOGFUNC(); resetHandled("ORDERED_DATE=NULL");
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::popupReset4Click(TObject *Sender) {
	LOGFUNC(); resetHandled("CHARGED_DATE=NULL");
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::FormContextPopup(TObject *Sender,
	TPoint &MousePos, bool &Handled) {
	// cannot add this code to Fmdetails because the Archive
	// contains edit fields which display their own context.
	ActiveRows->DoContextPopup(MousePos,Handled);
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::DataRowsContextPopup(TObject *Sender,
	const TPoint &MousePos, bool &Handled) {
	TDataRows * rows = ActiveRows;
	if(rows->editingRow() == false) {
		ReviewRows->PopupMenu = PopupMenu1;
		ScheduleRows->PopupMenu = PopupMenu2;
		PurchaseRows->PopupMenu = PopupMenu3;
		InvoiceRows->PopupMenu = PopupMenu4;
	} else {rows->PopupMenu = NULL;} // enable standard menu
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::macro1Click(TObject *Sender) {LOGFUNC();}
//---------------------------------------------------------------------------

void __fastcall TFpending1::removeItems(AnsiString pending) {
	TDataRows * rows = ActiveRows;
	if(rows->Row == 0 || editingRow()) {return;}
	int row = rows->Selection.Top;
	int end = rows->Selection.Bottom+1;
	int num = end-row;
	rows->gotoRow(row);

	AnsiString message;
	if(num > 1) {
		message = AnsiString::LoadStr(FPENDING_NUMBEROFITEMSREMOVE)+
			IntToStr(num)+", "+AnsiString::LoadStr(CONTINUEQUESTION);
	} else {message = AnsiString::LoadStr(FPENDING_REMOVEITEMQUESTION);}
	if(MSGYN(message.c_str()) == ID_NO) {return;}

	bool cleared = false;
	int result = num > 1 ? 0 : ID_YES;
	TGridRect sel = {rows->ColCount-1,row,0,row};
	try { // When a second readRow fails, release previous locks.
		for(rows->Selection = sel;row != end;row++) {
			rows->Row = row;
			if(GETDOCID(getId()) == false) {continue;}
			if(result != ID_YES) { // 0 or ID_NO
				if(getData(HANDLED_DATE).IsEmpty()) {
					if(result == 0) {
						result = ATTYNC(AnsiString::LoadStr(
							FPENDING_REMOVEUNHANDLEDITEMS).c_str());
						if(result == ID_CANCEL) {break;}
						else if(result == ID_NO) {continue;}
					} else {continue;} // ID_NO
				}
			}
			readRow(true);
			// Before removing, get exclusive access and compare. When the
			// user has just completed scheduling, it is important to know
			// that the matching appointment was cancelled before removing.
			TRYSLOCK(
				docs.execSQL("update docs "+Policy::lock+"set "+
					pending+"=0 where PERSONAL_ID="+per_id+
					" and DOC_NAME="+QuotedStr(doc_name));
			);
			rows->clearRow(row);
			modified = cleared = true;
		}
	} __finally {autosave();}
	// If an exception occurred on an item, do
	// not move the selection to the next item.
	if(cleared) {refreshNext();} // clearRow in loop
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::scheduleRemove1Click(TObject *Sender) {
	LOGFUNC(); removeItems("PENDING_SCHEDULE");
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::repurchaseRemove1Click(TObject *Sender) {
	LOGFUNC(); removeItems("PENDING_REPURCHASE");
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::invoiceRemove1Click(TObject *Sender) {
	LOGFUNC(); removeItems("PENDING_INVOICE");
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::selectAll1Click(TObject *Sender) {
	ActiveControl = ActiveRows;
	Word Key = 'a'; KeyDown(Key,TShiftState() << ssCtrl);
	hint_window->ReleaseHandle();
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::REVIEWShow(TObject *Sender) {
	LOGFUNC();
	clfix = clBtnFace;
	ScheduleButton->Visible = true;
	ScheduleButton->OnClick = reviewSchedule1Click;
	TFmdetail1::PageShow(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::SCHEDULEShow(TObject *Sender) {
	LOGFUNC();
	clfix = TColor(0xb0e8a0);
	ScheduleButton->Visible = true;
	ScheduleButton->OnClick = scheduleDocument1Click;
	TFmdetail1::PageShow(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::REPURCHASEShow(TObject *Sender) {
	LOGFUNC();
	clfix = TColor(0x90f0f0);
	ScheduleButton->Visible = false;
	TFmdetail1::PageShow(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::INVOICEShow(TObject *Sender) {
	LOGFUNC();
	clfix = TColor(0xf0b8e8);
	ScheduleButton->Visible = false;
	TFmdetail1::PageShow(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::FormClose(TObject *Sender,TCloseAction &Action) {
	TFmdetail1::FormClose(Sender,Action);
	// TFmdetail sets ModalResult = mrOk for VK_RETURN key.
	if(ActiveRows == ScheduleRows && ModalResult == mrOk) {
		scheduleDocument1Click(Sender);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::scheduleDocument1Click(TObject *Sender) {
	LOGFUNC();
	if(ScheduleRows->Row == 0 || editingRow()) {return;}
	id_t doc_id = getId(); if(GETDOCID(doc_id) == false) {return;}
	Fschedule1->setDocument(doc_id,ScheduleRows->Row);
	ModalResult = mrAll; // signal all forms must be closed
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::modify1Click(TObject *Sender) {
	Word Key = VK_F2; KeyDown(Key,TShiftState());
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::sendMail1Click(TObject *Sender) {
	LOGFUNC();
	TDataRows * rows = ActiveRows;
	if(rows->Row == 0 || rows->editingRow() ||
		GETDOCID(getId()) == false) {return;}
	AnsiString label,to;
	docs.execSQL("select CREATED_BY from docs where PERSONAL_ID="+
		per_id+" and DOC_NAME="+QuotedStr(doc_name));
	if(docs.next()) {to = docs.fieldByName("CREATED_BY").toString();}
	if(to.IsEmpty()) {to = "recipient";} // created_by could be empty
	if(Sender == NULL) {
		label = ReviewMenu->Items->Items[
			reviewRejected1->MenuIndex]->Caption;
		label = AnsiReplaceStr(label,"&","");
	} else {label = RowControl->ActivePage->Caption;}
	sendMail(to,"",doc_name+" ("+label+")","");
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::refresh1Click(TObject *Sender) {
	if(editingRow() == false) {refreshRows(getId());}
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::FormKeyPress(TObject *Sender, char &Key) {
	if(Key == VK_ESCAPE && editingRow()) {storeRow(); Key = '\0';}
	TFmdetail1::FormKeyPress(Sender,Key);
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::DataRowsSelectCell(TObject *Sender,
	int ACol,int ARow,bool &CanSelect) {
	if(ARow != ActiveRows->Row) {hint_window->ReleaseHandle();}
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::DataRowsMouseDown(TObject *Sender,
	TMouseButton Button, TShiftState Shift, int X, int Y) {
	if(Button == mbRight) {hint_window->ReleaseHandle(); return;}
	if(Shift.Contains(ssDouble)) {return;}
	TDataRows * rows = ActiveRows; if(rows->Row == 0 || editingRow()) {return;}
	if(GETDOCID(getId()) == false) {hint_window->ReleaseHandle(); return;}
	static int PRow = rows->Row;
	if(PRow == rows->Row && IsWindowVisible(hint_window->Handle)) {
		hint_window->ReleaseHandle(); return;
	} PRow = rows->Row;

	AnsiString hint = getData(rows->Meta->IndexOf("STATETEXT"));
	if(hint.Length()) {
		hint_window->Color = TColor(0xC0FFFF);
		TRect rect = hint_window->CalcHintRect(500,hint,NULL);
		TPoint point(X+30,Y+10); point = rows->ClientToScreen(point);
		rect.left = rect.left+point.x; rect.right = rect.right+point.x;
		rect.top = rect.top+point.y; rect.bottom = rect.bottom+point.y;
		hint_window->ReleaseHandle(); hint_window->ActivateHint(rect,hint);
		// Remove the topmost flag because the hint_window should not be on
		// top of other programs. It is not safe to keep the topmost flag,
		// and to release the window, before switching programs because our
		// own windows conflict with the hint_window as well. We cannot set
		// the stay on top flag of our own (modal) windows either because
		// they would interfere with the other programs. A downside of not
		// having the topmost flag is that the hint window is shown behind
		// the windows taskbar which apparently has its topmost flag set.
		SetWindowPos(hint_window->Handle,HWND_NOTOPMOST,0,0,0,0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::DataRowsDrawCell(TObject *Sender, int ACol,
	int ARow, const TRect &Rect, TGridDrawState State) {
	TDataRows * rows = ActiveRows;
	TCanvas * canvas = rows->Canvas;
	canvas->Font = Font; rows->setRowColor(State,clfix);
	AnsiString text = rows->Cells[ACol][ARow];
	int left = 0; TColor color(clBlack);
	if(ACol == LASTNAME) {
		if(TFoptions1::o.unsigned_red && ARow &&
			StrToInt(rows->getData(SIGNED,ARow)) == false) {
			color = TColor(CONFLICTCOLOR);
			rows->setRowColor(State,clfix,clWhite,color);
		}
	} else if(ACol == HANDLE_BEFORE && ARow) {
		if(StrToInt(rows->getData(PRIORITY,ARow))) {
			color = TColor(CONFLICTCOLOR);
			rows->setRowColor(State,clfix,clWhite,color);
		} else if(StrToInt(rows->getData(URGENT,ARow)) &&
			rows->getData(HANDLE_BEFORE,ARow).IsEmpty()) {
			text = AnsiString::LoadStr(FPENDING_URGENT);
			left = (Rect.Width()-canvas->TextWidth(text))/2;
		}
	} else if(ACol == HANDLED_DATE && ARow &&
		StrToInt(rows->getData(MODIFIED,ARow))) {
		color = TColor(CONFLICTCOLOR);
		rows->setRowColor(State,clfix,clWhite,color);
	}

	if(ARow) {
		TFontStyles style;
		if(rows->getData(HANDLED_DATE,ARow).IsEmpty() && (rows !=
			ScheduleRows || rows->getData(DOC_REJECTED_DATE,ARow).IsEmpty())) {
			canvas->Font = Femployee1->Font; // Font is scaled by windows (dpi)
			rows->setRowColor(State,clfix,clWhite,color,style << fsBold);
		}
		if(StrToInt(rows->getData(AVAILABLE,ARow)) == false) {
			rows->setRowColor(State,clfix,clWhite,clGrayText,style);
		}
	}

	if(ARow == rows->Row &&
		(int) rows->Meta->Objects[ACol] & METACHANGED) {
		rows->setRowColor(State,clfix,TColor(CONFLICTCOLOR));
	}
	int top = (Rect.Height()-canvas->TextHeight(text))/2;
	canvas->TextRect(Rect,Rect.left+left,Rect.Top+top,text);
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::DataRowsNewPage(TObject *Sender,TRect &Rect) {
	TFmdetail1::PrintNewPage(Rect); // LOGFUNC by TFmdetail
}
//---------------------------------------------------------------------------

// todo: print bold records?
void __fastcall TFpending1::printClick(TObject *Sender) {
	TFmdetail1::PrintSelection(); // LOGFUNC by TFmdetail
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::export1Click(TObject *Sender) {
	TFbase1::exportCsvFile(ActiveRows); // LOGFUNC by TFbase
}
//---------------------------------------------------------------------------

void __fastcall TFpending1::setMetaIndices(void) {
	DOC_NAME = ReviewRows->Meta->IndexOf("DOC_NAME");
	LASTNAME = ReviewRows->Meta->IndexOf("LASTNAME");
	COMMENT = ReviewRows->Meta->IndexOf("COMMENT");
	HANDLE_BEFORE = ReviewRows->Meta->IndexOf("REVIEW_BEFORE");
	HANDLED_DATE = ReviewRows->Meta->IndexOf("REJECTED_DATE");
	LUPDATE_UTC = ReviewRows->Meta->IndexOf("LAST_UPDATE_UTC");
	SIGNED = ReviewRows->Meta->IndexOf("SIGNED");
	URGENT = ReviewRows->Meta->IndexOf("URGENT");
	PRIORITY = ReviewRows->Meta->IndexOf("priority");
	MODIFIED = ReviewRows->Meta->IndexOf("modified");
	AVAILABLE = ReviewRows->Meta->IndexOf("available");

	DOC_PLANNED_DATE = HANDLED_DATE;
	DOC_REJECTED_DATE = ScheduleRows->Meta->IndexOf("REJECTED_DATE");
	DOC_ORDERED_DATE = ScheduleRows->Meta->IndexOf("ORDERED_DATE");
	DOC_CHARGED_DATE = ScheduleRows->Meta->IndexOf("CHARGED_DATE");
	PENDING_REPURCHASE = ScheduleRows->Meta->IndexOf("PENDING_REPURCHASE");
	PENDING_INVOICE = ScheduleRows->Meta->IndexOf("PENDING_INVOICE");
	DOC_DATA = ScheduleRows->Meta->IndexOf("DOC_DATA");
}
//---------------------------------------------------------------------------

