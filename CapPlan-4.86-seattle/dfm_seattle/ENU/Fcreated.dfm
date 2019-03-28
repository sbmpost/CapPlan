inherited Fcreated1: TFcreated1
  Left = 231
  Top = 13
  Caption = 'Created'
  ClientHeight = 678
  Ctl3D = False
  Menu = DocumentMenu
  OldCreateOrder = False
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 16
  inherited RowControl: TRowControl
    ActivePage = APPS
    object APPS: TTabSheet
      Caption = 'Appointments'
      object AppRows: TDataRows
        Left = 5
        Top = 8
        Width = 977
        Height = 628
        ColCount = 8
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
        OnBeforeRefresh = AppRowsBeforeRefresh
        OnDataToCell = AppRowsDataToCell
        OnCellToData = AppRowsCellToData
        OnPrintCell = DataRowsPrintCell
        OnNewPage = DataRowsNewPage
        MetaData = 
          'LASTNAME,ADDRESS,INFOTEXT,PAID,CREATED_DATE,PERSONAL_ID,APP_ID,C' +
          'ANCELLED'
        MetaLabels = 'NAME,ADDRESS,INFO,PAID,"CREATED ON"'
        MetaRowId = 'APP_ID'
        ColWidths = (
          200
          252
          258
          90
          171
          0
          0
          0)
      end
    end
    object DOCUMENTS: TTabSheet
      Caption = 'Documents'
      object DocumentRows: TDataRows
        Left = 5
        Top = 8
        Width = 977
        Height = 628
        ColCount = 10
        DefaultRowHeight = 18
        DefaultDrawing = False
        DoubleBuffered = True
        RowCount = 2
        ParentDoubleBuffered = False
        PopupMenu = PopupMenu2
        ScrollBars = ssVertical
        TabOrder = 0
        OnDblClick = DataRowsDblClick
        OnDrawCell = DataRowsDrawCell
        OnBeforeRefresh = DocumentRowsBeforeRefresh
        OnAfterRefresh = DocumentRowsAfterRefresh
        OnBeforeReadRow = DocumentRowsBeforeReadRow
        OnDataToCell = DocumentRowsDataToCell
        OnCellToData = DocumentRowsCellToData
        OnPrintCell = DataRowsPrintCell
        OnNewPage = DataRowsNewPage
        MetaData = 
          'LASTNAME,ADDRESS,COMMENT,schedule,CREATED_DATE,PERSONAL_ID,DOC_N' +
          'AME,LAST_UPDATE_UTC,available,DOC_DATA'
        MetaLabels = 'NAME,ADDRESS,COMMENT,SCHEDULE,"CREATED ON"'
        MetaRowId = 'PERSONAL_ID,DOC_NAME'
        ColWidths = (
          200
          252
          258
          90
          171
          0
          0
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
  object DocumentMenu: TMainMenu
    Left = 680
    Top = 65520
    object archive2: TMenuItem
      Caption = '&Archive'
      Enabled = False
      Visible = False
      OnClick = archive1Click
    end
    object list2: TMenuItem
      Caption = '&List'
      object refresh2: TMenuItem
        Caption = '&Refresh'
        ShortCut = 116
        OnClick = refresh1Click
      end
      object N2: TMenuItem
        Caption = '-'
      end
      object selectAll2: TMenuItem
        Caption = 'Select &All'
        ShortCut = 16449
        OnClick = selectAll1Click
      end
      object macro1: TMenuItem
        Caption = 'Run &Macro'
        ShortCut = 16461
        OnClick = macro1Click
      end
      object export2: TMenuItem
        Caption = '&Export Csv'
        ShortCut = 16453
        OnClick = export1Click
      end
      object print2: TMenuItem
        Caption = '&Print Items'
        ShortCut = 16464
        OnClick = print1Click
      end
    end
  end
  object PopupMenu1: TPopupMenu
    Left = 712
    Top = 65520
    object popupSelectAll1: TMenuItem
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
  object AppointmentMenu: TMainMenu
    Left = 648
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
  object PopupMenu2: TPopupMenu
    Left = 744
    Top = 65520
    object popupSelectAll2: TMenuItem
      Caption = 'Select &All'
      OnClick = selectAll1Click
    end
    object popupMacro1: TMenuItem
      Caption = 'Run &Macro'
      OnClick = macro1Click
    end
    object popupExport2: TMenuItem
      Caption = '&Export Csv'
      OnClick = export1Click
    end
    object popupPrint2: TMenuItem
      Caption = '&Print Items'
      OnClick = print1Click
    end
  end
end
