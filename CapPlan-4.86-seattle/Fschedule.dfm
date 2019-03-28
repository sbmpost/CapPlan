inherited Fschedule1: TFschedule1
  Left = 444
  Top = 263
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Plannen'
  ClientHeight = 302
  ClientWidth = 368
  OldCreateOrder = False
  Position = poDefaultPosOnly
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 16
  object Label2: TLabel [0]
    Left = 8
    Top = 24
    Width = 21
    Height = 25
    Caption = '1.'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -21
    Font.Name = 'Verdana'
    Font.Style = []
    ParentFont = False
  end
  object Shape3: TShape [1]
    Left = 190
    Top = 35
    Width = 22
    Height = 2
  end
  object Shape1: TShape [2]
    Left = 190
    Top = 67
    Width = 91
    Height = 2
    Visible = False
  end
  object Shape2: TShape [3]
    Left = 280
    Top = 49
    Width = 2
    Height = 20
    Visible = False
  end
  object Shape4: TShape [4]
    Left = 190
    Top = 115
    Width = 22
    Height = 2
  end
  object Label3: TLabel [5]
    Left = 8
    Top = 104
    Width = 21
    Height = 25
    Caption = '2.'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -21
    Font.Name = 'Verdana'
    Font.Style = []
    ParentFont = False
  end
  object Label4: TLabel [6]
    Left = 8
    Top = 184
    Width = 21
    Height = 25
    Caption = '3.'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -21
    Font.Name = 'Verdana'
    Font.Style = []
    ParentFont = False
  end
  object Shape5: TShape [7]
    Left = 190
    Top = 195
    Width = 22
    Height = 2
  end
  inherited Panel1: TPanel
    TabOrder = 9
  end
  object selectPerson: TButton
    Left = 30
    Top = 24
    Width = 161
    Height = 25
    Caption = 'Selecteer &persoon   '
    TabOrder = 0
    OnClick = selectPersonClick
  end
  object Edit1: TEdit
    Left = 212
    Top = 24
    Width = 133
    Height = 25
    TabStop = False
    AutoSize = False
    ReadOnly = True
    TabOrder = 1
  end
  object selectDocument: TButton
    Left = 30
    Top = 56
    Width = 161
    Height = 25
    Caption = 'Selecteer &document'
    TabOrder = 2
    Visible = False
    OnClick = selectDocumentClick
  end
  object Edit2: TEdit
    Left = 212
    Top = 104
    Width = 133
    Height = 25
    TabStop = False
    AutoSize = False
    ReadOnly = True
    TabOrder = 4
  end
  object selectAppointment: TButton
    Left = 30
    Top = 104
    Width = 161
    Height = 25
    Caption = '&Selecteer afspraak  '
    TabOrder = 3
    OnClick = selectAppointmentClick
  end
  object selectEntries: TButton
    Left = 30
    Top = 184
    Width = 161
    Height = 25
    Caption = 'Selecteer &cellen      '
    TabOrder = 5
    OnClick = selectEntriesClick
  end
  object Entries: TEdit
    Left = 212
    Top = 184
    Width = 133
    Height = 25
    TabStop = False
    AutoSize = False
    ReadOnly = True
    TabOrder = 6
    Text = 'Cellen:'
  end
  object AbortButton: TButton
    Left = 212
    Top = 248
    Width = 75
    Height = 25
    Caption = '&Afbreken'
    ModalResult = 5
    TabOrder = 8
    OnClick = AbortButtonClick
  end
  object FinishButton: TButton
    Left = 84
    Top = 248
    Width = 75
    Height = 25
    Caption = '&Voltooien'
    TabOrder = 7
    OnClick = FinishButtonClick
  end
end
