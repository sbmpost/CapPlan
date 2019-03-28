//$Id: Fbase.cpp,v 1.154 1998/01/22 19:37:31 darseq Exp $
#include <vcl.h>
#pragma hdrstop
#include <gdiplus.h>
#include <dateutils.hpp>
#include <Masks.hpp>
#include <mapi.h>
#include "Fbase.h"
#include "Fmain.h"
#include "Fimport.h"
#include "Fmdetail.h"
#include "Foptions.h"
#include "Fpending.h"
#include "Farchive.h"
#include "Fdetails.h"
#include "Fprint.h"
#include "Frange.h"
#include "Ffind.h"
#include "Fcodes.h"
#include "Fentries.h"
#include "Fcreated.h"
#include "Fschedule.h"
#include "Fcomplete.h"
#include "Femployee.h"
#include "ThreadModify.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFbase1 *Fbase1;
bool TFbase1::root_user;
TDateTime TFbase1::min_time;
TDateTime TFbase1::max_time;
TDateTime TFbase1::min_date;
TDateTime TFbase1::max_date;
TDateTime TFbase1::current_date;
TDateTime TFbase1::previous_date;
vector<Person> TFbase1::employees;
vector<Schedule> TFbase1::schedules;
Schedule TFbase1::grid;
Odbc * TFbase1::db = NULL;
Query TFbase1::db_query;
Variant TFbase1::word_app;
AnsiString TFbase1::word_cap;

AnsiString Policy::lock;
AnsiString Policy::lock_end;
//---------------------------------------------------------------------------

__fastcall TFbase1::TFbase1(TComponent* Owner) : TForm(Owner) {
	Color = TColor(0xF2D2B8);
	Panel1->BringToFront();
	modified = false;
}
//---------------------------------------------------------------------------

void __fastcall TFbase1::CreateParams(TCreateParams &Params) {
	// Windows ensures that overlapped windows stay on top of their
	// owners (not parents!). The vcl however always sets WndParent
	// to Application->Handle and it is not possible to change the
	// owner afterwards. To remedy this situation, apply the hack
	// below. To check the owner use: GetWindow(Handle,GW_OWNER).
	TForm::CreateParams(Params);
	if(dynamic_cast<TFarchive1 *>(this)) {
#ifdef INNODB // WINE <-> INNODB
		Params.WndParent = Fmain1->Handle;
#else
		Params.WndParent = Fpending1->Handle;
#endif
	} else if(dynamic_cast<TFimport1 *>(this)) {
		Params.WndParent = Fmain1->Handle;
	} else if(dynamic_cast<TFmdetail1 *>(this)) {
		Params.WndParent = Fmain1->Handle;
	} else if(dynamic_cast<TFemployee1 *>(this)) {
#ifdef INNODB // WINE <-> INNODB
		Params.WndParent = Fmain1->Handle;
#else
		Params.WndParent = Fmail1->Handle;
#endif
	} else if(dynamic_cast<TFemployee1 *>(this)) {
#ifdef INNODB // WINE <-> INNODB
		Params.WndParent = Fmain1->Handle;
#else
		Params.WndParent = Fmail1->Handle;
#endif
	} else if(dynamic_cast<TFrange1 *>(this)) {
		Params.WndParent = Fmain1->Handle;
	} else if(dynamic_cast<TFoptions1 *>(this)) {
		Params.WndParent = Fmain1->Handle;
	} else if(dynamic_cast<TFcodes1 *>(this)) {
		Params.WndParent = Fmain1->Handle;
	} else if(dynamic_cast<TFschedule1 *>(this)) {
		Params.WndParent = Fmain1->Handle;
	} else if(dynamic_cast<TFcomplete1 *>(this)) {
		Params.WndParent = Fpending1->Handle;
	} else if(dynamic_cast<TFdetails1 *>(this)) {
		Params.WndParent = Farchive1->Handle;
		// disabled: this does not seem very useful actually
		// if(Owner) {Params.WndParent = Farchive1->Handle;}
		// else {Params.WndParent = NULL;}
	} else if(dynamic_cast<TFmain1 *>(this)) {
		Params.WndParent = NULL;
		// The Application window is not the owner
		// of any form anymore. To prevent it from
		// displaying in the taskbar or taskmgr, it
		// is hidden when Fmain::FormShow is called.
		// When >= WINXP, make sure the application
		// window is always hidden from the taskbar
		// by applying the EX_TOOLWINDOW style.
		SetWindowLong(Application->Handle,GWL_EXSTYLE,
			GetWindowLong(Application->Handle,
			GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
	} else {throw ProgException("Unknown form");}
	// unfortunately we cannot override the
	// owner of dialogs (TOpenDialog, etc)
}
//---------------------------------------------------------------------------

void __fastcall TFbase1::FormClose(TObject *Sender, TCloseAction &Action) {
	// When closing, actually -leave- the focussed control.
	// If a form wants to return to the previously active
	// control, it should call exitFirst by itself storing
	// the ActiveControl in a local variable and restoring
	// it when the form activates.
	try {exitFirst(Sender);} catch(EAbort & e) {}
	Action = caHide;
}
//---------------------------------------------------------------------------

// exit data input controls before closing the form to store its data
class TWinControlHack : public TWinControl {public: __property OnExit;};
TWinControl * __fastcall TFbase1::exitFirst(TObject * Sender) {
	LOGFUNC();
	TWinControlHack * active_control = (TWinControlHack *) ActiveControl;
	if(active_control) {
		TNotifyEvent onexit = active_control->OnExit;
		if(ModalResult == mrAbort) {active_control->OnExit = NULL;}
		ActiveControl = NULL; // instantly produces exit event
		// execution continues when exit produced an exception
		active_control->OnExit = onexit;
	}
	// The OnExit event handler of the previously focused control is called
	// immediately when setting ActiveControl. Note that execution continues
	// even when an OnExit handler throws an exception because it was called
	// from the vcl thread. But we know that ModalResult was set to mrAbort.
	if(ModalResult == mrAbort) {Abort();} // Abort throws a silent exception
	return active_control;
}
//---------------------------------------------------------------------------

void __fastcall TFbase1::FormKeyPress(TObject *Sender, char &Key) {
	// Close() returns ModalResult mrCancel. To
	// signal a 'real' cancel, we use mrIgnore.
	if(Key == VK_ESCAPE) {ModalResult = mrIgnore; Key = '\0';}
	if(Key == VK_RETURN) {Key = '\0';} // prevent beep
}
//---------------------------------------------------------------------------

void __fastcall GetEncoderClsid(const WCHAR * format,
	CLSID * class_id) {
	UINT num = 0; UINT size = 0;
	Gdiplus::GetImageEncodersSize(&num,&size);
	if(size == 0) {Abort();}
	Gdiplus::ImageCodecInfo * codec_info = NULL;
	codec_info = (Gdiplus::ImageCodecInfo *) (malloc(size));
	if(codec_info == NULL) {Abort();}
	try {
		if(GetImageEncoders(num,size,codec_info) == Gdiplus::Ok) {
			UINT i=0;
			for(;i != num;i++) {
				if(wcscmp(codec_info[i].MimeType,format) == 0) {
					*class_id = codec_info[i].Clsid;
					break;
				}
			} if(i == num) {Abort();}
		} else {Abort();}
	} __finally {free(codec_info);}
}
//---------------------------------------------------------------------------

bool __fastcall TFbase1::MainFormCloseQuery(bool show_message) {
	bool CanClose = true;
	if(ThreadModify::threads.size()) {
		CanClose = false;
		if(show_message) {
			MSGO(AnsiString::LoadStr(FBASE_WAITINGCLOSEDOC).c_str());
		}
	} else if(modified) {
		CanClose = false;
		if(show_message) {
			MSGO(AnsiString::LoadStr(FMAIN_UNSAVEDCHANGES).c_str());
		}
	}
	return CanClose;
}
//---------------------------------------------------------------------------

void __fastcall TFbase1::WndProc(TMessage& Message) {
	if(ControlState.Contains(csDestroyingHandle)) {
		// ignore any pending window messages such as FormResize
		// when the window is in the process of being destroyed.
		Message.Result = 0;
	} else if(Message.Msg == CM_DIALOGKEY && (
		Message.WParam == VK_UP || Message.WParam == VK_DOWN ||
		Message.WParam == VK_LEFT || Message.WParam == VK_RIGHT)) {
		Message.Result = 0;
	} else if(Message.Msg == WM_SYSCOMMAND && (
		(Message.WParam & 0xfff0) == SC_MINIMIZE ||
		(Message.WParam & 0xfff0) == SC_RESTORE) &&
		// just minimize modelless Fdetails forms
		(dynamic_cast<TFdetails1 *>(this) == NULL ||
		this == Fdetails1)) {
		bool Handled = false;
		if(Application->OnMessage) {
			tagMSG msg = {Handle,Message.Msg,
				Message.WParam,Message.LParam,0,0,0};
			Application->OnMessage(msg,Handled);
		}
		if(Handled) {Message.Result = 0;}
		else {TForm::WndProc(Message);}
	} else if(Message.Msg == WM_ACTIVATE &&
		Message.WParam != WA_INACTIVE &&
		IsWindowEnabled(Handle) == FALSE) {
#ifdef _DEBUG
		char text[50] = {0};
		GetWindowText(Handle,text,sizeof(text));
		AnsiString msg("A disabled window is activated: ");
		Exceptions::writeLog(msg+text);
#endif
		// Windows actually sometimes activates a disabled window.
		// The vcl does not handle this correctly, so fix it here.
		// History of reproducing and solving the hidden form bug:
		// bug 1: modal form2 hides behind form1.
		// solution: disable windows XP ghosting feature.
		// bug 2: modal form2 still hides behind form1.
		// solution: set the owner of form2 to form1 in CreateParams.
		// bug 3: form1 receives focus while modal form2 is showing.
		// solution: reactivate form2 if form1 is currently disabled.
		// If ghosting windows is disabled, bug 2 and 3 can be shown
		// to occur by setting a breakpoint in Forms::ShowModal() on
		// the call to Show;. When the breakpoint hits, press the F8
		// key and then the F9 key. form1 will be activated and the
		// shortcuts of its menu can be used. QC report 3730 shows 1.
		PostMessage(Application->Handle,WM_ACTIVATE,WA_ACTIVE,0);
		Message.Result = 0; // message is handled
	} else if(Message.Msg == WM_ACTIVATEAPP &&
		Message.WParam == FALSE && Fmain1->Visible) {
		Fpending1->hint_window->ReleaseHandle();
		TForm::WndProc(Message);
	} else if(Message.Msg == WM_QUERYENDSESSION &&
		this == Fmain1) {
		if(MainFormCloseQuery(false)) {
			Exceptions::clearlog = true;
			Application->Terminate();
		} else {Message.Result = 0;}
	} else if(Message.Msg == WM_ENDSESSION &&
		this == Fmain1) {
		if(Message.WParam == TRUE) {
			Application->ProcessMessages();
			// needed to cleanup properly
		} else {MainFormCloseQuery(true);}
		TForm::WndProc(Message);
	} else if(Message.Msg == WM_SYSCOMMAND &&
		(Message.WParam & 0xfff0) == SC_CLOSE &&
		this == Fmain1) {
		if(MainFormCloseQuery(true)) {
			TForm::WndProc(Message);
		} else {Message.Result = 0;}
	} else if(Message.Msg == WM_APP) {
		Message.Result = 0; // signal failure
		Graphics::TBitmap * bitmap = GetFormImage();
		try {writePngImage(PRODUCT,bitmap);}
		__finally {delete bitmap;}
		unsigned short year,month,day;
		current_date.DecodeDate(&year,&month,&day);
		Message.Result = (year << 16) | (month << 8) | day;
	} else {TForm::WndProc(Message);}
}
//---------------------------------------------------------------------------

AnsiString __fastcall TFbase1::writePngImage(AnsiString filename,
	Graphics::TBitmap * graphics_bitmap) {
	WideString png_path = getPath("TMP");
	png_path = png_path+"\\"+filename+".png";
	DeleteFile(png_path);
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken,&gdiplusStartupInput,NULL);
	try {
		graphics_bitmap->Dormant();
		graphics_bitmap->FreeImage();
		HBITMAP hbitmap = graphics_bitmap->ReleaseHandle();
		Gdiplus::Bitmap bitmap(hbitmap,(HPALETTE)0);
		CLSID class_id;
		GetEncoderClsid(L"image/png",&class_id);
		bitmap.Save(png_path.c_bstr(),&class_id,NULL);
	} __finally {Gdiplus::GdiplusShutdown(gdiplusToken);}
	return AnsiString(png_path);
}
//---------------------------------------------------------------------------

void __fastcall TFbase1::refreshForms(void) {
	LOGFUNC();
	for(int i=0;i != Screen->FormCount;i++) {
		TFbase1 * form = dynamic_cast<TFbase1 *>(Screen->Forms[i]);
		if(form != NULL && form != this) {form->Refresh();}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFbase1::flushPaint(void) {
	MSG m;
	while(PeekMessage(&m,NULL,WM_PAINT,WM_PAINT,PM_REMOVE)) {
		DispatchMessage(&m);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFbase1::checkTimeBounds(TDateTime from,TDateTime until) {
	if(until < from) {
		throw UserException(FBASE_DATEORDERERROR);
	} else if(double(until - from) < 0.5/grid.tunits) {
		throw UserException(FBASE_VISIBLERANGEERROR);
	}

	int day = DayOfTheWeek(int(from)) - DayMonday;
	if(day < grid.wstart || day > grid.wstop - 1) {
		throw UserException(FBASE_VISIBLERANGEERROR);
	}

	if(TDateTime(double(from) - int(from)) < min_time ||
		TDateTime(double(until) - int(until)) > max_time) {
		throw UserException(FBASE_VISIBLERANGEERROR);
	}
}
//---------------------------------------------------------------------------

// skip days that are not part of the week as defined by wstart/wstop
TDateTime __fastcall TFbase1::weekDate(TDateTime from,TDateTime to) {
	int day = DayOfTheWeek(to) - DayMonday;
	int wstart = grid.wstart;
	int wstop = grid.wstop;
	if(day < wstart) {
		if(to > from) {day = wstart;}
		else {day = wstop - 1 - DAYS_IN_WEEK;}
	} else if(day > wstop - 1) {
		if(to > from) {day = wstart + DAYS_IN_WEEK;}
		else {day = wstop - 1;}
	}
	TDateTime new_date(int(StartOfTheWeek(to)) + day);
	// compensate when user points the calendar out of range.
	// drag/drop of RowCells is compensated by UnitsToDate()
	if(new_date < min_date) {new_date = min_date;}
	if(new_date > max_date) {new_date = max_date;}
	return new_date;
}
//---------------------------------------------------------------------------

TPrinter * __fastcall TFbase1::resetPrinter(void) {
	// from SetPrinter help: The
	// global TPrinter object is
	// freed automatically when
	// the application shuts down.
	TPrinter * printer = Printer();
	TCanvas * canvas = printer->Canvas;
	canvas->Brush->Color = clWhite;
	canvas->Pen->Color = clBlack;
	canvas->Pen->Width = 1;
	canvas->Pen->Style = psSolid;
	canvas->Font = Femployee1->Font;
	canvas->Font->Size = TFoptions1::o.font;
	return printer;
}
//---------------------------------------------------------------------------

TFbase1 * __fastcall TFbase1::showPanel(int msgindex) {
	TFbase1 * form = dynamic_cast<TFbase1 *>(Screen->ActiveForm);
	if(form) {
		form->Panel1->Top = (form->ClientHeight-form->Panel1->Height-50)/2;
		form->Panel1->Left = (form->ClientWidth-form->Panel1->Width)/2;
		form->Panel1->Caption = AnsiString::LoadStr(msgindex);
		form->Panel1->Visible = true; form->Panel1->Update();
	}
	return form;
}
//---------------------------------------------------------------------------

void __fastcall TFbase1::closePanel(TFbase1 * form) {
	if(form) {
		// while(eatMessages()) {Sleep(10);}
		form->Panel1->Visible = false;
	}
}
//---------------------------------------------------------------------------

AnsiString __fastcall TFbase1::MinutesToStr(int minutes) {
	int hours = minutes/60; minutes = minutes - 60*hours;
	AnsiString format("%02d:%02d"); if(hours > 99) {format = "%d:%02d";}
	char str[8+1]; snprintf(str,sizeof(str)-1,format.c_str(),hours,minutes);
	str[8] = '\0'; // when nsize chars written, snprintf does not add '\0'
	return String(str);
}
//---------------------------------------------------------------------------

int __fastcall TFbase1::StrToMinutes(AnsiString string) {
	AnsiString hours; AnsiString minutes; int i=1;
	for(;i <= string.Length() && string[i] != ':';i++) {hours += string[i];}
	for(++i;i <= string.Length();i++) {minutes += string[i];}
	return StrToInt(hours)*60+StrToInt(minutes);
}
//---------------------------------------------------------------------------

void __fastcall TFbase1::getEmployeeIds(Query & query,
	id_t schedule_id,list<id_t> * employee_ids) { // reference does not work?
	if(schedule_id == UNDEFINED) {schedule_id = "NULL";}
	AnsiString select = "select personal.PERSONAL_ID from personal inner "
		"join employees on personal.PERSONAL_ID=employees.PERSONAL_ID where "
		"employees.SCHEDULE_ID="+schedule_id;
	AnsiString restrict = "(HIDDEN=0 or CREATED_BY="+
		QuotedStr(db->getUsername())+")";
	if(!root_user) {select += (" and "+restrict);}
	select += " order by EMPLOYEE_ORDER";
	query.execSQL(select);
	while(query.next()) {
		employee_ids->push_back(query.fieldByName("PERSONAL_ID").toString());
	}
}
//---------------------------------------------------------------------------

bool __fastcall TFbase1::addEmployee(Query & query,id_t employee_id) {
	LOGFUNC();
	id_t grid_id = grid.id; if(grid_id == UNDEFINED) {grid_id = "NULL";}
	AnsiString select = "select personal.PERSONAL_ID,SCHEDULE_ID,LASTNAME,"
		"ADDRESS,CITY,TELEPHONE,TELEPHONE2,POSTCODE from personal";
	AnsiString restrict = "(HIDDEN=0 or CREATED_BY="+
		QuotedStr(db->getUsername())+")";
	if(employee_id == UNDEFINED) {
		select += " inner join employees on personal.PERSONAL_ID="
			"employees.PERSONAL_ID where employees.SCHEDULE_ID="+grid_id;
		if(!root_user) {select += (" and "+restrict);}
		select += " order by EMPLOYEE_ORDER";
	} else {
		select += " left join employees on personal.PERSONAL_ID="
			"employees.PERSONAL_ID and employees.SCHEDULE_ID="+grid_id+
			" where personal.PERSONAL_ID="+employee_id;
		if(!root_user) {select += (" and "+restrict);}
	}

	query.execSQL(select);
	Person person;
	person.id = UNDEFINED;
	while(query.next()) {
		person.get(query);
		employees.push_back(person);
	}
	return person.id != UNDEFINED;
}
//---------------------------------------------------------------------------

bool __fastcall TFbase1::addSchedule(Query & query,id_t schedule_id) {
	LOGFUNC();
	// when schedule_id is UNDEFINED, select all
	AnsiString select = "select * from schedules";
	AnsiString restrict = "(HIDDEN=0 or CREATED_BY="+
		QuotedStr(db->getUsername())+")";
	if(schedule_id == UNDEFINED) {
		if(!root_user) {select += (" where "+restrict);}
		select += " order by SCHEDULE_NAME";
	} else {
		select += " where SCHEDULE_ID="+schedule_id;
		if(!root_user) {select += (" and "+restrict);}
	}

	query.execSQL(select);
	Schedule schedule; schedule.id = UNDEFINED;
	while(query.next()) { // UNDEFINED or schedule was not public
		schedule.get(query);
		schedules.push_back(schedule);
	}
	return schedule.id != UNDEFINED;
	//PROG_IF(schedule.id == UNDEFINED && schedule_id != UNDEFINED,
	//	"schedule was deleted from the database but (hash) entry not");
	//may happen when the current grid is deleted but re-added by Refresh
}
//---------------------------------------------------------------------------

unsigned int __fastcall TFbase1::findEmployee(id_t employee_id) {
	unsigned int index;
	vector<Person>::const_iterator person_it = employees.begin();
	for(index=0;person_it != employees.end();person_it++,index++) {
		if(person_it->id == employee_id) {break;}
	}
	return index;
}
//---------------------------------------------------------------------------

unsigned int __fastcall TFbase1::findSchedule(id_t schedule_id) {
	unsigned int index;
	vector<Schedule>::const_iterator schedule_it = schedules.begin();
	for(index=0;schedule_it != schedules.end();schedule_it++,index++) {
		if(schedule_it->id == schedule_id) {break;}
	}
	return index;
}
//---------------------------------------------------------------------------

bool __fastcall TFbase1::eatMessages(void) {
	MSG m; // while waiting, eat all posted msgs
	bool process = PeekMessage(&m,NULL,0,0,PM_REMOVE);
	if(process && (m.message == WM_PAINT || m.message == WM_NCPAINT ||
		m.message == WM_NCLBUTTONDOWN || m.message == WM_NCRBUTTONDOWN ||
		m.message == WM_SYSCOMMAND || m.message == WM_NULL)) {
		// todo: eat WM_CLOSE/DESTROY/QUIT/ENDSESSION?
		if((m.message == WM_SYSCOMMAND && (m.wParam & 0xfff0) == SC_CLOSE)) {
			MSGO(AnsiString::LoadStr(FBASE_WAITINGCLOSEDOC).c_str());
		} else {DispatchMessage(&m);}
	}
	return process;
}
//---------------------------------------------------------------------------

bool __fastcall TFbase1::shellExecute(AnsiString & pathname,
	TDateTime lupdate_utc,bool locate,bool file_readonly,
	AnsiString macro,int * new_size,bool * thread_term) {
	LOGFUNC();
	bool file_modified = false;
	if(pathname.Length() &&
		MatchesMask(pathname,"*.bat") == false &&
		MatchesMask(pathname,"*.cmd") == false &&
		MatchesMask(pathname,"*.com") == false &&
		MatchesMask(pathname,"*.exe") == false) {
		SHELLEXECUTEINFO info = {0};
		info.cbSize = sizeof(SHELLEXECUTEINFO);
		// [info.fMask = info.fMask | SEE_MASK_NOASYNC;
		// disabled: otherwise shellExecute runs its own
		// message loop while opening the document in which
		// case the user can interact with the active form].
		// By specifying SEE_MASK_NOASYNC, shellExecuteEx runs its
		// own message loop. This ensures messages are handled for
		// any windows that are created at the time of the call. If
		// a thread (indirectly) creates a window, it must handle
		// its messages (see the WaitForSingleObject documentation)
		if(thread_term) {info.fMask |= SEE_MASK_FLAG_DDEWAIT;}
		info.hwnd = GetActiveHandle(); info.nShow = SW_NORMAL;
		char exe_path[MAX_PATH] = "explorer.exe";
		AnsiString params; int file_handle = -1;
		BY_HANDLE_FILE_INFORMATION info_before;
		if(locate == false) {
			info.fMask = SEE_MASK_NOCLOSEPROCESS;
			file_handle = fileopen(pathname,file_readonly == false);
			try {
				info_before = fileinfo(file_handle);
				if(checkVersion(lupdate_utc,info_before)) {
					file_readonly = true; // newer version
				}
			} __finally {closefile(file_handle);}

			// Find the executable associated with pathname. This
			// allows us to start a new instance of the executable.
			if((int) FindExecutable(pathname.c_str(),NULL,exe_path) <= 32) {
				AnsiString msg = AnsiString::LoadStr(FBASE_ERROROPENING);
				ATTO((msg+pathname).c_str()); Abort();
			}

			if(file_readonly) {
				// word > 2003 complains about the file being in use if
				// it is on a network share and is write locked. So use
				// file2tmp to create a local copy of the file into the
				// temporary directory and update pathname accordingly.
				// This also allows for the actual file to be overwritten
				// with a new version if a readonly old version is opened.
				pathname = file2tmp(pathname);
				file_handle = fileopen(pathname,true);
			} else {FileSetReadOnly(pathname,false);}

			// after file2tmp
			params = AnsiQuotedStr(pathname,'\"');
			if(MatchesMask(exe_path,"*winword.exe")) {
				// When a second winword instance is started, it hands
				// pathname over to the first and terminates immediately.
				// This behaviour can be suppressed using '/w' parameter.
				params += " /w"; if(macro.Length()) {params += " /m"+macro;}
			}
		} else {params = "/select,"+AnsiQuotedStr(pathname,'\"');}

		try {
			info.lpFile = exe_path;
			info.lpParameters = params.c_str();
			if(ShellExecuteEx(&info) == FALSE) {
				AnsiString msg = AnsiString::LoadStr(FBASE_ERROROPENING);
				ATTO((msg+pathname).c_str()); Abort();
			} else if(info.hProcess) {
				if(Exceptions::debug_mode) {
					Exceptions::writeLog("shellExecute wait enter");
				}
				while(WaitForSingleObject(info.hProcess,10) == WAIT_TIMEOUT) {
					if(thread_term == NULL) {eatMessages();}
					else if(*thread_term) {break;}
				}
				if(Exceptions::debug_mode) {
					Exceptions::writeLog("shellExecute wait exit");
				}
				CloseHandle(info.hProcess);
				SetForegroundWindow(GetActiveHandle());
				// If file2tmp was called, a local copy of pathname
				// was created which is readonly. In this situation
				// it makes no sense to compare before with after.
				if(file_readonly == false) {
					try {
						file_handle = fileopen(pathname,false);
						BY_HANDLE_FILE_INFORMATION info_after;
						info_after = fileinfo(file_handle);
						if((info_before.ftLastWriteTime.dwLowDateTime !=
							info_after.ftLastWriteTime.dwLowDateTime) ||
							(info_before.ftLastWriteTime.dwHighDateTime !=
							info_after.ftLastWriteTime.dwHighDateTime)) {
							if(new_size) {*new_size = info_after.nFileSizeLow;}
							file_modified = true;
						}
					} catch(EAbort & e) {
						if(thread_term) {*thread_term = true;}
						else throw;
					}
				}
			}
		} __finally {closefile(file_handle);}
	}
	return file_modified;
}
//---------------------------------------------------------------------------

// Execute a Macro in a word document. The document
// version is not checked, nor is it locked. Note
// macroExecute() supports only string parameters.
// Out:0 if not a word document or no int returned
int __fastcall TFbase1::macroExecute(AnsiString pathname,
	TDateTime lupdate_utc,AnsiString macro,
	AnsiString params,bool show_message) {
	LOGFUNC();
	int result = 0;
	if(TFoptions1::o.run_macros && pathname.Length() &&
		// case-insensitive compare
		(MatchesMask(pathname,"*.doc") ||
		MatchesMask(pathname,"*.docm") ||
		MatchesMask(pathname,"*.dotm"))) {
		flushPaint(); // before openWordDoc
		TFbase1 * form = showPanel(FBASE_RUNNINGDOCMACRO);
		try {
			openWordDoc(QuotedStr(pathname));
			try {result = doExecMacro("ActiveDocument."+macro,params);}
			__finally {closeWordDoc();}
			closePanel(form);
		} catch(Exception & e) {
			closePanel(form);
			if(show_message) {throw;}
			Abort();
		}
	}
	return result;
}
//---------------------------------------------------------------------------

void __fastcall TFbase1::closefile(int & file_handle) {
	if(file_handle != -1) {FileClose(file_handle); file_handle = -1;}
}
//---------------------------------------------------------------------------

int __fastcall TFbase1::fileopen(AnsiString pathname,bool writelock) {
	unsigned int mode = writelock ? fmShareDenyWrite : fmShareDenyNone;
	int file_handle = FileOpen(pathname,fmOpenRead | mode);
	if(file_handle == -1) {
		int msgindex = writelock ? FBASE_ERRORLOCKING : FBASE_ERROROPENING;
		ATTO((AnsiString::LoadStr(msgindex)+pathname).c_str()); Abort();
	}
	return file_handle;
}
//---------------------------------------------------------------------------

BY_HANDLE_FILE_INFORMATION __fastcall TFbase1::fileinfo(int file_handle) {
	BY_HANDLE_FILE_INFORMATION info;
	if(GetFileInformationByHandle((HANDLE) file_handle,&info) == 0) {
		ATTO(AnsiString::LoadStr(FBASE_ERRORGETFILEINF).c_str()); Abort();
	}
	return info;
}
//---------------------------------------------------------------------------

TFileStream * __fastcall TFbase1::createfile(AnsiString pathname) {
	TFileStream * file;
	// Note that the share mode is ignored by FileCreate (known bug).
	try {file = new TFileStream(pathname,fmCreate | fmShareDenyWrite);}
	catch(EFCreateError & e) {
		AnsiString msg = AnsiString::LoadStr(FBASE_ERRORCREATING);
		ATTO((msg+pathname).c_str()); Abort();
	}
	return file;
}
//---------------------------------------------------------------------------

void __fastcall TFbase1::deletefile(AnsiString pathname) {
	// note: cannot revert DeleteFile. not
	// a problem in auto_save mode though.
	FileSetReadOnly(pathname,false);
	if(DeleteFile(pathname) == false) {
		AnsiString msg = AnsiString::LoadStr(FBASE_ERRORDELETING);
		ATTO((msg+pathname).c_str()); Abort();
	}
}
//---------------------------------------------------------------------------

void __fastcall TFbase1::renamefile(AnsiString source,AnsiString destination,
	bool show_message) {
	if(RenameFile(source,destination) == false) {
		if(show_message) {
			AnsiString msg = AnsiString::LoadStr(FBASE_ERRORRENAMING);
			ATTO((msg+source).c_str());
		} Abort();
	}
}
//---------------------------------------------------------------------------

void __fastcall TFbase1::makedir(AnsiString pathname) {
	if(DirectoryExists(pathname) == false && CreateDir(pathname) == false) {
		if(DirectoryExists(pathname) == false) { // simultanious creation
			AnsiString msg = AnsiString::LoadStr(FBASE_ERRORCREATING);
			ATTO((msg+pathname).c_str()); Abort();
		}
	}
}
//---------------------------------------------------------------------------

class TFileStreamProgress : public TFileStream {
	public:
		TProgressBar * progressbar; int bytes; int io_buffer;
		__fastcall TFileStreamProgress(AnsiString FileName,Word Mode,
			TProgressBar * ProgressBar,int size) : TFileStream(FileName,Mode) {
			progressbar = ProgressBar; bytes = size; io_buffer = 0;
			progressbar->Position = 0; progressbar->Visible = true;
		}
		__fastcall ~TFileStreamProgress(void) {progressbar->Visible = false;}
	private:
		int __fastcall Write(const void *Buffer,int Count) {
			int bytes_written = TFileStream::Write(Buffer,Count);
			if(bytes_written) {
				if(io_buffer == 0) {
					io_buffer = bytes_written;
					progressbar->Max = bytes/io_buffer;
				}
				progressbar->Position = Size/io_buffer;
			}
			return bytes_written;
		}
};
//---------------------------------------------------------------------------

void __fastcall TFbase1::copyfile(AnsiString source,AnsiString destination,
	TProgressBar * progressbar,bool writelock_source) {
	TFileStream * in = NULL; TFileStream * out = NULL;
	try {
		// open in before out to prevent creating out for nothing
		try {
			if(writelock_source) {
				in = new TFileStream(source,fmOpenRead | fmShareDenyWrite);
			} else {
				in = new TFileStream(source,fmOpenRead | fmShareDenyNone);
			}
		} catch(EFOpenError & e) {
			if(writelock_source) {
				AnsiString msg = AnsiString::LoadStr(FBASE_ERRORLOCKING);
				ATTO((msg+source).c_str()); Abort();
			} else {
				AnsiString msg = AnsiString::LoadStr(FBASE_ERROROPENING);
				ATTO((msg+source).c_str()); Abort();
			}
		}
		try {
			if(progressbar) {
				out = new TFileStreamProgress(destination,
					fmCreate | fmShareDenyWrite,progressbar,
					in->Size);
			} else {
				out = new TFileStream(destination,
					fmCreate | fmShareDenyWrite);
			}
		} catch(EFCreateError & e) {
			AnsiString msg = AnsiString::LoadStr(FBASE_ERRORCREATING);
			ATTO((msg+destination).c_str()); Abort();
		}
		try {
			out->CopyFrom(in,0);
		} catch(EReadError & e) {
			AnsiString msg = AnsiString::LoadStr(FBASE_ERRORREADING);
			ATTO((msg+source).c_str()); Abort();
		} catch(EWriteError & e) {
			AnsiString msg = AnsiString::LoadStr(FBASE_ERRORWRITING);
			ATTO((msg+destination).c_str()); Abort();
		}
	} __finally {delete in; delete out;}
}
//---------------------------------------------------------------------------

AnsiString __fastcall TFbase1::normalizePath(AnsiString path) {
	//  foo -> foo, \\foo\\\bar\\ -> \\foo\bar
	AnsiString norm; int i=1;
	if(path.Length()) {norm += path[i++];}
	// support \\foo\bar for network path
	while(i <= path.Length()) {
		for(;i <= path.Length() && path[i] != '\\';i++) {norm += path[i];}
		for(;i <= path.Length() && path[i] == '\\';i++);
		if(i <= path.Length()) {norm += '\\';}
	}
	return norm;
}
//---------------------------------------------------------------------------

AnsiString __fastcall TFbase1::createDocPath(AnsiString path) {
	AnsiString doc_path(TFoptions1::o.documents_path);
	AnsiString norm; int i=1;
	if(path.Length()) {norm += path[i++];}
	while(i <= path.Length()) {
		for(;i <= path.Length() && path[i] != '\\';i++) {norm += path[i];}
		for(;i <= path.Length() && path[i] == '\\';i++);
		if(i <= path.Length()) {makedir(doc_path+"\\"+norm); norm += '\\';}
	}
	return doc_path+"\\"+norm;
}
//---------------------------------------------------------------------------

AnsiString __fastcall TFbase1::getPath(const char * environvariable) {
	AnsiString path = getenv(environvariable);
	if(path.Length()) {
		path = normalizePath(path)+"\\" PRODUCT;
		try {makedir(path);} catch(EAbort & e) {}
	} else {path = ExtractFileDir(Application->ExeName);}
	return path;
}
//---------------------------------------------------------------------------

AnsiString __fastcall TFbase1::file2tmp(AnsiString pathname) {
	// file2tmp assumes pathname contains documents_path
	int index = strlen(TFoptions1::o.documents_path)+1;
	AnsiString tmp_path = getPath("TMP")+
		pathname.SubString(index,
		pathname.Length()-index+1);
	ForceDirectories(ExtractFileDir(tmp_path));
	FileSetReadOnly(tmp_path,false);
	copyfile(pathname,tmp_path,NULL,false);
	return tmp_path;
}
//---------------------------------------------------------------------------

AnsiString __fastcall TFbase1::toFilename(AnsiString name) {
	// replace invalid windows filename chars with spaces.
	name = AnsiReplaceStr(name,"\\"," ");
	name = AnsiReplaceStr(name,"/"," ");
	name = AnsiReplaceStr(name,":"," ");
	name = AnsiReplaceStr(name,"*"," ");
	name = AnsiReplaceStr(name,"?"," ");
	name = AnsiReplaceStr(name,"\""," ");
	name = AnsiReplaceStr(name,"<"," ");
	name = AnsiReplaceStr(name,">"," ");
	name = AnsiReplaceStr(name,"|"," ");
	return name.Trim();
}
//---------------------------------------------------------------------------

void __fastcall TFbase1::exportCsvFile(TDataRows * rows) {
	LOGFUNC();
	AnsiString exp_path = getPath("TMP");
	exp_path = exp_path+"\\"+toFilename(grid.name)+".csv";
	TFileStream * file = createfile(exp_path);
	try {
		try {rows->exportSelection(file); shellExecute(exp_path);}
		catch(Exception & e) {Application->ShowException(&e);}
	} __finally {delete file;}
}
//---------------------------------------------------------------------------

void __fastcall TFbase1::restoreNetDrive(AnsiString netdrive) {
	LOGFUNC();
	if(netdrive.Length() == 2) { // X:, Y:, etc...
		UINT drivetype = GetDriveType((netdrive+"\\").c_str());
		if(drivetype == DRIVE_REMOTE || drivetype == DRIVE_NO_ROOT_DIR) {
			TRegistry * registry = Exceptions::getRegistry();
			if(registry->OpenKey(String("Network\\")+netdrive[1],false)) {
				AnsiString netpath = registry->ReadString("RemotePath").c_str();
				registry->CloseKey();
				NETRESOURCE rs = {0};
				rs.dwType = RESOURCETYPE_DISK;
				rs.lpLocalName = netdrive.c_str();
				rs.lpRemoteName = netpath.c_str();
				// When windows could not reconnect earlier, try again here.
				WNetAddConnection2(&rs,NULL,NULL,CONNECT_INTERACTIVE);
			}
		}
	}
}
//---------------------------------------------------------------------------

TDateTime __fastcall TFbase1::FileTimeToDateTime(FILETIME filetime) {
	SYSTEMTIME systemtime; TDateTime datetime;
	if(FileTimeToSystemTime(&filetime,&systemtime) == 0) {
		throw ProgException("FileTimeToSystemTime conversion error");
	}
	return SystemTimeToDateTime(systemtime);
}
//---------------------------------------------------------------------------

FILETIME __fastcall TFbase1::DateTimeToFileTime(TDateTime datetime) {
	SYSTEMTIME systemtime; FILETIME filetime;
	DateTimeToSystemTime(datetime,systemtime);
	if(SystemTimeToFileTime(&systemtime,&filetime) == 0) {
		throw ProgException("SystemTimeToFileTime conversion error");
	}
	return filetime;
}
//---------------------------------------------------------------------------

void __fastcall TFbase1::setLastWrite(AnsiString pathname,
	TDateTime lwrite_utc,bool show_message) {
	FILETIME filetime = DateTimeToFileTime(lwrite_utc);
	int file_handle = FileOpen(pathname,fmOpenWrite | fmShareDenyWrite);
	if(file_handle == -1) {
		if(show_message) {
			AnsiString msg = AnsiString::LoadStr(FBASE_ERRORLOCKING);
			ATTO((msg+pathname).c_str());
		} Abort();
	}
	try {
		if(SetFileTime((HANDLE) file_handle,NULL,NULL,&filetime) == 0) {
			if(show_message) {
				AnsiString msg = AnsiString::LoadStr(FBASE_ERRORWRITING);
				ATTO((msg+pathname).c_str());
			} Abort();
		}
	} __finally {closefile(file_handle);}
}
//---------------------------------------------------------------------------

bool __fastcall TFbase1::checkVersion(TDateTime lupdate_utc,
	BY_HANDLE_FILE_INFORMATION & info) {
	bool newer_document = false;
	TDateTime second_1(0,0,1,1); // 1s diff on ntfs, 2s on fat
	TDateTime second_2(0,0,2,1); // 1s diff on fat, 2s on ntfs
	// 1ms, see: http://qc.embarcadero.com/wc/qcmain.aspx?d=56957
	// The FAT file system rounds odd seconds upwards to the next
	// even second. Also note that if the database is updated with
	// the timestamp of the file instead of the other way around
	// (see ThreadModify), the database precision is in seconds.
	TDateTime lwrite_utc = FileTimeToDateTime(info.ftLastWriteTime);
	if(lwrite_utc > lupdate_utc && lwrite_utc-lupdate_utc > second_2){
		AnsiString msg = AnsiString::LoadStr(FBASE_FILEWASMODIFIED);
		if(ATTYN(msg.c_str()) != ID_YES) {Abort();}
		info.ftLastWriteTime = DateTimeToFileTime(lupdate_utc);
		// change info such that file_modified will be set (fix the problem)
	} else if(lupdate_utc > lwrite_utc && lupdate_utc-lwrite_utc > second_1) {
		AnsiString msg = AnsiString::LoadStr(FBASE_NEWERDOCVERSION);
		if(ATTYN(msg.c_str()) != ID_YES) {Abort();}
		newer_document = true;
	}
	return newer_document;
}
//---------------------------------------------------------------------------

int __fastcall TFbase1::doExecMacro(AnsiString macro,AnsiString params) {
	LOGFUNC();
	Variant result,object = word_app;
	TStringList * objs = new TStringList();
	TStringList * vals = new TStringList();
	objs->QuoteChar = '\''; objs->Delimiter = '.';
	vals->QuoteChar = '\''; vals->Delimiter = ',';
	BSTR arg0 = NULL; BSTR arg1 = NULL;
	try {
		objs->DelimitedText = macro;
		vals->DelimitedText = params;
		PROG_IF(objs->Count == 0,"Invalid macro name");
		PROG_IF(vals->Count > 2,"Too many parameters");
		String function = objs->Strings[objs->Count-1];
		for(int i=0;i < objs->Count-1;i++) {
			object = object.OlePropertyGet(objs->Strings[i]);
		}
		switch(vals->Count) {
			case 0:
				result = object.OleFunction(function);
			break;
			case 1:
				arg0 = StringToOleStr(vals->Strings[0]);
				result = object.OleFunction(function,arg0);
			break;
			case 2:
				arg0 = StringToOleStr(vals->Strings[0]);
				arg1 = StringToOleStr(vals->Strings[1]);
				result = object.OleFunction(function,arg0,arg1);
			break;
		}
	} __finally {
		delete objs;
		delete vals;
		SysFreeString(arg0);
		SysFreeString(arg1);
	}
	if(VarIsType(result,varInteger)) {return int(result);}
	return 0;
}
//---------------------------------------------------------------------------

void __fastcall TFbase1::openWordDoc(AnsiString pathname) {
	LOGFUNC();
	try {
		if(VarIsEmpty(word_app)) {
			word_app = CreateOleObject("Word.Application");
			word_cap = word_app.OlePropertyGet("Caption");
		}
		BSTR caption_new = NULL;
		BSTR caption_old = NULL;
		try {
			// find top word window (dialog)
			// and make really sure it shows
			AnsiString unique = IntToHex(
				(int) Application->Handle,8);
			caption_new = StringToOleStr(unique);
			word_app.OlePropertySet("Caption",caption_new);
			HWND hwnd = FindWindow(NULL,unique.c_str());
			SetWindowPos(hwnd,HWND_TOPMOST,0,0,0,0,
				SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
			SetForegroundWindow(GetLastActivePopup(hwnd));
			caption_old = StringToOleStr(word_cap);
			word_app.OlePropertySet("Caption",caption_old);
		} __finally {
			SysFreeString(caption_new);
			SysFreeString(caption_old);
		}
		doExecMacro("Documents.Open",pathname);
		// If the user switches to another application while the
		// document is -opening-, word may still be pushed to the
		// background. If this happens and the open macro shows a
		// message, it may be hidden forever waiting for user input.
		// This is probably fixed now by setting the TOPMOST style.
	} catch(Exception & e) {cleanWordApp(); throw;}
}
//---------------------------------------------------------------------------

void __fastcall TFbase1::closeWordDoc(void) {
	LOGFUNC();
	if(VarIsEmpty(word_app)) {return;}
	try {
		// disabled: Currently this functionality is not
		// needed. The code polls word before proceeding.
		// It would be better to wait for a user defined
		// message that the ActiveDocument sends when it
		// closes. This introduces a dependency however.
		// if(word_app.OlePropertyGet("Visible")) {
		//	do {eatMessages(); Sleep(100);}
		//	while(word_app.OlePropertyGet("Visible"));
		//	cleanWordApp();
		// } else {doExecMacro("ActiveDocument.Close");}
		doExecMacro("ActiveDocument.Close");
		// Reset Application.Visible if it was set by
		// the macro. In this way, the next macro run
		// will start with word being invisible again.
		word_app.OlePropertySet("Visible",false);
		SetForegroundWindow(GetActiveHandle());
	} catch(Exception & e) {cleanWordApp();}
}
//---------------------------------------------------------------------------

void __fastcall TFbase1::cleanWordApp(void) {
	LOGFUNC();
	if(VarIsEmpty(word_app)) {return;}
	TFbase1 * form = showPanel(FBASE_TERMINATINGWORD);
	try {
		doExecMacro("Application.Quit");
		SetForegroundWindow(GetActiveHandle());
		closePanel(form);
	} catch(Exception & e) {closePanel(form);}
	word_app.Clear();
}
//---------------------------------------------------------------------------

void __fastcall TFbase1::sendMail(AnsiString to,AnsiString body,
	AnsiString subject,AnsiString pathname) {
	LOGFUNC();
	if(to == POSTWAREMAIL) {
#ifdef MS_SQL
		subject += " (ms_sql)";
#elif defined INNODB
		subject += " (innodb)";
#elif defined ACCESS
		subject += " (access)";
#endif
		body = Exceptions::version+"\n"+body;
	}
	AnsiString curdir = GetCurrentDir();
	// Outlook messes up the current directory
	HINSTANCE inst = LoadLibrary("mapi32.dll");
	ShowWindow(Application->Handle,SW_SHOW);
	// Some mapi clients require a window that runs a
	// message pump in order to respond to user input.
	// The supplied window must be shown so the client
	// can return focus to it when sendmail completes.
	TFbase1 * form = showPanel(FBASE_SENDINGMAILMESSAGE);
	try {
		// LPMAPILOGON myMAPILogon;
		// LPMAPILOGOFF myMAPILogoff;
		LPMAPISENDMAIL myMAPISendMail;
		// myMAPILogon = (LPMAPILOGON) GetProcAddress(inst,"MAPILogon");
		// myMAPILogoff = (LPMAPILOGOFF) GetProcAddress(inst,"MAPILogoff");
		myMAPISendMail = (LPMAPISENDMAIL) GetProcAddress(inst,"MAPISendMail");
		if(myMAPISendMail) {
			ULONG_PTR mapi_session = NULL;
			// disabled: support 64 bit mail clients called
			// from a 32 bit capplan version or vice verca.
			// if(myMAPILogon) {
			//	if(myMAPILogon((ULONG) Application->Handle,NULL,NULL,
			//		0L,0L,&mapi_session) != SUCCESS_SUCCESS) {
			//		// If MAPI hasn't been setup, try to do so here.
			//		// If this fails just try to run MAPISendMail.
			//		myMAPILogon((ULONG) Application->Handle,NULL,NULL,
			//			MAPI_LOGON_UI,0L,&mapi_session);
			//	}
			// }
			MapiRecipDesc mapi_recipient;
			mapi_recipient.ulReserved = 0L;
			mapi_recipient.ulRecipClass = MAPI_TO;
			mapi_recipient.lpszName = to.c_str();
			mapi_recipient.lpszAddress = NULL;
			mapi_recipient.ulEIDSize = 0L;
			mapi_recipient.lpEntryID = NULL;

			unsigned long filecount = 0L;
			MapiFileDesc * attachment = NULL;
			if(FileExists(pathname)) {
				MapiFileDesc mapi_attachment;
				mapi_attachment.ulReserved = 0L;
				mapi_attachment.flFlags = 0L;
				mapi_attachment.nPosition = -1L;
				mapi_attachment.lpszPathName = pathname.c_str();
				mapi_attachment.lpszFileName = NULL;
				mapi_attachment.lpFileType = NULL;
				filecount = 1L; attachment = &mapi_attachment;
			}

			MapiMessage mapi_message = {0};
			mapi_message.ulReserved = 0L;
			mapi_message.lpszSubject = subject.c_str();
			mapi_message.lpszNoteText = body.c_str();
			mapi_message.lpszMessageType = NULL;
			mapi_message.lpszDateReceived = NULL;
			mapi_message.lpszConversationID = NULL;
			mapi_message.flFlags = 0L;
			mapi_message.lpOriginator = NULL;
			mapi_message.nRecipCount = 1L;
			mapi_message.lpRecips = &mapi_recipient;
			mapi_message.nFileCount	= filecount;
			mapi_message.lpFiles = attachment;

			if(myMAPISendMail(mapi_session,(ULONG) Application->Handle,
				&mapi_message,MAPI_DIALOG,0L) != SUCCESS_SUCCESS) {
			}
			// if(myMAPILogoff && mapi_session) {
			//	myMAPILogoff(mapi_session,(ULONG) Application->Handle,0L,0L);
			// }
		}
	} __finally {
		closePanel(form);
		ShowWindow(Application->Handle,SW_HIDE);
		FreeLibrary(inst); SetCurrentDir(curdir);
	}
}
//---------------------------------------------------------------------------

