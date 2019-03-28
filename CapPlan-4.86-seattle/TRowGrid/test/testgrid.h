//$Id: testgrid.h,v 1.8 2008/09/12 14:57:02 darseq Exp $
#ifndef testgridH
#define testgridH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "RowGrid.h"
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TMemo *Memo1;
	TRowGrid *RowGrid1;
	TButton *Button1;
	void __fastcall RowGrid1DrawRowCell(TObject *Sender, RowCell RowCell,
		  TRect Rect);
	void __fastcall RowGrid1DblClickRowCell(TObject *Sender, RowCell RowCell);
	void __fastcall RowGrid1InsertRowCell(TObject *Sender, RowCell &RowCell,
		  bool &CanInsert);
	void __fastcall RowGrid1RemoveRowCell(TObject *Sender, RowCell &RowCell,
		  bool &CanRemove);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall RowGrid1MouseMove(TObject *Sender, TShiftState Shift,
		  int X, int Y);
	void __fastcall RowGrid1ClearRowCell(TObject *Sender, RowCell &RowCell,
          bool &CanClear);
	void __fastcall RowGrid1DragRowCell(TObject *Sender, RowCell &RowCell,
          TCellState CellState, bool IsCopy, bool &CanDrag);
	void __fastcall RowGrid1Paint(TObject *Sender);
private:	// User declarations
	AnsiString __fastcall CellToString(RowCell & RowCell);
	AnsiString __fastcall CellStateToString(TCellState CellState);
	AnsiString __fastcall DragStateToString(Rowgrid::TDragState state);
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
	__fastcall ~TForm1(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
