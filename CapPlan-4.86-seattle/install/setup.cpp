//$Id: setup.cpp,v 1.16 2013/12/21 20:00:16 darseq Exp $
#include <vcl.h>
#include <registry.hpp>
#include <winsvc.h>
#pragma hdrstop

#include "setup.h"
#include "include/decs.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSetupBase *SetupBase;
list<TSetupBase *> TSetupBase::form_list;
//#pragma link "unzip32.a"
//---------------------------------------------------------------------------
__fastcall TSetupBase::TSetupBase(TComponent* Owner) : TForm(Owner) {
	form_list.push_back(this);
	Caption = Application->Title;
}
//---------------------------------------------------------------------------

void __fastcall TSetupBase::NextClick(TObject *Sender) {
	list<TSetupBase *>::iterator it = form_list.begin();
	for(;it != form_list.end();++it) {if(*it == this) {break;}}
	if(it == form_list.end()) {throw Exception("Invalid form");}
	if(++it != form_list.end()) {Hide(); (*it)->Show();}
}
//---------------------------------------------------------------------------

void __fastcall TSetupBase::BackClick(TObject *Sender) {
	list<TSetupBase *>::reverse_iterator rit = form_list.rbegin();
	for(;rit != form_list.rend();++rit) {if(*rit == this) {break;}}
	if(rit == form_list.rend()) {throw Exception("Invalid form");}
	if(++rit != form_list.rend()) {Hide(); (*rit)->Show();}
}
//---------------------------------------------------------------------------

void __fastcall TSetupBase::CancelClick(TObject *Sender) {Close();}
//---------------------------------------------------------------------------

AnsiString __fastcall TSetupBase::normalizePath(AnsiString path) {
	//  foo -> foo, \\foo\\\bar\\ -> \foo\bar
	AnsiString norm; int i=1;
	while(i <= path.Length()) {
		for(;i <= path.Length() && path[i] != '\\';i++) {norm += path[i];}
		for(;i <= path.Length() && path[i] == '\\';i++);
		if(i <= path.Length()) {norm += '\\';}
	}
	return norm;
}
//---------------------------------------------------------------------------

AnsiString __fastcall TSetupBase::getPath(AnsiString environment) {
	AnsiString path = normalizePath(getenv(environment.c_str()));
	if(path.IsEmpty()) {path = ExtractFileDrive(Application->ExeName);}
	return path;
}
//---------------------------------------------------------------------------

bool __fastcall TSetupBase::findKey(HKEY root,AnsiString key,AnsiString & find){
	bool found = false;
	TRegistry * registry = new TRegistry();
	registry->Access = KEY_READ;
	registry->RootKey = root;
	if(registry->OpenKey(key,false)) {
		TStringList * keys = new TStringList();
		registry->GetKeyNames(keys);
		registry->CloseKey();
		for(int i=0;i != keys->Count;i++) {
			if(keys->Strings[i].Length() >= find.Length() &&
				keys->Strings[i].SubString(0,find.Length()) == find) {
				find = keys->Strings[i];
				found = true; // continue search, choose longest match
			}
		}
		delete keys;
	}
	delete registry;
	return found;
}
//---------------------------------------------------------------------------

bool TSetupBase::doService(AnsiString server,AnsiString name,DWORD access_mode){
	SC_HANDLE s_h,sctrl_h;
	SERVICE_STATUS s_stat;

	sctrl_h = OpenSCManager(server.c_str(),NULL,SC_MANAGER_CONNECT);
	if(sctrl_h == NULL) {throw Exception("Could not connect Service Manager");}
	s_h = OpenService(sctrl_h,name.c_str(),SERVICE_QUERY_STATUS | access_mode);
	if(s_h == NULL) {throw Exception("Could not access Service");}

	DWORD wait_state;
	if(access_mode & SERVICE_START) {
		wait_state = SERVICE_RUNNING;
		if(!StartService(s_h,0,NULL)) {return false;}
	} else if(access_mode & SERVICE_STOP) {
		wait_state = SERVICE_STOPPED;
		if(!ControlService(s_h,SERVICE_CONTROL_STOP,&s_stat)) {return false;}
	} else {throw Exception("doService: Invalid access mode");}

	int count = 0;
	if(QueryServiceStatus(s_h,&s_stat) == 0) {return false;}
	while(s_stat.dwCurrentState != wait_state && count < 10) {
		DWORD chkpoint = s_stat.dwCheckPoint;
		DWORD waitTime = s_stat.dwWaitHint/10;
		waitTime = waitTime < 1000 ? 1000 : waitTime;
		waitTime = waitTime > 10000 ? 10000 : waitTime;
		Sleep(waitTime);

		if(QueryServiceStatus(s_h,&s_stat) == 0) {return false;}
		count = s_stat.dwCheckPoint > chkpoint ? 0 : count + 1;
	}
	CloseServiceHandle(s_h); CloseServiceHandle(sctrl_h);
	return s_stat.dwCurrentState == wait_state;
}
//---------------------------------------------------------------------------

int WINAPI print(LPSTR buffer, unsigned long size) {return size;}
void WINAPI beep(void) {return;}
int WINAPI replace(LPSTR filename) {return 1;}
int WINAPI password(LPSTR p, int n, LPCSTR m, LPCSTR name) {return 1;}
void WINAPI dllmessage(unsigned long, unsigned long, unsigned,
	unsigned, unsigned, unsigned, unsigned, unsigned,
	char, LPSTR, LPSTR, unsigned long, char) {
}
int WINAPI progress(LPCSTR filename,unsigned long orig_size) {return 0;}
/*
void __fastcall TSetupBase::unzip(LPSTR src_zip,LPSTR dst_dir) {
	DCL zip_options;
	zip_options.ExtractOnlyNewer = 0;  // do not extract only newer
	zip_options.SpaceToUnderscore = 0; // do not convert spaces to underscores
	zip_options.PromptToOverwrite = 0; // Overwrite all, do not query
	zip_options.fQuiet = 2;	           // 0=all msgs, 1=less msgs, 2=no msgs
	zip_options.ncflag = 0;            // do not write to stdout
	zip_options.ntflag = 0;            // do not test the zip file
	zip_options.nvflag = 0;            // do not give a verbose listing
	zip_options.nfflag = 0;            // do not freshen existing files only
	zip_options.nzflag = 0;            // do not display zip file comment
	zip_options.ndflag = 1;            // safely recreate directories
	zip_options.noflag = 1;            // overwrite all files
	zip_options.naflag = 0;            // do not convert CR to CRLF
	zip_options.nZIflag = 0;           // do not get zip info (1=failure?)
	zip_options.C_flag = 0;            // case insensitive
	zip_options.fPrivilege = 0;        // do not restore ACL?
	zip_options.lpszZipFN = src_zip;   // source archive
	zip_options.lpszExtractDir = dst_dir; // set to NULL for current directory
	USERFUNCTIONS functions = {print,beep,replace,password,dllmessage,progress};
	int result = Wiz_SingleEntryUnzip(0,NULL,0,NULL,&zip_options,&functions);
	if(result) {throw Exception(String("Error!: unzip failure: ")+result);}
}
//---------------------------------------------------------------------------
*/
void __fastcall TSetupBase::FormKeyPress(TObject *Sender, char &Key) {
	if(Key == VK_ESCAPE) {Close();}
}
//---------------------------------------------------------------------------

void __fastcall TSetupBase::FormClose(TObject *Sender,TCloseAction &Action) {
	AnsiString message = "Exit "+Application->Title+" setup?";
	if(MessageDlg(message.c_str(),mtConfirmation,
		TMsgDlgButtons() << mbYes << mbNo,0) == mrYes) {
		exit(0); // an immediate exit is requested
	} else {Action = caNone;}
}
//---------------------------------------------------------------------------

void __fastcall TSetupBase::FormShow(TObject *Sender) {
	if(Next->Enabled) {ActiveControl = Next;}
}
//---------------------------------------------------------------------------

