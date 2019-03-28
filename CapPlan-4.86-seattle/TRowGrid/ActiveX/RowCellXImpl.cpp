// ROWCELLXIMPL : Implementation of TRowCellXImpl (CoClass: RowCellX, Interface: IRowCellX)
// copyright by PostWare (S.B.M. Post)

#include <vcl.h>
#pragma hdrstop

#include "ROWCELLXIMPL.H"

/////////////////////////////////////////////////////////////////////////////
// TRowCellXImpl


STDMETHODIMP TRowCellXImpl::get_Row(long* Value)
{
  try
  {
	*Value = RowCell.Row;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowCellX);
  }
  return S_OK;
};


STDMETHODIMP TRowCellXImpl::set_Row(long Value)
{
  try
  {
	RowCell.Row = Value;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowCellX);
  }
  return S_OK;
};


STDMETHODIMP TRowCellXImpl::get_Start(long* Value)
{
  try
  {
	*Value = RowCell.Start;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowCellX);
  }
  return S_OK;
};


STDMETHODIMP TRowCellXImpl::set_Start(long Value)
{
  try
  {
	RowCell.Start = Value;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowCellX);
  }
  return S_OK;
};


STDMETHODIMP TRowCellXImpl::get_Units(long* Value)
{
  try
  {
	*Value = RowCell.Units;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowCellX);
  }
  return S_OK;
};


STDMETHODIMP TRowCellXImpl::set_Units(long Value)
{
  try
  {
	RowCell.Units = Value;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowCellX);
  }
  return S_OK;
};


STDMETHODIMP TRowCellXImpl::get_Zorder(long* Value)
{
  try
  {
	*Value = RowCell.Zorder;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowCellX);
  }
  return S_OK;
};


STDMETHODIMP TRowCellXImpl::set_Zorder(long Value)
{
  try
  {
	RowCell.Zorder = Value;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowCellX);
  }
  return S_OK;
};


STDMETHODIMP TRowCellXImpl::get_ObjectPtr(void** Value)
{
  try
  {
	*Value = RowCell.ObjectPtr;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowCellX);
  }
  return S_OK;
};


STDMETHODIMP TRowCellXImpl::set_ObjectPtr(void* Value)
{
  try
  {
	RowCell.ObjectPtr = Value;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowCellX);
  }
  return S_OK;
};


STDMETHODIMP TRowCellXImpl::equals(VARIANT_BOOL* IsEqual,
  IRowCellX* RowCellX)
{
  try
  {
	long Row,Start,Units,Zorder; void * ObjectPtr;
	RowCellX->get_Row(&Row);
	RowCellX->get_Start(&Start);
	RowCellX->get_Units(&Units);
	RowCellX->get_Zorder(&Zorder);
	RowCellX->get_ObjectPtr(&ObjectPtr);
	*IsEqual = (Row == RowCell.Row && Start == RowCell.Start &&
			Units == RowCell.Units && Zorder == RowCell.Zorder &&
			ObjectPtr == RowCell.ObjectPtr);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRowCellX);
  }
  return S_OK;
};

