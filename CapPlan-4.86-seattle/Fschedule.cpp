//$Id: Fschedule.cpp,v 1.53 2013/02/16 11:41:37 darseq Exp $
#include <vcl.h>
#pragma hdrstop
#include "Fschedule.h"
#include "Fcomplete.h"
#include "Foptions.h"
#include "Fpending.h"
#include "Farchive.h"
#include "Fmain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Fbase"
#pragma resource "*.dfm"
TFschedule1 *Fschedule1;
//---------------------------------------------------------------------------
__fastcall TFschedule1::TFschedule1(TComponent* Owner): TFbase1(Owner) {
	entries = Entries->Text;
	PER_LASTNAME = Farchive1->PersonRows->Meta->IndexOf("LASTNAME");
	DOC_LASTNAME = Fpending1->ScheduleRows->Meta->IndexOf("LASTNAME");
	PERSONAL_ID = Fpending1->ScheduleRows->Meta->IndexOf("PERSONAL_ID");
	DOC_NAME = Fpending1->ScheduleRows->Meta->IndexOf("DOC_NAME");
	selectDocument->Enabled = Fpending1->SCHEDULE->TabVisible;
	AbortButtonClick(this);
	Color = clBtnFace;
}
//---------------------------------------------------------------------------

void __fastcall TFschedule1::FormShow(TObject *Sender) {
	if(active_control) {ActiveControl = active_control;}
	checkDocId(); // update data if Fpending1 was opened
	int count = Fmain1->getEntryCount();
	Entries->Text = entries+" "+IntToStr(count);
	if(count && ActiveControl == selectEntries) {
		nextButton(FinishButton);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFschedule1::FormClose(TObject *Sender,TCloseAction &Action) {
	LOGFUNC();
	active_control = ActiveControl;
	TFbase1::FormClose(Sender,Action);
}
//---------------------------------------------------------------------------

void __fastcall TFschedule1::checkDocId(void) {
//  disabled: Fschedule doesn't call Fpending code anymore,
//  so it is not necessary to check if the row Fpending is
//  pointing to is the same as the one selected from here.
//	LOGFUNC();
//	if(is_document == false) {return;}
//	bool invalid_id = per_id == UNDEFINED;
//	invalid_id = invalid_id || doc_row == 0 ||
//		doc_row >= Fpending1->ScheduleRows->RowCount ||
//		per_id != Fpending1->ScheduleRows->Cells[PERSONAL_ID][doc_row] ||
//		doc_name != Fpending1->ScheduleRows->Cells[DOC_NAME][doc_row];
//	if(invalid_id) {AbortButtonClick(this);} // reset everything
}
//---------------------------------------------------------------------------

void __fastcall TFschedule1::selectPersonClick(TObject *Sender) {
	LOGFUNC(); setButton(selectPerson);
	if(Sender == NULL || getPerson()) {
		is_document = false;
		Edit1->Text = lastname;
		app_id = UNDEFINED; Edit2->Text = "";
		selectAppointment->Enabled = true;
		nextButton(selectAppointment);
	}
}
//---------------------------------------------------------------------------

// selecting a document from here is disabled.
void __fastcall TFschedule1::selectDocumentClick(TObject *Sender) {
	LOGFUNC(); setButton(selectDocument);
	if(Sender == NULL || getDocument()) { // sets doc_row
		is_document = true;
		per_id = Fpending1->ScheduleRows->Cells[PERSONAL_ID][doc_row];
		doc_name = Fpending1->ScheduleRows->Cells[DOC_NAME][doc_row];
		Edit1->Text = Fpending1->ScheduleRows->Cells[DOC_LASTNAME][doc_row];
		Fmain1->per_id = per_id; Fmain1->app_id = UNDEFINED;
		// make Farchive1 per_id match this document
		app_id = UNDEFINED; Edit2->Text = "";
		//disabled: only selecting instead of opening
		//try {Fpending1->open1Click(Sender);}
		/*__finally */ {checkDocId();}
		nextButton(selectAppointment);
		// check doc_id when pending refreshes its data
	} else if(Sender && Fpending1->ModalResult != mrAll) {checkDocId();}
}
//---------------------------------------------------------------------------

void __fastcall TFschedule1::selectAppointmentClick(TObject *Sender) {
	LOGFUNC();
	if(Sender == NULL || per_id != UNDEFINED) {
		setButton(selectAppointment);
		if(Sender == NULL || getAppointment()) {
			// If a document has been marked charged, still allow for setting
			// its link. A previously linked document results in a warning.
			Edit2->Text = "id: "+app_id;
			if(Fmain1->getEntryCount()) {nextButton(FinishButton);}
			else {nextButton(selectEntries);}
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFschedule1::selectEntriesClick(TObject *Sender) {
	setButton(selectEntries);
	Fmain1->RowGrid->Cursor = crHelp;
	Close();
}
//---------------------------------------------------------------------------

void __fastcall TFschedule1::FinishButtonClick(TObject *Sender) {
	LOGFUNC();
	if(per_id == UNDEFINED || app_id == UNDEFINED ||
		Fmain1->getEntryCount() == 0) {return;}
	Fmain1->per_id = per_id;
	Fmain1->app_id = app_id;
	setButton(FinishButton);
	try {
		try {Fmain1->entriesNew1Click(this);}
		catch(Exception &e) {nextButton(selectEntries); throw;}
	} __finally {
		// The entry count may be different now, so read it again.
		Entries->Text = entries+" "+IntToStr(Fmain1->getEntryCount());
	}
	if(is_document) {
		PostMessage(Fmain1->Handle,WM_COMMAND,
			Fmain1->pending1->Command,NULL);
	}
	AbortButtonClick(Sender);
	ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TFschedule1::AbortButtonClick(TObject *Sender) {
	per_id = app_id = doc_id = doc_name = UNDEFINED;
	Edit1->Text = lastname = ""; Edit2->Text = "";
	doc_row = 0; is_document = false;
	Fmain1->RowGrid->Cursor = crArrow;
	nextButton(selectPerson);
}
//---------------------------------------------------------------------------

void __fastcall TFschedule1::setButton(TButton * button) {
	selectPerson->Font->Style = TFontStyles();
	selectDocument->Font->Style = TFontStyles();
	selectAppointment->Font->Style = TFontStyles();
	selectEntries->Font->Style = TFontStyles();
	FinishButton->Font->Style = TFontStyles();
	if(button) {button->Font->Style = TFontStyles() << fsBold;}
}
//---------------------------------------------------------------------------

void __fastcall TFschedule1::nextButton(TButton * next_button) {
	if(next_button == selectDocument &&
		selectDocument->Enabled == false) {
		next_button = selectPerson;
	}
	ActiveControl = curr_button = next_button;
	setButton(next_button);
}
//---------------------------------------------------------------------------

void __fastcall TFschedule1::FormKeyPress(TObject *Sender, char &Key) {
	if(tolower(Key) == tolower(FinishButton->Caption[2])) {
		FinishButton->Click(); Key = '\0';
	} else if(tolower(Key) == tolower(AbortButton->Caption[2])) {
		AbortButton->Click(); Key = '\0';
	} else if(Key == VK_ESCAPE) {ActiveControl = curr_button;}
	TFbase1::FormKeyPress(Sender,Key);
}
//---------------------------------------------------------------------------

void __fastcall TFschedule1::FormKeyDown(TObject *Sender, WORD &Key,
	TShiftState Shift) {
	if(Key == VK_DOWN || Key == VK_UP ||
		Key == VK_RIGHT || Key == VK_LEFT) {
		ActiveControl = FindNextControl(ActiveControl,
			Key == VK_DOWN || Key == VK_RIGHT,true,false);
		Key = '\0';
	}
}
//---------------------------------------------------------------------------

bool __fastcall TFschedule1::getPerson(void) {
	LOGFUNC();
	try {
		Farchive1->APPS->TabVisible = false;
		Farchive1->DOCS->TabVisible = false;
		Farchive1->SCHEDULES->TabVisible = false;
		Farchive1->PersonRows->Cursor = crHelp;
		if(TFoptions1::o.remember == false) {
			Fmain1->per_id = UNDEFINED;
		} else if(per_id != UNDEFINED) {
			Fmain1->per_id = per_id;
		}
		Fmain1->view_archive(Farchive1->PERSONS);
	} __finally {
		Farchive1->APPS->TabVisible = true;
		Farchive1->DOCS->TabVisible = true;
		Farchive1->SCHEDULES->TabVisible = true;
		Farchive1->PersonRows->Cursor = crDefault;
	}
	if(Farchive1->ModalResult != mrIgnore &&
		Farchive1->ModalResult != mrAll &&
		Fmain1->per_id != UNDEFINED) {
		per_id = Fmain1->per_id;
		lastname = Farchive1->PersonRows->Cells[
			PER_LASTNAME][Farchive1->PersonRows->Row];
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------

// selecting a document from here is disabled.
bool __fastcall TFschedule1::getDocument(void) {
	LOGFUNC();
	TTabSheet * page = Fpending1->RowControl->ActivePage;
	// todo: what if Fpending1->SCHEDULE->TabVisible = false
	Fpending1->RowControl->ActivePage = Fpending1->SCHEDULE;
	bool rev_visible = Fpending1->REVIEW->TabVisible;
	bool pur_visible = Fpending1->REPURCHASE->TabVisible;
	bool inv_visible = Fpending1->INVOICE->TabVisible;
	// The Fpending tabs are not always all visible.
	try {
		Fpending1->REVIEW->TabVisible = false;
		Fpending1->REPURCHASE->TabVisible = false;
		Fpending1->INVOICE->TabVisible = false;
		Fpending1->ScheduleRows->Cursor = crHelp;
		if(doc_id != UNDEFINED) {
			Fmain1->doc_id = doc_id;
		}
		Fmain1->pending1Click(this);
	} __finally {
		Fpending1->REVIEW->TabVisible = rev_visible;
		Fpending1->REPURCHASE->TabVisible = pur_visible;
		Fpending1->INVOICE->TabVisible = inv_visible;
		Fpending1->ScheduleRows->Cursor = crDefault;
		Fpending1->RowControl->ActivePage = page;
		// when appointment entries were selected.
		int count = Fmain1->getEntryCount();
		Entries->Text = entries+" "+IntToStr(count);
	}
	if(Fpending1->ModalResult != mrIgnore &&
		Fpending1->ModalResult != mrAll &&
		Fmain1->doc_id != UNDEFINED) {
		doc_id = Fmain1->doc_id;
		doc_row = Fpending1->ScheduleRows->Row;
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------

bool __fastcall TFschedule1::getAppointment(void) {
	LOGFUNC();
	try {
		Farchive1->PERSONS->TabVisible = false;
		Farchive1->DOCS->TabVisible = false;
		Farchive1->SCHEDULES->TabVisible = false;
		Farchive1->AppRows->Cursor = crHelp;
		Fmain1->per_id = per_id;
		Fmain1->app_id = app_id;
		Fmain1->view_archive(Farchive1->APPS);
	} __finally {
		Farchive1->PERSONS->TabVisible = true;
		Farchive1->DOCS->TabVisible = true;
		Farchive1->SCHEDULES->TabVisible = true;
		Farchive1->AppRows->Cursor = crDefault;
		// when appointment entries were selected.
		int count = Fmain1->getEntryCount();
		Entries->Text = entries+" "+IntToStr(count);
	}
	if(Farchive1->ModalResult != mrIgnore &&
		Farchive1->ModalResult != mrAll &&
		Fmain1->app_id != UNDEFINED) {
		app_id = Fmain1->app_id;
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------

void __fastcall TFschedule1::setPerson(id_t person_id,AnsiString name) {
	per_id = Fmain1->per_id = person_id;
	lastname = name; selectPersonClick(NULL);
	active_control = ActiveControl;
}
//---------------------------------------------------------------------------

void __fastcall TFschedule1::setDocument(id_t document_id,int row) {
	doc_id = Fmain1->doc_id = document_id;
	doc_row = row; selectDocumentClick(NULL);
	active_control = ActiveControl;
}
//---------------------------------------------------------------------------

void __fastcall TFschedule1::setAppointment(id_t appointment_id) {
	app_id = Fmain1->app_id = appointment_id;
	selectAppointmentClick(NULL);
	active_control = ActiveControl;
}
//---------------------------------------------------------------------------

