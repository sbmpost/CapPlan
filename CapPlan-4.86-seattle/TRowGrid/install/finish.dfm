inherited SetupFinish: TSetupFinish
  Left = 252
  Top = 252
  OldCreateOrder = True
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Status: TLabel [2]
    Left = 192
    Top = 240
    Width = 30
    Height = 13
    Caption = 'Status'
  end
  object ProgressBar1: TProgressBar
    Left = 192
    Top = 264
    Width = 409
    Height = 18
    Min = 0
    Max = 100
    TabOrder = 3
  end
  object SETTINGS: TMemo
    Left = 177
    Top = 0
    Width = 432
    Height = 217
    TabStop = False
    BevelInner = bvNone
    BevelOuter = bvNone
    BorderStyle = bsNone
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 4
  end
end
