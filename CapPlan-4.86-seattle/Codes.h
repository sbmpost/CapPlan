//$Id: Codes.h,v 1.14 2013/02/16 12:18:36 darseq Exp $
#ifndef CodesH
#define CodesH
//---------------------------------------------------------------------------
#include <map>
#include "Query.h"
#include "defines.h"

class Code {
	public:
		AnsiString name,color,app_charge;
		__fastcall Code(void) {app_charge = "0";}
		bool __fastcall operator <(const Code & code) const {
			return name < code.name;
		}
		void __fastcall get(Query & query) {
			name = query.fieldByName("CODE_NAME").toString();
			color = query.fieldByName("CODE_COLOR").toString();
			app_charge = query.fieldByName("APP_CHARGE").toString();
		}
};
//---------------------------------------------------------------------------

typedef map<AnsiString,Code> str2code_t;
typedef void __fastcall (__closure * TGetCodeProc)(const Code & code);
class Codes : public Query {
	private:
		static str2code_t value2code;
	public:
		void __fastcall insert(id_t code_name,int color);
		void __fastcall remove(id_t code_name);
		void __fastcall refresh(void);
		static void __fastcall getCodeValues(TGetCodeProc CodeProc);
		static Code __fastcall color2code(AnsiString color_value);
		static Code __fastcall name2code(AnsiString code_name);
};
//---------------------------------------------------------------------------
#endif

