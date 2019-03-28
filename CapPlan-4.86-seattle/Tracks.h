//$Id: Tracks.h,v 1.137 1998/01/22 19:37:32 darseq Exp $
#ifndef TracksH
#define TracksH
//---------------------------------------------------------------------------
// #include <ComCtrls.hpp> // progressbar
#include <ComObj.hpp> // CreateClassID
#include <StrUtils.hpp>
#include <math.hpp>
#include "Query.h"
#include "defines.h"
#include "Exceptions.h"

class Schedule {
	public:
		id_t id; AnsiString name;
		int wstart,wstop,dstart,dstop,tunits;
		bool hidden; AnsiString created_by,note;

		__fastcall Schedule(void) {
			id = UNDEFINED; name = UNDEFINED;
			wstart = wstop = dstart = dstop = tunits = 0;
			hidden = true; created_by = note = UNDEFINED;
		}

		void __fastcall get(Query & query) {
			id = query.fieldByName("SCHEDULE_ID").toString();
			name = query.fieldByName("SCHEDULE_NAME").toString();
			wstart = StrToInt(query.fieldByName("WEEK_START").toString());
			wstop = StrToInt(query.fieldByName("WEEK_STOP").toString());
			dstart = StrToInt(query.fieldByName("DAY_START").toString());
			dstop = StrToInt(query.fieldByName("DAY_STOP").toString());
			tunits = StrToInt(query.fieldByName("DAY_UNITS").toString());
			hidden = StrToInt(query.fieldByName("HIDDEN").toString());
			created_by = query.fieldByName("CREATED_BY").toString();
		}
};
//---------------------------------------------------------------------------

class Scheds : public Query {
	private:
	public:
		AnsiString __fastcall getText(id_t schedule_id,AnsiString field_name);
		void __fastcall check(id_t sch_id) {getText(sch_id,"SCHEDULE_ID");}
		void __fastcall insert(Schedule & schedule);
		void __fastcall remove(Schedule & schedule);
};
//---------------------------------------------------------------------------

class Entry {
	public:
		id_t employee_id; TDateTime from; TDateTime until; // key
		TDateTime date_diff; // keep value when unit settings change
		id_t app_id; bool cancelled; int pattern; id_t uuid; // no key
		// when the employee is not part of the grid
		id_t schedule_id; AnsiString schedule_name;

		__fastcall Entry() {
			// maybe let Query childs do their linking in constr/~destr?
			employee_id = UNDEFINED; from = until = date_diff = 0;
			app_id = UNDEFINED; cancelled = false; pattern = 0;
			uuid = UNDEFINED; // export vEvent support.
		}

		unsigned int __fastcall hashcode(void) {
			// 0.25 * 24 = 06:00 hour, x.25 = date and 06:00 hour.
			return StrToInt(employee_id)+
				RoundTo(ACCURACY * double(from),0)+
				RoundTo(ACCURACY * double(until),0);
		}

		// TDateTime operators
		static bool __fastcall less_than(const TDateTime & d1,
			const TDateTime & d2) {
			int date_time1 = RoundTo(ACCURACY * double(d1),0);
			int date_time2 = RoundTo(ACCURACY * double(d2),0);
			return date_time1 < date_time2;
		}

		static bool __fastcall equal(const TDateTime & d1,
			const TDateTime & d2) {
			return less_than(d1,d2) == false &&
				less_than(d2,d1) == false;
		}

		// Entry compare functions and operators
		static bool __fastcall less(const Entry & e1,const Entry & e2) {
			if(e1.schedule_name < e2.schedule_name) {return true;}
			else if(e1.schedule_name > e2.schedule_name) {return false;}
			else if(e1.schedule_id < e2.schedule_id) {return true;}
			else if(e1.schedule_id > e2.schedule_id) {return false;}
			else {return int(e1.from) < int(e2.from);}
		}

		bool __fastcall operator ==(const Entry & entry) const {
			return entry.employee_id == employee_id &&
				equal(entry.from,from) &&
				equal(entry.until,until);
		}

		bool __fastcall operator !=(const Entry & entry) const {
			return (entry == *this) == false;
		}

		void __fastcall get(Query & query) {
			employee_id = query.fieldByName("EMPLOYEE_ID").toString();
			from = TDateTime(query.fieldByName("DATE_FROM").toString());
			until = TDateTime(query.fieldByName("DATE_UNTIL").toString());
			date_diff = TDateTime(query.fieldByName("DATE_DIFF").toString());
			app_id = query.fieldByName("APP_ID").toString();
			AnsiString is_cancelled = query.fieldByName("CANCELLED").toString();
			cancelled = is_cancelled != UNDEFINED ? StrToInt(is_cancelled) : 0;
			pattern = StrToInt(query.fieldByName("PATTERN").toString());
			uuid = query.fieldByName("UUID").toString();
		}
};
//---------------------------------------------------------------------------

class Entries : public Query {
	public:
		void __fastcall check(id_t per_id) {
			PROG_IF(per_id == UNDEFINED,
				"Invalid id, cannot check entries");
			execSQL("select APP_ID from entries "+Policy::lock+
				"where EMPLOYEE_ID="+per_id+Policy::lock_end);
			if(next()) {throw UserException(TR_ENTRIESEXIST);}
		}
		void __fastcall insert(Entry & e,AnsiString uuid = UNDEFINED) {
			e.uuid = uuid;
			if(e.uuid == UNDEFINED) {e.uuid = CreateClassID().SubString(2,36);}
			id_t app_id = e.app_id;
			if(app_id == UNDEFINED) {app_id = "NULL";}
			PROG_IF(e.employee_id == UNDEFINED,"Invalid employee id");
			execSQL("insert into entries "+Policy::lock+
				"(EMPLOYEE_ID,DATE_FROM,DATE_UNTIL,DATE_DIFF,"
				"PATTERN,UUID,APP_ID) values("+e.employee_id+"," DS+
				YMDHMS(e.from)+DS "," DS+YMDHMS(e.until)+DS "," DS+
				YMDHMS(e.date_diff)+DS ","+IntToStr(e.pattern)+
				",'"+e.uuid+"',"+app_id+")");
		}
		void __fastcall remove(Entry & e) {
			id_t app_id = e.app_id;
			if(app_id == UNDEFINED) {app_id = " is null";}
			else {app_id = "="+e.app_id;}
			execSQL("delete from entries "+Policy::lock+
				"where EMPLOYEE_ID="+e.employee_id+
				" and DATE_FROM=" DS+YMDHMS(e.from)+DS
				" and DATE_UNTIL=" DS+YMDHMS(e.until)+DS
				" and APP_ID"+app_id);
		}
		void __fastcall update(Entry & e1,Entry & e2) {
			id_t app_id = e1.app_id;
			if(app_id == UNDEFINED) {app_id = " is null";}
			else {app_id = "="+e1.app_id;}
			PROG_IF(e2.employee_id == UNDEFINED,"Invalid employee id");
			execSQL("update entries "+Policy::lock+"set EMPLOYEE_ID="+
				e2.employee_id+",DATE_FROM=" DS+YMDHMS(e2.from)+DS
				",DATE_UNTIL=" DS+YMDHMS(e2.until)+DS ",DATE_DIFF=" DS+
				YMDHMS(e2.date_diff)+DS",PATTERN="+IntToStr(e2.pattern)+
				" where EMPLOYEE_ID="+e1.employee_id+" and DATE_FROM=" DS+
				YMDHMS(e1.from)+DS " and DATE_UNTIL=" DS+YMDHMS(e1.until)+DS
				" and APP_ID"+app_id);
		}
};
//---------------------------------------------------------------------------

class Apps : public Query {
	private:
	public:
		AnsiString __fastcall getText(id_t app_id,AnsiString field_name);
		void __fastcall check(id_t app_id) {getText(app_id,"APP_ID");}
		void __fastcall validate(id_t app_id,AnsiString username);
		id_t __fastcall insert(id_t per_id,bool exclusive,bool paid,int color,
			AnsiString charge,AnsiString info,AnsiString created_by);
		void __fastcall remove(id_t app_id);
		void __fastcall erase(id_t per_id,AnsiString username,bool root_user);
};
//---------------------------------------------------------------------------

class Document {
	public:
		bool readonly;
		bool modified;
		bool schlist;
		bool restore;

		id_t per_id;
		id_t name;
		int size;
		// AnsiString comment;
		bool is_signed;
		bool is_urgent;
		bool schedule;
		bool pending_review;
		bool pending_schedule;
		// bool pending_repurchase;
		// bool pending_invoice;
		AnsiString last_update;
		AnsiString lupdate_utc;
		AnsiString rejected_date;
		AnsiString planned_date;
		AnsiString ordered_date;
		AnsiString charged_date;
		AnsiString created_date;
		AnsiString created_by;
		// AnsiString statetext;
		AnsiString data;

		__fastcall Document(void) {
			per_id = UNDEFINED;
			name = UNDEFINED;
			size = 0;
			is_signed = false;
			is_urgent = false;
			schedule = false;
			pending_review = false;
			pending_schedule = false;
			last_update = UNDEFINED;
			lupdate_utc = UNDEFINED;
			rejected_date = UNDEFINED;
			planned_date = UNDEFINED;
			ordered_date = UNDEFINED;
			charged_date = UNDEFINED;
			created_date = UNDEFINED;
			created_by = UNDEFINED;
			data = UNDEFINED;
		}
		void __fastcall initialize(bool doc_restore,bool review_tab) {
			restore = doc_restore; readonly = modified = false;
			schlist = pending_schedule || planned_date.Length() ||
				(pending_review == false && (rejected_date.Length() ||
				review_tab == false));
		}
		// void __fastcall get(Query & query) {}
};
//---------------------------------------------------------------------------

class Docs : public Query {
	private:
		// unsigned char io_buffer[IOBUFFER];
	public:
		// TProgressBar * progressbar;
		AnsiString __fastcall getText(id_t doc_id,AnsiString field_name);
		void __fastcall check(id_t doc_id) {getText(doc_id,"PERSONAL_ID");}
		AnsiString __fastcall EscapeStr(AnsiString str);
		int __fastcall getDocNumber(id_t per_id,AnsiString name,AnsiString ext);
		bool __fastcall getId(id_t doc_id,id_t & per_id,AnsiString & doc_name);
		void __fastcall insert(id_t doc_id,int doc_size,AnsiString comment,
			AnsiString username,AnsiString statetext,AnsiString doc_data) {
			AnsiString per_id,doc_name; getId(doc_id,per_id,doc_name);
			PROG_IF(per_id == UNDEFINED,"Invalid id, cannot insert document");
			execSQL("insert into docs "+Policy::lock+"values("+per_id+","+
				QuotedStr(doc_name)+","+IntToStr(doc_size)+","+QuotedStr(
				comment)+"," CURRENT_TIMESTAMP "," UTC_TIMESTAMP ",0,0,0,0,0,0,"
				"NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL," CURRENT_TIMESTAMP
				","+QuotedStr(username)+","+QuotedStr(statetext)+","+
#ifdef MS_SQL // a document path can be 260 characters at the most
				"convert(varchar,datepart(year," UTC_TIMESTAMP "))+'-'+"
				"convert(varchar,datepart(month," UTC_TIMESTAMP "))+"+
				QuotedStr("\\"+doc_data)+")");
#elif defined INNODB // todo: what if doc_data is truncated?
				"concat(date_format(" UTC_TIMESTAMP ",'%Y-%c'),"+
				QuotedStr(AnsiReplaceStr("\\"+doc_data,"\\","\\\\"))+"))");
#elif defined ACCESS // todo: what if doc_data is truncated?
				"format(" UTC_TIMESTAMP ",'YYYY-MM')+"+
				QuotedStr("\\"+doc_data)+")");
#endif
		}
		void __fastcall update(Document & doc) {
			LOGFUNC();
			AnsiString doc_data = doc.per_id+"\\"+doc.name;
			AnsiString sch_path = SCHEDULEPATH "\\"+doc_data;
			bool schedule_path = doc.data.Pos(sch_path) != 0;
			// Once SCHEDULEPATH is included, it is permanent.
			if(doc.schedule || schedule_path) {doc_data = sch_path;}
			if(doc.modified || (doc.schedule && schedule_path == false)) {
#ifdef MS_SQL
				doc_data = "convert(varchar,datepart(year," UTC_TIMESTAMP "))+"
					"'-'+convert(varchar,datepart(month," UTC_TIMESTAMP "))+"+
					QuotedStr("\\"+doc_data);
#elif defined INNODB
				doc_data = "concat(date_format(" UTC_TIMESTAMP ",'%Y-%c'),"+
					QuotedStr(AnsiReplaceStr("\\"+doc_data,"\\","\\\\"))+")";
#elif defined ACCESS
				doc_data = "format(" UTC_TIMESTAMP ",'YYYY-MM')+"+
					QuotedStr("\\"+doc_data);
#endif
#ifdef INNODB
			} else {doc_data = QuotedStr(AnsiReplaceStr(doc.data,"\\","\\\\"));}
#else
			} else {doc_data = QuotedStr(doc.data);}
#endif
			AnsiString is_pending = IntToStr((int) doc.schedule);
			AnsiString update = "update docs "+Policy::lock+"set ";
			update += "SIGNED="+IntToStr((int) doc.is_signed)+",";
			update += "URGENT="+IntToStr((int) doc.is_urgent)+",";
			if(doc.modified) {
				update += "DOC_SIZE="+IntToStr(doc.size)+",";
				update += "LAST_UPDATE=" CURRENT_TIMESTAMP ","
					"LAST_UPDATE_UTC=" UTC_TIMESTAMP ",";
			}
			if(doc.restore) { // if doc.planned, doc.schedule should be set.
				if(doc.schlist) {update+="PENDING_SCHEDULE="+is_pending+",";}
				else {update+="PENDING_REVIEW="+is_pending+",";}
				if(doc.ordered_date.Length()) {update+="PENDING_REPURCHASE=1,";}
				if(doc.charged_date.Length()) {update+="PENDING_INVOICE=1,";}
			} else if(doc.planned_date.IsEmpty()) { // is_pending = pending?
				if(doc.schlist) {update+="PENDING_SCHEDULE="+is_pending+",";}
				else {update+="PENDING_REVIEW="+is_pending+",";}
			}
			// update doc data month folder if
			// something changed (!= mrIgnore)
			update += "DOC_DATA="+doc_data+" ";
			update += "where PERSONAL_ID="+doc.per_id+
				" and DOC_NAME="+QuotedStr(doc.name);
			execSQL(update);
		}
		void __fastcall setLastUpdate(Document & doc) {
			LOGFUNC();
			execSQL("update docs "+Policy::lock+"set LAST_UPDATE="
				DS+YMDHMS(TDateTime(doc.last_update))+DS",LAST_UPDATE_UTC="
				DS+YMDHMS(TDateTime(doc.lupdate_utc))+DS" where PERSONAL_ID="+
				doc.per_id+" and DOC_NAME="+QuotedStr(doc.name));
		}
		// void __fastcall checkSize(int doc_size);
		// void __fastcall setData(id_t doc_id,TStream * Stream);
		// int __fastcall getData(id_t doc_id,int doc_size,TStream * stream);
		void __fastcall remove(id_t doc_id) {
			AnsiString per_id,doc_name; getId(doc_id,per_id,doc_name);
			PROG_IF(per_id == UNDEFINED,"Invalid id, cannot remove document");
			execSQL("delete from docs "+Policy::lock+"where PERSONAL_ID="+
				per_id+" and DOC_NAME="+QuotedStr(doc_name));
		}
		void __fastcall erase(id_t per_id) {
			PROG_IF(per_id == UNDEFINED,
				"Invalid id, cannot remove documents");
			execSQL("delete from docs "+Policy::lock+
				"where PERSONAL_ID="+per_id);
		}
};
//---------------------------------------------------------------------------

class Employees : public Query {
	public:
		void __fastcall insert(id_t schedule_id,id_t per_id) {
			PROG_IF(schedule_id == UNDEFINED || per_id == UNDEFINED,
				"Invalid id, cannot insert employee");
			execSQL("select max(EMPLOYEE_ORDER) as MAX_ORDER from employees "+
				Policy::lock+"where SCHEDULE_ID="+schedule_id+Policy::lock_end);
			PROG_IF(next() == false,"Could not get max employee order");

			int order = 1; // if this schedule has no employees yet
			AnsiString max_order = fieldByName("MAX_ORDER").toString();
			if(max_order != UNDEFINED) {order = StrToInt(max_order)+1;}
			execSQL("insert into employees "+Policy::lock+"values("+
				schedule_id+","+per_id+","+IntToStr(order)+")");
		}
		void __fastcall remove(id_t schedule_id,id_t per_id) {
			PROG_IF(schedule_id == UNDEFINED || per_id == UNDEFINED,
				"Invalid id, cannot remove employee");
			execSQL("delete from employees "+Policy::lock+
				"where SCHEDULE_ID="+schedule_id+
				" and PERSONAL_ID="+per_id);
		}
		void __fastcall erase(id_t per_id) {
			PROG_IF(per_id == UNDEFINED,
				"Invalid id, cannot remove employees");
			execSQL("delete from employees "+Policy::lock+
				"where PERSONAL_ID="+per_id);
		}
};
//---------------------------------------------------------------------------
#endif

