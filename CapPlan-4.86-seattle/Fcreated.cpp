//$Id: Fcreated.cpp,v 1.42 2013/02/16 12:18:36 darseq Exp $
#include <vcl.h>
#pragma hdrstop
#include "Foptions.h"
#include "Fcreated.h"
#include "Farchive.h"
#include "Fmain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Fmdetail"
#pragma link "RowControl"
#pragma link "DataRows"
#pragma resource "*.dfm"
TFcreated1 *Fcreated1;
//---------------------------------------------------------------------------
__fastcall TFcreated1::TFcreated1(TComponent* Owner) : TFmdetail1(Owner) {
	db->link(&docs); DateLabel = Caption;
	RowControl->Width = ClientWidth - RowControl->Left + 1;
	RowControl->Height = ClientHeight - RowControl->Top + 1;
	setMetaIndices();

	AppRows->ColWidths[0] = 200;
	AppRows->ColWidths[1] = 252;
	AppRows->ColWidths[2] = 258;
	AppRows->ColWidths[3] = 90;
	AppRows->ColWidths[4] = 171;

	DocumentRows->ColWidths[0] = 200;
	DocumentRows->ColWidths[1] = 252;
	DocumentRows->ColWidths[2] = 258;
	DocumentRows->ColWidths[3] = 90;
	DocumentRows->ColWidths[4] = 171;
}
//---------------------------------------------------------------------------

void __fastcall TFcreated1::FormCreate(TObject *Sender) {
	setMenu(AppointmentMenu,app,2);
	setMenu(DocumentMenu,doc,2);
}
//---------------------------------------------------------------------------

void __fastcall TFcreated1::FormDestroy(TObject *Sender) {
	db->unlink(&docs);
}
//---------------------------------------------------------------------------

void __fastcall TFcreated1::DataRowsDblClick(TObject *Sender) {
	archive1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFcreated1::autosave(void) {
	if(TFoptions1::auto_save) {save();}
}
//---------------------------------------------------------------------------

void __fastcall TFcreated1::archive1Click(TObject *Sender) {
	LOGFUNC();
	ActiveControl = ActiveRows; // keep focus on rows for key search.
	autosave(); // release locks if possible.
	int C = 40; int top = Farchive1->Top; int left = Farchive1->Left;
	if(Top+Farchive1->Height+C<=Screen->WorkAreaHeight) {Farchive1->Top=Top+C;}
	if(Left+Farchive1->Width+C<=Screen->WorkAreaWidth) {Farchive1->Left=Left+C;}
	try {
		if(rowSelected()) {
			id_t person_id = getData(PERSONAL_ID);
			if(ActiveRows == AppRows) {
				Fmain1->per_id = person_id;
				Fmain1->app_id = ids[Page] = getId();
				// Fmain1->doc_id = UNDEFINED;
				Fmain1->view_archive(Farchive1->APPS);
				// no beforeReadRow available
			} else if(person_id != UNDEFINED) {
				Fmain1->per_id = person_id;
				Fmain1->app_id = UNDEFINED;
				Fmain1->doc_id = ids[Page] = getId();
				Fmain1->view_archive(Farchive1->DOCS);
				readRow(false);
			}
		} else {Fmain1->archive1Click(Sender);}
		// do not refreshRows because it would take very long
	} __finally {Farchive1->Top = top; Farchive1->Left = left;}
}
//---------------------------------------------------------------------------

TModalResult __fastcall TFcreated1::view(TTabSheet * show_sheet) {
	LOGFUNC();
	AnsiString short_date(ShortDateFormat);
	ShortDateFormat = LongDateFormat;
	try {Caption = DateLabel+" >= "+DateToStr(
		current_date-CREATEDDAYS);}
	__finally {ShortDateFormat = short_date;}
	return TFmdetail1::view(show_sheet);
}
//---------------------------------------------------------------------------

void __fastcall TFcreated1::AppRowsDataToCell(TObject *Sender,
	int meta, AnsiString &data) {
	if(data == UNDEFINED) {
		if(meta == CANCELLED) {data = "1";}
	} else if(meta == PAID) {
		if(StrToInt(data)) {data = AnsiString::LoadStr(FARCHIVE_Y);}
		else {data = AnsiString::LoadStr(FARCHIVE_N);}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFcreated1::DocumentRowsDataToCell(TObject *Sender,
	int meta, AnsiString &data) {
	if(meta == SCHEDULE) {
		if(StrToInt(data)) {data = AnsiString::LoadStr(FARCHIVE_Y);}
		else {data = AnsiString::LoadStr(FARCHIVE_N);}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFcreated1::AppRowsCellToData(TObject *Sender,
	int meta, AnsiString &cell) {
	if(meta == PAID) {
		if(cell == AnsiString::LoadStr(FARCHIVE_Y)) {cell = "1";}
		else {cell = "0";}
	} else if(meta == CANCELLED && cell == UNDEFINED) {cell = "0";}
}
//---------------------------------------------------------------------------

void __fastcall TFcreated1::DocumentRowsCellToData(TObject *Sender,
	int meta, AnsiString &cell) {
	if(meta == SCHEDULE) {
		if(cell == AnsiString::LoadStr(FARCHIVE_Y)) {cell = "1";}
		else {cell = "0";}
	} else if(meta == DOC_NAME) {cell = QuotedStr(cell);}
	else if(meta == AVAILABLE && cell == UNDEFINED) {cell = "0";}
}
//---------------------------------------------------------------------------

void __fastcall TFcreated1::AppRowsBeforeRefresh(TObject *Sender,
	QueryPtr &ResultSet, id_t &Id) {
	LOGFUNC();
	docs.execSQL("select apps.APP_ID,CANCELLED,personal.PERSONAL_ID,"
		"LASTNAME,ADDRESS,INFOTEXT,PAID,apps.CREATED_DATE from apps,personal "
		"where personal.PERSONAL_ID=apps.PERSONAL_ID and apps.CREATED_DATE>="
		DS+YMD(current_date-CREATEDDAYS)+DS" group by apps.APP_ID,"
		"CANCELLED,personal.PERSONAL_ID,LASTNAME,ADDRESS,INFOTEXT,"
		"PAID,apps.CREATED_DATE order by PAID,apps.CREATED_DATE desc",
		ROWSET_SIZE); // group by APP_ID: no duplicates
	ResultSet = &docs;
}
//---------------------------------------------------------------------------

void __fastcall TFcreated1::DocumentRowsBeforeRefresh(TObject *Sender,
	QueryPtr &ResultSet, id_t &Id) {
	LOGFUNC();
	docs.execSQL(
		"select docs.PERSONAL_ID,DOC_NAME,LASTNAME,ADDRESS,COMMENT," IFELSE(
		"PENDING_REVIEW=1 or PENDING_SCHEDULE=1 or not PLANNED_DATE is null",
		"1","0")" as schedule,docs.CREATED_DATE,LAST_UPDATE_UTC,DOC_DATA "
		"from docs,personal where docs.PERSONAL_ID=personal.PERSONAL_ID "
		"and docs.CREATED_DATE>=" DS+YMD(current_date-CREATEDDAYS)+DS
		" order by 6,docs.CREATED_DATE desc",ROWSET_SIZE); // new first
	ResultSet = &docs;
}
//---------------------------------------------------------------------------

void __fastcall TFcreated1::DocumentRowsAfterRefresh(TObject *Sender,
	int &SelectRow, bool IsIdRow) {
	LOGFUNC();
	AnsiString doc_path(TFoptions1::o.documents_path);
	for(int row=1;row != DocumentRows->RowCount;row++) { // rowcount >= 1
		DocumentRows->setData(IntToStr((int) FileExists(doc_path+"\\"+
			DocumentRows->getData(DOC_DATA,row))),AVAILABLE,row);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFcreated1::DocumentRowsBeforeReadRow(TObject *Sender,
	  QueryPtr &ResultSet, id_t Id, bool &Compare) {
	LOGFUNC(); if(GETDOCID(Id) == false) {per_id = "NULL";}
	if(Compare) {Policy::setULock();}
	AnsiString query = "select PERSONAL_ID,DOC_NAME,COMMENT," IFELSE(
		"PENDING_REVIEW=1 or PENDING_SCHEDULE=1 or not PLANNED_DATE is null",
		"1","0")" as schedule,docs.CREATED_DATE,LAST_UPDATE_UTC,DOC_DATA "
		"from docs "+Policy::lock+"where docs.PERSONAL_ID="+per_id+
		" and DOC_NAME="+QuotedStr(doc_name)+Policy::lock_end;
	if(Compare) {TRYULOCK(docs.execSQL(query););}
	else {docs.execSQL(query);}
	ResultSet = &docs;
}
//---------------------------------------------------------------------------

void __fastcall TFcreated1::macro1Click(TObject *Sender) {
	LOGFUNC();
	TDataRows * rows = ActiveRows;
	if(rows->Row == 0) {return;}
	int row = rows->Selection.Top;
	int end = rows->Selection.Bottom+1;
	rows->gotoRow(row);

	AnsiString macro = "CreatedDocument";
	AnsiString message = macro+"(\""+rows->Cells[CREATED_DATE][0]+"\") ";
	message += AnsiString::LoadStr(WILLBEEXECUTED)+" ";
	message += AnsiString::LoadStr(CONTINUEQUESTION);
	if(MSGYN(message.c_str()) == ID_NO) {return;}

	AnsiString doc_path(TFoptions1::o.documents_path);
	TGridRect sel = {rows->ColCount-1,row,0,row};
	try {
		for(rows->Selection = sel;row != end;row++) {
			rows->Row = row;
			readRow(true); // also prevents simultanious execution
			if(macroExecute(doc_path+"\\"+getData(DOC_DATA),
				TDateTime(getData(LUPDATE_UTC)),macro,QuotedStr(
				getData(CREATED_DATE))) == ID_CANCEL) {
				break;
			}
		}
	} __finally {autosave();} // release TRYULOCK locks
}
//---------------------------------------------------------------------------

void __fastcall TFcreated1::selectAll1Click(TObject *Sender) {
	ActiveControl = ActiveRows;
	Word Key = 'a'; KeyDown(Key,TShiftState() << ssCtrl);
}
//---------------------------------------------------------------------------

void __fastcall TFcreated1::refresh1Click(TObject *Sender) {
	refreshRows(getId());
}
//---------------------------------------------------------------------------

void __fastcall TFcreated1::DataRowsDrawCell(TObject *Sender, int ACol,
	int ARow, const TRect &Rect, TGridDrawState State) {
	TDataRows * rows = ActiveRows;
	rows->setRowColor(State); TCanvas * canvas = rows->Canvas;
	AnsiString text = rows->Cells[ACol][ARow]; int left = 0;
	if(ACol == PAID) {left = (Rect.Width()-canvas->TextWidth(text))/2;}
	if(ARow) {
		if(rows == DocumentRows) {
			if(StrToInt(rows->getData(AVAILABLE,ARow)) == false) {
				rows->setRowColor(State,clBtnFace,clWhite,clGrayText);
			}
		} else if(StrToInt(getData(CANCELLED,ARow))) {
			rows->setRowColor(State,clBtnFace,clWhite,
				clBlack,TFontStyles() << fsStrikeOut);
		}
	}
	int top = (Rect.Height()-canvas->TextHeight(text))/2;
	canvas->TextRect(Rect,Rect.left+left,Rect.Top+top,text);
}
//---------------------------------------------------------------------------

void __fastcall TFcreated1::DataRowsPrintCell(TObject *Sender,
	long ACol, long ARow, TRect &Rect) {
	TCanvas * canvas = Printer()->Canvas;
	if(ACol == PAID) {
		// cannot use getCell for ARow = 0 (the label)
		AnsiString text = ActiveRows->Cells[ACol][ARow];
		Rect.left += (Rect.Width()-canvas->TextWidth(text))/2;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFcreated1::DataRowsNewPage(TObject *Sender,TRect &Rect) {
	TFmdetail1::PrintNewPage(Rect); // LOGFUNC by TFmdetail
}
//---------------------------------------------------------------------------

void __fastcall TFcreated1::print1Click(TObject *Sender) {
	TFmdetail1::PrintSelection(); // LOGFUNC by TFmdetail
}
//---------------------------------------------------------------------------

void __fastcall TFcreated1::export1Click(TObject *Sender) {
	TFbase1::exportCsvFile(ActiveRows); // LOGFUNC by TFbase
}
//---------------------------------------------------------------------------

void __fastcall TFcreated1::setMetaIndices(void) {
	PAID = AppRows->Meta->IndexOf("PAID"); SCHEDULE = PAID;
	CREATED_DATE = AppRows->Meta->IndexOf("CREATED_DATE");
	CANCELLED = AppRows->Meta->IndexOf("CANCELLED");

	DOC_NAME = DocumentRows->Meta->IndexOf("DOC_NAME");
	PERSONAL_ID = DocumentRows->Meta->IndexOf("PERSONAL_ID");
	LUPDATE_UTC = DocumentRows->Meta->IndexOf("LAST_UPDATE_UTC");
	AVAILABLE = DocumentRows->Meta->IndexOf("available");
	DOC_DATA = DocumentRows->Meta->IndexOf("DOC_DATA");
}
//---------------------------------------------------------------------------

