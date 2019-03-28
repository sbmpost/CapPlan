inherited Fmdetail1: TFmdetail1
  Left = 187
  Top = 26
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Fmdetail1'
  Color = clSkyBlue
  OnKeyDown = FormKeyDown
  OnMouseWheelDown = FormMouseWheelDown
  OnMouseWheelUp = FormMouseWheelUp
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 16
  inherited Panel1: TPanel
    TabOrder = 3
  end
  object RowControl: TRowControl
    Top = 8
    Width = 992
    Height = 670
    MultiLine = True
    TabOrder = 0
    TabStop = False
    OnChanging = RowControlChanging
  end
  object CancelButton: TButton
    Left = 910
    Top = 5
    Width = 75
    Height = 22
    Caption = 'Annuleren'
    TabOrder = 2
    OnClick = CancelButtonClick
  end
  object OkButton: TButton
    Left = 815
    Top = 5
    Width = 75
    Height = 22
    Caption = 'OK'
    ModalResult = 1
    TabOrder = 1
  end
end
