inherited Fprint1: TFprint1
  Caption = 'Print entries'
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 16
  inherited Label1: TLabel
    Visible = False
  end
  inherited Label2: TLabel
    Visible = False
  end
  inherited Label3: TLabel
    Width = 43
    Caption = 'Copi&es'
  end
  inherited Panel1: TPanel
    TabOrder = 12
  end
  inherited DatePicker1: TDateTimePicker
    Enabled = False
  end
  inherited DatePicker2: TDateTimePicker
    Enabled = False
  end
  object PreviewButton: TButton
    Left = 171
    Top = 364
    Width = 75
    Height = 25
    Caption = '&Preview'
    TabOrder = 10
    OnClick = PreviewButtonClick
  end
  inherited CancelButton: TButton
    TabOrder = 11
  end
  inherited TimePicker1: TDateTimePicker
    Enabled = False
  end
  inherited TimePicker2: TDateTimePicker
    Enabled = False
  end
  inherited CountEdit: TMaskEdit
    Visible = False
  end
  inherited CheckBox: TFixedCheckBox
    Caption = 'Output &times'
  end
end
