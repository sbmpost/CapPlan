//$Id: Fmain.h,v 1.229 1998/01/26 00:26:24 darseq Exp $
#ifndef FmainH
#define FmainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <Grids.hpp>
#include "Fbase.h"
#include "Fprint.h"
#include "FixedScrollBox.h"
#include "RowGrid.h"
#include "DataRows.h"
#include <Dialogs.hpp>
#include <set>
#include "hash.h"
#include "defines.h"
#include "Cells.h"
#include "Codes.h"
#include "versit/vcc.h"
//---------------------------------------------------------------------------

typedef vector<Cell>::iterator cell_it;
typedef enum {MAP_LEFT,MAP_RIGHT} mdir_t;
typedef enum {DAY_MODE,WEEK_MODE} mode_t;
typedef enum {FIRST,LAST,NEXT,PREV} browse_t;
typedef map<AnsiString,vector<Cell> > descr2cells_t;
enum {CANCELLED=0,PLANNED,SELECTED,CHANGED};
class TFmain1 : public TFbase1
{
__published:	// IDE-managed Components
	TMainMenu *MainMenu1;
	TMenuItem *save1;
	TMenuItem *abort1;
	TPanel *StatusPanel;
	TMenuItem *editDeselectAll1;
	TMenuItem *editDelete1;
	TMenuItem *editBlock1;
	TMenuItem *entriesNew1;
	TMenuItem *entriesPrev1;
	TMenuItem *entriesNext1;
	TMenuItem *archive1;
	TMenuItem *entriesSelect1;
	TMenuItem *editSetRange1;
	TMenuItem *editSplit1;
	TMenuItem *entriesLast1;
	TTimer *RefreshInfo;
	TMenuItem *entriesFirst1;
	TMenuItem *viewWeek1;
	TMenuItem *viewVertical1;
	TMenuItem *editPrint1;
	TStringGrid *TimeLabels;
	TMenuItem *pending1;
	TMenuItem *editDeselectChanged1;
	TMenuItem *toolsCodes1;
	TMenuItem *viewCancelled1;
	TMenuItem *N1;
	TMenuItem *N2;
	TLabel *EntryText;
	TMenuItem *editFindEmpty1;
	TDateTimePicker *DatePicker1;
	TButton *dayView1;
	TButton *weekView1;
	TMenuItem *toolsEntries1;
	TMenuItem *help1;
	TMenuItem *about1;
	TMenuItem *debugMode1;
	TMenuItem *index1;
	TMenuItem *schedule1;
	TPopupMenu *PopupMenu1;
	TMenuItem *popupSelectEntry1;
	TMenuItem *popupBlock1;
	TMenuItem *popupDelete1;
	TMenuItem *edit1;
	TMenuItem *editMove1;
	TMenuItem *editPaste1;
	TMenuItem *popupSplit1;
	TMenuItem *toolsOptions1;
	TMenuItem *N3;
	TMenuItem *view1;
	TMenuItem *N4;
	TMenuItem *popupDeselectAll1;
	TMenuItem *popupDeselectRed1;
	TMenuItem *editCancel1;
	TMenuItem *viewRefresh1;
	TMenuItem *popupFindEmpty1;
	TMenuItem *popupSetRange1;
	TMenuItem *popupPrintEntries1;
	TMenuItem *N5;
	TMenuItem *N6;
	TMenuItem *tools1;
	TMenuItem *toolsCreated1;
	TFixedScrollBox *ScrollBox;
	TShape *Shape2;
	TStringGrid *EmplRows;
	TShape *Shape4;
	TShape *Shape3;
	TRowGrid *RowGrid;
	TShape *Shape1;
	TMenuItem *CountText;
	TMenuItem *emailSupport1;
	TDataRows *DataRows;
	TMenuItem *editExport1;
	TMenuItem *home1;
	TTimer *KeepAlive;
	TMenuItem *editClearPattern1;
	TMenuItem *editCrossPattern1;
	TMenuItem *editFDiagPattern1;
	TMenuItem *popupClearPattern1;
	TMenuItem *popupCrossPattern1;
	TMenuItem *popupFDiagPattern1;
	TMenuItem *N7;
	TMenuItem *N8;
	TMenuItem *editImport1;
	TOpenDialog *OpenDialog;
	TMenuItem *editMail1;
	TMenuItem *N9;
	TMenuItem *popupMailEntries1;
	void __fastcall DatePicker1Change(TObject *Sender);
	void __fastcall abort1Click(TObject *Sender);
	void __fastcall save1Click(TObject *Sender);
	void __fastcall options1Click(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall editDeselectAll1Click(TObject *Sender);
	void __fastcall editDelete1Click(TObject *Sender);
	void __fastcall EmplRowsDrawCell(TObject *Sender, int ACol, int ARow,
		  TRect &Rect, TGridDrawState State);
	void __fastcall RowGridDrawRowCell(TObject *Sender, RowCell RowCell,
		  TRect Rect);
	void __fastcall RowGridInsertRowCell(TObject *Sender, RowCell &RowCell,
		  bool &CanInsert);
	void __fastcall RowGridRemoveRowCell(TObject *Sender, RowCell &RowCell,
		  bool &CanRemove);
	void __fastcall DatePicker1DropDown(TObject *Sender);
	void __fastcall editSetRange1Click(TObject *Sender);
	void __fastcall entriesNew1Click(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
		  TShiftState Shift);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall editSplit1Click(TObject *Sender);
	void __fastcall entriesSelect1Click(TObject *Sender);
	void __fastcall FormMouseWheelDown(TObject *Sender, TShiftState Shift,
		  TPoint &MousePos, bool &Handled);
	void __fastcall FormMouseWheelUp(TObject *Sender, TShiftState Shift,
		  TPoint &MousePos, bool &Handled);
	void __fastcall TimeLabelsDrawCell(TObject *Sender, int ACol, int ARow,
		  TRect &Rect, TGridDrawState State);
	void __fastcall RowGridDblClickRowCell(TObject *Sender, RowCell RowCell);
	void __fastcall RowGridMouseDown(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y);
	void __fastcall DatePicker1CloseUp(TObject *Sender);
	void __fastcall entriesFirst1Click(TObject *Sender);
	void __fastcall entriesLast1Click(TObject *Sender);
	void __fastcall RefreshInfoTimer(TObject *Sender);
	void __fastcall entriesPrev1Click(TObject *Sender);
	void __fastcall dayView1Click(TObject *Sender);
	void __fastcall viewWeek1Click(TObject *Sender);
	void __fastcall weekView1Click(TObject *Sender);
	void __fastcall viewVertical1Click(TObject *Sender);
	void __fastcall editPrint1Click(TObject *Sender);
	void __fastcall RowGridMouseMove(TObject *Sender, TShiftState Shift,
		  int X, int Y);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall pending1Click(TObject *Sender);
	void __fastcall RowGridClearRowCell(TObject *Sender, RowCell &RowCell,
		  bool &CanClear);
	void __fastcall RowGridDragRowCell(TObject *Sender, RowCell &RowCell,
		  TCellState CellState, bool IsCopy, bool &CanDrag);
	void __fastcall RowGridPaint(TObject *Sender);
	void __fastcall editDeselectChanged1Click(TObject *Sender);
	void __fastcall toolsCodes1Click(TObject *Sender);
	void __fastcall viewCancelled1Click(TObject *Sender);
	void __fastcall EmplRowsMouseDown(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y);
	void __fastcall EmplRowsMouseMove(TObject *Sender, TShiftState Shift,
		  int X, int Y);
	void __fastcall EmplRowsMouseUp(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y);
	void __fastcall editFindEmpty1Click(TObject *Sender);
	void __fastcall RowGridDrawRowLine(TObject *Sender, int Row, TRect Rect);
	void __fastcall toolsEntries1Click(TObject *Sender);
	void __fastcall aboutClick(TObject *Sender);
	void __fastcall debugClick(TObject *Sender);
	void __fastcall index1Click(TObject *Sender);
	void __fastcall schedule1Click(TObject *Sender);
	void __fastcall popupSelectEntry1Click(TObject *Sender);
	void __fastcall popupDelete1Click(TObject *Sender);
	void __fastcall RowGridContextPopup(TObject *Sender,
		  const TPoint &MousePos, bool &Handled);
	void __fastcall editCancel1Click(TObject *Sender);
	void __fastcall viewRefresh1Click(TObject *Sender);
	void __fastcall entriesNext1Click(TObject *Sender);
	void __fastcall toolsCreated1Click(TObject *Sender);
	void __fastcall archive1Click(TObject *Sender);
	void __fastcall RowGridDrawFocusRect(TObject *Sender, TRect Rect);
	void __fastcall EmplRowsDblClick(TObject *Sender);
	void __fastcall EmplRowsMouseWheelDown(TObject *Sender, TShiftState Shift,
		  TPoint &MousePos, bool &Handled);
	void __fastcall EmplRowsMouseWheelUp(TObject *Sender, TShiftState Shift,
		  TPoint &MousePos, bool &Handled);
	void __fastcall TimeLabelsMouseDown(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall emailSupport1Click(TObject *Sender);
	void __fastcall FormKeyPress(TObject *Sender, char &Key);
	void __fastcall editExport1Click(TObject *Sender);
	void __fastcall editImport1Click(TObject *Sender);
	void __fastcall TimeLabelsMouseMove(TObject *Sender, TShiftState Shift,
		  int X, int Y);
	void __fastcall home1Click(TObject *Sender);
	void __fastcall KeepAliveTimer(TObject *Sender);
	void __fastcall editClearPattern1Click(TObject *Sender);
	void __fastcall editFDiagPattern1Click(TObject *Sender);
	void __fastcall editCrossPattern1Click(TObject *Sender);
	void __fastcall popupClearPattern1Click(TObject *Sender);
	void __fastcall popupCrossPattern1Click(TObject *Sender);
	void __fastcall popupFDiagPattern1Click(TObject *Sender);
	void __fastcall editMail1Click(TObject *Sender);
private:	// User declarations
	// void __fastcall setModified(bool value) {
	//	TFbase1::setModified(value);
	//	MainMenu1->Items->Items[save1->MenuIndex]->Enabled = modified;
		// revert should always be enabled so the user can release locks
	// }
	TWindowState mainstate;
	TMenuItem * view_menu;
	AnsiString version;

	Persons persons;
	Entries entries;
	Scheds scheds;
	Cells cells;
	Codes codes;
	Apps apps;

	mode_t mode;
	int drag_row;
	int dest_row;
	int context_x;
	int context_y;
	bool dblclick;
	bool dragging;
	bool mousedown;
	bool reset_focus;
	bool inserting;
	bool deleting;
	bool hash_only;
	bool cell_swap;
	bool first_run;
	bool hash_sorted;
	bool browse_changed;
	hash_space::HashTable<Cell> * hash;
	set<id_t> employees_locked;
	TDateTime previous_new;
	TDateTime begin_date;
	TDateTime end_date;

	TDateTime print_date; id_t select_id;
	int __fastcall printHeader(TRect rect,TCanvas * canvas);
	int __fastcall printEmployee(Person & person, cells_t::iterator row,
		TRect rect,TCanvas * canvas,int space,TFprint1 * form);

	static AnsiString f_uid,f_sequence,f_date_stamp;
	static AnsiString f_date_from,f_date_until;
	static AnsiString f_lastname,f_location;
	static AnsiString f_telephone,f_telephone2;
	static AnsiString f_infotext;

	void __fastcall clearRowGrid(void);
	void __fastcall changeScheduleClick(TObject *Sender);
	void __fastcall onMessage(tagMSG & Msg,bool & Handled);
	void __fastcall onRestore(TObject * Sender);
	void __fastcall onMinimize(TObject * Sender);
	void __fastcall onException(TObject *Sender,Exception * e);
	AnsiString __fastcall CellToInfo(Cell & cell);
	int __fastcall DateToUnits(TDateTime date,mdir_t map_direction);
	TDateTime __fastcall UnitsToDate(int units,mdir_t map_direction);
	void __fastcall setSize(void);
	void __fastcall addMenuItem(const Schedule & schedule);
	void __fastcall getGrid(void);
	void __fastcall rollback(void);
	bool __fastcall reHash(Cell & hash_cell,Cell & new_cell,bool compare);
	bool __fastcall checkCell(Cell & hash_cell,Cell * drag = NULL);
	void __fastcall insertHashCell(Cell & cell);
	void __fastcall removeHashCell(Cell & cell);
	void __fastcall browseSelection(browse_t browse);
	void __fastcall removeSelection(bool changed_only = false);
	void __fastcall toggleRowCell(RowCell & RowCell);
	void __fastcall refreshGrid(void);
	void __fastcall Refresh(void);
	void __fastcall setVertical(void);
	void __fastcall setHorizontal(void);
	void __fastcall setRowCellPattern(TObject * Sender,TBrushStyle pattern);
	void __fastcall getVersion(int &major,int &minor,int &release,int &build);
	void __fastcall firstRun(void);
	void __fastcall checkHosts(void);
	void __fastcall checkLicense(void);
	void __fastcall selectEntries(unsigned int app_entries);
	void __fastcall findEntries(list<id_t> employee_ids,
		int & count,int start,int until,int min_units,
		Cells * cells);
	void __fastcall setCurrentDate(TDateTime date);
	void __fastcall setCountText(int index = 0);
	void __fastcall setMenu(void);
	void __fastcall insertImports(TObject * Sender,
		TDateTime begin,TDateTime end,descr2cells_t & descr2cells,
		id_t person_id,AnsiString calendar);
	TDateTime __fastcall fromVDate(const wchar_t * vdate,
		TDateTime time_correction);
	Cell __fastcall vEventToCell(VObject * vevent);
	AnsiString __fastcall EscapeFoldStr(AnsiString text);
	TStringList * vEventFile2Fragments(void);
	void __fastcall fillFragments(Cell & cell);
	AnsiString __fastcall employeeAsIcal(Person & person,
		cells_t::iterator row,TStringList & fragments);
	void __fastcall mailEntriesAsIcal(void);
	void __fastcall mailEntriesAsPng(void);
public:		// User declarations
	// public members for Fpending/
	// Fentries/Fschedule/Femployee
	id_t per_id,app_id,doc_id;

	int __fastcall printEntries(TPrinter * printer,TFprint1 * form);
	void __fastcall view_archive(TTabSheet * tabsheet);
	int __fastcall getEntryCount(void) {return hash->elementCount();}
	__fastcall TFmain1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFmain1 *Fmain1;
//---------------------------------------------------------------------------
#endif

