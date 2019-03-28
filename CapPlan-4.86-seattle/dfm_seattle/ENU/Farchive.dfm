inherited Farchive1: TFarchive1
  Left = 263
  Top = 38
  Caption = 'Archive'
  ClientHeight = 678
  Ctl3D = False
  Menu = PersonMenu
  OldCreateOrder = False
  Position = poDefaultPosOnly
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 16
  inherited Panel1: TPanel
    TabOrder = 4
  end
  inherited RowControl: TRowControl
    ActivePage = PERSONS
    object PERSONS: TTabSheet
      Caption = 'Personal data'
      OnShow = PERSONSShow
      object MemoLabel: TLabel
        Left = 474
        Top = 440
        Width = 40
        Height = 16
        Caption = 'MEM&O'
        FocusControl = Memo
      end
      object Label1: TLabel
        Left = 263
        Top = 5
        Width = 72
        Height = 16
        Caption = '&POSTCODE'
        FocusControl = Postcode
      end
      object PersonRows: TDataRows
        Left = 5
        Top = 65
        Width = 977
        Height = 362
        ColCount = 11
        DefaultRowHeight = 18
        DefaultDrawing = False
        DoubleBuffered = True
        RowCount = 2
        ParentDoubleBuffered = False
        ScrollBars = ssVertical
        TabOrder = 4
        OnContextPopup = DataRowsContextPopup
        OnDblClick = PersonRowsDblClick
        OnDrawCell = DataRowsDrawCell
        OnGetEditMask = PersonRowsGetEditMask
        OnSelectCell = PersonRowsSelectCell
        OnBeforeRefresh = PersonRowsBeforeRefresh
        OnAfterRefresh = PersonRowsAfterRefresh
        OnBeforeReadRow = PersonRowsBeforeReadRow
        OnAfterReadRow = PersonRowsAfterReadRow
        OnStoreRow = PersonRowsStoreRow
        OnCellToData = PersonRowsCellToData
        OnNewPage = DataRowsNewPage
        KeySearchColumn = 0
        MetaData = 
          'LASTNAME,POSTCODE,ADDRESS,CITY,TELEPHONE,TELEPHONE2,PERSONAL_ID,' +
          'HIDDEN,CREATED_BY,CREATED_DATE,MEMOTEXT'
        MetaLabels = 'NAME,POSTCODE,ADDRESS,CITY,TELEPHONE,TELEPHONE2,ID'
        MetaRowId = 'PERSONAL_ID'
        MetaSelect = 'LASTNAME,POSTCODE,ADDRESS,CITY,TELEPHONE,TELEPHONE2'
        ColWidths = (
          200
          100
          252
          150
          100
          100
          67
          0
          0
          0
          0)
      end
      object Lastname: TLabeledEdit
        Left = 5
        Top = 24
        Width = 219
        Height = 22
        EditLabel.Width = 37
        EditLabel.Height = 16
        EditLabel.Caption = 'NA&ME'
        LabelPosition = lpAbove
        LabelSpacing = 3
        TabOrder = 0
        OnChange = SearchChange
      end
      object Address: TLabeledEdit
        Left = 457
        Top = 24
        Width = 331
        Height = 22
        EditLabel.Width = 61
        EditLabel.Height = 16
        EditLabel.Caption = '&ADDRESS'
        LabelPosition = lpAbove
        LabelSpacing = 3
        TabOrder = 2
        OnChange = SearchChange
      end
      object City: TLabeledEdit
        Left = 827
        Top = 24
        Width = 155
        Height = 22
        EditLabel.Width = 32
        EditLabel.Height = 16
        EditLabel.Caption = '&CITY'
        LabelPosition = lpAbove
        LabelSpacing = 3
        TabOrder = 3
        OnChange = SearchChange
      end
      object Memo: TMemo
        Left = 5
        Top = 468
        Width = 977
        Height = 166
        MaxLength = 255
        ScrollBars = ssVertical
        TabOrder = 6
        OnEnter = MemoEnter
        OnExit = MemoExit
      end
      object Postcode: TMaskEdit
        Left = 263
        Top = 24
        Width = 155
        Height = 22
        TabOrder = 1
        Text = ''
        OnChange = SearchChange
      end
      object EmployeesBox: TFixedCheckBox
        Left = 5
        Top = 440
        Width = 257
        Height = 17
        TabStop = False
        Caption = '&FILTER EMPLOYEES'
        TabOrder = 5
        OnEnter = EmployeesBoxEnter
      end
    end
    object APPS: TTabSheet
      Caption = 'Appointments'
      OnShow = APPSShow
      object InfoLabel: TLabel
        Left = 474
        Top = 440
        Width = 32
        Height = 16
        Caption = 'INF&O'
        FocusControl = Info
      end
      object AppRows: TDataRows
        Left = 5
        Top = 8
        Width = 977
        Height = 419
        Hint = 'Click new to make a new appointment'
        ColCount = 15
        DefaultRowHeight = 18
        DefaultDrawing = False
        DoubleBuffered = True
        RowCount = 2
        ParentDoubleBuffered = False
        ScrollBars = ssVertical
        TabOrder = 0
        OnContextPopup = DataRowsContextPopup
        OnDblClick = AppRowsDblClick
        OnDrawCell = DataRowsDrawCell
        OnMouseUp = AppRowsMouseUp
        OnSelectCell = AppRowsSelectCell
        OnBeforeRefresh = AppRowsBeforeRefresh
        OnAfterRefresh = AppRowsAfterRefresh
        OnBeforeReadRow = AppRowsBeforeReadRow
        OnAfterReadRow = AppRowsAfterReadRow
        OnStoreRow = AppRowsStoreRow
        OnDataToCell = AppRowsDataToCell
        OnCellToData = AppRowsCellToData
        OnPrintCell = AppRowsPrintCell
        OnNewPage = DataRowsNewPage
        OnEndDoc = AppRowsEndDoc
        KeySearchColumn = 0
        MetaData = 
          'CODE_NAME,min_entries_date_from,max_entries_date_until,sum_entri' +
          'es_date_diff,PAID,PRICE,APP_ID,EXCLUSIVE,CANCELLED,COLOR,CHARGE,' +
          'count_entries_date_from,CREATED_BY,CREATED_DATE,INFOTEXT'
        MetaLabels = 'CODE,FROM,UNTIL,HOURS,PAID,PRICE,ID'
        MetaRowId = 'APP_ID'
        MetaSelect = 'CODE_NAME'
        ColWidths = (
          215
          171
          171
          115
          115
          115
          67
          0
          0
          0
          0
          0
          0
          0
          0)
      end
      object Info: TMemo
        Left = 5
        Top = 468
        Width = 977
        Height = 166
        MaxLength = 255
        ScrollBars = ssVertical
        TabOrder = 1
        OnEnter = InfoEnter
        OnExit = InfoExit
      end
    end
    object DOCS: TTabSheet
      Caption = 'Documents'
      OnShow = DOCSShow
      object StatLabel: TLabel
        Left = 454
        Top = 440
        Width = 80
        Height = 16
        Caption = 'E&XTRA INFO'
        FocusControl = Stat
      end
      object DocRows: TDataRows
        Left = 5
        Top = 8
        Width = 977
        Height = 419
        ColCount = 21
        DefaultRowHeight = 18
        DefaultDrawing = False
        DoubleBuffered = True
        RowCount = 2
        ParentDoubleBuffered = False
        ScrollBars = ssVertical
        TabOrder = 0
        OnContextPopup = DataRowsContextPopup
        OnDblClick = DocRowsDblClick
        OnDrawCell = DataRowsDrawCell
        OnSelectCell = DocRowsSelectCell
        OnBeforeRefresh = DocRowsBeforeRefresh
        OnAfterRefresh = DocRowsAfterRefresh
        OnBeforeReadRow = DocRowsBeforeReadRow
        OnAfterReadRow = DocRowsAfterReadRow
        OnStoreRow = DocRowsStoreRow
        OnDataToCell = DocRowsDataToCell
        OnCellToData = DocRowsCellToData
        OnPrintCell = DocRowsPrintCell
        OnNewPage = DataRowsNewPage
        KeySearchColumn = 0
        MetaData = 
          'DOC_NAME,COMMENT,schedule,rejected,DOC_SIZE,LAST_UPDATE,LAST_UPD' +
          'ATE_UTC,REJECTED_DATE,PLANNED_DATE,ORDERED_DATE,CHARGED_DATE,PEN' +
          'DING_REVIEW,PENDING_SCHEDULE,SIGNED,URGENT,CREATED_BY,CREATED_DA' +
          'TE,PERSONAL_ID,available,STATETEXT,DOC_DATA'
        MetaLabels = 'DOCUMENT,COMMENT,SCHEDULE,REJECTED,BYTES,"DATE MODIFIED"'
        MetaRowId = 'PERSONAL_ID,DOC_NAME'
        MetaSelect = 'COMMENT'
        ColWidths = (
          222
          258
          115
          115
          89
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
      object Stat: TMemo
        Left = 5
        Top = 468
        Width = 977
        Height = 166
        MaxLength = 255
        ScrollBars = ssVertical
        TabOrder = 2
        OnEnter = StatEnter
        OnExit = StatExit
      end
      object ProgressBar1: TProgressBar
        Left = 5
        Top = 438
        Width = 246
        Height = 18
        Max = 16
        Step = 1
        TabOrder = 1
        Visible = False
      end
    end
    object SCHEDULES: TTabSheet
      Caption = 'Schedules'
      OnShow = SCHEDULESShow
      object Label2: TLabel
        Left = 474
        Top = 440
        Width = 36
        Height = 16
        Caption = 'N&OTE'
        FocusControl = Note
      end
      object ScheduleRows: TDataRows
        Left = 5
        Top = 8
        Width = 977
        Height = 419
        ColCount = 12
        DefaultRowHeight = 18
        DefaultDrawing = False
        DoubleBuffered = True
        RowCount = 2
        ParentDoubleBuffered = False
        ScrollBars = ssVertical
        TabOrder = 0
        OnContextPopup = DataRowsContextPopup
        OnDblClick = ScheduleRowsDblClick
        OnDrawCell = DataRowsDrawCell
        OnGetEditMask = ScheduleRowsGetEditMask
        OnSelectCell = ScheduleRowsSelectCell
        OnBeforeRefresh = ScheduleRowsBeforeRefresh
        OnAfterRefresh = ScheduleRowsAfterRefresh
        OnBeforeReadRow = ScheduleRowsBeforeReadRow
        OnAfterReadRow = ScheduleRowsAfterReadRow
        OnStoreRow = ScheduleRowsStoreRow
        OnDataToCell = ScheduleRowsDataToCell
        OnCellToData = ScheduleRowsCellToData
        OnNewPage = DataRowsNewPage
        KeySearchColumn = 0
        MetaData = 
          'SCHEDULE_NAME,EMPLOYEE,WEEK_START,WEEK_STOP,DAY_START,DAY_STOP,D' +
          'AY_UNITS,SCHEDULE_ID,HIDDEN,CREATED_BY,CREATED_DATE,NOTETEXT'
        MetaLabels = 'SCHEDULE,EMPLOYEE,WEEKSTART,WEEKSTOP,DAYSTART,DAYSTOP,DAYUNITS,ID'
        MetaRowId = 'SCHEDULE_ID'
        MetaSelect = 'SCHEDULE_NAME,WEEK_START,WEEK_STOP,DAY_START,DAY_STOP,DAY_UNITS'
        ColWidths = (
          133
          128
          128
          128
          128
          128
          128
          67
          0
          0
          0
          0)
      end
      object Note: TMemo
        Left = 5
        Top = 468
        Width = 977
        Height = 166
        MaxLength = 255
        ScrollBars = ssVertical
        TabOrder = 1
        OnEnter = NoteEnter
        OnExit = NoteExit
      end
    end
  end
  inherited CancelButton: TButton
    TabOrder = 3
  end
  inherited OkButton: TButton
    TabOrder = 2
  end
  object ScheduleButton: TButton
    Left = 720
    Top = 5
    Width = 75
    Height = 22
    Caption = 'Schedule'
    TabOrder = 1
    OnClick = ScheduleButtonClick
  end
  object PersonMenu: TMainMenu
    Left = 368
    Top = 65520
    object personsNew1: TMenuItem
      Caption = '&New'
      OnClick = personsNew1Click
    end
    object personsEdit1: TMenuItem
      Caption = '&Edit'
      object modify1: TMenuItem
        Caption = '&Modify'
        ShortCut = 113
        OnClick = modify1Click
      end
      object personsDelete1: TMenuItem
        Caption = '&Delete'
        ShortCut = 16452
        OnClick = personsDelete1Click
      end
      object personsDuplicate1: TMenuItem
        Caption = 'D&uplicate'
        ShortCut = 16469
        OnClick = personsDuplicate1Click
      end
      object N2: TMenuItem
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
    object personsDetails1: TMenuItem
      Caption = '&Details'
      OnClick = personsDetails1Click
    end
    object personsSearch1: TMenuItem
      Caption = '&Search'
      OnClick = personsSearch1Click
    end
  end
  object AppointmentMenu: TMainMenu
    Left = 400
    Top = 65520
    object appsNew1: TMenuItem
      Caption = '&New'
      OnClick = appsNew1Click
    end
    object appsEdit1: TMenuItem
      Caption = '&Edit'
      object modify2: TMenuItem
        Caption = '&Modify'
        ShortCut = 113
        OnClick = modify1Click
      end
      object appsDelete1: TMenuItem
        Caption = '&Delete'
        ShortCut = 16452
        OnClick = appsDelete1Click
      end
      object appsDuplicate1: TMenuItem
        Caption = 'D&uplicate'
        ShortCut = 16469
        OnClick = appsDuplicate1Click
      end
      object N3: TMenuItem
        Caption = '-'
      end
      object selectAll2: TMenuItem
        Caption = 'Select &All'
        ShortCut = 16449
        OnClick = selectAll1Click
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
    object appsDetails1: TMenuItem
      Caption = '&Details'
      OnClick = appsDetails1Click
    end
    object appsSelect1: TMenuItem
      Caption = '&Select'
      OnClick = appsSelect1Click
    end
  end
  object CodePopup: TPopupMenu
    AutoHotkeys = maManual
    Left = 688
    Top = 65520
  end
  object ScheduleMenu: TMainMenu
    Left = 464
    Top = 65520
    object schedulesNew1: TMenuItem
      Caption = '&New'
      OnClick = schedulesNew1Click
    end
    object schedulesEdit1: TMenuItem
      Caption = '&Edit'
      object modify4: TMenuItem
        Caption = '&Modify'
        ShortCut = 113
        OnClick = modify1Click
      end
      object schedulesDelete1: TMenuItem
        Caption = '&Delete'
        ShortCut = 16452
        OnClick = schedulesDelete1Click
      end
      object schedulesDuplicate1: TMenuItem
        Caption = 'D&uplicate'
        ShortCut = 16469
        OnClick = schedulesDuplicate1Click
      end
      object N5: TMenuItem
        Caption = '-'
      end
      object selectAll4: TMenuItem
        Caption = 'Select &All'
        ShortCut = 16449
        OnClick = selectAll1Click
      end
      object export4: TMenuItem
        Caption = '&Export Csv'
        ShortCut = 16453
        OnClick = export1Click
      end
      object print4: TMenuItem
        Caption = '&Print Items'
        ShortCut = 16464
        OnClick = print1Click
      end
    end
    object schedulesDetails1: TMenuItem
      Caption = '&Details'
      OnClick = schedulesDetails1Click
    end
  end
  object DocumentMenu: TMainMenu
    Left = 432
    Top = 65520
    object docsNew1: TMenuItem
      Caption = '&New'
      OnClick = docsNew1Click
    end
    object docsEdit1: TMenuItem
      Caption = '&Edit'
      object modify3: TMenuItem
        Caption = '&Modify'
        ShortCut = 113
        OnClick = modify1Click
      end
      object docsDelete1: TMenuItem
        Caption = '&Delete'
        ShortCut = 16452
        OnClick = docsDelete1Click
      end
      object docsDuplicate1: TMenuItem
        Caption = 'D&uplicate'
        ShortCut = 16469
        OnClick = docsDuplicate1Click
      end
      object N4: TMenuItem
        Caption = '-'
      end
      object selectAll3: TMenuItem
        Caption = 'Select &All'
        ShortCut = 16449
        OnClick = selectAll1Click
      end
      object export3: TMenuItem
        Caption = '&Export Csv'
        ShortCut = 16453
        OnClick = export1Click
      end
      object print3: TMenuItem
        Caption = '&Print Items'
        ShortCut = 16464
        OnClick = print1Click
      end
    end
    object docsDetails1: TMenuItem
      Caption = '&Details'
      OnClick = docsDetails1Click
    end
    object Locate1: TMenuItem
      Caption = '&Locate'
      OnClick = docsLocate1Click
    end
    object docsView1: TMenuItem
      Caption = '&View'
      Enabled = False
      Visible = False
      OnClick = docsView1Click
    end
    object docsOpen1: TMenuItem
      Caption = '&Open'
      OnClick = docsModify1Click
    end
  end
  object OpenDialog: TOpenDialog
    Options = [ofHideReadOnly, ofNoChangeDir, ofFileMustExist, ofEnableSizing]
    Left = 720
    Top = 65520
  end
  object PopupMenu1: TPopupMenu
    Left = 496
    Top = 65520
    object popupDetails1: TMenuItem
      Caption = 'De&tails'
      OnClick = personsDetails1Click
    end
    object popupModify1: TMenuItem
      Caption = '&Modify'
      OnClick = modify1Click
    end
    object popupDelete1: TMenuItem
      Caption = '&Delete'
      OnClick = personsDelete1Click
    end
    object popupDuplicate1: TMenuItem
      Caption = 'D&uplicate'
      OnClick = personsDuplicate1Click
    end
    object N6: TMenuItem
      Caption = '-'
    end
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
  object PopupMenu2: TPopupMenu
    Left = 528
    Top = 65520
    object popupSelect1: TMenuItem
      Caption = '&Select'
      OnClick = appsSelect1Click
    end
    object popupDetails2: TMenuItem
      Caption = 'De&tails'
      OnClick = appsDetails1Click
    end
    object popupModify2: TMenuItem
      Caption = '&Modify'
      OnClick = modify1Click
    end
    object popupDelete2: TMenuItem
      Caption = '&Delete'
      OnClick = appsDelete1Click
    end
    object popupDuplicate2: TMenuItem
      Caption = 'D&uplicate'
      OnClick = appsDuplicate1Click
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object popupSelectAll2: TMenuItem
      Caption = 'Select &All'
      OnClick = selectAll1Click
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
  object PopupMenu3: TPopupMenu
    Left = 560
    Top = 65520
    object popupOpen1: TMenuItem
      Caption = '&Open'
      OnClick = docsModify1Click
    end
    object popupView1: TMenuItem
      Caption = '&View'
      Enabled = False
      Visible = False
      OnClick = docsView1Click
    end
    object popupLocate: TMenuItem
      Caption = '&Locate'
      OnClick = docsLocate1Click
    end
    object popupDetails3: TMenuItem
      Caption = 'De&tails'
      OnClick = docsDetails1Click
    end
    object popupModify3: TMenuItem
      Caption = '&Modify'
      OnClick = modify1Click
    end
    object popupDelete3: TMenuItem
      Caption = '&Delete'
      OnClick = docsDelete1Click
    end
    object popupDuplicate3: TMenuItem
      Caption = 'D&uplicate'
      OnClick = docsDuplicate1Click
    end
    object MenuItem9: TMenuItem
      Caption = '-'
    end
    object popupSelectAll3: TMenuItem
      Caption = 'Select &All'
      OnClick = selectAll1Click
    end
    object popupExport3: TMenuItem
      Caption = '&Export Csv'
      OnClick = export1Click
    end
    object popupPrint3: TMenuItem
      Caption = '&Print Items'
      OnClick = print1Click
    end
  end
  object PopupMenu4: TPopupMenu
    Left = 592
    Top = 65520
    object popupDetails4: TMenuItem
      Caption = 'De&tails'
      OnClick = schedulesDetails1Click
    end
    object popupModify4: TMenuItem
      Caption = '&Modify'
      OnClick = modify1Click
    end
    object popupDelete4: TMenuItem
      Caption = '&Delete'
      OnClick = schedulesDelete1Click
    end
    object popupDuplicate4: TMenuItem
      Caption = 'D&uplicate'
      OnClick = schedulesDuplicate1Click
    end
    object N7: TMenuItem
      Caption = '-'
    end
    object popupSelectAll4: TMenuItem
      Caption = 'Select &All'
      OnClick = selectAll1Click
    end
    object popupExport4: TMenuItem
      Caption = '&Export Csv'
      OnClick = export1Click
    end
    object popupPrint4: TMenuItem
      Caption = '&Print Items'
      OnClick = print1Click
    end
  end
end
