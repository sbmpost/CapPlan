//$Id: install.cpp,v 1.6 2006/09/08 01:15:29 darseq Exp $
#include <vcl.h>
#include <registry.hpp>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("setup.cpp", SetupBase);
USEFORM("welcome.cpp", SetupWelcome);
USEFORM("license.cpp", SetupLicense);
USEFORM("config.cpp", SetupConfig);
USEFORM("selectdir.cpp", Fselect);
USEFORM("finish.cpp", SetupFinish);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
#ifdef DEMO
		Application->Title = PRODUCT"-"VERSION"-demo";
#else
		Application->Title = PRODUCT"-"VERSION"-freeware";
#endif
		TRegistry * registry = new TRegistry();
		registry->RootKey = HKEY_CURRENT_USER;
		AnsiString current_key = "\\Software\\PostWare\\"PRODUCT;
		Application->Tag = registry->OpenKey(current_key,false);
		if(Application->Tag) { // already installed?
			AnsiString version = registry->ReadString("version");
			registry->CloseKey();
			if(version != Application->Title) {
				AnsiString message("The installer has detected that "+version+
					" is installed. It is recommended that you first uninstall "
					"that version before installing "+Application->Title+
					". Proceed anyway?");
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
