inherited Frange1: TFrange1
  Left = 417
  Top = 195
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Set range'
  ClientHeight = 400
  ClientWidth = 416
  OldCreateOrder = False
  OnMouseWheelDown = FormMouseWheelDown
  OnMouseWheelUp = FormMouseWheelUp
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 16
  object Label1: TLabel [0]
    Left = 7
    Top = 86
    Width = 130
    Height = 16
    Caption = 'Repeat range every'
  end
  object Label2: TLabel [1]
    Left = 183
    Top = 86
    Width = 136
    Height = 16
    Caption = '&weeks for as long as'
    FocusControl = CountEdit
  end
  object Label3: TLabel [2]
    Left = 364
    Top = 86
    Width = 41
    Height = 16
    Caption = 'w&eeks'
    FocusControl = TotalEdit
  end
  inherited Panel1: TPanel
    TabOrder = 11
  end
  object DatePicker1: TDateTimePicker
    Left = 7
    Top = 8
    Width = 256
    Height = 24
    Date = 38287.000000000000000000
    Time = 38287.000000000000000000
    DateFormat = dfLong
    TabOrder = 0
    OnCloseUp = DatePicker1CloseUp
    OnChange = DatePicker1Change
    OnDropDown = DatePickerDropDown
    OnKeyDown = DatePickerKeyDown
  end
  object DatePicker2: TDateTimePicker
    Left = 7
    Top = 48
    Width = 256
    Height = 24
    Date = 38287.000000000000000000
    Time = 38287.000000000000000000
    DateFormat = dfLong
    TabOrder = 2
    OnCloseUp = DatePicker2CloseUp
    OnChange = DatePicker2Change
    OnDropDown = DatePickerDropDown
    OnKeyDown = DatePickerKeyDown
  end
  object CancelButton: TButton
    Left = 293
    Top = 364
    Width = 75
    Height = 25
    Caption = '&Cancel'
    ModalResult = 5
    TabOrder = 10
  end
  object CheckListBox1: TCheckListBox
    Left = 7
    Top = 116
    Width = 401
    Height = 212
    ItemHeight = 16
    TabOrder = 6
  end
  object TimePicker1: TDateTimePicker
    Left = 303
    Top = 8
    Width = 105
    Height = 24
    Date = 38288.000000000000000000
    Format = 'H:mm'
    Time = 38288.000000000000000000
    DateMode = dmUpDown
    Kind = dtkTime
    TabOrder = 1
    OnChange = TimePickerChange
  end
  object TimePicker2: TDateTimePicker
    Left = 303
    Top = 48
    Width = 105
    Height = 24
    Date = 38288.000000000000000000
    Format = 'H:mm'
    Time = 38288.000000000000000000
    DateMode = dmUpDown
    Kind = dtkTime
    TabOrder = 3
    OnChange = TimePickerChange
  end
  object SelectBox: TFixedCheckBox
    Left = 10
    Top = 334
    Width = 126
    Height = 17
    Caption = 'Select &all'
    TabOrder = 7
    OnClick = SelectBoxClick
  end
  object CountEdit: TMaskEdit
    Left = 143
    Top = 84
    Width = 32
    Height = 20
    AutoSize = False
    EditMask = '99;0; '
    MaxLength = 2
    TabOrder = 4
    Text = ''
    OnExit = CountEditExit
  end
  object TotalEdit: TMaskEdit
    Left = 327
    Top = 84
    Width = 32
    Height = 20
    AutoSize = False
    EditMask = '99;0; '
    MaxLength = 2
    TabOrder = 5
    Text = ''
    OnExit = TotalEditExit
  end
  object OkButton: TButton
    Left = 49
    Top = 364
    Width = 75
    Height = 25
    Caption = '&OK'
    ModalResult = 1
    TabOrder = 9
  end
  object CheckBox: TFixedCheckBox
    Left = 151
    Top = 334
    Width = 126
    Height = 17
    Caption = '&Fit range'
    TabOrder = 8
  end
end
