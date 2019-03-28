inherited Fdetails1: TFdetails1
  Left = 321
  Top = 218
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Details'
  ClientHeight = 207
  ClientWidth = 444
  Font.Name = 'MS Sans Serif'
  OldCreateOrder = False
  Position = poDefaultPosOnly
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 16
  inherited Panel1: TPanel
    TabOrder = 3
  end
  object PageControl1: TPageControl
    Left = 8
    Top = 8
    Width = 430
    Height = 191
    ActivePage = ATTRIBUTES
    TabOrder = 0
    TabStop = False
    object ATTRIBUTES: TTabSheet
      Caption = 'Attributen'
      object Label2: TLabel
        Left = 16
        Top = 12
        Width = 67
        Height = 16
        Caption = 'gemaakt door:'
      end
      object CreatedBy: TLabel
        Left = 112
        Top = 12
        Width = 68
        Height = 16
        Caption = 'string value'
      end
      object Label3: TLabel
        Left = 16
        Top = 30
        Width = 67
        Height = 16
        Caption = 'gemaakt op:'
      end
      object Created: TLabel
        Left = 112
        Top = 30
        Width = 63
        Height = 16
        Caption = 'date value'
      end
      object AttributeBox1: TFixedCheckBox
        Left = 16
        Top = 58
        Width = 273
        Height = 17
        Caption = 'AttributeBox1'
        TabOrder = 0
      end
      object AttributeBox2: TFixedCheckBox
        Left = 16
        Top = 80
        Width = 273
        Height = 17
        Caption = 'AttributeBox2'
        TabOrder = 1
      end
      object AttributeBox3: TFixedCheckBox
        Left = 24
        Top = 102
        Width = 265
        Height = 17
        Caption = 'AttributeBox3'
        TabOrder = 2
      end
      object AttributeBox4: TFixedCheckBox
        Left = 16
        Top = 124
        Width = 273
        Height = 17
        Caption = 'AttributeBox4'
        TabOrder = 3
      end
    end
    object HISTORY: TTabSheet
      Caption = 'Geschiedenis'
      object PlannedLabel: TLabel
        Left = 16
        Top = 42
        Width = 52
        Height = 16
        Caption = 'gepland:'
      end
      object Planned: TLabel
        Left = 104
        Top = 42
        Width = 63
        Height = 16
        Caption = 'date value'
        Color = clBtnFace
        ParentColor = False
      end
      object OrderedLabel: TLabel
        Left = 16
        Top = 70
        Width = 51
        Height = 16
        Caption = 'besteld:'
      end
      object Ordered: TLabel
        Left = 104
        Top = 70
        Width = 63
        Height = 16
        Caption = 'date value'
      end
      object ChargedLabel: TLabel
        Left = 16
        Top = 98
        Width = 53
        Height = 16
        Caption = 'gefactureerd:'
      end
      object Charged: TLabel
        Left = 104
        Top = 98
        Width = 63
        Height = 16
        Caption = 'date value'
      end
    end
  end
  object CancelButton: TButton
    Left = 342
    Top = 133
    Width = 75
    Height = 25
    Caption = '&Annuleren'
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
    Left = 342
    Top = 85
    Width = 75
    Height = 25
    Caption = '&OK'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Verdana'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    OnClick = OkButtonClick
  end
end
