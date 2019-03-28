// ROWCELLXIMPL.H : Declaration of the TRowCellXImpl
// copyright by PostWare (S.B.M. Post)

#ifndef RowCellXImplH
#define RowCellXImplH

#define ATL_APARTMENT_THREADED

#include "RowGrid_TLB.H"


/////////////////////////////////////////////////////////////////////////////
// TRowCellXImpl     Implements IRowCellX, default interface of RowCellX
// ThreadingModel : Apartment
// Dual Interface : FALSE
// Event Support  : FALSE
// Default ProgID : RowGrid.RowCellX
// Description    : RowCells are part of a RowGrid
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TRowCellXImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TRowCellXImpl, &CLSID_RowCellX>,
  public IRowCellX
{
  TxRowCell RowCell;
public:
  TRowCellXImpl()
  {
  }

  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("RowGrid.RowCellX");
  DECLARE_DESCRIPTION("RowCells are part of a RowGrid");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
	TTypedComServerRegistrarT<TRowCellXImpl>
	regObj(GetObjectCLSID(), GetProgID(), GetDescription());
	return regObj.UpdateRegistry(bRegister);
  }


BEGIN_COM_MAP(TRowCellXImpl)
  COM_INTERFACE_ENTRY(IRowCellX)
END_COM_MAP()

// IRowCellX
public:
  STDMETHOD(get_Row(long* Value));
  STDMETHOD(set_Row(long Value));
  STDMETHOD(get_Start(long* Value));
  STDMETHOD(set_Start(long Value));
  STDMETHOD(get_Units(long* Value));
  STDMETHOD(set_Units(long Value));
  STDMETHOD(get_Zorder(long* Value));
  STDMETHOD(set_Zorder(long Value));
  STDMETHOD(get_ObjectPtr(void** Value));
  STDMETHOD(set_ObjectPtr(void* Value));
  STDMETHOD(equals(VARIANT_BOOL* IsEqual, IRowCellX* RowCellX));
};

#endif //RowCellXImplH
