inherited Fcomplete1: TFcomplete1
  Left = 394
  Top = 197
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Completion'
  ClientHeight = 200
  ClientWidth = 444
  Font.Name = 'MS Sans Serif'
  OldCreateOrder = False
  Position = poOwnerFormCenter
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 16
  inherited Panel1: TPanel
    TabOrder = 3
  end
  object CancelButton: TButton
    Left = 242
    Top = 166
    Width = 75
    Height = 25
    Caption = '&Cancel'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Verdana'
    Font.Style = []
    ParentFont = False
    TabOrder = 2
    OnClick = CancelButtonClick
  end
  object OkButton: TButton
    Left = 130
    Top = 166
    Width = 75
    Height = 25
    Caption = '&OK'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Verdana'
    Font.Style = []
    ModalResult = 1
    ParentFont = False
    TabOrder = 1
  end
  object GroupBox1: TGroupBox
    Left = 10
    Top = 4
    Width = 425
    Height = 150
    Caption = 'pending lists'
    TabOrder = 0
    object RepurchaseBox: TFixedCheckBox
      Left = 40
      Top = 72
      Width = 161
      Height = 17
      Caption = 'To re&purchase'
      TabOrder = 1
    end
    object InvoiceBox: TFixedCheckBox
      Left = 40
      Top = 98
      Width = 161
      Height = 17
      Caption = 'To &invoice'
      TabOrder = 2
    end
    object RemoveItemBox: TFixedCheckBox
      Left = 40
      Top = 46
      Width = 161
      Height = 17
      Caption = '&Remove item'
      TabOrder = 0
    end
  end
end
