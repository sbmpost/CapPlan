inherited Foptions1: TFoptions1
  Left = 359
  Top = 139
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Options'
  ClientHeight = 430
  ClientWidth = 537
  Menu = MainMenu1
  OldCreateOrder = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 16
  object GroupBox5: TGroupBox [0]
    Left = 0
    Top = 325
    Width = 537
    Height = 105
    Caption = 'values'
    TabOrder = 5
    object Label4: TLabel
      Left = 16
      Top = 20
      Width = 90
      Height = 16
      Caption = 'printer copies'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Verdana'
      Font.Style = []
      ParentFont = False
    end
    object Label5: TLabel
      Left = 218
      Top = 76
      Width = 123
      Height = 16
      Caption = 'default entry code'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Verdana'
      Font.Style = []
      ParentFont = False
    end
    object Label7: TLabel
      Left = 218
      Top = 20
      Width = 199
      Height = 16
      Caption = 'postcode (c=city,a=addr,x=?)'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Verdana'
      Font.Style = []
      ParentFont = False
    end
    object Label6: TLabel
      Left = 16
      Top = 76
      Width = 124
      Height = 16
      Caption = 'telephonenr length'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Verdana'
      Font.Style = []
      ParentFont = False
    end
    object Label8: TLabel
      Left = 16
      Top = 48
      Width = 105
      Height = 16
      Caption = 'printer font size'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Verdana'
      Font.Style = []
      ParentFont = False
    end
    object Label9: TLabel
      Left = 218
      Top = 48
      Width = 251
      Height = 16
      Caption = 'connection keepalive time (0=disable)'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Verdana'
      Font.Style = []
      ParentFont = False
    end
    object CopiesEdit: TMaskEdit
      Left = 168
      Top = 18
      Width = 33
      Height = 20
      AutoSize = False
      EditMask = '99;0; '
      MaxLength = 2
      TabOrder = 0
      Text = ''
      OnChange = EditChange
      OnExit = CopiesEditExit
    end
    object ColorBox: TColorBox
      Left = 368
      Top = 74
      Width = 153
      Height = 22
      Selected = clScrollBar
      Style = []
      Enabled = False
      ItemHeight = 16
      TabOrder = 5
      OnChange = ColorBoxChange
      OnExit = ColorBoxExit
    end
    object PostcodeEdit: TEdit
      Left = 440
      Top = 18
      Width = 81
      Height = 20
      AutoSize = False
      CharCase = ecLowerCase
      TabOrder = 1
      OnChange = EditChange
      OnExit = PostcodeEditExit
    end
    object TelephoneEdit: TMaskEdit
      Left = 168
      Top = 74
      Width = 33
      Height = 20
      AutoSize = False
      EditMask = '99;0; '
      MaxLength = 2
      TabOrder = 4
      Text = ''
      OnChange = EditChange
      OnExit = TelephoneEditExit
    end
    object FontEdit: TMaskEdit
      Left = 168
      Top = 46
      Width = 33
      Height = 20
      AutoSize = False
      EditMask = '99;0; '
      MaxLength = 2
      TabOrder = 2
      Text = ''
      OnChange = EditChange
      OnExit = FontEditExit
    end
    object TimeEdit: TMaskEdit
      Left = 488
      Top = 46
      Width = 33
      Height = 20
      AutoSize = False
      EditMask = '99;0; '
      MaxLength = 2
      TabOrder = 3
      Text = ''
      OnChange = EditChange
      OnExit = TimeEditExit
    end
  end
  object GroupBox3: TGroupBox [1]
    Left = 0
    Top = 141
    Width = 265
    Height = 185
    Caption = 'general settings'
    TabOrder = 3
    object CurrentBox: TFixedCheckBox
      Left = 16
      Top = 22
      Width = 241
      Height = 17
      Caption = 'make current schedule default'
      TabOrder = 0
      OnClick = CurrentBoxClick
    end
    object SearchBox: TFixedCheckBox
      Left = 16
      Top = 114
      Width = 241
      Height = 17
      Caption = 'search for persons while typing'
      TabOrder = 4
      OnClick = SearchBoxClick
    end
    object ClearOnNewBox: TFixedCheckBox
      Left = 16
      Top = 91
      Width = 241
      Height = 17
      Caption = 'clear selection on dragging new'
      TabOrder = 3
      OnClick = ClearOnNewBoxClick
    end
    object ClearAfterBox: TFixedCheckBox
      Left = 16
      Top = 68
      Width = 241
      Height = 17
      Caption = 'clear selection after edit entries'
      TabOrder = 2
      OnClick = ClearAfterBoxClick
    end
    object RememberBox: TFixedCheckBox
      Left = 16
      Top = 137
      Width = 241
      Height = 17
      Caption = 'remember selected person'
      TabOrder = 5
      OnClick = RememberBoxClick
    end
    object WeekBox: TFixedCheckBox
      Left = 16
      Top = 159
      Width = 241
      Height = 17
      Caption = 'default value of '#39'week/day'#39
      TabOrder = 6
      OnClick = WeekBoxClick
    end
    object RefreshBox: TFixedCheckBox
      Left = 16
      Top = 45
      Width = 241
      Height = 17
      Caption = 'refresh entries every 10 minutes'
      TabOrder = 1
      OnClick = RefreshBoxClick
    end
  end
  object GroupBox4: TGroupBox [2]
    Left = 272
    Top = 141
    Width = 265
    Height = 185
    Caption = 'documents'
    TabOrder = 4
    object LockBox: TFixedCheckBox
      Left = 16
      Top = 22
      Width = 241
      Height = 17
      Caption = 'lock current pending list'
      TabOrder = 0
      OnClick = LockBoxClick
    end
    object UnsignedBox: TFixedCheckBox
      Left = 16
      Top = 45
      Width = 241
      Height = 17
      Caption = 'highlight unsigned documents'
      TabOrder = 1
      OnClick = UnsignedBoxClick
    end
    object ScheduleBox: TFixedCheckBox
      Left = 16
      Top = 91
      Width = 241
      Height = 17
      Caption = 'default value of '#39'schedule'#39
      TabOrder = 3
      OnClick = ScheduleBoxClick
    end
    object RepurchaseBox: TFixedCheckBox
      Left = 16
      Top = 114
      Width = 241
      Height = 17
      Caption = 'default value of '#39'repurchase'#39
      TabOrder = 4
      OnClick = RepurchaseBoxClick
    end
    object InvoiceBox: TFixedCheckBox
      Left = 16
      Top = 137
      Width = 241
      Height = 17
      Caption = 'default value of '#39'invoice'#39
      TabOrder = 5
      OnClick = InvoiceBoxClick
    end
    object SignedBox: TFixedCheckBox
      Left = 16
      Top = 68
      Width = 241
      Height = 17
      Caption = 'schedule signed documents'
      TabOrder = 2
      OnClick = SignedBoxClick
    end
    object MacroBox: TFixedCheckBox
      Left = 16
      Top = 159
      Width = 241
      Height = 17
      Caption = 'macros: ToSchedule, To...'
      TabOrder = 6
      OnClick = MacroBoxClick
    end
  end
  object GroupBox2: TGroupBox
    Left = 0
    Top = 48
    Width = 537
    Height = 94
    Caption = 'schedule settings'
    TabOrder = 2
    object TimesBox: TFixedCheckBox
      Left = 288
      Top = 22
      Width = 241
      Height = 17
      Caption = 'default value of '#39'output times'#39
      TabOrder = 1
      OnClick = TimesBoxClick
    end
    object AlternateBox: TFixedCheckBox
      Left = 16
      Top = 68
      Width = 241
      Height = 17
      Caption = 'alternate entry colors each week'
      TabOrder = 4
      OnClick = AlternateBoxClick
    end
    object InfoBox: TFixedCheckBox
      Left = 16
      Top = 45
      Width = 241
      Height = 17
      Caption = 'entry: city / lastname / address'
      TabOrder = 2
      OnClick = InfoBoxClick
    end
    object VerticalBox: TFixedCheckBox
      Left = 288
      Top = 45
      Width = 241
      Height = 17
      Caption = 'default value of '#39'vertical view'#39
      TabOrder = 3
      OnClick = VerticalBoxClick
    end
    object RestrictBox: TFixedCheckBox
      Left = 288
      Top = 68
      Width = 241
      Height = 17
      Caption = 'appointments: '#39'restrict updates'#39
      TabOrder = 5
      OnClick = RestrictBoxClick
    end
    object PatternBox: TFixedCheckBox
      Left = 16
      Top = 22
      Width = 241
      Height = 17
      Caption = 'entry: hide xx and \\ patterns'
      TabOrder = 0
      OnClick = PatternBoxClick
    end
  end
  object GroupBox1: TGroupBox
    Left = 0
    Top = 0
    Width = 537
    Height = 49
    Caption = 'mainscreen'
    Color = clBtnFace
    ParentColor = False
    TabOrder = 1
    object Label1: TLabel
      Left = 16
      Top = 20
      Width = 95
      Height = 16
      Caption = 'rows (0=auto)'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Verdana'
      Font.Style = []
      ParentFont = False
    end
    object Label2: TLabel
      Left = 192
      Top = 20
      Width = 102
      Height = 16
      Caption = 'number of days'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Verdana'
      Font.Style = []
      ParentFont = False
    end
    object Label3: TLabel
      Left = 384
      Top = 20
      Width = 93
      Height = 16
      Caption = 'units per label'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Verdana'
      Font.Style = []
      ParentFont = False
    end
    object RowsEdit: TMaskEdit
      Left = 128
      Top = 18
      Width = 33
      Height = 20
      AutoSize = False
      EditMask = '99;0; '
      MaxLength = 2
      TabOrder = 0
      Text = ''
      OnChange = ViewChange
      OnExit = RowsEditExit
    end
    object DaysEdit: TMaskEdit
      Left = 304
      Top = 18
      Width = 33
      Height = 20
      AutoSize = False
      EditMask = '999;0; '
      MaxLength = 3
      TabOrder = 1
      Text = ''
      OnChange = ViewChange
      OnExit = DaysEditExit
    end
    object LabelsEdit: TMaskEdit
      Left = 488
      Top = 18
      Width = 33
      Height = 20
      AutoSize = False
      EditMask = '99;0; '
      MaxLength = 2
      TabOrder = 2
      Text = ''
      OnChange = ViewChange
      OnExit = LabelsEditExit
    end
  end
  object FontDialog1: TFontDialog
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    Left = 464
    Top = 65520
  end
  object MainMenu1: TMainMenu
    Left = 496
    Top = 65520
    object save1: TMenuItem
      Caption = '&Save'
      OnClick = save1Click
    end
    object Refresh1: TMenuItem
      Caption = '&Refresh'
      ShortCut = 116
      OnClick = Refresh1Click
    end
    object Entryfont1: TMenuItem
      Caption = '&Entry font'
      OnClick = Entryfont1Click
    end
    object Folders1: TMenuItem
      Caption = '&Folders'
      object DocumentsPath1: TMenuItem
        Caption = '&Documents path'
        OnClick = DocumentsPath1Click
      end
      object TemplatesPath1: TMenuItem
        Caption = '&Templates path'
        OnClick = TemplatesPath1Click
      end
    end
  end
end
