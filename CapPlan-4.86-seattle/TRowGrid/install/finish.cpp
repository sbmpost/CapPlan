//$Id: finish.cpp,v 1.15 2008/09/13 12:43:13 darseq Exp $
#define NO_WIN32_LEAN_AND_MEAN // required for including IShellLink
#include <vcl.h>
#include <comobj.hpp>
#include <registry.hpp>
#pragma hdrstop

#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <iostream.h>
#include <fstream.h>
#include <dirent.h>
#include <dir.h>
#include "finish.h"
#include "config.h"
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

void __fastcall TSetupFinish::install_database(void) {
}
//---------------------------------------------------------------------------

void __fastcall TSetupFinish::install_files(void) {
	// generate xor key from username and password
	int xor_key = 0; // username = payer_id: length > 0, password length = 8
	// xor_key should remain 0 in the demo version
	AnsiString username = SetupConfig->USERNAME->Text;
	AnsiString password = SetupConfig->PASSWORD->Text;
	for(int i=1;i <= password.Length();i++) {
		int digit = password[i];
		if(i <= username.Length()) {digit += username[i];}
		xor_key |= digit << ((i-1) * 4); // 8*4 = 32 bits = int
	}

	AnsiString exedir = ExtractFileDir(Application->ExeName);
	list<entry_info_t>::const_iterator it = files.begin();
	for(int i=0;it != files.end();it++,i++) {
		ProgressBar1->Position = i;
		AnsiString to = it->root;
		if(to == "NOINSTALL") {continue;}
		else if(to == "INSTALLROOT") {to = SetupConfig->INSTALLROOT->Text;}
		else if(to == "BCB6ROOT") {to = SetupConfig->BCB6ROOT->Text;}
		to += "\\"+it->base; // \foo\bar = \foo\\bar
		if(DirectoryExists(to) == false) {
			Status->Caption = "Creating directory: "+to;
			Application->HandleMessage(); // update Status label
			if(ForceDirectories(to) == false) {
				throw Exception("Error!: could not create directory");
			}
		}

		// Copying <-> the Status label may also indicate a directory
		Status->Caption = "Copying: "+to+"\\"+ExtractFileName(it->name);
		Application->HandleMessage(); // update Status label

		entry_copy_t entry_copy;
		entry_copy.to = to; entry_copy.xor_key = 0;
		if(it->patch) {entry_copy.xor_key = xor_key;}
		recurse(exedir+"\\"+it->name,"",cpEntry,0,true,(void *)&entry_copy);
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
	AnsiString command(SetupConfig->INSTALLROOT->Text+"\\test\\mygrid.exe");
	shell_link->SetPath(command.c_str());

	registry->RootKey = HKEY_CURRENT_USER;
	AnsiString current_key =
		"Software\\MicroSoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders";
	if(registry->OpenKey(current_key,false)) {
		AnsiString programs = registry->ReadString("Programs");
		AnsiString desktop = registry->ReadString("DeskTop");
		registry->CloseKey();

		AnsiString folder(programs+"\\PostWare");
		if(DirectoryExists(folder) == false && CreateDir(folder) == false) {
			throw Exception("Error!: unable to create directory: "+folder);
		}
		programlink = folder+"\\"+Application->Title+".lnk";
		if(persist_file->Save(WideString(programlink),false) == E_FAIL) {
			throw Exception("Error!: unable to create link: "+programlink);
		}
		desktoplink = desktop+"\\"+Application->Title+".lnk";
		if(persist_file->Save(WideString(desktoplink),false) == E_FAIL) {
			throw Exception("Error!: unable to create link: "+desktoplink);
		}
	} else {throw Exception("Error!: unable to access the registry");}
	persist_file->Release(); shell_link->Release();
}
//---------------------------------------------------------------------------

void __fastcall TSetupFinish::install_registry(void) {
	Status->Caption = "Writing registry values";
	Application->HandleMessage();

	// generate uninstall information
	registry->RootKey = HKEY_CURRENT_USER;
	current_key = "\\Software\\PostWare\\"PRODUCT;
	if(registry->OpenKey(current_key,true)) {
		registry->WriteString("version",Application->Title);
		registry->WriteString("bcb6root",SetupConfig->BCB6ROOT->Text);
		registry->WriteString("installroot",SetupConfig->INSTALLROOT->Text);
		registry->WriteString("programlink",programlink);
		registry->WriteString("desktoplink",desktoplink);
		registry->CloseKey();
	} else {throw Exception("Error!: unable to access the registry");}

	// add product to the software list
	registry->RootKey = HKEY_LOCAL_MACHINE;
	current_key = "\\SOFTWARE\\Microsoft\\Windows\\"
		"CurrentVersion\\Uninstall\\"PRODUCT;
	if(registry->OpenKey(current_key,true)) {
		registry->WriteString("DisplayName",Application->Title);
		registry->WriteString("DisplayIcon",
			SetupConfig->INSTALLROOT->Text+"\\"PRODUCT".ico");
		registry->WriteString("UninstallString",
			SetupConfig->INSTALLROOT->Text+"\\"+
			ExtractFileName(Application->ExeName));
		registry->CloseKey();
	} else {throw Exception("Error!: unable to access the registry");}
}
//---------------------------------------------------------------------------

void __fastcall TSetupFinish::install_specific(void) {
	Status->Caption = "Integrating TRowGrid into the borland IDE";
	Application->HandleMessage();

	// integrate TRowGrid into the borland builder IDE
	AnsiString description("RowGrid based on units ");
#ifdef DEMO
	description += "(demo)";
#else
	description += "(freeware)";
#endif
	AnsiString bpl_path = SetupConfig->BCB6ROOT->Text;
	bpl_path += "\\Projects\\bpl\\TRowGrid.bpl";
	registry->RootKey = HKEY_CURRENT_USER;
	current_key = "\\Software\\Borland\\C++Builder\\6.0\\Known Packages";
	if(registry->OpenKey(current_key,false)) {
		registry->WriteString(bpl_path,description);
		registry->CloseKey();
	} // else borland not installed, do not write registry value
	current_key = "\\Software\\Borland\\C++Builder\\6.0\\Palette";
	if(registry->OpenKey(current_key,false)) {
		AnsiString components = registry->ReadString("PostWare");
		if(components.Pos("RowGrid.TRowGrid;") == 0) {
			registry->WriteString("PostWare",components+"RowGrid.TRowGrid;");
		}
		registry->CloseKey();
	} // else borland not installed, do not write registry value
	current_key = "\\Software\\Borland\\C++Builder\\6.0\\Disabled Packages";
	if(registry->OpenKey(current_key,false)) {
		if(registry->ReadString(bpl_path).IsEmpty() == false) {
			int r = MessageDlg(
				"The installer has detected that the TRowGrid component\n"
				"is disabled in the Borland IDE. Do you want to re-enable it?",
				mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,0);
			if(r == ID_YES && registry->DeleteValue(bpl_path) == false) {
				throw Exception("Error!: unable to access the registry");
			}
		}
		registry->CloseKey();
	} // else borland not installed, do not write registry value
}
//---------------------------------------------------------------------------

void __fastcall TSetupFinish::uninstall_files(void) {
	list<entry_info_t>::const_iterator it = files.begin();
	for(int i=0;it != files.end();it++,i++) {
		ProgressBar1->Position = i;
		if(it->name == ExtractFileName(Application->ExeName)) {continue;}
//	cannot delete install.exe ->

		AnsiString to = it->root;
		if(to == "NOINSTALL") {continue;}
		else if(to == "INSTALLROOT") {to = installroot;}
		else if(to == "BCB6ROOT") {to = bcb6root;}
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
		throw Exception("Error!: unable to access the registry");
	}
	registry->RootKey = HKEY_LOCAL_MACHINE;
	current_key = "\\SOFTWARE\\Microsoft\\Windows\\"
		"CurrentVersion\\Uninstall\\"PRODUCT;
	if(registry->DeleteKey(current_key) == false) {
		throw Exception("Error!: unable to access the registry");
	}
}
//---------------------------------------------------------------------------

void __fastcall TSetupFinish::uninstall_specific(void) {
	Status->Caption = "Removing TRowGrid from the borland IDE";
	Application->HandleMessage();

	// remove TRowGrid from the borland builder IDE
	AnsiString bpl_path = bcb6root+"\\Projects\\bpl\\TRowGrid.bpl";
	registry->RootKey = HKEY_CURRENT_USER;
	current_key = "\\Software\\Borland\\C++Builder\\6.0\\Known Packages";
	if(registry->OpenKey(current_key,false)) {
		registry->DeleteValue(bpl_path);
		registry->CloseKey();
	}
	current_key = "\\Software\\Borland\\C++Builder\\6.0\\Palette";
	if(registry->OpenKey(current_key,false)) {
		AnsiString components = registry->ReadString("PostWare");
		for(int index;(index = components.Pos("RowGrid.TRowGrid;")) != 0;) {
			components.Delete(index,sizeof("RowGrid.TRowGrid;")-1);
		}
		if(components.IsEmpty()) {registry->DeleteValue("PostWare");}
		else {registry->WriteString("PostWare",components);}
		registry->CloseKey();
	}
}
//---------------------------------------------------------------------------

void __fastcall TSetupFinish::uninstall_database(void) {
}
//---------------------------------------------------------------------------

void __fastcall TSetupFinish::NextClick(TObject *Sender) {
	if(Next->Caption == "Finish") {Close(); return;}
	if(Next->Caption == "Install") {
		install_database();
		install_files();
		install_links();
		install_registry(); // uses links
		install_specific();
		Status->Caption = "Install complete";
	} else if(Next->Caption == "Uninstall") {
		Status->Caption = "Reading registry values";
		Application->HandleMessage();

		registry->RootKey = HKEY_CURRENT_USER;
		current_key = "\\Software\\PostWare\\"PRODUCT;
		if(registry->OpenKey(current_key,false)) {
			bcb6root = registry->ReadString("bcb6root");
			installroot = registry->ReadString("installroot");
			programlink = registry->ReadString("programlink");
			desktoplink = registry->ReadString("desktoplink");
			registry->CloseKey();
		} else {throw Exception("Error!: unable to access the registry");}

		uninstall_files();
		uninstall_links();
		uninstall_registry(); // uses current_key
		uninstall_specific();
		uninstall_database();
		Status->Caption = "Uninstall complete";
	}
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
	} else if(errno == EINVAL /*ENOTDIR*/) {doEntry(path,tail,true,depth,data);}
	else {PERROR();}
}
//---------------------------------------------------------------------------

void cpEntry(AnsiString path,AnsiString tail,
	bool path_is_file,int depth,void * data) {
	struct stat entry_stat;
	if(stat(path.c_str(),&entry_stat) == -1) {PERROR();}
	AnsiString to = ((entry_copy_t *) data)->to+tail;

	if(path_is_file) {
		if(!depth && DirectoryExists(to)) {to += "\\"+ExtractFileName(path);}
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
		SETTINGS->Lines->Add(" "PRODUCT" will be installed using the");
		SETTINGS->Lines->Add(" following settings:");
		SETTINGS->Lines->Add("");
		SETTINGS->Lines->Add(" Borland C++ Builder 6 PATH:");
		SETTINGS->Lines->Add("   "+SetupConfig->BCB6ROOT->Text);
		SETTINGS->Lines->Add(" "PRODUCT" installation PATH:");
		SETTINGS->Lines->Add("   "+SetupConfig->INSTALLROOT->Text);
		SETTINGS->Lines->Add(" Username:");
		SETTINGS->Lines->Add("   "+SetupConfig->USERNAME->Text);
	} else if(Next->Caption == "Uninstall") {
		SETTINGS->Lines->Clear();
		SETTINGS->Lines->Add("");
		SETTINGS->Lines->Add(" "PRODUCT" will be removed when you click ");
		SETTINGS->Lines->Add(" the Uninstall button.");
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

