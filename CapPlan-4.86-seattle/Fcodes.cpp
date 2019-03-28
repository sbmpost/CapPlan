//$Id: Fcodes.cpp,v 1.31 2013/12/21 20:00:16 darseq Exp $
#include <vcl.h>
#pragma hdrstop
#include "Fcodes.h"
#include "Foptions.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "DataRows"
#pragma link "Fbase"
#pragma resource "*.dfm"
TFcodes1 *Fcodes1;
//---------------------------------------------------------------------------
__fastcall TFcodes1::TFcodes1(TComponent* Owner) : TFbase1(Owner) {
	db->link(&codes);
	CODE_NAME = CodeRows->Meta->IndexOf("CODE_NAME");
	APP_CHARGE = CodeRows->Meta->IndexOf("APP_CHARGE");
	CODE_COLOR = CodeRows->Meta->IndexOf("CODE_COLOR");
	CodeRows->Canvas->Font = Font;

	CodeRows->ColWidths[0] = 150;
	CodeRows->ColWidths[1] = 150;
	CodeRows->ColWidths[2] = 150;
}
//---------------------------------------------------------------------------

void __fastcall TFcodes1::FormDestroy(TObject *Sender) {
	db->unlink(&codes);
}
//---------------------------------------------------------------------------

void __fastcall TFcodes1::FormShow(TObject *Sender) {
	ActiveControl = CodeRows;
}
//---------------------------------------------------------------------------

TModalResult __fastcall TFcodes1::view(void) {
	LOGFUNC();
	modified = false;
	CodeRows->refresh(UNDEFINED);
	return ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TFcodes1::FormClose(TObject *Sender, TCloseAction &Action) {
	LOGFUNC();
	TFbase1::FormClose(Sender,Action);
	CodeRows->clearRowState();
}
//---------------------------------------------------------------------------

void __fastcall TFcodes1::autosave(void) {
	if(TFoptions1::auto_save) {save();}
}
//---------------------------------------------------------------------------

void __fastcall TFcodes1::rollback(void) {
	LOGFUNC();
	CodeRows->clearRowState(); // before closing
	TFbase1::rollback(); // closes this form
}
//---------------------------------------------------------------------------

void __fastcall TFcodes1::CodeRowsDataToCell(TObject *Sender, int meta,
	AnsiString &data) {
	if(data != UNDEFINED && meta == APP_CHARGE) {
		data = CurrToStrF(Currency(data),ffFixed,2);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFcodes1::CodeRowsCellToData(TObject *Sender, int meta,
	AnsiString &cell) {
	if(meta == APP_CHARGE) {
		try {cell = CurrToStr(Currency(cell));}
		catch(EConvertError & e) {cell = CurrToStr(Currency("0"));}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFcodes1::CodeRowsBeforeRefresh(TObject *Sender,
	QueryPtr &ResultSet, id_t &Id) {
	LOGFUNC();
	codes.execSQL("select * from codes order by CODE_NAME");
	ResultSet = &codes;
}
//---------------------------------------------------------------------------

void __fastcall TFcodes1::CodeRowsBeforeReadRow(TObject *Sender,
	  QueryPtr &ResultSet, id_t Id, bool &Compare) {
	LOGFUNC(); if(Id == UNDEFINED) {Id = "NULL";}
	if(Compare) {Policy::setULock();} // not used for colors
	AnsiString query = "select * from codes "+Policy::lock+"where "
		"CODE_NAME="+QuotedStr(Id.SubString(0,MAXCODE))+Policy::lock_end;
	if(Compare) {TRYULOCK(codes.execSQL(query););}
	else {codes.execSQL(query);}
	ResultSet = &codes;
}
//---------------------------------------------------------------------------

void __fastcall TFcodes1::CodeRowsAfterReadRow(TObject *Sender,id_t Id,
	bool Compare) {
	LOGFUNC();
	if(Id != UNDEFINED && (CodeRows->RowState == rsEmpty ||
		CodeRows->RowState == rsChanged)) {
		// disabled, also see checkChanged()
		// if(TFoptions1::auto_save) {save();}
		throw UserException(SELECTION_CHANGED);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFcodes1::CodeRowsStoreRow(TObject *Sender, id_t Id) {
	LOGFUNC();
	int row = CodeRows->Row;
	if(Id == UNDEFINED) {CodeRows->clearRow(row); return;}
	Policy::setULock();
	AnsiString update = "update codes "+Policy::lock+"set ";
	if(CodeRows->getData(CODE_NAME,row) == UNDEFINED) {
		CodeRows->setData(Id,CODE_NAME,row);
	}
	update += "CODE_NAME="+QuotedStr(CodeRows->getData(
		CODE_NAME,row).SubString(0,MAXCODE))+",";
	update += "APP_CHARGE="+CodeRows->getData(APP_CHARGE,row)+" ";
	update += "where CODE_NAME="+QuotedStr(Id); // using the old Id
	try {
		TRYULOCK( // no rollback
			codes.execSQL(update);
			CodeRows->readRow(false);
			modified = true;
		); autosave();
	} catch(Exception & e) {
		Exceptions::handleException(this,&e);
		CodeRows->setData(Id,CODE_NAME,row);
		CodeRows->readRow(false);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFcodes1::new1Click(TObject *Sender) {
	LOGFUNC();
	if(CodeRows->editingRow()) {return;}
	bool result;
#if __BORLANDC__ >= 0x582
	Application->ModalPopupMode = pmExplicit;
	try {result = ColorDialog1->Execute(Handle);}
	__finally {Application->ModalPopupMode = pmNone;}
#else
	result = ColorDialog1->Execute();
#endif
	if(result) {
		int color = ColorDialog1->Color & 0x00FFFFFF;
		id_t code_name = IntToHex(color,6);
		TRYULOCK(
			codes.insert(code_name,color);
			modified = true;
		); autosave();
		CodeRows->refresh(code_name);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFcodes1::modify1Click(TObject *Sender) {
	Word Key = VK_F2; KeyDown(Key,TShiftState());
}
//---------------------------------------------------------------------------

void __fastcall TFcodes1::delete1Click(TObject *Sender) {
	LOGFUNC();
	int row = CodeRows->rowSelected();
	if(row == 0 || CodeRows->editingRow()) {return;}
	id_t code_name = CodeRows->getId(row);
	if(code_name == UNDEFINED) {return;}
	AnsiString message = AnsiString::LoadStr(FPENDING_REMOVEITEMQUESTION);
	if(ATTYN(message.c_str()) == ID_NO) {return;}
	TRYULOCK(codes.remove(code_name); modified = true;); autosave();
	CodeRows->clearRow(CodeRows->Row); CodeRows->refreshNext();
}
//---------------------------------------------------------------------------

void __fastcall TFcodes1::CodeRowsDblClick(TObject *Sender) {
	colors1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFcodes1::colors1Click(TObject *Sender) {
	LOGFUNC();
	int row = CodeRows->rowSelected();
	if(row == 0 || CodeRows->editingRow()) {return;}
	id_t code_name = CodeRows->getId(row);
	if(code_name == UNDEFINED) {return;}
	int color_old = StrToInt(CodeRows->getData(CODE_COLOR,row));
	ColorDialog1->Color = TColor(color_old);
	TStrings * old_strings = ColorDialog1->CustomColors;
	TStringList * new_strings = new TStringList();
	try {
		new_strings->Add("ColorA="+IntToHex(color_old,8));
		ColorDialog1->CustomColors = new_strings;
		bool result;
#if __BORLANDC__ >= 0x582
		Application->ModalPopupMode = pmExplicit;
		try {result = ColorDialog1->Execute(Handle);}
		__finally {Application->ModalPopupMode = pmNone;}
#else
		result = ColorDialog1->Execute();
#endif
		if(result) {
			int color_new = ColorDialog1->Color & 0x00FFFFFF;
			if(color_new != color_old) {
				AnsiString color = IntToStr(color_new);
				TRYULOCK(
					codes.execSQL("update codes "+Policy::lock+
						"set CODE_COLOR="+color+" where CODE_NAME="+
						QuotedStr(code_name));
					CodeRows->setData(color,CODE_COLOR,row);
					modified = true;
				);
				TRYSLOCK(
					codes.execSQL("update apps "+Policy::lock+
						"set COLOR="+color+" where COLOR="+
						IntToStr(color_old));
				); autosave();
				refreshForms(); // apps data has changed
				// If TFoptions1::o.insert_color == color_old, we
				// we can't update the configuration file because
				// each client has its own file and insert_color.
			}
		}
	} __finally {delete new_strings; ColorDialog1->CustomColors = old_strings;}
}
//---------------------------------------------------------------------------

void __fastcall TFcodes1::selectAll1Click(TObject *Sender) {
	ActiveControl = CodeRows;
	Word Key = 'a'; KeyDown(Key,TShiftState() << ssCtrl);
}
//---------------------------------------------------------------------------

void __fastcall TFcodes1::FormKeyPress(TObject *Sender, char &Key) {
	if(CodeRows->editingRow()) {
		if(Key == VK_RETURN) {return;}
		else if(Key == VK_ESCAPE) {CodeRows->storeRow(); Key = '\0';}
	}
	TFbase1::FormKeyPress(Sender,Key);
}
//---------------------------------------------------------------------------

void __fastcall TFcodes1::FormKeyDown(TObject *Sender, WORD &Key,
	TShiftState Shift) {
	if(Key == VK_F2 && CodeRows->rowSelected() &&
		CodeRows->editingRow() == false) {
		ActiveControl = CodeRows;
		CodeRows->editRow();
		Key = '\0';
	} else if(tolower(Key) == 'a' && Shift.Contains(ssCtrl) &&
		ActiveControl == CodeRows && CodeRows->selectAll()) {
		Key = '\0';
	}
}
//---------------------------------------------------------------------------

void __fastcall TFcodes1::CodeRowsDrawCell(TObject *Sender, int ACol,
	int ARow, const TRect &Rect, TGridDrawState State) {
	CodeRows->setRowColor(State);
	TCanvas * canvas = CodeRows->Canvas;
	AnsiString text = CodeRows->Cells[ACol][ARow]; int left = 0;
	if(ACol == APP_CHARGE) {left = Rect.Width()-10-canvas->TextWidth(text);}
	else if(ARow && ACol == CODE_COLOR) {
		if(text.Length()) {
			CodeRows->setRowColor(State,clBtnFace,TColor(StrToInt(text)));
			text = "";
		}
	}
	if(ARow == CodeRows->Row &&
		(int) CodeRows->Meta->Objects[ACol] & METACHANGED) {
		CodeRows->setRowColor(State,clBtnFace,TColor(CONFLICTCOLOR));
	}
	int top = (Rect.Height()-canvas->TextHeight(text))/2;
	canvas->TextRect(Rect,Rect.left+left,Rect.Top+top,text);
}
//---------------------------------------------------------------------------

void __fastcall TFcodes1::CodeRowsPrintCell(TObject *Sender,long ACol,
	long ARow,TRect &Rect) {
	if(ACol == APP_CHARGE) {
		TCanvas * canvas = Printer()->Canvas;
		AnsiString text = CodeRows->Cells[ACol][ARow];
		Rect.left += Rect.Width()-50-canvas->TextWidth(text);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFcodes1::print1Click(TObject *Sender) {
	LOGFUNC();
	resetPrinter(); Enabled = false;
	try {
		try {CodeRows->printSelection();}
		catch(Exception & e) {Application->ShowException(&e);}
	} __finally {Enabled = true; SetForegroundWindow(Handle);}
}
//---------------------------------------------------------------------------

void __fastcall TFcodes1::export1Click(TObject *Sender) {
	TFbase1::exportCsvFile(CodeRows); // LOGFUNC by TFbase
}
//---------------------------------------------------------------------------

