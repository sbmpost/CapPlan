//$Id: Fmdetail.h,v 1.50 2013/02/16 12:18:36 darseq Exp $
#ifndef FmdetailH
#define FmdetailH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include "Fbase.h"
#include "RowControl.h"
#include <vector>
#include "DataRows.h"
#include "Tracks.h"
//---------------------------------------------------------------------------

#define GETDOCID(doc_id) (docs.getId(doc_id,per_id,doc_name))
class TFmdetail1 : public TFbase1
{
__published:	// IDE-managed Components
	TRowControl *RowControl;
	TButton *CancelButton;
	TButton *OkButton;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
		  TShiftState Shift);
	void __fastcall FormMouseWheelDown(TObject *Sender, TShiftState Shift,
		  TPoint &MousePos, bool &Handled);
	void __fastcall FormMouseWheelUp(TObject *Sender, TShiftState Shift,
		  TPoint &MousePos, bool &Handled);
	void __fastcall RowControlChanging(TObject *Sender, bool &AllowChange,
		  int NewPage);
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall FormKeyPress(TObject *Sender, char &Key);
private:	// User declarations
	int FPage,print_nr; AnsiString print_date;
	TTabSheet * tabsheet; vector<TDataRows *> datarows;
	__inline TDataRows * __fastcall GetActiveRows(void) {
		return datarows[Page];
	}
	__inline TDataRows * __fastcall GetDataRows(int Index) {
		return datarows[Index];
	}
protected:
	vector<id_t> ids;
	vector<TMainMenu *> menus;
	id_t per_id,doc_name;
	virtual void __fastcall autosave(void);
	virtual void __fastcall rollback(void);
	virtual void __fastcall setDocData(Document & document) = 0;
	void __fastcall openDocument(Docs & docs);
	void __fastcall PrintSelection(void);
	void __fastcall PrintNewPage(TRect &Rect);
	void __fastcall setMenu(TMainMenu * menu,int menu_index,int help_index);
	__inline void __fastcall refreshRows(id_t id) {ActiveRows->refresh(id);}
	__inline void __fastcall refreshNext(void) {ActiveRows->refreshNext();}
	__inline int __fastcall rowSelected(void) {return ActiveRows->rowSelected();}
	__inline bool __fastcall editingRow(void) {
		return ActiveRows->editingRow() || dynamic_cast<TMemo *>(ActiveControl);
	}
	__inline void __fastcall readRow(bool check) {ActiveRows->readRow(check);}
	__inline void __fastcall editRow(void) {ActiveRows->editRow();}
	__inline void __fastcall storeRow(void) {ActiveRows->storeRow();}
	__inline void __fastcall clearRow(int row = 0) {
		TDataRows * rows = ActiveRows;
		if(row == 0) {row = rows->Row;}
		rows->clearRow(row);
	}
	__inline id_t __fastcall getId(int row = 0) {
		TDataRows * rows = ActiveRows;
		if(row == 0) {row = rows->Row;}
		if(row == 0) {return UNDEFINED;}
		return rows->getId(row);
	}
	__inline AnsiString __fastcall getCell(int meta,int row = 0) {
		TDataRows * rows = ActiveRows;
		if(row == 0) {row = rows->Row;}
		return rows->Cells[meta][row];
	}
	__inline void __fastcall setCell(AnsiString cell,int meta,int row = 0) {
		TDataRows * rows = ActiveRows;
		if(row == 0) {row = rows->Row;}
		rows->Cells[meta][row] = cell;
	}
	__inline AnsiString __fastcall getData(int meta,int row = 0) {
		TDataRows * rows = ActiveRows;
		if(row == 0) {row = rows->Row;}
		return rows->getData(meta,row);
	}
	__inline void __fastcall setData(AnsiString data,int meta,int row = 0) {
		TDataRows * rows = ActiveRows;
		if(row == 0) {row = rows->Row;}
		rows->setData(data,meta,row);
	}
	__inline void __fastcall checkChanged(void) {
		TDataRows * rows = ActiveRows;
		// If there was a change that is not visible still throw the exception
		// because buffered data is now old and must be read again. We use the
		// optimistic approach and assume in most cases nothing has changed.
		// The pessimistic approach is to execute readRow(true) before using
		// cell data (with locks being held while dialogs are open).
		if(rows->RowState == rsEmpty || rows->RowState == rsChanged) {
			// if(modified == false && TFoptions1::auto_save) {save();}
			// If modified, do not save: allow for CancelButton to be clicked.
			throw UserException(SELECTION_CHANGED);
		}
	}
public:		// User declarations
	__property int Page = {read = FPage};
	__property TDataRows * ActiveRows = {read = GetActiveRows};
	__property TDataRows * DataRows[int Index] = {read = GetDataRows};
	void __fastcall PageShow(TObject *Sender);
	__fastcall TFmdetail1(TComponent* Owner);
	TModalResult __fastcall view(TTabSheet * show_sheet);
	__inline void __fastcall refreshRow(void) {
		if(ActiveRows->Row && editingRow() == false) {readRow(false);}
	}
};
//---------------------------------------------------------------------------
extern PACKAGE TFmdetail1 *Fmdetail1;
//---------------------------------------------------------------------------
#endif

