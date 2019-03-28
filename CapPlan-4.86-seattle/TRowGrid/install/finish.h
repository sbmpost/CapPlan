//$Id: finish.h,v 1.5 2005/03/17 19:15:58 darseq Exp $
#ifndef finishH
#define finishH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "setup.h"
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <ComCtrls.hpp>
#include <list.h>
//---------------------------------------------------------------------------
typedef void (* do_entry_t) (AnsiString path,AnsiString tail,
	bool path_is_file,int depth,void * data);

typedef struct entry_info_label {
	AnsiString name; // filename including path
	AnsiString root; // destination root path
	AnsiString base; // destination append path
	bool patch; // whether this file is patched
} entry_info_t;

typedef struct entry_copy_label {
	AnsiString to; // root of all destination files
	int xor_key; // when key is 0, file is not xored
} entry_copy_t;

class TSetupFinish : public TSetupBase
{
__published:	// IDE-managed Components
	TProgressBar *ProgressBar1;
	TLabel *Status; TMemo *SETTINGS;
	void __fastcall NextClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
	TRegistry * registry;
	AnsiString current_key;
	AnsiString bcb6root,installroot;
	AnsiString programlink,desktoplink;
	list<entry_info_t> files;
	void __fastcall recurse(AnsiString path,AnsiString tail,
		do_entry_t doEntry,int depth,bool act_first,void * data);
	void __fastcall install_database(void);
	void __fastcall install_files(void);
	void __fastcall install_links(void);
	void __fastcall install_registry(void);
	void __fastcall install_specific(void);
	void __fastcall uninstall_files(void);
	void __fastcall uninstall_links(void);
	void __fastcall uninstall_registry(void);
	void __fastcall uninstall_specific(void);
	void __fastcall uninstall_database(void);
public:		// User declarations
	void __fastcall getFiles(AnsiString filename,list<entry_info_t> & files);
	__fastcall TSetupFinish(TComponent* Owner);
	__fastcall ~TSetupFinish(void) {delete registry;}
};
//---------------------------------------------------------------------------
extern PACKAGE TSetupFinish *SetupFinish;
//---------------------------------------------------------------------------
#endif
