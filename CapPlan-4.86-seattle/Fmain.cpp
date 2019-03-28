//$Id: Fmain.cpp,v 1.506 2016/08/17 23:19:33 darseq Exp $
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <math.hpp>
#include <dateutils.hpp>
#include <algorithm>
#include <time.h>
#include <map>
#include "Fmain.h"
#include "Fimport.h"
#include "Foptions.h"
#include "Fpending.h"
#include "Farchive.h"
#include "Frange.h"
#include "Fprint.h"
#include "Ffind.h"
#include "Fcodes.h"
#include "Fentries.h"
#include "Fcreated.h"
#include "Fschedule.h"
#include "Femployee.h"
#include "Fmail.h"
#include "ThreadModify.h"
#include "FastMM4BCB/FastMM4.hpp"

#ifdef _WIN64
#pragma link "versit.a"
#else
#pragma link "versit.lib"
#endif

#define SHOWCONFLICT(conflict,msg_id)\
do {\
	if(conflict) {\
		if(modified == false) {save1Click(Sender);}\
		refreshGrid(); browse_changed = true;\
		throw UserException(AnsiString::LoadStr(msg_id));\
		/* AnsiString message = AnsiString::LoadStr(msg_id);*/\
		/* if(msg_id == SELECTION_CHANGED && (modified ||*/\
		/*	RowGrid->DragState != Rowgrid::dsNone)) { // see save1Click */\
		/*	message += "\n" + AnsiString::LoadStr(FMAIN_USEREVERTFORCHANGES);*/\
		/*}*/\
		/*throw UserException(message);*/\
	}\
} while(0)

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Fbase"
#pragma link "RowGrid"
#pragma link "FixedScrollBox"
#pragma link "DataRows"
#pragma resource "*.dfm"
TFmain1 *Fmain1;
//---------------------------------------------------------------------------

typedef BOOL (WINAPI *sysinfo)(int uiAction,int uiPar,void * pvPar,int fWinIni);
__fastcall TFmain1::TFmain1(TComponent* Owner) : TFbase1(Owner) {
	SetForegroundWindow(Handle); // set Screen->ActiveForm
	hash = NULL; // db variable is static (see destructor)
	// edit FastMM4Options.inc to change debug settings
#if __BORLANDC__ <= 0x582
	RegisterExpectedMemoryLeak(sizeof(36), 1); // TWinHelpViewer
#endif
	HMODULE mod = GetModuleHandle("user32.dll");
	if(mod) {
		FARPROC disable = GetProcAddress(mod,"DisableProcessWindowsGhosting");
		if(disable) {disable();} // WINXP: showmodal() window behind main window
		sysinfo enable=(sysinfo) GetProcAddress(mod,"SystemParametersInfoA");
		if(enable) {enable(0x100B,0,(void *) 1,0);} // enable underline hot keys
	}

	int major,minor,release,build;
	getVersion(major,minor,release,build);
	version = "CapPlan v"+IntToStr(major)+"."+
		IntToStr(minor)+IntToStr(release)+//"."+
		/*IntToStr(build)+*/" (c) by s.b.m. post 2004-2019";
	// make sure debug messages are logged, and open default configuration
	Exceptions::initLog(version);
	debugMode1->Checked = Exceptions::debug_mode; // set by getVersion
	try {TFoptions1::openConfig(UNDEFINED);}
	catch(Exception & e) {
		Application->Terminate();
		Application->ProcessMessages();
		Application->ShowException(&e);
		return;
	}

	// Make sure TDateTime(string) convertions coincide with YMD and YMDHMS
	Application->UpdateFormatSettings = false; // Prevent format overrides
	DateSeparator = '-'; ShortDateFormat = "yyyy-mm-dd"; // See DateTimeString
	TimeSeparator = ':'; LongTimeFormat = "HH:mm:ss"; // See DateTimeString
	// SQL queries use dot for the decimal separator
	DecimalSeparator = '.'; ThousandSeparator = ',';
	// describes how 2 digits years are expanded
	TwoDigitYearCenturyWindow = TWODIGITYEARCENTURYWINDOW;

	// The hash contains cell data and represents the selection
	hash = new hash_space::HashTable<Cell> ((unsigned int) HASHSIZE);
	hash_sorted = browse_changed = false;

	// Use current_date instead of the constantly changing Date().
	// Refresh() corrects current_date and updates min/max values.
	previous_date = previous_new = current_date = Date();

	view_menu = MainMenu1->Items->Items[view1->MenuIndex];
	TMenuItem *NewItem = new TMenuItem(this);
	NewItem->Caption = "-"; view_menu->Add(NewItem);
#define VIEWMENUCOUNT 5
	grid.id = IntToStr(TFoptions1::o.default_id);
	per_id = app_id = doc_id = UNDEFINED;
	inserting = deleting = hash_only = false;
	reset_focus = first_run = false;
	context_x = context_y = -1;

	// Set the mainform size to the size of the screen.
	// Components resize with it if anchors were used.
	Left = 0; Top = 0; WindowState = wsMaximized;
	RowGrid->RowLineColor = TColor(ROWLINECOLOR);
	RowGrid->ShiftKeySwaps = false; // cell_swap not
	// supported in combination with entries.update
	TimeLabels->Canvas->Font = TimeLabels->Font;
	// Color = clSkyBlue;
	StatusPanel->Color = Color;
	ScrollBox->Color = Color;
	Application->HintPause = 100;
	Application->HintHidePause = -1;
	// EmplRows->DoubleBuffered = true;
	// disabled: seems to cause problems
	// on some systems (mirrored text??!).

	ODBCparams params; params.hwnd = Handle;
	db = new Odbc(params); db->link(&db_query);
	// db is a static variable used in all forms.
	db->link(&persons); db->link(&entries);
	db->link(&scheds); db->link(&cells);
	db->link(&codes); db->link(&apps);
	try {
		SetForegroundWindow(Handle);
#ifdef MS_SQL
		// todo: It seems that both MARS_Connection=yes and AnsiNPW=no
		// are not required anymore. Ansi nulls require us to compare
		// with 'is NULL' which is done now (at some places = NULL is
		// still used, but no rows are expected). Also all tables have
		// been created with ansi padding enabled and the ANSI_PADDING
		// setting is therefore ignored. Enabling ansi warnings should
		// not be a problem either although this has not been tested.
		// Also note that we don't have all these options for ACCESS.
		db->connect("DSN=MSSQLsource;MARS_Connection=yes;AnsiNPW=no");
#elif defined INNODB
		db->connect("DSN=MYSQLsource");
#elif defined ACCESS
		SetCurrentDir(ExtractFileDir(Application->ExeName));
#ifdef _WIN64
		db->connect("Driver={Microsoft Access Driver (*.mdb, *.accdb)};"
#else
		db->connect("Driver={Microsoft Access Driver (*.mdb)};"
#endif
			"DBQ=.\\CapPlan.mdb;ExtendedAnsiSQL=1");
#endif
		if(db->isConnected() == false) {
			Application->Terminate();
			Application->ProcessMessages();
		} else {checkHosts();}
	} catch(Exception & e) {
		Application->Terminate();
		Application->ProcessMessages();
		Application->ShowException(&e);
	}
	Policy::clrLock();
	// Refresh() by FormShow
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::FormCreate(TObject *Sender) {
	if(Application->Terminated) {
		Exceptions::clearlog = true;
		return;
	}
	mainstate = WindowState;
	Application->OnMessage = onMessage;
	Application->OnRestore = onRestore;
	Application->OnMinimize = onMinimize;
	Application->OnException = onException;
	// Exceptions that occur within the constructor are caught by the
	// try/catch block in planner.cpp. Exceptions that occur here are
	// diverted to Application->HandleException (calling OnException)
	AnsiString username = db->getUsername();
	root_user = username == "root";
#if defined MS_SQL || defined ACCESS
	// todo: use bcp (like mysqldump?) to take care of backups.
	db->setIsolation(READ_COMMITTED);
	// Use the (READCOMMITTED) hint during refresh to prevent setting locks.
	// snapshot isolation has been enabled for the database so getgrid does
	// not wait for uncommitted changes. Note that this snapshot isolation
	// is at the level of a single select so each select sees newer snapshots
#elif defined INNODB
	db->setIsolation(REPEATABLE_READ);
	// REPEATABLE_READ mode in innodb is actually SERIALIZABLE mode when only
	// using data from 'lock in share mode' selects. This allows us to refresh
	// with 'normal' selects without setting locks. Unfortunately innodb cannot
	// set next-key locks in READ_COMMITTED mode (in contrast to MS_SQL).
#endif
	db->setAutocommit(false);
	SetForegroundWindow(Handle);
	setMenu();
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::FormShow(TObject *Sender) {
	// remove the extra window from taskmgr
	ShowWindow(Application->Handle,SW_HIDE);
	if(Application->Terminated) {return;} Refresh();
	KeepAlive->Interval = TFoptions1::o.time * 60000;
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::clearRowGrid(void) {
	for(int r=0;r != RowGrid->RowCount;r++) {RowGrid->ClearRow(r);}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::RowGridClearRowCell(TObject *Sender,RowCell &RowCell,
	bool &CanClear) {
	Cell * cell = (Cell *) RowCell.Object;
	// [(Cell *) cast is actually necessary. Releasing a void
	// pointer results in too little memory being released.]
	CanClear = hash_only == false || cell->selected;
	// If hash_only, only clear the selected cells.
	if(CanClear) {delete cell; RowCell.Object = NULL;}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::FormDestroy(TObject *Sender) {
	LOGFUNC();
	try {
		cleanWordApp();
		RowGrid->ClearDragState();
		clearRowGrid();
		if(db) {
			db->unlink(&apps);
			db->unlink(&codes);
			db->unlink(&cells);
			db->unlink(&scheds);
			db->unlink(&entries);
			db->unlink(&persons);
			db->unlink(&db_query);
		}
		while(ThreadModify::threads.size()) {
			if(Exceptions::debug_mode) {
				Exceptions::writeLog("Terminate thread");
			}
			// OnTerminate removes the thread from the list
			ThreadModify * thread = ThreadModify::threads.back();
			thread->Terminate(); // frees the thread handle
			try {thread->WaitFor();} // throws invalid handle
			catch(Exception & e) {} // thread freed, continue
		}
		delete db; delete hash;
		TFoptions1::closeConfig();
		Exceptions::cleanup();
		Application->OnMessage = NULL;
		Application->OnRestore = NULL;
		Application->OnMinimize = NULL;
	} catch(Exception & e) {
		AnsiString message("Cleanup failure: ");
		e.Message.Insert(message,1);
		Exceptions::writeLog(message);
		message = e.Message;
		MSGO(message.c_str());
	}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::aboutClick(TObject *Sender) {
	AnsiString demo;
#ifdef ACCESS
	demo = "DEMO. The multiuser version is available at\n";
#endif
	MSGO((version+"\n"+demo+"http://www.postware.nl").c_str());
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::debugClick(TObject *Sender) {
	debugMode1->Checked = !debugMode1->Checked;
	Exceptions::debug_mode = debugMode1->Checked;
	if(Exceptions::debug_mode) {
		Exceptions::writeLog(version);
		AnsiString message(Exceptions::log_path);
		MSGO(("Tracing to: "+message).c_str());
	} else {MSGO("Debug mode disabled");}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::emailSupport1Click(TObject *Sender) {
	LOGFUNC(); AnsiString filename;
	if(Exceptions::debug_mode) {filename = Exceptions::copyLog();}
	sendMail(POSTWAREMAIL,"Your message:","CapPlan support",filename);
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::index1Click(TObject *Sender) {
	ShellExecute(NULL, "open", "http://www.postware.nl/cgi-bin/HELPindex",
		NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::FormClose(TObject *Sender, TCloseAction &Action) {
	// Show cleanup panel. If FormClose isn't
	// called, FormDestroy will do the cleanup
	LOGFUNC(); cleanWordApp();
	Exceptions::clearlog = true;
	Action = caFree;
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::save1Click(TObject * Sender) {
	LOGFUNC();
	if(RowGrid->DragState == Rowgrid::dsNone) {save();}
	// after save the hash already contains updated information
	/* RowGrid->ClearDragState(); */ // see rollback() disabled:
	// Only saving when DragState = dsNone. In RowGridInsertRowCell,
	// CanInsert is used to reset the DragState. [In RowGridInsertRowCell,
	// after calling entries.insert(), always ClearDragState()]
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::abort1Click(TObject * Sender) {
	LOGFUNC();
	// always rollback (release locks) if not
	// modified, otherwise display a warning.
	if(modified == false || TFoptions1::auto_save || ATTYN(
		AnsiString::LoadStr(REALLYDISCARDCHANGES).c_str()) == ID_YES) {
		rollback(); // reloads buffered schedules/employees/grid/codes
	}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::view_archive(TTabSheet * tabsheet) {
	LOGFUNC();
	if(RowGrid->DragState != Rowgrid::dsNone) {return;}

	// The archive form performs updates within the current transaction.
	// The modified and return status determine how to update/refresh.
	// If tabsheet is APPS/DOCS/VIEWS, then per_id cannot be undefined.
	int app_entries = 0;
	id_t document_id = UNDEFINED; // Fpending1 uses Fmain1->doc_id.
	if(tabsheet == Farchive1->DOCS) {document_id = doc_id;}
	TModalResult result = Farchive1->view(per_id,
		app_id,document_id,tabsheet,app_entries);
	// A rollback (result = mrAbort) clears the modified
	// values of both Farchive1 and Fmain1. The supplied
	// ids are set to UNDEFINED at the time Farchive1 is
	// closed. This ensures view_archive aborts properly.
	if(result == mrAbort) {abort1Click(this);}
	else if(Farchive1->getModified()) {modified = true;}
	else if(Fpending1->getModified()) {modified = true;}
	// Because view_archive can be called from Fpending1
	// and auto_save may be false, the modified value of
	// Fpending1 should be checked. Fcreated1/Fentries1
	// also call view_archive but never update modified.
	if(result != mrAbort && (modified == false ||
		TFoptions1::auto_save)) {save1Click(this);}
	// Release locks whenever this is possible.
	// Selections should not be cleared here.
	if(result != mrAbort && Farchive1->refresh_info) {
		vector<Cell> h_cells = hash->toVector();
		for(cell_it cell = h_cells.begin();cell != h_cells.end();cell++) {
			// Show changed data in the hash cells. MS_SQL always reads
			// most recent data whereas INNODB may overwrite recent data.
			// Note that refreshGrid must follow to show the updated hash.
			cells.readData(*cell); reHash(*cell,*cell,false);
		}
		if(app_entries == 0) {refreshGrid();} // refresh after save
	}
	if(app_entries) {selectEntries(app_entries);} // checks for a valid app_id.
	if(result == mrAll && Fschedule1->Visible == false) {schedule1Click(this);}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::archive1Click(TObject *Sender) {
	if(TFoptions1::o.remember == false) {per_id = UNDEFINED;}
	app_id = UNDEFINED; // doc_id = UNDEFINED;
	view_archive(Farchive1->PERSONS);
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::dayView1Click(TObject *Sender) {
	viewWeek1->Checked = false; viewWeek1Click(Sender);
	ActiveControl = DatePicker1;
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::weekView1Click(TObject *Sender) {
	viewWeek1->Checked = true; viewWeek1Click(Sender);
	ActiveControl = DatePicker1;
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::viewWeek1Click(TObject *Sender) {
	mode_t new_mode = viewWeek1->Checked ? WEEK_MODE : DAY_MODE;
	if(mode != new_mode) {mode = new_mode; Refresh();}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::changeScheduleClick(TObject *Sender) {
	if(RowGrid->DragState != Rowgrid::dsNone &&
		(RowGrid->DragState == Rowgrid::dsInsert ||
		RowGrid->CellState != Rowgrid::csCenter)) {return;}
	TMenuItem * sch_item = (TMenuItem *) Sender;
	Schedule schedule = schedules[sch_item->MenuIndex-VIEWMENUCOUNT];
	if(grid.id != schedule.id) {grid = schedule; Refresh();}
	// != also required when using ShortCuts
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::viewVertical1Click(TObject *Sender) {
	RowGrid->VerticalRows = viewVertical1->Checked;
	if(RowGrid->VerticalRows) {setVertical();}
	else {setHorizontal();}
	Refresh();
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::viewCancelled1Click(TObject *Sender) {refreshGrid();}
//---------------------------------------------------------------------------

void __fastcall TFmain1::options1Click(TObject *Sender) {
	Foptions1->view(Fpending1->RowControl->ActivePageIndex);
	KeepAlive->Interval = TFoptions1::o.time * 60000;
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::rollback(void) {
	LOGFUNC();
	TFbase1::rollback();
	// In case of rollback do reHash because otherwise the user may be under
	// the impression that an update succeeded after rollback. After commit,
	// the hash already contains the user updates and should not be reloaded.
	vector<Cell> h_cells = hash->toVector();
	for(cell_it cell = h_cells.begin();cell != h_cells.end();cell++) {
		// Do not check for data changes and consequently do not detect key
		// changes: When the rollback was caused by a TRYLOCK failure, the
		// user was already informed. When the user explicitely reverts, it
		// is possible that the hash (selection) silently changes (desirable?).
		cells.readCell(*cell); reHash(*cell,*cell,false);
	}
	// clearDragState after commit/rollback:
	// If the user is dragging a cell and commits, the cell being dragged
	// is lost. If we would not do this, then a subsequent failing update
	// would have to rollback to a state where a cell was being dragged.
	// But at that point drag cell does not contain recent data anymore.
	RowGrid->ClearDragState(); // after rollback clear dsDrag state
	Refresh(); // Refresh reloads schedule unit settings
}
//---------------------------------------------------------------------------

bool __fastcall TFmain1::reHash(Cell & hash_cell,Cell & new_cell,bool compare) {
	LOGFUNC(); Cell old_cell;
	hash_cell.selected = hash->search(hash_cell,old_cell);
	PROG_IF(hash_cell.selected == false && compare,"invalid hash cell");
	if(hash_cell.selected) {
		hash->erase();
		if(compare) {new_cell.changed = new_cell != old_cell;}
		// keep the value of changed when compare == false
		if(hash->search(new_cell,old_cell)) {hash->erase();}
		hash->insert(new_cell); new_cell.selected = true;
		hash_sorted = false; return new_cell.changed;
	}
	return false; // return not changed
}
//---------------------------------------------------------------------------

// the entries table can always contain entries that do not match the schedule
// settings. The user has already been warned that changing these settings is
// not recommended. If this is still done, no other clients should be active.
bool __fastcall TFmain1::checkCell(Cell & hash_cell,Cell * drag) {
	LOGFUNC();

	// acquire a lock on hash_cell.entry
	employees_locked.erase(hash_cell.entry.employee_id);
	Cell old_cell = hash_cell; old_cell.changed = false;
	TRYULOCK(
		// update hash_cell information when lock
		// succeeds to minimize database accesses.
		try {cells.readCell(hash_cell,drag);}
		catch(DBException & db_ex) {
			if(db_ex.errors.size() &&
				db_ex.errors[0].native_err == LOCK_TIMEOUT) {
				employees_locked.insert(old_cell.entry.employee_id);
				old_cell.changed = true; reHash(old_cell,old_cell,false);
			} else {throw;} // Integrity failures are not expected here
		}
	);
	hash_cell.selected = true; // supplying a -hash- cell
	if(old_cell.changed) {hash_cell.changed = true;} // lock failed, show cell
	else {EmplRows->Invalidate();} // lock acquired, reHash compares entry keys
	return hash_cell.changed;
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::insertHashCell(Cell & cell) {
	cell.entry.schedule_id = grid.id;
	cell.entry.schedule_name = grid.name;
	hash->insert(cell); cell.selected = true;
	hash_sorted = false; setCountText();
	// the value of cell.selected depends on the hash but not the other
	// way around so the selected value of the hash cell is undefined.
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::removeHashCell(Cell & cell) {
	PROG_IF(hash->search(cell,cell) == false,"invalid selection");
	hash->erase(); cell.selected = false;
	hash_sorted = false; setCountText();
}
//---------------------------------------------------------------------------

// remove the selection without accessing the database
void __fastcall TFmain1::removeSelection(bool changed_only) {
	LOGFUNC();
	row_t r_cells; unsigned rc = 0;
	for(cells_t::iterator row = RowGrid->Cells.begin();
		row != RowGrid->Cells.end();row++,rc++) {
		for(row_t::iterator rowcell = row->begin();
			rowcell != row->end();rowcell++) {
			Cell * cell = (Cell *) rowcell->Object;
			if(changed_only == false || cell->changed) {
				if(cell->planned) {
					cell->selected = false;
					// hash is cleared later
					cell->changed = false;
					rowcell->Zorder = PLANNED;
					if(cell->entry.cancelled) {
						rowcell->Zorder = CANCELLED;
					}
				} else {r_cells.push_back(*rowcell);}
			}
		}
		RowGrid->SortRow(rc); // Zorder changed
	}
	// remove empty cells (not planned)
	for(row_t::iterator rowcell = r_cells.begin();
		rowcell != r_cells.end();rowcell++) {
		delete (Cell *) rowcell->Object;
		RowGrid->RemoveRowCell(*rowcell);
	} // no need to sort when removing RowCells

	if(changed_only) {
		vector<Cell> h_cells = hash->toVector();
		for(cell_it cell = h_cells.begin();cell != h_cells.end();cell++) {
			if(cell->changed) {removeHashCell(*cell);}
		}
	} else {hash->clear(); setCountText();} // clear -entire- hash
	RowGrid->Invalidate();
}
//---------------------------------------------------------------------------

// INNODB: refreshGrid will display old data whenever a selected cell was
// deselected and unmodified until the next commit/rollback. It may even be
// that a selected cell is modified so its changes are visible here, but
// the user of the cell is not yet visible. This results in a blocked cell
// although it is not actually blocked.
void __fastcall TFmain1::refreshGrid(void) {
	LOGFUNC();
	try {
		int vert_pos = ScrollBox->VertScrollBar->Position;
		int horz_pos = ScrollBox->HorzScrollBar->Position;
		getGrid(); RowGrid->Invalidate(); setCountText();
		if(hash_only == false) {
			if(RowGrid->VerticalRows) {
				EmplRows->ColCount = employees.size();
				EmplRows->RowCount = 1;
			} else {
				EmplRows->RowCount = employees.size();
				EmplRows->ColCount = 1;
			}
			setSize();
			ScrollBox->VertScrollBar->Position = vert_pos;
			ScrollBox->HorzScrollBar->Position = horz_pos;

			// when current_date changes,
			// update time labels as well.
			int dunits = grid.dstop - grid.dstart;
			if(mode == DAY_MODE) {
				for(int i=0;i < dunits;i+=TFoptions1::o.labels) {
					TDateTime time(UnitsToDate(i,MAP_LEFT));
					AnsiString lbl = FormatDateTime("h':'mm",time);
					if(RowGrid->VerticalRows) {
						TimeLabels->Cells[0][i/TFoptions1::o.labels] = lbl;
					} else {TimeLabels->Cells[i/TFoptions1::o.labels][0] = lbl;}
				}
			} else if(mode == WEEK_MODE) {
				for(int i=0;i < TFoptions1::o.days;i++) {
					TDateTime date(UnitsToDate(i*dunits,MAP_LEFT));
					AnsiString lbl = FormatDateTime("ddd','d",date);
					if(RowGrid->VerticalRows) {
						lbl += "\n ("+IntToStr(WeekOf(date))+")";
						TimeLabels->Cells[0][i] = lbl;
					} else {
						lbl += " ("+IntToStr(WeekOf(date))+")";
						TimeLabels->Cells[i][0] = lbl;
					}
				}
			}
		}
	} catch(Exception & e) {
		AnsiString message("Error could not refresh, reason:");
		e.Message.Insert(message,1);
		throw;
	}
}
//---------------------------------------------------------------------------

// The left inclusive, right exclusive strategy cannot be used because it
// is not known from the right exclusive date how much exclusive this is.
// (this depends on the units settings which the user can change later).
int __fastcall TFmain1::DateToUnits(TDateTime date,mdir_t map_direction) {
	//    +-start-+--rgrid.units--+-------+---------options.units---------+
	// t: 2______1/4_____2/4_____3/4______3______1/4_____2/4_____3/4______4
	//    |       |       |       |       |       |       |       |       |
	//    |XXXXXXX|       |       |XXXXXXX|XXXXXXX|       |       |XXXXXXX|
	//    |_______|_______|_______|_______|_______|_______|_______|_______|
	// u: |       --->0   --->1  /        |       --->2   --->3           |
	//			 /              /
	// t:     1/4___RowCell___3/4
	//         |       .       |
	//         |       .       |
	//         |_______._______|
	// u: RIGHT--->0       1<---LEFT
	// The date argument should be positive and within a visible range. The
	// map_direction should not map outside of this range. When the user drags
	// a RowCell, UnitsToDate returns dates that are within a visible range.
	// RowCells should not be dragged such that date becomes negative. In other
	// words, RowCells should not be dragged before '1899-12-30 00:00:00'

	int wspace,dspace;
	int tunits = grid.tunits;
	int dunits = grid.dstop - grid.dstart;
	int wunits = grid.wstop - grid.wstart;
	TDateTime current(StartOfTheWeek(current_date));
	double days = date.Val - current.Val;

	int u;
	if(map_direction == MAP_LEFT) {
		u = RoundTo((days*tunits)+0.499999,0); u = u - 1;
	} else {u = RoundTo((days*tunits)-0.499999,0);}
	// Dates that are between the dates of unit boundaries are rounded
	// towards the nearest unit. The values -0.499999 and 0.499999 are
	// added to ensure we always get at least one unit. The value 0.5
	// cannot be used because then rounding depends on even/odd units.

	if(u >= 0) {
		wspace = grid.wstart;
		dspace = grid.dstart;
	} else {
		wspace = grid.wstop - DAYS_IN_WEEK;
		dspace = grid.dstop - tunits;
	}

	int day_units = u % grid.tunits;
	int week_days = (u / grid.tunits) % DAYS_IN_WEEK;
	if(u < 0) {day_units += grid.tunits; week_days += DAYS_IN_WEEK-1;}
	if(day_units < grid.dstart || day_units >= grid.dstop ||
		week_days < grid.wstart || week_days >= grid.wstop) {
		// After having added caldav support, devices may
		// actually insert entries that are not visible.
		// throw UserException(FMAIN_ENTRYINVISIBLEDATE);
	}

	int units = u;
	u = u - (int) (units/tunits)*(tunits - dunits);
	u = u - (int) (units/(tunits*DAYS_IN_WEEK))*(DAYS_IN_WEEK - wunits)*dunits;
	u = u - wspace*dunits - dspace; // substract for this day and week
	u = u - (DaySpan(current_date,current) - grid.wstart)*dunits;
	return u;
}
//---------------------------------------------------------------------------

TDateTime __fastcall TFmain1::UnitsToDate(int u,mdir_t map_direction) {
	//    +-start-+--rgrid.units--+-------+---------options.units---------+
	// t: 2______1/4_____2/4_____3/4______3______1/4_____2/4_____3/4______4
	//    |       |       |       |       |       |       |       |       |
	//    |XXXXXXX|       |       |XXXXXXX|XXXXXXX|       |       |XXXXXXX|
	//    |_______|_______|_______|_______|_______|_______|_______|_______|
	// u: |       <---0   <---1  /        |       <---2   <---3           |
	//			 /              /
	// t:     1/4___RowCell___3/4
	//         |       .       |
	//         |       .       |
	//         |_______._______|
	// u:  LEFT<---0       1--->RIGHT
	// RowCell unit 1 should be mapped to date 2.75. When RowCell.Units is used,
	// unit 2 of the following period is converted to date 3.25 which is wrong.
	// This is solved by supplying UnitsToDate with RowCell.Units-1 and setting
	// map direction to MAP_RIGHT.
	int dspace,wspace;
	int tunits = grid.tunits;
	int dunits = grid.dstop - grid.dstart;
	int wunits = grid.wstop - grid.wstart;
	TDateTime current(StartOfTheWeek(current_date));
	u = u + (DaySpan(current_date,current) - grid.wstart) * dunits;

	int units;
	if(u >= 0) {
		units = u;
		wspace = grid.wstart;
		dspace = grid.dstart;
	} else {
		units = u + 1;
		wspace = grid.wstop - DAYS_IN_WEEK;
		dspace = grid.dstop - tunits;
	}

	u = u + (int) (units/dunits)*(tunits - dunits);
	u = u + (int) (units/(dunits*wunits))*(DAYS_IN_WEEK - wunits)*tunits;
	u = u + wspace*tunits + dspace; // add for this day and week

	TDateTime date;
	if(u >= 0) {date = current + TDateTime((double) +u/tunits);}
	else {date = current - TDateTime((double) -u/tunits);}
	// -u: use positive TDateTime values only

	if(map_direction == MAP_RIGHT) {date = date + 1.0/tunits;}
	// compensate when the user drags cells out of range
	if(date < min_date+min_time) {date = min_date+min_time;}
	if(date > max_date+max_time) {date = max_date+max_time;}
	return date;
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::setSize(void) {
	LOGFUNC();
	int frw = RowGrid->FrameWidth; int rlw = RowGrid->RowLineWidth;
	int prc = TFoptions1::o.rows; int grc = RowGrid->RowCount;
	if(prc == 0) {prc = max(1,grc);} bool fits = grc <= prc;
	if(RowGrid->VerticalRows) {
		// first set all the widths so ScrollBox->ClientHeight becomes:
		// ScrollBox->Height - height of inserted horizontal scrollbar
		RowGrid->RowHeight = (ScrollBox->Width - 2*frw - (prc+1)*rlw)/prc;
		RowGrid->ClientWidth = RowGrid->RowHeight*grc + (grc+1)*rlw;
		EmplRows->DefaultColWidth = RowGrid->RowHeight;
		EmplRows->Width = RowGrid->ClientWidth - rlw;
		Shape1->Left = EmplRows->Left + EmplRows->Width;
		Shape2->Width = Shape1->Left - Shape4->Left + Shape4->Width;

		RowGrid->ClientHeight = 0; // remove vertical scrollbar if any.
		int hgt = ScrollBox->ClientHeight - EmplRows->Height - Shape2->Height;
		if(fits) {hgt = ScrollBox->Height - EmplRows->Height - Shape2->Height;}
		hgt = (hgt - 2*frw)/RowGrid->Units; if(hgt == 0) {hgt = 1;}
		RowGrid->UnitWidth = hgt; RowGrid->ClientHeight = hgt*RowGrid->Units;
		TimeLabels->Height = RowGrid->ClientHeight + 2; // +2: stringgrid border
	} else {
		// first set all the heights so ScrollBox->ClientWidth becomes:
		// ScrollBox->Width - width of inserted vertical scrollbar
		RowGrid->RowHeight = (ScrollBox->Height - 2*frw - (prc+1)*rlw)/prc;
		RowGrid->ClientHeight = RowGrid->RowHeight*grc + (grc+1)*rlw;
		EmplRows->DefaultRowHeight = RowGrid->RowHeight;
		EmplRows->Height = RowGrid->ClientHeight - rlw;
		Shape1->Top = EmplRows->Top + EmplRows->Height;
		Shape2->Height = Shape1->Top - Shape4->Top + Shape4->Height;

		RowGrid->ClientWidth = 0; // remove horizontal scrollbar if any.
		int wdt = ScrollBox->ClientWidth - EmplRows->Width - Shape2->Width;
		if(fits) {wdt = ScrollBox->Width - EmplRows->Width - Shape2->Width;}
		wdt = (wdt - 2*frw)/RowGrid->Units; if(wdt == 0) {wdt = 1;}
		RowGrid->UnitWidth = wdt; RowGrid->ClientWidth = wdt*RowGrid->Units;
		TimeLabels->Width = RowGrid->ClientWidth + 2; // +2: stringgrid border
	}

	int label_width = RowGrid->UnitWidth * TFoptions1::o.labels;
	int dunits = grid.dstop - grid.dstart;
	int day_width = dunits * RowGrid->UnitWidth;
	if(RowGrid->VerticalRows) {
		if(mode == DAY_MODE) {TimeLabels->DefaultRowHeight = label_width;}
		else if(mode == WEEK_MODE) {TimeLabels->DefaultRowHeight = day_width;}
	} else {
		if(mode == DAY_MODE) {TimeLabels->DefaultColWidth = label_width;}
		else if(mode == WEEK_MODE) {TimeLabels->DefaultColWidth = day_width;}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::addMenuItem(const Schedule & schedule) {
	TMenuItem * item = new TMenuItem(this);
	item->Caption = schedule.name;
	item->OnClick = changeScheduleClick;
	Word key = Word('1' + int(view_menu->Count-VIEWMENUCOUNT));
	item->ShortCut = ShortCut(key,TShiftState() << ssCtrl);
	view_menu->Add(item);
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::getGrid(void) {
	LOGFUNC();
	clearRowGrid(); // clearRowGrid before addEmployee
	if(hash_only == false) {
		id_t drag_employee = UNDEFINED;
		if(RowGrid->DragState != Rowgrid::dsNone &&
			(RowGrid->DragState == Rowgrid::dsInsert ||
			RowGrid->CellState != Rowgrid::csCenter)) {
			drag_employee = employees[RowGrid->DragCell.Row].id;
			// use DragCell instead of drag: OnDragRowCell
			// is not called when DragState = dsInsert
		}
		employees.clear();
		dragging = mousedown = false;
		addEmployee(persons,UNDEFINED);
		// todo: make display employees for planned cells only an option?
		// if(weekDate(Date(),Date()) <= current_date) {addEmployee(UNDEFINED);}
		if(drag_employee != UNDEFINED) {
			unsigned int row = findEmployee(drag_employee);
			if(row != employees.size() || addEmployee(persons,drag_employee)) {
				RowCell RowCell(row,RowGrid->DragCell.Start,
					RowGrid->DragCell.Units,RowGrid->DragCell.Zorder,
					RowGrid->DragCell.Object);
				RowGrid->DragCell = RowCell;
			}
		}
	}
	// Insert row cells using both database data and hash entries. The entry
	// keys of the hash may conflict with the entry keys read from the database
	// This is because checkCell may read updated keys from the database or the
	// user sets a range that overlaps with other entries. It may also be the
	// case that getGrid is called before checkCell, so an hash cell contains
	// data from an older snapshot. The user is informed when these conflicts
	// are detected at the moment of performing updates. From then on, the user
	// works with the changed but now locked and recent selection. Entry
	// overlaps are dragged/displayed according the priority of an entry:
	// database entry < hash entry < changed entry (also a hash entry).
	// To see how conflicts are detected look at checkCell/readCell.

	// using current_date, determine the visible range by setting begin and end
	// date. An entry should never contain a date that is out of visible range.
	begin_date = UnitsToDate(0,MAP_LEFT);
	int dunits = grid.dstop - grid.dstart;
	if(mode == DAY_MODE) {
		end_date = UnitsToDate(dunits - 1,MAP_RIGHT);
	} else if(mode == WEEK_MODE) {
		end_date = UnitsToDate(TFoptions1::o.days*dunits - 1,MAP_RIGHT);
	}

	// for each hash cell determine if it is to be readded to the grid
	vector<Cell *> g_cells; vector<Cell> h_cells = hash->toVector();
	for(cell_it cell = h_cells.begin();cell != h_cells.end();cell++) {
		Entry entry = cell->entry;
		if(Entry::less_than(entry.from,end_date) &&
			Entry::less_than(begin_date,entry.until)) {
			if(findEmployee(entry.employee_id) == employees.size()) {
				if(entry.schedule_id == grid.id) {
					if(addEmployee(persons,entry.employee_id) == false) {
						removeHashCell(*cell); continue;
						// person was deleted or hidden
					}
				} else if(findSchedule(entry.schedule_id) == schedules.size() &&
					addSchedule(scheds,entry.schedule_id) == false) {
					if(addEmployee(persons,entry.employee_id) == false) {
						removeHashCell(*cell); continue;
						// person was deleted or hidden
					} else {cell->entry.schedule_id = grid.id;}
				} else {continue;} // entry is available in another schedule
			}
			Cell * new_cell = new Cell(); *new_cell = *cell;
			// selected value of the hash cell itself is not updated
			new_cell->selected = true; // keep cell.changed/planned value
			g_cells.push_back(new_cell);
		}
	}

	if(hash_only == false) {RowGrid->RowCount = employees.size();}
	if(employees.empty()) {return;}

	//*************************************************************************
	if(hash_only == false) {
		AnsiString from = YMDHMS(begin_date);
		AnsiString until = YMDHMS(end_date);
		// Get appointments that will occupy the grid. right join ensures that
		// blocked entries, which have entries.app_id set to UNDEFINED, appear.
		AnsiString select = "select * from (apps right join entries on "
			"apps.APP_ID=entries.APP_ID) left join personal on "
			"apps.PERSONAL_ID=personal.PERSONAL_ID "
			"where entries.EMPLOYEE_ID in (";

		vector<Person>::const_iterator it = employees.begin();
		for(unsigned int r=0;it != employees.end();it++,r++) {
			select += it->id;
			if(r != employees.size()-1) {select += ",";}
		} select += ") and ";

		// |s1---e1|  |s2---e2|, choose |s2:FROM---e2:UNTIL|, s1:from, e1:until
		// no overlap:  (s2 >= e1 && e2 >= e1) || (s2 <= s1 && e2 <= s1), s <= e
		//    overlap: -(s2 >= e1 || e2 <= s1) -> s2 < e1 && e2 > s1
		select += "DATE_FROM<" DS+until+DS " and DATE_UNTIL>" DS+from+DS
			" order by EMPLOYEE_ID,DATE_FROM,DATE_UNTIL,CANCELLED desc";
			// MS_SQL: order by forces MS_SQL server to use primary index order
			// instead of the column order (differs from primary index order).

		cells.execSQL(select,ROWSET_SIZE);
		while(cells.next()) {
			Cell cell;
			cell.planned = true;
			cell.entry.get(cells);
			cell.entry.schedule_id = grid.id;
			cell.entry.schedule_name = grid.name;

			Cell h_cell; Cell * drag = (Cell *) RowGrid->DragCell.Object;
			if((cell.entry.cancelled && viewCancelled1->Checked == false) ||
				(hash->search(cell,h_cell) && h_cell.planned &&
				h_cell.entry.app_id == cell.entry.app_id) ||
				(RowGrid->DragState == Rowgrid::dsDragDrop &&
				drag->entry.app_id == cell.entry.app_id &&
				drag->entry == cell.entry)) {continue;}
			// If we would insert both the database cell and the hash cell, a
			// drag operation would only remove the selected version from the
			// rowgrid. To prevent this, only the hash cell is inserted. INNODB:
			// If a cell is not planned anymore, h_cell.planned is false. When
			// the hash_cell is deselected, the removed cell will show up again
			// until a new database snapshot is obtained.

			cell.data.get(cells);
			Cell * new_cell = new Cell(); *new_cell = cell;
			// selected value of the hash cell itself is not updated
			new_cell->selected = false; // cell.changed/planned already set
			g_cells.push_back(new_cell);
		}
	}
	//*************************************************************************

	int start,units; bool proceed = false;
	vector<Cell *>::const_iterator cell_it = g_cells.begin();
	for(;cell_it != g_cells.end();cell_it++) {
		Cell * cell = *cell_it;
		unsigned int row = findEmployee(cell->entry.employee_id);
		bool do_continue = row == employees.size();
		if(do_continue) {
#ifdef _DEBUG
			PROG_IF(cell->entry.schedule_id == grid.id,
				"entry record without personal record");
#endif
			if(cell->entry.schedule_id == grid.id && cell->selected) {
				removeHashCell(*cell); delete cell; // do not inform end user
			}
		} else {
			try {
				start = DateToUnits(cell->entry.from,MAP_RIGHT);
				units = DateToUnits(cell->entry.until,MAP_LEFT)-start+1;
			} catch(UserException & e) {
				if(cell->selected) {removeHashCell(*cell);} delete cell;
				if(proceed == false) {Exceptions::handleException(this,&e);}
				// do_continue is necessary for proper exception cleanup
				proceed = true; do_continue = true;
			}
		} if(do_continue) {continue;}

		if(cell->entry.app_id != UNDEFINED) {
			if(inserting || (hash_only == false &&
				cell->planned && cell->entry.cancelled == false)) {
				employees[row].total_diff += cell->entry.date_diff;
				employees[row].entry_count++;
			} else if(deleting) {
				// assumes app_id is still defined after deleting.
				employees[row].total_diff -= cell->entry.date_diff;
				employees[row].entry_count--;
			}
		}
		int zorder = PLANNED; if(cell->entry.cancelled) {zorder = CANCELLED;}
		if(cell->selected) {zorder = cell->changed ? CHANGED : SELECTED;}
		RowCell RowCell(row,start,units,zorder,cell);
		RowGrid->InsertRowCell(RowCell);
	}
	for(int r=0;r != RowGrid->RowCount;r++) {RowGrid->SortRow(r);}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::entriesNew1Click(TObject *Sender) {
	LOGFUNC();
	if(hash->elementCount() == 0) {
		throw UserException(FMAIN_SELECTENTRIESFIRST);
	}

	// Before inserting, check if all cells are actually empty
	// If someone else inserts one, we will see changes later.
	vector<Cell> h_cells = hash->toVector(); bool conflict = false;
	for(cell_it cell = h_cells.begin();cell != h_cells.end();cell++) {
		cell->changed = false; // reset changed value and reHash
		if(cell->planned) {conflict = true; cell->changed = true;}
		reHash(*cell,*cell,false);
	} SHOWCONFLICT(conflict,FMAIN_CANNOTINSERTONTOP);

	// If Fschedule1 is visible, app_id is properly defined.
	// Otherwise set app_id=UNDEFINED to add blocked entries.
	if(Fschedule1->Visible == false) {app_id = UNDEFINED;}

	// The following code does not check for overlapping selections that are
	// inserted. We allow for this to happen because the costs of checking
	// overlaps are not worth it. The user can always fix overlaps because
	// an exact overlap is prevented by not allowing duplicate hash entry keys.

	// update lock cells
	vector<Cell> old_cells;
	for(cell_it cell = h_cells.begin();cell != h_cells.end();cell++) {
		old_cells.push_back(*cell); // keys may change after checkCell
		conflict = checkCell(*cell) || conflict;
	} SHOWCONFLICT(conflict,SELECTION_LOCKED); // releases locks when conflict

	// reload the cells and the hash with the most recent data
	cell_it locked = old_cells.begin();
	for(cell_it cell = h_cells.begin();cell != h_cells.end();cell++,locked++) {
		conflict = reHash(*locked,*cell,true) || conflict;
	} SHOWCONFLICT(conflict,SELECTION_CHANGED);

	// insert entries and reload the hash with the new data
	Cell new_cell; new_cell.entry.app_id = app_id;
	TRYSLOCK(cells.readData(new_cell);); // updates planned/cancelled
	// INNODB: lock succeeds because checkCell locked app_id
	for(cell_it cell = h_cells.begin();cell != h_cells.end();cell++) {
		cell->entry.app_id = new_cell.entry.app_id;
		cell->entry.cancelled = new_cell.entry.cancelled;
		cell->entry.pattern = new_cell.entry.pattern;
		cell->entry.uuid = new_cell.entry.uuid;
		// date_diff came from database cell or was calculated
		// total_diff is updated when refreshGrid() is called
		TRYSLOCK(entries.insert(cell->entry); modified = true;);
		// todo: this could probably be written as a 'batch' insert.
		// if app_id does not exist, insert fails (integrity)
		cell->planned = true; cell->data = new_cell.data;
		reHash(*cell,*cell,false);
	}

	if(TFoptions1::auto_save) {save1Click(Sender);} // save and release
	inserting = hash_only = true;
	try {refreshGrid();} __finally {inserting = hash_only = false;}
	if(TFoptions1::o.clear_after) {removeSelection();}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::editDelete1Click(TObject *Sender) {
	LOGFUNC();
	// allow entries to be deleted/blocked while the user is dragging a cell.
	// save1Click ensures nothing is saved until the cell is dropped. Note that
	// regular entries cannot be scheduled because then Farchive may save.
	if(hash->elementCount() == 0) {
		throw UserException(FMAIN_SELECTENTRIESFIRST);
	}

	// Before deleting, check if all cells are actually planned.
	// If someone else deletes one, we will see changes later.
	vector<Cell> h_cells = hash->toVector(); bool conflict = false;
	for(cell_it cell = h_cells.begin();cell != h_cells.end();cell++) {
		cell->changed = false; // reset changed value and reHash
		if(cell->planned == false) {conflict = true; cell->changed = true;}
		reHash(*cell,*cell,false);
	} SHOWCONFLICT(conflict,FMAIN_CANNOTMODIFYEMPTY);

	if(root_user == false) {
		AnsiString username = db->getUsername();
		for(cell_it cell = h_cells.begin();cell != h_cells.end();cell++) {
			if(cell->data.exclusive && cell->data.created_by != username) {
				conflict = true; cell->changed = true;
				reHash(*cell,*cell,false);
			}
		} SHOWCONFLICT(conflict,ITEMSACCESSDENIED);
	}

	AnsiString message;
	if(hash->elementCount() > 1) {
		message = AnsiString::LoadStr(FMAIN_NUMBEROFENTRIESDELETE)+
			IntToStr((int) hash->elementCount())+", "+
			AnsiString::LoadStr(CONTINUEQUESTION);
	} else {message = AnsiString::LoadStr(FMAIN_DELETEENTRYQUESTION);}
	if(MSGYN(message.c_str()) == ID_NO) {return;}

	// update-lock cells
	vector<Cell> old_cells;
	for(cell_it cell = h_cells.begin();cell != h_cells.end();cell++) {
		old_cells.push_back(*cell); // keys may change after checkCell
		conflict = checkCell(*cell) || conflict;
	} SHOWCONFLICT(conflict,SELECTION_LOCKED);

	// reload the cells and the hash with the most recent data
	cell_it locked = old_cells.begin();
	for(cell_it cell = h_cells.begin();cell != h_cells.end();cell++,locked++) {
		conflict = reHash(*locked,*cell,true) || conflict;
	} SHOWCONFLICT(conflict,SELECTION_CHANGED);

	// delete entries, the appointment itself is not removed
	for(cell_it cell = h_cells.begin();cell != h_cells.end();cell++) {
		TRYSLOCK(entries.remove(cell->entry); modified = true;);
		cell->planned = false; reHash(*cell,*cell,false);
	}

	if(TFoptions1::auto_save) {save1Click(Sender);} // save and release
	deleting = hash_only = true;
	try {refreshGrid();} __finally {deleting = hash_only = false;}
	if(TFoptions1::o.clear_after) {removeSelection();}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::setRowCellPattern(TObject * Sender,
	TBrushStyle pattern) {
	LOGFUNC();
	if(hash->elementCount() == 0) {
		throw UserException(FMAIN_SELECTENTRIESFIRST);
	}

	// Before setting the pattern, check if all cells are actually
	// planned. If someone else sets it, we will see changes later.
	vector<Cell> h_cells = hash->toVector(); bool conflict = false;
	for(cell_it cell = h_cells.begin();cell != h_cells.end();cell++) {
		cell->changed = false; // reset changed value and reHash
		if(cell->planned == false) {conflict = true; cell->changed = true;}
		reHash(*cell,*cell,false);
	} SHOWCONFLICT(conflict,FMAIN_CANNOTMODIFYEMPTY);

	if(root_user == false) {
		AnsiString username = db->getUsername();
		for(cell_it cell = h_cells.begin();cell != h_cells.end();cell++) {
			if(cell->data.exclusive && cell->data.created_by != username) {
				conflict = true; cell->changed = true;
				reHash(*cell,*cell,false);
			}
		} SHOWCONFLICT(conflict,ITEMSACCESSDENIED);
	}

	if(hash->elementCount() > 1) {
		AnsiString message =
			AnsiString::LoadStr(FMAIN_NUMBEROFENTRIESMODIFY)+
			IntToStr((int) hash->elementCount())+", "+
			AnsiString::LoadStr(CONTINUEQUESTION);
		if(MSGYN(message.c_str()) == ID_NO) {return;}
	} // else {message = AnsiString::LoadStr(FMAIN_MODIFYENTRYQUESTION);}

	// update-lock cells
	vector<Cell> old_cells;
	for(cell_it cell = h_cells.begin();cell != h_cells.end();cell++) {
		old_cells.push_back(*cell); // keys may change after checkCell
		conflict = checkCell(*cell) || conflict;
	} SHOWCONFLICT(conflict,SELECTION_LOCKED);

	// reload the cells and the hash with the most recent data
	cell_it locked = old_cells.begin();
	for(cell_it cell = h_cells.begin();cell != h_cells.end();cell++,locked++) {
		conflict = reHash(*locked,*cell,true) || conflict;
	} SHOWCONFLICT(conflict,SELECTION_CHANGED);

	// set pattern
	for(cell_it cell = h_cells.begin();cell != h_cells.end();cell++) {
		cell->entry.pattern = pattern;
		TRYSLOCK(entries.update(cell->entry,cell->entry); modified = true;);
		reHash(*cell,*cell,false);
	}

	if(TFoptions1::auto_save) {save1Click(Sender);} // save and release
	hash_only = true; try {refreshGrid();} __finally {hash_only = false;}
	if(TFoptions1::o.clear_after) {removeSelection();}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::editSplit1Click(TObject *Sender) {
	LOGFUNC();
	if(hash->elementCount() == 0) {
		throw UserException(FMAIN_SETRANGEFIRST);
	}

	AnsiString message;
	if(hash->elementCount() > 1) {
		message = AnsiString::LoadStr(FMAIN_NUMBEROFENTRIESSPLIT)+
			IntToStr((int) hash->elementCount())+", "+
			AnsiString::LoadStr(CONTINUEQUESTION);
	} else {message = AnsiString::LoadStr(FMAIN_SPLITENTRYQUESTION);}
	if(MSGYN(message.c_str()) == ID_NO) {return;}

	// todo: review this because it seems dangerous not to know what you
	// are actually splitting (someone else may have replaced the cell.)
	// MS_SQL: it would be impossible to compare the cells below the ranges
	// because we first have to determine which cells belong to the ranges using
	// readCell. But in doing so, we may read more recent data. We chose to lock
	// only in order to prevent deadlocks at the cost of reading each cell twice
	// Note however that you do not know if the cell you think you are splitting
	// is actually the one being split. todo: this seems kind of unacceptable.
	vector<Cell> ranges = hash->toVector(); bool conflict = false;
	for(cell_it range = ranges.begin();range != ranges.end();range++) {
		Cell cell = *range; // keys may change after checkCell
		conflict = checkCell(cell) || conflict;
		if(conflict == false) {
			// Always deselect the cell being split. We are iterating over
			// cells while removing them. Therefore we must verify that
			// the cell being removed was not removed in an earlier pass.
			cell.selected = hash->search(cell,cell);
			if(cell.selected) {removeHashCell(cell);}
		}
	} SHOWCONFLICT(conflict,SELECTION_LOCKED);

	bool split_ok = true; AnsiString username = db->getUsername();
	for(cell_it range = ranges.begin();range != ranges.end();range++) {
		Cell cell; cell.entry = range->entry; TRYSLOCK(cells.readCell(cell););
		// readCell needed when several ranges on single planned cell were set
		// because the read cell might have been splitted in an earlier pass.
		// Also note that when the ranges overlap the split order is undefined.
		TDateTime range_from(range->entry.from);
		TDateTime range_until(range->entry.until);
		// note that cancelled cells cannot be split
		if(cell.planned == false || (root_user == false &&
			cell.data.exclusive && cell.data.created_by != username) ||
			Entry::less_than(range_from,cell.entry.from) || // range <= cell?
			Entry::less_than(cell.entry.until,range_until)) {
			if(cell.planned) {
				split_ok = false;
				range->changed = true;
				reHash(*range,*range,false);
			}
			continue;
		}

		Cell left = cell; Cell right = cell;
		bool insert_left = Entry::equal(cell.entry.from,range_from) == false;
		bool insert_right = Entry::equal(cell.entry.until,range_until) == false;
		bool do_continue = false; // necessary for proper exception cleanup
		try {
			if(insert_left) {
				left.planned = false;
				// Correct from when range starts at the begin of this day and
				// cell does not: cannot use DateToUnits(range_from,MAP_LEFT).
				int until = DateToUnits(range_from,MAP_RIGHT)-1;
				int start = DateToUnits(left.entry.from,MAP_RIGHT);
				left.entry.date_diff = (double) (until-start+1)/grid.tunits;
				left.entry.until = UnitsToDate(until,MAP_RIGHT);
			}
			if(insert_right) {
				right.planned = false;
				int start = DateToUnits(range_until,MAP_LEFT)+1;
				int until = DateToUnits(right.entry.until,MAP_LEFT);
				right.entry.date_diff = (double) (until-start+1)/grid.tunits;
				right.entry.from = UnitsToDate(start,MAP_LEFT);
			}
		} catch(UserException & e) {
			split_ok = false;
			range->changed = true;
			reHash(*range,*range,false);
			do_continue = true;
		} if(do_continue) {continue;}

		TRYSLOCK(
			// left and right locked by readCell
			if(insert_left) {entries.insert(left.entry);}
			if(insert_right) {entries.insert(right.entry);}
			entries.remove(cell.entry); // cell is not in hash
			modified = true;
		);
	}
	// hash->clear(): cell.selected set by refreshGrid
	if(split_ok && TFoptions1::o.clear_after) {hash->clear();}
	if(TFoptions1::auto_save) {save1Click(Sender);} refreshGrid();
	if(split_ok == false) {MSGO(AnsiString::LoadStr(FMAIN_SPLITFAIL).c_str());}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::entriesSelect1Click(TObject *Sender) {
	Farchive1->refresh_info = false; selectEntries(0);
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::selectEntries(unsigned int app_entries) {
	LOGFUNC();
	if(app_id == UNDEFINED) {return;}

	AnsiString f1,f2;
	if(root_user == false) {
		f1="inner join personal as y on entries.EMPLOYEE_ID=y.PERSONAL_ID ";
		f2="(y.HIDDEN=0 or y.CREATED_BY="+QuotedStr(db->getUsername())+") and ";
	}
	cells.execSQL("select * from ((entries inner join apps on "
		"entries.APP_ID=apps.APP_ID) inner join personal as x "
		"on apps.PERSONAL_ID=x.PERSONAL_ID) "+f1+"where "+f2+
		"apps.APP_ID="+app_id);

	removeSelection();
	while(cells.next()) {
		Cell cell; cell.entry.get(cells);
		cell.entry.schedule_id = grid.id;
		cell.entry.schedule_name = grid.name;
		cell.planned = !cell.entry.cancelled ||
			viewCancelled1->Checked;
		if(cell.planned) {cell.data.get(cells);}
		reHash(cell,cell,false); // needs schedule id/name
		if(cell.selected == false) {insertHashCell(cell);}
		// after insertHashCell, Zorder is set by refreshGrid
	}
	if(hash->elementCount()) {browseSelection(FIRST);}
	else if(Farchive1->refresh_info) {refreshGrid();}
	if(app_entries > hash->elementCount()) {
		AnsiString message(AnsiString::LoadStr(FMAIN_NUMBEROFENTRIESHIDDEN));
		MSGO((message+IntToStr((int) (app_entries-
			hash->elementCount()))).c_str());
	}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::editSetRange1Click(TObject *Sender) {
	LOGFUNC();
	int start,until;
	TDateTime begin,end;
	id_t select_id = UNDEFINED;
	if(hash->elementCount() == 1) {
		Cell cell = *(hash->toVector().begin());
		select_id = cell.entry.employee_id;
		begin = cell.entry.from;
		end = cell.entry.until;
		Frange1->DatePicker1->DateTime = int(begin);
		Frange1->DatePicker2->DateTime = int(end);
		Frange1->TimePicker1->DateTime = double(begin) - int(begin);
		Frange1->TimePicker2->DateTime = double(end) - int(end);
	}
	Frange1->SelectBox->Checked = false;
	// Frange::FormClose() ensures a valid
	// date range is specified and at least
	// one employee item has been selected.
	if(Frange1->view(begin,end,
		StrToInt(Frange1->CountEdit->Text),
		StrToInt(Frange1->TotalEdit->Text),
		select_id) != mrOk) {return;}
	int weeks = StrToInt(Frange1->CountEdit->Text);
	int total = StrToInt(Frange1->TotalEdit->Text);

	int count = INT_MAX;
	start = DateToUnits(Frange1->begin,MAP_RIGHT);
	until = DateToUnits(Frange1->end,MAP_LEFT);
	begin = UnitsToDate(start,MAP_LEFT);
	end = UnitsToDate(until,MAP_RIGHT);
	for(int week=0;week < total;week += weeks) {
		TDateTime range_from = begin + week * DAYS_IN_WEEK;
		TDateTime range_until = end + week * DAYS_IN_WEEK;
		if(Entry::less_than(max_date + max_time,range_until)) {break;}
		start = DateToUnits(range_from,MAP_RIGHT);
		until = DateToUnits(range_until,MAP_LEFT);
		if(Frange1->CheckBox->Checked) {
			AnsiString select =
				"select EMPLOYEE_ID,DATE_FROM,DATE_UNTIL,DATE_DIFF,PATTERN,"
				"UUID,entries.APP_ID,CANCELLED from ((entries left join "
				"apps on entries.APP_ID=apps.APP_ID) inner join personal "
				"on entries.EMPLOYEE_ID=personal.PERSONAL_ID) inner join "
				"employees on entries.EMPLOYEE_ID=employees.PERSONAL_ID "
				"where employees.SCHEDULE_ID="+grid.id+
				" and entries.EMPLOYEE_ID in (";
			list<id_t>::const_iterator it = Frange1->selected.begin();
			for(unsigned int r=0;it != Frange1->selected.end();it++,r++) {
				select += *it;
				if(r != Frange1->selected.size()-1) {select += ",";}
			} select += ") and ";
			select += "DATE_FROM<" DS+YMDHMS(range_until)+DS
				" and DATE_UNTIL>" DS+YMDHMS(range_from)+DS
				" order by EMPLOYEE_ORDER,DATE_FROM,DATE_UNTIL";
			cells.execSQL(select,ROWSET_SIZE);
			findEntries(Frange1->selected,count,start,until,1,&cells);
		} else {findEntries(Frange1->selected,count,start,until,1,NULL);}
	}
	hash_only = true; try {refreshGrid();} __finally {hash_only = false;}
	AnsiString message(AnsiString::LoadStr(FMAIN_NUMBEROFENTRIESSELECTED));
	MSGO((message+IntToStr((int) INT_MAX-count)).c_str());
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::editFindEmpty1Click(TObject *Sender) {
	LOGFUNC();
	TDateTime begin = begin_date;
	TDateTime end = end_date;
	Ffind1->DatePicker1->DateTime = int(begin);
	Ffind1->DatePicker2->DateTime = int(end);
	Ffind1->TimePicker1->DateTime = double(begin) - int(begin);
	Ffind1->TimePicker2->DateTime = double(end) - int(end);
	Ffind1->SelectBox->Checked = false;
	if(Ffind1->view(begin,end,1,1,grid.id) != mrOk) {return;}
	int count = StrToInt(Ffind1->CountEdit->Text);
	int min_units = StrToInt(Ffind1->TotalEdit->Text);

	removeSelection();
	list<id_t> employee_ids;
	Schedule restore_grid = grid;
	try {
		// Ffind dialog only displays schedules which are NOT HIDDEN.
		list<id_t>::const_iterator sch_it = Ffind1->selected.begin();
		for(int i=0;count && sch_it != Ffind1->selected.end();sch_it++,i++) {
			unsigned int grid_nr = findSchedule(*sch_it);
			PROG_IF(grid_nr == schedules.size(),"Schedule not added");
			grid = schedules[grid_nr];
			if(grid.id == UNDEFINED) {continue;}

			employee_ids.clear();
			getEmployeeIds(persons,grid.id,&employee_ids);
			int start = DateToUnits(Ffind1->begin,MAP_RIGHT);
			int until = DateToUnits(Ffind1->end,MAP_LEFT);
			TDateTime range_from = UnitsToDate(start,MAP_LEFT);
			TDateTime range_until = UnitsToDate(until,MAP_RIGHT);
			AnsiString select =
				"select EMPLOYEE_ID,DATE_FROM,DATE_UNTIL,DATE_DIFF,PATTERN,"
				"UUID,entries.APP_ID,CANCELLED from ((entries left join "
				"apps on entries.APP_ID=apps.APP_ID) inner join personal "
				"on entries.EMPLOYEE_ID=personal.PERSONAL_ID) inner join "
				"employees on entries.EMPLOYEE_ID=employees.PERSONAL_ID "
				"where employees.SCHEDULE_ID="+grid.id+
				" and DATE_FROM<" DS+YMDHMS(range_until)+
				DS" and DATE_UNTIL>" DS+YMDHMS(range_from)+DS;
			AnsiString restrict = "(HIDDEN=0 or personal.CREATED_BY="+
				QuotedStr(db->getUsername())+")";
			if(!root_user) {select += (" and "+restrict);}
			select += " order by EMPLOYEE_ORDER,DATE_FROM,DATE_UNTIL";
			cells.execSQL(select,ROWSET_SIZE);
			findEntries(employee_ids,count,start,until,min_units,&cells);
		}
	} __finally {grid = restore_grid;}
	browseSelection(FIRST);
	AnsiString message(AnsiString::LoadStr(FMAIN_NUMBEROFENTRIESSELECTED));
	MSGO((message+IntToStr(StrToInt(Ffind1->CountEdit->Text)-count)).c_str());
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::findEntries(list<id_t> employee_ids,
	int & count,int start,int until,int min_units,Cells * cells) {
	Entry next; Cell cell;
	int index = start;
	until = until+1;
	next.employee_id = UNDEFINED;
	list<id_t>::const_iterator emp_it = employee_ids.begin();
	if(emp_it != employee_ids.end()) {next.employee_id = *emp_it;}
	while(emp_it != employee_ids.end() && (
		count || next.employee_id != UNDEFINED)) {
		// continue even when count is zero,
		// the cells query must be cleared.
		if(*emp_it == next.employee_id) {
			next.employee_id = UNDEFINED;
			if(cells && cells->next()) {next.get(*cells);}
			if(*emp_it != next.employee_id) {
				if(count && until-index >= min_units) {
					cell.entry.employee_id = *emp_it;
					cell.entry.from = UnitsToDate(index,MAP_LEFT);
					cell.entry.until = UnitsToDate(until-1,MAP_RIGHT);
					cell.entry.date_diff = (double) (until-index)/grid.tunits;
					cell.selected = hash->search(cell,cell);
					if(cell.selected == false) {insertHashCell(cell); count--;}
				}
				index = start;
				emp_it++;
			}

			if(next.employee_id == UNDEFINED ||
				next.cancelled) {continue;}
			int nstart = DateToUnits(next.from,MAP_RIGHT);
			int nuntil = DateToUnits(next.until,MAP_LEFT);
			if(count && nstart-index >= min_units) {
				cell.entry.employee_id = next.employee_id;
				cell.entry.from = UnitsToDate(index,MAP_LEFT);
				cell.entry.until = UnitsToDate(nstart-1,MAP_RIGHT);
				cell.entry.date_diff = (double) (nstart-index)/grid.tunits;
				cell.selected = hash->search(cell,cell);
				if(cell.selected == false) {insertHashCell(cell); count--;}
			} index = nuntil+1;
		} else {
			if(count && until-start >= min_units) {
				cell.entry.employee_id = *emp_it;
				cell.entry.from = UnitsToDate(start,MAP_LEFT);
				cell.entry.until = UnitsToDate(until-1,MAP_RIGHT);
				cell.entry.date_diff = (double) (until-start)/grid.tunits;
				cell.selected = hash->search(cell,cell);
				if(cell.selected == false) {insertHashCell(cell); count--;}
			} emp_it++;
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::editDeselectAll1Click(TObject *Sender) {
	removeSelection(); // fast: no database access
}
//---------------------------------------------------------------------------

// disabled: this functionality does not seem to be very useful.
void __fastcall TFmain1::editDeselectChanged1Click(TObject *Sender) {
	removeSelection(true);
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::RowGridDragRowCell(TObject *Sender,
	RowCell &RowCell, TCellState CellState, bool IsCopy, bool &CanDrag) {
	LOGFUNC();
	Cell * drag = (Cell *) RowCell.Object;
	cell_swap = RowGrid->DragState != Rowgrid::dsNone;
	// put the cell being dragged in the hash. When checkCell detects changes,
	// this allows for RowCell.object to be refreshed by refreshGrid using the
	// updated information from the hash cell.
	if(drag->planned) { // prevent lock when dragging empty cells
		if(root_user == false && drag->data.exclusive &&
			drag->data.created_by != db->getUsername()) {
			throw UserException(ITEMSACCESSDENIED);
		}
		bool selected = drag->selected;
		if(selected == false) {insertHashCell(*drag);} // sets drag->selected
		Cell old_cell = *drag; // keys may change after checkCell
		bool conflict = checkCell(*drag);
		SHOWCONFLICT(conflict,SELECTION_LOCKED);
		// Consider this: someone else has moved drag to another location.
		// Then this user starts another drag operation on the moved cell.
		// At that point we start to drag and block with SHOWCONFLICT showing
		// the moved cell eventhough a drag operation is still in progress.
		conflict = reHash(old_cell,*drag,true);
		SHOWCONFLICT(conflict,SELECTION_CHANGED);
		if(selected == false) {removeHashCell(*drag);} // sets drag->selected
	}
	if(cell_swap) {EntryText->Caption = CellToInfo(*drag);}
	if(IsCopy) {Cell * cell = new Cell(); *cell = *drag; RowCell.Object = cell;}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::RowGridRemoveRowCell(TObject *Sender,
	RowCell &RowCell, bool &CanRemove) {
	LOGFUNC();
	Cell * drag = (Cell *) RowCell.Object;
	if(drag->planned) { // cell should exist in the database
/* disabled: using entries.update now
		TRYSLOCK(
			// not removing the appointment itself, keep data (info..)
			// entry and employee were shared locked by checkCell. Key
			// time range did not change either, remove should succeed.
			entries.remove(drag->entry);
			// when the update succeeds, the cell is deselected.
			// there is no need to read updates into the hash cell
			// Also do not set drag.planned. RowGridInsertRowCell uses
			// it to determine if drag was a planned cell in the past
			modified = true;
		); // We `manually' delete RowCell/object. Invalidate() by RowGrid
*/
		if(drag->entry.cancelled == false && drag->entry.app_id != UNDEFINED) {
			employees[RowCell.Row].total_diff -= drag->entry.date_diff;
			employees[RowCell.Row].entry_count--;
		}
	}
	// else: removing a newly inserted cell. A concurrent update cannot lead
	// to this as there would be hash cell changes in which case checkCell
	// complains. RemoveRowCell cannot be re-entered for an empty cell.
	// removeHashCell: remove dragged, selected cell from hash. If the cell
	// existed in the database it is deselected regardless of its old state
	// If the cell was newly inserted it will be reinserted by insertRowCell.
	// update failure: DragState not set, RowCell and its object are preserved

	bool selected = drag->selected; // keep the selected state
	if(selected) {removeHashCell(*drag);} // RowCell removed -> remove from hash
	drag->selected = selected; // restore RowCell selected state

	// cannot auto save here: refreshGrid() cannot be called because the
	// RowCell itself has not yet been removed by RowGrid. Otherwise RowCell
	// and its object would be deleted twice. This cannot be prevented by
	// setting CanRemove to false because this will prevent the dsDrag state
	// from being entered. we can auto save in RowGridInsertRowCell however.
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::RowGridInsertRowCell(TObject *Sender,
	RowCell &RowCell, bool &CanInsert) {
	LOGFUNC();

	if(RowGrid->DragState == Rowgrid::dsInsert && TFoptions1::o.clear_onnew) {
		// hash->clear(); refreshGrid(); <- invalid: getGrid may result
		// in a different employee list with RowCell.Row becoming invalid.
		removeSelection(); // calls RemoveRowCell but that should work ok
	}

	// The mapping of RowCell rows to employees is always valid as RowGrid is
	// initialized together with employees. The hash stores the mapped keys.
	Cell cell; // assume cell to be inserted is not planned
	cell.entry.employee_id = employees[RowCell.Row].id;
	cell.entry.from = UnitsToDate(RowCell.Start,MAP_LEFT);
	cell.entry.until = UnitsToDate(RowCell.Start+RowCell.Units-1,MAP_RIGHT);
	cell.entry.date_diff = (double) RowCell.Units/grid.tunits;

	// put the cell being dropped in the hash. When checkCell detects changes,
	// this allows for RowCell.object to be refreshed by refreshGrid using the
	// updated information from the hash cell.
	cell.selected = hash->search(cell,cell); // set cell.selected
	if(cell.selected == false) {insertHashCell(cell);} // depends on selected
	else {CanInsert = false;} // when inserting identical entries with ALT key

	Cell * drag = (Cell *) RowCell.Object;
	// RowCell may have been dragged after it was newly inserted. This means
	// that DragState != dsInsert and drag was initialized by DragRowCell.
	if(RowGrid->DragState != Rowgrid::dsInsert && drag->planned) {
		// drag was already checked/locked earlier. drag cell is not newly
		// inserted and is planned: It should have existed in the database.
		cell.planned = true; // required for checkCell
		cell.entry.app_id = drag->entry.app_id;
		cell.entry.cancelled = drag->entry.cancelled;
		cell.entry.pattern = drag->entry.pattern;
		cell.entry.uuid = drag->entry.uuid; // reuse uuid if dsDragDrop
		// Reuse app_id and its data (info,app_color,person). This also
		// allows for checkCell to detect overlapping cancelled cells.
		bool conflict; // Check and lock the cell that is dropped on.
		Cell old_cell = cell; // keys may change after checking the cell
		conflict = checkCell(cell,drag); // check the cell that is dropped on
		SHOWCONFLICT(conflict,SELECTION_LOCKED);
		conflict = reHash(old_cell,cell,true);
		SHOWCONFLICT(conflict,SELECTION_CHANGED);
		// RowGrid already prevents overlaps. CheckCell prevents overlaps
		// due to concurrent updates as it checks the key time range
		if(CanInsert) {
			PROG_IF(cell.planned,"Cell should not be planned");
			// CanInsert = false if a planned cell is resized and
			// now identical to an earlier selection on top of it
			TRYSLOCK( // must rollback when swapping cells
				if(RowGrid->DragState == Rowgrid::dsDragDrop) {
					entries.update(drag->entry,cell.entry);
				} else {entries.insert(cell.entry);}
				modified = true;
			);
			cell.planned = true;
			cell.data = drag->data;
			reHash(cell,cell,false);
			if(cell.entry.cancelled == false &&
				cell.entry.app_id != UNDEFINED) {
				employees[RowCell.Row].total_diff += cell.entry.date_diff;
				employees[RowCell.Row].entry_count++;
			}
			// selections should not be cleared here.
			if(cell_swap == false && TFoptions1::auto_save) {save();}
			// call save directly and ignore DragState.
		}
	} else if(cell_swap == false && modified && TFoptions1::auto_save) {save();}
	// else: this cell is really new, keep selected as this is what we want

	if(CanInsert) {
		if(RowGrid->DragState != Rowgrid::dsInsert && drag->selected == false) {
			removeHashCell(cell); // sets cell.selected, used by drag
		} else {RowCell.Zorder = SELECTED;} // SortRow() by RowGrid
		if(RowGrid->DragState == Rowgrid::dsInsert) {drag = new Cell();}
		*drag = cell; RowCell.Object = drag; // RowCell is inserted by RowGrid
		cell_swap = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::setCurrentDate(TDateTime date) {
	LOGFUNC();
	current_date = int(date);
	if(mode == WEEK_MODE) {
		// DateToUnits depends on current_date
		int units = DateToUnits(max_date+min_time,MAP_RIGHT);
		int dunits = grid.dstop - grid.dstart;
		int days = (TFoptions1::o.days-1) * dunits;
		// prevent current_date from becoming too large with respect to max_date
		if(units < days) {current_date = int(UnitsToDate(units-days,MAP_LEFT));}
	}

	DatePicker1->DateTime = current_date;
	if(DatePicker1->DroppedDown) {
		SYSTEMTIME systime; DateTimeToSystemTime(current_date,systime);
		TCommonCalendarHack * calendar = (TCommonCalendarHack *) DatePicker1;
		SendMessage(calendar->CalendarHandle,MCM_SETCURSEL,0,(LPARAM) &systime);
		InvalidateRect(calendar->CalendarHandle,NULL,FALSE);
	} else {
		// first refreshGrid, then draw drag cell
		// border (looks nicer on slow connection)
		refreshGrid();

		// update the unit settings of DragCell and
		// generate a mousemove to repaint DragCell
		if(RowGrid->DragState != Rowgrid::dsNone &&
			(RowGrid->DragState == Rowgrid::dsInsert ||
			RowGrid->CellState != Rowgrid::csCenter)) {
			int cell_start = RowGrid->DragCell.Start;
			int cell_units = RowGrid->DragCell.Units;
			int units = DateToUnits(previous_new+min_time,MAP_RIGHT);
			if(RowGrid->DragState == Rowgrid::dsInsert) {cell_start+=units;}
			else if(RowGrid->CellState == Rowgrid::csLeft) {cell_units+=units;}
			else {cell_start+=units; cell_units-=units;}
			if(cell_units >= 0) {
				// note that RowGrid does not check if the new position overlaps
				// with another cell that may have been inserted by refreshGrid
				RowCell RowCell(RowGrid->DragCell.Row,cell_start,cell_units,
					RowGrid->DragCell.Zorder,RowGrid->DragCell.Object);
				RowGrid->DragCell = RowCell;
				TPoint mouse_point = RowGrid->ScreenToClient(Mouse->CursorPos);
				int mouse_pos = (mouse_point.y << 16)|(mouse_point.x & 0xffff);
				SendMessage(RowGrid->Handle,WM_MOUSEMOVE,0,mouse_pos);
			}
		}
		if(current_date != previous_new || previous_date !=
			weekDate(previous_date,previous_date)) {
			previous_date = previous_new;
		}
		previous_new = current_date;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::DatePicker1Change(TObject *Sender) {
	if(DatePicker1->DateTime != current_date) {// mouse or escape key
		setCurrentDate(weekDate(current_date,DatePicker1->DateTime));
	}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::DatePicker1CloseUp(TObject *Sender) {
	if(current_date != previous_new) {setCurrentDate(current_date);}
	reset_focus = true; // ugly hack to reset datepicker focus
	PostMessage(DatePicker1->Handle,WM_KEYDOWN,VK_RIGHT,0);
	// PostMessage is necessary when the mouse was used.
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::browseSelection(browse_t browse) {
	LOGFUNC();
	if(hash->elementCount() == 0) {
		if(browse == NEXT || browse == PREV) {
			if(mode == WEEK_MODE) {
				int days = TFoptions1::o.days;
				if(days != 1) {days = days-1;}
				int dunits = grid.dstop - grid.dstart;
				if(browse == PREV) {dunits = -dunits;}
				TDateTime date(UnitsToDate(days*dunits,MAP_LEFT));
				setCurrentDate(weekDate(current_date,date));
			} else if(browse == PREV) {
				setCurrentDate(weekDate(current_date,current_date-1));
			} else {setCurrentDate(weekDate(current_date,current_date+1));}
		}
		return;
	}

	static vector<Cell> h_cells;
	// note that red cells have priority
	if(hash_sorted == false) {
		h_cells = hash->toVector();
		sort(h_cells.begin(),h_cells.end(),Cell::less);
		// note that changed (red) cells have priority
		hash_sorted = true;
	}

	cell_it cell = h_cells.begin();
	if(browse == NEXT || browse == PREV) {
		Cell key;
		key.changed = browse_changed;
		key.entry.schedule_id = grid.id;
		key.entry.schedule_name = grid.name;
		if(browse == NEXT) {
			key.entry.from = current_date+1;
			cell = lower_bound(h_cells.begin(),
				h_cells.end(),key,Cell::less);
			if(cell == h_cells.end()) {
				cell = h_cells.begin();
			}
		} else { // PREV
			key.entry.from = current_date-1;
			pair<vector<Cell>::reverse_iterator,
				vector<Cell>::reverse_iterator> pair;
			pair = equal_range(h_cells.rbegin(),
				h_cells.rend(),key,Cell::greater);
			if(pair.second != pair.first) {pair.second--;}
			if(pair.second == h_cells.rend()) {
				pair.second = h_cells.rbegin();
			} cell = (pair.second+1).base();
		}
	} else if(browse == LAST && cell != h_cells.end()) {
		cell = h_cells.end(); cell--;
	}

	if(cell != h_cells.end()) {
		browse_changed = cell->changed;
		if(cell->entry.schedule_id != grid.id) {
			unsigned int grid_nr = findSchedule(cell->entry.schedule_id);
			PROG_IF(grid_nr == schedules.size(),"Schedule not added for cell");
			grid = schedules[grid_nr]; Refresh();
		} setCurrentDate(int(cell->entry.from));
		setCountText(cell-h_cells.begin()+1);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::entriesFirst1Click(TObject *Sender) {
	browseSelection(FIRST);
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::entriesLast1Click(TObject *Sender) {
	browseSelection(LAST);
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::entriesPrev1Click(TObject *Sender) {
	browseSelection(PREV);
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::entriesNext1Click(TObject *Sender) {
	browseSelection(NEXT);
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::editCancel1Click(TObject *Sender) {
	if(modified) {abort1Click(Sender);}
	else {RowGrid->ClearDragState(); refreshGrid();}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::editClearPattern1Click(TObject *Sender) {
	setRowCellPattern(Sender,bsSolid);
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::editFDiagPattern1Click(TObject *Sender) {
	setRowCellPattern(Sender,bsFDiagonal);
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::editCrossPattern1Click(TObject *Sender) {
	setRowCellPattern(Sender,bsDiagCross);
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::viewRefresh1Click(TObject *Sender) {
	if(modified == false) {save1Click(Sender);} Refresh();
}
//---------------------------------------------------------------------------

// todo: with MS_SQL, use INSERT/DELETE/UPDATE trigger?
// The trigger could inform all clients to refresh data.
// http://www.thescripts.com/forum/thread81438.html
void __fastcall TFmain1::RefreshInfoTimer(TObject *Sender) {
	LOGFUNC();
	if(db == NULL || db->isConnected() == false) {return;}
	// Note that this handler is also executed when other
	// event handling code in this form opens a new window
	// (like a MessageBox). It is assumed that Active is
	// set to false just before the window opens which
	// enables us to prevent race conditions.
	if(Enabled && TFoptions1::o.auto_refresh &&
#ifdef INNODB
		// No dialogs or other forms active?
		//(the other forms may be modified)
		Active && modified == false &&
#endif
		RowGrid->DragState == Rowgrid::dsNone) {
#ifdef INNODB
		save(); refreshGrid(); // Refresh this form
#else
		if(Active) {refreshGrid();}
		else {refreshForms();} // Refresh the other forms
#endif
	}

/* disabled, single user version is free now
#ifdef ACCESS
	static unsigned timer_count = 0;
	static bool showing_message = false;
	if(showing_message == false) {
		if(++timer_count > 2 && Enabled &&
			RowGrid->DragState == Rowgrid::dsNone) {
			showing_message = true;
			if(MSGYN(AnsiString::LoadStr(
				FMAIN_CONSIDERGETLICENSE).c_str()) == ID_YES) {
				ShellExecute(NULL, "open","http://www.postware.nl/"
					"cgi-bin/GETlicense",NULL, NULL, SW_SHOWNORMAL);
			}
			timer_count = 0;
			showing_message = false;
		}
	}
#endif
*/
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::KeepAliveTimer(TObject *Sender) {
	LOGFUNC();
	if(db == NULL || db->isConnected() == false) {return;}
#ifndef ACCESS 	// keep the connection alive
	db_query.execSQL("select * from schedules where SCHEDULE_ID=NULL");
	PROG_IF(db_query.next(),"Invalid schedule record detected");
	// Would db_query.execSQL("select version()") be possible?
#endif
	// Using SQLTables() within a transaction which is rolled back due to a
	// deadlock may result in message: "New request is not allowed to start
	// because it should come with valid transaction descriptor". This also
	// seems to depend on the timer interval. It is likely that this is a
	// MS_SQL server (pooling?) bug so we use a non locking select instead.
	// This should also work for INNODB because of multiversioning (no lock)
	// The error: HY000 connection is busy with results for another command
	// was seen. Is this because of multithreading (the timer is a thread)?
	// http://forums.microsoft.com/MSDN/ShowPost.aspx?PostID=3183868&SiteID=1
	// Thread Safety: MARS operations are not thread-safe. This problem seems
	// gone now. The fix was to cleanup queries immediately after all rows
	// have been fetched.
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::home1Click(TObject *Sender) {
	id_t default_id = IntToStr(TFoptions1::o.default_id);
	bool refresh = false;
	if(grid.id != default_id) {
		unsigned int grid_nr = findSchedule(default_id);
		if(grid_nr != schedules.size()) {
			grid = schedules[grid_nr];
			refresh = true;
		}
	}
	if(mode != TFoptions1::o.week_mode) {
		if(mode == DAY_MODE) {weekView1Click(Sender);}
		else if(mode == WEEK_MODE) {dayView1Click(Sender);}
	} else if(refresh) {Refresh();}
	TDateTime today = Date(); today = weekDate(today,today);
	if(today != current_date) {setCurrentDate(today);}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::FormKeyPress(TObject *Sender, char &Key) {
	// VK_BACK is in keypress instead of keydown to prevent a sound.
	if(Key == VK_BACK) {setCurrentDate(previous_date); Key = '\0';}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::FormKeyDown(TObject *Sender, WORD &Key,
	TShiftState Shift) {
	if(Enabled == false) {Key = '\0';}
	if(DatePicker1->DroppedDown || reset_focus) {
		reset_focus = false;
	} else if(Key == VK_NEXT || Key == VK_PRIOR) {
		if(RowGrid->VerticalRows) {
			if(mode == WEEK_MODE) {
				int days = TFoptions1::o.days;
				if(days != 1) {days = days-1;}
				int dunits = grid.dstop - grid.dstart;
				if(Key == VK_PRIOR) {dunits = -dunits;}
				TDateTime date(UnitsToDate(days*dunits,MAP_LEFT));
				setCurrentDate(weekDate(current_date,date));
			} else if(Key == VK_PRIOR) {
				setCurrentDate(weekDate(current_date,current_date-1));
			} else {setCurrentDate(weekDate(current_date,current_date+1));}
		} else {
			int rows = TFoptions1::o.rows;
			if(rows != 1) {rows = rows-1;}
			int shift = RowGrid->RowHeight + RowGrid->RowLineWidth;
			if(Key == VK_PRIOR) {shift = -shift;}
			ScrollBox->VertScrollBar->Position =
				ScrollBox->VertScrollBar->Position +
				rows*shift;
		}
		Key = '\0'; // DateTimePicker should not respond
	} else if(Key == VK_LEFT || Key == VK_RIGHT ||
		Key == VK_UP || Key == VK_DOWN) {
		int days = DAYS_IN_WEEK; // not TFoptions1::days
		if(Key == VK_LEFT || Key == VK_RIGHT) {days = 1;}
		if(Key == VK_LEFT || Key == VK_DOWN) {days = -days;}
		setCurrentDate(weekDate(current_date,current_date+days));
		Key = '\0'; // DateTimePicker should not respond
	} else if(Key == VK_HOME) {home1Click(Sender); Key = '\0';}
	else if(Key == VK_RETURN) {Key = VK_F4;}
	else if(Key == VK_F1) {index1Click(Sender); Key = '\0';}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::FormMouseWheelDown(TObject *Sender,
	TShiftState Shift, TPoint &MousePos, bool &Handled) {
	if(Enabled) {Word Key = VK_NEXT; KeyDown(Key,TShiftState());}	
	Handled = true;
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::FormMouseWheelUp(TObject *Sender,
	TShiftState Shift, TPoint &MousePos, bool &Handled) {
	if(Enabled) {Word Key = VK_PRIOR; KeyDown(Key,TShiftState());}
	Handled = true;
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::EmplRowsMouseWheelDown(TObject *Sender,
	  TShiftState Shift, TPoint &MousePos, bool &Handled) {
	FormMouseWheelDown(Sender,Shift,MousePos,Handled);
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::EmplRowsMouseWheelUp(TObject *Sender,
	  TShiftState Shift, TPoint &MousePos, bool &Handled) {
	FormMouseWheelUp(Sender,Shift,MousePos,Handled);
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::FormResize(TObject *Sender) {
	LOGFUNC();
	// Open a secondary modal form and minimize the program.
	// Then use windows task manager, and 'switch to' the
	// program: We only receive a resize message so we need
	// to restore 'manually'.
#if __BORLANDC__ <= 0x582
	if(mainstate != WindowState && WindowState == wsMinimized) {
#else
	if(mainstate != WindowState && WindowState != wsMinimized) {
#endif
		tagMSG msg = {Handle,WM_SYSCOMMAND,SC_RESTORE,0,0,0};
		bool Handled = false;
		onMessage(msg,Handled);
	}
	mainstate = WindowState;
	setSize();
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::onMessage(tagMSG & Msg, bool & Handled) {
	WPARAM operation = Msg.wParam & 0xfff0;
	if(Msg.message == WM_SYSCOMMAND && (operation == SC_MINIMIZE ||
		operation == SC_RESTORE || operation == SC_MAXIMIZE)) {
		LOGFUNC(); // only log minimize/restore/maximize msgs
		static list<HWND> windows;
		if(operation == SC_MINIMIZE) {
			if(IsIconic(Handle) == FALSE) {
				Fpending1->hint_window->ReleaseHandle();
				// other active modal forms remain visible
				HWND topwindow = Screen->ActiveForm->Handle;
				while(topwindow != Handle) {
					if(IsWindowVisible(topwindow)) {
						if(Exceptions::debug_mode) {
							char text[50] = {0};
							GetWindowText(topwindow,text,sizeof(text));
							Exceptions::writeLog(String("Hide window: ")+text);
						}
						windows.push_back(topwindow);
						ShowWindow(topwindow,SW_HIDE);
					}
					topwindow = GetWindow(topwindow,GW_OWNER);
				}
#ifdef INNODB // WINE <-> INNODB
				topwindow = NULL;
				if(IsWindowVisible(Fpending1->Handle)) {
					topwindow = Fpending1->Handle;
				} else if(IsWindowVisible(Fcreated1->Handle)) {
					topwindow = Fcreated1->Handle;
				} else if(IsWindowVisible(Fentries1->Handle)) {
					topwindow = Fentries1->Handle;
				} else if(IsWindowVisible(Fschedule1->Handle)) {
					topwindow = Fschedule1->Handle;
				} else if(IsWindowVisible(Fprint1->Handle)) {
					topwindow = Fprint1->Handle;
				} else if(IsWindowVisible(Fmail1->Handle)) {
					topwindow = Fmail1->Handle;
				}
				if(topwindow) {
					windows.push_back(topwindow);
					ShowWindow(topwindow,SW_HIDE);
				}
#endif
				ShowWindow(Handle,SW_MINIMIZE);
			}
			Handled = true;
		} else { // restore/maximize
			bool is_iconic = IsIconic(Handle);
			if(is_iconic) {ShowWindow(Handle,SW_RESTORE);}
			while(windows.size()) {
				if(Exceptions::debug_mode) {
					char text[50] = {0};
					GetWindowText(windows.back(),text,sizeof(text));
					Exceptions::writeLog(String("Show window: ")+text);
				}
				ShowWindow(windows.back(),SW_SHOW);
				windows.pop_back();
			}
			Handled = is_iconic && operation == SC_RESTORE;
			// continue execution when maximizing
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::onRestore(TObject * Sender) {
	throw ProgException("Could not intercept 'Restore' (SendMessage call?)");
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::onMinimize(TObject * Sender) {
	throw ProgException("Could not intercept 'Minimize' (SendMessage call?)");
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::onException(TObject *Sender, Exception * e) {
	Exceptions::handleException(Sender,e);
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::Refresh(void) {
	LOGFUNC();
	schedules.clear();
	while(view_menu->Count > VIEWMENUCOUNT) {
		TMenuItem * sch_item = view_menu->Items[view_menu->Count-1];
		view_menu->Delete(view_menu->Count-1); delete sch_item;
	}
	addSchedule(scheds,UNDEFINED);
	unsigned int grid_nr;
	if(grid.id != UNDEFINED) {
		grid_nr = findSchedule(grid.id);
		if(grid_nr == schedules.size()) {
			addSchedule(scheds,grid.id);
		}
	}
	vector<Schedule>::const_iterator it = schedules.begin();
	for(;it != schedules.end();it++) {addMenuItem(*it);}

	grid_nr = findSchedule(grid.id);
	if(grid_nr == schedules.size()) {
		if(grid_nr == 0) {
			grid.id = UNDEFINED; grid.name = UNDEFINED;
			grid.wstart = 0; grid.wstop = DAYS_IN_WEEK;
			grid.dstart = 8; grid.dstop = 18;
			grid.tunits = 24;
			grid.hidden = true;
			grid.created_by = UNDEFINED;
			schedules.push_back(grid);
			addMenuItem(grid);
		} else {grid_nr = 0;}
	}
	grid = schedules[grid_nr];
	view_menu->Items[grid_nr+VIEWMENUCOUNT]->Checked = true;
	Caption = "CapPlan v4.86 - ["+db->getUsername()+"] "+
		AnsiString::LoadStr(FARCHIVE_SCHEDULE)+": " + grid.name;
	EntryText->Caption = "";

	if(FormState.Contains(fsShowing) ||
		grid.id != TFoptions1::opened_id) {
		if(Screen->ActiveForm == Foptions1 &&
			Foptions1->ModalResult == mrNone) {
			// this can happen if a schedule is deleted by someone else
			MSGO(AnsiString::LoadStr(FMAIN_CANNOTUPDATEOPTIONS).c_str());
			Foptions1->Close(); // does not close immediately
		}
		TFoptions1::openConfig(grid.id); // calls closeConfig()
		if(FormState.Contains(fsShowing) || grid.id != UNDEFINED) {
			if(FormState.Contains(fsShowing)) {
				viewCancelled1->Checked = false;
				viewWeek1->Checked = TFoptions1::o.week_mode;
				mode = viewWeek1->Checked ? WEEK_MODE : DAY_MODE;
			}
			viewVertical1->Checked = TFoptions1::o.vertical_rows;
			RowGrid->VerticalRows = viewVertical1->Checked;
			if(RowGrid->VerticalRows) {setVertical();}
			else {setHorizontal();}
		}
	}

	TFont * font = RowGrid->Canvas->Font;
	font->Charset = TFoptions1::o.charset;
	font->Name = TFoptions1::o.name;
	font->Size = TFoptions1::o.size;
	font->Color = TColor(TFoptions1::o.color);
	font->Pitch = (TFontPitch) TFoptions1::o.pitch;
	font->Style = TFoptions1::o.style;
	EmplRows->Canvas->Font = RowGrid->Canvas->Font;
	// EmplRows->Canvas->Font->Style = TFontStyles() << fsBold;
	EmplRows->GridLineWidth = RowGrid->RowLineWidth;
	// Refresh called by TFoptions and TFarchive

	int dunits = grid.dstop - grid.dstart;
	if(mode == DAY_MODE) {
		weekView1->Font->Style = TFontStyles();
		dayView1->Font->Style = TFontStyles() << fsUnderline;
		RowGrid->Units = dunits;
		int labelcount = dunits/TFoptions1::o.labels;
		if(dunits % TFoptions1::o.labels) {++labelcount;}
		if(RowGrid->VerticalRows) {
			TimeLabels->RowCount = labelcount;
			TimeLabels->ColCount = 1;
		} else {
			TimeLabels->ColCount = labelcount;
			TimeLabels->RowCount = 1;
		}
	} else if(mode == WEEK_MODE) {
		dayView1->Font->Style = TFontStyles();
		weekView1->Font->Style = TFontStyles() << fsUnderline;
		RowGrid->Units = TFoptions1::o.days * dunits;
		if(RowGrid->VerticalRows) {
			TimeLabels->RowCount = TFoptions1::o.days;
			TimeLabels->ColCount = 1;
		} else {
			TimeLabels->ColCount = TFoptions1::o.days;
			TimeLabels->RowCount = 1;
		}
	}

	min_date = TDateTime(MINYEAR,1,1);
	max_date = TDateTime(MAXYEAR,12,31);
	min_date = weekDate(min_date-1,min_date);
	max_date = weekDate(max_date+1,max_date);
	min_time = (double) grid.dstart/grid.tunits;
	max_time = (double) grid.dstop/grid.tunits;
	setCurrentDate(weekDate(current_date,current_date)); // uses min/max
	Frange1->DatePicker1->DateTime = int(begin_date);
	Frange1->DatePicker2->DateTime = int(end_date);
	Frange1->TimePicker1->DateTime = double(begin_date) - int(begin_date);
	Frange1->TimePicker2->DateTime = double(end_date) - int(end_date);

	if(FormState.Contains(fsShowing)) {
		// firstRun calls Refresh again. If firstRun fails, a
		// grid with grid id = UNDEFINED is still available.
		// firstRun will not try again until the next restart.
		if(grid.id == UNDEFINED) {firstRun();}
		else if(first_run == false) {
			try {checkLicense();}
			catch(Exception & e) {
				Exceptions::clearlog = true;
				Application->Terminate();
				Application->ProcessMessages();
				return;
			}
		}
	}

	codes.refresh(); // after firstRun
	Foptions1->refreshColorBoxItems();
	Farchive1->refreshPopupMenuItems();
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::setHorizontal(void) {
	LOGFUNC();
	ScrollBox->HorzScrollBar->Position = 0;
	StatusPanel->Width = ClientWidth;

	ScrollBox->Left = 8;
	ScrollBox->Width = ClientWidth - ScrollBox->Left;
	ScrollBox->Top = StatusPanel->Top + StatusPanel->Height + 22;
	ScrollBox->Height = ClientHeight - ScrollBox->Top;
	ScrollBox->VertScrollBar->Position = 0;

	EmplRows->Left = RowGrid->FrameWidth; // or something greater
	EmplRows->Top = RowGrid->FrameWidth + RowGrid->RowLineWidth;
	EmplRows->Options << goHorzLine; EmplRows->Options >> goVertLine;
	EmplRows->Width = 120; EmplRows->DefaultColWidth = EmplRows->Width;
	RowGrid->Left = EmplRows->Left + EmplRows->Width; RowGrid->Top = 0;

	int left = ScrollBox->Left + RowGrid->Left;
	TimeLabels->Left = left + RowGrid->FrameWidth - 1; // -1: stringgrid border
	TimeLabels->Height = 20; TimeLabels->DefaultRowHeight = TimeLabels->Height;
	TimeLabels->Top = ScrollBox->Top - TimeLabels->Height + 1; // separate times

	Shape1->Left = EmplRows->Left;
	Shape1->Height = RowGrid->FrameWidth;
	Shape1->Width = EmplRows->Width;

	Shape2->Width = RowGrid->FrameWidth;
	Shape2->Left = EmplRows->Left - Shape2->Width;
	Shape2->Top = EmplRows->Top - RowGrid->FrameWidth - RowGrid->RowLineWidth;

	Shape3->Left = EmplRows->Left;
	Shape3->Height = RowGrid->RowLineWidth;
	Shape3->Top = EmplRows->Top - Shape3->Height;
	Shape3->Width = EmplRows->Width;

	Shape4->Left = EmplRows->Left;
	Shape4->Height = RowGrid->FrameWidth;
	Shape4->Top = EmplRows->Top - RowGrid->RowLineWidth - Shape4->Height;
	Shape4->Width = EmplRows->Width;
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::setVertical(void) {
	LOGFUNC();
	ScrollBox->VertScrollBar->Position = 0;
	StatusPanel->Width = ClientWidth;

	ScrollBox->Left = 56;
	ScrollBox->Width = ClientWidth - 6 - ScrollBox->Left;
	ScrollBox->Top = StatusPanel->Top + StatusPanel->Height + 2;
	ScrollBox->Height = ClientHeight - ScrollBox->Top;
	ScrollBox->HorzScrollBar->Position = 0;

	EmplRows->Top = RowGrid->FrameWidth; // or something greater
	EmplRows->Left = RowGrid->FrameWidth + RowGrid->RowLineWidth;
	EmplRows->Options >> goHorzLine; EmplRows->Options << goVertLine;
	EmplRows->Height = 25; EmplRows->DefaultRowHeight = EmplRows->Height;
	RowGrid->Top = EmplRows->Top + EmplRows->Height; RowGrid->Left = 0;

	int top = ScrollBox->Top + RowGrid->Top;
	TimeLabels->Top = top + RowGrid->FrameWidth - 1; // -1: stringgrid border
	TimeLabels->Width = 50; TimeLabels->DefaultColWidth = TimeLabels->Width;
	TimeLabels->Left = ScrollBox->Left - TimeLabels->Width + 1; // merge border

	Shape1->Top = EmplRows->Top;
	Shape1->Width = RowGrid->FrameWidth;
	Shape1->Height = EmplRows->Height;

	Shape2->Height = RowGrid->FrameWidth;
	Shape2->Top = EmplRows->Top - Shape2->Height;
	Shape2->Left = EmplRows->Left - RowGrid->FrameWidth - RowGrid->RowLineWidth;

	Shape3->Top = EmplRows->Top;
	Shape3->Width = RowGrid->RowLineWidth;
	Shape3->Left = EmplRows->Left - Shape3->Width;
	Shape3->Height = EmplRows->Height;

	Shape4->Top = EmplRows->Top;
	Shape4->Width = RowGrid->FrameWidth;
	Shape4->Left = EmplRows->Left - RowGrid->RowLineWidth - Shape4->Width;
	Shape4->Height = EmplRows->Height;
}
//---------------------------------------------------------------------------

// todo: option to ommit description or to display description only
AnsiString __fastcall TFmain1::CellToInfo(Cell & cell) {
	AnsiString text;
	if(cell.planned && cell.entry.app_id != UNDEFINED) {
		AnsiString lastname = cell.data.person.lastname;
		AnsiString address = cell.data.person.address;
		AnsiString city = cell.data.person.city;
		AnsiString info = cell.data.info;
		if(TFoptions1::o.entry_info) {
			text = city;
			if(city.Length()) {text += " / ";} text += lastname;
			if(lastname.Length()) {text += " / ";} text += address;
			if(address.Length()) {text += " / ";} text += info;
		} else {
			text = lastname;
			if(lastname.Length()) {text += " / ";} text += address;
			if(address.Length()) {text += " / ";} text += city;
			if(city.Length()) {text += " / ";} text += info;
		}
	}
	return text;
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::EmplRowsDrawCell(TObject *Sender, int ACol,
	int ARow, TRect &Rect, TGridDrawState State) {
	// perform clipping
	if(RowGrid->VerticalRows) {
		if((RowGrid->Left + Rect.right <= 0) ||
			(RowGrid->Left + Rect.left >=
			ScrollBox->ClientWidth)) {return;}
	} else if((RowGrid->Top + Rect.bottom <= 0) ||
		(RowGrid->Top + Rect.top >=
		ScrollBox->ClientHeight)) {return;}

	unsigned rc = EmplRows->RowCount;
	if(RowGrid->VerticalRows) {rc = EmplRows->ColCount;}
	// Do not access employees if the connection failed
	// within getgrid after employees.clear was called.
	if(employees.size() != rc) {return;}

	TCanvas * canvas = EmplRows->Canvas;
	TColor color = canvas->Font->Color; Person person;
	if(RowGrid->VerticalRows) {person = employees[ACol];}
	else {person = employees[ARow];}
	if(person.employee) {
		canvas->Brush->Color = clWhite;
		if(employees_locked.find(person.id) != employees_locked.end()) {
			canvas->Font->Color = TColor(CONFLICTCOLOR);
		}
	} else {canvas->Brush->Color = TColor(CONFLICTCOLOR);}

	int width = 5; // Rect.Width() - canvas->TextWidth(text);
	int height = Rect.Height() - canvas->TextHeight(person.lastname);
	canvas->TextRect(Rect,Rect.left+width/2,Rect.top+height/2,person.lastname);
	canvas->Font->Color = color;

	if(dragging) {
		if(RowGrid->VerticalRows) {swap(ACol,ARow);}
		if(ARow == dest_row) {
			RECT rect = {Rect.left,Rect.top,Rect.right,Rect.bottom};
			if(RowGrid->VerticalRows) {
				if(dest_row < drag_row) {rect.right = rect.left + 2;}
				else {rect.left = rect.right - 2;}
			} else {
				if(dest_row < drag_row) {rect.bottom = rect.top + 2;}
				else {rect.top = rect.bottom - 2;}
			} DrawFocusRect(EmplRows->Canvas->Handle,&rect);
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::RowGridPaint(TObject *Sender) {/*LOGFUNC();*/}
//---------------------------------------------------------------------------

void __fastcall TFmain1::RowGridDrawRowLine(TObject *Sender, int Row,
	TRect Rect) {
	TCanvas * canvas = RowGrid->Canvas;
	canvas->Brush->Color = RowGrid->RowLineColor;
	canvas->FillRect(Rect);
	canvas->Pen->Color = RowGrid->TickColor;
	int middle4 = RowGrid->RowLineWidth/4;
	int dunits = grid.dstop - grid.dstart;
	for(int u=1;u != RowGrid->Units;u++) {
		int x = RowGrid->FrameWidth + u*RowGrid->UnitWidth;
		if(mode == WEEK_MODE && (u % dunits) == 0) {
			canvas->Pen->Color = clRed;
			canvas->Pen->Mode = pmMask;
			canvas->Pen->Style = psDot;
			if(RowGrid->VerticalRows) {
				canvas->MoveTo(Rect.left,x);
				canvas->LineTo(Rect.right+RowGrid->RowHeight,x);
			} else {
				canvas->MoveTo(x,Rect.top);
				canvas->LineTo(x,Rect.bottom+RowGrid->RowHeight);
			}
			canvas->Pen->Mode = pmCopy;
			canvas->Pen->Style = psSolid;
		} else {
			int labels = TFoptions1::o.labels;
			if(labels != 1 && (u % labels) == 0) {
				if(mode == WEEK_MODE) {
					canvas->Pen->Color = clBlack;
					if(RowGrid->VerticalRows) {
						canvas->MoveTo(Rect.left,x);
						canvas->LineTo(Rect.right,x);
					} else {
						canvas->MoveTo(x,Rect.top);
						canvas->LineTo(x,Rect.bottom);
					}
				} else if(mode == DAY_MODE) {
					canvas->Pen->Color = clBlack;
					canvas->Pen->Mode = pmMask;
					canvas->Pen->Style = psDot;
					if(RowGrid->VerticalRows) {
						canvas->MoveTo(Rect.left,x);
						canvas->LineTo(Rect.right+RowGrid->RowHeight,x);
					} else {
						canvas->MoveTo(x,Rect.top);
						canvas->LineTo(x,Rect.bottom+RowGrid->RowHeight);
					}
					canvas->Pen->Mode = pmCopy;
					canvas->Pen->Style = psSolid;
				}
			} else {
				canvas->Pen->Color = clBlack;
				if(RowGrid->VerticalRows) {
					canvas->MoveTo(Rect.left+middle4,x);
					canvas->LineTo(Rect.right-middle4,x);
				} else {
					canvas->MoveTo(x,Rect.top+middle4);
					canvas->LineTo(x,Rect.bottom-middle4);
				}
			}
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::RowGridDrawRowCell(TObject *Sender,RowCell RowCell,
	TRect Rect) {
	// perform clipping
	if(RowGrid->VerticalRows) {
		if((RowGrid->Left + Rect.right <= 0) ||
			(RowGrid->Left + Rect.left >=
			ScrollBox->ClientWidth)) {return;}
	} else if((RowGrid->Top + Rect.bottom <= 0) ||
		(RowGrid->Top + Rect.top >=
		ScrollBox->ClientHeight)) {return;}

	TCanvas * canvas = RowGrid->Canvas;
	Cell * cell = (Cell *) RowCell.Object;
	if(cell->selected) {
		if(cell->changed) {canvas->Brush->Color = TColor(CONFLICTCOLOR);}
		else if(cell->planned && cell->entry.app_id == UNDEFINED) {
			canvas->Brush->Color = TColor(SELECTBLOCKEDCOLOR);
		} else {canvas->Brush->Color = TColor(SELECTCOLOR);}
	} else if(cell->planned && cell->entry.app_id == UNDEFINED) {
		canvas->Brush->Color = TColor(BLOCKEDCOLOR);
	} else if(cell->planned && cell->data.app_color) {
		canvas->Brush->Color = TColor(cell->data.app_color);
	} else {
		if(TFoptions1::o.week_colors && WeekOf(cell->entry.from) % 2) {
			canvas->Brush->Color = TColor(WEEK1COLOR);
		} else {canvas->Brush->Color = TColor(WEEK2COLOR);}
	}

	// When a Rect overlaps with any of the rowgrid borders,
	// its coordinates are aligned with the overlapping border.
	int lim1 = RowGrid->FrameWidth - RowGrid->CellFrameWidth;
	int lim2 = RowGrid->FrameWidth + RowGrid->CellFrameWidth;
	if(RowGrid->VerticalRows) {
		lim2 += RowGrid->ClientHeight;
		if(Rect.top < lim1) {Rect.top = lim1;}
		if(Rect.bottom > lim2) {Rect.bottom = lim2;}
	} else {
		lim2 += RowGrid->ClientWidth;
		if(Rect.left < lim1) {Rect.left = lim1;}
		if(Rect.right > lim2) {Rect.right = lim2;}
	}

	canvas->Font->Style = TFontStyles();
	if(cell->planned) {
		if(cell->entry.cancelled) {
			canvas->Font->Style = TFontStyles() << fsStrikeOut;
		}
		if(TFoptions1::o.hide_patterns == false && cell->entry.pattern) {
			canvas->Brush->Style = TBrushStyle(cell->entry.pattern);
			TColor color = canvas->Brush->Color;
			canvas->Brush->Color = clBlack;
			SetBkColor(canvas->Handle,color);
		}
	}
	Frame3D(canvas,Rect,clBlack,clBlack,RowGrid->CellFrameWidth);

//	disabled: gdi+ > windows 98
//	BeginPath(canvas->Handle);
	canvas->FillRect(Rect);
	canvas->Brush->Style = bsSolid;
/*
	EndPath(canvas->Handle);
	SelectClipPath(canvas->Handle,RGN_COPY);
	TRIVERTEX vertices[3];

	unsigned char red = canvas->Brush->Color & 0x000000ff;
	unsigned char green = (canvas->Brush->Color & 0x0000ff00) >> 8;
	unsigned char blue = (canvas->Brush->Color & 0x00ff0000) >> 16;

	vertices[0].x = Rect.left;
	vertices[0].y = Rect.top;
	vertices[0].Red = (0x100*(red+1))-1;
	vertices[0].Green = (0x100*(green+1))-1;
	vertices[0].Blue = (0x100*(blue+1))-1;
	vertices[0].Alpha = 0xffff;

	vertices[1].x = Rect.right;
	vertices[1].y = Rect.bottom;
	vertices[1].Red = vertices[0].Red * 0.9;
	vertices[1].Green = vertices[0].Green * 0.9;
	vertices[1].Blue = vertices[0].Blue * 0.9;
	vertices[1].Alpha = 0xffff;

	GRADIENT_RECT r;
	r.UpperLeft = 0;
	r.LowerRight = 1;

	if(RowGrid->VerticalRows) {
		GradientFill(canvas->Handle,vertices, 2, &r, 1, GRADIENT_FILL_RECT_H);
	} else {
		GradientFill(canvas->Handle,vertices, 2, &r, 1, GRADIENT_FILL_RECT_V);
	}
*/
	TDataRows::wrapText(CellToInfo(*cell),canvas,Rect);
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::RowGridDrawFocusRect(TObject *Sender, TRect Rect) {
	Frame3D(RowGrid->Canvas,Rect,clBlack,clBlack,1);
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::TimeLabelsDrawCell(TObject *Sender, int ACol,
	  int ARow, TRect &Rect, TGridDrawState State) {
	TCanvas * canvas = TimeLabels->Canvas;
	canvas->Brush->Color = TColor(TIMEPANELCOLOR); // Color;
//	BeginPath(canvas->Handle);
	canvas->FillRect(Rect);
/*
	EndPath(canvas->Handle);
	SelectClipPath(canvas->Handle,RGN_COPY);
	TRIVERTEX vertices[2];

	unsigned char red = canvas->Brush->Color & 0x000000ff;
	unsigned char green = (canvas->Brush->Color & 0x0000ff00) >> 8;
	unsigned char blue = (canvas->Brush->Color & 0x00ff0000) >> 16;

	vertices[0].x = Rect.left;
	vertices[0].y = Rect.top;
	vertices[0].Red = (0x100*(red+1))-1;
	vertices[0].Green = (0x100*(green+1))-1;
	vertices[0].Blue = (0x100*(blue+1))-1;
	vertices[0].Alpha = 0xffff;

	vertices[1].x = Rect.right;
	vertices[1].y = Rect.bottom;
	vertices[1].Red = vertices[0].Red * 0.9;
	vertices[1].Green = vertices[0].Green * 0.9;
	vertices[1].Blue = vertices[0].Blue * 0.9;
	vertices[1].Alpha = 0xffff;

	GRADIENT_RECT r;
	r.UpperLeft = 0;
	r.LowerRight = 1;
	if(RowGrid->VerticalRows) {
		GradientFill(canvas->Handle,vertices, 2, &r, 1, GRADIENT_FILL_RECT_H);
	} else {
		GradientFill(canvas->Handle,vertices, 2, &r, 1, GRADIENT_FILL_RECT_V);
	}
*/
	AnsiString text = TimeLabels->Cells[ACol][ARow];
	if(RowGrid->VerticalRows) {Rect.Top += 2; Rect.left += 1;}
	else {Rect.top += 3; Rect.Left += 2;}
	if(mode == WEEK_MODE) {
		if(RowGrid->VerticalRows) {
			int height = Rect.Height()-canvas->TextHeight(text);
			if(height >= 0) {Rect.top += height/2;}
		} else {
			int width = Rect.Width()-canvas->TextWidth(text);
			if(width >= 0) {Rect.Left += width/2;}
		}
	}
	TDataRows::wrapText(text,canvas,Rect);
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::TimeLabelsMouseDown(TObject *Sender,
	TMouseButton Button, TShiftState Shift, int X, int Y) {
	if(Button == mbLeft && Shift.Contains(ssDouble)) {
		if(mode == WEEK_MODE) {
			int col,row; TimeLabels->MouseToCell(X,Y,col,row);
			if(RowGrid->VerticalRows) {swap(col,row);}
			dayView1Click(Sender);
			int dunits = grid.dstop - grid.dstart;
			TDateTime date = UnitsToDate(col*dunits,MAP_LEFT);
			if(date != current_date) {setCurrentDate(date);}
		} else if(mode == DAY_MODE) {weekView1Click(Sender);}
	}
	ActiveControl = DatePicker1;
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::TimeLabelsMouseMove(TObject *Sender,
	TShiftState Shift, int X, int Y) {
	int col,row; TimeLabels->MouseToCell(X,Y,col,row);
	if(RowGrid->DragState == Rowgrid::dsNone && row != -1 && col != -1) {
		EntryText->Caption = TimeLabels->Cells[col][row];
	}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::toggleRowCell(RowCell & RowCell) {
	LOGFUNC();
	RowGrid->RemoveRowCell(RowCell); // resets dragging
	Cell * cell = (Cell *) RowCell.Object;
	if(cell->selected) { // cell was in hash? (deselecting?)
		removeHashCell(*cell);
		cell->changed = false;
		if(cell->planned) {
			RowCell.Zorder = PLANNED;
			if(cell->entry.cancelled) {
				RowCell.Zorder = CANCELLED;
			}
		} else {delete cell; RowCell.Object = NULL;}
	} else {insertHashCell(*cell); RowCell.Zorder = SELECTED;}
	if(RowCell.Object) { // if (still) selected: not NULL
		RowGrid->InsertRowCell(RowCell);
		RowGrid->SortRow(RowCell.Row);
	}
	// Because of SortRow, always repaint entire row
	// If RowCell reinserted, paint row will draw it.
	RowGrid->PaintCell(RowCell,true);
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::RowGridContextPopup(TObject *Sender,
	const TPoint &MousePos, bool &Handled) {
	Handled = RowGrid->DragState != Rowgrid::dsNone;
#ifdef _WIN64
	POINT m_point;
	if(GetCursorPos(&m_point) == FALSE) {return;}
	TPoint mouse_point = RowGrid->ScreenToClient((const TPoint &) m_point);
	context_x = mouse_point.x; context_y = mouse_point.y;
#else
	context_x = MousePos.x; context_y = MousePos.y;
#endif
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::popupSelectEntry1Click(TObject *Sender) {
	if(context_x >= 0 && context_y >= 0) {
		TCellState state;
		RowCell RowCell = RowGrid->MouseToCell(context_x,context_y,state);
		if(state != Rowgrid::csNone && state != Rowgrid::csEmpty) {
			if(Sender) {toggleRowCell(RowCell);}
			else {insertHashCell(*((Cell *) RowCell.Object));}
		}
	} context_x = context_y = -1;
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::popupDelete1Click(TObject *Sender) {
	bool select_entry = hash->elementCount() == 0;
	if(select_entry) {popupSelectEntry1Click(Sender);}
	editDelete1Click(Sender);
	if(select_entry) {removeSelection();}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::popupClearPattern1Click(TObject *Sender) {
	if(hash->elementCount() == 0) {popupSelectEntry1Click(NULL);}
	editClearPattern1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::popupCrossPattern1Click(TObject *Sender) {
	if(hash->elementCount() == 0) {popupSelectEntry1Click(NULL);}
	editCrossPattern1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::popupFDiagPattern1Click(TObject *Sender) {
	if(hash->elementCount() == 0) {popupSelectEntry1Click(NULL);}
	editFDiagPattern1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::RowGridMouseDown(TObject *Sender,
	TMouseButton Button, TShiftState Shift, int X, int Y) {
	dblclick = true;
	if(RowGrid->DragState == Rowgrid::dsNone &&
		Shift.Contains(ssCtrl) && Button == mbLeft) {
		TCellState state;
		RowCell RowCell = RowGrid->MouseToCell(X,Y,state);
		if(state != Rowgrid::csNone && state != Rowgrid::csEmpty) {
			dblclick = false; toggleRowCell(RowCell);
			// dblclick blocks RowGridDblClickRowCell
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::RowGridMouseMove(TObject *Sender,
	TShiftState Shift, int X, int Y) {
	if(RowGrid->DragState == Rowgrid::dsNone) {
		AnsiString hint; TCellState state;
		RowCell RowCell = RowGrid->MouseToCell(X,Y,state);
		if(state == Rowgrid::csCenter) {
			Cell * cell = (Cell *) RowCell.Object;
			EntryText->Caption = CellToInfo(*cell);
			if(cell->planned == false && cell->selected &&
				cell->changed == false) {
				hint = AnsiString::LoadStr(FMAIN_DBLCLICKTOSCHEDULE);
			}
		}
		if(RowGrid->Hint != hint) {
			RowGrid->Hint = hint;
			Application->CancelHint();
		}
	} else {Application->CancelHint();}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::RowGridDblClickRowCell(TObject *Sender,
	RowCell RowCell) {
	LOGFUNC();
	if(dblclick) {
		dblclick = false;
		Cell * cell = (Cell *) RowCell.Object;
		if(cell->planned) {
			if(cell->entry.app_id != UNDEFINED) { // not blocked
				per_id = cell->data.person.id; // per_id always defined
				app_id = cell->entry.app_id;
				// doc_id = UNDEFINED; // forget selected file
				view_archive(Farchive1->APPS);
			}
		} else if(cell->selected) {schedule1Click(Sender);}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::DatePicker1DropDown(TObject *Sender) {
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

void __fastcall TFmain1::getVersion(int & major,int & minor,int & release,
	int & build) {
	unsigned long handle;
	AnsiString filename = ExtractFileName(Application->ExeName);
	unsigned long info_size = GetFileVersionInfoSize(filename.c_str(),&handle);
	if(info_size) {
		VS_FIXEDFILEINFO * file_info;
		unsigned int length; LPTSTR info;
		if((info = (LPTSTR) malloc(info_size)) != NULL) {
			if(GetFileVersionInfo(filename.c_str(),handle,info_size,info) &&
				VerQueryValue(info,"\\",(LPVOID *) &file_info,&length)) {
				major = HIWORD(file_info->dwFileVersionMS);
				minor = LOWORD(file_info->dwFileVersionMS);
				release = HIWORD(file_info->dwFileVersionLS);
				build = LOWORD(file_info->dwFileVersionLS);
#ifdef _DEBUG
				Exceptions::debug_mode = true;
#else
				Exceptions::debug_mode = LOWORD(
					file_info->dwFileFlags) & VS_FF_DEBUG;
#endif
			}
			free(info);
		}
	}
}
//---------------------------------------------------------------------------

// todo: print the employees and the rowgrid as you see it.
void __fastcall TFmain1::editPrint1Click(TObject *Sender) {
	LOGFUNC();
	TDateTime begin = begin_date;
	TDateTime end = end_date;
	Fprint1->SelectBox->Checked = true;
	Fprint1->CheckBox->Checked = TFoptions1::o.print_times;
	Fprint1->DatePicker1->DateTime = int(begin);
	Fprint1->DatePicker2->DateTime = int(end);
	Fprint1->TimePicker1->DateTime = double(begin) - int(begin);
	Fprint1->TimePicker2->DateTime = double(end) - int(end);
	// Frange makes sure a valid range is entered (see view)
	// note select_id has no effect when Sender is defined.
	if(Fprint1->view(begin,end,1,TFoptions1::o.copies,
		select_id) != mrOk) {return;}
	printEntries(resetPrinter(), Fprint1);
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::editMail1Click(TObject *Sender) {
	LOGFUNC();
	TDateTime begin = begin_date;
	TDateTime end = end_date;
	Fmail1->SelectBox->Checked = Sender != NULL;
	if(Fmail1->CheckBox->Enabled) {
		Fmail1->CheckBox->Checked = TFoptions1::o.print_times;
	}
	Fmail1->DatePicker1->DateTime = int(begin);
	Fmail1->DatePicker2->DateTime = int(end);
	Fmail1->TimePicker1->DateTime = double(begin) - int(begin);
	Fmail1->TimePicker2->DateTime = double(end) - int(end);
	// Frange makes sure a valid range is entered (see view)
	// note select_id has no effect when Sender is defined.
	if(Fmail1->view(begin,end,1,1,select_id) != mrOk) {return;}
	if(Fmail1->AttachIcalBox->Checked) {mailEntriesAsIcal();}
	else {mailEntriesAsPng();}
}
//---------------------------------------------------------------------------

int __fastcall TFmain1::printHeader(TRect rect,TCanvas * canvas) {
	LOGFUNC();
	canvas->Font->Style = TFontStyles() << fsBold;
	AnsiString day("week "+IntToStr(WeekOf(begin_date)));
	day += FormatDateTime("',' dddd yyyy'-'mm'-'dd",begin_date);
	rect.top = TDataRows::wrapText(day,canvas,rect);
	if(mode == WEEK_MODE) {
		day = "week "+IntToStr(WeekOf(end_date));
		day += FormatDateTime("',' dddd yyyy'-'mm'-'dd",end_date);
		rect.top = TDataRows::wrapText(day,canvas,rect);
	}
	day = "print id: "+FormatDateTime("yyyymmddhhmmss",print_date);
	if(Exceptions::debug_mode) {Exceptions::writeLog(day);}
	rect.top = TDataRows::wrapText(day,canvas,rect);
	canvas->Font->Style = TFontStyles();
	return rect.top;
}
//---------------------------------------------------------------------------

int __fastcall TFmain1::printEmployee(Person & person,
	cells_t::iterator row,TRect rect,TCanvas * canvas,
	int space,TFprint1 * form) {
	int left = rect.left; // inclusive
	int right = rect.right-1; // exclusive

	// horizontal line before printing employee
	canvas->MoveTo(left,rect.top);
	canvas->LineTo(right,rect.top);
	// rect.top += canvas->Pen->Width;
	int linetop = rect.top;

	// print employee itself
	AnsiString text = person.lastname;
	rect.top += space/5; rect.left += space/4;
	rect.top = TDataRows::wrapText(text,canvas,rect);
	rect.top += space/5; rect.left += space/2;
	rect.right -= space;

	for(row_t::iterator rowcell = row->begin();
		rowcell != row->end();rowcell++) {
		if(rect.top > rect.bottom) {break;}
		Cell * cell = (Cell *) rowcell->Object;
		if(cell->planned == false || cell->entry.cancelled ||
			cell->entry.app_id == UNDEFINED) {continue;}

		AnsiString text;
		TDateTime from = cell->entry.from;
		TDateTime until = cell->entry.until;
		if(Entry::less_than(from,begin_date)) {from = begin_date;}
		if(Entry::less_than(end_date,until)) {until = end_date;}
		if(form->CheckBox->Checked) {
			text = FormatDateTime("hh'.'mm'",from);
			text += FormatDateTime(" (ddd) '-' ",from);
			text += FormatDateTime("hh'.'mm'",until);
			text += FormatDateTime(" (ddd)",until);
		} else if(mode == WEEK_MODE) {
			text = FormatDateTime("yyyy'-'mm'-'dd (ddd) '-' ",from);
			text += FormatDateTime("yyyy'-'mm'-'dd (ddd)",until);
		} else {text = FormatDateTime("yyyy'-'mm'-'dd (ddd)",from);}

		text += ": ";
		AnsiString lastname = cell->data.person.lastname;
		AnsiString address = cell->data.person.address;
		AnsiString postcode = cell->data.person.postcode;
		AnsiString city = cell->data.person.city;
		AnsiString telephone = cell->data.person.telephone;
		AnsiString telephone2 = cell->data.person.telephone2;

		text += lastname;
		if(lastname.Length()) {text += " / ";} text += address;
		if(address.Length()) {text += " / ";} text += postcode;
		if(postcode.Length()) {text += " / ";} text += city;
		if(city.Length()) {text += " / ";} text += telephone;
		if(telephone.Length()) {text += " / ";} text += telephone2;
		canvas->Font->Style = TFontStyles() << fsBold;
		rect.top = TDataRows::wrapText(text,canvas,rect)+space/5;
		canvas->Font->Style = TFontStyles() << fsItalic; rect.left += space/4;
		rect.top = TDataRows::wrapText(cell->data.info,canvas,rect)+space/5;
		canvas->Font->Style = TFontStyles(); rect.left -= space/4;
	}

	// left vertical line going up from lines.Top to linetop
	canvas->MoveTo(left,rect.top);
	canvas->LineTo(left,linetop);

	// right vertical line going up from lines.Top to linetop
	canvas->MoveTo(right,rect.top);
	canvas->LineTo(right,linetop);

	// horizontal line after last printed employee entry
	canvas->MoveTo(left,rect.top);
	canvas->LineTo(right,rect.top);
	// rect.top += canvas->Pen->Width;
	return rect.top;
}
//---------------------------------------------------------------------------

int __fastcall TFmain1::printEntries(TPrinter * printer,TFprint1 * form) {
	LOGFUNC();
	TCanvas * canvas;
	int PageHeight,PageWidth;
	int top,left,space;
	if(printer) {
		canvas = printer->Canvas;
		PageHeight = printer->PageHeight;
		PageWidth = printer->PageWidth;
		top = PageHeight/20;
		left = PageWidth/20;
		space = 100;
	} else {
		Femployee1->Image->Height = 8000;
		canvas = Femployee1->Image->Canvas;
		canvas->FillRect(canvas->ClipRect);
		PageHeight = Femployee1->Image->Height;
		PageWidth = Femployee1->Image->Width;
		top = left = space = 20;
	}

	TRect lines(left,top,PageWidth-left,PageHeight-top);
	// Input events need to be ignored because TPrinter processes events
	// while printing. The Timer and FormEvents are not ignored however.
	TForm * active_form = Screen->ActiveForm; // see GetActiveHandle()
	active_form->Enabled = false;
	try {
		print_date = Now();
		if(printer) {printer->BeginDoc();}
		// setting printer->Copies does not seem to work
		int copies = StrToInt(form->TotalEdit->Text);
		while(copies-- > 0) {
			lines.Top = top; lines.Bottom = PageHeight-top;
			lines.Top = printHeader(lines,canvas) + space;
			int headertop = lines.Top; unsigned rc = 0;
			for(cells_t::iterator row = RowGrid->Cells.begin();
				row != RowGrid->Cells.end();row++,rc++) {
				Person person = employees[rc];
				if(person.entry_count == 0) {continue;}
				if(find(form->selected.begin(),form->selected.end(),
					person.id) == form->selected.end()) {continue;}
				int newtop = printEmployee(person,row,lines,canvas,space,form);
				if(newtop > lines.Bottom) {
					TRect clear_rect(0,
						lines.top-space+1,
						PageWidth,PageHeight);
					canvas->FillRect(clear_rect);
					if(lines.Top != headertop) {
						if(printer) {printer->NewPage();}
						lines.Top = top; lines.Bottom = PageHeight-top;
						lines.Top = printHeader(lines,canvas) + space;
						headertop = lines.Top;
					}
					newtop = printEmployee(person,row,lines,canvas,space,form);
					if(newtop > lines.Bottom) {
						AnsiString msg = AnsiString::LoadStr(
							FMAIN_INCOMPLETEPRINTOUT)+person.lastname;
						if(copies || form->selected.size() != 1) {
							msg += "\n"+AnsiString::LoadStr(CONTINUEQUESTION);
							if(ATTYN(msg.c_str()) == ID_NO) {copies = 0; break;}
						} else {ATTO(msg.c_str());}
					}
				} lines.Top = newtop + space;
			}
			if(printer) {
				if(copies) {printer->NewPage();}
			} else if(lines.Top < Femployee1->ScrollBox->ClientHeight) {
				Femployee1->Image->Height =
					Femployee1->ScrollBox->ClientHeight+1;
			} else if(lines.Top < Femployee1->Image->Height) {
				Femployee1->Image->Height = lines.Top;
			}
		}
		if(printer) {printer->EndDoc();}
	} catch(Exception & e) {
		if(printer) {printer->Abort();}
		Application->ShowException(&e);
	}
	active_form->Enabled = true;
	SetForegroundWindow(active_form->Handle);
	return lines.top;
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::editExport1Click(TObject *Sender) {
	LOGFUNC();
	DataRows->RowCount = 3;
	DataRows->setData(grid.id,0,1);
	DataRows->setData(YMDHMS(begin_date),2,1);
	DataRows->setData(YMDHMS(end_date),3,1);
	unsigned rc = 0;
	for(cells_t::iterator row = RowGrid->Cells.begin();
		row != RowGrid->Cells.end();row++,rc++) {
		Person person = employees[rc];
		if(person.employee == false) {continue;}
		for(row_t::iterator rowcell = row->begin();
			rowcell != row->end();rowcell++) {
			Cell * cell = (Cell *) rowcell->Object;
			if(cell->planned == false) {continue;}
			int index = DataRows->RowCount;
			DataRows->RowCount = index+1;
			DataRows->setData(person.id,0,index);
			DataRows->setData(person.lastname,1,index);
			DataRows->setData(YMDHMS(cell->entry.from),2,index);
			DataRows->setData(YMDHMS(cell->entry.until),3,index);
			DataRows->setData(IntToStr((int) RoundTo(double(
				cell->entry.date_diff)*24*60,1)),4,index);
			DataRows->setData(cell->entry.pattern,5,index);
			DataRows->setData(cell->entry.uuid,6,index);
			DataRows->setData(cell->data.app_charge,7,index);
			DataRows->setData(cell->data.app_color,8,index);
			DataRows->setData(cell->entry.app_id,9,index);
			DataRows->setData(IntToStr((int) cell->entry.cancelled),10,index);
			DataRows->setData(cell->data.person.lastname,11,index);
			DataRows->setData(cell->data.person.address,12,index);
			DataRows->setData(cell->data.person.postcode,13,index);
			DataRows->setData(cell->data.person.city,14,index);
			DataRows->setData(cell->data.info,15,index);
		}
	}
	DataRows->selectAll();
	exportCsvFile(DataRows);
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::editImport1Click(TObject *Sender) {
	LOGFUNC();
	OpenDialog->InitialDir = "";
	OpenDialog->FileName = "*.ics";
	bool result;
#if __BORLANDC__ >= 0x582
	Application->ModalPopupMode = pmExplicit;
	try {result = OpenDialog->Execute(Handle);}
	__finally {Application->ModalPopupMode = pmNone;}
#else
	result = OpenDialog->Execute();
#endif
	if(result == false) {return;} flushPaint();

	TDateTime end = begin_date;
	AnsiString calendar = ChangeFileExt(
		ExtractFileName(OpenDialog->FileName),"");
	descr2cells_t descr2cells = map<AnsiString,vector<Cell> >();
	VObject * vObject = Parse_MIME_FromFileName(
		AnsiString(OpenDialog->FileName).c_str());
	while(vObject) {
		const char * vName = vObjectName(vObject);
		if(strcmp(vName,VCCalProp) == 0) {
			VObjectIterator i;
			initPropIterator(&i,vObject);
			while(moreIteration(&i)) {
				VObject * v = nextVObject(&i);
				vName = vObjectName(v);
				if(strcmp(vName,"X-WR-CALNAME") == 0) {
					calendar = Utf8ToAnsi(vObjectUStringZValue(v));
					continue;
				}
				if(strcmp(vName,VCEventProp) != 0) {continue;}

				Cell cell;
				bool do_continue = false;
				try {cell = vEventToCell(v);}
				catch(EConvertError & e) {do_continue = true;}
				catch(UserException & e) {do_continue = true;}
				if(do_continue) {continue;}
				if(cell.entry.until > end) {
					end = cell.entry.until;
				}

				vector<Cell> cells;
				cells.push_back(cell);
				pair<AnsiString,vector<Cell> > descr2cell_pair;
				descr2cell_pair.first = cell.data.info;
				descr2cell_pair.second = cells;
				pair<descr2cells_t::iterator, bool> inserted =
					descr2cells.insert(descr2cell_pair);
				if(!inserted.second) {
					inserted.first->second.push_back(cell);
				}
			}
		}
		VObject * tmp = vObject;
		vObject = nextVObjectInList(vObject);
		cleanVObject(tmp);
	}

	id_t person_id = UNDEFINED;
	persons.execSQL("select personal.PERSONAL_ID from "
		"personal where LASTNAME="+QuotedStr(calendar));
	while(persons.next()) {
		if(person_id == UNDEFINED) {
			person_id = persons.fieldByName("PERSONAL_ID").toString();
		} else {
			person_id = UNDEFINED;
			break;
		}
	}

	int start,until;
	TDateTime begin = begin_date;
	Fimport1->DatePicker1->DateTime = int(begin);
	Fimport1->DatePicker2->DateTime = int(end);
	Fimport1->TimePicker1->DateTime = double(begin) - int(begin);
	Fimport1->TimePicker2->DateTime = double(end) - int(end);
	Fimport1->SelectBox->Checked = person_id == UNDEFINED;
	list<id_t> employee_ids;
	if(Fimport1->view(begin,end,
		StrToInt(Fimport1->CountEdit->Text),
		StrToInt(Fimport1->TotalEdit->Text),
		person_id) != mrOk) {return;}
	start = DateToUnits(Fimport1->begin,MAP_RIGHT);
	until = DateToUnits(Fimport1->end,MAP_LEFT);
	begin = UnitsToDate(start,MAP_LEFT);
	end = UnitsToDate(until,MAP_RIGHT);

	insertImports(Sender,begin,end,descr2cells,person_id,calendar);
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::insertImports(TObject * Sender,
	TDateTime begin,TDateTime end,descr2cells_t & descr2cells,
	id_t person_id,AnsiString calendar) {
	LOGFUNC();
	removeSelection();
	descr2cells_t::iterator app_it = descr2cells.begin();
	for(;app_it != descr2cells.end();app_it++) {
		cell_it cell = app_it->second.begin();
		for(;cell != app_it->second.end();cell++) {
			if(Entry::less_than(cell->entry.from,begin) ||
				Entry::less_than(end,cell->entry.until)) {
				continue;
			}
			list<id_t>::const_iterator emp_it = Fimport1->selected.begin();
			for(;emp_it != Fimport1->selected.end();emp_it++) {
				cell->entry.employee_id = *emp_it;
				cell->selected = hash->search(*cell,*cell);
				if(cell->selected == false) {insertHashCell(*cell);}
			}
		}
	}

	bool conflict = false;
	vector<Cell> old_cells;
	vector<Cell> h_cells = hash->toVector();
	for(cell_it cell = h_cells.begin();
		cell != h_cells.end();cell++) {
		old_cells.push_back(*cell);
		conflict = checkCell(*cell) || conflict;
	} SHOWCONFLICT(conflict,SELECTION_LOCKED);

	descr2cells.clear();
	bool import_ok = h_cells.size();
	cell_it locked = old_cells.begin();
	for(cell_it cell = h_cells.begin();
		cell != h_cells.end();cell++,locked++) {
		if(!reHash(*locked,*cell,true)) {
			vector<Cell> cells;
			cells.push_back(*locked);
			pair<AnsiString,vector<Cell> > descr2cell_pair;
			descr2cell_pair.first = locked->data.info;
			descr2cell_pair.second = cells;
			pair<descr2cells_t::iterator, bool> inserted =
				descr2cells.insert(descr2cell_pair);
			if(!inserted.second) {
				inserted.first->second.push_back(*locked);
			}
			locked->selected = hash->search(*locked,*locked);
			if(locked->selected) {removeHashCell(*locked);}
		} else {import_ok = false;}
	}

	if(descr2cells.size() && person_id == UNDEFINED) {
		Person person;
		person.created_by = db->getUsername();
		person.lastname = calendar;
		TRYULOCK(persons.insert(person); modified = true;);
		person_id = person.id;
	}

	app_it = descr2cells.begin();
	for(;app_it != descr2cells.end();app_it++) {
		Cell new_cell;
		AnsiString color = IntToStr(TFoptions1::o.insert_color);
		AnsiString charge = Codes::color2code(color).app_charge;
		TRYULOCK(
			new_cell.entry.app_id = apps.insert(
				person_id,TFoptions1::o.restrict_updates,
				false,TFoptions1::o.insert_color,charge,
				app_it->first,db->getUsername());
			modified = true;
		);

		TRYSLOCK(cells.readData(new_cell););
		cell_it cell = app_it->second.begin();
		for(;cell != app_it->second.end();cell++) {
			cell->entry.app_id = new_cell.entry.app_id;
			cell->entry.cancelled = new_cell.entry.cancelled;
			cell->entry.pattern = new_cell.entry.pattern;
			TRYSLOCK(
				entries.insert(cell->entry,cell->entry.uuid);
				modified = true;
			);
		}
	}

	if(TFoptions1::auto_save) {save1Click(Sender);} refreshGrid();
	if(import_ok) {MSGO(AnsiString::LoadStr(FMAIN_IMPORTSUCCESS).c_str());}
	else {MSGO(AnsiString::LoadStr(FMAIN_IMPORTFAIL).c_str());}
}
//---------------------------------------------------------------------------

TDateTime __fastcall TFmain1::fromVDate(const wchar_t * vdate,
	TDateTime time_correction) {
	wchar_t datetime[] = L"yyyy-mm-dd hh:mm:ss";
	wchar_t date[] = L"yyyy-mm-dd";
	int str_length = wcslen(vdate);
	if(str_length >= 8) {
		wcsncpy(datetime,vdate,4);
		wcsncpy(datetime+5,vdate+4,2);
		wcsncpy(datetime+8,vdate+6,2);
		if(str_length >= 15) {
			wcsncpy(datetime+11,vdate+9,2);
			wcsncpy(datetime+14,vdate+11,2);
			wcsncpy(datetime+17,vdate+13,2);
			return TDateTime(datetime,TDateTime::DateTime);
		} else {
			wcsncpy(date,datetime,10);
			return TDateTime(date,TDateTime::Date)+time_correction;
		}
	} else {throw new EConvertError("Not a valid date");}
}
//---------------------------------------------------------------------------

Cell __fastcall TFmain1::vEventToCell(VObject * vevent) {
	VObject * v;
	Cell new_cell;

	if((v = isAPropertyOf(vevent,VCUniqueStringProp)) != NULL) {
		new_cell.entry.uuid = AnsiReplaceStr(
			Utf8ToAnsi(vObjectUStringZValue(v)),"\\n","\r\n");
	}

	if((v = isAPropertyOf(vevent,VCDTstartProp)) != NULL) {
		new_cell.entry.from =
			fromVDate(vObjectUStringZValue(v),min_time);
	}

	if((v = isAPropertyOf(vevent,VCDTendProp)) != NULL) {
		new_cell.entry.until =
			fromVDate(vObjectUStringZValue(v),TDateTime(-1)+max_time);
	}

	if(!isAPropertyOf(vevent,VCDTstartProp) &&
		!isAPropertyOf(vevent,VCDTendProp)) {
		throw new EConvertError("Not a valid date");
	} else if(!isAPropertyOf(vevent,VCDTstartProp)) {
		new_cell.entry.from = TDateTime(int(new_cell.entry.until))+min_time;
	} else if(!isAPropertyOf(vevent,VCDTendProp)) {
		new_cell.entry.until = TDateTime(int(new_cell.entry.from))+max_time;
	}

	checkTimeBounds(new_cell.entry.from,new_cell.entry.until);

	int start,until;
	start = DateToUnits(new_cell.entry.from,MAP_RIGHT);
	until = DateToUnits(new_cell.entry.until,MAP_LEFT);
	new_cell.entry.from = UnitsToDate(start,MAP_LEFT);
	new_cell.entry.until = UnitsToDate(until,MAP_RIGHT);
	new_cell.entry.date_diff = (double) (until-start+1)/grid.tunits;

	AnsiString summary,description,url,location,calendar;
	if((v = isAPropertyOf(vevent,VCSummaryProp)) != NULL) {
		summary = AnsiReplaceStr(
			Utf8ToAnsi(vObjectUStringZValue(v)),"\\n","\r\n");
	}
	if((v = isAPropertyOf(vevent,VCDescriptionProp)) != NULL) {
		description = AnsiReplaceStr(
			Utf8ToAnsi(vObjectUStringZValue(v)),"\\n","\r\n");
	}
	if((v = isAPropertyOf(vevent,VCURLProp)) != NULL) {
		url = AnsiReplaceStr(
			Utf8ToAnsi(vObjectUStringZValue(v)),"\\n","\r\n");
	}
	if((v = isAPropertyOf(vevent,VCLocationProp)) != NULL) {
		location = AnsiReplaceStr(
			Utf8ToAnsi(vObjectUStringZValue(v)),"\\n","\r\n");
	}

	AnsiString info = summary;
	if(summary.Length()) {info += " / ";} info += location;
	if(location.Length()) {info += " / ";} info += url;
	if(url.Length()) {info += " / ";} info += description;
	new_cell.data.info = info;
	return new_cell;
}
//---------------------------------------------------------------------------

AnsiString __fastcall TFmain1::EscapeFoldStr(AnsiString text) {
	text = AnsiReplaceStr(text,"\\","\\\\");
	text = AnsiReplaceStr(text,";","\\;");
	text = AnsiReplaceStr(text,",","\\,");
	text = AnsiReplaceStr(text,"\r","");
	text = AnsiReplaceStr(text,"\n","\\n");

	// Fold text
	AnsiString str;
	for(int i=1;i != text.Length()+1;i++) {
		if(i % 50 == 0) {str += "\r\n ";}
		str += text[i];
	}
	return str;
}
//---------------------------------------------------------------------------

AnsiString TFmain1::f_uid;
AnsiString TFmain1::f_sequence;
AnsiString TFmain1::f_date_stamp;
AnsiString TFmain1::f_date_from;
AnsiString TFmain1::f_date_until;
AnsiString TFmain1::f_lastname;
AnsiString TFmain1::f_location;
AnsiString TFmain1::f_telephone;
AnsiString TFmain1::f_telephone2;
AnsiString TFmain1::f_infotext;

TStringList * TFmain1::vEventFile2Fragments(void) {
	LOGFUNC();
	TStringList * fragments = new TStringList();
	AnsiString exe = ExtractFileDir(Application->ExeName);
	int file_handle = fileopen(exe+"\\vEvent.ics",false);
	try {
		int file_length = FileSeek(file_handle,0,2);
		if(file_length != -1 && FileSeek(file_handle,0,0) != -1) {
			unsigned char * buffer = new unsigned char[file_length+1];
			try {
				AnsiString str;
				int bytes = FileRead(file_handle,buffer,file_length);
				for(int i=0;i != bytes;i++) {
					char c = (char) buffer[i];
					if(c == '<') {
						if(str.Length()) {
							fragments->Add(str);
						}
						str = "";
					} else if(c == '>') {
						if(str == "uid") {
							fragments->AddObject("",(TObject *) &f_uid);
						} else if(str == "sequence") {
							fragments->AddObject("",(TObject *) &f_sequence);
						} else if(str == "date_stamp") {
							fragments->AddObject("",(TObject *) &f_date_stamp);
						} else if(str == "date_from") {
							fragments->AddObject("",(TObject *) &f_date_from);
						} else if(str == "date_until") {
							fragments->AddObject("",(TObject *) &f_date_until);
						} else if(str == "lastname") {
							fragments->AddObject("",(TObject *) &f_lastname);
						} else if(str == "location") {
							fragments->AddObject("",(TObject *) &f_location);
						} else if(str == "telephone") {
							fragments->AddObject("",(TObject *) &f_telephone);
						} else if(str == "telephone2") {
							fragments->AddObject("",(TObject *) &f_telephone2);
						} else if(str == "infotext") {
							fragments->AddObject("",(TObject *) &f_infotext);
						}
						str = "";
					} else {str += c;}
				}
				if(str.Length()) {
					fragments->Add(str);
				}
			} __finally {delete[] buffer;}
		}
	} __finally {FileClose(file_handle);}
	return fragments;
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::fillFragments(Cell & cell) {
	f_uid = cell.entry.uuid;
	f_date_stamp = FormatDateTime("yyyymmdd'T'hhmmss",Now());
#ifdef _WIN64
	char seq_buf[20+1]; sprintf(seq_buf,"%lli",time(NULL));
#else
	char seq_buf[10+1]; sprintf(seq_buf,"%ld",time(NULL));
#endif
	f_sequence = String(seq_buf);
	f_date_from = FormatDateTime("yyyymmdd'T'hhmmss",cell.entry.from);
	f_date_until = FormatDateTime("yyyymmdd'T'hhmmss",cell.entry.until);
	f_lastname = EscapeFoldStr(cell.data.person.lastname);

	AnsiString postcode = cell.data.person.postcode;
	AnsiString city = cell.data.person.city;
	f_location = cell.data.person.address;
	if(f_location.Length()) {f_location += ", ";}
	f_location += postcode;
	if(postcode.Length()) {f_location += ", ";}
	f_location += city;
	f_location = EscapeFoldStr(f_location);

	f_telephone = cell.data.person.telephone;
	f_telephone2 = cell.data.person.telephone2;
	f_infotext = EscapeFoldStr(cell.data.info);
}
//---------------------------------------------------------------------------

AnsiString __fastcall TFmain1::employeeAsIcal(Person & person,
	cells_t::iterator row,TStringList & fragments) {
	AnsiString calendar_id = PRODUCT "-";
	calendar_id += grid.id+"-"+person.id;
	AnsiString vCalendar("BEGIN:VCALENDAR\r\nVERSION:2.0\r\n"
		"PRODID:-//PostWare//" PRODUCT " iCal Export//EN\r\n"
		"METHOD:PUBLISH\r\nCALSCALE:GREGORIAN\r\n"
		"X-WR-RELCALID:"+calendar_id+"\r\nX-WR-CALNAME:"+
		EscapeFoldStr(person.lastname)+"\r\n");

	for(row_t::iterator rowcell = row->begin();
		rowcell != row->end();rowcell++) {
		Cell * cell = (Cell *) rowcell->Object;
		if(cell->planned == false || cell->entry.cancelled ||
			cell->entry.app_id == UNDEFINED) {continue;}

		fillFragments(*cell);
		for(int i=0;i != fragments.Count;i++) {
			if(fragments.Strings[i].IsEmpty()) {
				vCalendar += *((AnsiString *) fragments.Objects[i]);
			} else {vCalendar += fragments.Strings[i];}
		}
	} vCalendar += "END:VCALENDAR\r\n";

	return vCalendar;
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::mailEntriesAsIcal(void) {
	LOGFUNC();
	TStringList * fragments = vEventFile2Fragments();
	try {
		unsigned rc = 0;
		for(cells_t::iterator row = RowGrid->Cells.begin();
			row != RowGrid->Cells.end();row++,rc++) {
			Person person = employees[rc];
			if(person.entry_count == 0) {continue;}
			if(find(Fmail1->selected.begin(),Fmail1->selected.end(),
				person.id) == Fmail1->selected.end()) {continue;}
			AnsiString vCalendar = employeeAsIcal(person,row,*fragments);
			AnsiString path = getPath("TMP");
			// ics import depends on the file name
			path = path+"\\"+toFilename(person.id)+".ics";
			TFileStream * file = createfile(path);
			try {file->Write(vCalendar.c_str(),vCalendar.Length());}
			__finally {delete file;}
			AnsiString to(person.lastname); if(to.IsEmpty()) {to = "recipient";}
			AnsiString date = FormatDateTime("yyyy'-'mm'-'dd (ddd)",begin_date);
			sendMail(to,"",date,path); // shellExecute(path,TDateTime(),true);
		}
	} __finally {delete fragments;}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::mailEntriesAsPng(void) {
	LOGFUNC();
	list<id_t> employee_ids = Fmail1->selected;
	list<id_t>::const_iterator it = employee_ids.begin();
	for(;it != employee_ids.end();it++) {
		// TFrange1::buildIdList checked existence
		Person person = employees[findEmployee(*it)];
		if(person.entry_count == 0) {continue;}
		Fmail1->selected.clear();
		Fmail1->selected.push_back(*it);
		Graphics::TBitmap * bitmap = new Graphics::TBitmap();
		try {
			int height = printEntries(NULL, Fmail1);
			bitmap->Assign(Femployee1->Image->Picture);
			bitmap->Height = height;
			AnsiString path = writePngImage(toFilename(person.id),bitmap);
			AnsiString to(person.lastname); if(to.IsEmpty()) {to = "recipient";}
			AnsiString date = FormatDateTime("yyyy'-'mm'-'dd (ddd)",begin_date);
			sendMail(to,"",date,path);
		} __finally {delete bitmap;}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::schedule1Click(TObject *Sender) {
	LOGFUNC();
	if(RowGrid->DragState != Rowgrid::dsNone) {return;}
	Fschedule1->Top = Top+84; Fschedule1->Left = Left+12;
	TModalResult result = Fschedule1->ShowModal();
	if(result != mrAbort && (modified == false ||
		TFoptions1::auto_save)) {save1Click(Sender);}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::pending1Click(TObject *Sender) {
	LOGFUNC();
	if(RowGrid->DragState != Rowgrid::dsNone) {return;}
	TModalResult result = Fpending1->view(
		Fpending1->RowControl->ActivePage,doc_id);
	if(result == mrAbort) {abort1Click(Sender);}
	// Could result in a double abort if Fpending1
	// called view_archive, and the archive aborts
	else if(Fpending1->getModified()) {modified = true;}
	if(result != mrAbort && (modified == false ||
		TFoptions1::auto_save)) {save1Click(Sender);}
	if(result == mrAll && Fschedule1->Visible == false) {
		schedule1Click(Sender);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::toolsCreated1Click(TObject *Sender) {
	LOGFUNC();
	if(RowGrid->DragState != Rowgrid::dsNone) {return;}
	TModalResult result = Fcreated1->view(
		Fcreated1->RowControl->ActivePage);
	if(result != mrAbort && (modified == false ||
		TFoptions1::auto_save)) {save1Click(Sender);}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::toolsEntries1Click(TObject *Sender) {
	LOGFUNC();
	if(RowGrid->DragState != Rowgrid::dsNone) {return;}
	TModalResult result = Fentries1->view(
		Fentries1->RowControl->ActivePage);
	if(result != mrAbort && (modified == false ||
		TFoptions1::auto_save)) {save1Click(Sender);}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::toolsCodes1Click(TObject *Sender) {
	LOGFUNC();
	if(RowGrid->DragState != Rowgrid::dsNone) {return;}
	TModalResult result = Fcodes1->view();
	if(result == mrAbort) {abort1Click(Sender);}
	else {
		if(Fcodes1->getModified()) {modified = true;}
		codes.refresh(); // no need to refreshGrid()
		Foptions1->refreshColorBoxItems();
		Farchive1->refreshPopupMenuItems();
	}
	if(result != mrAbort && (modified == false ||
		TFoptions1::auto_save)) {save1Click(Sender);}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::EmplRowsMouseDown(TObject *Sender,
	TMouseButton Button, TShiftState Shift, int X, int Y) {
	if(RowGrid->DragState != Rowgrid::dsNone) {return;}
	int col,row; EmplRows->MouseToCell(X,Y,col,row);
	if(RowGrid->VerticalRows) {swap(col,row);}
	if(Button != mbLeft && row != -1) {
		dragging = mousedown = false;
		Person person = employees[row];
		if(person.employee) {
			select_id = person.id;
			editMail1Click(NULL);
			ActiveControl = DatePicker1;
		}
	} else if(dragging == false &&
		Shift.Contains(ssDouble) == false) {
		dblclick = mousedown = true;
		drag_row = row;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::EmplRowsMouseMove(TObject *Sender,
	TShiftState Shift, int X, int Y) {
	dragging = false;
	if(RowGrid->DragState != Rowgrid::dsNone) {return;}
	int col,row; EmplRows->MouseToCell(X,Y,col,row);
	if(RowGrid->VerticalRows) {swap(col,row);}
	if(mousedown) {
		dragging = row != -1 && row != drag_row;
		dest_row = row;
	} else if(row != -1) {
		Person person = employees[row];
		EntryText->Caption = person.lastname+" ("+
			person.telephone+"/"+person.telephone2+")";
	}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::EmplRowsMouseUp(TObject *Sender,
	TMouseButton Button, TShiftState Shift, int X, int Y) {
	ActiveControl = DatePicker1;
	mousedown = false;
	if(dragging) {
		dragging = false;
		TRYSLOCK( // both update queries should succeed
			// Although there seems to be a direct relation between the
			// employee row and the employee order, an employee may be
			// marked hidden. So we select the order from the database.
			// Also when someone else deletes an employee from the database,
			// the order values are updated. We need the most recent values.
			persons.execSQL("select EMPLOYEE_ORDER from employees "+
				Policy::lock+"where SCHEDULE_ID="+grid.id+
				" and PERSONAL_ID="+employees[drag_row].id+
				Policy::lock_end);
			if(persons.next()) {
				int order1 = StrToInt(persons.fieldByName(
					"EMPLOYEE_ORDER").toString());
				persons.execSQL("select EMPLOYEE_ORDER from employees "+
					Policy::lock+"where SCHEDULE_ID="+grid.id+
					" and PERSONAL_ID="+employees[dest_row].id+
					Policy::lock_end);
				if(persons.next()) {
					int order2 = StrToInt(persons.fieldByName(
						"EMPLOYEE_ORDER").toString());
					int order = order2; AnsiString shift;
					if(order1 < order2) {order2 = order2+1; shift = "-1";}
					else {swap(order2,order1); order1 = order1-1; shift = "+1";}
					id_t employee_id = employees[drag_row].id;
					persons.execSQL("update employees "+Policy::lock+"set "
						"EMPLOYEE_ORDER=EMPLOYEE_ORDER+"+shift+" where "
						"SCHEDULE_ID="+grid.id+" and EMPLOYEE_ORDER>"+
						order1+" and EMPLOYEE_ORDER<"+order2);
					persons.execSQL("update employees "+Policy::lock+"set "
						"EMPLOYEE_ORDER="+order+" where SCHEDULE_ID="+
						grid.id+" and PERSONAL_ID="+employee_id);
					modified = true;
				}
			}
		);
		// release locks in case one of the persons.next() statements failed.
		if(modified == false || TFoptions1::auto_save) {save1Click(Sender);}
		refreshGrid();
	} else {EmplRows->Invalidate();}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::EmplRowsDblClick(TObject *Sender) {
	ActiveControl = DatePicker1;
	if(dblclick && dragging == false) {
		dblclick = false;
		if(drag_row != -1) {
			per_id = employees[drag_row].id;
			app_id = UNDEFINED;
			// doc_id = UNDEFINED;
			view_archive(Farchive1->PERSONS);
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::firstRun(void) {
	LOGFUNC();
	first_run = true;
	Employees employees_temp;
	db->link(&employees_temp);
	try {
		try {
			Policy::setSLock();
			persons.execSQL("select * from personal "+
				Policy::lock+Policy::lock_end);
			first_run = persons.next() == false;
			if(first_run) {
				Schedule schedule;
				schedule.name = AnsiString::LoadStr(FARCHIVE_SCHEDULE);
				schedule.wstart = 0; schedule.wstop = DAYS_IN_WEEK;
				schedule.dstart = 8; schedule.dstop = 18; schedule.tunits = 24;
				schedule.hidden = true; schedule.created_by = db->getUsername();
				scheds.insert(schedule); Person person;
				for(int i=0;i < 10;i++) {
					person.lastname = "person"+IntToStr(i);
					person.created_by = db->getUsername();
					persons.insert(person);
				}
				for(int i=0;i < 20;i++) { // after persons
					person.lastname = "employee"+IntToStr(i);
					person.created_by = db->getUsername();
					persons.insert(person);
					employees_temp.insert(schedule.id,person.id);
				}
				codes.execSQL("select * from codes "+
					Policy::lock+Policy::lock_end);
				if(codes.next() == false) {
					codes.execSQL("insert into codes "+Policy::lock+
						"values("+QuotedStr("appointment")+","+
						IntToStr(0x80FF00)+",10)");
					codes.execSQL("insert into codes "+Policy::lock+
						"values("+QuotedStr("charge 12.50")+","+
						IntToStr(0xFF0080)+",12.50)");
					codes.execSQL("insert into codes "+Policy::lock+
						"values("+QuotedStr("not available")+","+
						IntToStr(0x80FFFF)+",0)");
					codes.execSQL("insert into codes "+Policy::lock+
						"values("+QuotedStr("req attention")+","+
						IntToStr(0x0000FF)+",10)");
				}
				modified = true;
			}
			save1Click(this); // always release locks
		} catch(Exception & e) {
			TFbase1::rollback();
			// no Fmain::rollback(), would lead to another Refresh
			if(first_run) {
				MSGO(AnsiString::LoadStr(FMAIN_FIRSTRUNFAILURE).c_str());
			}
			throw;
		}
	} __finally {db->unlink(&employees_temp);}
	if(first_run) {
		if(MSGYN(AnsiString::LoadStr(
			FMAIN_FIRSTRUNSUCCESS).c_str()) == ID_YES) {
			PostMessage(DatePicker1->Handle,WM_KEYDOWN,VK_F1,0);
		}
		Refresh();
	}
}
//---------------------------------------------------------------------------

typedef INT (WINAPI * PFNLDBUSER) (LPSAFEARRAY FAR *,PCSTR,INT);
typedef CHAR * (WINAPI * PFNLDBUSERGETERR) (INT);
void __fastcall TFmain1::checkHosts(void) {
#ifdef ACCESS
	//	Get the library instance handle
	HINSTANCE inst = LoadLibrary("msldbusr.dll");
	if(inst == NULL) {throw Exception("Error: Unable to load msldbusr.dll");}
	try {
		PFNLDBUSER get_users; PFNLDBUSERGETERR get_error;
		get_users = (PFNLDBUSER) GetProcAddress(inst,"LDBUser_GetUsers");
		if(get_users == NULL) {throw Exception("Error: Unable to get proc");}
		get_error = (PFNLDBUSERGETERR) GetProcAddress(inst,"LDBUser_GetError");
		if(get_error == NULL) {throw Exception("Error: Unable to get proc");}
		int user_count = get_users(NULL,"CapPlan.mdb",0x8);
		if(user_count < 0) {
			throw Exception(String("Error: ")+get_error(user_count));
		} else if(user_count > 1) {
			throw Exception(AnsiString::LoadStr(FMAIN_TOOMANYCONNECTIONS));
		}
	} __finally {FreeLibrary(inst);}
#else
#ifdef MAXHOSTS
#ifdef MS_SQL
	// checkperm so we see -all- processes instead of just ours.
	AnsiString checkperm = "select 1 from fn_my_permissions(NULL,"
		"'SERVER') where permission_name='VIEW SERVER STATE'";
	AnsiString processes = "select net_address as Host,name "
		"as db from sys.sysprocesses,sys.databases where "
		"sys.sysprocesses.dbid=sys.databases.database_id";
#elif defined INNODB
	AnsiString checkperm = "select 1 from mysql.user where user="
		"substring_index(user(),'@',1) and host=substring_index("
		"user(),'@',-1) and Process_priv='Y'";
	AnsiString processes = "show processlist";
#endif
	db_query.execSQL(checkperm);
	if(db_query.next()) {
		set<AnsiString> hosts;
		db_query.execSQL(processes);
		while(db_query.next()) {
			if(db_query.fieldByName("db").toString().LowerCase() ==
				db->getDatabase().LowerCase()) {
				hosts.insert(db_query.fieldByName("Host").toString().c_str());
			}
		}
		if(hosts.size() <= MAXHOSTS) {return;}
		throw Exception(AnsiString::LoadStr(FMAIN_TOOMANYCONNECTIONS));
	} else {throw Exception("User needs view server state permissions");}
#endif
#endif
}
//---------------------------------------------------------------------------

#define LICENSEKEY 32 // length of a license key
#define LICENSETIME 2*sizeof(time_t) // in chars
#define LICENSEHOURS (24 * 30) // 30 days trial
void __fastcall TFmain1::checkLicense(void) {
#ifdef ACCESS
/* disabled, single user version is free now
	HANDLE fmapping = 0; LPVOID fmemory = 0;
	HANDLE file = CreateFile(Application->ExeName.c_str(),GENERIC_READ,
		FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if(file == INVALID_HANDLE_VALUE) {Abort();}
	try {
		fmapping = CreateFileMapping(file,NULL,PAGE_READONLY,0,0,NULL);
		if(fmapping == 0) {Abort();}
		fmemory = MapViewOfFile(fmapping,FILE_MAP_READ,0,0,0);
		if(fmemory == 0) {Abort();}
		MEMORY_BASIC_INFORMATION meminfo;
		if(VirtualQuery(fmemory,&meminfo,sizeof(meminfo)) == 0) {Abort();}
		unsigned long block_start = (unsigned long) fmemory;
		unsigned long block_end = block_start+meminfo.RegionSize;

		char h1[] = "by PostWare ";
		char h2[] = "(S.B.M. Post), ";
		char header[sizeof(h1)+sizeof(h2)-1] = "";
		strncat(header,h1,sizeof(h1)-1); strncat(header,h2,sizeof(h2)-1);
		for(;(block_start = (unsigned long) memchr((void *) block_start,
			header[0],block_end-block_start)) != NULL && memcmp(
			(void *) (block_start+1),header+1,sizeof(header)-2);
			block_start++);

		if(block_start) {
			char license[LICENSEKEY+1] = ""; char litime[LICENSETIME+1] = "";
			strncat(license,(char *) block_start+sizeof(header)-1,LICENSEKEY);
			strncat(litime,license+sizeof(license)-1-LICENSETIME,LICENSETIME);
			int value; sscanf(litime,"%08x",&value);
			int hours = (time(NULL)-value)/3600;
			AnsiString msg = AnsiString::LoadStr(FMAIN_TRIALDAYSLEFT).c_str();
			msg += IntToStr((LICENSEHOURS-hours)/24); MSGO(msg.c_str());
			if(hours > LICENSEHOURS) {Abort();}
		} else {Abort();}
	} __finally {
		if(fmemory) {UnmapViewOfFile(fmemory);}
		if(fmapping) {CloseHandle(fmapping);}
		CloseHandle(file);
	}
*/
#endif
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::setCountText(int index) {
	int count = hash->elementCount();
	static int current_index = 0;
	if(index) {current_index = index;}
	if(current_index > count) {current_index = count;}
	unsigned char data[80]; MENUITEMINFO item_info = {0};
	item_info.cbSize = sizeof(item_info);
	item_info.fMask = MIIM_STRING; AnsiString item;
	if(count) {item = IntToStr(current_index)+" / "+IntToStr(count);}
	item_info.dwTypeData = item.c_str();
	SetMenuItemInfo(Menu->Handle,10,TRUE,&item_info);
	DrawMenuBar(Handle);
}
//---------------------------------------------------------------------------

void __fastcall TFmain1::setMenu(void) {
	unsigned char data[80]; MENUITEMINFO item_info = {0};
	item_info.cbSize = sizeof(item_info); item_info.fMask = MIIM_TYPE;
	item_info.dwTypeData = (char *) data; item_info.cch = sizeof(data);
	if(GetMenuItemInfo(Menu->Handle,11,TRUE,&item_info)) {
		item_info.fType = item_info.fType | MFT_RIGHTJUSTIFY;
		SetMenuItemInfo(Menu->Handle,11,TRUE,&item_info);
	}
}
//---------------------------------------------------------------------------

