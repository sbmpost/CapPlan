//$Id: Fdetails.cpp,v 1.47 2013/12/09 23:29:50 darseq Exp $
#include <vcl.h>
#pragma hdrstop
#include "Fdetails.h"
#include "Foptions.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Fbase"
#pragma link "FixedCheckBox"
#pragma resource "*.dfm"
TFdetails1 *Fdetails1;
//---------------------------------------------------------------------------
__fastcall TFdetails1::TFdetails1(TComponent *Owner) : TFbase1(Owner) {
	Color = clBtnFace; reset_focus = false; DetailsLabel = Caption;
	Top = (Screen->WorkAreaHeight - Height)/2;
	Left = (Screen->WorkAreaWidth - Width)/2;
}
//---------------------------------------------------------------------------

void __fastcall TFdetails1::FormShow(TObject *Sender) {
	if(AttributeBox1->Enabled) {ActiveControl = AttributeBox1;}
}
//---------------------------------------------------------------------------

TModalResult __fastcall TFdetails1::view_per(bool & per_employee,
	bool & per_hidden,bool enable_hidden) {
	LOGFUNC();
	HISTORY->TabVisible = false;
	AttributeBox2->Visible = true;
	AttributeBox3->Visible = false;
	AttributeBox4->Visible = false;
	AttributeBox1->Caption = AnsiString::LoadStr(FDETAILS_PERSONEMPLOYEE);
	AttributeBox2->Caption = AnsiString::LoadStr(FDETAILS_PERSONHIDDEN);
	AttributeBox1->Checked = per_employee; AttributeBox1->Enabled = true;
	AttributeBox2->Checked = per_hidden;
	AttributeBox2->Enabled = enable_hidden;
	PageControl1->ActivePage = ATTRIBUTES;
	CancelButton->Enabled = true;
	TModalResult result = ShowModal();
	if(AttributeBox1->Checked != per_employee ||
		AttributeBox2->Checked != per_hidden) {
		per_employee = AttributeBox1->Checked;
		per_hidden = AttributeBox2->Checked;
	} else {result = mrIgnore;}
	return result;
}
//---------------------------------------------------------------------------

TModalResult __fastcall TFdetails1::view_app(bool & app_paid,
	bool & app_cancelled,bool & cancelled_intime,
	bool & app_exclusive,bool enable_exclusive) {
	LOGFUNC();
	HISTORY->TabVisible = false;
	AttributeBox2->Visible = true;
	AttributeBox3->Visible = true;
	AttributeBox4->Visible = true;
	AttributeBox1->Caption = AnsiString::LoadStr(FDETAILS_APPOINTMENTPAID);
	AttributeBox2->Caption = AnsiString::LoadStr(FDETAILS_APPOINTMENTCANCELLED);
	AttributeBox3->Caption = AnsiString::LoadStr(FDETAILS_CANCELLEDINTIME);
	AttributeBox4->Caption = AnsiString::LoadStr(FDETAILS_APPOINTMENTEXCLUSIVE);
	bool allow_modify = !app_exclusive || enable_exclusive;
	AttributeBox1->Checked = app_paid;
	AttributeBox1->Enabled = allow_modify;
	AttributeBox2->Checked = app_cancelled;
	AttributeBox2->Enabled = !app_cancelled && allow_modify;
	// The root user should not be able to uncancel an appointment because
	// we do not allow exact overlaps for entries that are not cancelled.
	AttributeBox3->Checked = app_cancelled && cancelled_intime;
	AttributeBox3->Enabled = app_cancelled && allow_modify;
	AttributeBox4->Checked = app_exclusive;
	AttributeBox4->Enabled = enable_exclusive;
	PageControl1->ActivePage = ATTRIBUTES;
	CancelButton->Enabled = true;
	TModalResult result;
	AttributeBox2->OnClick = CancelledBoxClick;
	try {result = ShowModal();}
	__finally {AttributeBox2->OnClick = NULL;}
	if(AttributeBox1->Checked != app_paid ||
		AttributeBox2->Checked != app_cancelled ||
		AttributeBox3->Checked != cancelled_intime ||
		AttributeBox4->Checked != app_exclusive) {
		app_paid = AttributeBox1->Checked;
		app_cancelled = AttributeBox2->Checked;
		cancelled_intime = AttributeBox3->Checked;
		app_exclusive = AttributeBox4->Checked;
	} else {result = mrIgnore;}
	return result;
}
//---------------------------------------------------------------------------

TModalResult __fastcall TFdetails1::view_doc(Document * document,
	bool enable_cancel,bool modal) {
	LOGFUNC();
	doc = document;
	HISTORY->TabVisible = true;
	AttributeBox2->Visible = true;
	AttributeBox3->Visible = true;
	AttributeBox4->Visible = true;
	Caption = DetailsLabel+": "+doc->name;
	CreatedBy->Caption = doc->created_by;
	Created->Caption = doc->created_date;
	Planned->Caption = doc->planned_date;
	Ordered->Caption = doc->ordered_date;
	Charged->Caption = doc->charged_date;
	AttributeBox1->Caption = AnsiString::LoadStr(FDETAILS_DOCUMENTSIGNED);
	AttributeBox2->Caption = AnsiString::LoadStr(FDETAILS_DOCUMENTSCHEDULE);
	AttributeBox3->Caption = AnsiString::LoadStr(FDETAILS_DOCUMENTURGENT);
	AttributeBox4->Caption = AnsiString::LoadStr(FDETAILS_DOCUMENTRESTORE);
	AttributeBox1->Checked = doc->is_signed;
	AttributeBox1->Enabled = true;
	AttributeBox2->Checked = doc->schedule || (doc->modified &&
		TFoptions1::o.schedule_doc);
	AttributeBox2->Enabled = !doc->schedule;
	AttributeBox3->Checked = doc->is_urgent;
	AttributeBox3->Enabled = AttributeBox2->Checked;
	AttributeBox4->Checked = doc->restore && doc->schedule && doc->modified;
	// allow to restore a document even when planned date has been reset
	AttributeBox4->Enabled = true; // disabled: !doc->restore;
	doc->restore = AttributeBox4->Checked;
	PageControl1->ActivePage = ATTRIBUTES;
	CancelButton->Enabled = enable_cancel;
	OnClose = onClose;
	AttributeBox1->OnClick = SignedBoxClick;
	AttributeBox2->OnClick = ScheduleBoxClick;
	if(modal) {ShowModal();}
	else {enable = TRUE; Show();}
	return ModalResult;
}
//---------------------------------------------------------------------------

TModalResult __fastcall TFdetails1::view_sch(bool & sch_hidden,
	bool enable_hidden) {
	LOGFUNC();
	HISTORY->TabVisible = false;
	AttributeBox2->Visible = false;
	AttributeBox3->Visible = false;
	AttributeBox4->Visible = false;
	AttributeBox1->Caption = AnsiString::LoadStr(FDETAILS_SCHEDULEHIDDEN);
	AttributeBox1->Checked = sch_hidden;
	AttributeBox1->Enabled = enable_hidden;
	PageControl1->ActivePage = ATTRIBUTES;
	CancelButton->Enabled = true;
	TModalResult result = ShowModal();
	if(AttributeBox1->Checked != sch_hidden) {
		sch_hidden = AttributeBox1->Checked;
	} else {result = mrIgnore;}
	return result;
}
//---------------------------------------------------------------------------

void __fastcall TFdetails1::OkButtonClick(TObject *Sender) {Close();}
//---------------------------------------------------------------------------

void __fastcall TFdetails1::CancelButtonClick(TObject *Sender) {
	// use Close() instead of setting ModalResult (not always modal)
	if(CancelButton->Enabled) {Close(); ModalResult = mrIgnore;}
}
//---------------------------------------------------------------------------

void __fastcall TFdetails1::onClose(TObject *Sender,TCloseAction &Action) {
	LOGFUNC();
	TFbase1::FormClose(Sender,Action);
	OnClose = TFbase1::FormClose;
	AttributeBox1->OnClick = NULL;
	AttributeBox2->OnClick = NULL;
	if(ModalResult != mrIgnore &&
		TFoptions1::o.signed_sched &&
		TFoptions1::o.schedule_doc &&
		AttributeBox1->Checked == false &&
		AttributeBox2->Enabled &&
		AttributeBox2->Checked) {
		enable = FALSE; // disable this form (see ThreadModify)
		if(MessageBox(GetActiveHandle(),AnsiString::LoadStr(
			FDETAILS_NOTSIGNEDSCHEDULE).c_str(),AnsiString(PRODUCT+String(": ")+
			doc->name).c_str(),MB_YESNO | MB_ICONQUESTION) == ID_NO) {
			AttributeBox2->Checked = false;
			AttributeBox3->Checked = false;
		}
	}

	if(AttributeBox1->Checked != doc->is_signed ||
		AttributeBox2->Checked != doc->schedule ||
		AttributeBox3->Checked != doc->is_urgent ||
		AttributeBox4->Checked != doc->restore) {
		doc->is_signed = AttributeBox1->Checked;
		doc->schedule = AttributeBox2->Checked;
		doc->is_urgent = AttributeBox3->Checked;
		doc->restore = AttributeBox4->Checked;
	} else {ModalResult = mrIgnore;}
}
//---------------------------------------------------------------------------

void __fastcall TFdetails1::SignedBoxClick(TObject *Sender) {
	// doc signed -> schedule
	if(AttributeBox1->Checked && TFoptions1::o.signed_sched &&
		AttributeBox2->Enabled /*&& isalnum(Caption[1])*/) {
		AttributeBox2->Checked = true;
		ScheduleBoxClick(Sender);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFdetails1::ScheduleBoxClick(TObject *Sender) {
	AttributeBox3->Enabled = AttributeBox2->Checked;
	if(AttributeBox2->Checked == false) {
		AttributeBox3->Checked = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFdetails1::CancelledBoxClick(TObject *Sender) {
	AttributeBox3->Checked = AttributeBox2->Checked;
	AttributeBox3->Enabled = AttributeBox2->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TFdetails1::FormKeyPress(TObject *Sender, char &Key) {
	if(tolower(Key) == tolower(OkButton->Caption[2])) {
		OkButton->Click(); Key = '\0';
	} else if(Key == VK_ESCAPE ||
		tolower(Key) == tolower(CancelButton->Caption[2])) {
		CancelButton->Click(); Key = '\0';
	} else if(Key == VK_RETURN) {
		OkButton->Click(); Key = '\0';
	}
	TFbase1::FormKeyPress(Sender,Key);
}
//---------------------------------------------------------------------------

void __fastcall TFdetails1::FormKeyDown(TObject *Sender, WORD &Key,
	  TShiftState Shift) {
	if(Key == VK_DOWN || Key == VK_UP ||
		Key == VK_RIGHT || Key == VK_LEFT) {
		ActiveControl = FindNextControl(ActiveControl,
			Key == VK_DOWN || Key == VK_RIGHT,true,false);
		Key = '\0';
	}
}
//---------------------------------------------------------------------------

