//$$---- activex control source ---- (stActiveXControlSource)
// copyright by PostWare (S.B.M. Post)
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <atl\atlvcl.h>

#include "RowGridXImpl.h"
//---------------------------------------------------------------------------
#pragma link "RowGrid"
#pragma package(smart_init)
STDMETHODIMP TRowGridXImpl::_set_Font(IFontDisp** Value)
{
  try
  {
    const DISPID dispid = -512;
    if (FireOnRequestEdit(dispid) == S_FALSE)
     return S_FALSE;
    SetVclCtlProp(m_VclCtl->Font, Value);
	FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::DrawTextBiDiModeFlagsReadingOnly(long* Value)
{
  try
  {
  *Value = m_VclCtl->DrawTextBiDiModeFlagsReadingOnly();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_AlignDisabled(VARIANT_BOOL* Value)
{
  try
  {
   *Value = m_VclCtl->AlignDisabled;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_CellBorderWidth(long* Value)
{
  try
  {
   *Value = (long)(m_VclCtl->CellBorderWidth);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_CellColor(::OLE_COLOR* Value)
{
  try
  {
   *Value = (::OLE_COLOR)(m_VclCtl->CellColor);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_CellFrameWidth(long* Value)
{
  try
  {
   *Value = (long)(m_VclCtl->CellFrameWidth);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_CheckPosition(VARIANT_BOOL* Value)
{
  try
  {
   *Value = m_VclCtl->CheckPosition;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_Color(::OLE_COLOR* Value)
{
  try
  {
   *Value = (::OLE_COLOR)(m_VclCtl->Color);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_Ctl3D(VARIANT_BOOL* Value)
{
  try
  {
   *Value = m_VclCtl->Ctl3D;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_DefaultDrawing(VARIANT_BOOL* Value)
{
  try
  {
   *Value = m_VclCtl->DefaultDrawing;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_DoubleBuffered(VARIANT_BOOL* Value)
{
  try
  {
   *Value = m_VclCtl->DoubleBuffered;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_DragCursor(short* Value)
{
  try
  {
   *Value = (short)(m_VclCtl->DragCursor);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_DragMode(TxDragMode* Value)
{
  try
  {
   *Value = (TxDragMode) ((int) m_VclCtl->DragMode);
   // explicitly cast to int: activeX enums are 32 bit
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_DragState(TxDragState* Value)
{
  try
  {
   *Value = (TxDragState) ((int) m_VclCtl->DragState);
   // explicitly cast to int: activeX enums are 32 bit
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_CellState(TxCellState* Value)
{
  try
  {
   *Value = (TxCellState) ((int) m_VclCtl->CellState);
   // explicitly cast to int: activeX enums are 32 bit
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_Enabled(VARIANT_BOOL* Value)
{
  try
  {
   *Value = m_VclCtl->Enabled;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_Font(IFontDisp** Value)
{
  try
  {
    GetVclCtlProp(m_VclCtl->Font, Value);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_FrameWidth(long* Value)
{
  try
  {
   *Value = (long)(m_VclCtl->FrameWidth);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_MinVisible(long* Value)
{
  try
  {
   *Value = (long)(m_VclCtl->MinVisible);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_ParentColor(VARIANT_BOOL* Value)
{
  try
  {
   *Value = m_VclCtl->ParentColor;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_ParentCtl3D(VARIANT_BOOL* Value)
{
  try
  {
   *Value = m_VclCtl->ParentCtl3D;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_RowCount(long* Value)
{
  try
  {
   *Value = (long)(m_VclCtl->RowCount);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_RowHeight(long* Value)
{
  try
  {
   *Value = (long)(m_VclCtl->RowHeight);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_RowLineColor(::OLE_COLOR* Value)
{
  try
  {
   *Value = (::OLE_COLOR)(m_VclCtl->RowLineColor);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_RowLineWidth(long* Value)
{
  try
  {
   *Value = (long)(m_VclCtl->RowLineWidth);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_TickColor(::OLE_COLOR* Value)
{
  try
  {
   *Value = (::OLE_COLOR)(m_VclCtl->TickColor);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_Units(long* Value)
{
  try
  {
   *Value = (long)(m_VclCtl->Units);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_UnitWidth(long* Value)
{
  try
  {
   *Value = (long)(m_VclCtl->UnitWidth);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_Visible(VARIANT_BOOL* Value)
{
  try
  {
   *Value = m_VclCtl->Visible;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::get_VisibleDockClientCount(long* Value)
{
  try
  {
   *Value = (long)(m_VclCtl->VisibleDockClientCount);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::IsRightToLeft(VARIANT_BOOL* Value)
{
  try
  {
  *Value = m_VclCtl->IsRightToLeft();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::set_CellBorderWidth(long Value)
{
  try
  {
    const DISPID dispid = 17;
    if (FireOnRequestEdit(dispid) == S_FALSE)
     return S_FALSE;
    m_VclCtl->CellBorderWidth = (int)(Value);
    FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::set_CellColor(::OLE_COLOR Value)
{
  try
  {
    const DISPID dispid = 23;
    if (FireOnRequestEdit(dispid) == S_FALSE)
     return S_FALSE;
	m_VclCtl->CellColor = (TColor)(Value);
    FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::set_CellFrameWidth(long Value)
{
  try
  {
    const DISPID dispid = 16;
    if (FireOnRequestEdit(dispid) == S_FALSE)
     return S_FALSE;
    m_VclCtl->CellFrameWidth = (int)(Value);
    FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::set_CheckPosition(VARIANT_BOOL Value)
{
  try
  {
    const DISPID dispid = 19;
    if (FireOnRequestEdit(dispid) == S_FALSE)
     return S_FALSE;
    m_VclCtl->CheckPosition = Value;
	FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::set_Color(::OLE_COLOR Value)
{
  try
  {
    const DISPID dispid = -501;
    if (FireOnRequestEdit(dispid) == S_FALSE)
	 return S_FALSE;
    m_VclCtl->Color = (TColor)(Value);
    FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::set_Ctl3D(VARIANT_BOOL Value)
{
  try
  {
    const DISPID dispid = 24;
    if (FireOnRequestEdit(dispid) == S_FALSE)
     return S_FALSE;
    m_VclCtl->Ctl3D = Value;
    FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::set_DefaultDrawing(VARIANT_BOOL Value)
{
  try
  {
	const DISPID dispid = 18;
    if (FireOnRequestEdit(dispid) == S_FALSE)
     return S_FALSE;
    m_VclCtl->DefaultDrawing = Value;
    FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::set_DoubleBuffered(VARIANT_BOOL Value)
{
  try
  {
    const DISPID dispid = 30;
    if (FireOnRequestEdit(dispid) == S_FALSE)
     return S_FALSE;
    m_VclCtl->DoubleBuffered = Value;
    FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::set_DragCursor(short Value)
{
  try
  {
	const DISPID dispid = 26;
    if (FireOnRequestEdit(dispid) == S_FALSE)
	 return S_FALSE;
    m_VclCtl->DragCursor = (TCursor)(Value);
    FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::set_DragMode(TxDragMode Value)
{
  try
  {
    const DISPID dispid = 27;
    if (FireOnRequestEdit(dispid) == S_FALSE)
     return S_FALSE;
    m_VclCtl->DragMode = (TDragMode)(Value);
	FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::set_Enabled(VARIANT_BOOL Value)
{
  try
  {
    const DISPID dispid = -514;
    if (FireOnRequestEdit(dispid) == S_FALSE)
     return S_FALSE;
	m_VclCtl->Enabled = Value;
    FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::set_Font(IFontDisp* Value)
{
  try
  {
    const DISPID dispid = -512;
    if (FireOnRequestEdit(dispid) == S_FALSE)
      return S_FALSE;
    SetVclCtlProp(m_VclCtl->Font, Value);
    FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::set_FrameWidth(long Value)
{
  try
  {
    const DISPID dispid = 15;
    if (FireOnRequestEdit(dispid) == S_FALSE)
     return S_FALSE;
    m_VclCtl->FrameWidth = (int)(Value);
    FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::set_MinVisible(long Value)
{
  try
  {
    const DISPID dispid = 21;
    if (FireOnRequestEdit(dispid) == S_FALSE)
     return S_FALSE;
    m_VclCtl->MinVisible = (int)(Value);
    FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::set_ParentColor(VARIANT_BOOL Value)
{
  try
  {
    const DISPID dispid = 28;
    if (FireOnRequestEdit(dispid) == S_FALSE)
	 return S_FALSE;
    m_VclCtl->ParentColor = Value;
    FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::set_ParentCtl3D(VARIANT_BOOL Value)
{
  try
  {
    const DISPID dispid = 25;
	if (FireOnRequestEdit(dispid) == S_FALSE)
     return S_FALSE;
    m_VclCtl->ParentCtl3D = Value;
    FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::set_RowCount(long Value)
{
  try
  {
    const DISPID dispid = 11;
    if (FireOnRequestEdit(dispid) == S_FALSE)
     return S_FALSE;
    m_VclCtl->RowCount = (int)(Value);
    FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::set_RowHeight(long Value)
{
  try
  {
    const DISPID dispid = 12;
    if (FireOnRequestEdit(dispid) == S_FALSE)
	 return S_FALSE;
    m_VclCtl->RowHeight = (int)(Value);
    FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::set_RowLineColor(::OLE_COLOR Value)
{
  try
  {
    const DISPID dispid = 22;
    if (FireOnRequestEdit(dispid) == S_FALSE)
     return S_FALSE;
    m_VclCtl->RowLineColor = (TColor)(Value);
    FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::set_RowLineWidth(long Value)
{
  try
  {
	const DISPID dispid = 13;
    if (FireOnRequestEdit(dispid) == S_FALSE)
     return S_FALSE;
    m_VclCtl->RowLineWidth = (int)(Value);
	FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::set_TickColor(::OLE_COLOR Value)
{
  try
  {
	const DISPID dispid = 10;
    if (FireOnRequestEdit(dispid) == S_FALSE)
     return S_FALSE;
    m_VclCtl->TickColor = (TColor)(Value);
	FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::set_Units(long Value)
{
  try
  {
    const DISPID dispid = 9;
    if (FireOnRequestEdit(dispid) == S_FALSE)
     return S_FALSE;
    m_VclCtl->Units = (int)(Value);
    FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::set_UnitWidth(long Value)
{
  try
  {
	const DISPID dispid = 14;
    if (FireOnRequestEdit(dispid) == S_FALSE)
     return S_FALSE;
    m_VclCtl->UnitWidth = (int)(Value);
    FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::set_Visible(VARIANT_BOOL Value)
{
  try
  {
    const DISPID dispid = 29;
    if (FireOnRequestEdit(dispid) == S_FALSE)
     return S_FALSE;
    m_VclCtl->Visible = Value;
    FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::SetSubComponent(VARIANT_BOOL IsSubComponent)
{
  try
  {
  m_VclCtl->SetSubComponent(IsSubComponent);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::UseRightToLeftReading(VARIANT_BOOL* Value)
{
  try
  {
  *Value = m_VclCtl->UseRightToLeftReading();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


STDMETHODIMP TRowGridXImpl::UseRightToLeftScrollBar(VARIANT_BOOL* Value)
{
  try
  {
  *Value = m_VclCtl->UseRightToLeftScrollBar();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};


void __fastcall TRowGridXImpl::CanResizeEvent(TObject *Sender,
  int &NewWidth, int &NewHeight, bool &Resize)
{
  long TempNewWidth;
  long TempNewHeight;
  VARIANT_BOOL TempResize;
  TempNewWidth = (long)NewWidth;
  TempNewHeight = (long)NewHeight;
  TempResize = (VARIANT_BOOL)Resize;
  Fire_OnCanResize(&TempNewWidth, &TempNewHeight, &TempResize);
  Resize = (VARIANT_BOOL)TempResize;
}


void __fastcall TRowGridXImpl::ClickEvent(TObject *Sender)
{
  Fire_OnClick();
}


void __fastcall TRowGridXImpl::ConstrainedResizeEvent(TObject *Sender,
  int &MinWidth, int &MinHeight, int &MaxWidth, int &MaxHeight)
{
  long TempMinWidth;
  long TempMinHeight;
  long TempMaxWidth;
  long TempMaxHeight;
  TempMinWidth = (long)MinWidth;
  TempMinHeight = (long)MinHeight;
  TempMaxWidth = (long)MaxWidth;
  TempMaxHeight = (long)MaxHeight;
  Fire_OnConstrainedResize(&TempMinWidth, &TempMinHeight, &TempMaxWidth, &TempMaxHeight);
  MaxHeight = (long)TempMaxHeight;
}


void __fastcall TRowGridXImpl::DblClickEvent(TObject *Sender)
{
  Fire_OnDblClick();
}


void __fastcall TRowGridXImpl::KeyPressEvent(TObject *Sender, char &Key)
{
  short TempKey;
  TempKey = (short)Key;
  Fire_OnKeyPress(&TempKey);
  Key = (short)TempKey;
}


void __fastcall TRowGridXImpl::ResizeEvent(TObject *Sender)
{
  Fire_OnResize();
}


STDMETHODIMP TRowGridXImpl::get_ClientWidth(long* Value)
{
  try
  {
   *Value = (long)(m_VclCtl->ClientWidth);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};
//---------------------------------------------------------------------------

STDMETHODIMP TRowGridXImpl::set_ClientWidth(long Value)
{
  try
  {
    const DISPID dispid = 8;
	if (FireOnRequestEdit(dispid) == S_FALSE)
     return S_FALSE;
	m_VclCtl->ClientWidth = (int)(Value);
    FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};
//---------------------------------------------------------------------------

STDMETHODIMP TRowGridXImpl::get_ClientHeight(long* Value)
{
  try
  {
   *Value = (long)(m_VclCtl->ClientHeight);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};
//---------------------------------------------------------------------------

STDMETHODIMP TRowGridXImpl::set_ClientHeight(long Value)
{
  try
  {
	const DISPID dispid = 33;
    if (FireOnRequestEdit(dispid) == S_FALSE)
	 return S_FALSE;
    m_VclCtl->ClientHeight = (int)(Value);
    FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};
//---------------------------------------------------------------------------

STDMETHODIMP TRowGridXImpl::get_HDC(void** Value)
{
  try
  {
    *Value = m_VclCtl->Canvas->Handle;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};
//---------------------------------------------------------------------------

STDMETHODIMP TRowGridXImpl::get_VerticalRows(VARIANT_BOOL* Value)
{
  try
  {
   *Value = m_VclCtl->VerticalRows;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};
//---------------------------------------------------------------------------

STDMETHODIMP TRowGridXImpl::set_VerticalRows(VARIANT_BOOL Value)
{
  try
  {
    const DISPID dispid = 37;
    if (FireOnRequestEdit(dispid) == S_FALSE)
     return S_FALSE;
    m_VclCtl->VerticalRows = Value;
    FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};
//---------------------------------------------------------------------------

STDMETHODIMP TRowGridXImpl::get_AltKeyInserts(VARIANT_BOOL* Value)
{
  try
  {
   *Value = m_VclCtl->AltKeyInserts;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};
//---------------------------------------------------------------------------

STDMETHODIMP TRowGridXImpl::set_AltKeyInserts(VARIANT_BOOL Value)
{
  try
  {
	const DISPID dispid = 38;
	if (FireOnRequestEdit(dispid) == S_FALSE)
	 return S_FALSE;
	m_VclCtl->AltKeyInserts = Value;
	FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};
//---------------------------------------------------------------------------

STDMETHODIMP TRowGridXImpl::get_ShiftKeyHolds(VARIANT_BOOL* Value)
{
  try
  {
   *Value = m_VclCtl->ShiftKeyHolds;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};
//---------------------------------------------------------------------------

STDMETHODIMP TRowGridXImpl::set_ShiftKeyHolds(VARIANT_BOOL Value)
{
  try
  {
	const DISPID dispid = 41;
	if (FireOnRequestEdit(dispid) == S_FALSE)
	 return S_FALSE;
	m_VclCtl->ShiftKeyHolds = Value;
	FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};
//---------------------------------------------------------------------------

STDMETHODIMP TRowGridXImpl::get_ShiftKeySwaps(VARIANT_BOOL* Value)
{
  try
  {
   *Value = m_VclCtl->ShiftKeySwaps;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};
//---------------------------------------------------------------------------

STDMETHODIMP TRowGridXImpl::set_ShiftKeySwaps(VARIANT_BOOL Value)
{
  try
  {
	const DISPID dispid = 20;
	if (FireOnRequestEdit(dispid) == S_FALSE)
	 return S_FALSE;
	m_VclCtl->ShiftKeySwaps = Value;
	FireOnChanged(dispid);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};
//---------------------------------------------------------------------------

unsigned char __fastcall TRowGridXImpl::Shift2State(TShiftState Shift) {
  unsigned char State = 0;
  if(Shift.Contains(ssShift)) {State |= (1 << 0);}
  if(Shift.Contains(ssAlt)) {State |= (1 << 1);}
  if(Shift.Contains(ssCtrl)) {State |= (1 << 2);}
  if(Shift.Contains(ssLeft)) {State |= (1 << 3);}
  if(Shift.Contains(ssRight)) {State |= (1 << 4);}
  if(Shift.Contains(ssMiddle)) {State |= (1 << 5);}
  if(Shift.Contains(ssDouble)) {State |= (1 << 6);}
  return State;
}
//---------------------------------------------------------------------------

IRectXPtr __fastcall TRowGridXImpl::Rect2Iface(TRect & Rect) {
  IRectXPtr IRectX; // no IRectX * RectX:
  // An IRectXPtr automatically calls addref/release as needed
  if(CoClassCreator::CoCreateInstance(CLSID_RectX,
	IID_IRectX,(LPVOID *) &IRectX) != S_OK) {
	throw Exception("Could not create Rect interface");
  }

  IRectX->set_Left(Rect.Left);
  IRectX->set_Top(Rect.Top);
  IRectX->set_Right(Rect.Right);
  IRectX->set_Bottom(Rect.Bottom);
  // note that the user can also use set_xxx functions.

  return IRectX;
}
//---------------------------------------------------------------------------

IRowCellXPtr __fastcall TRowGridXImpl::RowCell2Iface(RowCell & RowCell) {
  IRowCellXPtr IRowCellX; // no IRowCellX * RowCellX:
  // An IRowCellXPtr automatically calls addref/release as needed
  if(CoClassCreator::CoCreateInstance(CLSID_RowCellX,
	IID_IRowCellX,(LPVOID *) &IRowCellX) != S_OK) {
	throw Exception("Could not create RowCell interface");
  }

  IRowCellX->set_Row(RowCell.Row);
  IRowCellX->set_Start(RowCell.Start);
  IRowCellX->set_Units(RowCell.Units);
  IRowCellX->set_Zorder(RowCell.Zorder);
  IRowCellX->set_ObjectPtr(RowCell.Object);
  // note that the user can also use set_xxx functions.

  return IRowCellX;
}
//---------------------------------------------------------------------------

RowCell __fastcall TRowGridXImpl::Iface2RowCell(IRowCellXPtr IRowCellX) {
  long Row; IRowCellX->get_Row(&Row);
  long Start; IRowCellX->get_Start(&Start);
  long Units; IRowCellX->get_Units(&Units);
  long Zorder; IRowCellX->get_Zorder(&Zorder);
  void * ObjectPtr; IRowCellX->get_ObjectPtr(&ObjectPtr);
  RowCell RowCell(Row,Start,Units,Zorder,ObjectPtr);
  return RowCell;

/*
  long Row; IRowCellX->get_Row(&Row);
  cells_t * cells = &(m_VclCtl->Cells);
  row_t::iterator cell = (*cells)[Row].begin();
  for(;cell != (*cells)[Row].end();cell++) {
	VARIANT_BOOL equal;
	IRowCellX->equals(&equal,RowCell2Iface(*cell));
	if(equal) {return *cell;}
  }
  throw Exception("cell not present");
*/
}
//---------------------------------------------------------------------------

void __fastcall TRowGridXImpl::InsertRowCellEvent(TObject *Sender,
  RowCell &RowCell, bool &CanInsert)
{
  IRowCellXPtr IRowCellX = RowCell2Iface(RowCell);
  VARIANT_BOOL TempCanInsert = (VARIANT_BOOL) CanInsert;
  Fire_OnInsertRowCell(IRowCellX,&TempCanInsert);
  IRowCellX->get_ObjectPtr(&RowCell.Object);
  long Zorder; IRowCellX->get_Zorder(&Zorder);
  RowCell.Zorder = Zorder;
  CanInsert = TempCanInsert;
}
//---------------------------------------------------------------------------

void __fastcall TRowGridXImpl::RemoveRowCellEvent(TObject *Sender,
  RowCell &RowCell, bool &CanRemove)
{
  IRowCellXPtr IRowCellX = RowCell2Iface(RowCell);
  VARIANT_BOOL TempCanRemove = (VARIANT_BOOL) CanRemove;
  Fire_OnRemoveRowCell(IRowCellX,&TempCanRemove);
  IRowCellX->get_ObjectPtr(&RowCell.Object);
  long Zorder; IRowCellX->get_Zorder(&Zorder);
  RowCell.Zorder = Zorder;
  CanRemove = TempCanRemove;
}
//---------------------------------------------------------------------------

void __fastcall TRowGridXImpl::ClearRowCellEvent(TObject *Sender,
  RowCell &RowCell, bool &CanClear)
{
  IRowCellXPtr IRowCellX = RowCell2Iface(RowCell);
  VARIANT_BOOL TempCanClear = (VARIANT_BOOL) CanClear;
  Fire_OnClearRowCell(IRowCellX,&TempCanClear);
  IRowCellX->get_ObjectPtr(&RowCell.Object);
  long Zorder; IRowCellX->get_Zorder(&Zorder);
  RowCell.Zorder = Zorder;
  CanClear = TempCanClear;
}
//---------------------------------------------------------------------------

void __fastcall TRowGridXImpl::DragRowCellEvent(TObject *Sender,
  RowCell &RowCell, TCellState CellState, bool IsCopy, bool &CanDrag)
{
  IRowCellXPtr IRowCellX = RowCell2Iface(RowCell);
  VARIANT_BOOL TempCanDrag = (VARIANT_BOOL) CanDrag;
  TxCellState cellstate = (TxCellState) ((int) CellState);
  // explicitly cast to int: activeX enums are 32 bit
  Fire_OnDragRowCell(IRowCellX,cellstate,IsCopy,&TempCanDrag);
  IRowCellX->get_ObjectPtr(&RowCell.Object);
  long Zorder; IRowCellX->get_Zorder(&Zorder);
  RowCell.Zorder = Zorder;
  CanDrag = TempCanDrag;
}
//---------------------------------------------------------------------------

void __fastcall TRowGridXImpl::DrawFocusRectEvent(TObject *Sender,
  TRect Rect)
{
  Fire_OnDrawFocusRect(Rect2Iface(Rect));
}

void __fastcall TRowGridXImpl::DrawRowLineEvent(TObject *Sender, int Row,
  TRect Rect)
{
  Fire_OnDrawRowLine((long) Row,Rect2Iface(Rect));
}
//---------------------------------------------------------------------------

void __fastcall TRowGridXImpl::DrawRowCellEvent(TObject *Sender,
  RowCell RowCell, TRect Rect)
{
  Fire_OnDrawRowCell(RowCell2Iface(RowCell),Rect2Iface(Rect));
}
//---------------------------------------------------------------------------

void __fastcall TRowGridXImpl::DblClickRowCellEvent(TObject *Sender,
  RowCell RowCell)
{
  Fire_OnDblClickRowCell(RowCell2Iface(RowCell));
}
//---------------------------------------------------------------------------

void __fastcall TRowGridXImpl::PaintEvent(TObject *Sender)
{
  Fire_OnPaint();
}
//---------------------------------------------------------------------------

void __fastcall TRowGridXImpl::MouseDownEvent(TObject *Sender,
  TMouseButton Button, TShiftState Shift, int X, int Y)
{
  TxMouseButton button = (TxMouseButton) ((int) Button);
  // explicitly cast to int: activeX enums are 32 bit
  Fire_OnMouseDown(button,Shift2State(Shift),(long) X,(long) Y);
}
//---------------------------------------------------------------------------

void __fastcall TRowGridXImpl::MouseUpEvent(TObject *Sender,
  TMouseButton Button, TShiftState Shift, int X, int Y)
{
  TxMouseButton button = (TxMouseButton) ((int) Button);
  // explicitly cast to int: activeX enums are 32 bit
  Fire_OnMouseUp(button,Shift2State(Shift),(long) X,(long) Y);
}
//---------------------------------------------------------------------------

void __fastcall TRowGridXImpl::MouseMoveEvent(TObject *Sender,
  TShiftState Shift, int X, int Y)
{
  Fire_OnMouseMove(Shift2State(Shift),(long)X,(long)Y);
}
//---------------------------------------------------------------------------

void __fastcall TRowGridXImpl::KeyDownEvent(TObject *Sender,
  unsigned short &Key, TShiftState Shift)
{
  Fire_OnKeyDown(&Key,Shift2State(Shift));
}
//---------------------------------------------------------------------------

STDMETHODIMP TRowGridXImpl::GetCellArray(LPSAFEARRAY* RowCells,
  long* ArraySize, long Row)
{
  try
  {
	*RowCells = NULL;
	cells_t * cells = &(m_VclCtl->Cells);
	*ArraySize = (*cells)[Row].size();
	if(*ArraySize) {
	  SAFEARRAYBOUND boundaries[1];
	  boundaries[0].lLbound = 0; boundaries[0].cElements = *ArraySize;
	  if((*RowCells = SafeArrayCreate(VT_UNKNOWN,1,boundaries)) == NULL) {
		throw Exception("Could not create RowCell array");
	  }
	}

	row_t::iterator cell = (*cells)[Row].begin();
	for(long i=0;cell != (*cells)[Row].end();cell++,i++) {
	  IRowCellXPtr IRowCellX = RowCell2Iface(*cell);
	  if(SafeArrayPutElement(*RowCells,&i,IRowCellX) != S_OK) {
		throw Exception("Could not insert RowCell element");
	  }
	}
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};
//---------------------------------------------------------------------------

STDMETHODIMP TRowGridXImpl::InsertRowCell(IRowCellX* RowCellX)
{
  try
  {
	RowCell RowCell = Iface2RowCell(IRowCellXPtr(RowCellX,true));
	m_VclCtl->InsertRowCell(RowCell);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};
//---------------------------------------------------------------------------

STDMETHODIMP TRowGridXImpl::RemoveRowCell(IRowCellX* RowCellX)
{
  try
  {
	RowCell RowCell = Iface2RowCell(IRowCellXPtr(RowCellX,true));
	m_VclCtl->RemoveRowCell(RowCell);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};
//---------------------------------------------------------------------------

STDMETHODIMP TRowGridXImpl::MouseToCell(IRowCellX** RowCellX, long X,
  long Y, TxCellState* CellState)
{
  try
  {
	TCellState cellstate;
	RowCell RowCell = m_VclCtl->MouseToCell(X,Y,cellstate);
	*CellState = (TxCellState) ((int) cellstate);
	// explicitly cast to int: activeX enums are 32 bit
	IRowCellXPtr temp = RowCell2Iface(RowCell);
	// must copy through temp so ~TComInterface doesn't release: refcount = 1
	*RowCellX = temp; // assignment actually uses the TComInterface * operator.
	(*RowCellX)->AddRef(); // refcount = 2
	//~temp: refcount = 1
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};
//---------------------------------------------------------------------------

STDMETHODIMP TRowGridXImpl::get_DragCell(IRowCellX** RowCellX)
{
  try
  {
	RowCell RowCell = m_VclCtl->DragCell;
	IRowCellXPtr temp = RowCell2Iface(RowCell);
	*RowCellX = temp; (*RowCellX)->AddRef();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};
//---------------------------------------------------------------------------

STDMETHODIMP TRowGridXImpl::set_DragCell(IRowCellX* RowCellX)
{
  try
  {
	m_VclCtl->DragCell = Iface2RowCell(IRowCellXPtr(RowCellX,true));
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};
//---------------------------------------------------------------------------

STDMETHODIMP TRowGridXImpl::ClearRow(long Row)
{
  try
  {
	m_VclCtl->ClearRow(Row);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
}
//---------------------------------------------------------------------------

STDMETHODIMP TRowGridXImpl::SortRow(long Row)
{
  try
  {
	m_VclCtl->SortRow(Row);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};
//---------------------------------------------------------------------------

STDMETHODIMP TRowGridXImpl::PaintCell(IRowCellX* RowCellX,
  VARIANT_BOOL ClearOnly)
{
  try
  {
	RowCell RowCell = Iface2RowCell(IRowCellXPtr(RowCellX,true));
	m_VclCtl->PaintCell(RowCell,ClearOnly);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};
//---------------------------------------------------------------------------

STDMETHODIMP TRowGridXImpl::CellRect(IRectX** RectX, IRowCellX* RowCellX)
{
  try
  {
	RowCell RowCell = Iface2RowCell(IRowCellXPtr(RowCellX,true));
	TRect Rect = m_VclCtl->CellRect(RowCell);
	IRectXPtr temp = Rect2Iface(Rect);
	*RectX = temp; (*RectX)->AddRef();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};
//---------------------------------------------------------------------------

STDMETHODIMP TRowGridXImpl::RowLineRect(IRectX** RectX, long Row)
{
  try
  {
	TRect Rect = m_VclCtl->RowLineRect(Row);
	IRectXPtr temp = Rect2Iface(Rect);
	*RectX = temp; (*RectX)->AddRef();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};
//---------------------------------------------------------------------------

STDMETHODIMP TRowGridXImpl::ToggleFocus()
{
  try
  {
	m_VclCtl->ToggleFocus();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};
//---------------------------------------------------------------------------

STDMETHODIMP TRowGridXImpl::ClearDragState()
{
  try
  {
	m_VclCtl->ClearDragState();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowGridX);
  }
  return S_OK;
};
//---------------------------------------------------------------------------

