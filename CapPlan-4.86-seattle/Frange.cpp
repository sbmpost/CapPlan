//$Id: Frange.cpp,v 1.60 1998/01/22 19:37:32 darseq Exp $
#include <vcl.h>
#pragma hdrstop
#include "Frange.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Fbase"
#pragma link "FixedCheckBox"
#pragma resource "*.dfm"
TFrange1 *Frange1;
//---------------------------------------------------------------------------
__fastcall TFrange1::TFrange1(TComponent* Owner) : TFbase1(Owner) {
	Color = clBtnFace;
	reset_focus = false;
	CountEdit->Text = IntToStr(1);
	TotalEdit->Text = IntToStr(1);
	CheckBox->Checked = true;
}
//---------------------------------------------------------------------------

void __fastcall TFrange1::SelectBoxClick(TObject *Sender) {
	for(int i=0;i != CheckListBox1->Count;i++) {
		CheckListBox1->Checked[i] = SelectBox->Checked;
	}
	ActiveControl = CheckListBox1;
}
//---------------------------------------------------------------------------

void __fastcall TFrange1::FormShow(TObject *Sender) {
	if(CountEdit->Visible) {ActiveControl = CountEdit;}
	else {ActiveControl = CheckListBox1;}
}
//---------------------------------------------------------------------------

void __fastcall TFrange1::buildIdList(id_t select_id) {
	LOGFUNC();
	vector<Person>::const_iterator person_it = employees.begin();
	for(;person_it != employees.end();person_it++) {
		if(person_it->employee == false) {continue;}
		id_list.push_back(person_it->id);
		AnsiString text = person_it->lastname+" ("+MinutesToStr(
			RoundTo(double(person_it->total_diff)*24*60,1))+"/"+
			IntToStr(person_it->entry_count)+")";
		int index = CheckListBox1->Items->Add(text);
		CheckListBox1->Checked[index] = person_it->entry_count != 0 &&
			(SelectBox->Checked || person_it->id == select_id);
	}
}
//---------------------------------------------------------------------------

TModalResult __fastcall TFrange1::view(TDateTime begin_date,
	TDateTime end_date,int count_value,int total_value,
	id_t select_id) {
	LOGFUNC();

	id_list.clear();
	CheckListBox1->Items->Clear();
	buildIdList(select_id);

	CountEdit->Text = count_value;
	TotalEdit->Text = total_value;
	date1 = DatePicker1->DateTime;
	date2 = DatePicker2->DateTime;
	return ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TFrange1::setAndCheckInput(void) {
	selected.clear();
	list<id_t>::const_iterator it = id_list.begin();
	for(int i=0;i != CheckListBox1->Count;i++,it++) {
		if(CheckListBox1->Checked[i]) {selected.push_back(*it);}
	}
	if(selected.size() == 0) {
		throw UserException(FRANGE_NOITEMSCHECKED);
	}
	TDateTime from = DatePicker1->DateTime +
		double(TimePicker1->DateTime) - int(TimePicker1->DateTime);
	TDateTime until = DatePicker2->DateTime +
		double(TimePicker2->DateTime) - int(TimePicker2->DateTime);
	checkTimeBounds(from,until);
	begin = from;
	end = until;
}
//---------------------------------------------------------------------------

void __fastcall TFrange1::FormClose(TObject *Sender, TCloseAction &Action) {
	LOGFUNC();
	if(ModalResult == mrOk) {setAndCheckInput();}
	TFbase1::FormClose(Sender,Action);
}
//---------------------------------------------------------------------------

void __fastcall TFrange1::DatePicker1Change(TObject *Sender) {
	TDateTime new_date = DatePicker1->DateTime;
	if(new_date == date1) {return;}
	DatePicker1->DateTime = date1 = weekDate(date1,new_date);
	if(DatePicker1->DateTime > DatePicker2->DateTime) {
		DatePicker2->DateTime = date2 = DatePicker1->DateTime;
	}
	if(DatePicker1->DroppedDown) {
		SYSTEMTIME systime; DateTimeToSystemTime(date1,systime);
		TCommonCalendarHack * calendar = (TCommonCalendarHack *) DatePicker1;
		SendMessage(calendar->CalendarHandle,MCM_SETCURSEL,0,(LPARAM) &systime);
		InvalidateRect(calendar->CalendarHandle,NULL,FALSE);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFrange1::DatePicker2Change(TObject *Sender) {
	TDateTime new_date = DatePicker2->DateTime;
	if(new_date == date2) {return;}
	DatePicker2->DateTime = date2 = weekDate(date2,new_date);
	if(DatePicker1->DateTime > DatePicker2->DateTime) {
		DatePicker1->DateTime = date1 = DatePicker2->DateTime;
	}
	if(DatePicker2->DroppedDown) {
		SYSTEMTIME systime; DateTimeToSystemTime(date2,systime);
		TCommonCalendarHack * calendar = (TCommonCalendarHack *) DatePicker2;
		SendMessage(calendar->CalendarHandle,MCM_SETCURSEL,0,(LPARAM) &systime);
		InvalidateRect(calendar->CalendarHandle,NULL,FALSE);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFrange1::DatePicker1CloseUp(TObject *Sender) {
	DatePicker1->DateTime = date1; reset_focus = true;
	PostMessage(DatePicker1->Handle,WM_KEYDOWN,VK_RIGHT,0);
}
//---------------------------------------------------------------------------

void __fastcall TFrange1::DatePicker2CloseUp(TObject *Sender) {
	DatePicker2->DateTime = date2; reset_focus = true;
	PostMessage(DatePicker2->Handle,WM_KEYDOWN,VK_RIGHT,0);
}
//---------------------------------------------------------------------------

void __fastcall TFrange1::TimePickerChange(TObject *Sender) {
	TDateTimePicker * picker = (TDateTimePicker *) Sender;
	picker->Date = Now(); // needed when setting 0:00 time
	if(TDateTime(double(picker->DateTime) -
		int(picker->DateTime)) < min_time) {
		picker->DateTime = min_time;
	}
	if(TDateTime(double(picker->DateTime) -
		int(picker->DateTime)) > max_time) {
		picker->DateTime = max_time;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFrange1::FormMouseWheelDown(TObject *Sender,
	  TShiftState Shift, TPoint &MousePos, bool &Handled) {
	int index = CheckListBox1->ItemIndex;
	if(index < CheckListBox1->Count-1) {CheckListBox1->ItemIndex = index+1;}
	Handled = true;
}
//---------------------------------------------------------------------------

void __fastcall TFrange1::FormMouseWheelUp(TObject *Sender,
	  TShiftState Shift, TPoint &MousePos, bool &Handled) {
	int index = CheckListBox1->ItemIndex;
	if(0 < index) {CheckListBox1->ItemIndex = index-1;}
	Handled = true;
}
//---------------------------------------------------------------------------

void __fastcall TFrange1::FormKeyPress(TObject *Sender, char &Key) {
	int index = CheckListBox1->ItemIndex;
	if(Key == VK_SPACE && index != -1 && // when using the mouse wheel
		String(ActiveControl->ClassName()) == "TDateTimePicker") {
		CheckListBox1->Checked[index] = 1 - CheckListBox1->Checked[index];
		Key = '\0';
	} else if(tolower(Key) == tolower(OkButton->Caption[2])) {
		OkButton->Click(); Key = '\0';
	} else if(tolower(Key) == tolower(CancelButton->Caption[2])) {
		CancelButton->Click(); Key = '\0';
	} else if(Key == VK_RETURN) {
		if(ActiveControl != DatePicker1 &&
			ActiveControl != DatePicker2) {
			OkButton->Click(); Key = '\0';
		}
	}
	TFbase1::FormKeyPress(Sender,Key);
}
//---------------------------------------------------------------------------

void __fastcall TFrange1::DatePickerDropDown(TObject *Sender) {
	TCommonCalendarHack * calendar = (TCommonCalendarHack *) Sender;
	HWND CalendarHandle = calendar->CalendarHandle;
	long style = GetWindowLong(CalendarHandle,GWL_STYLE);
	SetWindowLong(CalendarHandle,GWL_STYLE,style | MCS_WEEKNUMBERS);
	// SetWindowLong(CalendarHandle,GWL_STYLE,style | MCS_NOTODAY);

	RECT rect; SendMessage(CalendarHandle,MCM_GETMINREQRECT,0,(LPARAM) &rect);
	int today_width = SendMessage(CalendarHandle,MCM_GETMAXTODAYWIDTH,0,0);
	if(today_width > rect.right) {rect.right = today_width;}
	SetWindowPos(CalendarHandle,0,rect.left,rect.top,rect.right-rect.left+2,
		rect.bottom-rect.top+2,SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
}
//---------------------------------------------------------------------------

void __fastcall TFrange1::DatePickerKeyDown(TObject *Sender,
	  WORD &Key, TShiftState Shift) {
	TDateTimePicker * picker = (TDateTimePicker *) Sender;
	if(picker->DroppedDown || reset_focus) {
		reset_focus = false;
	} else if(Key == VK_LEFT || Key == VK_RIGHT ||
		Key == VK_UP || Key == VK_DOWN) {
		TDateTime new_date = picker->DateTime;
		if(Key == VK_LEFT) {
			new_date = weekDate(new_date,new_date-1);
		} else if(Key == VK_RIGHT) {
			new_date = weekDate(new_date,new_date+1);
		} else if(Key == VK_UP) {
			new_date = weekDate(new_date,new_date+DAYS_IN_WEEK);
		} else if(Key == VK_DOWN) {
			new_date = weekDate(new_date,new_date-DAYS_IN_WEEK);
		}
		Key = '\0';
		picker->DateTime = new_date;
		if(picker == DatePicker1) {
			date1 = new_date;
			if(DatePicker1->DateTime > DatePicker2->DateTime) {
				DatePicker2->DateTime = date2 = DatePicker1->DateTime;
			}
		} else {
			date2 = new_date;
			if(DatePicker1->DateTime > DatePicker2->DateTime) {
				DatePicker1->DateTime = date1 = DatePicker2->DateTime;
			}
		}
	} else if(Key == VK_RETURN) {Key = VK_F4;}
}
//---------------------------------------------------------------------------

void __fastcall TFrange1::CountEditExit(TObject *Sender) {
	try {
		int every = StrToInt(CountEdit->Text);
		if(every <= 0) {CountEdit->Text = 1;}
		else {CountEdit->Text = every;}
	} catch(EConvertError & e) {CountEdit->Text = 1;}
}
//---------------------------------------------------------------------------

void __fastcall TFrange1::TotalEditExit(TObject *Sender) {
	try {
		int total = StrToInt(TotalEdit->Text);
		if(total <= 0) {TotalEdit->Text = 1;}
		else {TotalEdit->Text = total;}
	} catch(EConvertError & e) {TotalEdit->Text = 1;}
}
//---------------------------------------------------------------------------

