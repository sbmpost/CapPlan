inherited Fmain1: TFmain1
  Left = 204
  Top = 35
  Caption = 'Fmain1'
  Color = clSkyBlue
  Menu = MainMenu1
  OldCreateOrder = False
  Position = poDefault
  PrintScale = poPrintToFit
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnKeyDown = FormKeyDown
  OnMouseWheelDown = FormMouseWheelDown
  OnMouseWheelUp = FormMouseWheelUp
  OnResize = FormResize
  OnShow = FormShow
  DesignSize = (
    992
    678)
  PixelsPerInch = 96
  TextHeight = 16
  inherited Panel1: TPanel
    TabOrder = 4
  end
  object StatusPanel: TPanel
    Left = 0
    Top = 0
    Width = 992
    Height = 40
    Anchors = [akLeft, akTop, akRight]
    BevelOuter = bvNone
    Color = clSkyBlue
    TabOrder = 0
    DesignSize = (
      992
      40)
    object EntryText: TLabel
      Left = 400
      Top = 5
      Width = 577
      Height = 33
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'CelTekst'
      Color = clWhite
      ParentColor = False
      ShowAccelChar = False
      WordWrap = True
    end
    object DatePicker1: TDateTimePicker
      Left = 129
      Top = 10
      Width = 256
      Height = 24
      BevelEdges = []
      BevelInner = bvNone
      BevelOuter = bvNone
      Date = 37254.000000000000000000
      Time = 37254.000000000000000000
      DateFormat = dfLong
      TabOrder = 0
      OnCloseUp = DatePicker1CloseUp
      OnChange = DatePicker1Change
      OnDropDown = DatePicker1DropDown
    end
    object dayView1: TButton
      Left = 70
      Top = 11
      Width = 50
      Height = 22
      Caption = 'Dag'
      TabOrder = 2
      OnClick = dayView1Click
    end
    object weekView1: TButton
      Left = 9
      Top = 11
      Width = 50
      Height = 22
      Caption = 'Week'
      TabOrder = 1
      OnClick = weekView1Click
    end
  end
  object ScrollBox: TFixedScrollBox
    Left = 6
    Top = 72
    Width = 688
    Height = 415
    HorzScrollBar.Tracking = True
    VertScrollBar.Tracking = True
    Anchors = [akLeft, akTop, akRight, akBottom]
    BorderStyle = bsNone
    Color = clSkyBlue
    ParentColor = False
    TabOrder = 2
    object Shape3: TShape
      Left = 2
      Top = 2
      Width = 120
      Height = 10
      Brush.Color = clSilver
      Pen.Color = clSilver
    end
    object Shape2: TShape
      Left = 0
      Top = 0
      Width = 1
      Height = 200
    end
    object Shape1: TShape
      Left = 2
      Top = 198
      Width = 120
      Height = 1
    end
    object Shape4: TShape
      Left = 2
      Top = 0
      Width = 120
      Height = 1
    end
    object EmplRows: TStringGrid
      Left = 2
      Top = 12
      Width = 120
      Height = 185
      TabStop = False
      BorderStyle = bsNone
      ColCount = 1
      DefaultColWidth = 130
      DefaultRowHeight = 15
      DefaultDrawing = False
      FixedColor = clMedGray
      FixedCols = 0
      FixedRows = 0
      GridLineWidth = 10
      Options = [goHorzLine]
      ScrollBars = ssNone
      TabOrder = 1
      OnDblClick = EmplRowsDblClick
      OnDrawCell = EmplRowsDrawCell
      OnMouseDown = EmplRowsMouseDown
      OnMouseMove = EmplRowsMouseMove
      OnMouseUp = EmplRowsMouseUp
      OnMouseWheelDown = EmplRowsMouseWheelDown
      OnMouseWheelUp = EmplRowsMouseWheelUp
    end
    object RowGrid: TRowGrid
      Left = 122
      Width = 200
      Height = 200
      Cursor = crArrow
      Units = 36
      RowCount = 10
      RowHeight = 20
      RowLineWidth = 8
      UnitWidth = 15
      FrameWidth = 1
      CellBorderWidth = 5
      DefaultDrawing = False
      AltKeyInserts = True
      OnInsertRowCell = RowGridInsertRowCell
      OnRemoveRowCell = RowGridRemoveRowCell
      OnClearRowCell = RowGridClearRowCell
      OnDragRowCell = RowGridDragRowCell
      OnDrawRowLine = RowGridDrawRowLine
      OnDrawRowCell = RowGridDrawRowCell
      OnDrawFocusRect = RowGridDrawFocusRect
      OnDblClickRowCell = RowGridDblClickRowCell
      OnPaint = RowGridPaint
      TabOrder = 0
      Color = 13630206
      ParentColor = False
      PopupMenu = PopupMenu1
      OnContextPopup = RowGridContextPopup
      OnMouseDown = RowGridMouseDown
      OnMouseMove = RowGridMouseMove
    end
  end
  object TimeLabels: TStringGrid
    Left = 129
    Top = 56
    Width = 198
    Height = 17
    TabStop = False
    Color = clBtnFace
    ColCount = 9
    Ctl3D = False
    DefaultRowHeight = 17
    DefaultDrawing = False
    FixedCols = 0
    RowCount = 1
    FixedRows = 0
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Arial'
    Font.Style = []
    GridLineWidth = 0
    Options = []
    ParentCtl3D = False
    ParentFont = False
    ScrollBars = ssNone
    TabOrder = 1
    OnDrawCell = TimeLabelsDrawCell
    OnMouseDown = TimeLabelsMouseDown
    OnMouseMove = TimeLabelsMouseMove
  end
  object DataRows: TDataRows
    Left = 5
    Top = 400
    Width = 782
    Height = 120
    TabStop = False
    BorderStyle = bsNone
    ColCount = 16
    Ctl3D = True
    DefaultColWidth = 51
    DefaultRowHeight = 18
    DoubleBuffered = True
    RowCount = 2
    ParentCtl3D = False
    ParentDoubleBuffered = False
    ScrollBars = ssVertical
    TabOrder = 3
    Visible = False
    MetaData = 
      'ID,EMPLOYEE,FROM,UNTIL,DATE_DIFF,PATTERN,UUID,CHARGE,COLOR,APP_ID,CAN' +
      'CELLED,LASTNAME,ADDRESS,POSTCODE,CITY,INFOTEXT'
    MetaLabels = 
      'ID,WERKNEMER,VAN,"TOT EN MET",MINUTEN,PATROON,UUID,TARIEF,KLEUR,AFSPR' +
      'AAK,AFGEZEGD,NAAM,ADRES,POSTCODE,PLAATS,INFO'
    ColWidths = (
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
      0
      0)
  end
  object MainMenu1: TMainMenu
    AutoHotkeys = maManual
    Left = 416
    Top = 48
    object save1: TMenuItem
      Caption = '&Opslaan'
      Enabled = False
      Visible = False
      OnClick = save1Click
    end
    object abort1: TMenuItem
      Caption = '&Afbreken'
      Enabled = False
      Visible = False
      OnClick = abort1Click
    end
    object entriesNew1: TMenuItem
      Caption = '&Nieuw'
      Enabled = False
      Visible = False
      OnClick = entriesNew1Click
    end
    object schedule1: TMenuItem
      Caption = '&Plannen...'
      OnClick = schedule1Click
    end
    object edit1: TMenuItem
      Caption = '&Bewerken'
      object editBlock1: TMenuItem
        Caption = '&Blokkeren'
        ShortCut = 16450
        OnClick = entriesNew1Click
      end
      object editSplit1: TMenuItem
        Caption = '&Splitsen'
        ShortCut = 16467
        OnClick = editSplit1Click
      end
      object editDelete1: TMenuItem
        Caption = '&Verwijder'
        ShortCut = 16452
        OnClick = editDelete1Click
      end
      object editMove1: TMenuItem
        Caption = '&Verplaats'
        Enabled = False
        Visible = False
      end
      object editPaste1: TMenuItem
        Caption = '&Plakken'
        Enabled = False
        Visible = False
      end
      object entriesSelect1: TMenuItem
        Caption = '&Selecteer'
        Enabled = False
        Visible = False
        OnClick = entriesSelect1Click
      end
      object editCancel1: TMenuItem
        Caption = 'A&nnuleer'
        ShortCut = 27
        OnClick = editCancel1Click
      end
      object N3: TMenuItem
        Caption = '-'
      end
      object editDeselectAll1: TMenuItem
        Caption = 'Deselecteer &Alles'
        ShortCut = 16449
        OnClick = editDeselectAll1Click
      end
      object editDeselectChanged1: TMenuItem
        Caption = 'Deselecteer &Rood'
        Enabled = False
        Visible = False
        OnClick = editDeselectChanged1Click
      end
      object editSetRange1: TMenuItem
        Caption = 'Bereik Ins&tellen...'
        ShortCut = 16468
        OnClick = editSetRange1Click
      end
      object N5: TMenuItem
        Caption = '-'
      end
      object editExport1: TMenuItem
        Caption = '&Exporteer Csv'
        ShortCut = 16453
        OnClick = editExport1Click
      end
      object editImport1: TMenuItem
        Caption = '&Importeer Ical...'
        ShortCut = 16457
        OnClick = editImport1Click
      end
      object N9: TMenuItem
        Caption = '-'
      end
      object editFindEmpty1: TMenuItem
        Caption = '&Zoek Leeg...'
        ShortCut = 16454
        OnClick = editFindEmpty1Click
      end
      object editPrint1: TMenuItem
        Caption = '&Print Cellen...'
        ShortCut = 16464
        OnClick = editPrint1Click
      end
      object editMail1: TMenuItem
        Caption = '&Mail cellen...'
        ShortCut = 16461
        OnClick = editMail1Click
      end
      object N7: TMenuItem
        Caption = '-'
      end
      object editClearPattern1: TMenuItem
        Caption = 'Patroon []'
        ShortCut = 16496
        OnClick = editClearPattern1Click
      end
      object editCrossPattern1: TMenuItem
        Caption = 'Patroon xx'
        ShortCut = 16497
        OnClick = editCrossPattern1Click
      end
      object editFDiagPattern1: TMenuItem
        Caption = 'Patroon \\'
        ShortCut = 16498
        OnClick = editFDiagPattern1Click
      end
    end
    object N1: TMenuItem
      Caption = '|'
      Enabled = False
      Visible = False
    end
    object view1: TMenuItem
      Caption = 'Beel&d'
      object viewRefresh1: TMenuItem
        Caption = 'Ve&rvers'
        ShortCut = 116
        OnClick = viewRefresh1Click
      end
      object viewVertical1: TMenuItem
        AutoCheck = True
        Caption = '&Vertikaal'
        ShortCut = 16470
        OnClick = viewVertical1Click
      end
      object viewWeek1: TMenuItem
        AutoCheck = True
        Caption = '&Week/Dag'
        ShortCut = 16471
        OnClick = viewWeek1Click
      end
      object viewCancelled1: TMenuItem
        AutoCheck = True
        Caption = '&Afgezegd'
        ShortCut = 16451
        OnClick = viewCancelled1Click
      end
    end
    object archive1: TMenuItem
      Caption = '&Archief'
      OnClick = archive1Click
    end
    object pending1: TMenuItem
      Caption = '&Taken'
      OnClick = pending1Click
    end
    object tools1: TMenuItem
      Caption = '&Extra'
      object toolsCreated1: TMenuItem
        Caption = '&Gemaakt...'
        OnClick = toolsCreated1Click
      end
      object toolsEntries1: TMenuItem
        Caption = '&Cellen...'
        OnClick = toolsEntries1Click
      end
      object toolsCodes1: TMenuItem
        Caption = '&Kleuren...'
        OnClick = toolsCodes1Click
      end
      object toolsOptions1: TMenuItem
        Caption = '&Opties...'
        OnClick = options1Click
      end
    end
    object N2: TMenuItem
      Caption = '|'
      Enabled = False
    end
    object home1: TMenuItem
      Caption = '^'
      OnClick = home1Click
    end
    object entriesFirst1: TMenuItem
      Caption = '|<'
      ShortCut = 16420
      Visible = False
      OnClick = entriesFirst1Click
    end
    object entriesPrev1: TMenuItem
      Caption = '<<'
      OnClick = entriesPrev1Click
    end
    object entriesNext1: TMenuItem
      Caption = '>>'
      ShortCut = 114
      OnClick = entriesNext1Click
    end
    object entriesLast1: TMenuItem
      Caption = '>|'
      ShortCut = 16419
      Visible = False
      OnClick = entriesLast1Click
    end
    object CountText: TMenuItem
      Caption = '0 / 0'
    end
    object help1: TMenuItem
      Caption = '&Help'
      object index1: TMenuItem
        Caption = '&Index...'
        ShortCut = 112
        OnClick = index1Click
      end
      object debugMode1: TMenuItem
        Caption = '&Debug mode'
        OnClick = debugClick
      end
      object emailSupport1: TMenuItem
        Caption = '&Email support'
        OnClick = emailSupport1Click
      end
      object about1: TMenuItem
        Caption = '&About'
        OnClick = aboutClick
      end
    end
  end
  object RefreshInfo: TTimer
    Interval = 600000
    OnTimer = RefreshInfoTimer
    Left = 448
    Top = 48
  end
  object PopupMenu1: TPopupMenu
    Left = 512
    Top = 48
    object popupBlock1: TMenuItem
      Caption = '&Blokkeren'
      OnClick = entriesNew1Click
    end
    object popupSplit1: TMenuItem
      Caption = '&Splitsen'
      OnClick = editSplit1Click
    end
    object popupDelete1: TMenuItem
      Caption = '&Verwijder'
      OnClick = popupDelete1Click
    end
    object popupSelectEntry1: TMenuItem
      Caption = '&Selecteer'
      OnClick = popupSelectEntry1Click
    end
    object N4: TMenuItem
      Caption = '-'
    end
    object popupDeselectAll1: TMenuItem
      Caption = 'Deselecteer &Alles'
      OnClick = editDeselectAll1Click
    end
    object popupDeselectRed1: TMenuItem
      Caption = 'Deselecteer &Rood'
      Enabled = False
      Visible = False
      OnClick = editDeselectChanged1Click
    end
    object popupSetRange1: TMenuItem
      Caption = 'Bereik Ins&tellen...'
      OnClick = editSetRange1Click
    end
    object N6: TMenuItem
      Caption = '-'
    end
    object popupFindEmpty1: TMenuItem
      Caption = '&Zoek Leeg'
      OnClick = editFindEmpty1Click
    end
    object popupPrintEntries1: TMenuItem
      Caption = '&Print Cellen'
      OnClick = editPrint1Click
    end
    object popupMailEntries1: TMenuItem
      Caption = '&Mail Cellen...'
      OnClick = editMail1Click
    end
    object N8: TMenuItem
      Caption = '-'
    end
    object popupClearPattern1: TMenuItem
      Caption = 'Patroon []'
      OnClick = popupClearPattern1Click
    end
    object popupCrossPattern1: TMenuItem
      Caption = 'Patroon xx'
      OnClick = popupCrossPattern1Click
    end
    object popupFDiagPattern1: TMenuItem
      Caption = 'Patroon \\'
      OnClick = popupFDiagPattern1Click
    end
  end
  object KeepAlive: TTimer
    Interval = 60000
    OnTimer = KeepAliveTimer
    Left = 480
    Top = 48
  end
  object OpenDialog: TOpenDialog
    Options = [ofHideReadOnly, ofNoChangeDir, ofFileMustExist, ofEnableSizing]
    Left = 544
    Top = 48
  end
end
