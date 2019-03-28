//$Id: DataRows.cpp,v 1.57 2013/04/06 08:12:36 darseq Exp $
#include <vcl.h>
#pragma hdrstop
#pragma resource "*.res"
#include "DataRows.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
static inline void ValidCtrCheck(TDataRows *) {
	new TDataRows(NULL);
}
//---------------------------------------------------------------------------

namespace Datarows {
	void __fastcall PACKAGE Register() {
		TComponentClass classes[1] = {__classid(TDataRows)};
		RegisterComponents("PostWare", classes, 0);
	}
}
//---------------------------------------------------------------------------

__fastcall TDataRows::TDataRows(TComponent* Owner) : TStringGrid(Owner) {
	DoubleBuffered = true; DefaultPrinting = true;
	PrintCellPadding = 10; DefaultRowHeight = 18;
	KeySearchColumn = 1; prev_key = '\0';
	if(ComponentState.Contains(csDesigning)) {
		setRowCount(2); // 2 so column widths can be changed
	} else {setRowCount(1);}
	TStringGrid::FixedCols = 0; // remove fixed columns
	TGridOptions grid_options = TGridOptions();
	grid_options << goDrawFocusSelected;
	grid_options << goFixedVertLine;
	grid_options << goFixedHorzLine;
	grid_options << goVertLine;
	grid_options << goHorzLine;
	grid_options << goTabs;
	grid_options << goRowSelect;
	grid_options << goThumbTracking;
	grid_options >> goEditing;
	grid_options << goRangeSelect;
	TStringGrid::Options = grid_options;
	// It is possible that goEditing is false while the user still toggles
	// EditorMode by pressing F2 or ENTER. Therefore goEditing is used to
	// determine if the editor is active. F2 can still be used for editing
	// a row but when the row is deselected, edit mode is disabled again.
	// In that way row data can be reloaded just before a row is edited.
	ScrollBars = ssVertical;

	OnBeforeRefresh = NULL;
	OnAfterRefresh = NULL;
	OnBeforeReadRow = NULL;
	OnAfterReadRow = NULL;
	OnStoreRow = NULL;
	OnDataToCell = NULL;
	OnCellToData = NULL;
	OnPrintRow = NULL;
	OnPrintCell = NULL;
	OnNewPage = NULL;
	OnEndPage = NULL;
	OnEndDoc = NULL;
	FRowState = rsNone;

	FMeta = new TStringList();
	FLabels = new TStringList();
	FRowId = new TStringList();
	FSelect = new TStringList();

	// weird fix we need for wine
	// after bds grids.pas update
	caret_timer = new TTimer(this);
	caret_timer->Enabled = false;
	caret_timer->Interval = 100;
	caret_timer->OnTimer = ShowCaretTimer;
}
//---------------------------------------------------------------------------

__fastcall TDataRows::~TDataRows(void) {
	delete caret_timer;
	delete FSelect;
	delete FRowId;
	delete FLabels;
	delete FMeta;
}
//---------------------------------------------------------------------------

void __fastcall TDataRows::ShowCaretTimer(TObject *Sender) {
	caret_timer->Enabled = false;
	PostMessage(InplaceEditor->Handle,EM_SCROLLCARET,0,0);
}
//---------------------------------------------------------------------------

void __fastcall TDataRows::refresh(id_t id) {
	if(editingRow()) {
		throw Exception("DataRows::refresh, invalid RowState");
	}
	Query * query = NULL;
	if(OnBeforeRefresh) {OnBeforeRefresh(this,query,id);}
	int top_row = TopRow ? TopRow : 1;
	int select_row = 0; clear();
	if(query) {
		int row = 1;
		for(;query->next();row++) {
			if(row >= RowCount) {setRowCount(row*2);}
			for(int i=0;i != Meta->Count;i++) {
				Field field = query->fieldByName(Meta->Strings[i]);
				Cells[i][row] = data2cell(i,field.toString());
			}
			if(id != UNDEFINED && id == getId(row)) {
				select_row = row; id = UNDEFINED;
			}
		}
		setRowCount(row);
	}
	bool id_row = select_row; // is row nr 1 the id_row?
	if(id_row == false && RowCount > 1) {select_row = 1;}
	if(OnAfterRefresh) {OnAfterRefresh(this,select_row,id_row);}
	if(0 <= select_row && select_row < RowCount) {
		if(top_row > RowCount-VisibleRowCount) {
			// VisibleRowCount is always >= 1
			top_row = RowCount-VisibleRowCount;
		}
		// restore old scrollbar position and select.
		TopRow = top_row; Row = select_row; gotoRow(Row);
	} else {throw Exception("DataRows::refresh, invalid SelectRow");}
}
//---------------------------------------------------------------------------

void __fastcall TDataRows::readRow(bool compare) {
	if(editingRow()) {
		throw Exception("DataRows::readRow, invalid RowState");
	}
	if(rowSelected() == false) {
		throw Exception("DataRows::readRow, cannot read selection");
	}
	clearRowState(); Query * query = NULL; id_t id = getId(Row);
	if(OnBeforeReadRow) {OnBeforeReadRow(this,query,id,compare);}
	if(query) {
		if(query->next()) {
			FRowState = rsPresent;
			for(int i=0;i != Meta->Count;i++) {
				AnsiString meta = Meta->Strings[i];
				Field field = query->fieldByName(meta);
				if(field.col) {
					AnsiString data = field.toString();
					AnsiString cell = data2cell(i,data);
					if(compare && cell != Cells[i][Row]) {
						SETFLAG(i,METACHANGED);
						FRowState = rsChanged;
					}
					Cells[i][Row] = cell;
				} // else: field not present, do not compare/update cell
			}
			if(query->next()) {
				throw Exception(
					"DataRows::readRow, result set returned multiple rows");
			}
		} else {FRowState = rsEmpty; clearRow(Row);}
		InvalidateRow(Row); // if invisible columns are updated
	} // else: compare is false, key changed after update?
	if(OnAfterReadRow) {OnAfterReadRow(this,id,compare);}
}
//---------------------------------------------------------------------------

void __fastcall TDataRows::showCaret(void) {
#ifdef _DEBUG
	LOGFUNC();
#endif
	if(Options.Contains(goEditing)) {
		if(InplaceEditor->SelStart > InplaceEditor->Text.Length()) {
			InplaceEditor->SelStart = InplaceEditor->Text.Length();
		}
		caret_timer->Enabled = true;
		// disabled: to select all text, just use the VK_RETURN key
		// int start = InplaceEditor->SelStart;
		// if(start + InplaceEditor->SelLength > InplaceEditor->Text.Length()) {
		//	InplaceEditor->SelLength = InplaceEditor->Text.Length() - start;
		// }
		// InplaceEditor->Move(CellRect(Col,Row));
	}
}
//---------------------------------------------------------------------------

void __fastcall TDataRows::SetEditorMode(bool Value) {
	if(Value) {
		if(Options.Contains(goEditing) == false) {
			int i=0; for(;i != Meta->Count && !TSTFLAG(i,METASELECT);i++);
			if(i == Meta->Count) {
				throw Exception("DataRows::SetEditorMode, no selectable col");
			}
			TStringGrid::Options >> goRowSelect;
			TStringGrid::Options << goAlwaysShowEditor;
			if(Meta->Count > 1) {Col = 1; Col = 0; Col = i;}
			// Set Col before updating goEditing to
			// prevent TStringGrid::SelectCell call
			TStringGrid::Options << goEditing;
		}
		SetFocus();
		TStringGrid::EditorMode = true;
		showCaret();
	} else {
		TStringGrid::EditorMode = false;
		TStringGrid::Options >> goEditing;
		TStringGrid::Options >> goAlwaysShowEditor;
		TStringGrid::Options << goRowSelect;
		if(Meta->Count > 1) {Col = 1; Col = 0;} // select entire row again
	}
}
//---------------------------------------------------------------------------

TRect __fastcall TDataRows::NewPage(void) {
	TPrinter * printer = Printer();
	int top = printer->PageHeight/20;
	int left = printer->PageWidth/20;
	TRect page(left,top,
		printer->PageWidth-left,
		printer->PageHeight-top);
	if(OnNewPage) {OnNewPage(this,page);}
	return page;
}
//---------------------------------------------------------------------------

void __fastcall TDataRows::printSelection(int copies) {
	TPrinter * printer = Printer();
	try {
		printer->BeginDoc();
		while(copies-- > 0) {
			TRect page = NewPage();
			printRow(0,page);
			int start = Selection.Top;
			if(start == 0) {start = 1;}
			int end = Selection.Bottom+1;
			for(int row=start;row < end;row++) {
				printRow(row,page);
			}
			if(OnEndDoc) {OnEndDoc(this,page);}
			if(copies) {printer->NewPage();}
		}
		printer->EndDoc();
	} catch(Exception & e) {printer->Abort(); throw;}
}
//---------------------------------------------------------------------------

void __fastcall TDataRows::printRow(int row,TRect & page) {
	if(row < 0 || row >= RowCount) {
		throw Exception("DataRows::PrintRow, invalid row");
	}

	if(OnPrintRow) {
		bool can_print = true;
		OnPrintRow(this,Row,page,can_print);
		// allows for skipping rows or adding
		// space between rows (update page.top)
		if(can_print == false) {return;}
	}

	// print an invoice row
	TPrinter * printer = Printer();
	TCanvas * canvas = printer->Canvas;
	int col_left = page.left; int new_top = page.top;
	double stretch = ((double) page.Width())/(Width+1);
	for(int col=0;col != Labels->Count;col++) {
		TRect row_rect = page;
		row_rect.top += PrintCellPadding;
		row_rect.left = col_left + PrintCellPadding;
		int col_width = stretch * ColWidths[col];
		row_rect.right = col_left + col_width - PrintCellPadding;
		if(DefaultPrinting && row == 0) {
			canvas->Font->Style = TFontStyles() << fsBold;
			// Set canvas properties before calling OnPrintCell
			// so canvas->TextWidth() produces the right value.
		}
		if(OnPrintCell) {OnPrintCell(this,col,row,row_rect);}
		if(DefaultPrinting) {
			row_rect.top = wrapText(Cells[col][row],canvas,row_rect);
			if(row == 0) {canvas->Font->Style = TFontStyles();}
			row_rect.top += PrintCellPadding;
		}
		if(row_rect.top > new_top) {new_top = row_rect.top;} // find max
		col_left += col_width;
	}

	// print vertical lines
	col_left = page.left;
	for(int col=0;col != Labels->Count;col++) {
		canvas->MoveTo(col_left,page.top+1);
		canvas->LineTo(col_left,new_top-1);
		col_left += stretch * ColWidths[col];
	}
	// last vertical line of this row
	canvas->MoveTo(col_left,page.top+1);
	canvas->LineTo(col_left,new_top-1);

	// horizontal line before printing row
	if(row == 0) {
		canvas->MoveTo(page.left,page.top);
		canvas->LineTo(col_left,page.top);
	}

	// horizontal line after printing row
	canvas->MoveTo(page.left,new_top);
	canvas->LineTo(col_left,new_top);

	// undo print if the bottom of the page
	// was reached. redo on the next page.
	bool new_page = new_top > page.Bottom;
	if(new_page) {
		static bool retry_row = true;
		new_page = retry_row;
		if(OnEndPage) {OnEndPage(this,page,new_page);}
		if(retry_row == false) {new_page = false;}
		if(new_page) {
			retry_row = false;
			try {
				TRect clear_rect(0,page.top+1,
					printer->PageWidth,
					printer->PageHeight);
				canvas->FillRect(clear_rect);
				printer->NewPage(); page = NewPage();
				printRow(0,page); printRow(row,page);
			} __finally {retry_row = true;}
		}
	}
	if(new_page == false) {page.top = new_top;}
}
//---------------------------------------------------------------------------

int __fastcall TDataRows::wrapText(AnsiString text,TCanvas * canvas,TRect rect) {
	int last_break = 0; AnsiString word,line,rect_text;
	int bottom = rect.bottom; rect.bottom = rect.top;
	for(int i=1;i <= text.Length();i++) {
		if(text[i] == ' ' || text[i] == '\n') {
			last_break = i;
			rect_text += word+' '; word = "";
		} else if(iscntrl(text[i])) {continue;}
		else {word += text[i];} line += text[i];
		if(canvas->TextWidth(line) > rect.Width() || text[i] == '\n') {
			if(!last_break) {rect_text += word; word = "";}
			int next = canvas->TextHeight(rect_text) + canvas->TextHeight("/");
			if(last_break && (rect.bottom + next) <= bottom) {i = last_break;}
			else if(last_break) {rect_text += word; word = "";}
			if(i != last_break) {i--;}
			rect.bottom += canvas->TextHeight(rect_text)-2; // -2 add
			if(rect.bottom <= bottom) {
				rect_text.SetLength(rect_text.Length()-1);
				SetBkMode(canvas->Handle,TRANSPARENT);
				ExtTextOut(canvas->Handle,rect.left,rect.top-2,ETO_CLIPPED,
					&rect,rect_text.c_str(),rect_text.Length(),NULL); // -2 add
				rect.top = rect.bottom; last_break = 0;
				word = line = rect_text = "";
			} else {break;}
		}
	}
	rect_text += word; rect.bottom = bottom;
	SetBkMode(canvas->Handle,TRANSPARENT);
	ExtTextOut(canvas->Handle,rect.left,rect.top-2,ETO_CLIPPED,
		&rect,rect_text.c_str(),rect_text.Length(),NULL); // -2 add
	return rect.top + canvas->TextHeight(rect_text);
}
//---------------------------------------------------------------------------

void __fastcall TDataRows::KeySearch(Word &Key,int ACol) {
	char curr_key = tolower(Key);
	TDateTime curr_time = Time();
	int start_row = Row;
	static const TDateTime interval = TDateTime(0,0,1,0);
	if((key_buffer.Length() <= 1 && curr_key == prev_key) ||
		curr_time >= prev_time+interval) {
		key_buffer.SetLength(0); start_row++;
	}
	key_buffer += curr_key;
	prev_time = curr_time;
	prev_key = curr_key;

	int row = start_row;
	for(;row != RowCount;row++) {
		AnsiString cell = Cells[ACol][row].LowerCase();
		cell.SetLength(key_buffer.Length());
		if(cell == key_buffer) {
			Row = row;
			break;
		}
	}
	if(row == RowCount) {
		for(row=1;row != RowCount;row++) {
			AnsiString cell = Cells[ACol][row].LowerCase();
			cell.SetLength(key_buffer.Length());
			if(cell == key_buffer) {
				Row = row;
				break;
			}
		}
	}
	if(row == RowCount) {Beep();}
}
//---------------------------------------------------------------------------

