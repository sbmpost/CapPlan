//$Id: Farchive.h,v 1.117 2014/01/10 15:20:52 darseq Exp $
#ifndef FarchiveH
#define FarchiveH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <Mask.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include "Fbase.h"
#include "Fmdetail.h"
#include "FixedCheckBox.h"
#include "Clients.h"
#include "Tracks.h"
#include "Codes.h"
#include "DataRows.h"
#include "RowControl.h"
//---------------------------------------------------------------------------

class TFarchive1 : public TFmdetail1
{
__published:	// IDE-managed Components
	TTabSheet *PERSONS;
	TTabSheet *APPS;
	TTabSheet *SCHEDULES;
	TDataRows *PersonRows;
	TDataRows *AppRows;
	TDataRows *ScheduleRows;
	TMainMenu *PersonMenu;
	TMainMenu *AppointmentMenu;
	TMainMenu *ScheduleMenu;
	TMenuItem *personsNew1;
	TMenuItem *personsDelete1;
	TMenuItem *personsSearch1;
	TLabel *MemoLabel;
	TLabel *Label1;
	TLabeledEdit *Lastname;
	TMaskEdit *Postcode;
	TLabeledEdit *Address;
	TLabeledEdit *City;
	TMemo *Memo;
	TLabel *InfoLabel;
	TMemo *Info;
	TMenuItem *appsNew1;
	TMenuItem *appsDelete1;
	TPopupMenu *CodePopup;
	TMenuItem *schedulesNew1;
	TMenuItem *schedulesDelete1;
	TMenuItem *schedulesDetails1;
	TMemo *Note;
	TLabel *Label2;
	TMenuItem *appsSelect1;
	TTabSheet *DOCS;
	TDataRows *DocRows;
	TMainMenu *DocumentMenu;
	TMenuItem *docsNew1;
	TMenuItem *docsDelete1;
	TOpenDialog *OpenDialog;
	TMenuItem *docsOpen1;
	TMenuItem *docsView1;
	TMenuItem *docsDuplicate1;
	TMenuItem *print1;
	TMenuItem *print2;
	TMenuItem *print3;
	TMenuItem *print4;
	TMenuItem *docsDetails1;
	TMenuItem *personsDetails1;
	TMenuItem *appsDetails1;
	TFixedCheckBox *EmployeesBox;
	TMenuItem *personsDuplicate1;
	TMenuItem *appsDuplicate1;
	TMenuItem *schedulesDuplicate1;
	TMenuItem *modify1;
	TMenuItem *modify2;
	TMenuItem *modify3;
	TMenuItem *modify4;
	TMenuItem *Locate1;
	TMenuItem *personsEdit1;
	TMenuItem *appsEdit1;
	TMenuItem *docsEdit1;
	TMenuItem *schedulesEdit1;
	TPopupMenu *PopupMenu1;
	TMenuItem *popupModify1;
	TMenuItem *popupDuplicate1;
	TMenuItem *popupDelete1;
	TMenuItem *selectAll1;
	TMenuItem *selectAll2;
	TMenuItem *selectAll3;
	TMenuItem *selectAll4;
	TProgressBar *ProgressBar1;
	TMenuItem *popupDetails1;
	TPopupMenu *PopupMenu2;
	TMenuItem *popupDetails2;
	TMenuItem *popupDelete2;
	TMenuItem *popupDuplicate2;
	TPopupMenu *PopupMenu3;
	TMenuItem *popupDetails3;
	TMenuItem *MenuItem9;
	TMenuItem *popupDelete3;
	TMenuItem *popupDuplicate3;
	TPopupMenu *PopupMenu4;
	TMenuItem *popupDetails4;
	TMenuItem *popupModify4;
	TMenuItem *popupDelete4;
	TMenuItem *popupDuplicate4;
	TMenuItem *popupSelect1;
	TMenuItem *popupOpen1;
	TMenuItem *N2;
	TMenuItem *N3;
	TMenuItem *N4;
	TMenuItem *N5;
	TMenuItem *N1;
	TMenuItem *N6;
	TMenuItem *N7;
	TMenuItem *popupSelectAll1;
	TMenuItem *popupPrint1;
	TMenuItem *popupSelectAll2;
	TMenuItem *popupPrint2;
	TMenuItem *popupSelectAll3;
	TMenuItem *popupPrint3;
	TMenuItem *popupSelectAll4;
	TMenuItem *popupPrint4;
	TMenuItem *popupModify2;
	TMenuItem *popupModify3;
	TMenuItem *popupLocate;
	TMemo *Stat;
	TLabel *StatLabel;
	TMenuItem *popupView1;
	TButton *ScheduleButton;
	TMenuItem *export1;
	TMenuItem *export2;
	TMenuItem *export3;
	TMenuItem *export4;
	TMenuItem *popupExport1;
	TMenuItem *popupExport2;
	TMenuItem *popupExport3;
	TMenuItem *popupExport4;
	void __fastcall SearchChange(TObject *Sender);
	void __fastcall PersonRowsSelectCell(TObject *Sender, int ACol, int ARow,
		  bool &CanSelect);
	void __fastcall MemoExit(TObject *Sender);
	void __fastcall personsNew1Click(TObject *Sender);
	void __fastcall personsDelete1Click(TObject *Sender);
	void __fastcall personsSearch1Click(TObject *Sender);
	void __fastcall MemoEnter(TObject *Sender);
	void __fastcall FormKeyPress(TObject *Sender, char &Key);
	void __fastcall PersonRowsGetEditMask(TObject *Sender, int ACol, int ARow,
		  UnicodeString &Value);
	void __fastcall appsNew1Click(TObject *Sender);
	void __fastcall appsDelete1Click(TObject *Sender);
	void __fastcall AppRowsSelectCell(TObject *Sender, int ACol, int ARow,
		  bool &CanSelect);
	void __fastcall InfoEnter(TObject *Sender);
	void __fastcall InfoExit(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall AppRowsBeforeRefresh(TObject *Sender, QueryPtr &ResultSet,
		  id_t &Id);
	void __fastcall PersonRowsBeforeRefresh(TObject *Sender, QueryPtr &ResultSet,
		  id_t &Id);
	void __fastcall PERSONSShow(TObject *Sender);
	void __fastcall APPSShow(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
		  TShiftState Shift);
	void __fastcall SCHEDULESShow(TObject *Sender);
	void __fastcall ScheduleRowsBeforeRefresh(TObject *Sender,
		  QueryPtr &ResultSet, id_t &Id);
	void __fastcall schedulesNew1Click(TObject *Sender);
	void __fastcall schedulesDelete1Click(TObject *Sender);
	void __fastcall ScheduleRowsSelectCell(TObject *Sender, int ACol, int ARow,
		  bool &CanSelect);
	void __fastcall NoteEnter(TObject *Sender);
	void __fastcall NoteExit(TObject *Sender);
	void __fastcall schedulesDetails1Click(TObject *Sender);
	void __fastcall DataRowsDrawCell(TObject *Sender, int ACol, int ARow,
		  const TRect &Rect, TGridDrawState State);
	void __fastcall ScheduleRowsGetEditMask(TObject *Sender, int ACol, int ARow,
		  UnicodeString &Value);
	void __fastcall appsSelect1Click(TObject *Sender);
	void __fastcall DOCSShow(TObject *Sender);
	void __fastcall docsDelete1Click(TObject *Sender);
	void __fastcall DocRowsBeforeRefresh(TObject *Sender,
		  QueryPtr &ResultSet, id_t &Id);
	void __fastcall RowControlChanging(TObject *Sender, bool &AllowChange,
		  int NewPage);
	void __fastcall docsView1Click(TObject *Sender);
	void __fastcall DocRowsDblClick(TObject *Sender);
	void __fastcall PersonRowsDblClick(TObject *Sender);
	void __fastcall AppRowsDblClick(TObject *Sender);
	void __fastcall docsModify1Click(TObject *Sender);
	void __fastcall docsDuplicate1Click(TObject *Sender);
	void __fastcall AppRowsDataToCell(TObject *Sender, int meta,
		  AnsiString &data);
	void __fastcall DocRowsDataToCell(TObject *Sender, int meta,
		  AnsiString &data);
	void __fastcall ScheduleRowsDataToCell(TObject *Sender, int meta,
		  AnsiString &data);
	void __fastcall PersonRowsCellToData(TObject *Sender, int meta,
		  AnsiString &cell);
	void __fastcall AppRowsCellToData(TObject *Sender, int meta,
		  AnsiString &cell);
	void __fastcall DocRowsCellToData(TObject *Sender, int meta,
		  AnsiString &cell);
	void __fastcall ScheduleRowsCellToData(TObject *Sender, int meta,
		  AnsiString &cell);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall print1Click(TObject *Sender);
	void __fastcall DataRowsNewPage(TObject *Sender, TRect &Rect);
	void __fastcall AppRowsPrintCell(TObject *Sender, long ACol, long ARow,
		  TRect &Rect);
	void __fastcall DocRowsPrintCell(TObject *Sender, long ACol, long ARow,
		  TRect &Rect);
	void __fastcall AppRowsMouseUp(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y);
	void __fastcall PersonRowsBeforeReadRow(TObject *Sender, QueryPtr &ResultSet,
		  id_t Id, bool &Compare);
	void __fastcall AppRowsBeforeReadRow(TObject *Sender, QueryPtr &ResultSet,
		  id_t Id, bool &Compare);
	void __fastcall DocRowsBeforeReadRow(TObject *Sender, QueryPtr &ResultSet,
		  id_t Id, bool &Compare);
	void __fastcall ScheduleRowsBeforeReadRow(TObject *Sender,
		  QueryPtr &ResultSet, id_t Id, bool &Compare);
	void __fastcall PersonRowsAfterReadRow(TObject *Sender, id_t Id,
          bool Compare);
	void __fastcall AppRowsAfterReadRow(TObject *Sender, id_t Id,
          bool Compare);
	void __fastcall DocRowsAfterReadRow(TObject *Sender, id_t Id,
          bool Compare);
	void __fastcall ScheduleRowsAfterReadRow(TObject *Sender, id_t Id,
          bool Compare);
	void __fastcall PersonRowsStoreRow(TObject *Sender, id_t Id);
	void __fastcall AppRowsStoreRow(TObject *Sender, id_t Id);
	void __fastcall DocRowsStoreRow(TObject *Sender, id_t Id);
	void __fastcall ScheduleRowsStoreRow(TObject *Sender, id_t Id);
	void __fastcall PersonRowsAfterRefresh(TObject *Sender, int &SelectRow,
		  bool IsIdRow);
	void __fastcall AppRowsAfterRefresh(TObject *Sender, int &SelectRow,
		  bool IsIdRow);
	void __fastcall ScheduleRowsAfterRefresh(TObject *Sender, int &SelectRow,
		  bool IsIdRow);
	void __fastcall docsDetails1Click(TObject *Sender);
	void __fastcall personsDetails1Click(TObject *Sender);
	void __fastcall appsDetails1Click(TObject *Sender);
	void __fastcall EmployeesBoxEnter(TObject *Sender);
	void __fastcall personsDuplicate1Click(TObject *Sender);
	void __fastcall appsDuplicate1Click(TObject *Sender);
	void __fastcall schedulesDuplicate1Click(TObject *Sender);
	void __fastcall docsNew1Click(TObject *Sender);
	void __fastcall AppRowsEndDoc(TObject *Sender, TRect &Rect);
	void __fastcall modify1Click(TObject *Sender);
	void __fastcall docsLocate1Click(TObject *Sender);
	void __fastcall DocRowsAfterRefresh(TObject *Sender, int &SelectRow,
		  bool IsIdRow);
	void __fastcall DataRowsContextPopup(TObject *Sender,
		  const TPoint &MousePos, bool &Handled);
	void __fastcall selectAll1Click(TObject *Sender);
	void __fastcall ScheduleRowsDblClick(TObject *Sender);
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall DocRowsSelectCell(TObject *Sender, int ACol, int ARow,
		  bool &CanSelect);
	void __fastcall StatEnter(TObject *Sender);
	void __fastcall StatExit(TObject *Sender);
	void __fastcall ScheduleButtonClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall export1Click(TObject *Sender);
	void __fastcall FormMouseWheelDown(TObject *Sender, TShiftState Shift,
		  TPoint &MousePos, bool &Handled);
	void __fastcall FormMouseWheelUp(TObject *Sender, TShiftState Shift,
		  TPoint &MousePos, bool &Handled);
private:	// User declarations
	enum {per=0,app,doc,sch,PAGECOUNT};
	Persons persons; Apps apps; Entries entries;
	Docs docs; Scheds scheds; Employees employs;
	AnsiString ArchiveLabel,OpenLabel;
	int app_entries; bool set_refresh;
	void __fastcall setDocData(Document & document);
	void __fastcall checkOwner(int meta);
	void __fastcall itemClick(TObject *Sender);
	void __fastcall addPopupMenuItem(const Code & code);
	bool __fastcall setWeek(int & week_start,int & week_stop);
	bool __fastcall setDay(int & day_start,int & day_stop,int & tday_units);
	void __fastcall docsInsert(AnsiString src_path,
		id_t person_id,AnsiString filename);
	__inline Currency __fastcall getPrice(int row);
	void __fastcall setTotal(int row_start,int count);
	void __fastcall setMetaIndices(void);
	int LASTNAME,POSTCODE,ADDRESS,CITY,TELEPHONE,
		TELEPHONE2,PER_HIDDEN,MEMOTEXT;
	int APP_ID,EXCLUSIVE,CANCELLED,CODE_NAME,HOURS,PAID,
		PRICE,COLOR,CHARGE,FROM,UNTIL,ENTRIES,INFOTEXT;
	int DOC_NAME,DOC_SIZE,COMMENT,SIGNED,URGENT,SCHEDULE,
		REJECTED,LUPDATE_UTC,REJECTED_DATE,PLANNED_DATE,
		ORDERED_DATE,CHARGED_DATE,PENDING_REVIEW,
		PENDING_SCHEDULE,AVAILABLE,STATETEXT,DOC_DATA;
	int SCHEDULE_NAME,EMPLOYEE,WEEK_START,
		WEEK_STOP,DAY_START,DAY_STOP,DAY_UNITS,
		SCHEDULE_ID,SCH_HIDDEN,NOTETEXT;
	int PER_CREATED_BY,PER_CREATED_DATE,
		APP_CREATED_BY,APP_CREATED_DATE,
		DOC_CREATED_BY,DOC_CREATED_DATE,
		SCH_CREATED_BY,SCH_CREATED_DATE;
public:		// User declarations
	bool refresh_info;
	__fastcall TFarchive1(TComponent* Owner);
	void __fastcall refreshPopupMenuItems(void);
	TModalResult __fastcall view(id_t & person_id,id_t & appointment_id,
		id_t & document_id,TTabSheet * show_sheet,int & appointment_entries);
};
//---------------------------------------------------------------------------
extern PACKAGE TFarchive1 *Farchive1;
//---------------------------------------------------------------------------
#endif

