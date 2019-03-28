//$Id: finish.cpp,v 1.48 2013/12/22 15:36:43 darseq Exp $
#define NO_WIN32_LEAN_AND_MEAN // required for including IShellLink
#include <winsock2.h> // prevent winsock.h from being included
#include <vcl.h>
#include <comobj.hpp>
#include <registry.hpp>
#pragma hdrstop

#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
// #include <iostream.h>
#include <fstream>
#include <dirent.h>
#include <dir.h>
#include <string>
#include "finish.h"
#include "download.h"
#include "config.h"

#include "query.h"
#include "odbc.h"
#include "../include/Odbcinst.h"
#define MAXQUERY 1024

#ifdef MS_SQL
#define DBEXISTS 1801
#define LOGINEXISTS 15025
#define UEXISTS 15023
#define CREATE_DENIED 262
#define TBLEXISTS1 2714
#define TBLEXISTS2 2714
#define TBLEXISTS3 2714
#define DUPLICATEINDEX 1913
#elif defined INNODB
#define DBEXISTS 1007
#define TBLEXISTS1 1050
#define TBLEXISTS2 1005
#define TBLEXISTS3 1022
#define LOGIN_DENIED 1045
#define DUPLICATEINDEX 1061
#define DUPLICATEFOREIGNKEY 1826
#endif

#define IOBUFFER 256 // should be divisible by 4
#define MAXRECURSE 32 // maximum recursion depth
#define PERROR() (throw Exception(String("Error!: ")+strerror(errno)))
//#define PERROR()
//	(throw Exception(String("Error!: ")+strerror(errno)+
//	" at "+__FILE__+" line "+__LINE__+"."))
void cpEntry(AnsiString path,AnsiString tail,
	bool path_is_file,int depth,void * data);
void rmEntry(AnsiString path,AnsiString tail,
	bool path_is_file,int depth,void * data);
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "setup"
#pragma resource "*.dfm"
#ifdef _WIN64
#define NO_STRICT
#pragma link "odbccp32.a"
#undef NO_STRICT
#else
#pragma link "odbccp32.lib"
#endif
TSetupFinish *SetupFinish;
//---------------------------------------------------------------------------

__fastcall TSetupFinish::TSetupFinish(TComponent* Owner) : TSetupBase(Owner) {
	if(Application->Tag == false) {Next->Caption = "Install";}
	else {Next->Caption = "Uninstall"; Back->Enabled = false;}
	getFiles(ExtractFileDir(Application->ExeName)+"\\filelist.txt",files);
	ProgressBar1->Max = files.size()-1+1; registry = new TRegistry();
}
//---------------------------------------------------------------------------

void __fastcall TSetupFinish::getFiles(AnsiString filename,
	list<entry_info_t> & files) {

	ifstream fs_read(filename.c_str());
	if(fs_read.fail()) {
		throw Exception("Error!: could not open file: "+filename);
	}

	string line;
	while(getline(fs_read,line,'\n')) {
		unsigned int i=0; entry_info_t e;
		if(line.length() && line[0] == '#') {continue;}
		for(;i<line.length() && line[i]!=';';i++) {e.name += line[i];} i++;
		for(;i<line.length() && line[i]!=';';i++) {e.root += line[i];} i++;
		for(;i<line.length() && line[i]!=';';i++) {e.base += line[i];} i++;
		if(line.length()) {
			if(i+1 != line.length()) {
				throw Exception("Error!: invalid format file: "+filename);
			}
			e.patch = (line[i] == '1');
			files.push_back(e);
		}
	}
	fs_read.clear();
	fs_read.close();
	if(fs_read.fail()) {
		throw Exception("Error!: could not close file: "+filename);
	}
}
//---------------------------------------------------------------------------

TModalResult __fastcall TSetupFinish::install_database(void) {
	AnsiString hostname = SetupConfig->DATABASEHOST->Text;
#ifdef MS_SQL
	if(hostname.LowerCase() == "localhost") {
		Status->Caption = "Enabling remote connections";
		Application->HandleMessage();

		AnsiString instance;
		registry->Access = KEY_READ;
		registry->RootKey = HKEY_LOCAL_MACHINE;
#ifdef _WIN64
		current_key = "\\SOFTWARE\\Microsoft\\Microsoft SQL Server\\MSSQL10.SQLEXPRESS";
#else
		current_key = "\\SOFTWARE\\Microsoft\\Microsoft SQL Server\\MSSQL.1";
#endif
		if(registry->OpenKey(current_key,false)) {
			instance = registry->ReadString(""); registry->CloseKey();
		} else {throw Exception("Error!: unable to find SQL instance");}

		// enable tcp remote access
		registry->Access = KEY_WRITE;
		current_key += "\\MSSQLServer\\SuperSocketNetLib\\Tcp";
		if(registry->OpenKey(current_key,false)) {
			registry->WriteInteger("Enabled",1); registry->CloseKey();
		} else {throw Exception("Error!: unable to write the registry");}

		// database should listen on port 1433
		registry->Access = KEY_WRITE;
		current_key += "\\IPAll";
		if(registry->OpenKey(current_key,false)) {
			registry->WriteString("TcpPort","1433"); registry->CloseKey();
		} else {throw Exception("Error!: unable to write the registry");}

		Status->Caption = "Restarting SQL server...";
		Application->HandleMessage();
		stopService("","MSSQL$"+instance); // "" = localhost.
		if(startService("","MSSQL$"+instance) == false) {
			throw Exception("Could not start service: "+instance);
		}
		Sleep(5000); // make sure service is really up and running
	}
#endif

	Status->Caption = "Creating user DSN";
	Application->HandleMessage();
	AnsiString options;
#ifdef MS_SQL
	options += "DSN=MSSQLsource;";

// trusted connection disabled: if the user wants to override the connection
// method, he can select the 'trusted connection' checkbox in the sql server
// login dialog. On the other hand, if we would configure the DSN to always
// use a trusted connection, the user cannot override the connection method
//#ifdef SA_CONNECTION
	options += "Trusted_Connection=no;";
//#else
//	options += "Trusted_Connection=yes;";
//#endif
//	options += "MARS_Connection=yes;";
//	options += "AnsiNPW=no;";
#elif defined INNODB
	options += "DSN=MYSQLsource;";
	options += "UID=root;";
#endif
	options += "DATABASE=" PRODUCT ";";
	options += "SERVER="+hostname+";"; // end with ';' -> replace with '\0'

	char * str = options.c_str();
	for(int i=0;i < options.Length();i++) {if(str[i] == ';') {str[i] = '\0';}}
	if(SQLConfigDataSource(NULL,ODBC_ADD_DSN,SetupDownload->odbc_driver.c_str(),
		str) == false) {throw Exception("Error!: unable to create user DSN");}

	Status->Caption = "Connecting server using host: "+hostname;
	Application->HandleMessage();
	ODBCparams params; params.hwnd = Handle; Odbc odbc(params);

	AnsiString err;
	try {
#ifdef MS_SQL
#ifdef SA_CONNECTION
		odbc.connect("DSN=MSSQLsource;UID=sa;DATABASE=master");
#else
		odbc.connect("DSN=MSSQLsource;DATABASE=");
#endif
#elif defined INNODB
		odbc.connect("DSN=MYSQLsource;UID=root;DATABASE=mysql");
#endif
	} catch(DBException & e) {err = e.Message;}
	if(err.IsEmpty() == false || odbc.isConnected() == false) {
		AnsiString msg = "Could not connect to the database server. To create "
			"the database and to setup your\ndatabase account, this installer "
			"needs server access. You can only ignore this step if\nyou are "
			"absolutely sure that the database was already created and you "
			"already have\naccess. Note: It is safe to recreate the database "
			"because existing data is preserved.";
		if(err.IsEmpty() == false) {msg += "\n\nThe error message was:\n"+err;}
		int ret_val = MessageDlg(msg,mtWarning,
			TMsgDlgButtons()<<mbCancel<<mbRetry<<mbIgnore,0);
		if(ret_val == mrCancel) {Abort();}
		else {return ret_val;}
	}

#ifdef MS_SQL
	// the default database is somehow reset when connection fails and
	// the second connection screen pops up, and then connection is ok
	if(SQLConfigDataSource(NULL,ODBC_CONFIG_DSN,
		SetupDownload->odbc_driver.c_str(),str) == false) {
		throw Exception("Error!: unable to modify user DSN");
	}
#endif

	Status->Caption = "Connected!, initializing the database";
	Application->HandleMessage();
	Query query; odbc.link(&query);

	//--------------------------create database------------------------------
#if defined MS_SQL && !defined SA_CONNECTION
	try {
#endif
	try {query.execSQL("create database " PRODUCT);}
	catch(DBException & e) {
		if(e.errors.size() == 0 || e.errors[0].native_err != DBEXISTS) {
			throw;
		}
	}
#if defined MS_SQL && !defined SA_CONNECTION
	} catch(DBException & e) {
		if(e.errors.size() && e.errors[0].native_err == CREATE_DENIED) {
			int ret_val =
				MessageDlg("Could not create the database because your windows "
				"account does not have enough\ndatabase privileges. Please "
				"check these privileges on the server. You can only ignore\n"
				"this step if you are absolutely sure that the database was "
				"already created and you have\naccess. Note: It is safe to "
				"recreate the database because existing data is preserved.",
				mtWarning,TMsgDlgButtons()<<mbCancel<<mbRetry<<mbIgnore,0);
			if(ret_val == mrCancel) {Abort();}
			else {return ret_val;}
		} else {throw;}
	}
#endif

#ifdef MS_SQL
	query.execSQL("alter database "PRODUCT" set READ_COMMITTED_SNAPSHOT ON");
	query.execSQL("alter database "PRODUCT" set AUTO_CLOSE OFF");
	query.execSQL("alter database "PRODUCT" set RECOVERY FULL");
	// the user is responsible for running (scheduling) backup scripts
#endif
	query.execSQL("use " PRODUCT);

	//--------------------------create tables--------------------------------
	AnsiString exedir = ExtractFileDir(Application->ExeName);
	TSearchRec s; unsigned char string[MAXQUERY];
#ifdef MS_SQL
	bool error = FindFirst(exedir+"\\database\\mssql\\*.sql",faAnyFile,s);
#elif defined INNODB
	bool error = FindFirst(exedir+"\\database\\mysql\\*.sql",faAnyFile,s);
#endif
	if(error) {throw Exception("Error!: unable to open sql files");}
	while(error == false) {
#ifdef MS_SQL
		FILE * file = fopen((exedir+"\\database\\mssql\\"+s.Name).c_str(),"rb");
#elif defined INNODB
		FILE * file = fopen((exedir+"\\database\\mysql\\"+s.Name).c_str(),"rb");
#endif
		if(file == NULL) {throw Exception("Error!: unable to open sql files");}
		fseek(file,0,SEEK_END); int length = ftell(file);
		fseek(file,0,SEEK_SET); fread(string,MAXQUERY,1,file);

		int check1 = TBLEXISTS1;
		int check2 = TBLEXISTS2;
		int check3 = TBLEXISTS3;
		AnsiString cmd = "";
		for(int i=0;i < length;i++) {
			if(string[i] == ';') {
				if(s.Name == "z_foreign.sql") {
					try {query.execSQL(cmd);}
					catch(DBException & e) {
						if(e.errors.size() == 0 ||
							(e.errors[0].native_err != DUPLICATEFOREIGNKEY &&
							e.errors[0].native_err != DUPLICATEINDEX)) {
							throw;
						}
					}
				} else {
					try {query.execSQL(cmd);}
					catch(DBException & e) {
						if(e.errors.size() == 0 ||
							(e.errors[0].native_err != check1 &&
							e.errors[0].native_err != check2 &&
							e.errors[0].native_err != check3)) {
							throw;
						}
					}
					check1 = check2 = check3 = DUPLICATEINDEX;
				}
				cmd = "";
			} else {cmd += (char) string[i];}
		}
		error = FindNext(s);
	} FindClose(s);

	odbc.disconnect();
	return mrOk;
}
//---------------------------------------------------------------------------

/*
#ifdef MS_SQL
#ifndef TRUSTED_CONNECTION
	// MUST_CHANGE server setting only supported by XP professional
	TRYQUERY(query.execSQL("create login "PRODUCT" with password = 'postware'"),
		LOGINEXISTS);// "MUST_CHANGE,CHECK_POLICY = ON, CHECK_EXPIRATION = ON");
	TRYQUERY(query.execSQL("create user "PRODUCT" for login "PRODUCT),UEXISTS);
	query.execSQL("exec sp_addrolemember 'db_datareader','"PRODUCT"'");
	query.execSQL("exec sp_addrolemember 'db_datawriter','"PRODUCT"'");
#endif
#elif defined INNODB
	TRYQUERY(query.execSQL("insert into user (Host,User) values("
			"SUBSTRING_INDEX(USER(),'@',-1),'"PRODUCT"')"),DUPLICATEKEY);
	TRYQUERY(query.execSQL("insert into db (Host,Db,User,Select_priv,"
			"Insert_priv,Update_priv,Delete_priv) values("
			"SUBSTRING_INDEX(USER(),'@',-1),'"PRODUCT"','"PRODUCT"',"
			"'Y','Y','Y','Y')"),DUPLICATEKEY);
	query.execSQL("flush privileges");
#endif
*/

void __fastcall TSetupFinish::install_files(void) {
	// generate xor key from username and password
	int xor_key = 0; // username = payer_id: length > 0, password length = 8
	// [xor_key should remain 0 in the demo version]
#ifdef DEMO
	AnsiString username = PRODUCT;
	AnsiString password = "demo";
#else
	AnsiString username = SetupConfig->USERNAME->Text;
	AnsiString password = SetupConfig->PASSWORD->Text;
#endif
	for(int i=1;i <= password.Length();i++) {
		int digit = password[i];
		if(i <= username.Length()) {digit += username[i];}
		xor_key |= digit << ((i-1) * 4); // 8*4 = 32 bits = int
	}

	// start copying files using filelist.txt
	AnsiString exedir = ExtractFileDir(Application->ExeName);
	list<entry_info_t>::const_iterator it = files.begin();
	for(int i=0;it != files.end();it++,i++) {
		ProgressBar1->Position = i;
		AnsiString to = it->root;
		if(to.LowerCase() == "installroot") {
			to = SetupConfig->INSTALLROOT->Text;
//		} else if(to.LowerCase() == "publicdata") {
//			to = getenv("PUBLIC");
//			if(to.IsEmpty()) {to = getPath("ALLUSERSPROFILE");}
//			to = to+"\\"PRODUCT;
//		} else if(to.LowerCase() == "userprofile") {
//			to = getPath("USERPROFILE")+"\\"PRODUCT;
		} else if(to.LowerCase() == "noinstall") {continue;}
		to += "\\"+it->base; // \foo\bar = \foo\\bar
		if(DirectoryExists(to) == false) {
			Status->Caption = "Creating directory: "+to;
			Application->HandleMessage(); // update Status label
			if(ForceDirectories(to) == false) {
				throw Exception("Error!: could not create directory");
			}
		}

		// cfg hack: new config file is not copied if old one exists
		AnsiString filename = ExtractFileName(it->name);
		if(filename.LowerCase() == LowerCase(PRODUCT".cfg") &&
			FileExists(to+"\\" PRODUCT ".cfg")) {continue;}

		// Copying <-> the Status label may also indicate a directory
		Status->Caption = "Copying: "+to+"\\"+filename;
		Application->HandleMessage(); // update Status label

		entry_copy_t entry_copy;
		entry_copy.to = to; entry_copy.xor_key = 0;
		if(it->patch) {entry_copy.xor_key = xor_key;}
		recurse(exedir+"\\"+it->name,"",cpEntry,0,true,(void *)&entry_copy);
	}

	// cfg hack: copy config files of old installation to new installation
	registry->Access = KEY_READ;
	registry->RootKey = HKEY_CURRENT_USER;
	current_key = "\\Software\\PostWare\\" PRODUCT;
	if(registry->OpenKey(current_key,false)) {
		AnsiString from = registry->ReadString("installroot");
		registry->CloseKey();
		AnsiString to = SetupConfig->INSTALLROOT->Text;
		if(from.LowerCase() != to.LowerCase()) {
			entry_copy_t entry_copy;
			entry_copy.to = to; entry_copy.xor_key = 0;
			AnsiString cfg = PRODUCT".cfg";
			if(FileExists(from+"\\"+cfg)) {
				Status->Caption = "Overwrite: "+to+"\\"+cfg;
				Application->HandleMessage(); // update Status label
				cpEntry(from+"\\"+cfg,"",true,0,&entry_copy);
			}
			TSearchRec s;
			bool error = FindFirst(from+"\\" PRODUCT ".*",faAnyFile,s);
			while(error == false) {
				try {
					AnsiString ext = ExtractFileExt(s.Name);
					StrToInt(ext.SubString(2,ext.Length()-1));
					Status->Caption = "Copying: "+to+"\\"+s.Name;
					Application->HandleMessage(); // update Status label
					cpEntry(from+"\\"+s.Name,"",true,0,&entry_copy);
				} catch (EConvertError & e) {}
				error = FindNext(s);
			} FindClose(s);
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TSetupFinish::install_links(void) {
	Status->Caption = "Creating start menu/desktop links";
	Application->HandleMessage();

	IShellLink * shell_link;
	if(CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,
		IID_IShellLink,(void **) &shell_link) && shell_link == NULL) {
		throw Exception("Error!: CoCreateInstance failure");
	}

	IPersistFile * persist_file;
	if(shell_link->QueryInterface(IID_IPersistFile,
		(void **) &persist_file) && persist_file == NULL) {
		throw Exception("Error!: QueryInterface failure");
	}

	// can also set args/workdir/icon
	shell_link->SetDescription(PRODUCT" by PostWare");
	AnsiString command(SetupConfig->INSTALLROOT->Text+"\\" PRODUCT ".exe");
	shell_link->SetPath(command.c_str());

	registry->Access = KEY_READ;
	registry->RootKey = HKEY_CURRENT_USER;
	current_key = "Software\\MicroSoft\\Windows\\"
		"CurrentVersion\\Explorer\\Shell Folders";
	if(registry->OpenKey(current_key,false)) {
		AnsiString programs = registry->ReadString("Programs");
		AnsiString desktop = registry->ReadString("DeskTop");
		registry->CloseKey();

		AnsiString folder(programs+"\\PostWare");
		if(DirectoryExists(folder) == false && CreateDir(folder) == false) {
			throw Exception("Error!: unable to create directory: "+folder);
		}
		programlink = folder+"\\"+PRODUCT "-" VERSION+".lnk";
		if(persist_file->Save(WideString(programlink).c_bstr(),false) == E_FAIL) {
			throw Exception("Error!: unable to create link: "+programlink);
		}
		desktoplink = desktop+"\\"+PRODUCT "-" VERSION+".lnk";
		if(persist_file->Save(WideString(desktoplink).c_bstr(),false) == E_FAIL) {
			throw Exception("Error!: unable to create link: "+desktoplink);
		}
	} else {throw Exception("Error!: unable to read the registry");}
	persist_file->Release(); shell_link->Release();
}
//---------------------------------------------------------------------------

void __fastcall TSetupFinish::install_registry(void) {
	Status->Caption = "Writing registry values";
	Application->HandleMessage();

	// generate uninstall information
	registry->Access = KEY_WRITE;
	registry->RootKey = HKEY_CURRENT_USER;
	current_key = "\\Software\\PostWare\\" PRODUCT;
	if(registry->OpenKey(current_key,true)) {
		registry->WriteString("version",Application->Title);
		registry->WriteString("databasehost",SetupConfig->DATABASEHOST->Text);
		registry->WriteString("installroot",SetupConfig->INSTALLROOT->Text);
		registry->WriteString("programlink",programlink);
		registry->WriteString("desktoplink",desktoplink);
		registry->CloseKey();
	} else {throw Exception("Error!: unable to write the registry");}

	// add product to the software list
	registry->Access = KEY_WRITE;
	registry->RootKey = HKEY_LOCAL_MACHINE;
	current_key = "\\SOFTWARE\\Microsoft\\Windows\\"
		"CurrentVersion\\Uninstall\\" PRODUCT;
	if(registry->OpenKey(current_key,true)) {
		try {
			registry->WriteString("DisplayName",Application->Title);
			registry->WriteString("DisplayIcon",
				SetupConfig->INSTALLROOT->Text+"\\" PRODUCT ".ico");
			registry->WriteString("UninstallString",
				SetupConfig->INSTALLROOT->Text+"\\"+
				ExtractFileName(Application->ExeName));
		} catch(Exception & e) {
			// if the key already existed but we do not
			// have write access on LOCAL_MACHINE anymore
		}
		registry->CloseKey();
	} else {/*throw Exception("Error!: unable to access the registry");*/}
	// limited user account support
}
//---------------------------------------------------------------------------

void __fastcall TSetupFinish::install_specific(void) {
}
//---------------------------------------------------------------------------

void __fastcall TSetupFinish::uninstall_files(void) {
	list<entry_info_t>::const_iterator it = files.begin();
	for(int i=0;it != files.end();it++,i++) {
		ProgressBar1->Position = i;
		if(LowerCase(it->name) == LowerCase(
			ExtractFileName(Application->ExeName))) {continue;}
//	cannot delete install.exe ->

		AnsiString to = it->root;
		if(to.LowerCase() == "installroot") {
			to = installroot;
//		} else if(to.LowerCase() == "publicdata") {
//			to = getenv("PUBLIC");
//			if(to.IsEmpty()) {to = getPath("ALLUSERSPROFILE");}
//			to = to+"\\"PRODUCT;
//		} else if(to.LowerCase() == "userprofile") {
//			to = getPath("USERPROFILE")+"\\"PRODUCT;
		} else if(to.LowerCase() == "noinstall") {continue;}
		to += "\\"+it->base+"\\"+ExtractFileName(it->name);

		// Deleting <-> the Status label may also indicate a directory
		// recurse only removes it->name directories and keeps bcb6 dirs
		Status->Caption = "Deleting: "+to;
		Application->HandleMessage(); // update Status label
		try {recurse(to,"",rmEntry,0,false,NULL);}
		catch(Exception & e) {if(errno != ENOENT) {throw;}}
	}
//	cannot delete install.exe ->
//	Status->Caption = "Deleting: "+installroot;
//	rmEntry(installroot,"",false,0,NULL); // do remove installroot dir
}
//---------------------------------------------------------------------------

void __fastcall TSetupFinish::uninstall_links(void) {
	Status->Caption = "Deleting start menu/desktop links";
	Application->HandleMessage();

	try {recurse(programlink,"",rmEntry,0,false,NULL);}
	catch(Exception & e) {if(errno != ENOENT) {throw;}}
	try {recurse(desktoplink,"",rmEntry,0,false,NULL);}
	catch(Exception & e) {if(errno != ENOENT) {throw;}}
}
//---------------------------------------------------------------------------

void __fastcall TSetupFinish::uninstall_registry(void) {
	Status->Caption = "Writing registry values";
	Application->HandleMessage();

	if(registry->DeleteKey(current_key) == false) {
		throw Exception("Error!: unable to write the registry");
	}
	registry->RootKey = HKEY_LOCAL_MACHINE;
	current_key = "\\SOFTWARE\\Microsoft\\Windows\\"
		"CurrentVersion\\Uninstall\\" PRODUCT;
	if(registry->DeleteKey(current_key) == false) {
		// throw Exception("Error!: unable to write the registry");
		// limited user account support
	}
}
//---------------------------------------------------------------------------

void __fastcall TSetupFinish::uninstall_specific(void) {
}
//---------------------------------------------------------------------------

void __fastcall TSetupFinish::uninstall_database(void) {
	// note that we keep DSN as well
}
//---------------------------------------------------------------------------

void __fastcall TSetupFinish::NextClick(TObject *Sender) {
	if(Next->Caption == "Finish") {Close(); return;}

	Status->Caption = "Reading registry values";
	Application->HandleMessage();
	bool have_information = false;
	registry->Access = KEY_READ;
	registry->RootKey = HKEY_CURRENT_USER;
	current_key = "\\Software\\PostWare\\" PRODUCT;
	if(registry->OpenKey(current_key,false)) {
		databasehost = registry->ReadString("databasehost");
		installroot = registry->ReadString("installroot");
		programlink = registry->ReadString("programlink");
		desktoplink = registry->ReadString("desktoplink");
		have_information = true;
		registry->CloseKey();
	}

	Next->Enabled = false;
	try {
		if(Next->Caption == "Install") {
			while(install_database() == mrRetry);
			install_files();
			uninstall_links(); // uses registry info
			install_links();
			install_registry(); // uses links
			install_specific();
			Status->Caption = "Install complete";
		} else if(Next->Caption == "Uninstall") {
			if(have_information == false) {
				throw Exception("Error!: unable to access the registry");
			}
			uninstall_files();
			uninstall_links();
			uninstall_registry(); // uses current_key
			uninstall_specific();
			uninstall_database();
			Status->Caption = "Uninstall complete";
		}
	} __finally {Next->Enabled = true; ActiveControl = Next;}

	Next->Caption = "Finish";
	ProgressBar1->Position = ProgressBar1->Max;
	Back->Enabled = false; Cancel->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TSetupFinish::recurse(AnsiString path,AnsiString tail,
	do_entry_t doEntry,int depth,bool act_first,void * data) {
	if(depth == MAXRECURSE) {
		throw Exception("reached maximum recursion depth at: "+path);
	}

	DIR * dir_stream;
	if((dir_stream = opendir(path.c_str())) != NULL) {
		if(!depth) {tail += "\\"+ExtractFileName(path);}
		if(act_first) {doEntry(path,tail,false,depth,data);}
		struct dirent * entry;
		while((entry = readdir(dir_stream)) != NULL) {
			if(strcmp(entry->d_name,".") && strcmp(entry->d_name,"..")) {
				recurse(path+"\\"+entry->d_name,tail+"\\"+entry->d_name,
					doEntry,depth+1,act_first,data);
			}
		}
		if(closedir(dir_stream) == -1) {PERROR();}
		if(act_first == false) {doEntry(path,tail,false,depth,data);}
	} else if(errno == EINVAL /*ENOTDIR*/ || errno == ENOENT /*windows 9x*/) {
		doEntry(path,tail,true,depth,data);
	} else {PERROR();}
}
//---------------------------------------------------------------------------

void cpEntry(AnsiString path,AnsiString tail,
	bool path_is_file,int depth,void * data) {
	struct stat entry_stat;
	if(stat(path.c_str(),&entry_stat) == -1) {PERROR();}
	AnsiString to = ((entry_copy_t *) data)->to+tail;

	if(path_is_file) {
		if(!depth && DirectoryExists(to)) {to += "\\"+ExtractFileName(path);}
		if(path.LowerCase() == to.LowerCase()) {
			throw Exception("Copy entry, cannot copy a file to itself:\n"+path);
		}
		int fh_read = open(path.c_str(),O_RDONLY | O_BINARY);
		if(fh_read == -1) {PERROR();}
		int fh_write = open(to.c_str(),O_CREAT | O_TRUNC |
			O_WRONLY | O_BINARY,entry_stat.st_mode);
		if(fh_write == -1) {PERROR();}

		int xor_key = ((entry_copy_t *) data)->xor_key;
		int bytes; unsigned char buffer[IOBUFFER];
		while((bytes = read(fh_read,buffer,IOBUFFER)) != 0) {
			if(bytes == -1) {PERROR();}
			if(xor_key) {
				// very easy to crack but that is not the point
				// when bytes % 4 > 0, skip the last few bytes
				for(int i=0;i < bytes/4;i++) {
					int * value = ((int *) buffer)+i;
					*value = *value ^ xor_key;
				}
			}
			if(write(fh_write,buffer,bytes) == -1) {PERROR();}
		}

		if(close(fh_read) == -1) {PERROR();}
		if(close(fh_write) == -1) {PERROR();}
	} else if(!DirectoryExists(to) && mkdir(to.c_str()) == -1) {PERROR();}
}
//---------------------------------------------------------------------------

void rmEntry(AnsiString path,AnsiString tail,
	bool path_is_file,int depth,void * data) {
	if(path_is_file) {
		if(unlink(path.c_str()) == -1) {PERROR();}
	} else if(rmdir(path.c_str()) == -1) {PERROR();}
}
//---------------------------------------------------------------------------

void __fastcall TSetupFinish::FormShow(TObject *Sender) {
	if(Next->Caption == "Install") {
		SETTINGS->Lines->Clear();
		SETTINGS->Lines->Add("");
		SETTINGS->Lines->Add(" " PRODUCT " will be installed using the");
		SETTINGS->Lines->Add(" following settings:");
		SETTINGS->Lines->Add("");
		SETTINGS->Lines->Add(" odbc driver:");
		SETTINGS->Lines->Add("   "+SetupDownload->odbc_driver);
		SETTINGS->Lines->Add(" database host:");
		SETTINGS->Lines->Add("   "+SetupConfig->DATABASEHOST->Text);
		SETTINGS->Lines->Add(" " PRODUCT " installation PATH:");
		SETTINGS->Lines->Add("   "+SetupConfig->INSTALLROOT->Text);
		SETTINGS->Lines->Add(" Username:");
		SETTINGS->Lines->Add("   "+SetupConfig->USERNAME->Text);
	} else if(Next->Caption == "Uninstall") {
		SETTINGS->Lines->Clear();
		SETTINGS->Lines->Add("");
		SETTINGS->Lines->Add(" " PRODUCT " will be removed when you click ");
		SETTINGS->Lines->Add(" the Uninstall button.");
		SETTINGS->Lines->Add("");
		SETTINGS->Lines->Add(" Note: the database will not be removed");
	}

	Status->Caption = "Status";
	ProgressBar1->Position = 0;
	TSetupBase::FormShow(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TSetupFinish::FormClose(TObject *Sender, TCloseAction &Action) {
	if(Next->Caption == "Finish") {Application->Terminate();}
	else {TSetupBase::FormClose(Sender,Action);}
}
//---------------------------------------------------------------------------

