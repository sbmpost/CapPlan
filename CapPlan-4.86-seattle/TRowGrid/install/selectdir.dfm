object Fselect: TFselect
  Left = 652
  Top = 257
  BorderStyle = bsDialog
  Caption = 'Select directory'
  ClientHeight = 283
  ClientWidth = 291
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  PixelsPerInch = 96
  TextHeight = 13
  object DirectoryListBox1: TDirectoryListBox
    Left = 0
    Top = 32
    Width = 289
    Height = 249
    ItemHeight = 16
    TabOrder = 0
  end
  object DriveComboBox1: TDriveComboBox
    Left = 0
    Top = 0
    Width = 289
    Height = 19
    TabOrder = 1
    OnChange = DriveComboBox1Change
  end
end
