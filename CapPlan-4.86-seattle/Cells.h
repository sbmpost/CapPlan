// $Id: Cells.h,v 1.24 2013/08/21 19:35:28 darseq Exp $
//---------------------------------------------------------------------------
#ifndef CellsH
#define CellsH
//---------------------------------------------------------------------------
#include "Clients.h"
#include "Tracks.h"

class Data {
	public:
		AnsiString info; bool exclusive;
		int app_color; AnsiString app_charge;
		AnsiString created_by; Person person;
		__fastcall Data(void) {
			info = UNDEFINED;
			exclusive = false;
			app_color = 0;
			app_charge = UNDEFINED;
			created_by = UNDEFINED;
		}
		bool __fastcall operator !=(const Data & data) const {
			return data.info != info ||
				data.exclusive != exclusive ||
				data.app_color != app_color ||
				data.app_charge != app_charge ||
				data.created_by != created_by ||
				data.person != person;
		}
		void __fastcall get(Query & query) {
			info = query.fieldByName("INFOTEXT").toString();
			AnsiString is_exclusive = query.fieldByName("EXCLUSIVE").toString();
			exclusive = is_exclusive != UNDEFINED ? StrToInt(is_exclusive) : 0;
			// blocked appointments do not have a color
			AnsiString color = query.fieldByName("COLOR").toString();
			if(color != UNDEFINED) {app_color = StrToInt(color);}
			app_charge = query.fieldByName("CHARGE").toString();
			created_by = query.fieldByName("CREATED_BY").toString();
			person.get(query);
		}
};
//---------------------------------------------------------------------------

class Cell {
	public:
		bool selected,changed,planned; Entry entry; Data data;
		// cancelled/entry/data are only defined when planned == true. They are
		// set by readCell and getGrid so other functions can use them without
		// querying the database again. They are also used for comparing cells.
		// key part of an entry is set by the user and therefore always defined.
		__fastcall Cell(void) {selected = changed = planned = false;}

		// hashcode uses the entry key part
		unsigned int __fastcall hashcode(void) {return entry.hashcode();}

		// Equality used for hash search/insertion
		bool __fastcall operator ==(const Cell & cell) const {
			return cell.entry == entry; // compare uses the key part
		}

		// InEquality used for cell comparing before updating
		bool __fastcall operator !=(const Cell & cell) const {
			bool neq = cell.entry != entry || cell.planned != planned;
			if(planned == false) {return neq;} // both not planned? return neq
			// compare records id's as well, although the user will not able to
			// to tell the difference when the other data fields are identical.
			return neq || cell.entry.app_id != entry.app_id ||
				cell.entry.cancelled != entry.cancelled ||
				cell.entry.pattern != entry.pattern ||
				cell.data != data;
		}

		// Less than used for jumping through the selection
		static bool __fastcall less(const Cell & c1,const Cell & c2) {
			if(c1.changed && c2.changed == false) {return true;}
			else if(c1.changed == false && c2.changed) {return false;}
			else {return Entry::less(c1.entry,c2.entry);} // uses key part
		}

		// Jumping through the selection the other way around
		static bool __fastcall greater(const Cell & c1,const Cell & c2) {
			if(c1.changed && c2.changed == false) {return false;}
			else if(c1.changed == false && c2.changed) {return true;}
			else {return Entry::less(c2.entry,c1.entry);}
		}
};
//---------------------------------------------------------------------------

class Cells : public Query {
	public:
		void __fastcall readData(Cell & cell) {
			LOGFUNC();
			// read appointment data belonging to the cell
			if(cell.entry.app_id == UNDEFINED) {return;} // blocked cell?
			execSQL("select EXCLUSIVE,CANCELLED,INFOTEXT,COLOR,CHARGE,"
				"apps.CREATED_BY,personal.PERSONAL_ID,LASTNAME,ADDRESS,"
				"CITY,TELEPHONE,TELEPHONE2,POSTCODE from apps,personal "
				"where apps.PERSONAL_ID=personal.PERSONAL_ID and APP_ID="+
				cell.entry.app_id+Policy::lock_end);
			cell.planned = next(); if(cell.planned == false) {return;}
			cell.entry.cancelled=StrToInt(fieldByName("CANCELLED").toString());
			cell.data.get(*this);
		}

		// -All- entry records of the employee row are locked while checking for
		// overlap. This is because the entire row may contain entries that are
		// within the time range of the key. Checking the key prevents different
		// users from inserting overlapping cells although the user itself can
		// still insert overlapping cells using the ALT-key (see entriesNew).
		// Personal and appointment information must also be read so the user
		// can see if someone else inserted an appointment on an empty position.
		// MS_SQL:optimized by not locking personal and appointment information.
		void __fastcall readCell(Cell & cell,Cell * drag = NULL) {
			LOGFUNC();
			execSQL("select * from (apps right join entries "+Policy::lock+
				"on apps.APP_ID=entries.APP_ID) left join personal "
				"on apps.PERSONAL_ID=personal.PERSONAL_ID where EMPLOYEE_ID="+
				cell.entry.employee_id+" and DATE_FROM<" DS+
				cell.entry.until.DateTimeString()+DS " and DATE_UNTIL>" DS+
				cell.entry.from.DateTimeString()+DS" order by EMPLOYEE_ID,"
				"DATE_FROM,DATE_UNTIL,CANCELLED desc"+Policy::lock_end);
				// sets locks on cancelled appointments as well
				// When readCell reads zero records because the employee or the
				// schedule was removed, getgrid removes the cell from the hash

			if(cell.planned == false) {
				cell.entry.app_id = UNDEFINED;
				cell.entry.cancelled = false;
			}
			cell.planned = cell.changed = false;
			// cell.selected set by checkCell

			Entry cell_entry,entry;
			entry.schedule_id = cell.entry.schedule_id;
			entry.schedule_name = cell.entry.schedule_name;
			while(next()) {
				entry.get(*this);
				if(drag && drag->entry.app_id == entry.app_id &&
					drag->entry == entry) {continue;}
				// If an identical entry is found use that for the hash. If not
				// use the first overlapping one. Changes are detected by reHash
				if((!cell.planned && cell.entry.cancelled == entry.cancelled) ||
					(cell.entry.app_id == entry.app_id && cell.entry == entry)){
					// if eq, set planned to show cells someone else cancelled
					cell.planned = true;
					cell_entry = entry;
					cell.data.get(*this);
				}
			} if(cell.planned) {cell.entry = cell_entry;}
		}
};
//---------------------------------------------------------------------------
#endif

