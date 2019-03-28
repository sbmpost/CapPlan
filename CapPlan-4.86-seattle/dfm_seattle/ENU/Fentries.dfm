inherited Fentries1: TFentries1
  Left = 218
  Top = 23
  Caption = 'Entries'
  ClientHeight = 678
  Ctl3D = False
  Menu = WorkMenu
  OldCreateOrder = False
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 16
  inherited RowControl: TRowControl
    ActivePage = ACTIVE
    object ACTIVE: TTabSheet
      Caption = 'Active'
      object CurrentRows: TDataRows
        Left = 5
        Top = 8
        Width = 977
        Height = 628
        ColCount = 6
        DefaultRowHeight = 18
        DefaultDrawing = False
        DoubleBuffered = True
        RowCount = 2
        ParentDoubleBuffered = False
        PopupMenu = PopupMenu1
        ScrollBars = ssVertical
        TabOrder = 0
        OnDblClick = DataRowsDblClick
        OnDrawCell = DataRowsDrawCell
        OnBeforeRefresh = DataRowsBeforeRefresh
        OnDataToCell = DataRowsDataToCell
        OnCellToData = DataRowsCellToData
        OnNewPage = DataRowsNewPage
        MetaData = 'LASTNAME,ADDRESS,INFOTEXT,APP_ID,CANCELLED,PERSONAL_ID'
        MetaLabels = 'NAME,ADDRESS,INFO'
        MetaRowId = 'APP_ID'
        ColWidths = (
          200
          252
          521
          0
          0
          0)
      end
    end
    object STARTING: TTabSheet
      Caption = 'Starting'
      object StartingRows: TDataRows
        Left = 5
        Top = 8
        Width = 977
        Height = 628
        ColCount = 6
        DefaultRowHeight = 18
        DefaultDrawing = False
        DoubleBuffered = True
        RowCount = 2
        ParentDoubleBuffered = False
        PopupMenu = PopupMenu1
        ScrollBars = ssVertical
        TabOrder = 0
        OnDblClick = DataRowsDblClick
        OnDrawCell = DataRowsDrawCell
        OnBeforeRefresh = DataRowsBeforeRefresh
        OnDataToCell = DataRowsDataToCell
        OnCellToData = DataRowsCellToData
        OnNewPage = DataRowsNewPage
        MetaData = 'LASTNAME,ADDRESS,INFOTEXT,APP_ID,CANCELLED,PERSONAL_ID'
        MetaLabels = 'NAME,ADDRESS,INFO'
        MetaRowId = 'APP_ID'
        ColWidths = (
          200
          252
          521
          0
          0
          0)
      end
    end
    object ENDING: TTabSheet
      Caption = 'Ending'
      object EndingRows: TDataRows
        Left = 5
        Top = 8
        Width = 977
        Height = 628
        ColCount = 6
        DefaultRowHeight = 18
        DefaultDrawing = False
        DoubleBuffered = True
        RowCount = 2
        ParentDoubleBuffered = False
        PopupMenu = PopupMenu1
        ScrollBars = ssVertical
        TabOrder = 0
        OnDblClick = DataRowsDblClick
        OnDrawCell = DataRowsDrawCell
        OnBeforeRefresh = DataRowsBeforeRefresh
        OnDataToCell = DataRowsDataToCell
        OnCellToData = DataRowsCellToData
        OnNewPage = DataRowsNewPage
        MetaData = 'LASTNAME,ADDRESS,INFOTEXT,APP_ID,CANCELLED,PERSONAL_ID'
        MetaLabels = 'NAME,ADDRESS,INFO'
        MetaRowId = 'APP_ID'
        ColWidths = (
          150
          302
          521
          0
          0
          0)
      end
    end
  end
  inherited OkButton: TButton
    Caption = 'Archive'
    ModalResult = 0
    OnClick = archive1Click
  end
  object WorkMenu: TMainMenu
    Left = 720
    Top = 65520
    object archive1: TMenuItem
      Caption = '&Archive'
      Enabled = False
      Visible = False
      OnClick = archive1Click
    end
    object list1: TMenuItem
      Caption = '&List'
      object refresh1: TMenuItem
        Caption = '&Refresh'
        ShortCut = 116
        OnClick = refresh1Click
      end
      object N1: TMenuItem
        Caption = '-'
      end
      object selectAll1: TMenuItem
        Caption = 'Select &All'
        ShortCut = 16449
        OnClick = selectAll1Click
      end
      object export1: TMenuItem
        Caption = '&Export Csv'
        ShortCut = 16453
        OnClick = export1Click
      end
      object print1: TMenuItem
        Caption = '&Print Items'
        ShortCut = 16464
        OnClick = print1Click
      end
    end
  end
  object PopupMenu1: TPopupMenu
    Left = 752
    Top = 65520
    object selectAll2: TMenuItem
      Caption = 'Select &All'
      OnClick = selectAll1Click
    end
    object popupExport1: TMenuItem
      Caption = '&Export Csv'
      OnClick = export1Click
    end
    object popupPrint1: TMenuItem
      Caption = '&Print Items'
      OnClick = print1Click
    end
  end
end
