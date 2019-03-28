// RECTXIMPL : Implementation of TRectXImpl (CoClass: RectX, Interface: IRectX)
// copyright by PostWare (S.B.M. Post)

#include <vcl.h>
#pragma hdrstop

#include "RECTXIMPL.H"

/////////////////////////////////////////////////////////////////////////////
// TRectXImpl

STDMETHODIMP TRectXImpl::get_Left(long* Value)
{
  try
  {
	*Value = Rect.Left;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRectX);
  }
  return S_OK;
};


STDMETHODIMP TRectXImpl::set_Left(long Value)
{
  try
  {
	Rect.Left = Value;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRectX);
  }
  return S_OK;
};


STDMETHODIMP TRectXImpl::get_Top(long* Value)
{
  try
  {
	*Value = Rect.Top;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRectX);
  }
  return S_OK;
};


STDMETHODIMP TRectXImpl::set_Top(long Value)
{
  try
  {
	Rect.Top = Value;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRectX);
  }
  return S_OK;
};


STDMETHODIMP TRectXImpl::get_Right(long* Value)
{
  try
  {
	*Value = Rect.Right;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IRectX);
  }
  return S_OK;
};


STDMETHODIMP TRectXImpl::set_Right(long Value)
{
  try
  {
	Rect.Right = Value;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRectX);
  }
  return S_OK;
};


STDMETHODIMP TRectXImpl::get_Bottom(long* Value)
{
  try
  {
	*Value = Rect.Bottom;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRectX);
  }
  return S_OK;
};


STDMETHODIMP TRectXImpl::set_Bottom(long Value)
{
  try
  {
	Rect.Bottom = Value;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRectX);
  }
  return S_OK;
};

