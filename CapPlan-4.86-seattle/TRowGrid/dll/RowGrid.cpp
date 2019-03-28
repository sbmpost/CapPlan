//---------------------------------------------------------------------------
#include <vcl.h>
#include <windows.h>
#pragma hdrstop
//---------------------------------------------------------------------------
//   Important note about DLL memory management when your DLL uses the
//   static version of the RunTime Library:
//
//   If your DLL exports any functions that pass String objects (or structs/
//   classes containing nested Strings) as parameter or function results,
//   you will need to add the library MEMMGR.LIB to both the DLL project and
//   any other projects that use the DLL.  You will also need to use MEMMGR.LIB
//   if any other projects which use the DLL will be performing new or delete
//   operations on any non-TObject-derived classes which are exported from the
//   DLL. Adding MEMMGR.LIB to your project will change the DLL and its calling
//   EXE's to use the BORLNDMM.DLL as their memory manager.  In these cases,
//   the file BORLNDMM.DLL should be deployed along with your DLL.
//
//   To avoid using BORLNDMM.DLL, pass string information using "char *" or
//   ShortString parameters.
//
//   If your DLL uses the dynamic version of the RTL, you do not need to
//   explicitly add MEMMGR.LIB as this will be done implicitly for you
//---------------------------------------------------------------------------

#include <math.hpp>
#include <algorithm>
#include "RowGrid.h"
#ifdef _DEBUG
#define PTRSIZE 8+1
// +1: '\0' character
#include "Exceptions.h"
#endif

#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason,
	void* lpReserved) {
	return 1;
}
//---------------------------------------------------------------------------

// todo: RowGrid: add keyboard support? <-> TGraphicControl -> TWinControl
// todo: RowGrid: add scroll support. i.e when Units*UnitWidth > ClientWidth
__fastcall TRowGrid::TRowGrid(TComponent* Owner) : TCustomControl(Owner) {
	internal_call = dragging = false;
#ifdef LICENSENR
	Caption = "Copyright 2004-2005 by PostWare (S.B.M. Post), "LICENSENR;
#else
	Caption = "Copyright 2004-2005 by PostWare (S.B.M. Post), Demo version";
#endif

	FDragState = dsNone;
	// cursor_col = cursor_row = -1;

	Units = 36; Ticks = 4; RowCount = 10; RowHeight = 20;
	RowLineWidth = 10; UnitWidth = 15; CellBorderWidth = 8;
	FrameWidth = 2; // CursorWidth = 3;
	CellFrameWidth = 1;
	Width = Units*UnitWidth + 2*FrameWidth;
	Height = (RowCount+1)*RowLineWidth + RowCount*RowHeight + 2*FrameWidth;

	DefaultDrawing = true;
	CheckPosition = true;
	CrossBorders = true;
	MinVisible = 1;
	RowLineColor = clSkyBlue;
	CellColor = clBackground;
	OnInsertRowCell = NULL;
	OnRemoveRowCell = NULL;
	OnDrawRowCell = NULL;
	OnDragRowCell = NULL;
	OnMouseOverRowCell = NULL;
	OnDblClickRowCell = NULL;

	// what if multiple TRowGrid instances?
	Screen->Cursors[crArrowSplit] = LoadCursor(HInstance,"ArrowSplit");
	Screen->Cursors[crArrowLeft] = LoadCursor(HInstance,"ArrowLeft");
	Screen->Cursors[crArrowRight] = LoadCursor(HInstance,"ArrowRight");
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::insertRowCell(int row,int start,int units,
	int zorder) {
	internal_call = true; InsertRowCell(row,start,units,zorder,NULL);
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::removeRowCell(RowCell & row_cell) {
	internal_call = true; RemoveRowCell(row_cell);
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::InsertRowCell(int Row,int Start,int Units,int Zorder,
	void * Object) {
	if(Row < 0 || Row >= RowCount) {
		throw Exception("RowGrid::InsertRowCell, row index out of bounds");
	}
	RowCell insert_cell;
	insert_cell.FRow = Row;
	insert_cell.FStart = Start;
	insert_cell.FUnits = Units;
	insert_cell.FZorder = Zorder;
	insert_cell.object = Object;

	can_insert = true;
	if(internal_call) {
		internal_call = false;
		// clear internal_call as soon as possible:
		// suppose exception in OnInsertRowCell event
#ifdef _DEBUG
		AnsiString message("OnInsertRowCell");
		message += " row:"+IntToStr(insert_cell.Row);
		message += " start:"+IntToStr(insert_cell.Start);
		message += " units:"+IntToStr(insert_cell.Units);
		message += " zorder:"+IntToStr(insert_cell.Zorder);
		Exceptions::writeLog(message);
#endif
		if(OnInsertRowCell) {OnInsertRowCell(this,insert_cell,can_insert);}
	}

	// After OnInsertRowCell: The cell may have been
	// supplied with an object or can_insert was cleared
	if(can_insert) {Cells[Row].push_back(insert_cell);}
//	if(cursor_col == col && cursor_row == row) {cursor_col = col+1;}
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::RemoveRowCell(RowCell & RowCell) {
	if(RowCell.Row < 0 || RowCell.Row >= RowCount) {
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
		snprintf(string,PTRSIZE,"%x",cell->object);
		message += " object:"+String(string);
		Exceptions::writeLog(message);
#endif
		// User is responsible for releasing cell->object. When
		// internal_call this should be done in OnRemoveCell.
		if(OnRemoveRowCell) {OnRemoveRowCell(this,*cell,can_remove);}
	}

	// After OnRemoveRowCell: The cell object must
	// first be deleted or can_remove was cleared.
	if(can_remove) {Cells[RowCell.Row].erase(cell);}
//	if(cursor_col == col && cursor_row == row) {cursor_col = cursor_row = -1;}
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::Paint(void) {
#ifdef _DEBUG
	AnsiString message("Paint");
	Exceptions::writeLog(message);
#endif
	TRect frame(0,0,Width,Height);
	Frame3D(Canvas,frame,clBlack,clBlack,FrameWidth);

	cells_t::iterator row = Cells.begin();
	TRect rect(FrameWidth,0,FrameWidth + Units*UnitWidth,0);
	for(int r = 0;r <= RowCount;r++,row++) {
		int top = FrameWidth + r*(RowLineWidth+RowHeight);
		rect.Top = top; rect.Bottom = top + RowLineWidth;
		PaintRow(rect); if(r == RowCount) {break;}

		sort(row->begin(),row->end());
		row_t::iterator cell = row->begin();
		for(;cell != row->end();cell++) {PaintCell(*cell);}
	}
	if(DragState != dsNone) {draw_twice = false; drawFocusRect(old_rect);}
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::PaintCell(RowCell & row_cell) {
	TRect rect;
	rect.Left = FrameWidth + row_cell.Start*UnitWidth;
	rect.Right = rect.Left + row_cell.Units*UnitWidth;
	rect.Top = FrameWidth + RowLineWidth +
		row_cell.Row*(RowLineWidth+RowHeight);
	rect.Bottom = rect.Top + RowHeight;

	if(rect.Left >= FrameWidth + ClientWidth ||
		rect.Right <= FrameWidth) {return;}

	if(rect.Left < 0) {rect.Left = 0;}
	int width = ClientWidth + 2*FrameWidth;
	if(rect.Right > width) {rect.Right = width;}

	bool cursor = false;
//	bool cursor = (col == cursor_col && row == cursor_row);
	if(DefaultDrawing) {
		// Frame3D reduces the size of rect by the specified width
		if(cursor) {/*Frame3D(Canvas,rect,clBlue,clBlue,CursorWidth);*/}
		else {Frame3D(Canvas,rect,clBlack,clBlack,CellFrameWidth);}
		Canvas->Brush->Color = CellColor;
#ifdef LICENSENR
		Canvas->FillRect(rect);
#else
		Canvas->TextRect(rect,rect.Left,rect.Top,"DEMO VERSION");
#endif
	}
#ifdef LICENSENR
	if(OnDrawRowCell) {OnDrawRowCell(this,row_cell,rect,cursor);}
#endif
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::PaintRow(TRect & rect) {
	Canvas->Brush->Color = RowLineColor;
	Canvas->FillRect(rect);
	Canvas->Pen->Color = clBlack;
	int middle2 = (rect.Bottom-rect.Top)/2;
	int middle4 = (rect.Bottom-rect.Top)/4;
	Canvas->MoveTo(FrameWidth,rect.Top + middle2);
	Canvas->LineTo(FrameWidth + Units*UnitWidth,rect.Top + middle2);
	for(int u=1;u < Units;u++) {
		int x = FrameWidth + u*UnitWidth;
		if((u % Ticks) == 0) {
			Canvas->Pen->Color = clRed;
			Canvas->MoveTo(x,rect.Top);
			Canvas->LineTo(x,rect.Bottom);
		} else {
			Canvas->Pen->Color = clBlack;
			Canvas->MoveTo(x,rect.Top+middle4);
			Canvas->LineTo(x,rect.Bottom-middle4);
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::drawFocusRect(TRect & rect) {
	TRect new_rect = rect; Canvas->Brush->Color = clWhite; // where no dots
	Canvas->Pen->Mode = pmNotXor; Canvas->Pen->Style = psDot;
	if(draw_twice) {Frame3D(Canvas,old_rect,clBlack,clBlack,1);}
	Frame3D(Canvas,new_rect,clBlack,clBlack,1);
	Canvas->Pen->Mode = pmCopy; Canvas->Pen->Style = psSolid;
	old_rect = rect; draw_twice = true;
}
//---------------------------------------------------------------------------

RowCell __fastcall TRowGrid::MouseToCell(int X,int Y,TCellState & State) {
	State = csNone;
	RowCell row_cell;
	row_cell.FRow = 0;
	row_cell.FZorder = 0;
	int old_top = FrameWidth;

	bool search = true;
	bool restore_dragging = dragging;
	cells_t::iterator row = Cells.begin();
	for(int r = 0;search && r <= RowCount;r++,row++) {
		int top = FrameWidth + r*(RowLineWidth+RowHeight);
		if(top + RowLineWidth <= Y) { // <=, top & left included
			if(r == RowCount) {break;}
			if(Y < top + RowLineWidth + RowHeight) {
				sort(row->begin(),row->end());
				row_t::reverse_iterator cell = row->rbegin();
				for(;search && cell != row->rend();cell++) {
					int left = FrameWidth + cell->Start*UnitWidth;
					int right = left + cell->Units*UnitWidth;
					// <=, top & left included
					if(left <= X && X < right) {
						search = false;
						State = csCenter;
						if(X-left <= CellBorderWidth) {State = csLeft;}
						if(right-X <= CellBorderWidth) {State = csRight;}
						row_cell = *cell;
					}
				}
				if(search) {
					search = false;
//					if(FrameWidth < X && X < FrameWidth + Units*UnitWidth) {
						row_cell.FRow = r; State = csEmpty;
//					}
				}
			}
		} else {
			search = false;
			if(r && old_top + RowLineWidth <= Y && Y < top) {
//				&& FrameWidth < X && X < FrameWidth + Units*UnitWidth) {
				row_cell.FRow = r-1; State = csEmpty;
			}
		}
		old_top = top;
	}
	dragging = restore_dragging;

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
	snprintf(string,PTRSIZE,"%x",row_cell.object);
	message += " object:"+String(string);
	Exceptions::writeLog(message);
#endif

	return row_cell;
}
//---------------------------------------------------------------------------

bool __fastcall TRowGrid::MouseToShift(int X,int & old_start,int & new_start) {
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
	int x = X;
	if(DragState != dsInsert) {
		int left = FrameWidth + drag_cell.Start*UnitWidth;
		x = x - drag_x + left;
	}
	new_start = RoundTo((x - FrameWidth) * 1.0/UnitWidth,0);
	if(DragState == dsInsert) {
		old_start = RoundTo((drag_x - FrameWidth) * 1.0/UnitWidth,0);
		if(new_start < old_start) {cell_state = csLeft;}
		else {cell_state = csRight;}
	} else {old_start = drag_cell.Start;}

	// the new position (new_start) may not be valid. If CheckPosition is true,
	// we check for overlap with other cells and change new_start if necessary
	bool pos_ok = true;
	if(CheckPosition) {
		if(DragState == dsInsert || cell_state != csCenter) {
			pos_ok = checkPosition(drag_x,old_start,new_start);
		} else {pos_ok = checkPosition(X,old_start,new_start);}
	}

#ifdef _DEBUG
	AnsiString message("MouseToShift");
	message += " old_start:"+IntToStr(old_start);
	message += " new_start:"+IntToStr(new_start);
	message += " DragState:"+IntToStr(DragState);
	message += " pos_ok:"+IntToStr(pos_ok);
	Exceptions::writeLog(message);
#endif

	return pos_ok;
}
//---------------------------------------------------------------------------

bool __fastcall TRowGrid::checkPosition(int X,int & old_start,int & new_start) {
	// find markers m1 and m2 around X, X must be an csEmpty position
	int m1 = 0; int m2 = Units;
	row_t::iterator min; bool min_set = false;
	row_t::iterator max; bool max_set = false;
	row_t::iterator cell = Cells[drag_cell.Row].begin();
	row_t::iterator end = Cells[drag_cell.Row].end();
	for(;cell != end;cell++) {
		int min_r; int max_l; // |l min r| <= X < |l max r|
		int left = FrameWidth + cell->Start*UnitWidth;
		int right = left + cell->Units*UnitWidth;
		if(right <= X) {
			if(min_set == false) {min_set = true; min = cell; min_r = right;}
			else if(min_r < right) {min = cell; min_r = right;}
		}
		if(left > X) {
			if(max_set == false) {max_set = true; max = cell; max_l = left;}
			else if(max_l > left) {max = cell; max_l = left;}
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
	int units = 0; if(DragState != dsInsert) {units = drag_cell.Units;}
	int lim = MinVisible; if(lim > units) {lim = units;}
	int c = 1; if(DragState == dsInsert) {c = 0;}
	bool drop_ok = true;
	if(cell_state == csLeft) { // dsInsert/Resize
		if(new_start > Units-lim) {new_start = Units-lim;}
		if(new_start < m1) {new_start = m1;}
		if(new_start > old_start+units-c) {new_start = old_start+units-c;}
	} else if(cell_state == csRight) { // dsInsert/Resize
		if(new_start < lim-units) {new_start = lim-units;}
		if(new_start > m2-units) {new_start = m2-units;}
		if(new_start < old_start-units+c) {new_start = old_start-units+c;}
	} else { // dragging and dropping onto another location
		if(new_start < lim-units) {new_start = lim-units;}
		if(new_start < m1) {
			if(m1 != 0) {drop_ok = false;} // cell does not fit
			else if(CrossBorders == false) {new_start = m1;}
		}
		if(new_start > Units-lim) {new_start = Units-lim;}
		if(new_start > m2-units) {
			if(m2 != Units) {drop_ok = false;} // cell does not fit
			else if(CrossBorders == false) {new_start = m2-units;}
		}
	}
	return drop_ok;
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::MouseMove(TShiftState Shift,int X,int Y) {
#ifndef _DEBUG
	TCellState move_state = moveMouse(X,Y);
#endif
	if((dragging && cell_state != csNone) || DragState != dsNone) {
		// Set DragState in move and not in down: before dblclick, down
		// is called which would lead to an undesired state change. It may
		// still be however that move changes the state before a dblclick
		// in which case the dblclick is ignored by setting dblclick = false
		if(dragging && DragState == dsNone) {
			dragging = false; dblclick = false; // also see MouseDown
			// Do not try over and over again if state transition fails once.
			// This also ensures that MouseDown was called and drag_cell was
			// properly set. To ensure that drag_cell represents a valid cell,
			// dragging is cleared when the RowGrid Cells property is accessed
			TDragState state = dsNone;
			if(cell_state == csEmpty) {state = dsInsert;}
			else if(drag_button == mbLeft) {state = dsDragDrop;}
			else if(drag_button == mbRight) {state = dsCopyDrop;}
			if(state == dsDragDrop || state == dsCopyDrop) {dragRowCell(state);}
			FDragState = state; // set after dragRowCell <-> event exception
		}

		if(DragState != dsNone) {
			// [When cell remains at its position, the mouse cell sometimes is
			// placed at the wrong position. This is caused by x which is not
			// being properly converted back to units: units -> x -> units+/-1]
			TRect new_rect; int old_start,new_start;
			MouseToShift(X,old_start,new_start);
			int x = FrameWidth + new_start*UnitWidth;
			int height = RowHeight;
			int top = FrameWidth + RowLineWidth +
				drag_cell.Row*(RowLineWidth+height);
			if(DragState == dsInsert) {
				int x_old = FrameWidth + old_start*UnitWidth;
				if(x_old < x) {new_rect = TRect(x_old,top,x,top+height);}
				else {new_rect = TRect(x,top,x_old,top+height);}
			} else { // dsDragDrop/dsCopyDrop
				int left = FrameWidth + drag_cell.Start*UnitWidth;
				int right = left + drag_cell.Units*UnitWidth;
				int width = right - left;
				if(cell_state == csLeft) {
					new_rect=TRect(x,top,right,top+height);
				} else if(cell_state == csRight) {
					new_rect=TRect(left,top,x+width,top+height);
				} else {
					new_rect=TRect(x,Y-drag_y+top,x+width,Y+height-drag_y+top);
				}
			}
			drawFocusRect(new_rect);
		}
	} else {
		TCursor cursor = crDefault;
#ifndef _DEBUG
		if(move_state == csLeft) {cursor = (TCursor) crArrowRight;}
		else if(move_state == csRight) {cursor = (TCursor) crArrowLeft;}
#endif
		Screen->Cursor = cursor;
	}
	TControl::MouseMove(Shift,X,Y);
	// TWinControl::MouseMove(Shift,X,Y);
}
//---------------------------------------------------------------------------

TCellState __fastcall TRowGrid::moveMouse(int X,int Y) {
	TCellState move_state; RowCell move_cell;
	move_cell = MouseToCell(X,Y,move_state);
	if(move_state != csNone && move_state != csEmpty) {
		// todo: && (prev_cell == move_cell) == false
		// prev_cell = move_cell;
		// how should prev_cell be cleared when its matching
		// RowCell is cleared and how to initialize prev_cell
//#ifdef _DEBUG
//		AnsiString message("OnMouseOverRowCell");
//		message += " row:"+IntToStr(move_cell.Row);
//		message += " start:"+IntToStr(move_cell.Start);
//		message += " units:"+IntToStr(move_cell.Units);
//		Exceptions::writeLog(message);
//#endif
		if(OnMouseOverRowCell) {OnMouseOverRowCell(this,move_cell);}
	}
	return move_state;
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::dragRowCell(Rowgrid::TDragState & new_state) {
#ifdef _DEBUG
	AnsiString message("OnDragRowCell");
	message += " row:"+IntToStr(drag_cell.Row);
	message += " start:"+IntToStr(drag_cell.Start);
	message += " units:"+IntToStr(drag_cell.Units);
	message += " zorder:"+IntToStr(drag_cell.Zorder);

	char string[PTRSIZE];
	snprintf(string,PTRSIZE,"%x",drag_cell.object);
	message += " object:"+String(string);
	Exceptions::writeLog(message);
#endif
	if(new_state == dsCopyDrop && cell_state != csCenter) {
		// resizing is not allowed when the new state equals dsCopyDrop.
		new_state = dsNone;
	} else {
		if(OnDragRowCell) {
			bool can_drag = true;
			OnDragRowCell(this,drag_cell,cell_state,
				new_state == dsCopyDrop,can_drag);
			if(can_drag == false) {new_state = dsNone;}
		}

		if(new_state == dsDragDrop) {
			// removeRowCell creates an empty position for checkPosition
			removeRowCell(drag_cell);
			if(can_remove == false) {new_state = dsNone;}
			else if(cell_state == csCenter) {
				Invalidate(); Screen->Cursor = (TCursor) crDrag;
			}
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::DblClick(void) {
// assuming dblclick only possible with left mouse button
#ifdef _DEBUG
	Exceptions::writeLog("DblClick");
#endif
	if(dblclick && DragState == dsNone) {
		dblclick = false; // see MouseDown
		TCellState dbl_state; RowCell dbl_cell;
		dbl_cell = MouseToCell(drag_x,drag_y,dbl_state);
		if(dbl_state != csNone && dbl_state != csEmpty) {
#ifdef _DEBUG
			AnsiString message("OnDblClickRowCell");
			message += " row:"+IntToStr(dbl_cell.Row);
			message += " start:"+IntToStr(dbl_cell.Start);
			message += " units:"+IntToStr(dbl_cell.Units);
			message += " zorder:"+IntToStr(dbl_cell.Zorder);

			char string[PTRSIZE];
			snprintf(string,PTRSIZE,"%x",dbl_cell.object);
			message += " object:"+String(string);
			Exceptions::writeLog(message);
#endif
			if(OnDblClickRowCell) {OnDblClickRowCell(this,dbl_cell);}
		}
		// click very often while inserting a zero units cell
		// - mouseup clears DragState, cell_state became csLeft/csRight
		// - dblclick is entered while drag_cell was never inserted.
		// click very often on a cell while holding left-ctrl
		// - rowgrid is refreshed by TFmain::MouseDown
		// - dblclick uses invalid drag_cell object
		// => drag_cell is not valid after mouseup/mousedown/mousemove
		// => mousemove checks dragging before entering state to fix this
		// => mouseup only works with drag_cell if DragState was set
		// => DblClick determines the cell when it is needed to fix this
		// => dblclick var is used for other purposes:
		//    - dblclick after dblclick or dblclick after move
	}
	TControl::DblClick();
	// TWinControl::DblClick();
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::MouseDown(TMouseButton Button,TShiftState Shift,
	int X,int Y) {
#ifdef _DEBUG
	Exceptions::writeLog("MouseDown");
#endif
	if((Button == mbLeft || Button == mbRight) && DragState == dsNone &&
		Shift.Contains(ssDouble) == false) { // down, up, dblclick, up, down
		// future: perhaps move MouseToCell to dragRowCell just like DblClick
		dragging = true; drag_hold = false; draw_twice = false;
		drag_button = Button; drag_x = X; drag_y = Y;
		drag_cell = MouseToCell(drag_x,drag_y,cell_state);
		if(cell_state == csEmpty) {Screen->Cursor = (TCursor) crArrowSplit;}
		else if(cell_state == csCenter) {/*check and paint cursor*/}

		dblclick = true;
		// when dblclick, move, down, click, up, dblclick
		// - down ignored since move sets DragState -> dsDragDrop
		// - second dblclick would use drag_cell that was just removed
		// when down, move, click, up, dblclick
		// - DragState: dsNone -> dsDragDrop -> dsNone and dblclick called

		if(Shift.Contains(ssShift)) {drag_hold = true; MouseMove(Shift,X,Y);}
	}
	TControl::MouseDown(Button,Shift,X,Y);
	// TWinControl::MouseDown(Button,Shift,X,Y);
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::MouseUp(TMouseButton Button,TShiftState Shift,
	int X,int Y) {
#ifdef _DEBUG
	Exceptions::writeLog("MouseUp");
#endif
	if((Button == mbLeft || Button == mbRight) && drag_hold == false) {
		dragging = false; // after only down, up: move should see a down again
		int old_start,new_start,x;
		if(DragState == dsInsert) {
			MouseToShift(X,old_start,new_start);
			if(new_start < old_start) {
				insertRowCell(drag_cell.Row,new_start,
					old_start-new_start,drag_cell.Zorder);
				if(can_insert) {ClearDragState();}
			} else if(new_start > old_start) {
				insertRowCell(drag_cell.Row,old_start,
					new_start-old_start,drag_cell.Zorder);
				if(can_insert) {ClearDragState();}
			} else {ClearDragState();}
			// new_start == old_start: empty cell, do not insert
		} else if(DragState == dsDragDrop || DragState == dsCopyDrop) {
			if(cell_state == csCenter) {
				TCellState drop_state;
				RowCell drop_cell = MouseToCell(X,Y,drop_state);
				if((CheckPosition == false && drop_state != csNone) ||
					drop_state == csEmpty) {
					// clicked on an empty position
					RowCell old_cell = drag_cell; // for restoring drag_cell
					drag_cell.FRow = drop_cell.Row; // before MouseToShift
					// maybe supply MouseToShift with a row argument instead?
					// try to fit the dragged cell onto the empty position
					if(MouseToShift(X,old_start,new_start)) {
						// restore before possible exception
						drag_cell = old_cell;
						insertRowCell(drop_cell.Row,new_start,
							drag_cell.Units,drag_cell.Zorder);
						if(can_insert) {ClearDragState();}
					} else {drag_cell = old_cell;}
				}
			} else { // border movement, only for dsDragDrop (see dragRowcell)
				// MouseToShift adjusts movement so the dragged cell will fit
				MouseToShift(X,old_start,new_start);
				// keep drag_cell, no need to restore
				RowCell new_cell = drag_cell;
				if(cell_state == csLeft) {
					new_cell.FStart = new_start;
					new_cell.FUnits += old_start-new_start;
				} else {new_cell.FUnits += new_start-old_start;}
				insertRowCell(new_cell.Row,new_cell.Start,
					new_cell.Units,new_cell.Zorder);
				if(can_insert) {ClearDragState();}
			}
		}
	} else {drag_hold = false;}
	TControl::MouseUp(Button,Shift,X,Y);
	// TWinControl::MouseUp(Button,Shift,X,Y);
}
//---------------------------------------------------------------------------

void __fastcall TRowGrid::ClearDragState(void) {
#ifdef _DEBUG
    Exceptions::writeLog("ClearDragState");
#endif
	FDragState = dsNone; dragging = false;
	Invalidate(); Screen->Cursor = crDefault;
}
//---------------------------------------------------------------------------

/*
void __fastcall TRowGrid::KeyDown(Word &Key,TShiftState Shift) {
#ifdef _DEBUG
	Exceptions::writeLog("KeyDown");
#endif
	switch(Key) {
	}

// todo: keyboard support for cursor movement and selecting cells
//	case VK_SPACE:
//		if(StringGrid1->Options.Contains(goEditing) == false) {
//			bool no_select = false;
//			StringGrid1SelectCell(Sender,StringGrid1->Col,
//				StringGrid1->Row,no_select);
//		}
//	break;

//  dirty VK_SPACE CanSelect hack
//	if(0 > GetKeyState(VK_CONTROL) || CanSelect == false) {
//	} else if(0 > GetKeyState(VK_SHIFT)) {
//		int row_from = StringGrid1->Row; int row_to = ARow;
//		int col_from = StringGrid1->Col; int col_to = ACol;
//		if(row_from > row_to) {row_from = ARow; row_to = StringGrid1->Row;}
//		if(col_from > col_to) {col_from = ACol; col_to = StringGrid1->Col;}
//		for(int row=row_from;row <= row_to;row++) {
//			for(int col=col_from;col <= col_to;col++) {
//				Cell & cell = grid[row-1][col-1];
//				if(cell.selected) {
//					Cell key;
//					PROG_IF(hash->search(cell,key) == false,"wrong selection");
//					cell.selected = false; hash->erase(); // selected -> repaint
//				} else {cell.selected = true; hash->insert(cell);}
//			}
//		}
//		StringGrid1->Repaint(); // need to repaint several cells here
//		Label10->Caption = IntToStr(hash->elementCount());
//	}
	TWinControl::KeyDown(Key,Shift);
}
//---------------------------------------------------------------------------
*/

