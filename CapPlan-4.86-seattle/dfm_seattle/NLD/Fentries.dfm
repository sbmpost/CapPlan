inherited Fentries1: TFentries1
  Left = 218
  Top = 23
  Caption = 'Cellen'
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
      Caption = 'Actief'
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
        MetaLabels = 'NAAM,ADRES,INFO'
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
      Caption = 'Begint'
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
        MetaLabels = 'NAAM,ADRES,INFO'
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
      Caption = 'Eindigt'
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
        MetaLabels = 'NAAM,ADRES,INFO'
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
    Caption = 'Archief'
    ModalResult = 0
    OnClick = archive1Click
  end
  object WorkMenu: TMainMenu
    Left = 720
    Top = 65520
    object archive1: TMenuItem
      Caption = '&Archief'
      Enabled = False
      Visible = False
      OnClick = archive1Click
    end
    object list1: TMenuItem
      Caption = '&Lijst'
      object refresh1: TMenuItem
        Caption = '&Ververs'
        ShortCut = 116
        OnClick = refresh1Click
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
  end
  object PopupMenu1: TPopupMenu
    Left = 752
    Top = 65520
    object selectAll2: TMenuItem
      Caption = '&Selecteer Alles'
      OnClick = selectAll1Click
    end
    object popupExport1: TMenuItem
      Caption = '&Exporteer Csv'
      OnClick = export1Click
    end
    object popupPrint1: TMenuItem
      Caption = '&Print Ingangen'
      OnClick = print1Click
    end
  end
end
