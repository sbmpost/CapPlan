//$Id: Frange.h,v 1.26 1998/01/22 19:37:32 darseq Exp $
#ifndef FrangeH
#define FrangeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <CheckLst.hpp>
#include <Mask.hpp>
#include "Fbase.h"
#include "FixedCheckBox.h"
#include <list>
//---------------------------------------------------------------------------

class TCommonCalendarHack : public TCommonCalendar {
	// The TDateTimePicker component really sucks
	public: __property CalendarHandle;
};

class TFrange1 : public TFbase1
{
__published:	// IDE-managed Components
	TDateTimePicker *DatePicker1;
	TDateTimePicker *DatePicker2;
	TButton *CancelButton;
	TCheckListBox *CheckListBox1;
	TDateTimePicker *TimePicker1;
	TDateTimePicker *TimePicker2;
	TFixedCheckBox *SelectBox;
	TMaskEdit *CountEdit;
	TMaskEdit *TotalEdit;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TButton *OkButton;
	TFixedCheckBox *CheckBox;
	void __fastcall TimePickerChange(TObject *Sender);
	void __fastcall FormMouseWheelUp(TObject *Sender, TShiftState Shift,
		  TPoint &MousePos, bool &Handled);
	void __fastcall FormMouseWheelDown(TObject *Sender, TShiftState Shift,
		  TPoint &MousePos, bool &Handled);
	void __fastcall FormKeyPress(TObject *Sender, char &Key);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall DatePickerDropDown(TObject *Sender);
	void __fastcall DatePickerKeyDown(TObject *Sender, WORD &Key,
		  TShiftState Shift);
	void __fastcall SelectBoxClick(TObject *Sender);
	void __fastcall CountEditExit(TObject *Sender);
	void __fastcall TotalEditExit(TObject *Sender);
	void __fastcall DatePicker1Change(TObject *Sender);
	void __fastcall DatePicker1CloseUp(TObject *Sender);
	void __fastcall DatePicker2Change(TObject *Sender);
	void __fastcall DatePicker2CloseUp(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
	bool reset_focus;
	TDateTime date1,date2;
protected:
	list<id_t> id_list;
	virtual void __fastcall buildIdList(id_t select_id);
	void __fastcall setAndCheckInput(void);
public:		// User declarations
	TDateTime begin,end;
	list<id_t> selected;
	__fastcall TFrange1(TComponent* Owner);
	TModalResult __fastcall view(TDateTime begin_date,
		TDateTime end_date,int count_value,int total_value,
		id_t select_id);
};
//---------------------------------------------------------------------------
extern PACKAGE TFrange1 *Frange1;
//---------------------------------------------------------------------------
#endif

