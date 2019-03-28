//$Id: Fdetails.h,v 1.27 2013/03/28 14:01:16 darseq Exp $
#ifndef FdetailsH
#define FdetailsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include "Fbase.h"
#include "FixedCheckBox.h"
//---------------------------------------------------------------------------

class TFdetails1 : public TFbase1
{
__published:
	TPageControl *PageControl1;
	TTabSheet *ATTRIBUTES;
	TTabSheet *HISTORY;
	TFixedCheckBox *AttributeBox1;
	TFixedCheckBox *AttributeBox2;
	TButton *CancelButton;
	TLabel *CreatedBy;
	TLabel *Created;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *PlannedLabel;
	TLabel *OrderedLabel;
	TLabel *ChargedLabel;
	TLabel *Planned;
	TLabel *Ordered;
	TLabel *Charged;
	TFixedCheckBox *AttributeBox3;
	TButton *OkButton;
	TFixedCheckBox *AttributeBox4;
	void __fastcall FormKeyPress(TObject *Sender, char &Key);
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
		  TShiftState Shift);
	void __fastcall OkButtonClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
private:
	bool reset_focus; Document * doc;
	TDateTime date; AnsiString DetailsLabel;
	void __fastcall onClose(TObject *Sender,TCloseAction &Action);
	void __fastcall SignedBoxClick(TObject *Sender);
	void __fastcall ScheduleBoxClick(TObject *Sender);
	void __fastcall CancelledBoxClick(TObject *Sender);
public:
	BOOL enable;
	virtual __fastcall TFdetails1(TComponent *Owner);
	TModalResult __fastcall view_per(bool & per_employee,bool & per_hidden,
		bool enable_hidden);
	TModalResult __fastcall view_app(bool & app_paid,bool & app_cancelled,
		bool & cancelled_intime,bool & app_exclusive,bool enable_exclusive);
	TModalResult __fastcall view_doc(Document * document,bool enable_cancel,
		bool modal);
	TModalResult __fastcall view_sch(bool & sch_hidden,bool enable_hidden);
};
//---------------------------------------------------------------------------
extern TFdetails1 *Fdetails1;
//---------------------------------------------------------------------------
#endif

