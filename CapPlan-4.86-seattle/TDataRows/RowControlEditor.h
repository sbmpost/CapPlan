//$Id: RowControlEditor.h,v 1.2 2013/12/21 20:00:16 darseq Exp $
#ifndef RowControlEditorH
#define RowControlEditorH
//---------------------------------------------------------------------------
#include <DesignIntf.hpp>
#include <DesignEditors.hpp>
//---------------------------------------------------------------------------
#if __BORLANDC__ <= 0x582
typedef AnsiString UnicodeString;
#endif

namespace Rowcontroleditor {

class PACKAGE TRowControlEditor : public TDefaultEditor {
public:
	int __fastcall GetVerbCount(void) {return 4;}
	void __fastcall PrepareItem(int Index, const _di_IMenuItem AItem);
	UnicodeString __fastcall GetVerb(int Index);
	void __fastcall ExecuteVerb(int Index);
};
//---------------------------------------------------------------------------
} // end namespace Rowcontroleditor
#if !defined(NO_IMPLICIT_NAMESPACE_USE)
using namespace Rowcontroleditor;
#endif

#endif

