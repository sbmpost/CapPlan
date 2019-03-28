inherited SetupConfig: TSetupConfig
  Left = 306
  Top = 235
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 184
    Top = 8
    Width = 417
    Height = 137
    Caption = 'Installation locations'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 3
    object DATABASEHOST: TLabeledEdit
      Left = 16
      Top = 48
      Width = 305
      Height = 24
      Hint = 
        'Default: '#39'localhost'#39' (change to 127.0.0.1 for limited user accou' +
        'nt)'
      EditLabel.Width = 86
      EditLabel.Height = 16
      EditLabel.Caption = 'database host'
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
      LabelSpacing = 10
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      Text = 'localhost'
      OnKeyPress = EditKeyPress
    end
    object INSTALLROOT: TLabeledEdit
      Left = 16
      Top = 105
      Width = 305
      Height = 24
      Hint = 
        'The installer will copy the program files to this location (chan' +
        'ge to a local path for limited user account)'
      EditLabel.Width = 151
      EditLabel.Height = 16
      EditLabel.Caption = 'planner installation PATH'
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
      LabelSpacing = 10
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
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
      TabOrder = 2
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
      Hint = 'Required for proper installation'
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
      LabelSpacing = 10
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      OnChange = UserInfoChange
      OnKeyPress = EditKeyPress
    end
    object PASSWORD: TLabeledEdit
      Left = 16
      Top = 105
      Width = 305
      Height = 24
      Hint = 'This field is case sensitive'
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
      LabelSpacing = 10
      MaxLength = 12
      ParentFont = False
      ParentShowHint = False
      PasswordChar = '*'
      ShowHint = True
      TabOrder = 1
      OnChange = UserInfoChange
      OnKeyPress = EditKeyPress
    end
  end
end
