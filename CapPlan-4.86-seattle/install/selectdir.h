//$Id: selectdir.h,v 1.2 2010/12/13 00:20:57 darseq Exp $
#ifndef selectdirH
#define selectdirH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <FileCtrl.hpp>
//---------------------------------------------------------------------------
class TFselect : public TForm
{
__published:	// IDE-managed Components
	TDirectoryListBox *DirectoryListBox1;
	TDriveComboBox *DriveComboBox1;
	void __fastcall DriveComboBox1Change(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TFselect(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFselect *Fselect;
//---------------------------------------------------------------------------
#endif

