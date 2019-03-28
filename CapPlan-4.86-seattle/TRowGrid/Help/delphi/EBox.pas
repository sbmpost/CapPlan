//$Id: EBox.pas,v 1.6 2011/10/21 21:48:35 darseq Exp $
unit EBox;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, OleCtrls, RowGrid_TLB;

type
  TForm1 = class(TForm)
    Memo1: TMemo;
    RowGridX1: TRowGridX;
    procedure RowGridX1MouseDown(Sender: TObject; MouseButton: TOleEnum;
      ShiftState: Byte; X, Y: Integer);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure RowGridX1ClearRowCell(Sender: TObject;
      const RowCellX: IRowCellX; var CanClear: WordBool);
    procedure RowGridX1Click(Sender: TObject);
    procedure RowGridX1DblClickRowCell(Sender: TObject;
      const RowCellX: IRowCellX);
    procedure RowGridX1InsertRowCell(Sender: TObject;
      const RowCellX: IRowCellX; var CanInsert: WordBool);
    procedure RowGridX1DrawRowCell(Sender: TObject;
      const RowCellX: IRowCellX; const RectX: IRectX);
    procedure RowGridX1DragRowCell(Sender: TObject;
      const RowCellX: IRowCellX; CellState: TOleEnum; IsCopy: WordBool;
      var CanDrag: WordBool);
    procedure RowGridX1KeyDown(Sender: TObject; var Key: OleVariant;
      ShiftState: Byte);
    procedure Memo1KeyDown(Sender: TObject; var Key: Word;
      Shift: TShiftState);
  private
    click_x,click_y: Integer;
    clicked_cell: IRowCellX;
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

implementation

{$R *.dfm}

var selectedCell: IRowCellX;
var selected_dragged: boolean;

procedure TForm1.FormCreate(Sender: TObject);
begin
    Memo1.Visible := false;
end;

procedure TForm1.FormDestroy(Sender: TObject);
var count: Integer;
begin
    RowGridX1.ClearDragState();
    // RowGridX1.RowCount := 0; would work too
    for count := 0 to RowGridX1.RowCount-1 do
        RowGridX1.ClearRow(count);
end;

procedure TForm1.RowGridX1ClearRowCell(Sender: TObject;
  const RowCellX: IRowCellX; var CanClear: WordBool);
var s: TStringList;
begin
    // CanClear defaults to false when using ClearRow
    CanClear := true;
    RowCellX.Get_ObjectPtr(Pointer(s));
    s.Destroy();
end;

procedure TForm1.RowGridX1MouseDown(Sender: TObject; MouseButton: TOleEnum;
  ShiftState: Byte; X, Y: Integer);
var rowCell: IRowCellX;
    oldCell: IRowCellX;
    cellState: TxCellState;
    equal: WordBool;
begin
    click_x := X; click_y := Y;
    if(RowGridX1.DragState = dsNone) then
    begin
        // find clicked cell
        RowGridX1.MouseToCell(rowCell, X, Y, cellState);
        if(cellState <> csNone) and (cellState <> csEmpty) then
        begin
            // check if this cell was clicked earlier
            equal := false;
            if(selectedCell <> nil) then rowCell.equals(equal,selectedCell);
            if(equal = false) then
            begin
                // deselect the previous cell and select the clicked cell
                oldCell := selectedCell; selectedCell := rowCell;
                if(oldCell <> nil) then RowGridX1.PaintCell(oldCell,false);
                RowGridX1.PaintCell(selectedCell,false);
            end;
        end;
    end;
end;

procedure TForm1.RowGridX1KeyDown(Sender: TObject; var Key: OleVariant;
  ShiftState: Byte);
var s: TStringList;
    my_key: Word;
begin
    my_key := VarType(Key); // key stored in type field??!
    if(my_key = VK_DELETE) then
    begin
        if(selectedCell <> nil) then
        begin
            // remove selected cell from the grid
            selectedCell.Get_ObjectPtr(Pointer(s));
            s.Destroy();
            RowGridX1.RemoveRowCell(selectedCell);
            RowGridX1.PaintCell(selectedCell,true);
            selectedCell := nil;
        end;
    end;
end;

procedure TForm1.Memo1KeyDown(Sender: TObject; var Key: Word;
  Shift: TShiftState);
begin
    if(Key = VK_RETURN) then RowGridX1Click(Sender);
    if(Key = VK_ESCAPE) then Memo1.Visible := false;
end;

procedure TForm1.RowGridX1Click(Sender: TObject);
var s: TStringList;
begin
    if(Memo1.Visible) then
    begin
        // update the text of clicked cell
        clicked_cell.Get_ObjectPtr(Pointer(s));
        s.Clear(); s.AddStrings(Memo1.Lines);
        // hide Memo and repaint the clicked cell
        Memo1.Visible := false;
        RowGridX1.PaintCell(clicked_cell,false);
        RowGridX1.SetFocus();
    end;
end;

procedure TForm1.RowGridX1DblClickRowCell(Sender: TObject;
  const RowCellX: IRowCellX);
var s: TStringList;
begin
    // show memo near the double clicked cell
    clicked_cell := RowCellX;
    Memo1.Left := RowGridX1.Left + click_x - Memo1.Width div 2;
    Memo1.Top := RowGridX1.Top + click_y;
    // show text of clicked cell in the memo
    Memo1.Lines.Clear();
    clicked_cell.Get_ObjectPtr(Pointer(s));
    Memo1.Lines.AddStrings(s);
    Memo1.Visible := true;
    Memo1.SetFocus();
end;

procedure TForm1.RowGridX1DragRowCell(Sender: TObject;
  const RowCellX: IRowCellX; CellState: TOleEnum; IsCopy: WordBool;
  var CanDrag: WordBool);
var s,t: TStringList;
    equal: WordBool;
begin
    // Is the cell being dragged the selected one?
    // If so, then selectedCell must be set to nil
    if(selectedCell <> nil) then
    begin
        if(IsCopy = false) then
        begin
            RowCellX.equals(equal,selectedCell);
            if(equal) then
            begin
                selectedCell := nil;
                selected_dragged := true;
            end;
        end;
    end;

    // If this is a copied cell, then we need a newly
    // allocated object that is a copy of the original
    if(IsCopy = true) then
    begin
        RowCellX.Get_ObjectPtr(Pointer(t));
        s := TStringList.Create(); s.AddStrings(t);
        RowCellX.Set_ObjectPtr(Pointer(s));
    end;
end;

procedure TForm1.RowGridX1InsertRowCell(Sender: TObject;
  const RowCellX: IRowCellX; var CanInsert: WordBool);
var s: TStringList;
begin
    // Allocate a new object each time a new cell is inserted. If
    // the cell was already there, the object is already allocated.
    RowCellX.Get_ObjectPtr(Pointer(s));
    if(RowGridX1.DragState = dsInsert) then s := TStringList.Create();
    RowCellX.Set_ObjectPtr(Pointer(s));
    // restore selectedcell if it was being dragged
    if(selected_dragged) then selectedCell := RowCellX;
    selected_dragged := false;
end;

procedure TForm1.RowGridX1DrawRowCell(Sender: TObject;
  const RowCellX: IRowCellX; const RectX: IRectX);
var
    s: TStringList;
    equal: WordBool;
    left,top,right,bottom: Integer;
    canvas: TCanvas; c_handle: Pointer;
    c_rect: TRect;
begin
    // get a canvas rect that belongs to this cell
    RectX.Get_Left(left); c_rect.Left := left;
    RectX.Get_Top(top); c_rect.Top := top;
    RectX.Get_Right(right); c_rect.Right := right;
    RectX.Get_Bottom(bottom); c_rect.Bottom := bottom;

    canvas := TCanvas.Create();
    c_handle := RowGridX1.HDC;
    canvas.Handle := HDC(c_handle);
    canvas.Brush.Color := RowGridX1.CellColor;

    // set a different cell color for selected cell
    if(selectedCell <> nil) then
    begin
        RowCellX.equals(equal,selectedCell);
        if(equal) then canvas.Brush.Color := clAqua;
    end;

    // draw text to the canvas rect of this cell
    RowCellX.Get_ObjectPtr(Pointer(s));
    canvas.TextRect(c_rect,c_rect.left,c_rect.top,Trim(s.Text));
    canvas.Destroy();
end;

end.
