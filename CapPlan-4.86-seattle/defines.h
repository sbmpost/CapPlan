//$Id: defines.h,v 1.117 2013/12/21 20:00:16 darseq Exp $
#ifndef DEFINES_H
#define DEFINES_H
typedef AnsiString id_t;

#if __BORLANDC__ <= 0x582
typedef AnsiString UnicodeString;
#else
#define DateSeparator FormatSettings.DateSeparator
#define TimeSeparator FormatSettings.TimeSeparator
#define ShortDateFormat FormatSettings.ShortDateFormat
#define LongDateFormat FormatSettings.LongDateFormat
#define LongTimeFormat FormatSettings.LongTimeFormat
#define DecimalSeparator FormatSettings.DecimalSeparator
#define ThousandSeparator FormatSettings.ThousandSeparator
#define TwoDigitYearCenturyWindow FormatSettings.TwoDigitYearCenturyWindow
#endif

#include "planner.rh"
#define MAXPOSTCODE 12  		// personal.sql
#define MAXLASTNAME 30  		// personal.sql
#define MAXADDRESS 60   		// personal.sql
#define MAXCITY 30      		// personal.sql
#define MAXTELEPHONE 16 		// personal.sql
#define MAXSCHNAME 16			// schedules.sql
#define MAXDOCNAME 60			// docs.sql
#define MAXCOMMENT MAXDOCNAME   // docs.sql
#define MAXCODE 16      		// codes.sql
#ifdef ACCESS
#define MAXTEXT 256
#else
#define MAXTEXT 1024
#endif

// #define IOBUFFER (1024*16) // 16kb buffer, should be divisible by 4
#define CONFLICTCOLOR 0x3D3DF0
#define SELECTCOLOR clAqua
#define SELECTBLOCKEDCOLOR SELECTCOLOR // 0xFFC820
#define BLOCKEDCOLOR 0xA56E3A
#define TIMEPANELCOLOR (clBlue + 0x009070)
#define WEEK1COLOR (clGreen + 0x604060)
#define WEEK2COLOR 0xB0958F
#define ROWLINECOLOR 0xDAC09B
#define PERSONSLIMIT 100 // howmany persons can be displayed at max
#define CREATEDDAYS 100 // created records that can be displayed max
#define ROWSET_SIZE 100 // amount of memory to reserve for many rows

// SLOCKS and ULOCKS are compatible. SLOCKS and XLOCKS are not. SLOCKS and
// timeout = 0 is not an option. When updating data and someone else has a
// SLOCK on it, the update fails and the entire transaction must be rolled
// back. So we use ULOCKS whenever timeout = 0.
#ifdef MS_SQL
#define DS "'"
#define LAST_ID "SCOPE_IDENTITY()"
#define ISNULL(value,replace)\
"isnull(" value "," replace ")"
#define ISNULL2(value1,value2,replace)\
"isnull(" value1 ",isnull(" value2 "," replace "))"
#define IFELSE(expr,value1,value2)\
"case when " expr " then " value1 " else " value2 " end"
#define CURRENT_TIMESTAMP "CURRENT_TIMESTAMP"
#define UTC_TIMESTAMP "GETUTCDATE()"
#define SLOCK "with (serializable)"
#define ULOCK "with (serializable,updlock)"
#define DEADLOCK 1205		// "40001"
#define LOCK_TIMEOUT 1222	// "42000"
#define DUPLICATEKEY 2627   // "23000"
#define DUPLICATEUNQ 2601   // "23000"
#define SERVER_GONE 10054   // "08S01"
#define SERVER_LOST 17824   // "08S01" ?
#define SERVER_DOWN 6005	// "42000"
#define ODBC_GONE_LOST "08S01" // Fbase.h: if db->rollback() fails
#define ACCESS_DENIED1 229	// "42000" = conflict with LOCK_TIMEOUT
#define ACCESS_DENIED2 230	// permission denied on column

#elif defined INNODB
#define DS "'"
#define LAST_ID "LAST_INSERT_ID()"
#define ISNULL(value,replace)\
"ifnull(" value "," replace ")"
#define ISNULL2(value1,value2,replace)\
"ifnull(" value1 ",ifnull(" value2 "," replace "))"
#define IFELSE(expr,value1,value2)\
"case when " expr " then " value1 " else " value2 " end"
#define CURRENT_TIMESTAMP "CURRENT_TIMESTAMP"
#define UTC_TIMESTAMP "UTC_TIMESTAMP" // needs mysql >= 4.1.1
// #define UTC_TIMESTAMP "CURRENT_TIMESTAMP"
#define SLOCK "lock in share mode"
#define ULOCK "for update"  // only XLOCK's in INNODB database
#define DUPLICATEKEY 1062	// mysql duplicate key error code
#define DUPLICATEUNQ 1169   // mysql duplicate unique index
#define DEADLOCK 1213		// mysql deadlock error code
#define LOCK_TIMEOUT 1205	// mysql lock timeout error code
#define SERVER_GONE 2006	// mysql server has gone away code
#define SERVER_LOST 2013	// mysql connection lost during query
#define SERVER_DOWN 1053	// mysql server shutdown in progress
#define ODBC_GONE_LOST ""	// todo: dummy, not sure if necessary?
#define ACCESS_DENIED1 1044	// not enough privileges error code
#define ACCESS_DENIED2 1227	// need %s privilege for this operation

#elif defined ACCESS
#define DS "#" // weird stuff
#define LAST_ID "@@IDENTITY"
#define ISNULL(value,replace)\
"IIf(isnull(" value ")," replace "," value ")"
#define ISNULL2(value1,value2,replace)\
"IIf(isnull(" value1 "),IIf(isnull(" value2 \
")," replace "," value2 ")," value1 ")"
#define IFELSE(expr,value1,value2)\
"IIf(" expr "," value1 "," value2 ")"
#define CURRENT_TIMESTAMP "Now()"
#define UTC_TIMESTAMP "Now()"
#define LOCK_TIMEOUT -1102
#define DUPLICATEKEY -1605
#define DUPLICATEUNQ -1605
// #define ACCESS_DENIED1 229
// #define ACCESS_DENIED2 230
#endif

#define INTEGRITY_VIOLATION "23000" // integrity contraint violation
#define ACCURACY (24*60) // accuracy to the minute, same should hold for db
#define TWODIGITYEARCENTURYWINDOW 50
#define DAYS_IN_WEEK 7
#define WEEKS_IN_MONTH 4
#define ZERODATE "1899-12-30" // TDateTime starts counting from this date
#define MINYEAR 2000 // TDateTime > 0 <-> 1899, > changeover gregorian calendar
#define MAXYEAR 2999 // prevents values greater than a TRect/unit int (2^32)
#define HASHSIZE 257 // should be prime
#define PRODUCT "CapPlan"
#define POSTWAREMAIL "postware@gmail.com"
#define DOCUMENTPATH "documents" // created by the installer
#define TEMPLATEPATH "templates" // template folder name
#define SCHEDULEPATH "schedule" // documents that are scheduled are moved here

// Open two documents and minimize CapPlan. Then close the
// first document and its Fdetails form. Because the form is
// destroyed and CapPlan is minimized, Screen->ActiveForm is
// now NULL. After closing the second document, following up
// code cannot use Screen->ActiveForm at that moment.
#define GetActiveHandle() (Screen->ActiveForm ? \
Screen->ActiveForm->Handle : Application->Handle)

// use MessageBox because vcl MessageDlg is flawed:
// - owner window cannot be set so it may be hidden
// - the button positions and default cannot be set
// - it is difficult to support multiple languages

#define MSGO(mymesg) \
(MessageBox(GetActiveHandle(),mymesg,PRODUCT,\
MB_OK | MB_ICONINFORMATION))

#define MSGYN(mymesg) \
(MessageBox(GetActiveHandle(),mymesg,PRODUCT,\
MB_YESNO | MB_ICONQUESTION))

#define ATTO(mymesg) \
(MessageBox(GetActiveHandle(),mymesg,PRODUCT,\
MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND))

#define ATTYN(mymesg) \
(MessageBox(GetActiveHandle(),mymesg,PRODUCT,\
MB_YESNO | MB_ICONEXCLAMATION | MB_SETFOREGROUND))

#define ATTYNC(mymesg) \
(MessageBox(GetActiveHandle(),mymesg,PRODUCT,\
MB_YESNOCANCEL | MB_ICONEXCLAMATION | MB_SETFOREGROUND))

#define MSGEXC(mymesg) \
(MessageBox(GetActiveHandle(),mymesg,PRODUCT,\
MB_OK | MB_ICONSTOP | MB_SETFOREGROUND))

class Policy {
	public:
		static AnsiString lock,lock_end;
		static void setULock(void) {
#ifdef MS_SQL
			lock = ULOCK " ";
#elif defined INNODB
			lock_end = " " ULOCK;
#endif
		}
		static void setSLock(void) {
#ifdef MS_SQL
			lock = SLOCK " ";
#elif defined INNODB
			lock_end = " " SLOCK;
#endif
		}
		static void clrLock(void) {lock = lock_end = "";}
};
#endif //DEFINES_H

