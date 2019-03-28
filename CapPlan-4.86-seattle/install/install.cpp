//$Id: install.cpp,v 1.16 2011/08/30 18:06:41 darseq Exp $
#include <vcl.h>
#include <registry.hpp>
// #include <Psapi.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("finish.cpp", SetupFinish);
USEFORM("download.cpp", SetupDownload);
USEFORM("setup.cpp", SetupBase);
USEFORM("welcome.cpp", SetupWelcome);
USEFORM("selectdir.cpp", Fselect);
USEFORM("config.cpp", SetupConfig);
USEFORM("license.cpp", SetupLicense);
//---------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		FARPROC disable = GetProcAddress(
			GetModuleHandle("user32.dll"),"DisableProcessWindowsGhosting");
		if(disable) {disable();}
/* disabled: not supported on windows 9x
		DWORD process_ids[1024],bytes,processes;
		if(EnumProcesses(process_ids,sizeof(process_ids),&bytes)) {
			processes = bytes/sizeof(DWORD);
			for(unsigned i=0;i < processes;i++) {
				if(process_ids[i]) {
					HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION |
						PROCESS_VM_READ,FALSE,process_ids[i]);
					if(process) {
						HMODULE module;
						if(EnumProcessModules(process,&module,
							sizeof(module),&bytes)) {
							char name[MAX_PATH];
							GetModuleBaseName(process,module,name,
								sizeof(name)/sizeof(char));
							if(String(name) == String(PRODUCT)+".exe") {
								AnsiString message(PRODUCT" is running! Please "
									"close "PRODUCT" before you continue.");
								MessageDlg(message.c_str(),mtWarning,
									TMsgDlgButtons() << mbOK,0);
							}
						}
					}
				}
			}
		}
*/
#ifdef DEMO
		Application->Title = PRODUCT "-" VERSION "-demo";
#elif defined FREEWARE
		Application->Title = PRODUCT "-" VERSION "-freeware";
#else
		Application->Title = PRODUCT "-" VERSION "-full";
#endif
		TRegistry * registry = new TRegistry();
		registry->Access = KEY_READ; // necessary?
		registry->RootKey = HKEY_CURRENT_USER;
		AnsiString current_key = "\\Software\\PostWare\\" PRODUCT;
		Application->Tag = registry->OpenKey(current_key,false);
		if(Application->Tag) { // already installed?
			AnsiString version = registry->ReadString("version");
			registry->CloseKey();
			if(version != Application->Title) {
				AnsiString message("The installer has detected that "+version+
					" is installed.\nDo you want to replace that version with "+
					Application->Title+"?");
				MessageBeep(MB_ICONEXCLAMATION);
				if(MessageDlg(message.c_str(),mtWarning,
					TMsgDlgButtons() << mbYes << mbNo,0) == mrYes) {
					Application->Tag = false; // force install
				} else {Application->Terminate();} // delayed until Run
			}
		}
		delete registry;

		if(Application->Tag == false) { // install?
			Application->CreateForm(__classid(TSetupWelcome), &SetupWelcome);
			Application->CreateForm(__classid(TSetupLicense), &SetupLicense);
			Application->CreateForm(__classid(TSetupConfig), &SetupConfig);
			Application->CreateForm(__classid(TSetupDownload), &SetupDownload);
			Application->CreateForm(__classid(TFselect), &Fselect);
		}
		Application->CreateForm(__classid(TSetupFinish), &SetupFinish);
		Application->HintHidePause = 5000;
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
