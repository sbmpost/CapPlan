//$Id: Clients.h,v 1.31 2013/02/16 11:41:37 darseq Exp $
#ifndef ClientsH
#define ClientsH
//---------------------------------------------------------------------------
#include "Query.h"
#include "defines.h"

class Person {
	public:
		id_t id; bool employee;
		AnsiString lastname; // cell data
		AnsiString address,city; // cell data
		AnsiString telephone,telephone2; // printing
		AnsiString postcode; // insertion and printing
		bool hidden; AnsiString created_by,memo;
		TDateTime total_diff; // for employees only
		int entry_count; // for employees only

		__fastcall Person(void) {
			id = UNDEFINED; employee = false;
			lastname = UNDEFINED;
			address = city = UNDEFINED;
			telephone = telephone2 = UNDEFINED;
			postcode = UNDEFINED; hidden = false;
			created_by = memo = UNDEFINED;
			total_diff = entry_count = 0;
		}

		// Telephone numbers are not shown in a cell but are used for
		// printing only. Therefore this information is not compared.
		bool __fastcall operator !=(const Person & person) const {
			return person.id != id ||
				person.lastname != lastname ||
				person.address != address ||
				person.city != city;
		}

		void __fastcall get(Query & query) {
			id = query.fieldByName("PERSONAL_ID").toString();
			employee = query.fieldByName("SCHEDULE_ID").toString() != UNDEFINED;
			lastname = query.fieldByName("LASTNAME").toString();
			address = query.fieldByName("ADDRESS").toString();
			city = query.fieldByName("CITY").toString();
			// invisible telephone number/postcode fields. only for printing
			telephone = query.fieldByName("TELEPHONE").toString();
			telephone2 = query.fieldByName("TELEPHONE2").toString();
			postcode = query.fieldByName("POSTCODE").toString();
			// these fields are not used to display a cell in the grid
			// hidden = StrToInt(query.fieldByName("HIDDEN").toString());
			// created_by = query.fieldByName("CREATED_BY").toString();
		}
};
//---------------------------------------------------------------------------

class Persons : public Query {
	public:
		AnsiString __fastcall getCaption(id_t per_id);
		AnsiString __fastcall getText(id_t per_id,AnsiString field_name);
		void __fastcall check(id_t per_id) {getText(per_id,"PERSONAL_ID");}
		void __fastcall insert(Person & person);
		void __fastcall remove(Person & person);
};
//---------------------------------------------------------------------------
#endif

