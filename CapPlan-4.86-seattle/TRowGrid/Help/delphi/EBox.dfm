object Form1: TForm1
  Left = 236
  Top = 116
  Width = 552
  Height = 311
  Caption = 'Copyright by PostWare 2004 - 2008'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object RowGridX1: TRowGridX
    Left = 0
    Top = 0
    Width = 544
    Height = 281
    ParentFont = False
    TabOrder = 0
    OnClick = RowGridX1Click
    OnInsertRowCell = RowGridX1InsertRowCell
    OnDrawRowCell = RowGridX1DrawRowCell
    OnDragRowCell = RowGridX1DragRowCell
    OnDblClickRowCell = RowGridX1DblClickRowCell
    OnMouseDown = RowGridX1MouseDown
    OnKeyDown = RowGridX1KeyDown
    OnClearRowCell = RowGridX1ClearRowCell
    ControlData = {
      545046300854526F774772696400055769647468032002064865696768740319
      010B436C69656E745769647468031C020C436C69656E74486569676874031501
      05556E697473022408526F77436F756E74020A09526F7748656967687402140C
      526F774C696E655769647468020A09556E69745769647468020F0000}
  end
  object Memo1: TMemo
    Left = 64
    Top = 128
    Width = 185
    Height = 89
    Lines.Strings = (
      'Memo1')
    TabOrder = 1
    OnKeyDown = Memo1KeyDown
  end
end
