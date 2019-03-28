//$Id: ThreadModify.h,v 1.11 2013/02/16 12:18:36 darseq Exp $
#ifndef ThreadModifyH
#define ThreadModifyH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include "Fdetails.h"
#include "Tracks.h"
#include "Odbc.h"
//---------------------------------------------------------------------------

class ThreadModify : public TThread
{
private:
	bool modified;
	TFdetails1 * fdetails;
	TFileStream * logfile;
	AnsiString doc_path; Docs docs;
	AnsiString doc_data,lupdate_utc;
	void __fastcall doTerminate(TObject* Sender);
	void __fastcall writeLog(AnsiString string);
	void __fastcall showDetails(void);
	void __fastcall readDocData(void);
	void __fastcall safeExecute(void);
	void __fastcall save(void) {
		LOGFUNC(); writeLog("save");
		if(modified && thread_db->isConnected()) {
#ifdef ACCESS
			TRYSLOCK(docs.execSQL("COMMIT"););
			docs.execSQL("BEGIN TRANSACTION");
#else
			TRYSLOCK(thread_db->commit(););
#endif
		} modified = false;
	}
	void __fastcall rollback(void) {
		LOGFUNC(); writeLog("rollback");
		if(modified && thread_db->isConnected()) {
#ifdef ACCESS
			docs.execSQL("ROLLBACK");
			docs.execSQL("BEGIN TRANSACTION");
#else
			thread_db->rollback();
#endif
		} modified = false;
	}
protected:
	void __fastcall Execute(void);
public:
	Odbc * thread_db; Document doc;
	static vector<ThreadModify *> threads;
	__fastcall ThreadModify(Odbc * db);
};
//---------------------------------------------------------------------------
#endif

