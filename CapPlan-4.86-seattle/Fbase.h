//$Id: Fbase.h,v 1.109 1998/01/22 19:37:31 darseq Exp $
#ifndef FbaseH
#define FbaseH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ComCtrls.hpp>
#include <ComObj.hpp>
#include <ExtCtrls.hpp>
#include <vector>
#include "Exceptions.h"
#include "defines.h"
#include "Clients.h"
#include "Tracks.h"
#include "Odbc.h"
#include "DataRows.h"
//---------------------------------------------------------------------------

#define TRYALOCK(lock_code,lock_policy,request_no_rollback)\
do {\
	try {\
		lock_policy();\
		lock_code\
		Policy::clrLock();\
	} catch(Exception & e) {\
		Policy::clrLock();\
		if(dynamic_cast<UserException *>(&e)) {throw;}\
		/* It is assumed that UserExceptions are checked beforehand */\
		/* only the first read statements may produce UserException */\
		DBException * db_ex = dynamic_cast<DBException *>(&e);\
		if(request_no_rollback && db_ex && db_ex->errors.size()) {\
			if(db_ex->errors[0].native_err == LOCK_TIMEOUT) {\
				throw UserException(SELECTION_LOCKED);\
			} else if(db_ex->errors[0].native_err == DUPLICATEKEY ||\
				db_ex->errors[0].native_err == DUPLICATEUNQ) {\
				throw UserException(DUPLICATE_KEY);\
			}\
		}\
		rollback();\
		throw;\
	}\
} while(0)

#define TRYULOCK(lock_code) TRYALOCK(lock_code,Policy::setULock,true)
#define TRYSLOCK(lock_code) TRYALOCK(lock_code,Policy::setSLock,false)
// ULOCK: timeout = throw without rollback, integrity failure not possible
// SLOCK: timeout = throw with rollback, integrity: continue without rollback?
// suggestion: SLOCK disable timeout? how about multiple queries executing?
// new suggestion: ULOCK to prevent deadlocks as much as possible. no loss
// of concurrency between capplan clients because integrity check() 's are
// done by the database now. If integrity failure also rollback (we assume
// integrity failures do not happen very often).
//---------------------------------------------------------------------------

class TFbase1 : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormKeyPress(TObject *Sender, char &Key);
private:	// User declarations
	bool Fmodified;
	static Variant word_app; static AnsiString word_cap;
	void __fastcall CreateParams(TCreateParams &Params);
	bool __fastcall MainFormCloseQuery(bool show_message);
	static int __fastcall doExecMacro(AnsiString macro,AnsiString params = "");
protected:
	__property bool modified = {read = getModified,write = setModified};
	virtual void __fastcall setModified(bool value) {Fmodified = value;}
	virtual void __fastcall save(void) {
		LOGFUNC();
		if(db->isConnected()) {
#ifdef ACCESS
			TRYSLOCK(db_query.execSQL("COMMIT"););
			db_query.execSQL("BEGIN TRANSACTION");
#else
			TRYSLOCK(db->commit(););
#endif
		} modified = false;
	}
	virtual void __fastcall rollback(void) {
		LOGFUNC();
		if(db->isConnected()) {
#ifdef ACCESS
			db_query.execSQL("ROLLBACK");
			db_query.execSQL("BEGIN TRANSACTION");
#else
			db->rollback();
#endif
		} modified = false;
		for(int i=0;i != Screen->FormCount && Screen->Forms[i]->Visible;i++) {
			TFbase1 * form = dynamic_cast<TFbase1 *>(Screen->Forms[i]);
			if(form) {
				if(form != Application->MainForm) {form->ModalResult = mrAbort;}
				else {form->modified = false;} // sets modified value of Fmain1
				// The Forms property is always sorted according to the
				// display order. When the program returns to the vcl
				// thread, FormClose is immediately called and no other
				// events can interfere. The form is closed because the
				// entire transaction and all entry changes are invalid.
				// 'double' close when called by FormClose?
			}
		}
	}
	TWinControl * __fastcall exitFirst(TObject * Sender);
	virtual void __fastcall WndProc(TMessage& Message);
	static TPrinter * __fastcall resetPrinter(void);
	static TFbase1 * __fastcall showPanel(int msgindex);
	static void __fastcall closePanel(TFbase1 * form);
	static void __fastcall openWordDoc(AnsiString pathname);
	static void __fastcall closeWordDoc(void);
	static void __fastcall cleanWordApp(void);
public:		// User declarations
	static bool root_user;
	static TDateTime min_time,max_time;
	static TDateTime min_date,max_date;
	static TDateTime current_date;
	static TDateTime previous_date;
	static vector<Person> employees;
	static vector<Schedule> schedules;
	static Schedule grid;
	static Odbc * db;
	static Query db_query;
	__fastcall TFbase1(TComponent* Owner);
	static AnsiString __fastcall writePngImage(AnsiString filename,
		Graphics::TBitmap * graphics_bitmap);
	void __fastcall refreshForms(void);
	bool __fastcall getModified(void) {return Fmodified;}
	virtual void __fastcall Refresh(void) {/*LOGFUNC();*/}
	static void __fastcall flushPaint(void);
	static void __fastcall checkTimeBounds(TDateTime from,TDateTime until);
	static TDateTime __fastcall weekDate(TDateTime from,TDateTime to);
	static AnsiString __fastcall MinutesToStr(int minutes);
	static int __fastcall StrToMinutes(AnsiString string);
	static void __fastcall getEmployeeIds(Query & query,
		id_t schedule_id,list<id_t> * employee_ids);
	static bool __fastcall addEmployee(Query & query,id_t employee_id);
	static bool __fastcall addSchedule(Query & query,id_t view_id);
	static unsigned int __fastcall findEmployee(id_t employee_id);
	static unsigned int __fastcall findSchedule(id_t view_id);
	static bool __fastcall eatMessages(void);
	static bool __fastcall shellExecute(AnsiString & pathname,
		TDateTime lupdate_utc = TDateTime(),bool locate = true,
		bool file_readonly = false,AnsiString macro = "",
		int * new_size = NULL,bool * thread_term = NULL);
	static int __fastcall macroExecute(AnsiString pathname,
		TDateTime lupdate_utc,AnsiString macro,
		AnsiString params = "",bool show_message = false);
	static void __fastcall closefile(int & file_handle);
	static int __fastcall fileopen(AnsiString pathname,bool writelock);
	static BY_HANDLE_FILE_INFORMATION __fastcall fileinfo(int file_handle);
	static TFileStream * __fastcall createfile(AnsiString pathname);
	static void __fastcall deletefile(AnsiString pathname);
	static void __fastcall renamefile(AnsiString source,
		AnsiString destination,bool show_message = true);
	static void __fastcall makedir(AnsiString pathname);
	static void __fastcall copyfile(AnsiString source,AnsiString destination,
		TProgressBar * progressbar = NULL,bool writelock_source = true);
	static AnsiString __fastcall normalizePath(AnsiString path);
	static AnsiString __fastcall createDocPath(AnsiString path);
	static AnsiString __fastcall getPath(const char * environvariable);
	static AnsiString __fastcall file2tmp(AnsiString pathname);
	static AnsiString __fastcall toFilename(AnsiString name);
	static void __fastcall exportCsvFile(TDataRows * rows);
	static void __fastcall restoreNetDrive(AnsiString netdrive);
	static TDateTime __fastcall FileTimeToDateTime(FILETIME filetime);
	static FILETIME __fastcall DateTimeToFileTime(TDateTime datetime);
	static void __fastcall setLastWrite(AnsiString pathname,
		TDateTime lwrite_utc,bool show_message = true);
	static bool __fastcall checkVersion(TDateTime lupdate_utc,
		BY_HANDLE_FILE_INFORMATION & file_info);
	static void __fastcall sendMail(AnsiString to,AnsiString body,
		AnsiString subject,AnsiString pathname);
};
//---------------------------------------------------------------------------
extern PACKAGE TFbase1 *Fbase1;
//---------------------------------------------------------------------------
#endif

