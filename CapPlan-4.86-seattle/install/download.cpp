//$Id: download.cpp,v 1.14 2013/12/22 03:15:03 darseq Exp $
#include <vcl.h>
#include <registry.hpp>
#pragma hdrstop

// #include <iostream.h>
#include <fstream>
#include <string>
#include "download.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "setup"
#pragma resource "*.dfm"
TSetupDownload *SetupDownload;
//---------------------------------------------------------------------------
__fastcall TSetupDownload::TSetupDownload(TComponent* Owner)
	: TSetupBase(Owner) {
	programs[ODBCDRIVER] = new ODBCdriver(ODBCstatus,ODBCprogress);
	programs[SQLSERVER] = new SQLserver(SQLstatus,SQLprogress);
	getUrls(ExtractFileDir(Application->ExeName)+"\\downloads.txt");

#ifdef MS_SQL
	Memo1->Lines->Add(
		"      NOTE: To install Microsoft SQLEXPRESS server,");
	Memo1->Lines->Add(
		"      Microsoft .NET Framework 2.0 or higher is required.");
#elif defined INNODB
	Memo1->Lines->Add(
		"  NOTE: while configuring MySQL server, enable InnoDB");
	Memo1->Lines->Add("  support");
#endif
}
//---------------------------------------------------------------------------

__fastcall TSetupDownload::~TSetupDownload(void) {
	for(int i=0;i != PROGRAMCOUNT;i++) {delete programs[i];}
}
//---------------------------------------------------------------------------

void __fastcall TSetupDownload::getUrls(AnsiString filename) {
	ifstream fs_read(filename.c_str());
	if(fs_read.fail()) {
		return; // refer to postware.nl if no downloads file available.
		// throw Exception("Error!: could not open file: "+filename);
	}

	string line;
	while(getline(fs_read,line,'\n')) {
		unsigned int i=0; download_info_t d;
		if(line.length() && line[0] == '#') {continue;}
		for(;i<line.length() && line[i]!=';';i++) {d.package += line[i];} i++;
		for(;i<line.length() && line[i]!=';';i++) {d.arch += line[i];} i++;
		for(;i<line.length();i++) {d.url += line[i];}
#ifdef _WIN64
		AnsiString arch("x64");
#else
		AnsiString arch("x86");
#endif
#ifdef MS_SQL
		AnsiString driver("MSSQLdriver");
		AnsiString server("MSSQLserver");
#elif defined INNODB
		AnsiString driver("MYSQLdriver");
		AnsiString server("MYSQLserver");
#endif
		if(d.package == driver && d.arch == arch) {
			program_urls[ODBCDRIVER] = d.url;
		}
		if(d.package == server && d.arch == arch) {
			program_urls[SQLSERVER] = d.url;
		}
	}
	fs_read.clear();
	fs_read.close();
	if(fs_read.fail()) {
		throw Exception("Error!: could not close file: "+filename);
	}
}
//---------------------------------------------------------------------------

void __fastcall TSetupDownload::FormShow(TObject *Sender) {
	for(int i=0;i != PROGRAMCOUNT;i++) {programs[i]->RefreshStatus();}
	TSetupBase::FormShow(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TSetupDownload::NextClick(TObject *Sender) {
	// while the http get method runs, click messages are accepted
	static bool in_progress = false; if(in_progress) {return;}
	try {
		in_progress = true;
		Back->Enabled = false;
		Next->Enabled = false;
		for(int i=0;i != PROGRAMCOUNT;i++) {
			if(programs[i]->Status == DOWNLOAD) {
				programs[i]->Download(program_urls[i]);
			}
		}

		for(int i=0;i != PROGRAMCOUNT;i++) {
			if(programs[i]->Status == AVAILABLE) {
				programs[i]->Install();
			}
		}

		for(int i=0;i != PROGRAMCOUNT;i++) {
			// Here status is either AVAILABLE or INSTALLED.
			// ProgramStatus::Install is called again after
			// the installation round to see if perhaps one
			// of the programs installed the others as well.
			programs[i]->ProgramStatus::Install();
			if(programs[i]->Status != INSTALLED) {
				in_progress = false;
				Back->Enabled = true;
				Next->Enabled = true;
				ActiveControl = Next;
				return;
			}
		}

		// besides being a warning, this message also causes NextClick to be
		// delayed until the user clicks ok. This is necessary because there
		// seems to be a conflict between this installer receiving focus again
		// and the focus being set to the finish form when next is clicked.
#ifdef MS_SQL
#ifdef SA_CONNECTION
		MessageBeep(MB_ICONEXCLAMATION);
		MessageDlg("In the next screen you will be asked for the System "
			"Administrator\npassword after you click install. This is the "
			"password you provided\nfor the SA account while installing "
			"SQLEXPRESS. You may verify\nthat the SA account is properly "
			"enabled by changing the setup of\n\"Microsoft SQL Server 2005\" "
			"from the 'add/remove programs' list.",
			mtWarning,TMsgDlgButtons() << mbOK,0);
#endif
#elif defined INNODB
		MessageDlg("In the next screen you will be asked for the \"root\" "
			"password after you click install.\nThis is the same password you "
			"provided while configuring MySQL. Therefore you\nshould make sure "
			"that you completed the MySQL server instance configuration\n"
			"wizard. To (re)start this wizard navigate to the MySQL entry of "
			"the start menu.",mtWarning,TMsgDlgButtons() << mbOK,0);
#endif
		odbc_driver = ((ODBCdriver *) programs[ODBCDRIVER])->driver;
		SetFocus(); TSetupBase::NextClick(Sender);
	} __finally {
		in_progress = false;
		Back->Enabled = true;
		Next->Enabled = true;
		ActiveControl = Next;
	}
}
//---------------------------------------------------------------------------

