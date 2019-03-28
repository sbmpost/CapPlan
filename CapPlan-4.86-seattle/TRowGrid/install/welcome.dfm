inherited SetupWelcome: TSetupWelcome
  Left = 352
  Top = 321
  OldCreateOrder = True
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
      ''
      '  So I actually released another version of the RowGrid.'
      '  This version has been optimized for drawing RowCells'
      '  and adds cell swap support. The component works in'
      '  any environment that can import ActiveX components'
      '  (delphi, lazarus, word, IE...). It comes with C++ source,'
      '  binaries and example programs. Also included is a java'
      '  and a pascal port (see the RowGridJ & lazarus folders)'
      ''
      '  This installer will install the RowGrid component. After the'
      '  installer completes, refer to the file readme.txt to complete'
      '  the ActiveX installation. To continue click next.')
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
