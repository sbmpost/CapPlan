inherited SetupConfig: TSetupConfig
  Left = 304
  Top = 233
  OldCreateOrder = True
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 184
    Top = 8
    Width = 417
    Height = 137
    Caption = 'Installation directories'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 3
    object BCB6ROOT: TLabeledEdit
      Left = 16
      Top = 48
      Width = 305
      Height = 24
      EditLabel.Width = 168
      EditLabel.Height = 16
      EditLabel.Caption = 'Borland C++ Builder 6 PATH'
      EditLabel.Font.Charset = DEFAULT_CHARSET
      EditLabel.Font.Color = clWindowText
      EditLabel.Font.Height = -13
      EditLabel.Font.Name = 'MS Sans Serif'
      EditLabel.Font.Style = []
      EditLabel.ParentFont = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      LabelPosition = lpAbove
      LabelSpacing = 10
      ParentFont = False
      TabOrder = 0
      OnKeyPress = EditKeyPress
    end
    object Button1: TButton
      Left = 352
      Top = 48
      Width = 41
      Height = 25
      Caption = '..\..'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Microsoft Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 1
      OnClick = Button1Click
    end
    object INSTALLROOT: TLabeledEdit
      Left = 16
      Top = 105
      Width = 305
      Height = 24
      EditLabel.Width = 158
      EditLabel.Height = 16
      EditLabel.Caption = 'RowGrid installation PATH'
      EditLabel.Font.Charset = DEFAULT_CHARSET
      EditLabel.Font.Color = clWindowText
      EditLabel.Font.Height = -13
      EditLabel.Font.Name = 'MS Sans Serif'
      EditLabel.Font.Style = []
      EditLabel.ParentFont = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      LabelPosition = lpAbove
      LabelSpacing = 10
      ParentFont = False
      TabOrder = 2
      OnKeyPress = EditKeyPress
    end
    object Button2: TButton
      Left = 352
      Top = 104
      Width = 41
      Height = 25
      Caption = '..\..'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Microsoft Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 3
      OnClick = Button2Click
    end
  end
  object GroupBox2: TGroupBox
    Left = 184
    Top = 152
    Width = 417
    Height = 137
    Caption = 'User information (which you received by mail)'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 4
    Visible = False
    object USERNAME: TLabeledEdit
      Left = 16
      Top = 49
      Width = 305
      Height = 24
      CharCase = ecUpperCase
      EditLabel.Width = 63
      EditLabel.Height = 16
      EditLabel.Caption = 'Username'
      EditLabel.Font.Charset = DEFAULT_CHARSET
      EditLabel.Font.Color = clWindowText
      EditLabel.Font.Height = -13
      EditLabel.Font.Name = 'MS Sans Serif'
      EditLabel.Font.Style = []
      EditLabel.ParentFont = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      LabelPosition = lpAbove
      LabelSpacing = 10
      ParentFont = False
      TabOrder = 0
      OnChange = UserInfoChange
      OnKeyPress = EditKeyPress
    end
    object PASSWORD: TLabeledEdit
      Left = 16
      Top = 105
      Width = 305
      Height = 24
      CharCase = ecLowerCase
      EditLabel.Width = 60
      EditLabel.Height = 16
      EditLabel.Caption = 'Password'
      EditLabel.Font.Charset = DEFAULT_CHARSET
      EditLabel.Font.Color = clWindowText
      EditLabel.Font.Height = -13
      EditLabel.Font.Name = 'MS Sans Serif'
      EditLabel.Font.Style = []
      EditLabel.ParentFont = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      LabelPosition = lpAbove
      LabelSpacing = 10
      MaxLength = 8
      ParentFont = False
      PasswordChar = '*'
      TabOrder = 1
      OnChange = UserInfoChange
      OnKeyPress = EditKeyPress
    end
  end
end
