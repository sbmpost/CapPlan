//$Id: download.h,v 1.13 2013/12/22 03:15:03 darseq Exp $
#ifndef downloadH
#define downloadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <ComCtrls.hpp>
#include <process.h>
#include <math.hpp>
#include "setup.h"
#include "config.h"

#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdHTTP.hpp>
#include <IdSSLOpenSSL.hpp>
#pragma alignment
// show alignment problems (enable general messages)
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#if __BORLANDC__ <= 0x582
typedef const int length_t;
typedef AnsiString UnicodeString;
#else
typedef __int64 length_t;
#endif

//---------------------------------------------------------------------------
typedef enum {ODBCDRIVER=0,SQLSERVER,PROGRAMCOUNT} program_t;
typedef enum {DOWNLOAD,AVAILABLE,INSTALLED} status_t;
typedef struct download_info_label {
	AnsiString package;
	AnsiString arch;
	AnsiString url;
} download_info_t;

class Program { // define Program interface
private:
	status_t FStatus; TIdHTTP * Http;
	TIdSSLIOHandlerSocketOpenSSL * Ssl;
	void __fastcall HttpSuccess(TObject * Sender,
		TWorkMode AWorkMode) {DownloadSuccess();}
	void __fastcall HttpPacketRecvd(TObject * Sender,
		TWorkMode AWorkMode,length_t AWorkCount) {
		DownloadReceive(Sender,AWorkCount);
	}
	void __fastcall HttpRedirect(TObject * Sender,UnicodeString & dest,
		int & NumRedirect,bool & Handled,TIdHTTPMethod & VMethod) {
		DownloadRedirect(Handled);
	}
	void __fastcall HttpAboutToSend(TObject * Sender,
		TIdHeaderList * AHeaders,bool & VContinue) {
/*
		disabled: probably Indy does not require this
		for(int i=0;i != AHeaders->Count;i++) {
			AnsiString value = AHeaders->Strings[0];
			AHeaders->Delete(0); AnsiString clean;
			for(int j=1;j <= value.Length();j++) {
				if(value[j] != '\r' && value[j] != '\n') {
					clean += value[j];
				}
			}
			AHeaders->Add(clean);
		}
*/
	}
protected:
	virtual status_t __fastcall GetStatus(void) {return FStatus;}
	virtual void __fastcall SetStatus(status_t status) {FStatus = status;}
	virtual void __fastcall DownloadSuccess(void) = 0;
	virtual void __fastcall DownloadFailure(void) = 0;
	virtual void __fastcall DownloadReceive(TObject * Sender,length_t pos) {}
	virtual void __fastcall DownloadRedirect(bool & Handled) {}
	void __fastcall Download(AnsiString from,AnsiString to) {
		TFileStream * stream = new TFileStream(to,fmCreate);
		try {
			try {Http->Get(from,stream); DownloadSuccess();}
			catch(Exception & e) {DownloadFailure(); throw;}
		} __finally {delete stream;}
	}
public:
	__property status_t Status = {read = GetStatus};
	virtual void __fastcall Download(AnsiString from) = 0;
	virtual void __fastcall Install(void) = 0;
	__fastcall Program() {
		SetStatus(DOWNLOAD);
		Http = new TIdHTTP(NULL);
		Ssl = new TIdSSLIOHandlerSocketOpenSSL(NULL);
		Ssl->SSLOptions->SSLVersions = TIdSSLVersions();
		Ssl->SSLOptions->SSLVersions << TIdSSLVersion::sslvTLSv1_2;
		Ssl->SSLOptions->Mode = sslmUnassigned;
		Http->IOHandler = Ssl;
		Http->HandleRedirects = true;
		Http->OnHeadersAvailable = HttpAboutToSend;
		Http->OnWork = HttpPacketRecvd;
		Http->OnRedirect = HttpRedirect;
	}
	__fastcall ~Program(void) {delete Http; delete Ssl;}
};

class ProgramStatus : public Program {
private:
	TLabel * Label; TProgressBar * Progress;
	AnsiString FText; AnsiString Caption;
	void __fastcall SetText(AnsiString text) {
		FText = text; Label->Caption = Caption+text;
	}
protected:
	__property AnsiString Text = {read = FText, write = SetText};
	virtual bool __fastcall Installed(void) = 0;
	virtual void __fastcall DownloadSuccess(void) {SetStatus(AVAILABLE);}
	virtual void __fastcall DownloadFailure(void) {
		Text = "Download failure!, retry by clicking next";
		Progress->Position = 0;
	}
	virtual void __fastcall DownloadReceive(TObject * Sender,length_t pos) {
		TIdHTTP * Http = (TIdHTTP *) Sender;
		Text = "Receiving data from "+Http->Request->Host;
		length_t tot = Http->Response->ContentLength;
		if(Progress->Max != tot/1024) {Progress->Max = tot/1024;}
		if(Progress->Position != pos/1024) {
			Progress->Position = pos/1024;
			if(tot) {
				Text = Text+", "+IntToStr((length_t)
					RoundTo((100.0 * pos)/tot,0))+"%";
				MSG m;
				while(PeekMessage(&m,NULL,0,0,PM_REMOVE |
					PM_QS_INPUT | PM_QS_PAINT)) {
					DispatchMessage(&m);
				}
			}
		}
	}
	virtual void __fastcall DownloadRedirect(bool & Handled) {
		Text = "Waiting for data";
	}
public:
	void __fastcall RefreshStatus(void) {
		// cannot check for Installed() in the constructor as the
		// virtual versions are not yet available at that point.
		// when Installed() is false while Status == INSTALLED,
		// Status cannot be reset because it is not known whether
		// host != localhost or the user uninstalled the Program.
		if(Installed()) {SetStatus(INSTALLED);}
		else {SetStatus(Status);} // detect host change
	}
	virtual void __fastcall Install(void) {
		if(Installed()) {SetStatus(INSTALLED);}
		else {Text = "Installation incomplete!, retry by clicking next";}
	}
	virtual __fastcall ~ProgramStatus(void) {}
	ProgramStatus(TLabel * label,TProgressBar * progress) : Program() {
		Label = label; Progress = progress; Caption = Label->Caption;
	}
};

class ODBCdriver : public ProgramStatus {
private:
	AnsiString tempdir,to;
	virtual bool __fastcall Installed(void) {
#ifdef MS_SQL
		driver = "SQL Server Native Client";
#elif defined INNODB
		driver = "MySQL";
#endif
		AnsiString key = "\\SOFTWARE\\ODBC\\ODBCINST.INI";
		return TSetupBase::findKey(HKEY_LOCAL_MACHINE,key,driver);
	}
	virtual void __fastcall DownloadFailure(void) {
		MessageBeep(MB_ICONEXCLAMATION);
#ifdef MS_SQL
		MessageDlg("Unable to download ODBC/SQLNCLI! Do you have a "
			"working internet connection?\nYou may try again by clicking "
			"next. You may also download and install the SQLNCLI\n"
			"driver yourself and run this installer afterwards.",
			mtWarning,TMsgDlgButtons() << mbOK,0);
#elif defined INNODB
		MessageDlg("Unable to download ODBC/connector! Do you have a "
			"working internet connection?\nYou may try again by clicking "
			"next. You may also download and install the MyODBC\n"
			"driver yourself and run this installer afterwards.",
			mtWarning,TMsgDlgButtons() << mbOK,0);
#endif
		ProgramStatus::DownloadFailure();
	}
	virtual void __fastcall SetStatus(status_t status) {
		if(status == DOWNLOAD) {
#ifdef MS_SQL
			Text = "None detected, SQLNCLI will be downloaded";
#elif defined INNODB
			Text = "None detected, MyODBC will be downloaded";
#endif
		} else if(status == INSTALLED) {
			Text = "Detected: "+driver;
		} else if(status == AVAILABLE) {Text = "Download complete";}
		Program::SetStatus(status);
	}
public:
	AnsiString driver;
	virtual void __fastcall Download(AnsiString from) {
// SetStatus(AVAILABLE);
		Text = "Downloading...";
		if(from.IsEmpty()) {
#ifdef MS_SQL
			from = "http://www.postware.nl/cgi-bin/MSSQLdriver";
#elif defined INNODB
			from = "http://www.postware.nl/cgi-bin/MYSQLdriver";
#endif
		}
		if(DirectoryExists(tempdir) == false && CreateDir(tempdir) == false) {
			throw Exception("Error!: unable to create directory: "+tempdir);
		} else {Program::Download(from,tempdir+"\\"+to);}
	}
	virtual void __fastcall Install(void) {
		Text = "Installing: ...\\"+to;
		AnsiString msi = "\""+tempdir+"\\"+to+"\"";
		AnsiString p1 = TSetupBase::getPath("windir")+"\\system32\\msiexec";
		AnsiString p2 = TSetupBase::getPath("windir")+"\\system\\msiexec";
		if(spawnlp(P_WAIT,p1.c_str(),p1.c_str(),"/i",msi.c_str(),NULL) == -1 &&
			spawnlp(P_WAIT,p2.c_str(),p2.c_str(),"/i",msi.c_str(),NULL) == -1) {
			throw Exception("Error!: unable to invoke \""+to+"\". "
				"Note that msi packages require the \"microsoft installer\". "
				"Make sure it is on your system and retry by clicking next");
		}
		ProgramStatus::Install();
	}
	virtual __fastcall ~ODBCdriver(void) {}
	ODBCdriver(TLabel * label,TProgressBar * progress) :
		ProgramStatus(label,progress) {
		tempdir = TSetupBase::getPath("TEMP")+"\\"+Application->Title;
#ifdef MS_SQL
		to = "sqlncli.msi";
#elif defined INNODB
		to = "MyODBC.msi";
#endif
	}
};

class SQLserver : public ProgramStatus {
private:
	AnsiString server,tempdir,to;
	virtual bool __fastcall Installed(void) {
#ifdef MS_SQL
		server = "SQL Server";
		AnsiString key =
			"\\SOFTWARE\\Microsoft\\Microsoft SQL Server\\Services";
#elif defined INNODB
		server = "MySQL Server";
#ifdef _WIN64
		AnsiString key = "\\SOFTWARE\\Wow6432Node\\MYSQL AB";
#else
		AnsiString key = "\\SOFTWARE\\MySQL AB";
#endif
#endif
		return TSetupBase::findKey(HKEY_LOCAL_MACHINE,key,server);
	}
	virtual void __fastcall DownloadFailure(void) {
		MessageBeep(MB_ICONEXCLAMATION);
#ifdef MS_SQL
		MessageDlg("Unable to download SQL Server 2005! Do you have a "
			"working internet connection?\nYou may try again by clicking "
			"next. You may also download and install SQLEXPRESS yourself\nand "
			"run this installer afterwards.",
			mtWarning,TMsgDlgButtons() << mbOK,0);
#elif defined INNODB
		MessageDlg("Unable to download MySQL 4.1 or higher! Do you have a "
			"working internet connection?\nYou may try again by clicking "
			"next. You may also download and install MySQL yourself\nand "
			"run this installer afterwards.",
			mtWarning,TMsgDlgButtons() << mbOK,0);
#endif
		ProgramStatus::DownloadFailure();
	}
	virtual status_t __fastcall GetStatus(void) {
		if(SetupConfig->DATABASEHOST->Text != "localhost") {return INSTALLED;}
		return Program::GetStatus();
	}
	virtual void __fastcall SetStatus(status_t status) {
		AnsiString host = SetupConfig->DATABASEHOST->Text;
		if(host != "localhost") {Text = "Using host: "+host; return;}
		if(status == DOWNLOAD) {
#ifdef MS_SQL
			Text = "None detected, SQLEXPRESS will be downloaded";
#elif defined INNODB
			Text = "None detected, MySQL will be downloaded";
#endif
		} else if(status == INSTALLED) {
			Text = "Detected: "+server;
		} else if(status == AVAILABLE) {Text = "Download complete";}
		Program::SetStatus(status);
	}
public:
	virtual void __fastcall Download(AnsiString from) {
// SetStatus(AVAILABLE);
		Text = "Downloading...";
		if(from.IsEmpty()) {
#ifdef MS_SQL
			from = "http://www.postware.nl/cgi-bin/MSSQLserver";
#elif defined INNODB
			from = "http://www.postware.nl/cgi-bin/MYSQLserver";
#endif
		}
		if(DirectoryExists(tempdir) == false && CreateDir(tempdir) == false) {
			throw Exception("Error!: unable to create directory: "+tempdir);
		} else {Program::Download(from,tempdir+"\\"+to);}
	}
	virtual void __fastcall Install(void) {
// Text = "Unzipping: ...\\SQLserver.zip";
// TSetupBase::unzip((tempdir+"\\SQLserver.zip").c_str(),tempdir.c_str());
		Text = "Installing: ...\\"+to;
#ifdef MS_SQL
		AnsiString exe = tempdir+"\\"+to;
		if(spawnlp(P_WAIT,exe.c_str(),exe.c_str(),NULL) == -1) {
			throw Exception("Error!: unable to invoke \""+to+"\". "
				"Retry by clicking next");
		}
#elif defined INNODB
		AnsiString msi = "\""+tempdir+"\\"+to+"\"";
		AnsiString p1 = TSetupBase::getPath("windir")+"\\system32\\msiexec";
		AnsiString p2 = TSetupBase::getPath("windir")+"\\system\\msiexec";
		if(spawnlp(P_WAIT,p1.c_str(),p1.c_str(),"/i",msi.c_str(),NULL) == -1 &&
			spawnlp(P_WAIT,p2.c_str(),p2.c_str(),"/i",msi.c_str(),NULL) == -1) {
			throw Exception("Error!: unable to invoke \""+to+"\". "
				"Note that msi packages require the \"microsoft installer\". "
				"Make sure it is on your system and retry by clicking next");
		}
#endif
		ProgramStatus::Install();
	}
	virtual __fastcall ~SQLserver(void) {}
	SQLserver(TLabel * label,TProgressBar * progress) :
		ProgramStatus(label,progress) {
		tempdir = TSetupBase::getPath("TEMP")+"\\"+Application->Title;
#ifdef MS_SQL
		to = "SQLserver.exe";
#elif defined INNODB
		to = "SQLserver.msi";
#endif
	}
};

class TSetupDownload : public TSetupBase
{
__published:	// IDE-managed Components
	TMemo *Memo1;
	TLabel *ODBCstatus;
	TLabel *SQLstatus;
	TProgressBar *ODBCprogress;
	TProgressBar *SQLprogress;
	void __fastcall NextClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
	ProgramStatus * programs[PROGRAMCOUNT];
	AnsiString program_urls[PROGRAMCOUNT];
	void __fastcall getUrls(AnsiString filename);
public:		// User declarations
	AnsiString odbc_driver;
	__fastcall TSetupDownload(TComponent* Owner);
	__fastcall ~TSetupDownload(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TSetupDownload *SetupDownload;
//---------------------------------------------------------------------------
#endif

