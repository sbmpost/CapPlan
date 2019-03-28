// RECTXIMPL.H : Declaration of the TRectXImpl
// copyright by PostWare (S.B.M. Post)

#ifndef RectXImplH
#define RectXImplH

#define ATL_APARTMENT_THREADED

#include "RowGrid_TLB.H"


/////////////////////////////////////////////////////////////////////////////
// TRectXImpl     Implements IRectX, default interface of RectX
// ThreadingModel : Apartment
// Dual Interface : FALSE
// Event Support  : FALSE
// Default ProgID : RowGrid.RectX
// Description    : Rectangle interface
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TRectXImpl : 
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TRectXImpl, &CLSID_RectX>,
  public IRectX
{
  TxRect Rect;
public:
  TRectXImpl()
  {
  }

  // Data used when registering Object 
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("RowGrid.RectX");
  DECLARE_DESCRIPTION("Rectangle interface");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
    TTypedComServerRegistrarT<TRectXImpl>
    regObj(GetObjectCLSID(), GetProgID(), GetDescription());
    return regObj.UpdateRegistry(bRegister);
  }


BEGIN_COM_MAP(TRectXImpl)
  COM_INTERFACE_ENTRY(IRectX)
END_COM_MAP()

// IRectX
public:
  STDMETHOD(get_Left(long* Value));
  STDMETHOD(set_Left(long Value));
  STDMETHOD(get_Top(long* Value));
  STDMETHOD(set_Top(long Value));
  STDMETHOD(get_Right(long* Value));
  STDMETHOD(set_Right(long Value));
  STDMETHOD(get_Bottom(long* Value));
  STDMETHOD(set_Bottom(long Value));
};

#endif //RectXImplH
