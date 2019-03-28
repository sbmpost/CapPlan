object Form1: TForm1
  Left = 244
  Top = 132
  Width = 377
  Height = 539
  Caption = '(c) PostWare 2004-2012'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object RowGrid1: TRowGrid
    Left = 8
    Top = 8
    Width = 354
    Height = 169
    Units = 14
    RowCount = 5
    RowHeight = 20
    RowLineWidth = 10
    UnitWidth = 25
    AltKeyInserts = True
    OnInsertRowCell = RowGrid1InsertRowCell
    OnRemoveRowCell = RowGrid1RemoveRowCell
    OnClearRowCell = RowGrid1ClearRowCell
    OnDragRowCell = RowGrid1DragRowCell
    OnDrawRowCell = RowGrid1DrawRowCell
    OnDblClickRowCell = RowGrid1DblClickRowCell
    OnPaint = RowGrid1Paint
    TabOrder = 0
    ParentShowHint = False
    OnMouseMove = RowGrid1MouseMove
  end
  object Memo1: TMemo
    Left = 8
    Top = 368
    Width = 353
    Height = 134
    ReadOnly = True
    TabOrder = 1
  end
  object Button1: TButton
    Left = 280
    Top = 248
    Width = 75
    Height = 25
    Caption = 'VerticalRows'
    TabOrder = 2
    OnClick = Button1Click
  end
end
