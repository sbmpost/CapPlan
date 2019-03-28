inherited Fmail1: TFmail1
  Caption = 'Mail Entries'
  PixelsPerInch = 96
  TextHeight = 16
  inherited Label3: TLabel
    Visible = False
  end
  inherited Panel1: TPanel
    TabOrder = 13
  end
  inherited CancelButton: TButton
    TabOrder = 12
  end
  inherited TotalEdit: TMaskEdit
    Visible = False
  end
  inherited OkButton: TButton
    TabOrder = 10
  end
  inherited PreviewButton: TButton
    TabOrder = 11
  end
  object AttachIcalBox: TFixedCheckBox
    Left = 279
    Top = 334
    Width = 126
    Height = 17
    Caption = 'Attach as &Ical'
    TabOrder = 9
    OnClick = AttachIcalBoxClick
  end
end
