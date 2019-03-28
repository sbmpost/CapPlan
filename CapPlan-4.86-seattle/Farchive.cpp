//$Id: Farchive.cpp,v 1.236 1998/01/22 19:37:31 darseq Exp $
#include <vcl.h>
#pragma hdrstop
#include <dateutils.hpp>
#include <strutils.hpp>
#include "Farchive.h"
#include "Foptions.h"
#include "Fdetails.h"
#include "Fpending.h"
#include "Fcreated.h"
#include "Fentries.h"
#include "Fschedule.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Fbase"
#pragma link "Fmdetail"
#pragma link "FixedCheckBox"
#pragma link "DataRows"
#pragma link "RowControl"
#pragma resource "*.dfm"
TFarchive1 *Farchive1;
//---------------------------------------------------------------------------
// todo: hints for buttons
__fastcall TFarchive1::TFarchive1(TComponent* Owner) : TFmdetail1(Owner) {
	db->link(&persons); db->link(&apps);
	db->link(&docs); db->link(&scheds);
	db->link(&employs); db->link(&entries);

	// docs.progressbar = ProgressBar1;
	Memo->MaxLength = MAXTEXT-1;
	Info->MaxLength = MAXTEXT-1;
	Stat->MaxLength = MAXTEXT-1;
	Note->MaxLength = MAXTEXT-1;

	Lastname->MaxLength = MAXLASTNAME;
	// Postcode length set by view()
	Address->MaxLength = MAXADDRESS;
	City->MaxLength = MAXCITY;

	ArchiveLabel = Caption; AppRows->Hint = "";
	OpenLabel = DocumentMenu->Items->Items[
		docsOpen1->MenuIndex]->Caption;
	OpenLabel = AnsiReplaceStr(OpenLabel,"&","");
	RowControl->Width = ClientWidth - RowControl->Left + 1;
	RowControl->Height = ClientHeight - RowControl->Top + 1;
	Top = (Screen->WorkAreaHeight - Height)/2;
	Left = (Screen->WorkAreaWidth - Width)/2;
	setMetaIndices();

	PersonRows->ColWidths[0] = 200;
	PersonRows->ColWidths[1] = 100;
	PersonRows->ColWidths[2] = 252;
	PersonRows->ColWidths[3] = 150;
	PersonRows->ColWidths[4] = 100;
	PersonRows->ColWidths[5] = 100;
	PersonRows->ColWidths[6] = 67;

	AppRows->ColWidths[0] = 215;
	AppRows->ColWidths[1] = 171;
	AppRows->ColWidths[2] = 171;
	AppRows->ColWidths[3] = 115;
	AppRows->ColWidths[4] = 115;
	AppRows->ColWidths[5] = 115;
	AppRows->ColWidths[6] = 67;

	DocRows->ColWidths[0] = 222;
	DocRows->ColWidths[1] = 258;
	DocRows->ColWidths[2] = 115;
	DocRows->ColWidths[3] = 115;
	DocRows->ColWidths[4] = 89;
	DocRows->ColWidths[5] = 171;

	ScheduleRows->ColWidths[0] = 133;
	ScheduleRows->ColWidths[1] = 128;
	ScheduleRows->ColWidths[2] = 128;
	ScheduleRows->ColWidths[3] = 128;
	ScheduleRows->ColWidths[4] = 128;
	ScheduleRows->ColWidths[5] = 128;
	ScheduleRows->ColWidths[6] = 128;
	ScheduleRows->ColWidths[7] = 67;
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::FormCreate(TObject *Sender) {
	setMenu(PersonMenu,per,4); setMenu(AppointmentMenu,app,4);
	setMenu(DocumentMenu,doc,5); setMenu(ScheduleMenu,sch,3);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::FormDestroy(TObject *Sender) {
	db->unlink(&persons); db->unlink(&apps);
	db->unlink(&docs); db->unlink(&scheds);
	db->unlink(&employs); db->unlink(&entries);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::refreshPopupMenuItems(void) {
	CodePopup->Items->Clear(); // also deletes items.
	Codes::getCodeValues(addPopupMenuItem);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::checkOwner(int meta) {
	if(root_user == false && getData(meta) != db->getUsername()) {
		throw UserException(ITEMSACCESSDENIED);
	}
}
//---------------------------------------------------------------------------

TModalResult __fastcall TFarchive1::view(id_t & person_id,id_t & appointment_id,
	id_t & document_id,TTabSheet * show_sheet,int & appointment_entries) {
	LOGFUNC();

	Lastname->OnChange = NULL; Lastname->Text = "";
	Postcode->OnChange = NULL; Postcode->Text = "";
	Address->OnChange = NULL; Address->Text = "";
	City->OnChange = NULL; City->Text = "";

	UnicodeString postcode_mask;
	PersonRowsGetEditMask(this,POSTCODE,0,postcode_mask);
	Postcode->EditMask = postcode_mask; // when empty, clear editmask
	if(Postcode->EditMask.IsEmpty()) {Postcode->MaxLength = MAXPOSTCODE;}

	Lastname->OnChange = SearchChange;
	Postcode->OnChange = SearchChange;
	Address->OnChange = SearchChange;
	City->OnChange = SearchChange;

	app_entries = 0;
	set_refresh = true;
	refresh_info = false;
	CancelButton->Enabled = true;
	EmployeesBox->Checked = false;
	// when tabsheet is APPS/DOCS/SCHEDULES,
	// person_id should match the person
	ids[per] = person_id; ids[app] = appointment_id;
	ids[doc] = document_id; ids[sch] = grid.id;
	TModalResult result = TFmdetail1::view(show_sheet);
	// keep schedule_id when mrIgnore, Refresh() can handle invalid sch_ids.
	person_id = ids[per]; appointment_id = ids[app]; document_id = ids[doc];
	appointment_entries = app_entries;
	return result;
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::PERSONSShow(TObject *Sender) {
	LOGFUNC();
	TWinControl * active_control = Lastname;
	try {
		Caption = ArchiveLabel;
		AnsiString lastname = Lastname->Text.Trim();
		AnsiString postcode = Postcode->Text.Trim();
		AnsiString address = Address->Text.Trim();
		AnsiString city = City->Text.Trim();
		// when the user double clicks an entry we need ids[per]
		if(ids[per] != UNDEFINED && lastname.IsEmpty()) {
			if(postcode.IsEmpty()) {
				if(address.IsEmpty()) {
					if(city.IsEmpty()) {
						Lastname->OnChange = NULL;
						Lastname->Text = persons.getText(ids[per],"LASTNAME");
						if(Lastname->Text.IsEmpty()) {
							Postcode->OnChange = NULL;
							Postcode->Text = persons.getText(
								ids[per],"POSTCODE");
							if(Postcode->Text.Length()) {
								active_control = Postcode;
							}
						}
					} else {active_control = City;}
				} else {active_control = Address;}
			} else {active_control = Postcode;}
		}
	} catch(Exception & e) {Exceptions::handleException(Sender,&e);}
	// make sure OnChange is properly set when an exception occured.
	Lastname->OnChange = SearchChange; Postcode->OnChange = SearchChange;
	// PageShow, refreshRows depends on Lastname/Postcode/Address/City values
	ActiveControl = active_control; ScheduleButton->Visible = false;
	TFmdetail1::PageShow(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::APPSShow(TObject *Sender) {
	LOGFUNC();
	try {
		// should we check for a hidden person_id here?
		// probably not because whenever a person is scheduled,
		// the user implicitely says the person is not hidden.
		// Note that getCaption does not lock whatsoever.
		AnsiString caption = persons.getCaption(ids[per]);
		Caption = ArchiveLabel+": "+caption;
		// create caption in two passes because the compiler does
		// not properly handle exceptions while adding strings.
		if(ids[app] != UNDEFINED) {apps.check(ids[app]);} // does not lock
		// MS_SQL: check() to inform the user when an entry was doubled clicked
		// and the appointment was removed by someone else. INNODB: when there
		// was an entry conflict, a double clicked appointment may not exist yet
		// (in the current snapshot). MS_SQL/INNODB:refreshApps will not display
		// the appointment but now the user knows why.
	} catch(Exception & e) {Exceptions::handleException(Sender,&e);}
	ActiveControl = AppRows;
	ScheduleButton->Visible = Fpending1->Visible == false &&
		Fcreated1->Visible == false && Fentries1->Visible == false;
	TFmdetail1::PageShow(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::DOCSShow(TObject *Sender) {
	LOGFUNC();
	// if a certain file was selected and the user jumps back to the
	// matching person and back, then inform the user if someone else
	// deleted the file in between.
	try {
		AnsiString caption = persons.getCaption(ids[per]);
		Caption = ArchiveLabel+": "+caption;
		docs.check(ids[doc]);
	} catch(Exception & e) {Exceptions::handleException(Sender,&e);}
	ActiveControl = DocRows; ScheduleButton->Visible = false;
	TFmdetail1::PageShow(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::SCHEDULESShow(TObject *Sender) {
	LOGFUNC();
	try {
		AnsiString caption = persons.getCaption(ids[per]);
		Caption = ArchiveLabel+": "+caption;
		// It is of no use to check if
		// the current grid exists here.
	} catch(Exception & e) {Exceptions::handleException(Sender,&e);}
	ActiveControl = ScheduleRows; ScheduleButton->Visible = false;
	TFmdetail1::PageShow(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::FormClose(TObject *Sender,TCloseAction &Action) {
	TFmdetail1::FormClose(Sender,Action);
	// Both mrCancel (close) and mrOk confirm the selection
	if(ModalResult != mrAbort && ModalResult != mrIgnore &&
		(ModalResult == mrAll || Fschedule1->Visible) &&
		ActiveRows == AppRows && ids[app] != UNDEFINED) {
		// Note that we chose not to lock when validating an
		// appointment. If someone else sets cancelled/exclusive
		// after the archive was opened, it will not be detected.
		// INNODB: a commit is necessary to see the change.
		apps.validate(ids[app],db->getUsername());
	}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::ScheduleButtonClick(TObject *Sender) {
	LOGFUNC();
	ActiveControl = AppRows; if(AppRows->Row == 0) {return;}
	id_t appointment_id = getId(); if(appointment_id == UNDEFINED) {return;}
	if(Fschedule1->Visible == false) {
		// If an entry is double clicked, personrows has not refreshed yet.
		Fschedule1->setPerson(ids[per],persons.getText(ids[per],"LASTNAME"));
	} // else: a document may currently be selected
	Fschedule1->setAppointment(appointment_id);
	ModalResult = mrAll; // signal all forms must be closed
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::RowControlChanging(TObject *Sender,
	bool &AllowChange, int NewPage) {
	TFmdetail1::RowControlChanging(Sender,AllowChange,NewPage);
	AllowChange = AllowChange && (NewPage == per ||
		NewPage == sch || ids[per] != UNDEFINED);
	// id is also UNDEFINED when someone else
	// deleted the row (see readRow)
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::AppRowsDataToCell(TObject *Sender, int meta,
	AnsiString &data) {
	if(data != UNDEFINED) {
		if(meta == PAID) {
			if(StrToInt(data)) {data = AnsiString::LoadStr(FARCHIVE_Y);}
			else {data = AnsiString::LoadStr(FARCHIVE_N);}
		} else if(meta == HOURS) {data = MinutesToStr(StrToInt(data));}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::DocRowsDataToCell(TObject *Sender, int meta,
	AnsiString &data) {
	if(data != UNDEFINED && (meta == SCHEDULE || meta == REJECTED)) {
		if(StrToInt(data)) {data = AnsiString::LoadStr(FARCHIVE_Y);}
		else {data = AnsiString::LoadStr(FARCHIVE_N);}
	} // database data is not quoted and therefore need not be unquoted
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::ScheduleRowsDataToCell(TObject *Sender, int meta,
	AnsiString &data) {
	if(meta == EMPLOYEE) {
		if(data != UNDEFINED) {data = AnsiString::LoadStr(FARCHIVE_Y);}
		else {data = AnsiString::LoadStr(FARCHIVE_N);}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::PersonRowsCellToData(TObject *Sender, int meta,
	  AnsiString &cell) {
	if(meta == PER_HIDDEN && cell == UNDEFINED) {cell = "0";}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::AppRowsCellToData(TObject *Sender, int meta,
	AnsiString &cell) {
	if(meta == PAID) {
		if(cell == AnsiString::LoadStr(FARCHIVE_Y)) {cell = "1";}
		else {cell = "0";}
	} else if(meta == HOURS) {
		if(cell != UNDEFINED) {cell = IntToStr(StrToMinutes(cell));}
		else {cell = "0";}
	} else if((meta == EXCLUSIVE || meta == CANCELLED || meta == ENTRIES ||
		meta == CHARGE) && cell == UNDEFINED) {cell = "0";}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::DocRowsCellToData(TObject *Sender, int meta,
	AnsiString &cell) {
	if(meta == SCHEDULE || meta == REJECTED) {
		if(cell == AnsiString::LoadStr(FARCHIVE_Y)) {cell = "1";}
		else {cell = "0";}
	} else if(meta == DOC_NAME) {cell = QuotedStr(cell);}
	else if(meta == DOC_SIZE && cell == UNDEFINED) {cell = "0";}
	else if(meta == PENDING_REVIEW && cell == UNDEFINED) {cell = "0";}
	else if(meta == PENDING_SCHEDULE && cell == UNDEFINED) {cell = "0";}
	else if(meta == AVAILABLE && cell == UNDEFINED) {cell = "0";}
	else if(meta == SIGNED && cell == UNDEFINED) {cell = "0";} 
	else if(meta == URGENT && cell == UNDEFINED) {cell = "0";}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::ScheduleRowsCellToData(TObject *Sender, int meta,
	AnsiString &cell) {
	if(meta == EMPLOYEE) {
		if(cell == AnsiString::LoadStr(FARCHIVE_Y)) {cell = "1";}
		else {cell = "0";}
	} else if(meta == SCH_HIDDEN && cell == UNDEFINED) {cell = "0";}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::PersonRowsBeforeRefresh(TObject *Sender,
	  QueryPtr &ResultSet, id_t &Id) {
	LOGFUNC();
	AnsiString lastname = Lastname->Text.Trim();
	AnsiString postcode = Postcode->Text.Trim();
	AnsiString address = Address->Text.Trim();
	AnsiString city = City->Text.Trim(); // not used when determining the limit
	int limit = PERSONSLIMIT; // defines: PERSONSLIMIT determines the row limit
	int length = lastname.Length() + postcode.Length() + address.Length();
	if(length >= 8) {length = 8;} limit = limit * (1 << length);

	AnsiString filter,order;
	if(EmployeesBox->Checked) {
		id_t grid_id = grid.id; if(grid_id == UNDEFINED) {grid_id = "NULL";}
		filter = " right join employees on (personal.PERSONAL_ID="
			"employees.PERSONAL_ID and employees.SCHEDULE_ID="+grid_id+")";
		order = "order by employees.EMPLOYEE_ORDER";
	} else {order = "order by personal.PERSONAL_ID desc";}

	AnsiString select;
	AnsiString restrict = "(HIDDEN=0 or CREATED_BY="+
		QuotedStr(db->getUsername())+")";
	// note that the first % symbol makes it impossible to use indices
#if defined MS_SQL || defined ACCESS
	select = "select top "+IntToStr(limit)+" personal.PERSONAL_ID,"
		"LASTNAME,ADDRESS,POSTCODE,CITY,TELEPHONE,TELEPHONE2,"
		"CREATED_BY,CREATED_DATE,HIDDEN,MEMOTEXT from ";
	if(EmployeesBox->Checked) {select += "(personal"+filter+")";}
	else {select += "personal";}
	// note that the search fields don't support wildcard symbols.
	select += " where LASTNAME like "+QuotedStr("%"+lastname+"%")+
		" and ADDRESS like "+QuotedStr("%"+address+"%")+
		" and CITY like "+QuotedStr("%"+city+"%")+
		" and POSTCODE like "+QuotedStr(postcode+"%");
	if(root_user == false) {select += (" and "+restrict);}
	select += " "+order;
#elif defined INNODB
	select = "select personal.PERSONAL_ID,LASTNAME,ADDRESS,POSTCODE,CITY,"
		"TELEPHONE,TELEPHONE2,CREATED_BY,CREATED_DATE,HIDDEN,MEMOTEXT "
		"from personal"+filter+" where LASTNAME like "+QuotedStr("%"+
		lastname+"%")+" and ADDRESS like "+QuotedStr("%"+address+"%")+
		" and CITY like "+QuotedStr("%"+city+"%")+
		" and POSTCODE like "+QuotedStr(postcode+"%"); // optimize % here
	if(root_user == false) {select += (" and "+restrict);}
	select += " "+order+" limit "+IntToStr(limit);
#endif
	persons.execSQL(select);
	ResultSet = &persons;
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::AppRowsBeforeRefresh(TObject *Sender,
	  QueryPtr &ResultSet, id_t &Id) {
	LOGFUNC();
	apps.execSQL("select apps.APP_ID,EXCLUSIVE,CANCELLED,PAID,"
		"COLOR,CHARGE,CREATED_BY,CREATED_DATE,INFOTEXT,"
		"min(DATE_FROM) as min_entries_date_from,"
		"max(DATE_UNTIL) as max_entries_date_until,"
		"count(DATE_FROM) as count_entries_date_from,"
		// display number of entries even for
		// employees we do not have access to
#ifdef MS_SQL
		"sum(datediff(minute,'" ZERODATE "',DATE_DIFF)) "
#elif defined INNODB
		"sum(TO_DAYS(DATE_DIFF)*24*60+TIME_TO_SEC("
			"DATE_DIFF) div 60-TO_DAYS('" ZERODATE "')*24*60) "
#elif defined ACCESS
		"sum(datediff('n','" ZERODATE "',DATE_DIFF)) "
#endif
		// right join: skip entries that have no appointment (blocked).
		// Do not skip appointments without entries however (not planned).
		"as sum_entries_date_diff from (entries right join apps on "
		"entries.APP_ID=apps.APP_ID) where apps.PERSONAL_ID="+ids[per]+
		" group by apps.APP_ID,EXCLUSIVE,CANCELLED,PAID,COLOR,CHARGE,"
		"CREATED_BY,CREATED_DATE,INFOTEXT order by apps.APP_ID desc");
	ResultSet = &apps;
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::DocRowsBeforeRefresh(TObject *Sender,
	  QueryPtr &ResultSet, id_t &Id) {
	LOGFUNC();
	docs.execSQL("select PERSONAL_ID,DOC_NAME,DOC_SIZE,COMMENT,"
		IFELSE("PENDING_REVIEW=1 or PENDING_SCHEDULE=1 or "
		"PLANNED_DATE is not NULL","1","0")" as schedule,"
		IFELSE("LAST_UPDATE < REJECTED_DATE","1","0")" as "
		"rejected,LAST_UPDATE,LAST_UPDATE_UTC,REJECTED_DATE,"
		"PLANNED_DATE,ORDERED_DATE,CHARGED_DATE,PENDING_REVIEW,"
		"PENDING_SCHEDULE,SIGNED,URGENT,CREATED_BY,CREATED_DATE,"
		"STATETEXT,DOC_DATA from docs where PERSONAL_ID="+
		ids[per]+" order by CREATED_DATE desc");
	ResultSet = &docs;
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::ScheduleRowsBeforeRefresh(TObject *Sender,
	  QueryPtr &ResultSet, id_t &Id) {
	LOGFUNC();
	id_t person_id = ids[per]; if(person_id == UNDEFINED) {person_id = "NULL";}
	AnsiString select = "select schedules.SCHEDULE_ID,SCHEDULE_NAME,"
		"PERSONAL_ID as EMPLOYEE,WEEK_START,WEEK_STOP,DAY_START,DAY_STOP,"
		"DAY_UNITS,CREATED_BY,CREATED_DATE,HIDDEN,NOTETEXT from schedules "
		"left join employees on (schedules.SCHEDULE_ID=employees.SCHEDULE_ID "
		"and employees.PERSONAL_ID="+person_id+")";
	AnsiString restrict = "(schedules.HIDDEN=0 or schedules.CREATED_BY="+
		QuotedStr(db->getUsername())+")";
	if(root_user == false) {select += (" where "+restrict);}
	select += " order by schedules.SCHEDULE_ID desc";
	scheds.execSQL(select);
	ResultSet = &scheds;
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::PersonRowsAfterRefresh(TObject *Sender,
	  int &SelectRow, bool IsIdRow) {
	LOGFUNC();
	if(SelectRow == 0) { // Check the row that will be selected.
		if(ActiveControl == Memo) {ActiveControl = Lastname;}
		// when enabled false and control was focused: ActiveControl -> NULL
		Memo->Text = AnsiString::LoadStr(FARCHIVE_ENTERDESCRIPTIONHERE);
		Memo->Enabled = false;
	} else {Memo->Enabled = true; Memo->Text = getCell(MEMOTEXT,SelectRow);}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::AppRowsAfterRefresh(TObject *Sender,
	  int &SelectRow, bool IsIdRow) {
	LOGFUNC();
	for(int row=1;row != AppRows->RowCount;row++) { // rowcount >= 1
		setData(Codes::color2code(getData(COLOR,row)).name,CODE_NAME,row);
		setData(CurrToStrF(getPrice(row),ffFixed,2),PRICE,row);
	}
	if(SelectRow == 0) {
		if(ActiveControl == Info) {ActiveControl = AppRows;}
		Info->Text = AnsiString::LoadStr(FARCHIVE_ENTERDESCRIPTIONHERE);
		Info->Enabled = false;
	} else {
		AppRows->addRow(); AppRows->addRow(); setTotal(SelectRow,1);
		Info->Enabled = true; Info->Text = getCell(INFOTEXT,SelectRow);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::DocRowsAfterRefresh(TObject *Sender,
	int &SelectRow, bool IsIdRow) {
	LOGFUNC();
	AnsiString doc_path(TFoptions1::o.documents_path);
	for(int row=1;row != DocRows->RowCount;row++) { // rowcount >= 1
		DocRows->setData(IntToStr((int) FileExists(doc_path+"\\"+
			DocRows->getData(DOC_DATA,row))),AVAILABLE,row);
	}
	if(SelectRow == 0) {
		if(ActiveControl == Stat) {ActiveControl = DocRows;}
		Stat->Text = AnsiString::LoadStr(FARCHIVE_ENTERDESCRIPTIONHERE);
		Stat->Enabled = false;
	} else {Stat->Enabled = true; Stat->Text = getCell(STATETEXT,SelectRow);}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::ScheduleRowsAfterRefresh(TObject *Sender,
	  int &SelectRow, bool IsIdRow) {
	LOGFUNC();
	if(SelectRow == 0) {
		if(ActiveControl == Note) {ActiveControl = ScheduleRows;}
		Note->Text = AnsiString::LoadStr(FARCHIVE_ENTERDESCRIPTIONHERE);
		Note->Enabled = false; if(grid.id != UNDEFINED) {refreshForms();}
	} else {
		Note->Enabled = true; Note->Text = getCell(NOTETEXT,SelectRow);
		id_t schedule_id = getData(SCHEDULE_ID,SelectRow);
		if(grid.id != schedule_id) {grid.id = schedule_id; refreshForms();}
	}
}
//---------------------------------------------------------------------------

// When a transaction involves the currently selected row only, it does not
// seem necessary to lock. It is possible to 'update where field1=old_data,
// field2=old_data,...' and check if the row was actually affected. In this
// way no additional reads are required. This strategy has the disadvantage
// that the user is informed about a (locking) conflict only after the user
// has edited the fields.
void __fastcall TFarchive1::PersonRowsBeforeReadRow(TObject *Sender,
	  QueryPtr &ResultSet, id_t Id, bool &Compare) {
	LOGFUNC(); if(Id == UNDEFINED) {Id = "NULL";}
	if(Compare) {Policy::setULock();}
	AnsiString query = "select PERSONAL_ID,LASTNAME,ADDRESS,POSTCODE,CITY,"
		"TELEPHONE,TELEPHONE2,HIDDEN,MEMOTEXT from personal "+Policy::lock+
		"where PERSONAL_ID="+Id+Policy::lock_end;
	if(Compare) {TRYULOCK(persons.execSQL(query););}
	else {persons.execSQL(query);}
	ResultSet = &persons;
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::AppRowsBeforeReadRow(TObject *Sender,
	  QueryPtr &ResultSet, id_t Id, bool &Compare) {
	LOGFUNC(); if(Id == UNDEFINED) {Id = "NULL";}
	if(Compare) {Policy::setULock();}
	// Note that HOURS is not taken into account here. The PRICE is only
	// updated by AppRowsStoreRow and when all AppRows are refreshed.
	AnsiString query = "select APP_ID,EXCLUSIVE,CANCELLED,PAID,COLOR,CHARGE,"
		"INFOTEXT from apps "+Policy::lock+"where APP_ID="+Id+Policy::lock_end;
	if(Compare) {TRYULOCK(apps.execSQL(query););}
	else {apps.execSQL(query);}
	ResultSet = &apps;
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::DocRowsBeforeReadRow(TObject *Sender,
	  QueryPtr &ResultSet, id_t Id, bool &Compare) {
	LOGFUNC(); if(GETDOCID(Id) == false) {per_id = "NULL";}
	if(Compare) {Policy::setULock();}
	AnsiString query = "select PERSONAL_ID,DOC_NAME,DOC_SIZE,COMMENT," IFELSE(
		"PENDING_REVIEW=1 or PENDING_SCHEDULE=1 or PLANNED_DATE is not NULL",
		"1","0")" as schedule," IFELSE("LAST_UPDATE < REJECTED_DATE","1","0")
		" as rejected,LAST_UPDATE,LAST_UPDATE_UTC,REJECTED_DATE,PLANNED_DATE,"
		"ORDERED_DATE,CHARGED_DATE,PENDING_REVIEW,PENDING_SCHEDULE,SIGNED,"
		"URGENT,STATETEXT,DOC_DATA from docs "+Policy::lock+
		"where PERSONAL_ID="+per_id+" and DOC_NAME="+
		QuotedStr(doc_name)+Policy::lock_end;
	if(Compare) {TRYULOCK(docs.execSQL(query););}
	else {docs.execSQL(query);}
	ResultSet = &docs;
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::ScheduleRowsBeforeReadRow(TObject *Sender,
	  QueryPtr &ResultSet, id_t Id, bool &Compare) {
	LOGFUNC(); if(Id == UNDEFINED) {Id = "NULL";}
	if(Compare) {Policy::setULock();}
	AnsiString query = "select SCHEDULE_ID,SCHEDULE_NAME,WEEK_START,WEEK_STOP,"
		"DAY_START,DAY_STOP,DAY_UNITS,HIDDEN,NOTETEXT from schedules "+
		Policy::lock+"where SCHEDULE_ID="+Id+Policy::lock_end;
	if(Compare) {TRYULOCK(scheds.execSQL(query););}
	else {scheds.execSQL(query);}
	ResultSet = &scheds;
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::PersonRowsAfterReadRow(TObject *Sender, id_t Id,
	bool Compare) {
	LOGFUNC();
	Memo->Text = getCell(MEMOTEXT);
	// check for Id != UNDEFINED when
	// readRow is called a second time
	if(Id != UNDEFINED) {checkChanged();}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::AppRowsAfterReadRow(TObject *Sender, id_t Id,
	bool Compare) {
	LOGFUNC();
	setData(Codes::color2code(getData(COLOR)).name,CODE_NAME);
	setData(CurrToStrF(getPrice(AppRows->Row),ffFixed,2),PRICE);
	setTotal(AppRows->Row,1);
	Info->Text = getCell(INFOTEXT);
	if(Id != UNDEFINED) {checkChanged();}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::DocRowsAfterReadRow(TObject *Sender, id_t Id,
	bool Compare) {
	LOGFUNC();
	Stat->Text = getCell(STATETEXT);
	if(Id != UNDEFINED) {checkChanged();}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::ScheduleRowsAfterReadRow(TObject *Sender, id_t Id,
	bool Compare) {
	LOGFUNC();
	Note->Text = getCell(NOTETEXT);
	if(Id != UNDEFINED) {checkChanged();}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::PersonRowsStoreRow(TObject *Sender, id_t Id) {
	LOGFUNC();
	if(Id == UNDEFINED) {clearRow(); return;}

	Policy::setSLock();
	AnsiString update =
		"update personal "+Policy::lock+"set ";
	update += "LASTNAME="+QuotedStr(getData(
		LASTNAME).SubString(0,MAXLASTNAME))+",";
	update += "POSTCODE="+QuotedStr(getData(
		POSTCODE).SubString(0,MAXPOSTCODE))+",";
	update += "ADDRESS="+QuotedStr(getData(
		ADDRESS).SubString(0,MAXADDRESS))+",";
	update += "CITY="+QuotedStr(getData(
		CITY).SubString(0,MAXCITY))+",";
	update += "TELEPHONE="+QuotedStr(getData(
		TELEPHONE).SubString(0,MAXTELEPHONE))+",";
	update += "TELEPHONE2="+QuotedStr(getData(
		TELEPHONE2).SubString(0,MAXTELEPHONE))+" ";
	update += "where PERSONAL_ID="+Id;
	TRYSLOCK(
		persons.execSQL(update);
		readRow(false); // INNODB & REPEATABLE READ: put within TRYLOCK
		// The data that is provided to the database and the data that
		// is actually stored may be different. When storing currency
		// values, the database float accuracy may not be enough and
		// CURRENT_TIMESTAMP can only be known if it is read again. By
		// reading back the stored values, the user gets what he sees.
		// Also note that user input may be truncated by the database.
		modified = refresh_info = true;
	);
}
//---------------------------------------------------------------------------

// The PRICE column is not part of the database data and is calculated after
// refresh and when storerow has been called. The CHARGE field allows refresh
// to recalculate the price if the number of HOURS has changed. If HOURS is
// changed by others while viewing the appointment, the old hours value is used.
void __fastcall TFarchive1::AppRowsStoreRow(TObject *Sender, id_t Id) {
	LOGFUNC();
	if(Id == UNDEFINED) {clearRow(); return;}
	// future: appointments store code_name instead of color value?
	// Note that the colors of appointments that already have been
	// inserted are changed when its code record changes. we do not
	// want to update the charge values of appointments unless the
	// code value of the appointment is changed.
	Code code = Codes::name2code(getData(CODE_NAME));
	TRYSLOCK(
		apps.execSQL("update apps "+Policy::lock+"set COLOR="+code.color+
			",CHARGE="+code.app_charge+" where APP_ID="+Id);
		// setData(code.color,COLOR); setData(code.app_charge,CHARGE);
		readRow(false); // display default code name if it did not exist
		modified = refresh_info = true;
	);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::DocRowsStoreRow(TObject *Sender, id_t Id) {
	LOGFUNC(); if(GETDOCID(Id) == false) {clearRow(); return;}
	Policy::setSLock();
	AnsiString update = "update docs "+Policy::lock+"set ";
	update += "COMMENT="+QuotedStr(getData(COMMENT).SubString(0,MAXCOMMENT));
	update += " where PERSONAL_ID="+per_id+" and DOC_NAME="+QuotedStr(doc_name);
	TRYSLOCK(docs.execSQL(update); readRow(false); modified = true;);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::ScheduleRowsStoreRow(TObject *Sender, id_t Id) {
	LOGFUNC();
	if(Id == UNDEFINED) {clearRow(); return;}

	Policy::setSLock();
	AnsiString update = "update schedules "+Policy::lock+"set ";
	try {
		int wstart = StrToInt(getData(WEEK_START));
		int wstop = StrToInt(getData(WEEK_STOP));
		int dstart = StrToInt(getData(DAY_START));
		int dstop = StrToInt(getData(DAY_STOP));
		int tunits = StrToInt(getData(DAY_UNITS));
		if(setWeek(wstart,wstop)) {
			update += "WEEK_START="+IntToStr(wstart)+",";
			update += "WEEK_STOP="+IntToStr(wstop)+",";
		}
		if(setDay(dstart,dstop,tunits)) {
			update += "DAY_START="+IntToStr(dstart)+",";
			update += "DAY_STOP="+IntToStr(dstop)+",";
			update += "DAY_UNITS="+IntToStr(tunits)+",";
		}
	} catch (EConvertError & e) {}
	update += "SCHEDULE_NAME="+QuotedStr(getData(
		SCHEDULE_NAME).SubString(0,MAXSCHNAME))+" ";
	update += "where SCHEDULE_ID="+Id;
	TRYSLOCK(scheds.execSQL(update); readRow(false); modified = true;);
	refreshForms(); // FMain::Refresh() depends on view unit data
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::personsNew1Click(TObject *Sender) {
	LOGFUNC();
	if(editingRow()) {return;} // exitFirst(Sender);
	AnsiString lastname = Lastname->Text.Trim();
	AnsiString postcode = Postcode->Text.Trim();
	AnsiString address = Address->Text.Trim();
	AnsiString city = City->Text.Trim();
	AnsiString format = TFoptions1::o.postcode_format;
	// Prevent empty records by demanding at least a name or postal code.
	// A newly inserted personal record always displays first (higher id)
	if(lastname.IsEmpty() && postcode.IsEmpty()) {
		ActiveControl = Lastname;
		throw UserException(FARCHIVE_NEEDNAMEORPOSTCODE);
	} else if(postcode.Length() && format.Length() &&
		postcode.Length() != format.Length()) {
		ActiveControl = Postcode;
		throw UserException(FARCHIVE_NEEDNAMEORPOSTCODE);
	}

	// there is no need to readRow because it is always valid to insert a new
	// person record. It is impossible to insert duplicate keys because a
	// unique key is generated at the moment of insertion. Also locking records
	// that are about to be inserted is not required to ensure serializability.
	// todo: check for valid user input, dates should match ShortDateFormat?

	Person person;
	person.lastname = lastname;
	person.postcode = postcode;
	person.address = address;
	person.city = city;
	person.created_by = db->getUsername();
	TRYULOCK( // nothing bad happens when insert fails: no rollback
		// person.id is locked by definition (insertion)
		persons.insert(person);
		modified = true;
	);
	EmployeesBox->Checked = false;
	refreshRows(person.id);
	// refreshRows to show the inserted person. Outside of TRYLOCK because
	// it is obvious that others could not have changed any data of the newly
	// inserted person. Records that were changed at an earlier time and
	// were also modified by others will display recent data as soon as the
	// user selects and edits them.

	modify1Click(Sender);
	PersonRows->editCol(POSTCODE); // first jump to postcode
	if(city.Length()) {PersonRows->editCol(TELEPHONE);}
	else if(address.Length()) {PersonRows->editCol(CITY);}
	else if(postcode.Length()) {PersonRows->editCol(ADDRESS);}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::personsDuplicate1Click(TObject *Sender) {
	LOGFUNC();
	if(rowSelected() == false || editingRow()) {return;}
	if(getId() == UNDEFINED) {return;}
	Person person;
	person.lastname = getData(LASTNAME);
	person.address = getData(ADDRESS);
	person.city = getData(CITY);
	person.telephone = getData(TELEPHONE);
	person.telephone2 = getData(TELEPHONE2);
	person.postcode = getData(POSTCODE);
	person.memo = getData(MEMOTEXT);
	person.created_by = db->getUsername();
	TRYULOCK(persons.insert(person); modified = true;);
	EmployeesBox->Checked = false;
	refreshRows(person.id);
	modify1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::appsNew1Click(TObject *Sender) {
	LOGFUNC();
	if(editingRow()) {return;} // exitFirst(Sender);
	id_t appointment_id;
	AnsiString color = IntToStr(TFoptions1::o.insert_color);
	AnsiString charge = Codes::color2code(color).app_charge;
	TRYULOCK( // nothing bad happens when insert fails: no rollback
		appointment_id = apps.insert(ids[per],TFoptions1::o.restrict_updates,
			false,TFoptions1::o.insert_color,charge,"",db->getUsername());
		modified = true;
	); refreshRows(appointment_id);
	set_refresh = false;
	ActiveControl = Info;
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::appsDuplicate1Click(TObject *Sender) {
	LOGFUNC();
	if(rowSelected() == false || editingRow()) {return;}
	if(getId() == UNDEFINED) {return;}
	id_t appointment_id;
	TRYULOCK(
		appointment_id = apps.insert(ids[per],
			StrToInt(getData(EXCLUSIVE)),false,
			StrToInt(getData(COLOR)),getData(CHARGE),
			getData(INFOTEXT),db->getUsername());
		modified = true;
	); refreshRows(appointment_id);
	modify1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::docsInsert(AnsiString src_path,
	id_t person_id,AnsiString filename) {
	LOGFUNC();
	int doc_size;
	int file_handle = fileopen(src_path,false);
	try {doc_size = fileinfo(file_handle).nFileSizeLow;}
	__finally {closefile(file_handle);}
	AnsiString doc_ex = ExtractFileExt(filename);
	filename = ChangeFileExt(filename,"");
	AnsiString doc_name = toFilename(persons.getText(
		ids[per],"LASTNAME"))+" - "+filename;
	AnsiString doc_nr = IntToStr(docs.getDocNumber(person_id,doc_name,doc_ex));
	doc_name = doc_name.SubString(0,MAXDOCNAME-doc_nr.Length()-doc_ex.Length());
	filename = filename.SubString(0,MAXCOMMENT-doc_nr.Length()-doc_ex.Length());
	doc_name = doc_name+doc_nr+doc_ex; filename = filename+doc_nr+doc_ex;
	id_t document_id = person_id+","+QuotedStr(doc_name);
	AnsiString doc_data(person_id+"\\"+doc_name);
	AnsiString username(db->getUsername());
	TRYULOCK( // nothing bad happens when insert fails: no rollback
		// persons.check(ids[per]); // docs table uses person_id
		docs.insert(document_id,doc_size,filename,username,"",doc_data);
		modified = true;
	); refreshRows(document_id); // refresh LUPDATE_UTC

	try {
		try { // todo: rollback when not auto_save?
			AnsiString doc_path = createDocPath(getData(DOC_DATA));
			FileSetReadOnly(doc_path,false); // always overwrite
			copyfile(src_path,doc_path,ProgressBar1);
			// A succesful copyfile cannot be undone.
			setLastWrite(doc_path,TDateTime(getData(LUPDATE_UTC)));
			setData("1",AVAILABLE);
		} catch(Exception & e) {rollback(); throw;}
	} __finally {autosave();}
	// autosave before a possible rollback but block
	// others until after performing file operations
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::docsNew1Click(TObject *Sender) {
	LOGFUNC();
	if(editingRow()) {return;} // exitFirst(Sender);
	OpenDialog->InitialDir = ""; // >= windows vista fix, using FileName
	OpenDialog->FileName = String(TFoptions1::o.templates_path)+"\\*.*";
	bool result;
#if __BORLANDC__ >= 0x582
	Application->ModalPopupMode = pmExplicit;
	try {result = OpenDialog->Execute(Handle);}
	__finally {Application->ModalPopupMode = pmNone;}
#else
	result = OpenDialog->Execute();
#endif
	if(result == false) {return;} flushPaint();
	doc_name = ExtractFileName(OpenDialog->FileName);
	docsInsert(OpenDialog->FileName,ids[per],doc_name);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::docsDuplicate1Click(TObject *Sender) {
	LOGFUNC();
	if(rowSelected() == false || editingRow() ||
		GETDOCID(getId()) == false) {return;}
	AnsiString src_path(TFoptions1::o.documents_path);
	src_path = src_path+"\\"+getData(DOC_DATA);
	docsInsert(src_path,per_id,doc_name.SubString(
		doc_name.LastDelimiter("-")+2,doc_name.Length()));
	modify1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::PersonRowsDblClick(TObject *Sender) {
	if(editingRow()) {return;}
	if(Fschedule1->Visible && Fpending1->Visible == false) {ModalResult = mrOk;}
	else {RowControl->SelectNextPage(true,true);}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::AppRowsDblClick(TObject *Sender) {
	if(editingRow()) {return;}
	if(Fschedule1->Visible && Fpending1->Visible == false) {ModalResult = mrOk;}
	else {appsSelect1Click(Sender);}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::DocRowsDblClick(TObject *Sender) {
	docsModify1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::ScheduleRowsDblClick(TObject *Sender) {
	schedulesDetails1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::docsLocate1Click(TObject *Sender) {
	LOGFUNC(); docsView1Click(NULL); // NULL indicates 'locate'.
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::docsView1Click(TObject *Sender) {
	LOGFUNC();
	if(rowSelected() == false || editingRow() ||
		GETDOCID(getId()) == false) {return;}
	if(Sender) {readRow(true);} // get most recent last_update values
	// Note that shellExecute is non-blocking so locks
	// are immediately released when auto_save is enabled.
	try {
		AnsiString doc_path(TFoptions1::o.documents_path);
		doc_path = doc_path+"\\"+getData(DOC_DATA);
		shellExecute(doc_path,TDateTime(getData(LUPDATE_UTC)),Sender == NULL);
	} __finally {if(Sender) {autosave();}} // release locks if possible
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::setDocData(Document & document) {
	// These fields are checked/locked by readrow.
	document.per_id = per_id;
	document.name = doc_name;
	document.size = StrToInt(getData(DOC_SIZE));
	document.lupdate_utc = getData(LUPDATE_UTC);
	document.is_signed = StrToInt(getData(SIGNED));
	document.is_urgent = StrToInt(getData(URGENT));
	document.schedule = StrToInt(getData(SCHEDULE));
	document.pending_review = StrToInt(getData(PENDING_REVIEW));
	document.pending_schedule = StrToInt(getData(PENDING_SCHEDULE));
	document.rejected_date = getData(REJECTED_DATE);
	document.planned_date = getData(PLANNED_DATE);
	document.ordered_date = getData(ORDERED_DATE);
	document.charged_date = getData(CHARGED_DATE);
	document.created_date = getData(DOC_CREATED_DATE);
	document.created_by = getData(DOC_CREATED_BY);
	document.data = getData(DOC_DATA);
	document.initialize(true,TFoptions1::review_tab);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::docsModify1Click(TObject *Sender) {
	LOGFUNC(); openDocument(docs);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::schedulesNew1Click(TObject *Sender) {
	LOGFUNC();
	if(editingRow()) {return;} // exitFirst(Sender);
	Schedule schedule;
	schedule.name = AnsiString::LoadStr(FARCHIVE_SCHEDULE);
	schedule.wstart = 0; schedule.wstop = DAYS_IN_WEEK;
	schedule.dstart = 8; schedule.dstop = 18; schedule.tunits = 24;
	schedule.created_by = db->getUsername();
	TRYULOCK( // nothing bad happens when insert fails: no rollback
		// no need to check ids[per] because
		// a schedule row does not depend on it
		scheds.insert(schedule);
		modified = true;
	); refreshRows(schedule.id);
	ActiveControl = ScheduleRows;
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::schedulesDuplicate1Click(TObject *Sender) {
	LOGFUNC();
	if(rowSelected() == false || editingRow()) {return;}
	id_t schedule_id = getId(); if(schedule_id == UNDEFINED) {return;}

	Schedule schedule; // id set by insert
	schedule.name = getData(SCHEDULE_NAME);
	schedule.wstart = StrToInt(getData(WEEK_START));
	schedule.wstop = StrToInt(getData(WEEK_STOP));
	schedule.dstart = StrToInt(getData(DAY_START));
	schedule.dstop = StrToInt(getData(DAY_STOP));
	schedule.tunits = StrToInt(getData(DAY_UNITS));
	schedule.created_by = db->getUsername();
	TRYULOCK(scheds.insert(schedule); modified = true;);
	TRYSLOCK( // This should almost never fail. If it does, rollback.
		// We always allow for the owner of a personal record to hide the
		// person. Therefore if someone else had added the person to its
		// schedule, it now becomes invisible. If the record is unhidden
		// again, it reappears. To prevent unexpected reappearances, we
		// only add the persons that are visible at the time of copying.
		// We don't need this data to be the latest hence no lock policy.
		AnsiString select = "select employees.PERSONAL_ID from employees,"
			"personal where employees.PERSONAL_ID=personal.PERSONAL_ID "
			"and employees.SCHEDULE_ID="+schedule_id;
		AnsiString restrict = "(HIDDEN=0 or CREATED_BY="+
			QuotedStr(schedule.created_by)+")";
		if(root_user == false) {select += (" and "+restrict);}
		select += " order by employees.EMPLOYEE_ORDER";
		employs.execSQL(select);
		vector<id_t> ids; vector<id_t>::const_iterator it;
		while(employs.next()) {
			ids.push_back(employs.fieldByName("PERSONAL_ID").toString());
		}
		for(it=ids.begin();it != ids.end();it++) {
			employs.insert(schedule.id,*it);
		}
	); refreshRows(schedule.id);
	modify1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::personsDelete1Click(TObject *Sender) {
	LOGFUNC();
	if(rowSelected() == false || editingRow()) {return;}
	Person person; person.id = getId();
	if(person.id == UNDEFINED) {return;}
	// checkOwner(PER_CREATED_BY);
	// no exitFirst(): ignore own changes

	bool check_entries = ~GetKeyState(VK_LCONTROL) & 0x8000; // before dialog
	AnsiString message = AnsiString::LoadStr(FARCHIVE_REALLYDELETEPERSON);
	if(ATTYN(message.c_str()) == ID_NO) {return;}

	readRow(true); // clears possible rsEditing state
	list<AnsiString> doc_data;
	TRYSLOCK(
		if(check_entries) {entries.check(person.id);}
		apps.erase(person.id,db->getUsername(),root_user);
		docs.execSQL("select DOC_DATA from docs "+Policy::lock+
			"where PERSONAL_ID="+person.id+Policy::lock_end);
		while(docs.next()) {
			doc_data.push_back(docs.fieldByName("DOC_DATA").toString());
		}
		if(doc_data.size()) {docs.erase(person.id);}
		employs.erase(person.id);
		persons.remove(person); // person.id was locked by readRow
		ids[app] = ids[doc] = UNDEFINED;
		modified = refresh_info = true;
	); clearRow(); refreshNext();
	// refreshNext to show the deleted person. Outside of TRYLOCK because it
	// does not matter that others may have changed data because the record
	// is removed anyway. Records that were changed at an earlier time and were
	// also modified by others will display recent data as soon as the user
	// selects and edits them.
	AnsiString doc_path(TFoptions1::o.documents_path);
	list<AnsiString>::const_iterator it = doc_data.begin();
	for(;it != doc_data.end();it++) {
		// continue deleting if error occured
		try {deletefile(doc_path+"\\"+*it);}
		catch(EAbort & e) {}
	}
	it = doc_data.begin(); // cleanup the directory (only if it is empty)
	if(it != doc_data.end()) {RemoveDir(ExtractFileDir(doc_path+"\\"+*it));}
	autosave();
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::appsDelete1Click(TObject *Sender) {
	LOGFUNC();
	if(rowSelected() == false || editingRow()) {return;}
	id_t appointment_id = getId(); if(appointment_id == UNDEFINED) {return;}
	if(StrToInt(getData(EXCLUSIVE))) {checkOwner(APP_CREATED_BY);}

	AnsiString message = AnsiString::LoadStr(FARCHIVE_REALLYDELETEAPPOINTMENT);
	if(ATTYN(message.c_str()) == ID_NO) {return;}

	readRow(true); // before cancelLinks
	// appointment_id locked by readRow()
	TRYSLOCK(apps.remove(appointment_id); modified = refresh_info = true;);
	clearRow(); refreshNext();
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::docsDelete1Click(TObject *Sender) {
	LOGFUNC();
	if(rowSelected() == false || editingRow()) {return;}
	id_t doc_id = getId(); if(GETDOCID(doc_id) == false) {return;}
	// checkOwner(DOC_CREATED_BY);

	AnsiString message = AnsiString::LoadStr(FARCHIVE_REALLYDELETEDOCUMENT);
	if(ATTYN(message.c_str()) == ID_NO) {return;}
	AnsiString doc_path(TFoptions1::o.documents_path);
	doc_path = doc_path+"\\"+getData(DOC_DATA);

	readRow(true); // doc_id locked by readRow()
	TRYSLOCK(docs.remove(doc_id); modified = true;);
	clearRow(); refreshNext();
	try {
		deletefile(doc_path);
		RemoveDir(ExtractFileDir(doc_path));
	} __finally {autosave();}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::schedulesDelete1Click(TObject *Sender) {
	LOGFUNC();
	if(rowSelected() == false || editingRow()) {return;}
	Schedule schedule; schedule.id = getId();
	if(schedule.id == UNDEFINED) {return;}
	checkOwner(SCH_CREATED_BY);

	AnsiString message = AnsiString::LoadStr(FARCHIVE_REALLYDELETESCHEDULE);
	if(ATTYN(message.c_str()) == ID_NO) {return;}

	readRow(true); // schedule_id locked by readRow()
	TRYSLOCK(scheds.remove(schedule); modified = true;);
	clearRow(); refreshNext(); // refreshForms by SelectCell
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::personsDetails1Click(TObject *Sender) {
	LOGFUNC();
	if(rowSelected() == false || editingRow()) {return;}
	id_t person_id = getId(); if(person_id == UNDEFINED) {return;}

	TRYULOCK(
		id_t grid_id = grid.id; if(grid_id == UNDEFINED) {grid_id = "NULL";}
		// Note that anyone can add/remove employees when a schedule is visible.
		employs.execSQL("select PERSONAL_ID from employees "+Policy::lock+
			"where PERSONAL_ID="+person_id+" and SCHEDULE_ID="+
			grid_id+Policy::lock_end);
	);
	bool per_employee = employs.next();
	bool old_employee = per_employee;
	bool per_hidden = StrToInt(getData(PER_HIDDEN));
	AnsiString created_by = getData(PER_CREATED_BY);
	Fdetails1->Caption = getData(LASTNAME);
	Fdetails1->CreatedBy->Caption = created_by;
	Fdetails1->Created->Caption = getData(PER_CREATED_DATE);
	if(Fdetails1->view_per(per_employee,per_hidden,root_user ||
		created_by == db->getUsername()) == mrIgnore) {return;}

	readRow(true);
	TRYSLOCK( // Both queries should succeed.
		if(per_employee != old_employee && grid.id != UNDEFINED) {
			if(per_employee) {employs.insert(grid.id,person_id);}
			else {employs.remove(grid.id,person_id);}
		}
		AnsiString is_hidden = IntToStr((int) per_hidden);
		persons.execSQL("update personal "+Policy::lock+"set HIDDEN="+
			is_hidden+" where PERSONAL_ID="+person_id);
		setData(is_hidden,PER_HIDDEN); // can only be set by owner
		modified = true; // person is always visible for the owner
	); if(per_employee != old_employee) {refreshForms();}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::appsDetails1Click(TObject *Sender) {
	LOGFUNC();
	if(rowSelected() == false || editingRow()) {return;}
	id_t appointment_id = getId(); if(appointment_id == UNDEFINED) {return;}

	bool app_paid = StrToInt(getData(PAID));
	bool app_exclusive = StrToInt(getData(EXCLUSIVE));
	bool app_cancelled = StrToInt(getData(CANCELLED));
	Currency charge(getData(CHARGE));
	bool cancelled_intime = charge < 0;
	AnsiString created_by = getData(APP_CREATED_BY);
	Fdetails1->Caption = appointment_id;
	Fdetails1->CreatedBy->Caption = created_by;
	Fdetails1->Created->Caption = getData(APP_CREATED_DATE);
	if(Fdetails1->view_app(app_paid,app_cancelled,
		cancelled_intime,app_exclusive,root_user ||
		created_by == db->getUsername()) == mrIgnore) {return;}
	if(cancelled_intime && charge > 0) {charge = -charge;}
	if(!cancelled_intime && charge < 0) {charge = -charge;}

	readRow(true);
	TRYSLOCK(
		AnsiString is_paid = IntToStr((int) app_paid);
		AnsiString is_exclusive = IntToStr((int) app_exclusive);
		AnsiString is_cancelled = IntToStr((int) app_cancelled);
		AnsiString charge_str = CurrToStr(charge);
		apps.execSQL("update apps "+Policy::lock+"set EXCLUSIVE="+
			is_exclusive+",CANCELLED="+is_cancelled+",PAID="+is_paid+
			",CHARGE="+charge_str+" where APP_ID="+appointment_id);
		setData(is_paid,PAID);
		setData(is_exclusive,EXCLUSIVE);
		setData(is_cancelled,CANCELLED);
		setData(charge_str,CHARGE);
		setData(CurrToStrF(getPrice(AppRows->Row),ffFixed,2),PRICE);
		modified = refresh_info = true;
	); setTotal(AppRows->Row,1);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::docsDetails1Click(TObject *Sender) {
	LOGFUNC();
	if(rowSelected() == false || editingRow() ||
		GETDOCID(getId()) == false) {return;}

	Document doc; setDocData(doc);
	if(Fdetails1->view_doc(&doc,true,true) == mrIgnore) {return;}
	AnsiString doc_path(TFoptions1::o.documents_path);
	doc_path = doc_path+"\\"+doc.data;
	readRow(true);
	try {
		TRYSLOCK( // todo: cannot rollback to original file
			docs.update(doc);
			readRow(false);
			modified = true;
		);
		try {
			AnsiString doc_data = getData(DOC_DATA);
			if(doc_data != doc.data) { // compare doc_data, not path
				AnsiString src_path(TFoptions1::o.documents_path);
				src_path = src_path+"\\"+doc.data;
				doc_path = createDocPath(doc_data);
				if(FileExists(doc_path)) {deletefile(doc_path);}
				renamefile(src_path,doc_path);
				RemoveDir(ExtractFileDir(src_path)); // ignore return value
			}
		} catch(Exception & e) {rollback(); throw;}
	} __finally {autosave();}
	if(!doc.pending_review && !doc.pending_schedule &&
		doc.planned_date.IsEmpty() && doc.schedule) {
		TDateTime lutc(doc.lupdate_utc);
		if(doc.schlist) {macroExecute(doc_path,lutc,"ToSchedule");}
		else {macroExecute(doc_path,lutc,"ToReview");}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::schedulesDetails1Click(TObject *Sender) {
	LOGFUNC();
	if(rowSelected() == false || editingRow()) {return;}
	id_t schedule_id = getId(); if(schedule_id == UNDEFINED) {return;}

	bool sch_hidden = StrToInt(getData(SCH_HIDDEN));
	AnsiString created_by = getData(SCH_CREATED_BY);
	Fdetails1->Caption = getData(SCHEDULE_NAME);
	Fdetails1->CreatedBy->Caption = created_by;
	Fdetails1->Created->Caption = getData(SCH_CREATED_DATE);
	if(Fdetails1->view_sch(sch_hidden,root_user ||
		created_by == db->getUsername()) == mrIgnore) {return;}

	readRow(true);
	TRYSLOCK(
		AnsiString is_hidden = IntToStr((int) sch_hidden);
		scheds.execSQL("update schedules "+Policy::lock+"set HIDDEN="+
			is_hidden+" where SCHEDULE_ID="+schedule_id);
		setData(is_hidden,SCH_HIDDEN);
		modified = true;
	);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::personsSearch1Click(TObject *Sender) {
	// ActiveControl = exitFirst(Sender);
	// exitFirst: selected row may change when auto_search disabled
	// disabled: checking for editingRow now.
	if(editingRow()) {return;}
	refreshRows(getId(PersonRows->Row));
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::EmployeesBoxEnter(TObject *Sender) {
	LOGFUNC();
	EmployeesBox->Checked = 1-EmployeesBox->Checked;
	refreshRows(getId(PersonRows->Row));
	ActiveControl = Lastname;
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::appsSelect1Click(TObject *Sender) {
	if(rowSelected()) {
		app_entries = StrToInt(getData(ENTRIES));
		ModalResult = mrOk;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::SearchChange(TObject *Sender) {
	if(TFoptions1::o.auto_search) {refreshRows(getId(PersonRows->Row));}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::modify1Click(TObject *Sender) {
	Word Key = VK_F2; KeyDown(Key,TShiftState());
	// rows->editingRow() skips ActiveControl == TMemo
	if(ActiveRows == AppRows && AppRows->editingRow()) {
		bool handled = false;
		AppRows->DoContextPopup(TPoint(-1,-1),handled);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::selectAll1Click(TObject *Sender) {
	TMemo * memo = dynamic_cast<TMemo *>(ActiveControl);
	if(memo) {memo->SelectAll(); return;}
	ActiveControl = ActiveRows;
	Word Key = 'a'; KeyDown(Key,TShiftState() << ssCtrl);
	if(Key == '\0' && ActiveRows == AppRows) { // selectAll succesful?
		int row = AppRows->Selection.Top;
		int end = AppRows->Selection.Bottom+1;
		if(end-row > 1) {setTotal(row,end-row);}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::CancelButtonClick(TObject *Sender) {
	// Do not give focus to CancelButton when it is clicked.
	if(ActiveRows == PersonRows) {ActiveControl = Lastname;}
	else {ActiveControl = ActiveRows;}
	TFmdetail1::CancelButtonClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::FormKeyPress(TObject *Sender, char &Key) {
	if(Key == VK_ESCAPE) {
		// after ESCAPE set focus to Lastname for fast insertions.
		// To start editing again, F2 should be pressed anyway
		if(ActiveRows == PersonRows && ActiveControl != Lastname) {
			ActiveControl = Lastname; Key = '\0';
		} else if(dynamic_cast<TMemo *>(ActiveControl)) {
			ActiveControl = ActiveRows; Key = '\0';
		} else if(editingRow()) {storeRow(); Key = '\0';}
	}
	TFmdetail1::FormKeyPress(Sender,Key);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::FormKeyDown(TObject *Sender, WORD &Key,
	TShiftState Shift) {
	if(Key == VK_UP || Key == VK_DOWN ||
		Key == VK_PRIOR || Key == VK_NEXT || Key == VK_RETURN) {
		if(dynamic_cast<TMemo *>(ActiveControl)) {return;}
	} else if(Key == VK_F2) {
		if(ActiveRows == AppRows) {
			// AppRows->editingRow() bypasses ActiveControl == TMemo check
			if(AppRows->rowSelected() == false || AppRows->editingRow() ||
				AppRows->Row >= AppRows->RowCount-2) {return;}
			if(StrToInt(getData(EXCLUSIVE))) {checkOwner(APP_CREATED_BY);}
		} else if(ActiveRows == ScheduleRows) {
			if(ScheduleRows->rowSelected() == false ||
				ScheduleRows->editingRow()) {return;}
			if(getId() != UNDEFINED) {checkOwner(SCH_CREATED_BY);}
			// checkOwner throws an exception if not the owner.
			ATTO(AnsiString::LoadStr(FARCHIVE_INCONSISTENCYWARNING).c_str());
		} // note that persons/appointments/documents may be edited by others
	}
	TFmdetail1::FormKeyDown(Sender,Key,Shift);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::FormMouseWheelDown(TObject *Sender,
	TShiftState Shift, TPoint &MousePos, bool &Handled) {
	if(dynamic_cast<TMemo *>(ActiveControl)) {return;}
	TFmdetail1::FormMouseWheelDown(Sender,Shift,MousePos,Handled);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::FormMouseWheelUp(TObject *Sender,
	TShiftState Shift, TPoint &MousePos, bool &Handled) {
	if(dynamic_cast<TMemo *>(ActiveControl)) {return;}
	TFmdetail1::FormMouseWheelUp(Sender,Shift,MousePos,Handled);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::PersonRowsSelectCell(TObject *Sender,int ACol,
	int ARow,bool &CanSelect) {
	LOGFUNC();
	if(PersonRows->Row != ARow) {
		Memo->Text = getCell(MEMOTEXT,ARow);
		if(getId(ARow) != ids[per]) {
			// do not reset when only moving back and
			// forward from personal tab to other tabs
			ids[app] = ids[doc] = UNDEFINED;
		}
	} else if(PersonRows->Col == POSTCODE && editingRow()) {
		AnsiString city = getCell(CITY);
		AnsiString address = getCell(ADDRESS);
		AnsiString postcode = getCell(POSTCODE).Trim();
		AnsiString postcode_format = TFoptions1::o.postcode_format;
		int format_length = postcode_format.Length();
		if((city.IsEmpty() || address.IsEmpty()) &&
			format_length && postcode.Length() == format_length) {
			AnsiString pattern;
			for(int i=1;i <= format_length;i++) {
				if(postcode_format[i] == 'c') {pattern += postcode[i];}
				else {pattern += "_";}
			}
#if defined MS_SQL || defined ACCESS
			persons.execSQL("select top 1 * from personal where POSTCODE "
				"like "+QuotedStr(pattern));
#elif defined INNODB
			persons.execSQL("select * from personal where POSTCODE "
				"like "+QuotedStr(pattern)+" LIMIT 1");
#endif
			if(persons.next()) {
				if(city.IsEmpty()) { // Do not overwrite if set manually.
					city = persons.fieldByName("CITY").toString().Trim();
				}

				// Sometimes different streets in the same
				// city actually have the same postal code.
				if(address.IsEmpty()) {
					pattern = "";
					for(int i=1;i <= format_length;i++) {
						if(postcode_format[i] == 'c' ||
							postcode_format[i] == 'a') {
							pattern += postcode[i];
						} else {pattern += "_";}
					}
#if defined MS_SQL || defined ACCESS
					persons.execSQL("select top 1 * from personal where "
						"POSTCODE like "+QuotedStr(pattern));
#elif defined INNODB
					persons.execSQL("select * from personal where POSTCODE "
						"like "+QuotedStr(pattern)+" LIMIT 1");
#endif
					if(persons.next()) {
						AnsiString number = " ";
						int num_index = address.Trim().LastDelimiter(" ");
						if(num_index) {
							number +=
								address.SubString(num_index+1,address.Length());
						}
						address = persons.fieldByName("ADDRESS").toString();
						num_index = address.Trim().LastDelimiter(" ");
						if(num_index) {address.SetLength(num_index);}
						address = address.Trim(); // addr   num -> addr num
						if(address.Length()) {address += number;}
					}
				}
			}
		}
		setCell(city,CITY);
		setCell(address,ADDRESS);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::AppRowsSelectCell(TObject *Sender, int ACol,
	  int ARow, bool &CanSelect) {
	LOGFUNC();
	if(AppRows->Row != ARow) {Info->Text = getCell(INFOTEXT,ARow);}
	// selectAll may have updated total in which case
	// it does not match the selected row anymore. So
	// we update total when the row stays the same.
	if(editingRow() == false) {setTotal(ARow,1);}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::DocRowsSelectCell(TObject *Sender, int ACol,
	int ARow, bool &CanSelect) {
	if(DocRows->Row != ARow) {LOGFUNC(); Stat->Text = getCell(STATETEXT,ARow);}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::ScheduleRowsSelectCell(TObject *Sender, int ACol,
	  int ARow, bool &CanSelect) {
	if(ScheduleRows->Row != ARow) {
		LOGFUNC();
		Note->Text = getCell(NOTETEXT,ARow);
		id_t schedule_id = getData(SCHEDULE_ID,ARow);
		if(grid.id != schedule_id) {grid.id = schedule_id; refreshForms();}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::MemoEnter(TObject *Sender) {
	LOGFUNC();
	// not using memo text for insertion because
	// multiple inserts would then reuse the text
	// reload data *before* entering Memo
	try {Memo->Tag = 1; readRow(true);}
	// row is valid and editingRow = false
	catch(Exception & e) {Memo->Tag = 0; ActiveControl = Lastname; throw;}
	// Memo is entered even when Enter throws an exception
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::InfoEnter(TObject *Sender) {
	LOGFUNC();
	try {
		if(StrToInt(getData(EXCLUSIVE))) {checkOwner(APP_CREATED_BY);}
		Info->Tag = 1; readRow(true);
	} catch(Exception & e) {Info->Tag = 0; ActiveControl = AppRows; throw;}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::StatEnter(TObject *Sender) {
	LOGFUNC();
	try {Stat->Tag = 1; readRow(true);}
	catch(Exception & e) {Stat->Tag = 0; ActiveControl = DocRows; throw;}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::NoteEnter(TObject *Sender) {
	LOGFUNC();
	try {
		if(getId() != UNDEFINED) {checkOwner(SCH_CREATED_BY);}
		Note->Tag = 1; readRow(true);
	} catch(Exception & e) {Note->Tag = 0; ActiveControl = ScheduleRows; throw;}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::MemoExit(TObject *Sender) {
	LOGFUNC();
	if(Memo->Modified == false) {return;}
	if(rowSelected() == false || Memo->Tag == 0) {return;}
	// row is zero when the last person is deleted and Memo is disabled
	id_t person_id = getId();
	if(person_id == UNDEFINED) {Memo->Text = ""; return;}
	AnsiString text = Memo->Text.TrimRight();
#ifdef INNODB
	text = AnsiReplaceStr(text,"\\","\\\\");
#endif
	TRYSLOCK(
		persons.execSQL("update personal "+Policy::lock+"set MEMOTEXT="+
			QuotedStr(PersonRows->cell2data(MEMOTEXT,text))+
			" where PERSONAL_ID="+person_id);
		setCell(text,MEMOTEXT);
		modified = true;
	);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::InfoExit(TObject *Sender) {
	LOGFUNC();
	if(Info->Modified == false) {return;}
	if(rowSelected() == false || Info->Tag == 0) {return;}
	id_t appointment_id = getId();
	if(appointment_id == UNDEFINED) {Info->Text = ""; return;}
	AnsiString text = Info->Text.TrimRight();
#ifdef INNODB
	text = AnsiReplaceStr(text,"\\","\\\\");
#endif
	TRYSLOCK(
		apps.execSQL("update apps "+Policy::lock+"set INFOTEXT="+
			QuotedStr(AppRows->cell2data(INFOTEXT,text))+
			" where APP_ID="+appointment_id);
		setCell(text,INFOTEXT);
		modified = true;
		if(set_refresh) {refresh_info = true;}
		set_refresh = true;
	);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::StatExit(TObject *Sender) {
	LOGFUNC();
	if(Stat->Modified == false) {return;}
	if(rowSelected() == false || Stat->Tag == 0) {return;}
	id_t doc_id = getId();
	if(GETDOCID(doc_id) == false) {Stat->Text = ""; return;}
	AnsiString text = Stat->Text.TrimRight();
#ifdef INNODB
	text = AnsiReplaceStr(text,"\\","\\\\");
#endif
	TRYSLOCK(
		docs.execSQL("update docs "+Policy::lock+"set STATETEXT="+
			QuotedStr(DocRows->cell2data(STATETEXT,text))+
			" where PERSONAL_ID="+per_id+" and DOC_NAME="+
			QuotedStr(doc_name));
		setCell(text,STATETEXT);
		modified = true;
	);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::NoteExit(TObject *Sender) {
	LOGFUNC();
	if(Note->Modified == false) {return;}
	if(rowSelected() == false || Note->Tag == 0) {return;}
	id_t schedule_id = getId();
	if(schedule_id == UNDEFINED) {Note->Text = ""; return;}
	AnsiString text = Note->Text.TrimRight();
#ifdef INNODB
	text = AnsiReplaceStr(text,"\\","\\\\");
#endif
	TRYSLOCK(
		scheds.execSQL("update schedules "+Policy::lock+"set NOTETEXT="+
			QuotedStr(ScheduleRows->cell2data(NOTETEXT,text))+
			" where SCHEDULE_ID="+schedule_id);
		setCell(text,NOTETEXT);
		modified = true;
	);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::PersonRowsGetEditMask(TObject *Sender, int ACol,
	  int ARow, UnicodeString &Value) {
	// TMaskEdit generates exceptions for masks containing required positions.
	// (used FormatMaskText as a reference)
	if(ACol == TELEPHONE || ACol == TELEPHONE2) {
		if(TFoptions1::o.telephone_length) {
			for(int i=0;i != TFoptions1::o.telephone_length;i++) {Value += '9';}
			Value += ";0;*";
		}
	} else if(ACol == POSTCODE) {
		if(TFoptions1::o.postcode_format[0]) {
			Value = ">";
			for(int i=0;TFoptions1::o.postcode_format[i];i++) {Value += 'a';}
			Value += ";0;*";
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::ScheduleRowsGetEditMask(TObject *Sender, int ACol,
	  int ARow, UnicodeString &Value) {
	if(ACol == WEEK_START || ACol == WEEK_STOP) {Value = "9;0;*";}
	else if(ACol == DAY_START || ACol == DAY_STOP || ACol == DAY_UNITS) {
		Value = "99;0;*";
	}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::DataRowsContextPopup(TObject *Sender,
	const TPoint &MousePos, bool &Handled) {
	TDataRows * rows = ActiveRows;
	if(rows->editingRow() == false || rows == AppRows) {
		if(rows->editingRow() == false) {
			PersonRows->PopupMenu = PopupMenu1;
			AppRows->PopupMenu = PopupMenu2;
			DocRows->PopupMenu = PopupMenu3;
			ScheduleRows->PopupMenu = PopupMenu4;
		} else {AppRows->PopupMenu = CodePopup;}
	} else {rows->PopupMenu = NULL;} // enable standard menu
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::itemClick(TObject * Sender) {
	if(editingRow()) { // just to make sure...
		setData(((TMenuItem *) Sender)->Caption,CODE_NAME);
		AppRows->storeRow();
	}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::addPopupMenuItem(const Code & code) {
	TMenuItem * menu_item = new TMenuItem(Farchive1);
	menu_item->Caption = code.name;
	menu_item->OnClick = Farchive1->itemClick;
	CodePopup->Items->Add(menu_item);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::DataRowsDrawCell(TObject *Sender, int ACol,
	  int ARow, const TRect &Rect, TGridDrawState State) {
	TDataRows * rows = ActiveRows;
	rows->setRowColor(State);
	TCanvas * canvas = rows->Canvas;
	AnsiString text = rows->Cells[ACol][ARow];
	int left = 0; TColor color(clBlack);
	if(rows == DocRows) {
		if(ACol == DOC_NAME) {
			if(TFoptions1::o.unsigned_red && ARow &&
				StrToInt(rows->getData(SIGNED,ARow)) == false &&
				StrToInt(rows->getData(SCHEDULE,ARow))) {
				color = TColor(CONFLICTCOLOR);
				rows->setRowColor(State,clBtnFace,clWhite,color);
			}
		} else if(ACol == SCHEDULE) {
			left = (Rect.Width()-canvas->TextWidth(text))/2;
			if(ARow && StrToInt(rows->getData(URGENT,ARow))) {
				color = TColor(CONFLICTCOLOR);
				rows->setRowColor(State,clBtnFace,clWhite,color);
			}
		} else if(ACol == REJECTED) {
			left = (Rect.Width()-canvas->TextWidth(text))/2;
			if(ARow && StrToInt(rows->getData(REJECTED,ARow))) {
				color = TColor(CONFLICTCOLOR);
				rows->setRowColor(State,clBtnFace,clWhite,color);
			}
		} else if(ACol == DOC_SIZE) {
			left = Rect.Width()-8-canvas->TextWidth(text);
		}
	} else if(rows == AppRows) {
		if(ACol == FROM || ACol == UNTIL) {
			if(ARow && text.Length()) {
				text += "/"+IntToStr(WeekOf(TDateTime(text)));
			}
		} else if(ACol == HOURS) {
			if(ARow) {left = Rect.Width()-35-canvas->TextWidth(text);}
			else {left = Rect.Width()-28-canvas->TextWidth(text);}
			if(ARow && text.Length()) {text += "/"+getCell(ENTRIES,ARow);}
		} else if(ACol == PAID) {
			left = (Rect.Width()-canvas->TextWidth(text))/2;
		} else if(ACol == PRICE) {
			left = Rect.Width()-10-canvas->TextWidth(text);
		}
	} else if(rows == ScheduleRows &&
		ACol != SCHEDULE_NAME && ACol != SCHEDULE_ID) {
		left = (Rect.Width()-canvas->TextWidth(text))/2;
	}

	bool frame = false;
	if(rows == PersonRows && ARow && StrToInt(rows->getData(PER_HIDDEN,ARow))) {
		rows->setRowColor(State,clBtnFace,clWhite,TColor(CONFLICTCOLOR));
	} else if(rows == AppRows && ARow && ARow < AppRows->RowCount-2) {
		// frame = StrToInt(rows->getData(EXCLUSIVE,ARow));
		// disabled: there may be a lot of exclusive rows
		// if TFoptions1::o.restrict_updates has been set.
		if(StrToInt(rows->getData(CANCELLED,ARow))) {
			rows->setRowColor(State,clBtnFace,clWhite,
				color,TFontStyles() << fsStrikeOut);
		}
	} else if(rows == DocRows && ARow && StrToInt(rows->getData(AVAILABLE,ARow))
		== false) {rows->setRowColor(State,clBtnFace,clWhite,clGrayText);}

	if(ARow == rows->Row && (int) rows->Meta->Objects[ACol] & METACHANGED) {
		rows->setRowColor(State,clBtnFace,TColor(CONFLICTCOLOR));
	}
	int top = (Rect.Height()-canvas->TextHeight(text))/2;
	canvas->TextRect(Rect,Rect.left+left,Rect.top+top,text);
	if(frame) {
		if(State.Contains(gdSelected)) {
			canvas->Pen->Color = clWhite;
		} else {canvas->Pen->Color = clBlack;}
		canvas->MoveTo(Rect.left,Rect.top+2);
		canvas->LineTo(Rect.right,Rect.top+2);
		canvas->MoveTo(Rect.left,Rect.bottom-3);
		canvas->LineTo(Rect.right,Rect.bottom-3);
		canvas->Pen->Color = clBlack;
	}
}
//---------------------------------------------------------------------------

bool __fastcall TFarchive1::setWeek(int & week_start,int & week_stop) {
	if(0 <= week_start && 1 <= week_stop) {
		if(week_stop > DAYS_IN_WEEK) {week_stop = DAYS_IN_WEEK;}
		if(week_start >= week_stop) {week_start = week_stop-1;}
		return true;
	} else {return false;}
}
//---------------------------------------------------------------------------

bool __fastcall TFarchive1::setDay(int & day_start,int & day_stop,
	int & tday_units) {
	// 0 <= dstart < dstop <= tunits, 0 <= 0 < 1 <= 1
	if(tday_units % 2 == 0 &&
		0 <= day_start && 1 <= day_stop && 1 <= tday_units) {
		if(day_stop > tday_units) {day_stop = tday_units;}
		if(day_start >= day_stop) {day_start = day_stop-1;}
		return true;
	} else {return false;}
}
//---------------------------------------------------------------------------

__inline Currency __fastcall TFarchive1::getPrice(int row) {
	return Currency(getData(CHARGE,row)) *
		StrToInt(getData(HOURS,row))/60.0;
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::setTotal(int row_start,int count) {
	Currency total_price(0);
	int rowcount = AppRows->RowCount;
	if(row_start + count > rowcount-2) {
		count = rowcount-2-row_start;
	}
	for(;count-- > 0;row_start++) {
		bool paid = StrToInt(getData(PAID,row_start));
		Currency row_price(getData(PRICE,row_start));
		if((paid == false && row_price > 0) ||
			(paid && row_price < 0)) {
			total_price += row_price;
		}
	}
	setCell(AnsiString::LoadStr(FARCHIVE_TOTAL),PAID,rowcount-1);
	setData(CurrToStrF(total_price,ffFixed,2),PRICE,rowcount-1);
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::AppRowsMouseUp(TObject *Sender,
	TMouseButton Button, TShiftState Shift, int X, int Y) {
	if(AppRows->Row == 0) {return;} // RowCount > 1
	int row = AppRows->Selection.Top;
	int end = AppRows->Selection.Bottom+1;
	if(end-row > 1) {setTotal(row,end-row);}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::AppRowsPrintCell(TObject *Sender, long ACol,
	long ARow, TRect &Rect) {
	TCanvas * canvas = Printer()->Canvas;
	// cannot use getCell for ARow = 0 (the label)
	AnsiString text = ActiveRows->Cells[ACol][ARow];
	if(ARow == AppRows->RowCount-2) {
		Rect.top += canvas->TextHeight("/");
	} else if(ACol == HOURS) {
		Rect.left += Rect.Width()-210-canvas->TextWidth(text);
		// disabled: the number of entries is not printed
		// if(ARow) {Rect.left += Rect.Width()-280-canvas->TextWidth(text);}
		// else {Rect.left += Rect.Width()-210-canvas->TextWidth(text);}
	} else if(ACol == PAID) {
		Rect.left += (Rect.Width()-canvas->TextWidth(text))/2;
	} else if(ACol == PRICE) {
		Rect.left += Rect.Width()-50-canvas->TextWidth(text);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::AppRowsEndDoc(TObject *Sender, TRect &Rect) {
	int rowcount = AppRows->RowCount;
	int endrow = AppRows->Selection.Bottom+1;
	if(endrow < rowcount) { // rowcount = 1 -> endrow = 1
		// If a subselection was printed, print total ourselves
		if(endrow != rowcount-1) {AppRows->printRow(rowcount-2,Rect);}
		AppRows->printRow(rowcount-1,Rect);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::DocRowsPrintCell(TObject *Sender, long ACol,
	long ARow, TRect &Rect) {
	TCanvas * canvas = Printer()->Canvas;
	AnsiString text = ActiveRows->Cells[ACol][ARow];
	if(ACol == SCHEDULE || ACol == REJECTED) {
		Rect.left += (Rect.Width()-canvas->TextWidth(text))/2;
	} else if(ACol == DOC_SIZE) {
		Rect.left += Rect.Width()-50-canvas->TextWidth(text);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::DataRowsNewPage(TObject *Sender, TRect &Rect) {
	TFmdetail1::PrintNewPage(Rect); // LOGFUNC by TFmdetail
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::print1Click(TObject *Sender) {
	TFmdetail1::PrintSelection(); // LOGFUNC by TFmdetail
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::export1Click(TObject *Sender) {
	TFbase1::exportCsvFile(ActiveRows); // LOGFUNC by TFbase
}
//---------------------------------------------------------------------------

void __fastcall TFarchive1::setMetaIndices(void) {
	LASTNAME = PersonRows->Meta->IndexOf("LASTNAME");
	POSTCODE = PersonRows->Meta->IndexOf("POSTCODE");
	ADDRESS = PersonRows->Meta->IndexOf("ADDRESS");
	CITY = PersonRows->Meta->IndexOf("CITY");
	TELEPHONE = PersonRows->Meta->IndexOf("TELEPHONE");
	TELEPHONE2 = PersonRows->Meta->IndexOf("TELEPHONE2");
	PER_HIDDEN = PersonRows->Meta->IndexOf("HIDDEN");
	PER_CREATED_BY = PersonRows->Meta->IndexOf("CREATED_BY");
	PER_CREATED_DATE = PersonRows->Meta->IndexOf("CREATED_DATE");
	MEMOTEXT = PersonRows->Meta->IndexOf("MEMOTEXT");

	APP_ID = AppRows->Meta->IndexOf("APP_ID");
	EXCLUSIVE = AppRows->Meta->IndexOf("EXCLUSIVE");
	CANCELLED = AppRows->Meta->IndexOf("CANCELLED");
	CODE_NAME = AppRows->Meta->IndexOf("CODE_NAME");
	FROM = AppRows->Meta->IndexOf("min_entries_date_from");
	UNTIL = AppRows->Meta->IndexOf("max_entries_date_until");
	ENTRIES = AppRows->Meta->IndexOf("count_entries_date_from");
	HOURS = AppRows->Meta->IndexOf("sum_entries_date_diff");
	PAID = AppRows->Meta->IndexOf("PAID");
	PRICE = AppRows->Meta->IndexOf("PRICE");
	COLOR = AppRows->Meta->IndexOf("COLOR");
	CHARGE = AppRows->Meta->IndexOf("CHARGE");
	APP_CREATED_BY = AppRows->Meta->IndexOf("CREATED_BY");
	APP_CREATED_DATE = AppRows->Meta->IndexOf("CREATED_DATE");
	INFOTEXT = AppRows->Meta->IndexOf("INFOTEXT");

	DOC_NAME = DocRows->Meta->IndexOf("DOC_NAME");
	DOC_SIZE = DocRows->Meta->IndexOf("DOC_SIZE");
	COMMENT = DocRows->Meta->IndexOf("COMMENT");
	SIGNED = DocRows->Meta->IndexOf("SIGNED");
	URGENT = DocRows->Meta->IndexOf("URGENT");
	SCHEDULE = DocRows->Meta->IndexOf("schedule");
	REJECTED = DocRows->Meta->IndexOf("rejected");
	LUPDATE_UTC = DocRows->Meta->IndexOf("LAST_UPDATE_UTC");
	REJECTED_DATE = DocRows->Meta->IndexOf("REJECTED_DATE");
	PLANNED_DATE = DocRows->Meta->IndexOf("PLANNED_DATE");
	ORDERED_DATE = DocRows->Meta->IndexOf("ORDERED_DATE");
	CHARGED_DATE = DocRows->Meta->IndexOf("CHARGED_DATE");
	PENDING_REVIEW = DocRows->Meta->IndexOf("PENDING_REVIEW");
	PENDING_SCHEDULE = DocRows->Meta->IndexOf("PENDING_SCHEDULE");
	DOC_CREATED_BY = DocRows->Meta->IndexOf("CREATED_BY");
	DOC_CREATED_DATE = DocRows->Meta->IndexOf("CREATED_DATE");
	AVAILABLE = DocRows->Meta->IndexOf("available");
	STATETEXT = DocRows->Meta->IndexOf("STATETEXT");
	DOC_DATA = DocRows->Meta->IndexOf("DOC_DATA");

	SCHEDULE_NAME = ScheduleRows->Meta->IndexOf("SCHEDULE_NAME");
	EMPLOYEE = ScheduleRows->Meta->IndexOf("EMPLOYEE");
	WEEK_START = ScheduleRows->Meta->IndexOf("WEEK_START");
	WEEK_STOP = ScheduleRows->Meta->IndexOf("WEEK_STOP");
	DAY_START = ScheduleRows->Meta->IndexOf("DAY_START");
	DAY_STOP = ScheduleRows->Meta->IndexOf("DAY_STOP");
	DAY_UNITS = ScheduleRows->Meta->IndexOf("DAY_UNITS");
	SCHEDULE_ID = ScheduleRows->Meta->IndexOf("SCHEDULE_ID");
	SCH_HIDDEN = ScheduleRows->Meta->IndexOf("HIDDEN");
	SCH_CREATED_BY = ScheduleRows->Meta->IndexOf("CREATED_BY");
	SCH_CREATED_DATE = ScheduleRows->Meta->IndexOf("CREATED_DATE");
	NOTETEXT = ScheduleRows->Meta->IndexOf("NOTETEXT");
}
//---------------------------------------------------------------------------

