//$Id: Fentries.cpp,v 1.34 2013/02/16 11:41:37 darseq Exp $
#include <vcl.h>
#pragma hdrstop
#include "Fentries.h"
#include "Foptions.h"
#include "Farchive.h"
#include "Fmain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Fmdetail"
#pragma link "RowControl"
#pragma link "DataRows"
#pragma resource "*.dfm"
TFentries1 *Fentries1;
//---------------------------------------------------------------------------
__fastcall TFentries1::TFentries1(TComponent* Owner) : TFmdetail1(Owner) {
	db->link(&entries); DateLabel = Caption;
	RowControl->Width = ClientWidth - RowControl->Left + 1;
	RowControl->Height = ClientHeight - RowControl->Top + 1;
	setMetaIndices();

	CurrentRows->ColWidths[0] = 200;
	CurrentRows->ColWidths[1] = 252;
	CurrentRows->ColWidths[2] = 521;

	StartingRows->ColWidths[0] = 200;
	StartingRows->ColWidths[1] = 252;
	StartingRows->ColWidths[2] = 521;

	EndingRows->ColWidths[0] = 200;
	EndingRows->ColWidths[1] = 252;
	EndingRows->ColWidths[2] = 521;
}
//---------------------------------------------------------------------------

void __fastcall TFentries1::FormCreate(TObject *Sender) {
	setMenu(WorkMenu,active,2);
	setMenu(WorkMenu,starting,2);
	setMenu(WorkMenu,ending,2);
}
//---------------------------------------------------------------------------

void __fastcall TFentries1::FormDestroy(TObject *Sender) {
	db->unlink(&entries);
}
//---------------------------------------------------------------------------

void __fastcall TFentries1::DataRowsDblClick(TObject *Sender) {
	archive1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFentries1::autosave(void) {
	if(TFoptions1::auto_save) {save();}
}
//---------------------------------------------------------------------------

void __fastcall TFentries1::archive1Click(TObject *Sender) {
	LOGFUNC();
	ActiveControl = ActiveRows; // keep focus on rows for key search.
	autosave(); // release locks if possible
	int C = 40; int top = Farchive1->Top; int left = Farchive1->Left;
	if(Top+Farchive1->Height+C<=Screen->WorkAreaHeight) {Farchive1->Top=Top+C;}
	if(Left+Farchive1->Width+C<=Screen->WorkAreaWidth) {Farchive1->Left=Left+C;}
	try {
		if(rowSelected()) {
			Fmain1->per_id = getData(PERSONAL_ID);
			Fmain1->app_id = ids[Page] = getId();
			// Fmain1->doc_id = UNDEFINED;
			Fmain1->view_archive(Farchive1->APPS);
		} else {Fmain1->archive1Click(Sender);}
	} __finally {Farchive1->Top = top; Farchive1->Left = left;}
}
//---------------------------------------------------------------------------

TModalResult __fastcall TFentries1::view(TTabSheet * show_sheet) {
	LOGFUNC();
	AnsiString short_date(ShortDateFormat);
	ShortDateFormat = LongDateFormat;
	try {Caption = DateLabel+": "+DateToStr(current_date);}
	__finally {ShortDateFormat = short_date;}
	return TFmdetail1::view(show_sheet);
}
//---------------------------------------------------------------------------

void __fastcall TFentries1::DataRowsDataToCell(TObject *Sender, int meta,
	AnsiString &data) {if(meta == CANCELLED && data == UNDEFINED) {data = "1";}}
//---------------------------------------------------------------------------

void __fastcall TFentries1::DataRowsCellToData(TObject *Sender, int meta,
	AnsiString &cell) {if(meta == CANCELLED && cell == UNDEFINED) {cell = "0";}}
//---------------------------------------------------------------------------

void __fastcall TFentries1::DataRowsBeforeRefresh(TObject *Sender,
	QueryPtr &ResultSet, id_t &Id) {
	LOGFUNC();
	AnsiString f1;
	AnsiString begin(YMD(current_date));
	AnsiString end(YMD(current_date+1));
	if(ActiveRows == CurrentRows) {
		f1 = "DATE_FROM<" DS+end+DS " and DATE_UNTIL>" DS+begin+DS " and ";
	}
	id_t grid_id = grid.id; if(grid_id == UNDEFINED) {grid_id = "NULL";}
	AnsiString select = "select apps.APP_ID,CANCELLED,personal.PERSONAL_ID,"
		"LASTNAME,ADDRESS,INFOTEXT,min(DATE_FROM),max(DATE_UNTIL) from "
		"((entries right join apps on entries.APP_ID=apps.APP_ID) left join "
		"employees on entries.EMPLOYEE_ID=employees.PERSONAL_ID) inner join "
		"personal on (personal.PERSONAL_ID=apps.PERSONAL_ID) where "
		"(entries.EMPLOYEE_ID is null or employees.SCHEDULE_ID="+
		grid_id+") and "+f1+"personal.PERSONAL_ID=apps.PERSONAL_ID group by "
		"apps.APP_ID,CANCELLED,personal.PERSONAL_ID,LASTNAME,ADDRESS,INFOTEXT";
	if(ActiveRows == StartingRows) {
		select += " having min(DATE_FROM)>=" DS+
			begin+DS " and min(DATE_FROM)<" DS+end+DS;
	} else if(ActiveRows == EndingRows) {
		select += " having max(DATE_UNTIL)>=" DS+
			begin+DS " and max(DATE_UNTIL)<" DS+end+DS;
	}
	select += " order by LASTNAME";
	entries.execSQL(select);
	ResultSet = &entries;
}
//---------------------------------------------------------------------------

void __fastcall TFentries1::selectAll1Click(TObject *Sender) {
	ActiveControl = ActiveRows;
	Word Key = 'a'; KeyDown(Key,TShiftState() << ssCtrl);
}
//---------------------------------------------------------------------------

void __fastcall TFentries1::refresh1Click(TObject *Sender) {
	refreshRows(getId());
}
//---------------------------------------------------------------------------

void __fastcall TFentries1::DataRowsDrawCell(TObject *Sender, int ACol,
	int ARow, const TRect &Rect, TGridDrawState State) {
	TDataRows * rows = ActiveRows;
	rows->setRowColor(State); TCanvas * canvas = rows->Canvas;
	AnsiString text = rows->Cells[ACol][ARow]; int left = 0;
	if(ARow && StrToInt(rows->getData(CANCELLED,ARow))) {
		rows->setRowColor(State,clBtnFace,clWhite,
			clBlack,TFontStyles() << fsStrikeOut);
	}
	int top = (Rect.Height()-canvas->TextHeight(text))/2;
	canvas->TextRect(Rect,Rect.left+left,Rect.Top+top,text);
}
//---------------------------------------------------------------------------

void __fastcall TFentries1::DataRowsNewPage(TObject *Sender, TRect &Rect) {
	TFmdetail1::PrintNewPage(Rect); // LOGFUNC by TFmdetail
}
//---------------------------------------------------------------------------

void __fastcall TFentries1::print1Click(TObject *Sender) {
	TFmdetail1::PrintSelection(); // LOGFUNC by TFmdetail
}
//---------------------------------------------------------------------------

void __fastcall TFentries1::export1Click(TObject *Sender) {
	TFbase1::exportCsvFile(ActiveRows); // LOGFUNC by TFbase
}
//---------------------------------------------------------------------------

void __fastcall TFentries1::setMetaIndices(void) {
	APP_ID = StartingRows->Meta->IndexOf("APP_ID");
	CANCELLED = StartingRows->Meta->IndexOf("CANCELLED");
	PERSONAL_ID = StartingRows->Meta->IndexOf("PERSONAL_ID");
}
//---------------------------------------------------------------------------

