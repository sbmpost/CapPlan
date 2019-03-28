//$Id: RowControl.h,v 1.3 2008/12/24 03:05:02 darseq Exp $
#ifndef RowControlH
#define RowControlH
//---------------------------------------------------------------------------
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------

namespace Rowcontrol {

typedef void __fastcall (__closure * TTabChangingEventEx)
	(TObject* Sender,bool &AllowChange,int NewPage);
class PACKAGE TRowControl : public TPageControl {
private:
	TTabChangingEventEx FOnChanging;
	DYNAMIC bool __fastcall CanChange(void) {
		bool AllowChange = true;
		if(OnChanging) {
			TPoint point = Mouse->CursorPos;
			TCustomTabControl * tab_control = this;
			point = tab_control->ScreenToClient(point);
			int NewPage = tab_control->IndexOfTabAt(point.x,point.y);
			OnChanging(this,AllowChange,NewPage);
		}
		return AllowChange;
	}
protected:
public:
	__fastcall TRowControl(TComponent* Owner);
__published:

// default properties (used TPageControl as a reference)
// TPageControl:
	__property ActivePage;

// TCustomTabControl:
	__property HotTrack;
	__property Images;
	__property MultiLine;
	__property OwnerDraw;
	__property RaggedRight;
	__property ScrollOpposite;
	__property Style;
	__property TabHeight;
	__property TabIndex;
	__property TabPosition;
	__property TabWidth;

// TWinControl:
	__property DockSite;
	__property TabOrder;
	__property TabStop;

// TControl:
	__property Align;
	__property Anchors;
	__property BiDiMode;
	__property Constraints;
	__property Cursor;
	__property DragCursor;
	__property DragKind;
	__property DragMode;
	__property Enabled;
	__property Font;
	__property Height;
	__property HelpContext;
	__property HelpKeyword;
	__property HelpType;
	__property Hint;
	__property Left;
	__property Name;
	__property ParentBiDiMode;
	__property ParentFont;
	__property ParentShowHint;
	__property PopupMenu;
	__property ShowHint;
	__property Top;
	__property Visible;
	__property Width;

// TComponent:
	__property Tag;

// default events (publish all available events)
// TCustomTabControl:
	__property OnChange;
	__property TTabChangingEventEx OnChanging =
		{read=FOnChanging, write=FOnChanging};
	__property OnDrawTab;
	__property OnGetImageIndex;

// TWinControl:
	__property OnDockDrop;
	__property OnDockOver;
	__property OnEnter;
	__property OnExit;
	__property OnGetSiteInfo;
	__property OnUnDock;

// TControl:
	__property OnContextPopup;
	__property OnDragDrop;
	__property OnDragOver;
	__property OnEndDock;
	__property OnEndDrag;
	__property OnMouseDown;
	__property OnMouseMove;
	__property OnMouseUp;
	__property OnResize;
	__property OnStartDock;
	__property OnStartDrag;

};
//---------------------------------------------------------------------------
} // end namespace Rowcontrol
#if !defined(NO_IMPLICIT_NAMESPACE_USE)
using namespace Rowcontrol;
#endif

#endif

