//$Id: testgrid.cpp,v 1.13 2008/09/12 14:57:02 darseq Exp $
#include <vcl.h>
#pragma hdrstop

#include "testgrid.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RowGrid"
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
class StringObject {
public:
	AnsiString string;
	__fastcall StringObject(AnsiString AString) {string = AString;}
};

__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner) {
/*
	RowGrid1->RowCount = 5;
	int height = RowGrid1->RowLineWidth * (RowGrid1->RowCount+1);
	RowGrid1->RowHeight = (RowGrid1->ClientHeight-height)/RowGrid1->RowCount;
	RowGrid1->ClientHeight = RowGrid1->RowHeight * RowGrid1->RowCount + height;

	RowGrid1->Units = 20;
	RowGrid1->UnitWidth = RowGrid1->ClientWidth/RowGrid1->Units;
	RowGrid1->ClientWidth = RowGrid1->UnitWidth * RowGrid1->Units;
*/
}
//---------------------------------------------------------------------------

__fastcall TForm1::~TForm1(void) {
	RowGrid1->ClearDragState();
	for(int r=0;r < RowGrid1->RowCount;r++) {RowGrid1->ClearRow(r);}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RowGrid1ClearRowCell(TObject *Sender,
	  RowCell &RowCell,bool &CanClear) {
	CanClear = true;
	delete RowCell.Object;
	RowCell.Object = NULL;
}
//---------------------------------------------------------------------------

AnsiString __fastcall TForm1::CellToString(RowCell & RowCell) {
	AnsiString string;
	string += "row:"+IntToStr(RowCell.Row)+
	string += " start:"+IntToStr(RowCell.Start)+
	string += " units:"+IntToStr(RowCell.Units)+
	string += " zorder:"+IntToStr(RowCell.Zorder);
	if(RowCell.Object) {string += ((StringObject *) RowCell.Object)->string;}
	return string;
}
//---------------------------------------------------------------------------

AnsiString __fastcall TForm1::CellStateToString(TCellState CellState) {
	switch(CellState) {
		case csLeft: return "csLeft";
		case csRight: return "csRight";
		case csCenter: return "csCenter";
		case csEmpty: return "csEmpty";
		case csNone: return "csNone";
		default: return "";
	}
}
//---------------------------------------------------------------------------

AnsiString __fastcall TForm1::DragStateToString(Rowgrid::TDragState state) {
	switch(state) {
		case dsDragDrop: return "dsDragDrop";
		case dsCopyDrop: return "dsCopyDrop";
		case dsInsert: return "dsInsert";
		case dsNone: return "dsNone";
		default: return "";
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RowGrid1DragRowCell(TObject *Sender,
	  RowCell &RowCell, TCellState CellState, bool IsCopy, bool &CanDrag) {
	// SHIFT+left click holds the cell
	Memo1->Lines->Add("OnDragRowCell "+CellToString(RowCell)+
		" - CellState: "+CellStateToString(CellState)+ " - IsCopy: "+
		IntToStr(IsCopy)+" - DragState:"+
		DragStateToString(RowGrid1->DragState));
	if(IsCopy) {RowCell.Object = NULL;}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RowGrid1InsertRowCell(TObject *Sender,
	  RowCell &RowCell, bool &CanInsert) {
	RowCell.Object = new StringObject(" - StringObject insert size:"+
		IntToStr(RowGrid1->Cells[RowCell.Row].size())); // before CellToString
	Memo1->Lines->Add("OnInsertRowCell "+CellToString(RowCell)+
		" - DragState:"+DragStateToString(RowGrid1->DragState));
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RowGrid1RemoveRowCell(TObject *Sender,
	  RowCell &RowCell, bool &CanRemove) {
	Memo1->Lines->Add("OnRemoveRowCell "+CellToString(RowCell));
	delete RowCell.Object; // after CellToString
	RowCell.Object = NULL;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RowGrid1Paint(TObject *Sender) {
	Memo1->Lines->Add("OnPaint");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RowGrid1DrawRowCell(TObject *Sender,RowCell RowCell,
	TRect Rect) {
	Memo1->Lines->Add("OnDrawRowCell "+CellToString(RowCell));
	// no cursor (keyboard) support yet, only mouse support
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RowGrid1DblClickRowCell(TObject *Sender,
	  RowCell RowCell) {
	Memo1->Lines->Add("OnDblClickRowCell "+CellToString(RowCell));
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender) {
	RowGrid1->VerticalRows = 1 - RowGrid1->VerticalRows;
	int clientwidth = RowGrid1->ClientWidth;
	int clientheight = RowGrid1->ClientHeight;
	RowGrid1->ClientWidth = clientheight;
	RowGrid1->ClientHeight = clientwidth;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RowGrid1MouseMove(TObject *Sender,
	  TShiftState Shift, int X, int Y) {
	RowGrid1->ShowHint = false;
	if(RowGrid1->DragState == dsNone) {
		TCellState state; RowCell RowCell = RowGrid1->MouseToCell(X,Y,state);
		if(state != csNone && state != csEmpty) {
			RowGrid1->Hint = "CellStart: "+IntToStr(RowCell.Start);
			RowGrid1->ShowHint = true;
		}
	}
}
//---------------------------------------------------------------------------

