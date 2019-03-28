inherited SetupDownload: TSetupDownload
  Left = 252
  Top = 197
  Caption = 'SetupDownload'
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object SQLstatus: TLabel [2]
    Left = 192
    Top = 240
    Width = 62
    Height = 13
    Caption = 'SERVER => '
  end
  object ODBCstatus: TLabel [3]
    Left = 192
    Top = 192
    Width = 62
    Height = 13
    Caption = 'DRIVER  => '
  end
  object Memo1: TMemo
    Left = 177
    Top = 0
    Width = 432
    Height = 177
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
      '  '
      '  This application uses an SQL database server to store its'
      '  data. It also uses an ODBC driver to communicate with the'
      '  server. When you click next, the installer will download and'
      '  install the server and driver.'
      '')
    ParentFont = False
    ReadOnly = True
    TabOrder = 3
  end
  object ODBCprogress: TProgressBar
    Left = 192
    Top = 216
    Width = 409
    Height = 18
    TabOrder = 4
  end
  object SQLprogress: TProgressBar
    Left = 192
    Top = 264
    Width = 409
    Height = 18
    TabOrder = 5
  end
end
