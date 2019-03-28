// $Id: RowGrid.h,v 1.40 2013/02/16 12:18:37 darseq Exp $
// copyright by PostWare (S.B.M. Post)
//---------------------------------------------------------------------------
#ifndef RowGridH
#define RowGridH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <math.hpp>
#ifdef _USE_OLD_RW_STL
#pragma message "To use the old stl, RowGrid.cpp must be recompiled!"
#endif
#include <vector>
//---------------------------------------------------------------------------

namespace Rowgrid {
// same namespace as Register
using namespace std;

class RowCell {
private:
	friend class TRowGrid;
	__fastcall RowCell(void) {}
	int FRow,FStart,FUnits;
public:
	__property int Row = {read = FRow};
	__property int Start = {read = FStart};
	__property int Units = {read = FUnits};
	int Zorder; void * Object;

	__fastcall RowCell(int Row,int Start,int Units,
		int Zorder = 0,void * Object = NULL) {
		if(Row < 0 || Units < 0) {
			throw Exception("RowGrid::RowCell, invalid Row/Units argument");
		}
		FRow = Row; FStart = Start; FUnits = Units;
		this->Zorder = Zorder; this->Object = Object;
	}
	bool __fastcall operator ==(const RowCell & RowCell) const {
		return Row == RowCell.Row &&
			Start == RowCell.Start &&
			Units == RowCell.Units &&
			Zorder == RowCell.Zorder &&
			Object == RowCell.Object;
	}
	bool __fastcall operator <(const RowCell & RowCell) const {
		if(Zorder < RowCell.Zorder) {return true;}
		else if(Zorder > RowCell.Zorder) {return false;}
		else if(Start < RowCell.Start) {return true;}
		else if(Start > RowCell.Start) {return false;}
		else {return Units < RowCell.Units;}
	}
};

typedef enum {dsDragDrop,dsCopyDrop,dsInsert,dsNone} TDragState;
typedef enum {csLeft,csCenter,csRight,csEmpty,csNone} TCellState;
typedef void __fastcall (__closure * TInsertRowCellEvent)
	(TObject* Sender,RowCell & RowCell,bool & CanInsert);
typedef void __fastcall (__closure * TRemoveRowCellEvent)
	(TObject* Sender,RowCell & RowCell,bool & CanRemove);
typedef void __fastcall (__closure * TClearRowCellEvent)
	(TObject* Sender,RowCell & RowCell,bool & CanClear);
typedef void __fastcall (__closure * TDragRowCellEvent)
	(TObject* Sender,RowCell & RowCell,TCellState CellState,
	bool IsCopy,bool & CanDrag);
// following events provide RowCell copies instead of references
typedef void __fastcall (__closure * TDrawRowLineEvent)
	(TObject* Sender,int Row,TRect Rect);
typedef void __fastcall (__closure * TDrawRowCellEvent)
	(TObject* Sender,RowCell RowCell,TRect Rect);
typedef void __fastcall (__closure * TDrawFocusRectEvent)
	(TObject* Sender,TRect Rect);
typedef void __fastcall (__closure * TDblClickRowCellEvent)
	(TObject* Sender,RowCell RowCell);

typedef vector<RowCell> row_t;
typedef vector<row_t> cells_t;
class PACKAGE TRowGrid : public TCustomControl {
private:
	enum {crArrowLeft=1,crArrowRight,crArrowTop,crArrowBottom,
		crArrowHoriz,crArrowVert};
// properties
	int FUnits;
	int FRowCount;
	int FRowHeight;
	int FRowLineWidth;
	int FUnitWidth;
	int FFrameWidth;
	int FCellFrameWidth;
	int FCellBorderWidth;
	bool FDefaultDrawing;
	bool FCheckPosition;
	bool FVerticalRows;
	bool FAltKeyInserts;
	bool FShiftKeyHolds;
	bool FShiftKeySwaps;
	int FMinVisible;
	TColor FRowLineColor;
	TColor FCellColor;
	TColor FTickColor;
	TDragState FDragState;
	TCellState FCellState;
	RowCell FDragCell;
	cells_t FCells;

// events
	TInsertRowCellEvent FOnInsertRowCell;
	TRemoveRowCellEvent FOnRemoveRowCell;
	TClearRowCellEvent FOnClearRowCell;
	TDragRowCellEvent FOnDragRowCell;
	TDrawRowLineEvent FOnDrawRowLine;
	TDrawRowCellEvent FOnDrawRowCell;
	TDrawFocusRectEvent FOnDrawFocusRect;
	TDblClickRowCellEvent FOnDblClickRowCell;
	TNotifyEvent FOnPaint;

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
		if(MinVisible > Units) {MinVisible = Units;}
	}
	int __fastcall GetRowCount(void) {
		bool restore_dragging = dragging;
		int cells_size = Cells.size();
		dragging = restore_dragging;
		return cells_size;
	}
	void __fastcall SetRowCount(int Value) {
		if(Value >= 0) {
			for(int r=Value;r < RowCount;r++) {
				internal_call = true;
				ClearRow(r);
			}
			// don't care about ClearRow can_clear, always resize
			Cells.resize(Value); FRowCount = Value; Invalidate();
			// do not restore dragging: Cells structure has changed
		}
	}
	void __fastcall SetRowHeight(int Value) {
		if(Value >= 0) {
			if(RowHeight) {
				int top = FrameWidth + RowLineWidth;
				int o_top = top + DragCell.Row * (RowLineWidth+RowHeight);
				int n_top = top + DragCell.Row * (RowLineWidth+Value);
				if(VerticalRows) {
					drag_x = RoundTo(
						n_top + (drag_x-o_top) * Value * 1.0/RowHeight,0);
				} else {
					drag_y = RoundTo(
						n_top + (drag_y-o_top) * Value * 1.0/RowHeight,0);
				}
			}
			FRowHeight = Value; Invalidate();
		}
	}
	void __fastcall	SetRowLineWidth(int Value) {
		if(Value >= 0) {
			int top = FrameWidth + RowLineWidth;
			int o_top = top + DragCell.Row * (RowLineWidth+RowHeight);
			int n_top = FrameWidth + Value + DragCell.Row * (Value+RowHeight);
			if(VerticalRows) {drag_x = n_top + (drag_x-o_top);}
			else {drag_y = n_top + (drag_y-o_top);}
			FRowLineWidth = Value; Invalidate();
		}
	}
	void __fastcall SetUnitWidth(int Value) {
		if(Value >= 1) {
			if(VerticalRows) {
				drag_y = RoundTo(drag_y * Value * 1.0/UnitWidth,0);
			} else {drag_x = RoundTo(drag_x * Value * 1.0/UnitWidth,0);}
			FUnitWidth = Value; Invalidate();
		}
	}
	void __fastcall SetFrameWidth(int Value) {
		if(Value >= 0) {FFrameWidth = Value; Invalidate();}
	}
	void __fastcall SetCellFrameWidth(int Value) {
		if(Value >= 0) {FCellFrameWidth = Value; Invalidate();}
	}
	void __fastcall SetCellBorderWidth(int Value) {
		if(Value >= 0) {FCellBorderWidth = Value;}
	}
	void __fastcall SetDefaultDrawing(bool Value) {
		FDefaultDrawing = Value; Invalidate();
	}
	void __fastcall SetVerticalRows(bool Value) {
		if(Value != VerticalRows) {
			swap(drag_x,drag_y);
			FVerticalRows = Value;
			if(DragState != dsNone) {
				if(DragState == dsInsert) {
					if(VerticalRows) {setCursor(crArrowVert);}
					else {setCursor(crArrowHoriz);}
				} else if(CellState == csLeft) {
					if(VerticalRows) {setCursor(crArrowBottom);}
					else {setCursor(crArrowRight);}
				} else if(CellState == csRight) {
					if(VerticalRows) {setCursor(crArrowTop);}
					else {setCursor(crArrowLeft);}
				}
			}
			Invalidate();
		}
	}
	void __fastcall SetMinVisible(int Value) {
		if(Value > Units) {Value = Units;}
		if(Value >= 0) {FMinVisible = Value;}
	}
	void __fastcall SetRowLineColor(TColor Value) {
		FRowLineColor = Value; Invalidate();
	}
	void __fastcall SetCellColor(TColor Value) {
		FCellColor = Value; Invalidate();
	}
	void __fastcall SetTickColor(TColor Value) {
		FTickColor = Value; Invalidate();
	}
	cells_t & __fastcall GetCells(void) {dragging = false; return FCells;}
	TCursor __fastcall GetCursor(void) {return TCustomControl::Cursor;}
	void __fastcall SetCursor(TCursor cursor) {
		setCursor(cursor); new_cursor = Cursor;
	}
	void __fastcall setCursor(int cursor) {
		TCustomControl::Cursor = (TCursor) cursor;
		TWinControl * active_form = this;
		while(active_form->Parent) {active_form = active_form->Parent;}
		if(Parent == NULL || Screen->ActiveForm == active_form) {
			::SetCursor(Screen->Cursors[Cursor]);
		}
	}

// internal
	bool dblclick;
	bool can_insert;
	bool can_remove;
	bool internal_call;

// drawing
	TRect old_rect;
	bool draw_twice;
	TCursor new_cursor;

// dragging
	bool dragging;
	bool drag_hold;
	bool chk_position;
	int drag_x,drag_y;
	TMouseButton drag_button;

	void __fastcall Paint(void);
	void __fastcall PaintRow(int Row); // calls doPaintCell/PaintRowLine
	void __fastcall doPaintCell(RowCell & row_cell,bool clear_only = false);
	void __fastcall PaintRowLine(int Row);
	void __fastcall PaintFrame(void);
	void __fastcall flipRect(TRect & rect);
	void __fastcall drawFocusRect(TRect & rect);
	void __fastcall updateCursor(void);
	bool __fastcall MouseToShift(int X,int Y,int & new_start);
	int __fastcall doMouseToShift(int X,int Y,int & new_start);
	bool __fastcall MouseToRow(int Y,int & row);
	bool __fastcall checkPosition(int X,int Y,int & new_start);
	void __fastcall insertRowCell(int row,int start,int units,
		int zorder,void * object);
	void __fastcall removeRowCell(RowCell & row_cell);
	void __fastcall dragRowCell(RowCell & row_cell,TDragState & new_state);
	void __fastcall doClearDragState(void);
	void __fastcall ClearRowCell(RowCell & row_cell,bool & can_clear) {
		if(OnClearRowCell) {OnClearRowCell(this,row_cell,can_clear);}
	}

	// call by using SendMessage(RowGrid->Handle,...). Ex:
	// int position = (point.y << 16) | (point.x & 0xffff);
	// SendMessage(RowGrid->Handle,WM_MOUSEMOVE,0,position);
	DYNAMIC void __fastcall DblClick(void);
	DYNAMIC void __fastcall MouseMove(TShiftState Shift,int X,int Y);
	DYNAMIC void __fastcall MouseUp(TMouseButton Button,TShiftState Shift,
		int X,int Y);
	DYNAMIC void __fastcall MouseDown(TMouseButton Button,TShiftState Shift,
		int X,int Y);
protected:
public:
	__property Canvas; // make Canvas public
	__property TDragState DragState = {read = FDragState, nodefault};
	// CellState/DragCell only defined when DragState == dsDragDrop/dsCopyDrop
	__property TCellState CellState = {read = FCellState, nodefault};
	__property RowCell DragCell = {read = FDragCell, write = FDragCell};
	// Do not allow for individual DragCell values to be changed (make updating
	// DragCell more difficult because this 'feature' should be used with care)
	__property cells_t Cells = {read = GetCells};

	__fastcall TRowGrid(TComponent* Owner);
	void __fastcall ClearRow(int Row);
	void __fastcall SortRow(int Row);
	void __fastcall InsertRowCell(RowCell & RowCell);
	void __fastcall RemoveRowCell(RowCell & RowCell);
	RowCell __fastcall MouseToCell(int X,int Y,TCellState & State);
	void __fastcall ToggleFocus(void) {drawFocusRect(old_rect);}
	void __fastcall PaintCell(RowCell & RowCell,bool ClearOnly = false) {
		if(DragState != dsNone) {ToggleFocus();} // clear focus
		doPaintCell(RowCell,ClearOnly);
		if(ClearOnly) {
			PaintRow(RowCell.Row);
			PaintFrame();
		} else {PaintRowLine(RowCell.Row);}
		if(DragState != dsNone) {ToggleFocus();} // restore focus
	}
	TRect __fastcall CellRect(RowCell & RowCell) {
		TRect rect;
		rect.Left = FrameWidth + RowCell.Start*UnitWidth;
		rect.Right = rect.Left + RowCell.Units*UnitWidth;
		rect.Top = FrameWidth + RowLineWidth +
			RowCell.Row*(RowLineWidth+RowHeight);
		rect.Bottom = rect.Top + RowHeight;
		if(VerticalRows) {flipRect(rect);}
		return rect;
	}
	TRect __fastcall RowLineRect(int Row) {
		TRect rect;
		rect.Left = FrameWidth;
		rect.Right = rect.Left + Units*UnitWidth;
		rect.Top = FrameWidth + Row*(RowLineWidth+RowHeight);
		rect.Bottom = rect.Top + RowLineWidth;
		if(VerticalRows) {flipRect(rect);}
		return rect;
	}
	void __fastcall ClearDragState(void) {
		if(DragState != dsNone) {
			ToggleFocus(); // clear focus
			if(DragState == dsDragDrop || DragState == dsCopyDrop) {
				doPaintCell(DragCell,true);
				PaintRow(DragCell.Row); PaintFrame();
				bool can_clear = true; ClearRowCell(DragCell,can_clear);
				// Because the RowGrid can be used as an ActiveX component,
				// we let the application itself delete DragCell.Object.
			}
		}
		doClearDragState();
	}
__published:
// new properties
	__property int ClientWidth = {
		read = GetClientWidth, write = SetClientWidth, stored=false, nodefault
	};
	__property int ClientHeight = {
		read = GetClientHeight, write = SetClientHeight, stored=false, nodefault
	};
	__property int Units = {
		read = FUnits, write = SetUnits, nodefault
	};
	__property int RowCount = {
		read = GetRowCount, write = SetRowCount, nodefault
	};
	__property int RowHeight = {
		read = FRowHeight, write = SetRowHeight, nodefault
	};
	__property int RowLineWidth = {
		read = FRowLineWidth, write = SetRowLineWidth, nodefault
	};
	__property int UnitWidth = {
		read = FUnitWidth, write = SetUnitWidth, nodefault
	};
	__property int FrameWidth = {
		read = FFrameWidth, write = SetFrameWidth, default = 2
	};
	__property int CellFrameWidth = {
		read = FCellFrameWidth, write = SetCellFrameWidth, default = 1
	};
	__property int CellBorderWidth = {
		read = FCellBorderWidth, write = SetCellBorderWidth, default = 8
	};
	__property bool DefaultDrawing = {
		read = FDefaultDrawing, write = SetDefaultDrawing, default = true
	};
	__property bool CheckPosition = {
		read = FCheckPosition, write = FCheckPosition, default = true
	};
	__property bool VerticalRows = {
		read = FVerticalRows, write = SetVerticalRows, default = false
	};
	__property bool AltKeyInserts = {
		read = FAltKeyInserts, write = FAltKeyInserts, default = false
	};
	__property bool ShiftKeyHolds = {
		read = FShiftKeyHolds, write = FShiftKeyHolds, default = true
	};
	__property bool ShiftKeySwaps = {
		read = FShiftKeySwaps, write = FShiftKeySwaps, default = true
	};
	__property int MinVisible = {
		read = FMinVisible, write = SetMinVisible, default = 1
	};
	__property TColor RowLineColor = {
		read = FRowLineColor, write = SetRowLineColor, default = clSkyBlue
	};
	__property TColor CellColor = {
		read = FCellColor, write = SetCellColor, default = clBackground
	};
	__property TColor TickColor = {
		read = FTickColor, write = SetTickColor, default = clBlack
	};

// new events
	__property TInsertRowCellEvent OnInsertRowCell = {
		read = FOnInsertRowCell, write = FOnInsertRowCell
	};
	__property TRemoveRowCellEvent OnRemoveRowCell = {
		read = FOnRemoveRowCell, write = FOnRemoveRowCell
	};
	__property TClearRowCellEvent OnClearRowCell = {
		read = FOnClearRowCell, write = FOnClearRowCell
	};
	__property TDragRowCellEvent OnDragRowCell = {
		read = FOnDragRowCell, write = FOnDragRowCell
	};
	__property TDrawRowLineEvent OnDrawRowLine = {
		read = FOnDrawRowLine, write = FOnDrawRowLine
	};
	__property TDrawRowCellEvent OnDrawRowCell = {
		read = FOnDrawRowCell, write = FOnDrawRowCell
	};
	__property TDrawFocusRectEvent OnDrawFocusRect = {
		read = FOnDrawFocusRect, write = FOnDrawFocusRect
	};
	__property TDblClickRowCellEvent OnDblClickRowCell = {
		read = FOnDblClickRowCell, write = FOnDblClickRowCell
	};
	__property TNotifyEvent OnPaint = {
		read = FOnPaint, write = FOnPaint
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
	__property Cursor = {read = GetCursor, write = SetCursor};
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

#endif

