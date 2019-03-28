inherited SetupWelcome: TSetupWelcome
  Left = 354
  Top = 323
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object welcome: TMemo [2]
    Left = 177
    Top = 0
    Width = 432
    Height = 296
    TabStop = False
    BevelInner = bvNone
    BevelOuter = bvNone
    BorderStyle = bsNone
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    Lines.Strings = (
      ''
      '  Welcome to the CapPlan installation program.'
      ''
      '  CapPlan enables you to schedule employees'
      '  (or other resources). It allows for multiple users'
      '  working at the same time. More information is'
      '  available at http://www.postware.nl'
      ''
      ''
      ''
      ''
      ''
      '  This installer will install the CapPlan application.'
      '  To continue click next.')
    ParentFont = False
    ReadOnly = True
    TabOrder = 0
  end
  inherited Back: TButton
    TabOrder = 1
  end
  inherited Next: TButton
    TabOrder = 2
  end
  inherited Cancel: TButton
    TabOrder = 3
  end
end
