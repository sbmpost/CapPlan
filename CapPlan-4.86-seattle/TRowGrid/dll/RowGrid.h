// $Id: RowGrid.h,v 1.1 2005/05/08 22:13:05 darseq Exp $
//---------------------------------------------------------------------------
#ifndef RowGridH
#define RowGridH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include "vector.h"
//---------------------------------------------------------------------------

namespace Rowgrid {
// same namespace as Register

class RowCell {
friend class TRowGrid;
private:
	int FRow;
	int FStart;
	int FUnits;
	int FZorder;
public:
	bool __fastcall operator ==(const RowCell & RowCell) const {
		return RowCell.Row == Row &&
			RowCell.Start == Start &&
			RowCell.Units == Units &&
			RowCell.Zorder == Zorder;
	}

	bool __fastcall operator <(const RowCell & RowCell) const {
		return Zorder < RowCell.Zorder;
	}

	void __fastcall SetZorder(int Value) {if(Value >= 0) {FZorder = Value;}}

	__property int Row = {read = FRow};
	__property int Start = {read = FStart};
	__property int Units = {read = FUnits};
	__property int Zorder = {read = FZorder,write = SetZorder};
	void * object;
};

typedef enum {dsDragDrop,dsCopyDrop,dsInsert,dsNone} TDragState;
//typedef enum {dsDrag,dsDragHold,dsInsert,dsNone} TDragState;
typedef enum {csLeft,csCenter,csRight,csEmpty,csNone} TCellState;
typedef void __fastcall (__closure * TInsertRowCellEvent)
	(TObject* Sender,RowCell & RowCell,bool & CanInsert);
typedef void __fastcall (__closure * TRemoveRowCellEvent)
	(TObject* Sender,RowCell & RowCell,bool & CanRemove);
// following events provide RowCell copies instead of references
typedef void __fastcall (__closure * TDrawRowCellEvent)
	(TObject* Sender,RowCell RowCell,TRect Rect,bool Cursor);
typedef void __fastcall (__closure * TDragRowCellEvent)
	(TObject* Sender,RowCell RowCell,TCellState CellState,
	bool IsCopy,bool & CanDrag);
typedef void __fastcall (__closure * TMouseOverRowCellEvent)
	(TObject* Sender,RowCell RowCell);
typedef void __fastcall (__closure * TDblClickRowCellEvent)
	(TObject* Sender,RowCell RowCell);

typedef vector<RowCell> row_t;
typedef vector<row_t> cells_t;
class PACKAGE TRowGrid : public TCustomControl {
private:
	enum {crArrowLeft=1,crArrowRight,crArrowSplit};
// properties
	int FUnits;
	int FTicks;
	int FRowCount;
	int FRowHeight;
	int FRowLineWidth;
	int FUnitWidth;
	int FFrameWidth;
	int FCursorWidth;
	int FCellFrameWidth;
	int FCellBorderWidth;
	bool FDefaultDrawing;
	bool FCheckPosition;
	bool FCrossBorders;
	int FMinVisible;
	TColor FRowLineColor;
	TColor FCellColor;
	TDragState FDragState;
	cells_t FCells;

// events
	TInsertRowCellEvent FOnInsertRowCell;
	TRemoveRowCellEvent FOnRemoveRowCell;
	TDrawRowCellEvent FOnDrawRowCell;
	TDragRowCellEvent FOnDragRowCell;
	TMouseOverRowCellEvent FOnMouseOverRowCell;
	TDblClickRowCellEvent FOnDblClickRowCell;

	int __fastcall GetClientWidth(void) {
		return Width - 2*FrameWidth;
	}
	void __fastcall SetClientWidth(int Value) {
		if(Value >= 0) {Width = Value + 2*FrameWidth;}
	}
	int __fastcall GetClientHeight(void) {
		return Height - 2*FrameWidth;
	}
	void __fastcall SetClientHeight(int Value) {
		if(Value >= 0) {Height = Value + 2*FrameWidth;}
	}
	void __fastcall SetUnits(int Value) {
		if(Value >= 1) {FUnits = Value; Invalidate();}
	}
	void __fastcall SetTicks(int Value) {
		if(Value >= 1) {FTicks = Value; Invalidate();}
	}
	int __fastcall GetRowCount(void) {return Cells.size();}
	void __fastcall SetRowCount(int Value) {
		if(Value >= 1) {FRowCount = Value; Cells.resize(Value); Invalidate();}
	}
	void __fastcall SetRowHeight(int Value) {
		if(Value >= 0) {FRowHeight = Value; Invalidate();}
	}
	void __fastcall	SetRowLineWidth(int Value) {
		if(Value >= 0) {FRowLineWidth = Value; Invalidate();}
	}
	void __fastcall SetUnitWidth(int Value) {
		if(Value >= 1) {
			if(UnitWidth) {drag_x = drag_x * (double) Value/UnitWidth;}
			FUnitWidth = Value; Invalidate();
		}
	}
	void __fastcall SetFrameWidth(int Value) {
		if(Value >= 0) {FFrameWidth = Value; Invalidate();}
	};
	void __fastcall SetCursorWidth(int Value) {
		if(Value >= 0) {FCursorWidth = Value; Invalidate();}
	};
	void __fastcall SetCellFrameWidth(int Value) {
		if(Value >= 0) {FCellFrameWidth = Value; Invalidate();}
	};
	void __fastcall SetRowLineColor(TColor Value) {
		FRowLineColor = Value; Invalidate();
	}
	void __fastcall SetCellColor(TColor Value) {
		FCellColor = Value; Invalidate();
	}
	cells_t & __fastcall GetCells(void) {dragging = false; return FCells;}

// internal
	bool dblclick;
	bool can_insert;
	bool can_remove;
	bool internal_call;
//	RowCell prev_cell;

// drawing
	TRect old_rect;
	bool draw_twice;
//	int cursor_col,cursor_row;

// dragging
	bool dragging;
	bool drag_hold;
	int drag_x,drag_y;
	RowCell drag_cell;
	TCellState cell_state;
	TMouseButton drag_button;

	void __fastcall Paint(void);
	void __fastcall PaintRow(TRect & rect);
	void __fastcall PaintCell(RowCell & row_cell);
	double __fastcall getRowHeight(void);
	void __fastcall drawFocusRect(TRect & rect);
	bool __fastcall MouseToShift(int X,int & old_start,int & new_start);
	bool __fastcall checkPosition(int X,int & old_start,int & new_start);
	void __fastcall insertRowCell(int row,int start,int units,int zorder);
	void __fastcall removeRowCell(RowCell & row_cell);
	TCellState __fastcall moveMouse(int X,int Y);
	void __fastcall dragRowCell(TDragState & new_state);
protected:
public:
	__property Canvas; // make Canvas public
	__property TDragState DragState = {read = FDragState, nodefault};
	__property cells_t Cells = {read = GetCells};

	__fastcall TRowGrid(TComponent* Owner);
	void __fastcall InsertRowCell(int Row,int Start,int Units,int Zorder = 0,
		void * Object = NULL);
	void __fastcall RemoveRowCell(RowCell & RowCell);
	RowCell __fastcall MouseToCell(int X,int Y,TCellState & State);
	void __fastcall ClearDragState(void);

	// Provide some flexibility: allow 'external' calls to these
	// functions or allow them to be reimplemented. To implement
	// event handlers, use the OnEvent versions like: OnClick
	DYNAMIC void __fastcall DblClick(void);
	DYNAMIC void __fastcall MouseMove(TShiftState Shift,int X,int Y);
	DYNAMIC void __fastcall MouseUp(TMouseButton Button,TShiftState Shift,
		int X,int Y);
	DYNAMIC void __fastcall MouseDown(TMouseButton Button,TShiftState Shift,
		int X,int Y);
	// DYNAMIC void __fastcall KeyDown(Word &Key,TShiftState Shift);
__published:
// new properties
	__property int ClientWidth = {
		read = GetClientWidth, write = SetClientWidth, nodefault
	};
	__property int ClientHeight = {
		read = GetClientHeight, write = SetClientHeight, nodefault
	};
	__property int Units = {
		read = FUnits, write = SetUnits, default = 36
	};
	__property int Ticks = {
		read = FTicks, write = SetTicks, default = 4
	};
	__property int RowCount = {
		read = GetRowCount, write = SetRowCount, default = 10
	};
	__property int RowHeight = {
		read = FRowHeight, write = SetRowHeight, default = 20
	};
	__property int RowLineWidth = {
		read = FRowLineWidth, write = SetRowLineWidth, default = 10
	};
	__property int UnitWidth = {
		read = FUnitWidth, write = SetUnitWidth, default = 15
	};
	__property int FrameWidth = {
		read = FFrameWidth, write = SetFrameWidth, default = 2
	};
//	__property int CursorWidth = {
//		read = FCursorWidth, write = SetCursorWidth, default = 3
//	};
	__property int CellFrameWidth = {
		read = FCellFrameWidth, write = SetCellFrameWidth, default = 1
	};
	__property int CellBorderWidth = {
		read = FCellBorderWidth, write = FCellBorderWidth, default = 8
	};
	__property bool DefaultDrawing = {
		read = FDefaultDrawing, write = FDefaultDrawing, default = true
	};
	__property bool CheckPosition = {
		read = FCheckPosition, write = FCheckPosition, default = true
	};
	__property bool CrossBorders = {
		read = FCrossBorders, write = FCrossBorders, default = true
	};
	__property int MinVisible = {
		read = FMinVisible, write = FMinVisible, default = 1
	};
	__property TColor RowLineColor = {
		read = FRowLineColor, write = SetRowLineColor, default = clSkyBlue
	};
	__property TColor CellColor = {
		read = FCellColor, write = SetCellColor, default = clBackground
	};

// new events
	__property TInsertRowCellEvent OnInsertRowCell = {
		read = FOnInsertRowCell, write = FOnInsertRowCell
	};
	__property TRemoveRowCellEvent OnRemoveRowCell = {
		read = FOnRemoveRowCell, write = FOnRemoveRowCell
	};
	__property TDrawRowCellEvent OnDrawRowCell = {
		read = FOnDrawRowCell, write = FOnDrawRowCell
	};
	__property TDragRowCellEvent OnDragRowCell = {
		read = FOnDragRowCell, write = FOnDragRowCell
	};
	__property TMouseOverRowCellEvent OnMouseOverRowCell = {
		read = FOnMouseOverRowCell, write = FOnMouseOverRowCell
	};
	__property TDblClickRowCellEvent OnDblClickRowCell = {
		read = FOnDblClickRowCell, write = FOnDblClickRowCell
	};

// default properties (used TDrawGrid as a reference)
// TWinControl:
	__property Ctl3D;
	__property ParentCtl3D;
	__property TabOrder;
	__property TabStop;

// TControl:
	__property Align;
	__property Anchors;
	__property BiDiMode;
	__property Color;
	__property Cursor;
	__property DragCursor;
	__property DragKind;
	__property DragMode;
	__property Enabled;
	__property Font;
	__property Height;
	__property HelpContext;
	__property HelpKeyword;
	__property HelpType;
	__property Hint;
	__property Left;
	__property Name;
	__property ParentBiDiMode;
	__property ParentColor;
	__property ParentFont;
	__property ParentShowHint;
	__property PopupMenu;
	__property ShowHint;
	__property Top;
	__property Visible;
	__property Width;

// TComponent:
	__property Tag;

// default events (publish all available events)
// TWinControl:
	__property OnDockDrop;
	__property OnDockOver;
	__property OnEnter;
	__property OnExit;
	__property OnGetSiteInfo;
	__property OnKeyDown;
	__property OnKeyPress;
	__property OnKeyUp;
	__property OnUnDock;

// TControl:
	__property OnCanResize;
	__property OnClick;
	__property OnConstrainedResize;
	__property OnContextPopup;
	__property OnDblClick;
	__property OnDragDrop;
	__property OnDragOver;
	__property OnEndDock;
	__property OnEndDrag;
	__property OnMouseDown;
	__property OnMouseMove;
	__property OnMouseUp;
	__property OnMouseWheel;
	__property OnMouseWheelDown;
	__property OnMouseWheelUp;
	__property OnResize;
	__property OnStartDock;
	__property OnStartDrag;
};
//---------------------------------------------------------------------------
} // end namespace Rowgrid
#if !defined(NO_IMPLICIT_NAMESPACE_USE)
using namespace Rowgrid;
#endif
	extern "C" __declspec(dllexport) void DrawGrid(TForm * form) {
		TRowGrid * rowgrid = new TRowGrid(form);
		rowgrid->Parent = form;
	}
#endif

