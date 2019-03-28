//$Id: Fmdetail.cpp,v 1.59 2013/03/19 20:39:13 darseq Exp $
#include <vcl.h>
#pragma hdrstop
#include "Fmdetail.h"
#include "Fschedule.h"
#include "ThreadModify.h"
#include "Foptions.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Fbase"
#pragma link "DataRows"
#pragma link "RowControl"
#pragma resource "*.dfm"
TFmdetail1 *Fmdetail1;
//---------------------------------------------------------------------------
__fastcall TFmdetail1::TFmdetail1(TComponent* Owner) : TFbase1(Owner) {
	// note that this code assumes that the application
	// does not change the Fmdetail design at runtime.
	for(int i=0;i != RowControl->PageCount;i++) {
		TDataRows * rows = NULL;
		TTabSheet * Sheet = RowControl->Pages[i];
		for(int j=0;j != Sheet->ControlCount;j++) {
			if((rows = dynamic_cast<TDataRows *>(Sheet->Controls[j])) != NULL) {
				if(Sheet->OnShow == NULL) {Sheet->OnShow = PageShow;}
				datarows.push_back(rows);
				ids.push_back(UNDEFINED);
				menus.push_back(NULL);
				break; // always choose the first DataRows component
			}
		}
		PROG_IF(rows == NULL,"TFmdetail1: Encounted TabSheet without DataRows");
	}
	PROG_IF(datarows.empty(),"TFmdetail1: RowControl needs at least one page");
	RowControl->ActivePage = NULL; FPage = -1;
	for(int i=0;i != RowControl->PageCount;i++) {
		DataRows[i]->Canvas->Font = Font;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFmdetail1::FormShow(TObject *Sender) {
	PROG_IF(tabsheet->OnShow == NULL,"TFmdetail1: OnShow cannot be NULL");
	// Call OnShow here. if we would call OnShow before ShowModal (from
	// view), the form may close when an exception is thrown in OnShow.
	if(RowControl->ActivePage == tabsheet) {tabsheet->OnShow(tabsheet);}
	else {RowControl->ActivePage = tabsheet;}
}
//---------------------------------------------------------------------------

TModalResult __fastcall TFmdetail1::view(TTabSheet * show_sheet) {
	modified = false;
	int i = 0;
	if(show_sheet == NULL ||
		show_sheet->TabVisible == false) {
		for(;i != RowControl->PageCount;i++) {
			show_sheet = RowControl->Pages[i];
			if(show_sheet->TabVisible) {break;}
		}
	}
	TModalResult result = mrNone;
	if(i != RowControl->PageCount) {
		tabsheet = show_sheet;
		result = ShowModal();
	}
	return result;
}
//---------------------------------------------------------------------------

void __fastcall TFmdetail1::FormClose(TObject *Sender,TCloseAction &Action) {
	LOGFUNC();
	TFbase1::FormClose(Sender,Action);
	// check ModalResult only after exitFirst
	if(ModalResult != mrAbort) {
		TDataRows * rows = ActiveRows;
		if(rows->rowSelected()) {
			ids[Page] = rows->getId(rows->Row);
		} else {ids[Page] = UNDEFINED;}
	} else {for(int i=0;i != RowControl->PageCount;i++) {ids[i] = UNDEFINED;}}
	for(int i=0;i != RowControl->PageCount;i++) {DataRows[i]->clearRowState();}
	// for(int i=0;i != RowControl->PageCount;i++) {DataRows[i]->clear();}
	// keeping row data now because the Fschedule window needs it.
}
//---------------------------------------------------------------------------

void __fastcall TFmdetail1::PageShow(TObject *Sender) {
	// cannot use ActivePage because this is the page we came from
	FPage = ((TTabSheet *) Sender)->PageIndex; Menu = menus[Page];
	// ActiveControl = ActiveRows; disabled, do not override ActiveControl
	refreshRows(ids[Page]);
}
//---------------------------------------------------------------------------

void __fastcall TFmdetail1::RowControlChanging(TObject *Sender,
	  bool &AllowChange, int NewPage) {
	TDataRows * rows = DataRows[Page];
	if(rows->rowSelected()) {
		ids[Page] = rows->getId(rows->Row);
	} else {ids[Page] = UNDEFINED;}
}
//---------------------------------------------------------------------------

void __fastcall TFmdetail1::CancelButtonClick(TObject *Sender) {
	LOGFUNC();
	if(CancelButton->Enabled) {
		if(modified) {
			if(ATTYN(AnsiString::LoadStr(
				REALLYDISCARDCHANGES).c_str()) == ID_YES) {
				rollback(); // ModalResult becomes mrAbort
			} // else: do nothing
		} else {ModalResult = mrIgnore;}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFmdetail1::autosave(void) {
	if(TFoptions1::auto_save) {
		// Note that we cannot save if modified
		// is false and autosave is disabled
		// because Fmain itself may be modified.
		save(); CancelButton->Enabled = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFmdetail1::rollback(void) {
	LOGFUNC(); // was called by TRYLOCK exception handler
	ActiveRows->clearRowState(); // before closing
	TFbase1::rollback(); // closes the form
}
//---------------------------------------------------------------------------

void __fastcall TFmdetail1::openDocument(Docs & docs) {
	LOGFUNC();
	if(rowSelected() == false || editingRow() ||
		GETDOCID(getId()) == false) {return;}

	TFbase1 * form = showPanel(FMDETAIL_OPENINGDOCUMENT);
	try {
		ThreadModify * thread = new ThreadModify(db);
		// The thread has been added to the threads list,
		// Resume must be called so WaitFor can complete.
		try {
			setDocData(thread->doc);
			db->unlink(&docs);
			Odbc * base_db = db;
			db = thread->thread_db;
			db->link(&docs);
			// first try without autosaving in order
			// to keep the cancel button active. Let
			// readRow lock on behalf of thread_db.
			// note Fbase1 save/rollback use db.
			try {
				try {readRow(true);}
				catch(UserException & e) {
					if(GETDOCID(getId())) { // record is present?
						// retry by first releasing our own locks
						// if we have them. If the failure was due
						// to changed data, we won't notice however.
						db = base_db; // autosave operates on db
						try {autosave();} // release our locks.
						__finally {db = thread->thread_db;}
						try {
							readRow(true);
							setDocData(thread->doc);
						} catch(UserException & e) {
							AnsiString msg = AnsiString::LoadStr(
								FMDETAIL_DOCALREADYOPENED);
							if(ATTYN(msg.c_str()) == ID_NO) {Abort();}
							else {thread->doc.readonly = true;}
						}
					} else {throw;}
				}
			} __finally {
				db->unlink(&docs);
				db = base_db;
				db->link(&docs);
			}
		} catch(Exception & e) {
			thread->Terminate(); // signal Terminated
			thread->Resume(); // actually terminate
			throw;
		}
		thread->Resume();
	} __finally {closePanel(form);}
}
//---------------------------------------------------------------------------

void __fastcall TFmdetail1::FormKeyPress(TObject *Sender, char &Key) {
	if(Key == VK_RETURN && editingRow()) {return;}
	TFbase1::FormKeyPress(Sender,Key);
}
//---------------------------------------------------------------------------

void __fastcall TFmdetail1::FormKeyDown(TObject *Sender, WORD &Key,
	TShiftState Shift) {
	// no vk_insert/delete: other components may respond to these keys
	TDataRows * rows = ActiveRows;
	if(Key == VK_F2) {
		if(rows->rowSelected() == false || rows->editingRow()) {return;}
		exitFirst(Sender); // first exit (may lead to Abort())
		ActiveControl = rows; rows->editRow();
		Key = '\0';
	} else if((Key == VK_UP || Key == VK_DOWN ||
		Key == VK_PRIOR || Key == VK_NEXT) && ActiveControl != rows) {
		TWinControl * old_control = exitFirst(Sender);
		Word key = Key; rows->KeyDown(key,Shift);
		ActiveControl = old_control;
		Key = '\0'; // prevent scrolling through other fields
	} else if(Key == VK_RETURN) {
		if(rows->editingRow()) {return;} // rows respond to enter in edit mode.
		else if(Fschedule1->Visible) {ModalResult = mrOk;}
		else {RowControl->SelectNextPage(!Shift.Contains(ssShift),true);}
		Key = '\0'; // VK_RETURN key handled (prevent beep).
	} else if(tolower(Key) == 'a' && Shift.Contains(ssCtrl) &&
		ActiveControl == rows && rows->selectAll()) {
		Key = '\0';
	}
}
//---------------------------------------------------------------------------

void __fastcall TFmdetail1::FormMouseWheelDown(TObject *Sender,
	  TShiftState Shift, TPoint &MousePos, bool &Handled) {
	Word key = VK_DOWN; TShiftState state;
	KeyDown(key,state); Handled = true;
}
//---------------------------------------------------------------------------

void __fastcall TFmdetail1::FormMouseWheelUp(TObject *Sender,
	  TShiftState Shift, TPoint &MousePos, bool &Handled) {
	Word key = VK_UP; TShiftState state;
	KeyDown(key,state); Handled = true;
}
//---------------------------------------------------------------------------

void __fastcall TFmdetail1::PrintSelection(void) {
	LOGFUNC();
	resetPrinter(); print_date = Now().DateTimeString();
	print_nr = 0; Enabled = false;
	try {
		try {ActiveRows->printSelection();}
		catch(Exception & e) {Application->ShowException(&e);}
	} __finally {Enabled = true; SetForegroundWindow(Handle);}
}
//---------------------------------------------------------------------------

void __fastcall TFmdetail1::PrintNewPage(TRect &Rect) {
	LOGFUNC();
	TCanvas * canvas = Printer()->Canvas;
	canvas->Font->Style = TFontStyles() << fsBold;
	Rect.top = TDataRows::wrapText(Caption,canvas,Rect);
	AnsiString text = FormatDateTime("yyyy'-'mm'-'dd, hh':'mm",
		TDateTime(print_date));
	text += " ("+IntToStr(++print_nr)+")";
	if(Exceptions::debug_mode) {Exceptions::writeLog(text);}
	Rect.top = TDataRows::wrapText(text,canvas,Rect);
	text = RowControl->ActivePage->Caption;
	Rect.top = TDataRows::wrapText(text,canvas,Rect) + 100;
	canvas->Font->Style = TFontStyles();
}
//---------------------------------------------------------------------------

void __fastcall TFmdetail1::setMenu(TMainMenu * menu,
	int menu_index,int help_index) {
	menus[menu_index] = menu;
//	Menu = menu; // select the menu first, necessary?
//	unsigned char data[80]; MENUITEMINFO item_info = {0};
//	item_info.cbSize = sizeof(item_info); item_info.fMask = MIIM_TYPE;
//	item_info.dwTypeData = (char *) data; item_info.cch = sizeof(data);
//	if(GetMenuItemInfo(Menu->Handle,help_index,TRUE,&item_info)) {
//		item_info.fType = item_info.fType | MFT_RIGHTJUSTIFY;
//		SetMenuItemInfo(Menu->Handle,help_index,TRUE,&item_info);
//		// SendMessage(menu->WindowHandle, CM_MENUCHANGED, 0, 0);
//	}
//	// DrawMenuBar(Handle);
}
//---------------------------------------------------------------------------

