// $Id: RowGrid.cpp,v 1.56 2013/12/09 23:29:55 darseq Exp $
// copyright by PostWare (S.B.M. Post)
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#pragma resource "*.res"

#include <algorithm>
#include "RowGrid.h"
#ifdef _DEBUG
#include "Exceptions.h"
#define PTRSIZE 8+1
// +1: '\0' character
#endif
#define MOVEDELTA 4
// when dragging cells
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
static inline void ValidCtrCheck(TRowGrid *) {
	new TRowGrid(NULL);
}
//---------------------------------------------------------------------------

namespace Rowgrid {
	void __fastcall PACKAGE Register() {
		TComponentClass classes[1] = {__classid(TRowGrid)};
		RegisterComponents("PostWare", classes, 0);
	}
}
//---------------------------------------------------------------------------

// todo: RowGrid: add scroll support. i.e when Units*UnitWidth > ClientWidth
__fastcall TRowGrid::TRowGrid(TComponent* Owner) : TCustomControl(Owner) {
	internal_call = dragging = false;
	Caption = "Copyright 2004-2010 by PostWare (S.B.M. Post), "
		"01234567890123456789012345678901"; // 32 characters
	FDragState = dsNone;
	FCellState = csNone;

	FUnitWidth = 15; // use FUnitWidth
	Units = 36; RowCount = 10; RowHeight = 20;
	RowLineWidth = 10; CellBorderWidth = 8;
	FrameWidth = 2; CellFrameWidth = 1;
	Width = Units*UnitWidth + 2*FrameWidth;
	Height = (RowCount+1)*RowLineWidth +
		RowCount*RowHeight + 2*FrameWidth;

	DoubleBuffered = true;
	DefaultDrawing = true;
	CheckPosition = true;
	VerticalRows = false;
	AltKeyInserts = false;
	ShiftKeyHolds = true;
	ShiftKeySwaps = true;
	MinVisible = 1;
	RowLineColor = clSkyBlue;
	CellColor = clBackground;
	TickColor = clBlack;
	OnInsertRowCell = NULL;
	OnRemoveRowCell = NULL;
	OnClearRowCell = NULL;
	OnDragRowCell = NULL;
	OnDrawRowLine = NULL;
	OnDrawRowCell = NULL;
	OnDrawFocusRect = NULL;
	OnDblClickRowCell = NULL;
	OnPaint = NULL;

	// what if multiple TRowGrid instances?
	Screen->Cursors[crArrowLeft] = LoadCursor(HInstance,"ArrowLeft");
	Screen->Cursors[crArrowRight] = LoadCursor(HInstance,"ArrowRight");
	Screen->Cursors[crArrowTop] = LoadCursor(HInstance,"ArrowTop");
	Screen->Cursors[crArrowBottom] = LoadCursor(HInstance,"ArrowBottom");
	Screen->Cursors[crArrowHoriz] = LoadCursor(HInstance,"ArrowHoriz");
	Screen->Cursors[crArrowVert] = LoadCursor(HInstance,"ArrowVert");
	new_cursor = Cursor;
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::SortRow(int Row) {
	if(Row < 0 || Row >= RowCount) {
		throw Exception("RowGrid::SortRow, row index out of bounds");
	}
	bool restore_dragging = dragging;
	// cell positions remain
	row_t & row = Cells[Row];
	sort(row.begin(),row.end());
	dragging = restore_dragging;
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::ClearRow(int Row) {
	if(Row < 0 || Row >= RowCount) {
		throw Exception("RowGrid::ClearRow, row index out of bounds");
	}
	row_t & row = Cells[Row];
	row_t::iterator cell = row.begin();
	bool can_clear_value = internal_call;
	internal_call = false; // reset before ClearRowCell
	// set can_clear to false to indicate that the event
	// was caused by the application calling ClearRow().
	// when RowCount was set, can_clear is true and ignored.
	while(cell != row.end()) {
		bool can_clear = can_clear_value;
		ClearRowCell(*cell,can_clear);
		if(can_clear) {cell = row.erase(cell);}
		else {cell++;}
	}
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::insertRowCell(int row,int start,int units,
	int zorder,void * object) {
	RowCell row_cell(row,start,units,zorder,object);
	internal_call = true; InsertRowCell(row_cell);
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::removeRowCell(RowCell & row_cell) {
	internal_call = true; RemoveRowCell(row_cell);
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::InsertRowCell(RowCell & RowCell) {
	if(RowCell.Row >= RowCount) {
		throw Exception("RowGrid::InsertRowCell, row index out of bounds");
	}
	can_insert = true;
	bool sort_row = false;
	if(internal_call) {
		internal_call = false; sort_row = true;
		// clear internal_call as soon as possible:
		// suppose exception in OnInsertRowCell event
#ifdef _DEBUG
		AnsiString message("OnInsertRowCell");
		message += " row:"+IntToStr(RowCell.Row);
		message += " start:"+IntToStr(RowCell.Start);
		message += " units:"+IntToStr(RowCell.Units);
		message += " zorder:"+IntToStr(RowCell.Zorder);
		char string[PTRSIZE];
		snprintf(string,PTRSIZE,"%x",RowCell.Object);
		message += " object:"+String(string);
		Exceptions::writeLog(message);
#endif
		if(OnInsertRowCell) {OnInsertRowCell(this,RowCell,can_insert);}
	}

	// After OnInsertRowCell: The cell may have been
	// supplied with an object or can_insert was cleared
	if(can_insert) {
		Cells[RowCell.Row].push_back(RowCell);
		if(sort_row) {SortRow(RowCell.Row);}
	}
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::RemoveRowCell(RowCell & RowCell) {
	if(RowCell.Row >= RowCount) {
		throw Exception("RowGrid::RemoveRowCell, row index out of bounds");
	}
	row_t::iterator cell = Cells[RowCell.Row].begin();
	row_t::iterator end = Cells[RowCell.Row].end();
	for(;cell != end;cell++) {if(*cell == RowCell) {break;}}
	// break: when identical cells, delete the first one encountered

	if(cell == end) {
		throw Exception("RowGrid::RemoveRowCell, cell not present");
	}

	can_remove = true;
	if(internal_call) {
		internal_call = false;
		// clear internal_call as soon as possible:
		// suppose exception in OnRemoveRowCell event
#ifdef _DEBUG
		AnsiString message("OnRemoveRowCell");
		message += " row:"+IntToStr(cell->Row);
		message += " start:"+IntToStr(cell->Start);
		message += " units:"+IntToStr(cell->Units);
		message += " zorder:"+IntToStr(cell->Zorder);
		char string[PTRSIZE];
		snprintf(string,PTRSIZE,"%x",cell->Object);
		message += " object:"+String(string);
		Exceptions::writeLog(message);
#endif
		// User is responsible for releasing cell->object. When
		// internal_call this should be done in OnRemoveRowCell.
		if(OnRemoveRowCell) {OnRemoveRowCell(this,RowCell,can_remove);}
	}

	// After OnRemoveRowCell: The cell object must
	// first be deleted or can_remove was cleared.
	if(can_remove) {Cells[RowCell.Row].erase(cell);}
	// no need to SortRow() when removing RowCells
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::Paint(void) {
#ifdef _DEBUG
	// MessageBeep(-1);
	AnsiString message("Paint");
	Exceptions::writeLog(message);
#endif
	if(OnPaint) {OnPaint(this);}
	for(int r=0;r <= RowCount;r++) {
		if(r != RowCount) {PaintRow(r);}
		else {PaintRowLine(r);}
	}
	PaintFrame(); // toggle after PaintFrame
	if(DragState != dsNone) {ToggleFocus();} // show focus
	else {updateCursor();} // when the rowgrid was updated by app
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::PaintRow(int Row) {
	// restore: only reading from Cells
	bool restore_dragging = dragging;
	row_t & row = Cells[Row];
	row_t::iterator cell = row.begin();
	for(;cell != row.end();cell++) {doPaintCell(*cell);}
	dragging = restore_dragging;
	PaintRowLine(Row);
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::doPaintCell(RowCell & row_cell,bool clear) {
	TRect rect = CellRect(row_cell);
	if(VerticalRows) {
		if(rect.Top >= FrameWidth + ClientHeight ||
			rect.Bottom <= FrameWidth) {return;}
	} else if(rect.Left >= FrameWidth + ClientWidth ||
			rect.Right <= FrameWidth) {return;}
	if(clear) {
		Canvas->Brush->Color = Color;
		Canvas->FillRect(rect); return;
	}
	if(DefaultDrawing) {
		Frame3D(Canvas,rect,clBlack,clBlack,CellFrameWidth);
		Canvas->Brush->Color = CellColor;
		Canvas->FillRect(rect);
	}
	if(OnDrawRowCell) {OnDrawRowCell(this,row_cell,rect);}
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::PaintRowLine(int Row) {
	TRect rect = RowLineRect(Row);
	if(DefaultDrawing) {
		Canvas->Brush->Color = RowLineColor;
		Canvas->FillRect(rect);
		Canvas->Pen->Color = TickColor;
		int middle2 = RowLineWidth/2;
		int middle4 = RowLineWidth/4;
		if(VerticalRows) {
			Canvas->MoveTo(rect.Left + middle2,FrameWidth);
			Canvas->LineTo(rect.Left + middle2,FrameWidth + Units*UnitWidth);
		} else {
			Canvas->MoveTo(FrameWidth,rect.Top + middle2);
			Canvas->LineTo(FrameWidth + Units*UnitWidth,rect.Top + middle2);
		}
		for(int u=1;u != Units;u++) {
			int x = FrameWidth + u*UnitWidth;
			if(VerticalRows) {
				Canvas->MoveTo(rect.Left+middle4,x);
				Canvas->LineTo(rect.Right-middle4,x);
			} else {
				Canvas->MoveTo(x,rect.Top+middle4);
				Canvas->LineTo(x,rect.Bottom-middle4);
			}
		}
	}
	if(OnDrawRowLine) {OnDrawRowLine(this,Row,rect);}
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::PaintFrame(void) {
	TRect frame(0,0,Width,Height);
	Frame3D(Canvas,frame,clBlack,clBlack,FrameWidth);
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::flipRect(TRect & rect) {
	int left = rect.Left; int right = rect.Right;
	int top = rect.Top; int bottom = rect.Bottom;
	rect.Left = top; rect.Right = bottom;
	rect.Top = left; rect.Bottom = right;
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::drawFocusRect(TRect & rect) {
	Canvas->Brush->Color = clWhite; // where no dots
	Canvas->Pen->Mode = pmNotXor;
	Canvas->Pen->Style = psDot;
	if(DefaultDrawing) {
		TRect tmp_rect = rect; // tmp_rect changes
		Frame3D(Canvas,tmp_rect,clBlack,clBlack,1);
	}
	old_rect = rect; draw_twice = true;
	if(OnDrawFocusRect) {OnDrawFocusRect(this,rect);}
	Canvas->Pen->Mode = pmCopy;
	Canvas->Pen->Style = psSolid;
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::updateCursor(void) {
	POINT m_point;
	if(GetCursorPos(&m_point) == FALSE) {return;}
	TPoint mouse_point = ScreenToClient((const TPoint &) m_point);
	TCellState cursor_state;
	MouseToCell(mouse_point.x,mouse_point.y,cursor_state);
	// [future: by calling MouseToCell for each move we might just
	// as well store the movement information for checkPosition
	// note that the mouse coordinates in MouseUp need not equal
	// the mouse position within move (is this true?) Also note
	// that updateCursor is called from Paint() as well.]
	setCursor(new_cursor);
	if(cursor_state == csLeft) {
		if(VerticalRows) {setCursor(crArrowBottom);}
		else {setCursor(crArrowRight);}
	} else if(cursor_state == csRight) {
		if(VerticalRows) {setCursor(crArrowTop);}
		else {setCursor(crArrowLeft);}
	}
}
//---------------------------------------------------------------------------

RowCell __fastcall TRowGrid::MouseToCell(int X,int Y,TCellState & State) {
	if(VerticalRows) {swap(X,Y);}

	State = csNone;
	RowCell row_cell(0,0,0,0,NULL);
	if(MouseToRow(Y,row_cell.FRow)) {
		bool search = true;
		bool restore_dragging = dragging;
		row_t row = Cells[row_cell.Row];
		row_t::const_reverse_iterator cell = row.rbegin();
		row_t::const_reverse_iterator end = row.rend();
		for(;search && cell != end;cell++) {
			int left = FrameWidth + cell->Start*UnitWidth;
			int right = left + cell->Units*UnitWidth;
			// <=, top & left included
			if(left <= X && X < right) {
				search = false; State = csCenter;
				if(X-left < CellBorderWidth) {State = csLeft;}
				if(right-1-X < CellBorderWidth) {State = csRight;}
				row_cell = *cell;
			}
		}
		if(search) {
			State = csEmpty;
			row_cell.FStart = RoundTo((X - FrameWidth) * 1.0/UnitWidth,0);
		}
		dragging = restore_dragging;
	}

/*
#ifdef _DEBUG
	AnsiString message("MouseToCell");
	message += " X:"+IntToStr(X);
	message += " Y:"+IntToStr(Y);
	message += " row:"+IntToStr(row_cell.Row);
	message += " start:"+IntToStr(row_cell.Start);
	message += " units:"+IntToStr(row_cell.Units);
	message += " zorder:"+IntToStr(row_cell.Zorder);
	message += " state:"+IntToStr(State);

	char string[PTRSIZE];
	snprintf(string,PTRSIZE,"%x",row_cell.Object);
	message += " object:"+String(string);
	Exceptions::writeLog(message);
#endif
*/

	return row_cell;
}
//---------------------------------------------------------------------------

bool __fastcall TRowGrid::MouseToShift(int X,int Y,int & new_start) {
	int check_x = doMouseToShift(X,Y,new_start);
	if(VerticalRows) {Y = check_x;} else {X = check_x;}
	bool cell_fits = checkPosition(X,Y,new_start);

#ifdef _DEBUG
	AnsiString message("MouseToShift");
	message += " old_start:"+IntToStr(DragCell.Start);
	message += " new_start:"+IntToStr(new_start);
	message += " DragState:"+IntToStr(DragState);
	message += " cell_fits:"+IntToStr((int) cell_fits);
	Exceptions::writeLog(message);
#endif

	return cell_fits;
}
//---------------------------------------------------------------------------

int __fastcall TRowGrid::doMouseToShift(int X,int Y,int & new_start) {
	int drag_pos = drag_x; if(VerticalRows) {X = Y; drag_pos = drag_y;}
	// determine the new position (new_start) of the cell being dragged
	// determine the old position (old_start) of the cell being dragged
	// new_start - old_start: howmuch the mouse has shifted in units
	//     csCenter          csCenter          csRight           csLeft
	//       dx--->X        X<---dx              dx--->X        X<---dx
	//  o_____|_   |        |  o_|_____     o_____|_ __|_      _|__o__|____
	//  |--->n__|__|_     n_|__|__     |    |--->n__|____|   n|____|__     |
	//  |____|__|    |    |<---|__|____|    |____|__|____|    |<---|__|____|
	//       |_______|    |_______|              |_______|    |_______|
	// ---------------------------------------------------------------------
	int check_x = X;
	if(DragState != dsInsert) {
		int left = FrameWidth + DragCell.Start*UnitWidth;
		X = X - drag_pos + left;
	}

	new_start = RoundTo((X - FrameWidth) * 1.0/UnitWidth,0);
	if(DragState == dsInsert) {
		if(new_start < DragCell.Start) {FCellState = csLeft;}
		else {FCellState = csRight;}
	}

	// prevent dragging one cellborder over the other
	if(DragState != dsInsert) {
		if(CellState == csLeft) {
			if(new_start > DragCell.Start+DragCell.Units-1) {
				new_start = DragCell.Start+DragCell.Units-1;
			}
		} else if(CellState == csRight) {
			if(new_start < DragCell.Start-DragCell.Units+1) {
				new_start = DragCell.Start-DragCell.Units+1;
			}
		}
	}

	// check_x is always bounded by DragCell
	if(CellState == csCenter) {
		int left = FrameWidth + new_start*UnitWidth;
		int right = left + DragCell.Units*UnitWidth;
		if(check_x >= right) {check_x = right-1;}
		if(check_x < left) {check_x = left;}
	} else {check_x = drag_pos;}
	return check_x;
}
//---------------------------------------------------------------------------

bool __fastcall TRowGrid::MouseToRow(int Y,int & row) {
	int row_height = RowLineWidth+RowHeight;
	if(row_height) {row = (Y - FrameWidth)/row_height;}
	int row_space = Y - FrameWidth - row*row_height;
	int last_row = FrameWidth + RowCount*row_height;
	return RowLineWidth <= row_space && Y < last_row;
}
//---------------------------------------------------------------------------

bool __fastcall TRowGrid::checkPosition(int X,int Y,int & new_start) {
	if(VerticalRows) {swap(X,Y);}

	// find markers m1 and m2 around X
	int m1 = 0; int m2 = Units; // rowgrid borders
	row_t::const_iterator min; bool min_set = false;
	row_t::const_iterator max; bool max_set = false;
	int row = DragCell.Row; bool cs_empty = true;
	if(CellState == csCenter) {cs_empty = MouseToRow(Y,row);}
	// the new position (new_start) may not be valid. If CheckPosition is true,
	// we check for overlap with other cells and change new_start if necessary
	if(cs_empty && chk_position) { // when cs_empty, the mouse is within a row
		// m1: maximum right before X (min_r)
		// m2: minimum left after X (max_l)
		row_t::const_iterator cell = Cells[row].begin();
		row_t::const_iterator end = Cells[row].end();
		for(;cell != end;cell++) {
			int min_r; int max_l; // |l min r| <= X < |l max r|
			int left = FrameWidth + cell->Start*UnitWidth;
			int right = left + cell->Units*UnitWidth;
			bool greater = false; bool smaller = false;
			if(right <= X) {
				if(min_set == false) {min_set = true;min = cell;min_r = right;}
				else if(min_r < right) {min = cell; min_r = right;}
			} else {smaller = true;} // X < right
			if(left > X) {
				if(max_set == false) {max_set = true;max = cell;max_l = left;}
				else if(max_l > left) {max = cell; max_l = left;}
			} else {greater = true;} // X >= left
			if(greater && smaller) {cs_empty = false;}
		}
	}
	if(min_set) {m1 = min->Start + min->Units;}
	if(max_set) {m2 = max->Start;}

#ifdef _DEBUG
	AnsiString message("checkPosition");
	message += " X:"+IntToStr(X);
	message += " m1:"+IntToStr(m1);
	message += " m2:"+IntToStr(m2);
	Exceptions::writeLog(message);
#endif

	// check if new_start fits between m1 and m2
	int units = 0; if(DragState != dsInsert) {units = DragCell.Units;}
	int lim = MinVisible; if(lim > units) {lim = units;}
	bool cell_fits = true; m2 = m2-units;
	if(CellState == csLeft) { // dsInsert/Resize
		if(DragCell.Start+units > Units &&
			// only check when rowgrid border is crossed
			new_start > Units-lim) {new_start = Units-lim;}
		if(new_start < m1) {new_start = m1;}
	} else if(CellState == csRight) { // dsInsert/Resize
		if(DragCell.Start < 0 &&
			// only check when rowgrid border is crossed
			new_start < lim-units) {new_start = lim-units;}
		if(new_start > m2) {new_start = m2;}
	} else { // dragging and dropping onto another location
		if(cs_empty) {
			// try to fit the cell between m1 and m2
			// m1 overrides m2 (give m1 more priority)
		//  disabled: not really necessary
		//	if(max_set && new_start > m2) {new_start = m2;}
		//	if(min_set && new_start < m1) {new_start = m1;}
		} else {cell_fits = false;}
		// make sure at least MinVisible units are visible
		if(new_start > Units-lim) {new_start = Units-lim;}
		if(new_start < lim-units) {new_start = lim-units;}
		if(max_set && new_start > m2) {cell_fits = false;}
		if(min_set && new_start < m1) {cell_fits = false;}
	}
	return cell_fits;
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::MouseMove(TShiftState Shift,int X,int Y) {
	// Invalidate(), MouseMove, Paint sequence is invalid. Paint first:
	// Invalidate() erases the background and RowGrid::Paint() indeed
	// assumes the background has been erased. This is not true if
	// MouseMove paints before the WM_PAINT message is handled. To
	// bypass the message queue, the Update() method is called.
	Update(); // Only paints when there are invalidated regions.
	if(dragging && (drag_hold == false || Cursor == crHandPoint) &&
		abs(X-drag_x) < MOVEDELTA &&
		abs(Y-drag_y) < MOVEDELTA) {TControl::MouseMove(Shift,X,Y); return;}
		// when MOVEDELTA=1, this is still possible! (ActiveX only?)

	if((dragging && CellState != csNone) || DragState != dsNone) {
		// Set DragState in move and not in down: before dblclick, down
		// is called which would lead to an undesired state change. It may
		// still be however that move changes the state before a dblclick
		// in which case the dblclick is ignored by setting dblclick = false
		if(dragging) { // dragging -> DragState = dsNone
			dragging = false; dblclick = false; // also see MouseDown
			// Do not try over and over again if state transition fails once.
			// This also ensures that MouseDown was called and DragCell was
			// properly set. To ensure that DragCell represents a valid cell,
			// dragging is cleared when the RowGrid Cells property is accessed
			TDragState state = dsNone;
			if(CellState == csEmpty) {state = dsInsert;}
			else if(drag_button == mbLeft) {state = dsDragDrop;}
			else if(drag_button == mbRight) {state = dsCopyDrop;}
			if(state == dsDragDrop || state == dsCopyDrop) {
				dragRowCell(DragCell,state);
			}
			FDragState = state; // set after dragRowCell <-> event exception
		}

		if(DragState != dsNone) {
			// [When cell remains at its position, the mouse cell sometimes is
			// placed at the wrong position. This is caused by x which is not
			// being properly converted back to units: units -> x -> units+/-1]
			TRect new_rect; int new_start;
			MouseToShift(X,Y,new_start);
			int x = FrameWidth + new_start*UnitWidth;
			int height = RowHeight;
			int top = FrameWidth + RowLineWidth +
				DragCell.Row*(RowLineWidth+height);
			int left = FrameWidth + DragCell.Start*UnitWidth;
			int right = left + DragCell.Units*UnitWidth;
			int width = right - left;
			if(CellState == csLeft) {
				new_rect = TRect(x,top,right,top+height);
			} else if(CellState == csRight) {
				new_rect = TRect(left,top,x+width,top+height);
			} else if(VerticalRows) {
				new_rect = TRect(x,X-drag_x+top,x+width,X+height-drag_x+top);
			} else {
				new_rect = TRect(x,Y-drag_y+top,x+width,Y+height-drag_y+top);
			}
			if(VerticalRows) {flipRect(new_rect);}
			// draw_twice = true when Paint() was first
			if(draw_twice) {ToggleFocus();} // clear
			drawFocusRect(new_rect); // new focus
		}
	} else {updateCursor();}
	TControl::MouseMove(Shift,X,Y);
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::dragRowCell(RowCell & row_cell,
	Rowgrid::TDragState & new_state) {
#ifdef _DEBUG
	AnsiString message("OnDragRowCell");
	message += " row:"+IntToStr(row_cell.Row);
	message += " start:"+IntToStr(row_cell.Start);
	message += " units:"+IntToStr(row_cell.Units);
	message += " zorder:"+IntToStr(row_cell.Zorder);

	char string[PTRSIZE];
	snprintf(string,PTRSIZE,"%x",row_cell.Object);
	message += " object:"+String(string);
	Exceptions::writeLog(message);
#endif
	// no resize allowed when new_state equals dsCopyDrop.
	if(new_state != dsCopyDrop || CellState == csCenter) {
		if(OnDragRowCell) {
			bool can_drag = true; void * Object = row_cell.Object;
			bool is_copy = (new_state == dsCopyDrop && DragState == dsNone);
			OnDragRowCell(this,row_cell,CellState,is_copy,can_drag);
			if(can_drag == false) {new_state = dsNone;}
			else if(is_copy && row_cell.Object && row_cell.Object == Object) {
				throw Exception("RowGrid::DragRowCell, Copied RowCell "
					"requires object pointer to newly allocated object");
			}
		}

		if(new_state == dsDragDrop || DragState == dsCopyDrop) {
			// removeRowCell creates an empty position for checkPosition
			removeRowCell(row_cell);
			if(can_remove == false) {new_state = dsNone;}
			else if(CellState == csCenter) {
				// when DragState != dsNone, focus was drawn earlier
				if(DragState != dsNone) {ToggleFocus();} // clear
				doPaintCell(row_cell,true); PaintRow(row_cell.Row);
				PaintFrame(); setCursor(crHandPoint);
			}
		}
	} else {new_state = dsNone;}
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::DblClick(void) {
// assuming dblclick only possible with left mouse button
#ifdef _DEBUG
	Exceptions::writeLog("DblClick");
#endif
	if(dblclick && DragState == dsNone) {
		dblclick = false; // see MouseDown
		dragging = false; setCursor(new_cursor);
		TCellState dbl_state;
		RowCell dbl_cell = MouseToCell(drag_x,drag_y,dbl_state);
		if(dbl_state != csNone && dbl_state != csEmpty) {
#ifdef _DEBUG
			AnsiString message("OnDblClickRowCell");
			message += " row:"+IntToStr(dbl_cell.Row);
			message += " start:"+IntToStr(dbl_cell.Start);
			message += " units:"+IntToStr(dbl_cell.Units);
			message += " zorder:"+IntToStr(dbl_cell.Zorder);

			char string[PTRSIZE];
			snprintf(string,PTRSIZE,"%x",dbl_cell.Object);
			message += " object:"+String(string);
			Exceptions::writeLog(message);
#endif
			if(OnDblClickRowCell) {OnDblClickRowCell(this,dbl_cell);}
		}
		// click very often while inserting a zero units cell
		// - mouseup clears DragState, CellState became csLeft/csRight
		// - dblclick is entered while DragCell was never inserted.
		// click very often on a cell while holding left-ctrl
		// - rowgrid is refreshed by TFmain::MouseDown
		// - dblclick uses invalid DragCell object
		// => DragCell is not valid after mouseup/mousedown/mousemove
		// => mousemove checks dragging before entering state to fix this
		// => mouseup only works with DragCell if DragState was set
		// => DblClick determines the cell when it is needed to fix this
		// => dblclick var is used for other purposes:
		//    - dblclick after dblclick or dblclick after move
	}
	TControl::DblClick();
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::MouseDown(TMouseButton Button,TShiftState Shift,
	int X,int Y) {
#ifdef _DEBUG
	Exceptions::writeLog("MouseDown");
#endif
	if(DragState == dsNone && Shift.Contains(ssDouble) == false) {
		// check for ssDouble: down, up, dblclick, up, down
		// [future: perhaps move MouseToCell to MouseMove just like DblClick.
		// forget this, we need dragging anyway when state transition fails.
		// Also we do not want MouseToCell to find another DragCell when the
		// Cells structure changes in between MouseDown and MouseMove]
		dragging = true; drag_hold = false; chk_position = CheckPosition;
		draw_twice = false; drag_button = Button; drag_x = X; drag_y = Y;
		DragCell = MouseToCell(drag_x,drag_y,FCellState);
		if(AltKeyInserts && Shift.Contains(ssAlt) && CellState != csNone) {
			chk_position = false;
			if(CellState != csEmpty) {
				FCellState = csEmpty;
				// DragCell.Zorder++;
				// application should
				// manage Zorder itself
				// (using MouseToCell).
			}
		}
		if(CellState == csEmpty) {
			DragCell.FUnits = 0; DragCell.Object = NULL; int x;
			if(VerticalRows) {x = drag_y; setCursor(crArrowVert);}
			else {x = drag_x; setCursor(crArrowHoriz);}
			DragCell.FStart = RoundTo((x - FrameWidth) * 1.0/UnitWidth,0);
			if(Button != mbLeft) {dragging = false; setCursor(new_cursor);}
		} else if(Button == mbMiddle) {dragging = false;}

		dblclick = true;
		// when dblclick, move, down, click, up, dblclick
		// - down ignored since move sets DragState -> dsDragDrop
		// - second dblclick would use DragCell that was just removed
		// when down, move, click, up, dblclick
		// - DragState: dsNone -> dsDragDrop -> dsNone and dblclick called

		bool scroll = GetKeyState(VK_SCROLL) & 0x0001; // undocumented
		if(ShiftKeyHolds && (Shift.Contains(ssShift) ||
			(scroll && Shift.Contains(ssCtrl) == false))) {
			drag_hold = true;
			if(scroll && Button == mbLeft) {
				if(CellState == csCenter) {setCursor(crHandPoint);}
			} else {MouseMove(Shift,X,Y);}
		}
	}
	TControl::MouseDown(Button,Shift,X,Y);
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::MouseUp(TMouseButton Button,TShiftState Shift,
	int X,int Y) {
#ifdef _DEBUG
	Exceptions::writeLog("MouseUp");
#endif
	if((Button == mbLeft || Button == mbRight) && drag_hold == false) {
		dragging = false; // after only down, up: move should see a down again
		int new_start;
		if(DragState == dsInsert) {
			MouseToShift(X,Y,new_start);
			if(new_start < DragCell.Start) {
				insertRowCell(DragCell.Row,new_start,
					DragCell.Start-new_start,DragCell.Zorder,NULL);
				if(can_insert) {
					ToggleFocus(); PaintRow(DragCell.Row);
					PaintFrame(); doClearDragState();
				}
			} else if(new_start > DragCell.Start) {
				insertRowCell(DragCell.Row,DragCell.Start,
					new_start-DragCell.Start,DragCell.Zorder,NULL);
				if(can_insert) {
					ToggleFocus(); PaintRow(DragCell.Row);
					PaintFrame(); doClearDragState();
				}
			} else {ToggleFocus(); doClearDragState();}
			// else new_start == DragCell.Start: empty cell, do not insert
		} else if(DragState == dsDragDrop || DragState == dsCopyDrop) {
			if(CellState == csCenter) {
				TCellState drop_state;
				int check_x = doMouseToShift(X,Y,new_start);
				if(VerticalRows) {Y = check_x;} else {X = check_x;}
				RowCell new_cell = MouseToCell(X,Y,drop_state);
				if(drop_state != csNone) {
					bool cell_swap = drop_state != csEmpty &&
						ShiftKeySwaps && (Shift.Contains(ssShift) ||
						GetKeyState(VK_SCROLL) & 0x0001);
					RowCell new_drag = DragCell;
					TDragState state = DragState;
					if(cell_swap) { // make empty position
						dragRowCell(new_cell,state);
						new_drag.FRow = DragCell.Row;
						new_drag.FStart = DragCell.Start -
							(new_start-new_cell.Start);
						new_drag.FUnits = new_cell.Units;
						new_drag.Zorder = new_cell.Zorder;
						new_drag.Object = new_cell.Object;
						// do not allow overlap during swap
					}
					if(state == DragState) { // can_drag && can_remove?
						if(cell_swap) {new_start = new_cell.Start;}
						// try to fit the dragged cell onto the empty position
						bool cell_fits = checkPosition(X,Y,new_start);
						if(cell_fits || cell_swap) {
							if(cell_fits) {new_cell.FUnits = DragCell.Units;}
							new_cell.Zorder = DragCell.Zorder;
							new_cell.Object = DragCell.Object;
							DragCell = new_drag; // before possible exception
							try {
								try {
									// DragState can be dsCopyDrop here
									insertRowCell(new_cell.Row,new_start,
										new_cell.Units,new_cell.Zorder,
										new_cell.Object);
								} catch(...) {can_insert = false; throw;}
								if(can_insert) { // check skipped when exception
									// cell_swap: dragRowCell already cleared
									if(cell_swap == false) {ToggleFocus();}
									PaintRow(new_cell.Row); PaintFrame();
									if(cell_swap == false) {doClearDragState();}
								}
							} __finally {
								if(cell_swap) {
									FDragState = dsDragDrop;
									if(can_insert == false) {
										bool can_clear = true;
										ClearRowCell(new_cell,can_clear);
									}
									draw_twice = false; // draw new_rect
									MouseMove(TShiftState(),X,Y);
								}
							}
						}
					}
				}
			} else { // border movement, only for dsDragDrop (see dragRowcell)
				// MouseToShift adjusts movement so the dragged cell will fit
				// (this may not be true when an application writes DragCell)
				MouseToShift(X,Y,new_start);
				RowCell new_cell = DragCell;
				if(CellState == csLeft) {
					new_cell.FStart = new_start;
					new_cell.FUnits += DragCell.Start-new_start;
				} else {new_cell.FUnits += new_start-DragCell.Start;}
				insertRowCell(new_cell.Row,new_cell.Start,
					new_cell.Units,new_cell.Zorder,new_cell.Object);
				if(can_insert) {
					ToggleFocus(); doPaintCell(DragCell,true); // clear DragCell
					PaintRow(new_cell.Row); PaintFrame(); doClearDragState();
				}
			}
		} else {setCursor(new_cursor);}
	} else {drag_hold = false;}
	TControl::MouseUp(Button,Shift,X,Y);
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::doClearDragState(void) {
#ifdef _DEBUG
	Exceptions::writeLog("doClearDragState");
#endif
	FDragState = dsNone; dragging = false;
	setCursor(new_cursor);
}
//---------------------------------------------------------------------------

