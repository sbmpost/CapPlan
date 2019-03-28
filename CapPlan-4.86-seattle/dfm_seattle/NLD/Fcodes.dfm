inherited Fcodes1: TFcodes1
  Left = 449
  Top = 132
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Kleuren'
  ClientHeight = 471
  ClientWidth = 466
  Color = clSkyBlue
  Menu = MainMenu1
  OldCreateOrder = False
  OnDestroy = FormDestroy
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 16
  inherited Panel1: TPanel
    TabOrder = 1
  end
  object CodeRows: TDataRows
    Left = 5
    Top = 10
    Width = 456
    Height = 455
    ColCount = 3
    DefaultRowHeight = 18
    DefaultDrawing = False
    DoubleBuffered = True
    RowCount = 2
    ParentDoubleBuffered = False
    ScrollBars = ssVertical
    TabOrder = 0
    OnDblClick = CodeRowsDblClick
    OnDrawCell = CodeRowsDrawCell
    OnBeforeRefresh = CodeRowsBeforeRefresh
    OnBeforeReadRow = CodeRowsBeforeReadRow
    OnAfterReadRow = CodeRowsAfterReadRow
    OnStoreRow = CodeRowsStoreRow
    OnDataToCell = CodeRowsDataToCell
    OnCellToData = CodeRowsCellToData
    OnPrintCell = CodeRowsPrintCell
    MetaData = 'CODE_NAME,APP_CHARGE,CODE_COLOR'
    MetaLabels = 'CODE,TARIEF,KLEUR'
    MetaRowId = 'CODE_NAME'
    MetaSelect = 'CODE_NAME,APP_CHARGE'
    ColWidths = (
      150
      150
      150)
  end
  object MainMenu1: TMainMenu
    Left = 360
    Top = 65520
    object new1: TMenuItem
      Caption = '&Nieuw'
      OnClick = new1Click
    end
    object edit1: TMenuItem
      Caption = '&Bewerken'
      object modify1: TMenuItem
        Caption = '&Bewerk'
        ShortCut = 113
        OnClick = modify1Click
      end
      object delete1: TMenuItem
        Caption = '&Verwijder'
        ShortCut = 16452
        OnClick = delete1Click
      end
      object N1: TMenuItem
        Caption = '-'
      end
      object selectAll1: TMenuItem
        Caption = '&Selecteer Alles'
        ShortCut = 16449
        OnClick = selectAll1Click
      end
      object export1: TMenuItem
        Caption = '&Exporteer Csv'
        ShortCut = 16453
        OnClick = export1Click
      end
      object print1: TMenuItem
        Caption = '&Print Ingangen'
        ShortCut = 16464
        OnClick = print1Click
      end
    end
    object colors1: TMenuItem
      Caption = '&Kleuren'
      OnClick = colors1Click
    end
  end
  object ColorDialog1: TColorDialog
    Left = 328
    Top = 65520
  end
end
