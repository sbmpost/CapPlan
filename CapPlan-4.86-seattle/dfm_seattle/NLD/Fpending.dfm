inherited Fpending1: TFpending1
  Left = 229
  Top = 37
  Caption = 'Taken'
  ClientHeight = 678
  Ctl3D = False
  Menu = ReviewMenu
  OldCreateOrder = False
  OnContextPopup = FormContextPopup
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 16
  inherited Panel1: TPanel
    TabOrder = 4
  end
  inherited RowControl: TRowControl
    ActivePage = REVIEW
    object REVIEW: TTabSheet
      Caption = 'Controleren'
      OnShow = REVIEWShow
      object ReviewRows: TDataRows
        Left = 5
        Top = 8
        Width = 977
        Height = 628
        ColCount = 15
        DefaultRowHeight = 18
        DefaultDrawing = False
        DoubleBuffered = True
        RowCount = 2
        ParentDoubleBuffered = False
        PopupMenu = PopupMenu1
        ScrollBars = ssVertical
        TabOrder = 0
        OnContextPopup = DataRowsContextPopup
        OnDblClick = DataRowsDblClick
        OnDrawCell = DataRowsDrawCell
        OnGetEditText = DataRowsGetEditText
        OnMouseDown = DataRowsMouseDown
        OnSelectCell = DataRowsSelectCell
        OnBeforeRefresh = ReviewRowsBeforeRefresh
        OnAfterRefresh = DataRowsAfterRefresh
        OnBeforeReadRow = ReviewRowsBeforeReadRow
        OnAfterReadRow = DataRowsAfterReadRow
        OnStoreRow = ReviewRowsStoreRow
        OnCellToData = DataRowsCellToData
        OnNewPage = DataRowsNewPage
        MetaData = 
          'LASTNAME,ADDRESS,COMMENT,REVIEW_BEFORE,REJECTED_DATE,PERSONAL_ID' +
          ',DOC_NAME,LAST_UPDATE_UTC,SIGNED,URGENT,priority,modified,availa' +
          'ble,STATETEXT,DOC_DATA'
        MetaLabels = 'NAAM,ADRES,NOTITIE,PRIORITEIT,"DATUM AFGEKEURD"'
        MetaRowId = 'PERSONAL_ID,DOC_NAME'
        MetaSelect = 'COMMENT,REVIEW_BEFORE'
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
          0
          0
          0
          0
          0
          0)
      end
    end
    object SCHEDULE: TTabSheet
      Caption = 'Plannen'
      OnShow = SCHEDULEShow
      object ScheduleRows: TDataRows
        Left = 5
        Top = 8
        Width = 977
        Height = 628
        ColCount = 20
        DefaultRowHeight = 18
        DefaultDrawing = False
        DoubleBuffered = True
        RowCount = 2
        ParentDoubleBuffered = False
        PopupMenu = PopupMenu2
        ScrollBars = ssVertical
        TabOrder = 0
        OnContextPopup = DataRowsContextPopup
        OnDblClick = DataRowsDblClick
        OnDrawCell = DataRowsDrawCell
        OnGetEditText = DataRowsGetEditText
        OnMouseDown = DataRowsMouseDown
        OnSelectCell = DataRowsSelectCell
        OnBeforeRefresh = ScheduleRowsBeforeRefresh
        OnAfterRefresh = DataRowsAfterRefresh
        OnBeforeReadRow = ScheduleRowsBeforeReadRow
        OnAfterReadRow = DataRowsAfterReadRow
        OnStoreRow = ScheduleRowsStoreRow
        OnCellToData = DataRowsCellToData
        OnNewPage = DataRowsNewPage
        MetaData = 
          'LASTNAME,ADDRESS,COMMENT,PLAN_BEFORE,PLANNED_DATE,PERSONAL_ID,DO' +
          'C_NAME,LAST_UPDATE_UTC,SIGNED,URGENT,priority,modified,available' +
          ',REJECTED_DATE,ORDERED_DATE,CHARGED_DATE,PENDING_REPURCHASE,PEND' +
          'ING_INVOICE,STATETEXT,DOC_DATA'
        MetaLabels = 'NAAM,ADRES,NOTITIE,PRIORITEIT,"DATUM GEPLAND"'
        MetaRowId = 'PERSONAL_ID,DOC_NAME'
        MetaSelect = 'COMMENT,PLAN_BEFORE'
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
          0
          0
          0
          0
          0
          0
          0
          0
          0
          0
          0)
      end
    end
    object REPURCHASE: TTabSheet
      Caption = 'Inkoop'
      OnShow = REPURCHASEShow
      object PurchaseRows: TDataRows
        Left = 5
        Top = 8
        Width = 977
        Height = 628
        ColCount = 15
        DefaultRowHeight = 18
        DefaultDrawing = False
        DoubleBuffered = True
        RowCount = 2
        ParentCtl3D = False
        ParentDoubleBuffered = False
        PopupMenu = PopupMenu3
        ScrollBars = ssVertical
        TabOrder = 0
        OnContextPopup = DataRowsContextPopup
        OnDblClick = DataRowsDblClick
        OnDrawCell = DataRowsDrawCell
        OnGetEditText = DataRowsGetEditText
        OnMouseDown = DataRowsMouseDown
        OnSelectCell = DataRowsSelectCell
        OnBeforeRefresh = PurchaseRowsBeforeRefresh
        OnAfterRefresh = DataRowsAfterRefresh
        OnBeforeReadRow = PurchaseRowsBeforeReadRow
        OnAfterReadRow = DataRowsAfterReadRow
        OnStoreRow = PurchaseRowsStoreRow
        OnCellToData = DataRowsCellToData
        OnNewPage = DataRowsNewPage
        MetaData = 
          'LASTNAME,ADDRESS,COMMENT,ORDER_BEFORE,ORDERED_DATE,PERSONAL_ID,D' +
          'OC_NAME,LAST_UPDATE_UTC,SIGNED,URGENT,priority,modified,availabl' +
          'e,STATETEXT,DOC_DATA'
        MetaLabels = 'NAAM,ADRES,NOTITIE,PRIORITEIT,"DATUM BESTELD"'
        MetaRowId = 'PERSONAL_ID,DOC_NAME'
        MetaSelect = 'COMMENT,ORDER_BEFORE'
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
          0
          0
          0
          0
          0
          0)
      end
    end
    object INVOICE: TTabSheet
      Caption = 'Facturen'
      OnShow = INVOICEShow
      object InvoiceRows: TDataRows
        Left = 5
        Top = 8
        Width = 977
        Height = 628
        ColCount = 15
        DefaultRowHeight = 18
        DefaultDrawing = False
        DoubleBuffered = True
        RowCount = 2
        ParentCtl3D = False
        ParentDoubleBuffered = False
        PopupMenu = PopupMenu4
        ScrollBars = ssVertical
        TabOrder = 0
        OnContextPopup = DataRowsContextPopup
        OnDblClick = DataRowsDblClick
        OnDrawCell = DataRowsDrawCell
        OnGetEditText = DataRowsGetEditText
        OnMouseDown = DataRowsMouseDown
        OnSelectCell = DataRowsSelectCell
        OnBeforeRefresh = InvoiceRowsBeforeRefresh
        OnAfterRefresh = DataRowsAfterRefresh
        OnBeforeReadRow = InvoiceRowsBeforeReadRow
        OnAfterReadRow = DataRowsAfterReadRow
        OnStoreRow = InvoiceRowsStoreRow
        OnCellToData = DataRowsCellToData
        OnNewPage = DataRowsNewPage
        MetaData = 
          'LASTNAME,ADDRESS,COMMENT,CHARGE_BEFORE,CHARGED_DATE,PERSONAL_ID,' +
          'DOC_NAME,LAST_UPDATE_UTC,SIGNED,URGENT,priority,modified,availab' +
          'le,STATETEXT,DOC_DATA'
        MetaLabels = 'NAAM,ADRES,NOTITIE,PRIORITEIT,"DATUM GEFACTUREERD"'
        MetaRowId = 'PERSONAL_ID,DOC_NAME'
        MetaSelect = 'COMMENT,CHARGE_BEFORE'
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
          0
          0
          0
          0
          0
          0)
      end
    end
  end
  inherited CancelButton: TButton
    TabOrder = 3
  end
  inherited OkButton: TButton
    Caption = 'Archief'
    ModalResult = 0
    TabOrder = 2
    OnClick = archive1Click
  end
  object ScheduleButton: TButton
    Left = 720
    Top = 5
    Width = 75
    Height = 22
    Caption = 'Plannen'
    TabOrder = 1
  end
  object RepurchaseMenu: TMainMenu
    Left = 416
    Top = 65520
    object repurchaseArchive1: TMenuItem
      Caption = '&Archief'
      Enabled = False
      Visible = False
      OnClick = archive1Click
    end
    object open3: TMenuItem
      Caption = '&Openen'
      OnClick = open1Click
    end
    object repurchaseList1: TMenuItem
      Caption = '&Lijst'
      object comment3: TMenuItem
        Caption = '&Bewerk'
        ShortCut = 113
        OnClick = modify1Click
      end
      object message3: TMenuItem
        Caption = '&Mailen'
        ShortCut = 115
        OnClick = sendMail1Click
      end
      object refresh3: TMenuItem
        Caption = '&Ververs'
        ShortCut = 116
        OnClick = refresh1Click
      end
      object N7: TMenuItem
        Caption = '-'
      end
      object repurchaseRemove1: TMenuItem
        Caption = '&Weghalen'
        ShortCut = 46
        OnClick = repurchaseRemove1Click
      end
      object selectAll3: TMenuItem
        Caption = '&Selecteer Alles'
        ShortCut = 16449
        OnClick = selectAll1Click
      end
      object macro3: TMenuItem
        Caption = '&Macro Uitvoeren'
        Enabled = False
        ShortCut = 16461
        Visible = False
        OnClick = macro1Click
      end
      object export3: TMenuItem
        Caption = '&Exporteer Csv'
        ShortCut = 16453
        OnClick = export1Click
      end
      object print3: TMenuItem
        Caption = '&Print Ingangen'
        ShortCut = 16464
        OnClick = printClick
      end
    end
    object repurchaseOrdered1: TMenuItem
      Caption = '&Besteld'
      OnClick = repurchaseOrdered1Click
    end
  end
  object ScheduleMenu: TMainMenu
    Left = 384
    Top = 65520
    object scheduleArchive1: TMenuItem
      Caption = '&Archief'
      Enabled = False
      Visible = False
      OnClick = archive1Click
    end
    object open2: TMenuItem
      Caption = '&Openen'
      OnClick = open1Click
    end
    object scheduleList1: TMenuItem
      Caption = '&Lijst'
      object comment2: TMenuItem
        Caption = '&Bewerk'
        ShortCut = 113
        OnClick = modify1Click
      end
      object message2: TMenuItem
        Caption = '&Mailen'
        ShortCut = 115
        OnClick = sendMail1Click
      end
      object refresh2: TMenuItem
        Caption = '&Ververs'
        ShortCut = 116
        OnClick = refresh1Click
      end
      object N6: TMenuItem
        Caption = '-'
      end
      object scheduleRemove1: TMenuItem
        Caption = '&Weghalen'
        ShortCut = 46
        OnClick = scheduleRemove1Click
      end
      object SelectAll2: TMenuItem
        Caption = '&Selecteer Alles'
        ShortCut = 16449
        OnClick = selectAll1Click
      end
      object macro2: TMenuItem
        Caption = '&Macro Uitvoeren'
        Enabled = False
        ShortCut = 16461
        Visible = False
        OnClick = macro1Click
      end
      object export2: TMenuItem
        Caption = '&Exporteer Csv'
        ShortCut = 16453
        OnClick = export1Click
      end
      object print2: TMenuItem
        Caption = '&Print Ingangen'
        ShortCut = 16464
        OnClick = printClick
      end
    end
    object scheduleDocument1: TMenuItem
      Caption = '&Plannen'
      Enabled = False
      Visible = False
      OnClick = scheduleDocument1Click
    end
    object scheduleRejected1: TMenuItem
      Caption = '&Afgekeurd'
      OnClick = reviewRejected1Click
    end
    object scheduleComplete1: TMenuItem
      Caption = '&Gepland'
      OnClick = scheduleComplete1Click
    end
  end
  object InvoiceMenu: TMainMenu
    Left = 448
    Top = 65520
    object invoiceArchive1: TMenuItem
      Caption = '&Archief'
      Enabled = False
      Visible = False
      OnClick = archive1Click
    end
    object open4: TMenuItem
      Caption = '&Openen'
      OnClick = open1Click
    end
    object invoiceList1: TMenuItem
      Caption = '&Lijst'
      object comment4: TMenuItem
        Caption = '&Bewerk'
        ShortCut = 113
        OnClick = modify1Click
      end
      object message4: TMenuItem
        Caption = '&Mailen'
        ShortCut = 115
        OnClick = sendMail1Click
      end
      object refresh4: TMenuItem
        Caption = '&Ververs'
        ShortCut = 116
        OnClick = refresh1Click
      end
      object N8: TMenuItem
        Caption = '-'
      end
      object invoiceRemove1: TMenuItem
        Caption = '&Weghalen'
        ShortCut = 46
        OnClick = invoiceRemove1Click
      end
      object selectAll4: TMenuItem
        Caption = '&Selecteer Alles'
        ShortCut = 16449
        OnClick = selectAll1Click
      end
      object macro4: TMenuItem
        Caption = '&Macro Uitvoeren'
        Enabled = False
        ShortCut = 16461
        Visible = False
        OnClick = macro1Click
      end
      object export4: TMenuItem
        Caption = '&Exporteer Csv'
        ShortCut = 16453
        OnClick = export1Click
      end
      object print4: TMenuItem
        Caption = 'Print Ingangen'
        ShortCut = 16464
        OnClick = printClick
      end
    end
    object invoiceCharged1: TMenuItem
      Caption = '&Gefactureerd'
      OnClick = invoiceCharged1Click
    end
  end
  object ReviewMenu: TMainMenu
    Left = 352
    Top = 65520
    object reviewArchive1: TMenuItem
      Caption = '&Archief'
      Enabled = False
      Visible = False
      OnClick = archive1Click
    end
    object open1: TMenuItem
      Caption = '&Openen'
      OnClick = open1Click
    end
    object reviewList1: TMenuItem
      Caption = '&Lijst'
      object comment1: TMenuItem
        Caption = '&Bewerk'
        ShortCut = 113
        OnClick = modify1Click
      end
      object message1: TMenuItem
        Caption = '&Mailen'
        ShortCut = 115
        OnClick = sendMail1Click
      end
      object refresh1: TMenuItem
        Caption = '&Ververs'
        ShortCut = 116
        OnClick = refresh1Click
      end
      object N5: TMenuItem
        Caption = '-'
      end
      object selectAll1: TMenuItem
        Caption = '&Selecteer Alles'
        ShortCut = 16449
        OnClick = selectAll1Click
      end
      object macro1: TMenuItem
        Caption = '&Macro Uitvoeren'
        Enabled = False
        ShortCut = 16461
        Visible = False
        OnClick = macro1Click
      end
      object export1: TMenuItem
        Caption = '&Exporteer Csv'
        ShortCut = 16453
        OnClick = export1Click
      end
      object print1: TMenuItem
        Caption = '&Print Ingangen'
        ShortCut = 16464
        OnClick = printClick
      end
    end
    object reviewSchedule1: TMenuItem
      Caption = '&Goedgekeurd'
      Enabled = False
      Visible = False
      OnClick = reviewSchedule1Click
    end
    object reviewRejected1: TMenuItem
      Caption = '&Afgekeurd'
      OnClick = reviewRejected1Click
    end
  end
  object PopupMenu1: TPopupMenu
    Left = 480
    Top = 65520
    object popupOpen1: TMenuItem
      Caption = '&Openen'
      OnClick = open1Click
    end
    object popupSchedule1: TMenuItem
      Caption = '&Goedgekeurd'
      OnClick = reviewSchedule1Click
    end
    object popupRejected1: TMenuItem
      Caption = '&Afgekeurd'
      OnClick = reviewRejected1Click
    end
    object popupReset1: TMenuItem
      Caption = '&Terugzetten'
      OnClick = popupReset1Click
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object popupSelectAll1: TMenuItem
      Caption = '&Selecteer Alles'
      OnClick = selectAll1Click
    end
    object popupMacro1: TMenuItem
      Caption = '&Macro Uitvoeren'
      Enabled = False
      Visible = False
      OnClick = macro1Click
    end
    object popupExport1: TMenuItem
      Caption = '&Exporteer Csv'
      OnClick = export1Click
    end
    object popupPrint1: TMenuItem
      Caption = '&Print Ingangen'
      OnClick = printClick
    end
  end
  object PopupMenu2: TPopupMenu
    Left = 512
    Top = 65520
    object popupOpen2: TMenuItem
      Caption = '&Openen'
      OnClick = open1Click
    end
    object popupSchedule2: TMenuItem
      Caption = '&Plannen'
      OnClick = scheduleDocument1Click
    end
    object popupRejected2: TMenuItem
      Caption = '&Afgekeurd'
      OnClick = reviewRejected1Click
    end
    object popupPlanned1: TMenuItem
      Caption = '&Gepland'
      OnClick = scheduleComplete1Click
    end
    object popupReset2: TMenuItem
      Caption = '&Terugzetten'
      OnClick = popupReset2Click
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object popupRemove1: TMenuItem
      Caption = '&Weghalen'
      OnClick = scheduleRemove1Click
    end
    object popupSelectAll2: TMenuItem
      Caption = '&Selecteer Alles'
      OnClick = selectAll1Click
    end
    object popupMacro2: TMenuItem
      Caption = '&Macro Uitvoeren'
      Enabled = False
      Visible = False
      OnClick = macro1Click
    end
    object ExportCsv1: TMenuItem
      Caption = '&Exporteer Csv'
      OnClick = export1Click
    end
    object popupPrint2: TMenuItem
      Caption = '&Print Ingangen'
      OnClick = printClick
    end
  end
  object PopupMenu3: TPopupMenu
    Left = 544
    Top = 65520
    object popupOpen3: TMenuItem
      Caption = '&Openen'
      OnClick = open1Click
    end
    object popupOrdered1: TMenuItem
      Caption = '&Besteld'
      OnClick = repurchaseOrdered1Click
    end
    object popupReset3: TMenuItem
      Caption = '&Terugzetten'
      OnClick = popupReset3Click
    end
    object N3: TMenuItem
      Caption = '-'
    end
    object popupRemove2: TMenuItem
      Caption = '&Weghalen'
      OnClick = repurchaseRemove1Click
    end
    object popupSelectAll3: TMenuItem
      Caption = '&Selecteer Alles'
      OnClick = selectAll1Click
    end
    object popupMacro3: TMenuItem
      Caption = '&Macro Uitvoeren'
      Enabled = False
      Visible = False
      OnClick = macro1Click
    end
    object popupExport3: TMenuItem
      Caption = '&Exporteer Csv'
      OnClick = export1Click
    end
    object popupPrint3: TMenuItem
      Caption = '&Print Ingangen'
      OnClick = printClick
    end
  end
  object PopupMenu4: TPopupMenu
    Left = 576
    Top = 65520
    object popupOpen4: TMenuItem
      Caption = '&Openen'
      OnClick = open1Click
    end
    object popupCharged1: TMenuItem
      Caption = '&Gefactureerd'
      OnClick = invoiceCharged1Click
    end
    object popupReset4: TMenuItem
      Caption = '&Terugzetten'
      OnClick = popupReset4Click
    end
    object N4: TMenuItem
      Caption = '-'
    end
    object popupRemove3: TMenuItem
      Caption = '&Weghalen'
      OnClick = invoiceRemove1Click
    end
    object popupSelectAll4: TMenuItem
      Caption = '&Selecteer Alles'
      OnClick = selectAll1Click
    end
    object popupMacro4: TMenuItem
      Caption = '&Macro Uitvoeren'
      Enabled = False
      Visible = False
      OnClick = macro1Click
    end
    object popupExport4: TMenuItem
      Caption = '&Exporteer Csv'
      OnClick = export1Click
    end
    object popupPrint4: TMenuItem
      Caption = '&Print Ingangen'
      OnClick = printClick
    end
  end
end
