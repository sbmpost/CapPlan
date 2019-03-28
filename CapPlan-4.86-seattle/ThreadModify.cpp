//$Id: ThreadModify.cpp,v 1.38 2013/03/28 14:01:17 darseq Exp $
#include <vcl.h>
#pragma hdrstop
#include <algorithm>
#include "ThreadModify.h"
#include "Foptions.h"
#include "Fmdetail.h"
#pragma package(smart_init)
vector<ThreadModify *> ThreadModify::threads;
//---------------------------------------------------------------------------

__fastcall ThreadModify::ThreadModify(Odbc * db) : TThread(true) {
	LOGFUNC();
	thread_db = NULL;
	fdetails = NULL;
	logfile = NULL;
	try {
		ODBCparams params = db->getParams();
		// disabled: closing the application takes very long
		// and the CPTimeout (1 min) is simply not worth it.
		// params.pooling = true; // reuse old connections
		// params.driver_completion = SQL_DRIVER_NOPROMPT;
		thread_db = new Odbc(params);
		thread_db->link(&docs);
		thread_db->connect(db);
		if(thread_db->isConnected()) {
			fdetails = new TFdetails1(NULL);
			modified = true; // assume modified
			threads.push_back(this);
		} else {Abort();}
	} catch(Exception & e) {doTerminate(this); throw;}
	FreeOnTerminate = true; OnTerminate = doTerminate;
}
//---------------------------------------------------------------------------

void __fastcall ThreadModify::doTerminate(TObject* Sender) {
	LOGFUNC(); // doTerminate runs in the vcl main thread
	vector<ThreadModify *>::iterator it =
		find(threads.begin(),threads.end(),this);
	if(it != threads.end()) {threads.erase(it);}
	// erase the thread from the list before handling any
	// exceptions. This prevents a possible deadlock where
	// handleException calls ProcessMessages which in turn
	// calls FormDestroy which calls WaitFor if the thread
	// would still be in the list.
	if(FatalException) {
		Exception * e = (Exception *) FatalException;
		writeLog("Exception: "+e->ClassName()+
			" ## message: " + e->Message);
		if(dynamic_cast<EAbort *>(e) == NULL) {
			Exceptions::handleException(this,e);
		}
	}
	delete logfile; logfile = NULL;
	delete fdetails; fdetails = NULL;
	if(thread_db) {thread_db->unlink(&docs);}
	delete thread_db; thread_db = NULL;
	// If FreeOnTerminate is set, the thread object
	// itself is deleted by Classes::ThreadProc().
}
//---------------------------------------------------------------------------

void __fastcall ThreadModify::writeLog(AnsiString string) {
	if(logfile != NULL) {
		AnsiString message = "|"+DateToStr(Date())+"|";
		message += TimeToStr(Time())+"| "+string+"\r\n";
		logfile->Write(message.c_str(),message.Length());
	}
}
//---------------------------------------------------------------------------

void __fastcall ThreadModify::Execute(void) {
	if(Terminated) {return;} // readRow failed
	// Sometimes the debugger hangs while debugging a thread.
	// To prevent this from happening, disable advanced text
	// services (ctfmon.exe) in Regional and Language Settings
	// http://www.virtualdub.org/blog/pivot/entry.php?id=118
	AnsiString log_path = TFbase1::getPath("APPDATA");
	AnsiString stamp = FormatDateTime("yyyymmddhhmmss",Now());
	log_path = log_path+"\\"+stamp+".log";
	try {
		AnsiString macro("DocPending");
		// if(doc.schedule) {macro = "DocPending";}
		if(doc.rejected_date.IsEmpty() &&
			doc.planned_date.IsEmpty()) {macro = "";}
		// shellExecute waits for document to close,
		// cannot use the main vcl thread to execute.
		// todo: are all file functions reentrant?
		doc_path = TFoptions1::o.documents_path;
		doc_path = doc_path+"\\"+doc.data;
		doc.modified = TFbase1::shellExecute(doc_path,TDateTime(
			doc.lupdate_utc),false,doc.readonly,macro,&doc.size,
			&Terminated);

		// Create log if shellExecute succeeded.
		// Otherwise a 'regular' EAbort results
		// in a logfile which isn't deleted. If
		// the task is terminated while editing
		// a document however, it is not logged.
		try {logfile = new TFileStream(log_path,
			fmCreate | fmShareDenyWrite);}
		catch(Exception & e) {logfile = NULL;}
		writeLog(Exceptions::version);
		writeLog("shellExecute: "+doc_path);
		writeLog("doc modified: "+IntToStr((int) doc.modified));

		// bool show = doc.restore && doc.readonly == false;
		bool show = doc.restore && doc.modified;
		if(Terminated == false && show) {
			writeLog("Thread synchronize1 enter");
			Synchronize(&showDetails);
			writeLog("Thread synchronize1 exit");
			while(Terminated == false && fdetails->Visible) {
				// If the vcl thread is terminating,
				// stop sending messages to fdetails.
				if(Application->Terminated == false) {
					// If the user shows a secondary modal form,
					// DisableTaskWindows also disables fdetails.
					// So we bypass the vcl and make sure it is
					// enabled, calling the windows API directly.
					EnableWindow(fdetails->Handle,fdetails->enable);
				}
				Sleep(10); // prevents 50-100% CPU usage
			}
			writeLog("Thread sleep exit");
		}

		if(Terminated == false && (doc.modified ||
			(show && fdetails->ModalResult != mrIgnore))) {
			writeLog("Thread synchronize2 enter");
			// The main thread created fdetails and is
			// responsible for handling its messages.
			// Therefore our thread should be able to
			// WaitForSingleObject safely (see Fbase).
			Synchronize(&safeExecute);
			writeLog("Thread synchronize2 exit");
		}
	} __finally {save();} // always release locks
	// If the task is ended using the task manager, Fmain1
	// must be able to receive WM_CLOSE. If however another
	// modal form is active, Fmain1 needs to be disabled so
	// WM_CLOSE will never be sent. Because the other forms
	// do not receive a WM_CLOSE either, the task cannot be
	// terminated gracefully: is this a task manager bug?
	// Also see http://vb.mvps.org/articles/dd200109.asp
	if(Terminated) {writeLog("Thread was terminated");}
	else {delete logfile; logfile = NULL; DeleteFile(log_path);}
}
//---------------------------------------------------------------------------

void __fastcall ThreadModify::showDetails(void) {
	LOGFUNC();
	writeLog("\tshowDetails");
	int C = 0; // todo: a window nr method is necessary
	int top = fdetails->Top; int left = fdetails->Left;
	if(top+fdetails->Height+C<=Screen->WorkAreaHeight) {fdetails->Top=top+C;}
	if(left+fdetails->Width+C<=Screen->WorkAreaWidth) {fdetails->Left=left+C;}
	fdetails->view_doc(&doc,false,false); // vcl is not thread safe
}
//---------------------------------------------------------------------------

void __fastcall ThreadModify::readDocData(void) {
	LOGFUNC();
	writeLog("\treadDocData");
	AnsiString query = "select LAST_UPDATE_UTC,DOC_DATA from docs "+
		Policy::lock+"where PERSONAL_ID="+doc.per_id+" and DOC_NAME="+
		QuotedStr(doc.name)+Policy::lock_end;
	docs.execSQL(query); docs.next();
	doc_data = docs.fieldByName("DOC_DATA").toString();
	lupdate_utc = docs.fieldByName("LAST_UPDATE_UTC").toString();
	writeLog("\tlupdate_utc: "+lupdate_utc+", "+doc_data);
}
//---------------------------------------------------------------------------

void __fastcall ThreadModify::safeExecute(void) {
	LOGFUNC();
	writeLog("\tsafeExecute");
	// note that rollback operates on db_thread.
	TRYSLOCK(docs.update(doc); readDocData(););
	try {
		if(doc_data != doc.data) {
			// compare doc_data, not path
			AnsiString src_path = doc_path;
			writeLog("\tcreatepath: "+doc_data);
			doc_path = TFbase1::createDocPath(doc_data);
			writeLog("\tdeletefile: "+doc_path);
			if(FileExists(doc_path)) {TFbase1::deletefile(doc_path);}
			// If renamefile fails, another process may be holding a handle
			// to the file. For example, a background process may have opened
			// the file just after it was closed, but before safeExecute runs.
			// If this happens, copy the file without locking (it is unlikely
			// that the background process will be writing to the file, but a
			// write lock could still have been obtained). The alternative is
			// to rollback the database but then the 'signed', 'schedule' and
			// 'restore' flags are ignored and file changes go by unnoticed.
			writeLog("\trenamefile: "+src_path);
			try {TFbase1::renamefile(src_path,doc_path,false);}
			catch(EAbort & e) {
				writeLog("\tcopyfile: "+src_path);
				TFbase1::copyfile(src_path,doc_path,NULL,false);
			}
			writeLog("\tremovedir");
			RemoveDir(ExtractFileDir(src_path)); // ignore return value
		}
		if(doc.modified) {
			// If setLastWrite fails, the file is probably write locked by
			// another process. So instead of updating the last write time,
			// read it and update the database itself. Note that this time
			// value depends on the local system clock so it may actually
			// be smaller than the previous update time (doc.lupdate_utc).
			writeLog("\tsetLastWrite: "+doc_path);
			try {TFbase1::setLastWrite(doc_path,TDateTime(lupdate_utc),false);}
			catch(EAbort & e) {
				FILETIME filetime_loc,filetime_utc;
				writeLog("\tfileopen: "+doc_path);
				int file_handle = TFbase1::fileopen(doc_path,false);
				try {
					writeLog("\tfileinfo: "+doc_path);
					filetime_utc = TFbase1::fileinfo(
						file_handle).ftLastWriteTime;
					FileTimeToLocalFileTime(&filetime_utc,&filetime_loc);
					doc.last_update = TFbase1::FileTimeToDateTime(
						filetime_loc).DateTimeString();
					doc.lupdate_utc = TFbase1::FileTimeToDateTime(
						filetime_utc).DateTimeString();
					writeLog("\tlast_update: "+doc.last_update+
						", lupdate_utc:"+doc.lupdate_utc);
					TRYSLOCK(docs.setLastUpdate(doc););
				} __finally {TFbase1::closefile(file_handle);}
				lupdate_utc = doc.lupdate_utc;
			}
		}
	} catch(Exception & e) {rollback(); throw;}
	save();

	try {
		if(!doc.pending_review && !doc.pending_schedule &&
			doc.planned_date.IsEmpty() && doc.schedule) {
			TDateTime lutc(lupdate_utc);
			// Another macroExecute may still be running if an exception
			// occurred within it. For as long as the MessageBox shows, it
			// dispatches WM_NULL messages (see below) and our thread will
			// be allowed to Synchronize. To make sure we can enter safely,
			// macroExecute calls handleException at the very end.
			if(doc.schlist) {TFbase1::macroExecute(doc_path,lutc,"ToSchedule");}
			else {TFbase1::macroExecute(doc_path,lutc,"ToReview");}
		}
	} __finally {
#if defined MS_SQL || defined ACCESS
		TFmdetail1 * fmdetail1 = dynamic_cast<TFmdetail1 *>(Screen->ActiveForm);
		if(fmdetail1 && fmdetail1->Enabled && fmdetail1->Active) {
			// A routine which shows a modal window (MessageBox/Exception/Form)
			// results in a new message pump. This allows for other code in the
			// program to be executed like a painting or timer handler. Because
			// parent windows are disabled, input messages arrive at the modal
			// window. This window should not change any data of its caller as
			// this may lead to a race condition. So we check Enabled to see if
			// fmdetail is printing and Active to see if a MessageBox is open.
			// Only if both are false, it is safe to call fmdetail1->refreshRow.
#ifdef ACCESS
			Sleep(1000); // http://support.microsoft.com/kb/180223
			// ACCESS needs some time to update (dbForceOSFlush).
			// Unfortunately we cannot use ODBC to force a flush.
#endif
			fmdetail1->refreshRow();
		}
#endif
	}
}
//---------------------------------------------------------------------------

