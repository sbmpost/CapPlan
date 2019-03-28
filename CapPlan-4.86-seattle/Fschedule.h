//$Id: Fschedule.h,v 1.26 2013/02/16 11:41:37 darseq Exp $
#ifndef FscheduleH
#define FscheduleH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include "Fbase.h"
#include "DataRows.h"
//---------------------------------------------------------------------------

class TFschedule1 : public TFbase1
{
__published:	// IDE-managed Components
	TButton *selectPerson;
	TLabel *Label2;
	TEdit *Edit1;
	TShape *Shape3;
	TButton *selectDocument;
	TShape *Shape1;
	TShape *Shape2;
	TEdit *Edit2;
	TShape *Shape4;
	TButton *selectAppointment;
	TLabel *Label3;
	TLabel *Label4;
	TButton *selectEntries;
	TShape *Shape5;
	TEdit *Entries;
	TButton *AbortButton;
	TButton *FinishButton;
	void __fastcall selectPersonClick(TObject *Sender);
	void __fastcall selectAppointmentClick(TObject *Sender);
	void __fastcall FinishButtonClick(TObject *Sender);
	void __fastcall selectEntriesClick(TObject *Sender);
	void __fastcall selectDocumentClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormKeyPress(TObject *Sender, char &Key);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
		  TShiftState Shift);
	void __fastcall AbortButtonClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
	AnsiString entries;
	id_t per_id,app_id;
	id_t doc_id,doc_name;
	AnsiString lastname;
	int doc_row; bool is_document;
	TWinControl * active_control;
	TButton * curr_button;
	void __fastcall checkDocId(void);
	bool __fastcall getPerson(void);
	bool __fastcall getDocument(void);
	bool __fastcall getAppointment(void);
	void __fastcall setButton(TButton * button);
	void __fastcall nextButton(TButton * next_button);
	int PER_LASTNAME,DOC_LASTNAME,PERSONAL_ID,DOC_NAME;
public:		// User declarations
	void __fastcall setPerson(id_t person_id,AnsiString name);
	void __fastcall setDocument(id_t document_id,int row);
	void __fastcall setAppointment(id_t appointment_id);
	__fastcall TFschedule1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFschedule1 *Fschedule1;
//---------------------------------------------------------------------------
#endif

