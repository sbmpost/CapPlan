inherited SetupLicense: TSetupLicense
  Left = 272
  Top = 205
  OldCreateOrder = True
  PixelsPerInch = 96
  TextHeight = 13
  object LICENSE: TMemo
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
    Font.Height = -13
    Font.Name = 'Courier New'
    Font.Style = []
    Lines.Strings = (
      'License Agreement for RowGrid-1.0.2.64-freeware '
      '("Software")'
      'Copyright (c) 2004-2012 by PostWare (S.B.M. Post)'
      'All Rights Reserved.'
      ''
      '1. USE.'
      'You may use this Software in any way provided that '
      'the following conditions are met:'
      ''
      '- When distributing this Software and/or any Works '
      'derived from it, you must provide an exact copy of '
      'this document (license.txt).'
      ''
      '- You are not allowed to add, remove or change any '
      'copyright notices in this Software. This includes '
      'all source files.'
      ''
      '- If you change the source code, you must clearly '
      'document that you have done so. It should be clear '
      'to your users that they are using a modified '
      'version of this Software.'
      ''
      '2. LIMITED WARRANTY.'
      'THE SOFTWARE IS PROVIDED AS IS AND PostWare '
      'DISCLAIMS ALL WARRANTIES RELATING TO THIS SOFTWARE, '
      'WHETHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT '
      'LIMITED TO ANY IMPLIED WARRANTIES OF '
      'MERCHANTABILITY AND FITNESS FOR A PARTICULAR '
      'PURPOSE.'
      ''
      '3. LIMITATION ON CONSEQUENTIAL DAMAGES. NEITHER '
      'PostWare NOR ANYONE INVOLVED IN THE CREATION, '
      'PRODUCTION, OR DELIVERY OF THIS SOFTWARE SHALL BE '
      'LIABLE FOR ANY INDIRECT, CONSEQUENTIAL, OR '
      'INCIDENTAL DAMAGES ARISING OUT OF THE USE OR '
      'INABILITY TO USE SUCH SOFTWARE EVEN IF PostWare HAS '
      'BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES OR '
      'CLAIMS. THE PERSON USING THE SOFTWARE BEARS ALL '
      'RISK AS TO THE QUALITY AND PERFORMANCE OF THE '
      'SOFTWARE.'
      '')
    ParentFont = False
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 3
  end
  object CheckBox1: TCheckBox
    Left = 8
    Top = 324
    Width = 241
    Height = 17
    Caption = '&Accept the terms of the agreement'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 4
    OnClick = CheckBox1Click
  end
end
