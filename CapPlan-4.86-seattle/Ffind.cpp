//$Id: Ffind.cpp,v 1.10 2011/08/03 22:18:21 darseq Exp $
#include <vcl.h>
#pragma hdrstop
#include "Ffind.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FixedCheckBox"
#pragma link "Frange"
#pragma resource "*.dfm"
TFfind1 *Ffind1;
//---------------------------------------------------------------------------
__fastcall TFfind1::TFfind1(TComponent* Owner) : TFrange1(Owner) {
}
//---------------------------------------------------------------------------

void __fastcall TFfind1::buildIdList(id_t select_id) {
	LOGFUNC();
	vector<Schedule>::const_iterator schedule_it = schedules.begin();
	for(;schedule_it != schedules.end();schedule_it++) {
		id_list.push_back(schedule_it->id);
		int index = CheckListBox1->Items->Add(schedule_it->name);
		CheckListBox1->Checked[index] = SelectBox->Checked ||
			schedule_it->id == select_id;
	}
}
//---------------------------------------------------------------------------

