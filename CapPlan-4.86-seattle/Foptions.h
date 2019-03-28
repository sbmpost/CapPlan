//$Id: Foptions.h,v 1.81 2013/03/22 17:58:41 darseq Exp $
#ifndef FoptionsH
#define FoptionsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
#include <Mask.hpp>
#include "Fbase.h"
#include "FixedCheckBox.h"
#include "Codes.h"
//---------------------------------------------------------------------------

#define NAMESIZE 50
typedef struct options_label { // written in this order to cfgfile...
// Font
	TFontCharset charset;
	char name[NAMESIZE+1];
	int color;
	int size;
	int pitch;
	TFontStyles style;

// grid size
	int rows;
	int days;
	int labels;

// checkbox options
	bool clear_after;
	bool clear_onnew;
	bool remember;
	bool auto_search;
	bool auto_refresh;
	bool week_colors;
	bool entry_info;
	bool print_times;
	bool week_mode;
	bool vertical_rows;
	bool restrict_updates;
	bool signed_sched;
	bool unsigned_red;
	bool invoice;
	char page_nr;

// values/paths
	int copies;
	int insert_color;
	int telephone_length;
	char postcode_format[MAXPOSTCODE+1];
	char reserved_string[NAMESIZE+1];
	char documents_path[MAX_PATH];
	char templates_path[MAX_PATH];
	// MAX_PATH includes '\0' char
	int default_id;

// additional checkbox options
	bool schedule_doc;
	bool repurchase;

// final options
	int font;
	int time;

// final final options
	bool hide_patterns;
	bool run_macros;
} options_t;

class TFoptions1 : public TFbase1
{
__published:	// IDE-managed Components
	TFontDialog *FontDialog1;
	TMainMenu *MainMenu1;
	TMenuItem *save1;
	TMenuItem *Entryfont1;
	TMenuItem *DocumentsPath1;
	TMenuItem *TemplatesPath1;
	TMenuItem *Refresh1;
	TMenuItem *Folders1;
	TGroupBox *GroupBox5;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TMaskEdit *CopiesEdit;
	TColorBox *ColorBox;
	TEdit *PostcodeEdit;
	TMaskEdit *TelephoneEdit;
	TGroupBox *GroupBox3;
	TFixedCheckBox *SearchBox;
	TFixedCheckBox *ClearOnNewBox;
	TFixedCheckBox *ClearAfterBox;
	TFixedCheckBox *RememberBox;
	TFixedCheckBox *CurrentBox;
	TGroupBox *GroupBox2;
	TFixedCheckBox *TimesBox;
	TFixedCheckBox *AlternateBox;
	TFixedCheckBox *InfoBox;
	TFixedCheckBox *VerticalBox;
	TFixedCheckBox *RestrictBox;
	TGroupBox *GroupBox1;
	TMaskEdit *RowsEdit;
	TMaskEdit *DaysEdit;
	TMaskEdit *LabelsEdit;
	TGroupBox *GroupBox4;
	TFixedCheckBox *LockBox;
	TFixedCheckBox *UnsignedBox;
	TFixedCheckBox *SignedBox;
	TFixedCheckBox *ScheduleBox;
	TFixedCheckBox *RepurchaseBox;
	TFixedCheckBox *InvoiceBox;
	TFixedCheckBox *WeekBox;
	TMaskEdit *FontEdit;
	TMaskEdit *TimeEdit;
	TFixedCheckBox *RefreshBox;
	TFixedCheckBox *PatternBox;
	TFixedCheckBox *MacroBox;
	void __fastcall RowsEditExit(TObject *Sender);
	void __fastcall ClearAfterBoxClick(TObject *Sender);
	void __fastcall ClearOnNewBoxClick(TObject *Sender);
	void __fastcall RememberBoxClick(TObject *Sender);
	void __fastcall save1Click(TObject *Sender);
	void __fastcall SearchBoxClick(TObject *Sender);
	void __fastcall DaysEditExit(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Entryfont1Click(TObject *Sender);
	void __fastcall ColorBoxExit(TObject *Sender);
	void __fastcall RefreshBoxClick(TObject *Sender);
	void __fastcall LabelsEditExit(TObject *Sender);
	void __fastcall TimesBoxClick(TObject *Sender);
	void __fastcall AlternateBoxClick(TObject *Sender);
	void __fastcall VerticalBoxClick(TObject *Sender);
	void __fastcall WeekBoxClick(TObject *Sender);
	void __fastcall InfoBoxClick(TObject *Sender);
	void __fastcall PostcodeEditExit(TObject *Sender);
	void __fastcall TelephoneEditExit(TObject *Sender);
	void __fastcall RestrictBoxClick(TObject *Sender);
	void __fastcall CopiesEditExit(TObject *Sender);
	void __fastcall CurrentBoxClick(TObject *Sender);
	void __fastcall ScheduleBoxClick(TObject *Sender);
	void __fastcall UnsignedBoxClick(TObject *Sender);
	void __fastcall SignedBoxClick(TObject *Sender);
	void __fastcall DocumentsPath1Click(TObject *Sender);
	void __fastcall TemplatesPath1Click(TObject *Sender);
	void __fastcall Refresh1Click(TObject *Sender);
	void __fastcall EditChange(TObject *Sender);
	void __fastcall ColorBoxChange(TObject *Sender);
	void __fastcall ViewChange(TObject *Sender);
	void __fastcall LockBoxClick(TObject *Sender);
	void __fastcall InvoiceBoxClick(TObject *Sender);
	void __fastcall RepurchaseBoxClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FontEditExit(TObject *Sender);
	void __fastcall TimeEditExit(TObject *Sender);
	void __fastcall MacroBoxClick(TObject *Sender);
	void __fastcall PatternBoxClick(TObject *Sender);
private:	// User declarations
	bool refresh; // refreshForms() yes or no
	int page_index;
	void __fastcall addColorBoxItem(const Code & code);
	__property bool modified = {write = setModified};
	void __fastcall setModified(bool value) {
		MainMenu1->Items->Items[
			save1->MenuIndex]->Enabled = value;
	}
	static bool internal_call;
	static void __fastcall MakeConfig(AnsiString sch);
	static void __fastcall OpenConfig(id_t schedule_id) {
		internal_call = true; openConfig(schedule_id);
	}
	static FILE * cfgfile;
	static id_t default_id;
	static bool default_remember;
	static bool default_week_mode;
	static bool default_auto_search;
	static bool default_clear_after;
	static bool default_clear_onnew;
	static bool default_auto_refresh;
	static bool default_schedule_doc;
	static bool default_unsigned_red;
	static bool default_signed_sched;
	static bool default_run_macros;
	static bool default_repurchase;
	static bool default_invoice;
	static char default_page_nr;
	static char default_docs_path[MAX_PATH];
	static char default_tmpl_path[MAX_PATH];
	static char default_postcode_format[MAXPOSTCODE+1];
	static int default_telephone_length;
	static int default_insert_color;
	static int default_copies;
	static int default_font;
	static int default_time;
public:		// User declarations
	static bool auto_save;
	static bool review_tab;
	static id_t opened_id;
	static options_t o;
	__fastcall TFoptions1(TComponent* Owner);
	void __fastcall refreshColorBoxItems(void);
	void __fastcall view(int pending_page_index);
	static void __fastcall openConfig(id_t schedule_id);
	static void __fastcall saveConfig(void);
	static void __fastcall closeConfig(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TFoptions1 *Foptions1;
//---------------------------------------------------------------------------
#endif

