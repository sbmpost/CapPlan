//$Id: DataRows.h,v 1.48 2013/12/21 20:00:16 darseq Exp $
#ifndef DataRowsH
#define DataRowsH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <Grids.hpp>
#include "../defines.h"
#include "../Query.h"
#ifdef _DEBUG
#include "../Exceptions.h"
#endif
//---------------------------------------------------------------------------
#include <strutils.hpp>

#if __BORLANDC__ <= 0x582
typedef char char_t;
#else
typedef WideChar char_t;
#define ListSeparator FormatSettings.ListSeparator
#endif

namespace Datarows { // same namespace as register
#define METASELECT 0x1
#define METACHANGED 0x2
#define SETFLAG(i,flag) \
	Meta->Objects[i] = (TObject *) ((int) Meta->Objects[i] | flag)
#define CLRFLAG(i,flag) \
	Meta->Objects[i] = (TObject *) ((int) Meta->Objects[i] & ~flag)
#define TSTFLAG(i,flag) ((int) Meta->Objects[i] & flag)

typedef Query * QueryPtr;
typedef void __fastcall (__closure * TBeforeRefreshEvent)
	(TObject* Sender,QueryPtr & ResultSet,id_t & Id);
typedef void __fastcall (__closure * TAfterRefreshEvent)
	(TObject* Sender,int & SelectRow,bool IsIdRow);
typedef void __fastcall (__closure * TBeforeReadRowEvent)
	(TObject* Sender,QueryPtr & ResultSet,id_t Id,bool & Compare);
typedef void __fastcall (__closure * TAfterReadRowEvent)
	(TObject* Sender,id_t Id,bool Compare);
typedef void __fastcall (__closure * TStoreRowEvent)
	(TObject* Sender,id_t Id);
typedef void __fastcall (__closure * TDataToCellEvent)
	(TObject* Sender,int meta,AnsiString & data);
typedef void __fastcall (__closure * TCellToDataEvent)
	(TObject* Sender,int meta,AnsiString & cell);
typedef void __fastcall (__closure * TPrintRowEvent)
	(TObject* Sender,long ARow,TRect & Rect,bool & CanPrint);
typedef void __fastcall (__closure *TPrintCellEvent)
	(TObject* Sender,long ACol,long ARow,TRect & Rect);
typedef void __fastcall (__closure *TNewPageEvent)
	(TObject* Sender,TRect & Rect);
typedef void __fastcall (__closure *TEndPageEvent)
	(TObject* Sender,TRect & Rect,bool & NewPage);
typedef void __fastcall (__closure *TEndDocEvent)
	(TObject* Sender,TRect & Rect);

typedef enum {rsEditing,rsChanged,rsPresent,rsEmpty,rsNone} TRowState;
class PACKAGE TDataRows : public TStringGrid {
private:
	TBeforeRefreshEvent FOnBeforeRefresh;
	TAfterRefreshEvent FOnAfterRefresh;
	TBeforeReadRowEvent FOnBeforeReadRow;
	TAfterReadRowEvent FOnAfterReadRow;
	TStoreRowEvent FOnStoreRow;
	TDataToCellEvent FOnDataToCell;
	TCellToDataEvent FOnCellToData;
	TPrintRowEvent FOnPrintRow;
	TPrintCellEvent FOnPrintCell;
	TNewPageEvent FOnNewPage;
	TEndPageEvent FOnEndPage;
	TEndDocEvent FOnEndDoc;

	bool FDefaultPrinting;
	int FPrintCellPadding;
	int FKeySearchColumn;
	TStringList * FMeta;
	TStringList * FLabels;
	TStringList * FRowId;
	TStringList * FSelect;
	TTimer * caret_timer;
	TRowState FRowState;
	id_t editrow_id;
	TDateTime prev_time;
	AnsiString key_buffer;
	char prev_key;

	void __fastcall ShowCaretTimer(TObject *Sender);
	AnsiString __fastcall GetMeta(void) {return Meta->CommaText;}
	void __fastcall SetMeta(AnsiString meta) {
		Meta->CommaText = meta;
		MetaSelect = MetaSelect;
		TStringGrid::ColCount = Meta->Count;
		clearRowState();
	}
	AnsiString __fastcall GetLabels(void) {return Labels->CommaText;}
	void __fastcall SetLabels(AnsiString labels) {
		Labels->CommaText = labels;
		for(int i=0;i != ColCount && i != Labels->Count;i++) {
			Cells[i][0] = Labels->Strings[i]; // RowCount is always >= 1
		}
	}
	AnsiString __fastcall GetSelect(void) {return Select->CommaText;}
	void __fastcall SetSelect(AnsiString select) {
		Select->CommaText = select;
		for(int i=0;i != Meta->Count;i++) {CLRFLAG(i,METASELECT);}
		for(int i=0;i != Select->Count;i++) {
			int index = Meta->IndexOf(Select->Strings[i]);
			if(index != -1) {SETFLAG(index,METASELECT);}
		}
	}
	AnsiString __fastcall GetRowId(void) {return RowId->CommaText;}
	void __fastcall SetRowId(AnsiString id) {RowId->CommaText = id;}
	void __fastcall SetEditorMode(bool Value);
	int __fastcall GetFixedCols(void) {return TStringGrid::FixedCols;}
	int __fastcall GetFixedRows(void) {return TStringGrid::FixedRows;}
	bool __fastcall GetEditorMode(void) {return TStringGrid::EditorMode;}
	TGridOptions __fastcall GetOptions(void) {return TStringGrid::Options;}
	// Supply a copy of Options so another class cannot change its values
	int __fastcall getColCount(void) {return TStringGrid::ColCount;}
	void __fastcall setColCount(int Value) {
		// Must be writeable so columns
		// can be resized in design mode
		TStringGrid::ColCount = Value;
		if(KeySearchColumn > ColCount) {KeySearchColumn = 0;}
	}
	int __fastcall getRowCount(void) {return TStringGrid::RowCount;}
	void __fastcall setRowCount(int Value) {
		TStringGrid::RowCount = Value; // RowCount is always >= 1
		if(TStringGrid::RowCount > 1) {TStringGrid::FixedRows = 1;}
		else {TStringGrid::FixedRows = 0;}
	}
	virtual bool __fastcall SelectCell(int ACol, int ARow) {
		if(Row != ARow) {
			if(editingRow()) {storeRow();}
			else {clearRowState();} // reset changed when the user scrolls
			// We do not editRow(ARow) because we do not want to readRows
			// each time the user scrolls but only after the user hits F2.
		} else if(Options.Contains(goEditing)) {
			if(!TSTFLAG(ACol,METASELECT)) {
				int direction = ACol > Col ? +1 : -1;
				for(int i=ACol;i != -1 && i != Meta->Count;i=i+direction) {
					// When setting Col we get a recursive call. A
					// little bit error prone but it seems to work.
					if(TSTFLAG(i,METASELECT)) {Col = i; break;}
				}
				return false;
			}
		} else if(editingRow()) {return true;}
		return TStringGrid::SelectCell(ACol,ARow);
	}
	void __fastcall showCaret(void);
	void __fastcall setPrintCellPadding(int Value) {
		if(Value >= 0) {FPrintCellPadding = Value;}
	}
	void __fastcall setKeySearchColumn(int Value) {
		if(Value >= 0 && Value <= ColCount) {
			FKeySearchColumn = Value;
		} else {FKeySearchColumn = 0;}
	}
	TRect __fastcall NewPage(void);
	void __fastcall exportRow(int row,TStream * stream) {
		AnsiString line;
		for(int col=0;col != Labels->Count;col++) {
			if(col) {line += ListSeparator;}
			line += AnsiQuotedStr(AnsiReplaceStr(
				Cells[col][row],"\r",""),'\"');
		}
		line += "\n";
		stream->Write(line.c_str(),line.Length());
	}
protected:
public:
	__fastcall TDataRows(TComponent* Owner);
	__fastcall ~TDataRows(void);
	void __fastcall printSelection(int copies = 1);
	void __fastcall printRow(int row,TRect & page);
	void __fastcall exportSelection(TStream * stream) {
		int start = Selection.Top;
		if(start == 0) {start = 1;}
		int end = Selection.Bottom+1;
		exportRow(0,stream);
		for(int row=start;row < end;row++) {
			exportRow(row,stream);
		}
	}
	static int __fastcall wrapText(AnsiString text,TCanvas * canvas,TRect rect);
	__inline void __fastcall setRowColor(TGridDrawState State,
		TColor clfix = TColor(clBtnFace),TColor brush = TColor(clWhite),
		TColor font = TColor(clBlack),TFontStyles style = TFontStyles(),
		bool highlight = false) {
		Canvas->Font->Style = style;
		if(State.Contains(gdFixed)) {
			Canvas->Brush->Color = clfix;
			Canvas->Font->Color = clBlack;
		} else if(State.Contains(gdSelected)) {
			if(brush == clWhite || highlight) {
				Canvas->Brush->Color = clHighlight;
			} else {
				Canvas->Brush->Color = brush;
				font = clBlack;
			}
			if(font == clBlack) {
				Canvas->Font->Color = clWhite;
			} else {Canvas->Font->Color = font;}
		} else {
			Canvas->Brush->Color = brush;
			Canvas->Font->Color = font;
		}
	}
	__inline AnsiString __fastcall cell2data(int meta,AnsiString cell) {
		if(OnCellToData) {OnCellToData(this,meta,cell);} return cell;
	}
	__inline AnsiString __fastcall data2cell(int meta,AnsiString data) {
		if(OnDataToCell) {OnDataToCell(this,meta,data);} return data;
	}
	__inline id_t __fastcall getId(int row) {
		if(row <= 0 || row >= RowCount) {
			throw Exception("DataRows::getId, invalid row");
		}

		AnsiString id;
		if(RowId->Count) {
			for(int i=0;i != RowId->Count;i++) {
				int meta = Meta->IndexOf(RowId->Strings[i]);
				if(meta == -1) {
					throw Exception("DataRows::getId, could not find id column "+
						RowId->Strings[i]);
				}
				AnsiString data = getData(meta,row);
				if(data == UNDEFINED) {id = UNDEFINED; break;}
				if(i) {id += ",";} id += data;
			}
		} else {id = UNDEFINED;}
		return id;
	}
	__inline AnsiString __fastcall getData(int meta,int row) {
		return cell2data(meta,Cells[meta][row]);
	}
	__inline void __fastcall setData(AnsiString data,int meta,int row) {
		Cells[meta][row] = data2cell(meta,data);
	}
	__inline bool __fastcall editingRow(void) {
		return RowState == rsEditing;
	}
	__inline void __fastcall gotoRow(int row) {
		if(row < 0) {row = 0;}
		if(row >= RowCount) {row = RowCount-1;}
		if(row && row < TopRow) {TopRow = row;}
		else if(TopRow < row-VisibleRowCount+1) {
			TopRow = row-VisibleRowCount+1;
		}
	}
	__inline int __fastcall rowSelected(void) {
		if(Selection.Top != Selection.Bottom) {
			TGridRect sel = {ColCount-1,Row,0,Row};
			Selection = sel; gotoRow(Row);
		}
		return Row;
	}
	__inline bool __fastcall selectAll(void) {
		bool ok = editingRow() == false &&
			ColCount > 0 && RowCount > 1;
		if(ok) {
			if(Row != 1) {SelectCell(0,1);}
			TGridRect sel = {ColCount-1,RowCount-1,0,1};
			Selection = sel;
		}
		return ok;
	}
	void __fastcall clearRow(int row) {
		if(row < 0 || row >= RowCount) {
			throw Exception("DataRows::clearRow, invalid row");
		}
		for(int i=0;i != Meta->Count;i++) {
			setData(cell2data(i,UNDEFINED),i,row);
		}
	}
	int __fastcall addRow(void) {
		int row = RowCount;
		setRowCount(row+1);
		clearRow(row);
		return row;
	}
	void __fastcall clearRowState(void) {
#ifdef _DEBUG
		LOGFUNC();
#endif
		for(int i=0;i != Meta->Count;i++) {CLRFLAG(i,METACHANGED);}
		// Do not depend on the value of EditorMode: see storeRow.
		if(editingRow()) {SetEditorMode(false);} FRowState = rsNone;
	}
	void __fastcall clear(void) {
#ifdef _DEBUG
		LOGFUNC();
#endif
		clearRowState();
		setRowCount(1);
	}
	void __fastcall refreshNext(void) {
		int row = Row+1;
		id_t row_id = UNDEFINED;
		for(;row != RowCount &&
			(row_id = getId(row)) ==
			UNDEFINED;row++);
		if(row == RowCount) {
			// try current row first
			for(row = Row;row &&
				(row_id = getId(row)) ==
				UNDEFINED;row--);
		}
		refresh(row_id);
	}
	void __fastcall refresh(id_t id);
	void __fastcall readRow(bool compare);
	void __fastcall editRow(void) {
		if(editingRow()) {
			throw Exception("DataRows::editRow, invalid RowState");
		}
		if(rowSelected() == false) {
			throw Exception("DataRows::editRow, cannot edit selection");
		}
		int i=0; for(;i != Meta->Count && !TSTFLAG(i,METASELECT);i++);
		if(i == Meta->Count) {return;} // when there is no selectable column.
		readRow(true); editrow_id = getId(Row);
		FRowState = rsEditing; SetEditorMode(true);
	}
	void __fastcall editCol(int col) {
		if(col < 0 || col >= ColCount) {
			throw Exception("DataRows::editCol, invalid col");
		}
		if(editingRow() == false) {editRow();}
		if(editingRow() && TSTFLAG(col,METASELECT)) {
			Col = col; SetEditorMode(true);
			// SetEditorMode blinks the cursor
		}
	}
	void __fastcall storeRow(void) {
		if(editingRow() == false) {
			// storeRow is not private because the application may
			// wish to bind a key to store the row. When not editing,
			// storeRow can/should not be called however.
			throw Exception("DataRows::storeRow, invalid RowState");
		}
		if(rowSelected() == false) {
			// Setting the Selection property also changes the Row.
			// This check prevents the wrong row from being stored.
			throw Exception("DataRows::storeRow, cannot store selection");
		}
		// EditorMode = false; // somehow necessary when copy-paste masked field
		clearRowState(); // Always clearRowState for otherwise DoExit() is
		// called over and over again when OnStoreRow throws an Exception
		// If there was a failure, then the edited data is not restored. To
		// do so OnStoreRow could define an error handler and call readRow.
		if(OnStoreRow) {OnStoreRow(this,editrow_id);}
	}
	__property bool EditorMode = {read = GetEditorMode};
	__property TStringList * Meta = {read = FMeta};
	__property TStringList * Labels = {read = FLabels};
	__property TStringList * RowId = {read = FRowId};
	__property TStringList * Select = {read = FSelect};

	DYNAMIC void __fastcall DoExit(void) {
		if(editingRow()) {storeRow();}
		TStringGrid::DoExit();
	}
	DYNAMIC void __fastcall KeyPress(char_t &Key) {
		if(Key == VK_TAB && editingRow()) {SetEditorMode(true);}
		TStringGrid::KeyPress(Key);
	}
	DYNAMIC void __fastcall KeySearch(Word &Key,int ACol);
	DYNAMIC void __fastcall KeyDown(Word &Key,TShiftState Shift) {
		if(Key == VK_TAB && Shift.Contains(ssShift) && Col == 0) {return;}
		if(KeySearchColumn && editingRow() == false && Shift.Empty() &&
			(isalnum(Key) || isspace(Key)) && rowSelected()) {
			KeySearch(Key,KeySearchColumn-1); return;
			// To implement other key filtering conditions
			// or to capture all keys, set KeySearchColumn
			// to 0 and call KeySearch as appropriate.
		}
		TStringGrid::KeyDown(Key,Shift);
	}
	DYNAMIC void __fastcall KeyUp(Word &Key,TShiftState Shift) {
		if(Key == VK_END) {showCaret();} // strip off mask chars
		TStringGrid::KeyUp(Key,Shift);
	}
	DYNAMIC void __fastcall MouseMove(Classes::TShiftState Shift,
		int X, int Y) {
		if(ComponentState.Contains(csDesigning)) {
			TStringGrid::MouseMove(Shift,X,Y);
		} else {TControl::MouseMove(Shift,X,Y);}
	}
	// When the mouse is used to select a range, TCustomGrid::MoveAnchor sets
	// FAnchor.X to ColCount - 1 which causes the grid to shift to the right.
	// Therefore only allow ranges to be set using mouse clicks and shift key.
	// ranges that fall outside the visible part can be set using vk_down key.
	// todo: when the user selects a range using the mouse and clicks
	// a cell in the right bottom corner, the anchor is still moved.
	// have a look at TimedScroll?
	DYNAMIC void __fastcall MouseDown(Controls::TMouseButton Button,
		Classes::TShiftState Shift, int X, int Y) {
		if(Button == mbRight && editingRow() == false &&
			Selection.Top == Selection.Bottom) {
			int ACol,ARow; MouseToCell(X,Y,ACol,ARow);
			if(ARow > 0) {Row = ARow; Col = ACol;}
		}
		TStringGrid::MouseDown(Button,Shift,X,Y);
		KillTimer(Handle,1); // undo the damage TStringGrid has done
	}
	DYNAMIC void __fastcall DoContextPopup(const Types::TPoint &MousePos,
		bool &Handled) {
		TStringGrid::DoContextPopup(MousePos,Handled);
		if(Handled == false && PopupMenu) {
			int x = MousePos.x; int y = MousePos.y;
			if(Row && Selection.Top == Selection.Bottom) {
				TRect cell_rect = CellRect(Col,Row);
				if(x < 0) {x = cell_rect.left+cell_rect.Width()/2;}
				y = cell_rect.top+cell_rect.Height()/2;
			}
			TPoint point(x,y);
			point = ClientToScreen(point);
			PopupMenu->Popup(point.x,point.y);
			Handled = true;
		}
	}
	// Just to be sure, hide non visible columns.
	DYNAMIC void __fastcall ColWidthsChanged(void) {
		// Note that Labels->Count may be larger than ColCount.
		for(int i=Labels->Count;i < ColCount;i++) {ColWidths[i] = 0;}
		TStringGrid::ColWidthsChanged();
	}
__published:
	__property TBeforeRefreshEvent OnBeforeRefresh = {
		read = FOnBeforeRefresh, write = FOnBeforeRefresh
	};
	__property TAfterRefreshEvent OnAfterRefresh = {
		read = FOnAfterRefresh, write = FOnAfterRefresh
	};
	__property TBeforeReadRowEvent OnBeforeReadRow = {
		read = FOnBeforeReadRow, write = FOnBeforeReadRow
	};
	__property TAfterReadRowEvent OnAfterReadRow = {
		read = FOnAfterReadRow, write = FOnAfterReadRow
	};
	__property TStoreRowEvent OnStoreRow = {
		read = FOnStoreRow, write = FOnStoreRow
	};
	__property TDataToCellEvent OnDataToCell = {
		read = FOnDataToCell, write = FOnDataToCell
	};
	__property TCellToDataEvent OnCellToData = {
		read = FOnCellToData, write = FOnCellToData
	};
	__property TPrintRowEvent OnPrintRow = {
		read = FOnPrintRow, write = FOnPrintRow
	};
	__property TPrintCellEvent OnPrintCell = {
		read = FOnPrintCell, write = FOnPrintCell
	};
	__property TNewPageEvent OnNewPage = {
		read = FOnNewPage, write = FOnNewPage
	};
	__property TEndPageEvent OnEndPage = {
		read = FOnEndPage, write = FOnEndPage
	};
	__property TEndDocEvent OnEndDoc = {
		read = FOnEndDoc, write = FOnEndDoc
	};
	__property bool DefaultPrinting = {
		read = FDefaultPrinting, write = FDefaultPrinting, default = true
	};
	__property int PrintCellPadding = {
		read = FPrintCellPadding, write = setPrintCellPadding, default = 10
	};
	__property int KeySearchColumn = {
		read = FKeySearchColumn, write = setKeySearchColumn, default = 1
	};
	__property AnsiString MetaData = {
		read = GetMeta, write = SetMeta
	};
	__property AnsiString MetaLabels = {
		read = GetLabels, write = SetLabels
	};
	__property AnsiString MetaRowId = {
		read = GetRowId, write = SetRowId
		// note that MetaRowId is not required
	};
	__property AnsiString MetaSelect = {
		read = GetSelect, write = SetSelect
	};
	__property TRowState RowState = {read = FRowState};

// default properties (used TStringGrid as a reference)
	__property BorderStyle;
	__property ColCount = {read = getColCount, write = setColCount};
	__property DefaultColWidth;
	__property DefaultDrawing;
	__property DefaultRowHeight;
	__property FixedColor;
	__property int FixedCols = {read = GetFixedCols};
	__property int FixedRows = {read = GetFixedRows};
	__property TGridOptions Options = {read = GetOptions};
	__property int RowCount = {read = getRowCount, write = setRowCount};
	__property ScrollBars;

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
// TStringGrid:
	__property OnColumnMoved;
	__property OnDrawCell;
	__property OnGetEditMask;
	__property OnGetEditText;
	__property OnRowMoved;
	__property OnSelectCell;
	__property OnSetEditText;
	__property OnTopLeftChanged;

// TWinControl:
//	__property OnDockDrop;
//	__property OnDockOver;
	__property OnEnter;
	__property OnExit;
//	__property OnGetSiteInfo;
	__property OnKeyDown;
	__property OnKeyPress;
	__property OnKeyUp;
//	__property OnUnDock;

// TControl:
//	__property OnCanResize;
	__property OnClick;
//	__property OnConstrainedResize;
	__property OnContextPopup;
	__property OnDblClick;
	__property OnDragDrop;
	__property OnDragOver;
	__property OnEndDock;
	__property OnEndDrag;
	__property OnMouseDown;
	__property OnMouseMove;
	__property OnMouseUp;
//	__property OnMouseWheel;
	__property OnMouseWheelDown;
	__property OnMouseWheelUp;
//	__property OnResize;
	__property OnStartDock;
	__property OnStartDrag;
};
//---------------------------------------------------------------------------
} // end namespace Datarows
#if !defined(NO_IMPLICIT_NAMESPACE_USE)
using namespace Datarows;
#endif

#endif

