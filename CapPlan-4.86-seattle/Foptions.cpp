//$Id: Foptions.cpp,v 1.124 2013/12/21 20:00:16 darseq Exp $
#include <vcl.h>
#pragma hdrstop
#include <FileCtrl.hpp>
#include "Foptions.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Fbase"
#pragma link "FixedCheckBox"
#pragma resource "*.dfm"
TFoptions1 *Foptions1;
bool TFoptions1::internal_call = false;
FILE * TFoptions1::cfgfile = NULL;
id_t TFoptions1::opened_id = UNDEFINED;
// The options class really should be rewritten. Currently it is
// like this in order to stay compatible with existing cfg files.
// todo: use separate files for global and local options: cfg,cfl
// this means there will be two public option structures (weird?)
// todo: change cfg format: option=value. Use registry instead?
id_t TFoptions1::default_id;
bool TFoptions1::default_remember;
bool TFoptions1::default_week_mode;
bool TFoptions1::default_auto_search;
bool TFoptions1::default_clear_after;
bool TFoptions1::default_clear_onnew;
bool TFoptions1::default_auto_refresh;
bool TFoptions1::default_schedule_doc;
bool TFoptions1::default_unsigned_red;
bool TFoptions1::default_signed_sched;
bool TFoptions1::default_run_macros;
bool TFoptions1::default_repurchase;
bool TFoptions1::default_invoice;
char TFoptions1::default_page_nr;
bool TFoptions1::auto_save = true;
bool TFoptions1::review_tab = false;
char TFoptions1::default_docs_path[MAX_PATH];
char TFoptions1::default_tmpl_path[MAX_PATH];
char TFoptions1::default_postcode_format[MAXPOSTCODE+1];
int TFoptions1::default_telephone_length;
int TFoptions1::default_insert_color;
int TFoptions1::default_copies;
int TFoptions1::default_font;
int TFoptions1::default_time;
options_t TFoptions1::o;
//---------------------------------------------------------------------------

__fastcall TFoptions1::TFoptions1(TComponent* Owner) : TFbase1(Owner) {
	Color = clBtnFace; PostcodeEdit->MaxLength = MAXPOSTCODE;
	// modified = false; by Fbase1
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::FormShow(TObject *Sender) {
	ActiveControl = RowsEdit;
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::addColorBoxItem(const Code & code) {
	ColorBox->AddItem(code.name,(TObject *) StrToInt(code.color));
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::refreshColorBoxItems(void) {
	ColorBox->Clear();
	Codes::getCodeValues(&addColorBoxItem);
	ColorBox->Selected = TColor(default_insert_color);
	if((unsigned) ColorBox->Selected == 0xFF000000) {
		o.insert_color = default_insert_color = 0;
		// Avoid inserting entries with invalid codes
		// by using the zero color instead. Note that
		// this temporary change becomes permanent if
		// some other options are changed and saved.
		ColorBox->Selected = TColor(o.insert_color);
		if((unsigned) ColorBox->Selected == 0xFF000000) {
			Code code; // The zero color must be added
			code.name = IntToHex(o.insert_color,6);
			code.color = IntToStr(o.insert_color);
			addColorBoxItem(code);
			ColorBox->Selected = TColor(o.insert_color);
		}
	} ColorBox->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::ColorBoxChange(TObject *Sender) {
	EditChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::EditChange(TObject *Sender) {
	if(Active) {modified = true;} // Edit Text is updated by view
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::ViewChange(TObject *Sender) {
	if(Active) {modified = true; refresh = true;}
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::view(int pending_page_index) {
	LOGFUNC();
	page_index = pending_page_index;
	ClearAfterBox->Checked = default_clear_after;
	ClearOnNewBox->Checked = default_clear_onnew;
	RememberBox->Checked = default_remember;
	WeekBox->Checked = default_week_mode;
	SearchBox->Checked = default_auto_search;
	RefreshBox->Checked = default_auto_refresh;
	AlternateBox->Checked = o.week_colors;
	InfoBox->Checked = o.entry_info;
	TimesBox->Checked = o.print_times;
	VerticalBox->Checked = o.vertical_rows;
	RestrictBox->Checked = o.restrict_updates;
	PatternBox->Checked = o.hide_patterns;
	ScheduleBox->Checked = default_schedule_doc;
	UnsignedBox->Checked = default_unsigned_red;
	MacroBox->Checked = default_run_macros;
	SignedBox->Checked = default_signed_sched;
	LockBox->Checked = default_page_nr != 0;
	LockBox->Enabled = page_index != -1 || LockBox->Checked;
	CurrentBox->Checked = opened_id == default_id;
	CurrentBox->Enabled = opened_id != UNDEFINED &&
		CurrentBox->Checked == false;
	RepurchaseBox->Checked = default_repurchase;
	InvoiceBox->Checked = default_invoice;

	RowsEdit->Text = o.rows;
	DaysEdit->Text = o.days;
	LabelsEdit->Text = o.labels;
	CopiesEdit->Text = default_copies;
	FontEdit->Text = default_font;
	TimeEdit->Text = default_time;
	TelephoneEdit->Text = default_telephone_length;
	PostcodeEdit->Text = default_postcode_format;
	refresh = false; ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::ClearAfterBoxClick(TObject *Sender) {
	o.clear_after = default_clear_after = ClearAfterBox->Checked;
	modified = true;
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::ClearOnNewBoxClick(TObject *Sender) {
	o.clear_onnew = default_clear_onnew = ClearOnNewBox->Checked;
	modified = true;
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::RememberBoxClick(TObject *Sender) {
	o.remember = default_remember = RememberBox->Checked;
	modified = true;
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::WeekBoxClick(TObject *Sender) {
	o.week_mode = default_week_mode = WeekBox->Checked;
	modified = true;
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::SearchBoxClick(TObject *Sender) {
	o.auto_search = default_auto_search = SearchBox->Checked;
	modified = true;
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::RefreshBoxClick(TObject *Sender) {
	o.auto_refresh = default_auto_refresh = RefreshBox->Checked;
	modified = true;
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::AlternateBoxClick(TObject *Sender) {
	o.week_colors = AlternateBox->Checked;
	modified = true; refresh = true;
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::InfoBoxClick(TObject *Sender) {
	o.entry_info = InfoBox->Checked;
	modified = true; refresh = true;
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::TimesBoxClick(TObject *Sender) {
	o.print_times = TimesBox->Checked; modified = true;
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::VerticalBoxClick(TObject *Sender) {
	o.vertical_rows = VerticalBox->Checked; modified = true;
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::RestrictBoxClick(TObject *Sender) {
	o.restrict_updates = RestrictBox->Checked; modified = true;
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::PatternBoxClick(TObject *Sender) {
	o.hide_patterns = PatternBox->Checked;
	modified = true; refresh = true;
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::MacroBoxClick(TObject *Sender) {
	o.run_macros = default_run_macros = MacroBox->Checked;
	modified = true;
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::SignedBoxClick(TObject *Sender) {
	o.signed_sched = default_signed_sched = SignedBox->Checked;
	modified = true;
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::UnsignedBoxClick(TObject *Sender) {
	o.unsigned_red = default_unsigned_red = UnsignedBox->Checked;
	modified = true;
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::RepurchaseBoxClick(TObject *Sender) {
	o.repurchase = default_repurchase = RepurchaseBox->Checked;
	modified = true;
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::ScheduleBoxClick(TObject *Sender) {
	o.schedule_doc = default_schedule_doc = ScheduleBox->Checked;
	modified = true;
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::LockBoxClick(TObject *Sender) {
	char page_nr = LockBox->Checked ? page_index+1 : 0;
	o.page_nr = default_page_nr = page_nr;
	modified = true; LockBox->Enabled = page_index != -1;
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::InvoiceBoxClick(TObject *Sender) {
	o.invoice = default_invoice = InvoiceBox->Checked;
	modified = true;
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::CurrentBoxClick(TObject *Sender) {
	default_id = opened_id; o.default_id = StrToInt(default_id);
	modified = true; CurrentBox->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::RowsEditExit(TObject *Sender) {
	try {
		int rows = StrToInt(RowsEdit->Text);
		if(rows < 0) {rows = o.rows;}
		RowsEdit->Text = o.rows = rows;
		// modified set by EditChange()
	} catch(EConvertError & e) {RowsEdit->Text = o.rows;}
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::DaysEditExit(TObject *Sender) {
	try {
		int days = StrToInt(DaysEdit->Text);
		if(days <= 0) {days = o.days;}
		DaysEdit->Text = o.days = days;
	} catch(EConvertError & e) {DaysEdit->Text = o.days;}
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::LabelsEditExit(TObject *Sender) {
	try {
		int labels = StrToInt(LabelsEdit->Text);
		if(labels <= 0) {labels = o.labels;}
		LabelsEdit->Text = o.labels = labels;
	} catch(EConvertError & e) {LabelsEdit->Text = o.labels;}
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::CopiesEditExit(TObject *Sender) {
	try {
		int copies = StrToInt(CopiesEdit->Text);
		if(copies < 0) {copies = o.copies;}
		CopiesEdit->Text = o.copies = default_copies = copies;
	} catch(EConvertError & e) {CopiesEdit->Text = o.copies;}
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::FontEditExit(TObject *Sender) {
	try {
		int font = StrToInt(FontEdit->Text);
		if(font <= 0) {font = o.font;}
		FontEdit->Text = o.font = default_font = font;
	} catch(EConvertError & e) {FontEdit->Text = o.font;}
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::TimeEditExit(TObject *Sender) {
	try {
		int time = StrToInt(TimeEdit->Text);
		if(time < 0) {time = o.time;}
		TimeEdit->Text = o.time = default_time = time;
	} catch(EConvertError & e) {TimeEdit->Text = o.time;}
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::ColorBoxExit(TObject *Sender) {
	if((unsigned) ColorBox->Selected != 0xFF000000) {
		o.insert_color = default_insert_color = ColorBox->Selected;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::TelephoneEditExit(TObject *Sender) {
	try {
		int length = StrToInt(TelephoneEdit->Text);
		if(length < 0 || length > MAXTELEPHONE) {length = o.telephone_length;}
		o.telephone_length = default_telephone_length = length;
		TelephoneEdit->Text = o.telephone_length;
	} catch(EConvertError & e) {TelephoneEdit->Text = o.telephone_length;}
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::PostcodeEditExit(TObject *Sender) {
	AnsiString text = PostcodeEdit->Text.Trim();
	strncpy(default_postcode_format,text.c_str(),MAXPOSTCODE);
	strncpy(o.postcode_format,text.c_str(),MAXPOSTCODE);
	default_postcode_format[MAXPOSTCODE] = '\0';
	o.postcode_format[MAXPOSTCODE] = '\0';
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::DocumentsPath1Click(TObject *Sender) {
	AnsiString dir = o.documents_path; dir += "\\";
	UnicodeString unicode_dir = UnicodeString(dir);
#if __BORLANDC__ <= 0x582
	if(SelectDirectory(o.documents_path,WideString(),unicode_dir)) {
#else
	if(SelectDirectory(UnicodeString(o.documents_path),
		WideString(),unicode_dir,TSelectDirExtOpts(),this)) {
#endif
		strncpy(default_docs_path,normalizePath(unicode_dir).c_str(),MAX_PATH);
		strncpy(o.documents_path,normalizePath(unicode_dir).c_str(),MAX_PATH);
		modified = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::TemplatesPath1Click(TObject *Sender) {
	AnsiString dir = o.templates_path; dir += "\\";
	UnicodeString unicode_dir = UnicodeString(dir);
#if __BORLANDC__ <= 0x582
	if(SelectDirectory(o.templates_path,WideString(),unicode_dir)) {
#else
	if(SelectDirectory(UnicodeString(o.templates_path),
		WideString(),unicode_dir,TSelectDirExtOpts(),this)) {
#endif
		strncpy(default_tmpl_path,normalizePath(unicode_dir).c_str(),MAX_PATH);
		strncpy(o.templates_path,normalizePath(unicode_dir).c_str(),MAX_PATH);
		modified = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::MakeConfig(AnsiString sch) {
	LOGFUNC();
	o.charset = 0; // magic values
	strncpy(o.name,"Verdana",NAMESIZE); o.name[NAMESIZE] = '\0';
	o.color = 0x80000008; o.size = 9; o.pitch = 0;
	o.style = TFontStyles(); o.rows = 0; o.days = 7; o.labels = 1;
	o.clear_after = true; o.clear_onnew = false; o.remember = false;
	o.week_mode = true; o.auto_search = true; o.auto_refresh = false;
	o.week_colors = false; o.entry_info = false; o.print_times = true;
	o.vertical_rows = false; o.restrict_updates = false;
	o.signed_sched = false; o.unsigned_red = false;
	o.invoice = false; o.page_nr = 0; o.schedule_doc = false;
	o.repurchase = false; o.copies = 1; o.insert_color = 0;
	o.telephone_length = 10; o.font = 9; o.time = 0;
	o.hide_patterns = false; o.run_macros = false;
	strncpy(o.postcode_format,"ccccaa",MAXPOSTCODE);
	o.postcode_format[MAXPOSTCODE] = '\0';
	AnsiString exe = ExtractFileDir(Application->ExeName);
	strncpy(o.documents_path,(exe+"\\" DOCUMENTPATH).c_str(),MAX_PATH);
	strncpy(o.templates_path,(exe+"\\" TEMPLATEPATH).c_str(),MAX_PATH);
	strncpy(o.reserved_string,"CapPlan (c) by s.b.m. post",NAMESIZE);
	o.reserved_string[NAMESIZE] = '\0'; o.default_id = 0; // set by openConfig

	FILE * schfile;
	if((schfile = fopen(sch.c_str(),"wb")) == NULL) {
		throw ProgException(AnsiString::LoadStr(FBASE_ERRORCREATING)+sch);
	}
	fseek(schfile,0,SEEK_SET);
	fwrite((void *) &o,sizeof(options_t),1,schfile);
	fflush(schfile); fclose(schfile);
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::openConfig(id_t schedule_id) {
	LOGFUNC();
	AnsiString appdata = getPath("APPDATA");
	AnsiString cfg = appdata+"\\" PRODUCT+".cfg"; AnsiString sch = cfg;
	if(schedule_id != UNDEFINED) {sch = ChangeFileExt(sch,"."+schedule_id);}
	if(internal_call == false && FileExists(sch) == false) {MakeConfig(sch);}

	FILE * tmpfile;
	if((tmpfile = fopen(sch.c_str(),"r+b")) == NULL) {
		if(internal_call) {internal_call = false; return;}
		throw ProgException(AnsiString::LoadStr(FBASE_ERROROPENING)+sch);
	}
	internal_call = false; closeConfig();
	cfgfile = tmpfile; fseek(cfgfile,0,SEEK_SET);
	if(fread((void *) &o,sizeof(options_t),1,cfgfile) != 1) {
		// todo: reactivate this code after all clients have been upgraded
		// throw ProgException(AnsiString::LoadStr(FBASE_INVALIDFILE)+sch);
		// the user can delete the invalid file to recreate it.
	}

	opened_id = schedule_id;
	if(opened_id == UNDEFINED) {
		default_id = IntToStr(o.default_id);
		default_remember = o.remember;
		default_week_mode = o.week_mode;
		default_auto_search = o.auto_search;
		default_clear_after = o.clear_after;
		default_clear_onnew = o.clear_onnew;
		default_auto_refresh = o.auto_refresh;
		default_schedule_doc = o.schedule_doc;
		default_unsigned_red = o.unsigned_red;
		default_signed_sched = o.signed_sched;
		default_run_macros = o.run_macros;
		default_repurchase = o.repurchase;
		default_invoice = o.invoice;
		default_page_nr = o.page_nr;
		default_copies = o.copies;
		default_font = o.font;
		default_time = o.time;
		default_insert_color = o.insert_color;
		default_telephone_length = o.telephone_length;
		strncpy(default_postcode_format,o.postcode_format,MAXPOSTCODE+1);
		strncpy(default_docs_path,o.documents_path,MAX_PATH);
		strncpy(default_tmpl_path,o.templates_path,MAX_PATH);
		restoreNetDrive(ExtractFileDrive(default_docs_path));
		restoreNetDrive(ExtractFileDrive(default_tmpl_path));
	} else {
		o.default_id = StrToInt(default_id);
		o.remember = default_remember;
		o.week_mode = default_week_mode;
		o.auto_search = default_auto_search;
		o.clear_after = default_clear_after;
		o.clear_onnew = default_clear_onnew;
		o.auto_refresh = default_auto_refresh;
		o.schedule_doc = default_schedule_doc;
		o.unsigned_red = default_unsigned_red;
		o.signed_sched = default_signed_sched;
		o.run_macros = default_run_macros;
		o.repurchase = default_repurchase;
		o.invoice = default_invoice;
		o.page_nr = default_page_nr;
		o.copies = default_copies;
		o.font = default_font;
		o.time = default_time;
		o.insert_color = default_insert_color;
		o.telephone_length = default_telephone_length;
		strncpy(o.postcode_format,default_postcode_format,MAXPOSTCODE+1);
		strncpy(o.documents_path,default_docs_path,MAX_PATH);
		strncpy(o.templates_path,default_tmpl_path,MAX_PATH);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::saveConfig(void) {
	LOGFUNC();
	fseek(cfgfile,0,SEEK_SET);
	fwrite((void *) &o,sizeof(options_t),1,cfgfile);
	fflush(cfgfile);

	id_t id = default_id;
	bool remember = default_remember;
	bool week_mode = default_week_mode;
	bool auto_search = default_auto_search;
	bool clear_after = default_clear_after;
	bool clear_onnew = default_clear_onnew;
	bool auto_refresh = default_auto_refresh;
	bool schedule_doc = default_schedule_doc;
	bool unsigned_red = default_unsigned_red;
	bool signed_sched = default_signed_sched;
	bool run_macros = default_run_macros;
	bool repurchase = default_repurchase;
	bool invoice = default_invoice;
	char page_nr = default_page_nr;
	int copies = default_copies;
	int font = default_font;
	int time = default_time;
	int insert_color = default_insert_color;
	int telephone_length = default_telephone_length;
	char postcode_format[MAXPOSTCODE+1];
	char docs_path[MAX_PATH],tmpl_path[MAX_PATH];
	strncpy(postcode_format,default_postcode_format,MAXPOSTCODE+1);
	strncpy(docs_path,default_docs_path,MAX_PATH);
	strncpy(tmpl_path,default_tmpl_path,MAX_PATH);
	id_t reopen_id = opened_id;

	OpenConfig(UNDEFINED);

	default_id = id;
	o.default_id = StrToInt(default_id);
	o.remember = default_remember = remember;
	o.week_mode = default_week_mode = week_mode;
	o.auto_search = default_auto_search = auto_search;
	o.clear_after = default_clear_after = clear_after;
	o.clear_onnew = default_clear_onnew = clear_onnew;
	o.auto_refresh = default_auto_refresh = auto_refresh;
	o.schedule_doc = default_schedule_doc = schedule_doc;
	o.unsigned_red = default_unsigned_red = unsigned_red;
	o.signed_sched = default_signed_sched = signed_sched;
	o.run_macros = default_run_macros = run_macros;
	o.repurchase = default_repurchase = repurchase;
	o.invoice = default_invoice = invoice;
	o.page_nr = default_page_nr = page_nr;
	o.copies = default_copies = copies;
	o.font = default_font = font;
	o.time = default_time = time;
	o.insert_color = default_insert_color = insert_color;
	o.telephone_length = default_telephone_length = telephone_length;
	strncpy(default_postcode_format,postcode_format,MAXPOSTCODE+1);
	strncpy(default_docs_path,docs_path,MAX_PATH);
	strncpy(o.documents_path,docs_path,MAX_PATH);
	strncpy(default_tmpl_path,tmpl_path,MAX_PATH);
	strncpy(o.templates_path,tmpl_path,MAX_PATH);

	fseek(cfgfile,0,SEEK_SET);
	fwrite((void *) &o,sizeof(options_t),1,cfgfile);
	fflush(cfgfile);
	OpenConfig(reopen_id);
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::closeConfig(void) {
	LOGFUNC();
	if(cfgfile != NULL) {fclose(cfgfile); cfgfile = NULL;}
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::Entryfont1Click(TObject *Sender) {
	LOGFUNC();
	ActiveControl = exitFirst(Sender); // refreshForms follows
	FontDialog1->Options >> fdApplyButton;
	TFont * font = FontDialog1->Font;
	font->Charset = o.charset;
	font->Name = o.name;
	font->Color = TColor(o.color);
	font->Size = o.size;
	font->Pitch = TFontPitch(o.pitch);
	font->Style = o.style;
	bool result;
#if __BORLANDC__ >= 0x582
	Application->ModalPopupMode = pmExplicit;
	try {result = FontDialog1->Execute(Handle);}
	__finally {Application->ModalPopupMode = pmNone;}
#else
	result = FontDialog1->Execute();
#endif
	if(result) {
		o.charset = font->Charset;
		AnsiString name = font->Name;
		strncpy(o.name,name.c_str(),NAMESIZE);
		o.name[NAMESIZE] = '\0';
		o.color = font->Color;
		o.size = font->Size;
		o.pitch = (int) font->Pitch;
		o.style = font->Style;
		modified = true;
		refresh = false;
		refreshForms();
	}
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::Refresh1Click(TObject *Sender) {
	LOGFUNC();
	ActiveControl = exitFirst(Sender);
	refresh = false; refreshForms();
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::save1Click(TObject *Sender) {
	ActiveControl = exitFirst(Sender);
	saveConfig(); modified = false;
}
//---------------------------------------------------------------------------

void __fastcall TFoptions1::FormClose(TObject *Sender,TCloseAction &Action) {
	LOGFUNC();
	TFbase1::FormClose(Sender,Action);
	// ActiveControl remains NULL: no OnExit
	if(refresh) {refresh = false; refreshForms();}
}
//---------------------------------------------------------------------------

