//$Id: setup.h,v 1.10 2013/12/21 20:00:16 darseq Exp $
#ifndef setupH
#define setupH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <list>
using namespace std;
//---------------------------------------------------------------------------
class TSetupBase : public TForm
{
__published:	// IDE-managed Components
	TShape *Shape1;
	TButton *Back;
	TButton *Next;
	TButton *Cancel;
	TImage *Image1;
	void __fastcall NextClick(TObject *Sender);
	void __fastcall BackClick(TObject *Sender);
	void __fastcall CancelClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormKeyPress(TObject *Sender, char &Key);
private:	// User declarations
	static list<TSetupBase *> form_list;
	static bool doService(AnsiString server,AnsiString name,DWORD access_mode);
public:		// User declarations
	static bool startService(AnsiString server,AnsiString name) {
		return doService(server,name,SERVICE_START);
	}
	static bool stopService(AnsiString server,AnsiString name) {
		return doService(server,name,SERVICE_STOP);
	}
	static AnsiString __fastcall normalizePath(AnsiString path);
	static AnsiString __fastcall getPath(AnsiString environment);
	static bool __fastcall findKey(HKEY root,AnsiString key,AnsiString & find);
	static void __fastcall unzip(LPSTR src_zip,LPSTR dst_dir);
	__fastcall TSetupBase(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSetupBase *SetupBase;
//---------------------------------------------------------------------------
#endif

