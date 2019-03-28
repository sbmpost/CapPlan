//$Id: Exceptions.cpp,v 1.102 2013/03/22 17:58:41 darseq Exp $
#include <vcl.h>
#pragma hdrstop
#include "Exceptions.h"
#include "defines.h"
#include "Query.h"
#include "Fbase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
TFileStream * Exceptions::logfile = NULL;
TRegistry * Exceptions::registry = NULL;
bool Exceptions::clearlog = false;
bool Exceptions::debug_mode = false;
char * Exceptions::log_path = NULL;
AnsiString Exceptions::version;

void __fastcall Exceptions::initLog(AnsiString app_version) {
	version = app_version; log_path = new char[MAX_PATH];
	strncpy(log_path,(TFbase1::getPath("APPDATA")+
		"\\" PRODUCT+".log").c_str(),MAX_PATH);
#ifndef _DEBUG
	if(FileExists(log_path) == false) {
#endif
	// If _DEBUG, always clear the logfile
	try {logfile = new TFileStream(log_path,
		fmCreate | fmShareDenyNone);}
	catch(Exception & e) {logfile = NULL;}
#ifndef _DEBUG
	}
#endif
	if(openLog()) {
		AnsiString filename = copyLog();
		if(ATTYN(AnsiString::LoadStr(
			EX_SENDBUGREPORTQUESTION).c_str()) == ID_YES) {
			TFbase1::sendMail(POSTWAREMAIL,
				"CapPlan terminated unexpectedly",
				"CapPlan bug report",filename);
		}
	}
}
//---------------------------------------------------------------------------

int __fastcall Exceptions::openLog(void) {
	int bytes = 0; closeLog();
	// note: Only one instance of CapPlan is
	// allowed to open and write the logfile.
	try {logfile = new TFileStream(log_path,
		fmOpenReadWrite | fmShareDenyWrite);}
	catch(Exception & e) {logfile = NULL;}
	if(logfile) {bytes = logfile->Seek(0L,soEnd);} // use soEnd
	else {ATTO(AnsiString::LoadStr(EX_CANNOTAPPENDCREATELOG).c_str());}
	return bytes;
}
//---------------------------------------------------------------------------

AnsiString __fastcall Exceptions::copyLog(void) {
	AnsiString filename = TFbase1::getPath("APPDATA");
	filename = filename+"\\"+String(tmpnam(NULL));
	if(logfile != NULL) {
		try {
			writeLog("writing temporary error file: "+filename);
			TFileStream * tempfile = new TFileStream(filename,
				fmCreate | fmShareDenyWrite);
			try {tempfile->CopyFrom(logfile,0); writeLog("done");}
			__finally {delete tempfile;}
		} catch(Exception & e) {
			writeLog("could not write error file: "+e.Message);
		}
	}
	return filename;
}
//---------------------------------------------------------------------------

void __fastcall Exceptions::writeLog(AnsiString string) {
	if(logfile != NULL) {
		// Outlook/Outlook express may strip the carriage
		// returns when mailing the log. This depends on
		// the Tools | Options | Send | HTML Settings.
		// Base64 encoding is required to preserve them.
		// Unfortunately with Simple MAPI it cannot be set.
		AnsiString message = "|"+DateToStr(Date())+"|";
		message += TimeToStr(Time())+"| "+string+"\r\n";
		logfile->Write(message.c_str(),message.Length());
	}
}
//---------------------------------------------------------------------------

void __fastcall Exceptions::closeLog(void) {
	delete logfile; logfile = NULL;
}
//---------------------------------------------------------------------------

void __fastcall Exceptions::cleanup(void) {
	LOGFUNC();
	closeLog();
#ifndef _DEBUG
	if(clearlog) {
		try {logfile = new TFileStream(log_path,
			fmCreate | fmShareDenyNone);}
		catch(Exception & e) {logfile = NULL;}
		closeLog();
	}
#endif
	delete[] log_path; log_path = NULL;
	delete registry; registry = NULL;
}
//---------------------------------------------------------------------------

TRegistry * __fastcall Exceptions::getRegistry(void) {
	if(registry == NULL) {registry = new TRegistry();}
	registry->RootKey = HKEY_CURRENT_USER;
	registry->Access = KEY_READ;
	return registry;
}
//---------------------------------------------------------------------------

void __fastcall Exceptions::handleException(TObject *Sender, Exception * e) {
	LOGFUNC();
	AnsiString message(e->Message);
	AnsiString class_name = e->ClassName();
	DBException * db_ex;
	//-----try to catch all known exceptions we know a user might produce----
	//-----potential fatal exceptions should be caught at a lower level  ----
	//-----if possible. UserExceptions can become fatal if generated in  ----
	//-----the middle of a database changing proces (unwanted save might ----
	//-----occur afterwards)
	if(Exceptions::debug_mode) {LOGEXCEPTION();}
	if(dynamic_cast<UserException *>(e)) {
		MSGO(message.c_str());
		return;
	} else if(dynamic_cast<EOleSysError *>(e)) {
		ATTO(("OLE error: "+message+" ("+IntToHex((int)
		((EOleSysError *) e)->ErrorCode,8)+")").c_str());
		return;
	} else {
#if defined MS_SQL || defined INNODB
		db_ex = dynamic_cast<DBException *>(e);
		if(db_ex && db_ex->errors.size()) {
			if(db_ex->errors[0].native_err == DEADLOCK ||
				db_ex->errors[0].native_err == LOCK_TIMEOUT) {
				ATTO(AnsiString::LoadStr(EX_TRANSACTIONFAILURE).c_str());
				return;
			} else if(String((char *) db_ex->errors[0].sql_state) ==
				INTEGRITY_VIOLATION) { // for example DUPLICATEKEY
				ATTO(AnsiString::LoadStr(EX_INTEGRITYVIOLATION).c_str());
				return;
			} else if(db_ex->errors[0].native_err == SERVER_GONE ||
				db_ex->errors[0].native_err == SERVER_LOST ||
				db_ex->errors[0].native_err == SERVER_DOWN ||
				String((char *)db_ex->errors[0].sql_state) == ODBC_GONE_LOST) {
				if(Application->Terminated == false) {
					Exceptions::clearlog = true; // this is not an error
					try {TFbase1::db->rollback();} catch(Exception & e) {}
					// always rollback() to make sure disconnect() succeeds
					TFbase1::db->disconnect(); // disable Timers (before ATTO)
					// Sometimes, when the computer resumes from standby mode
					// and the connection is lost, ATTO loses keyboard focus.
					// This was fixed by not using Application->MessageBox but
					// calling MessageBox directly. This also allows us to set
					// the right window owner using Screen->ActiveForm->Handle
					// For more information on this, see TFbase1::CreateParams()
					ATTO(AnsiString::LoadStr(EX_CONNECTIONFAILED).c_str());
					Application->Terminate(); Application->ProcessMessages();
				}
				return;
			} else if(db_ex->errors[0].native_err == ACCESS_DENIED1 ||
				db_ex->errors[0].native_err == ACCESS_DENIED2) {
				ATTO(AnsiString::LoadStr(EX_ACCESSDENIED).c_str());
				return;
			}
		} else if(db_ex && Application->Terminated) {return;}
#endif
	}

	//-----not a known exception -> so fatal_exception--------------
	// copyLog() only when the user chooses to terminate and restart
	if(Exceptions::debug_mode == false) {LOGEXCEPTION();}
	if(Application->Terminated == false) {
		SetForegroundWindow(GetActiveHandle());
		Odbc * db = TFbase1::db; TFbase1::db = NULL;
		// disables Timer/keep_alive (before the MessageBox)
		MSGEXC(message.c_str()); // display once, log always
		int r = ATTYN(AnsiString::LoadStr(EX_FATALERROR).c_str());
		TFbase1::db = db; // cleanup properly or re-enable Timer.
		if(r == ID_YES) { // only rollback if the user terminates.
			Exceptions::clearlog = false; // keep log for next run
			try {TFbase1::db->rollback();} catch(Exception & e) {}
			TFbase1::db->disconnect(); Application->Terminate();
			Application->ProcessMessages();
		}
	}
}
//---------------------------------------------------------------------------

