//$Id: planner.cpp,v 1.40 1998/01/22 19:37:32 darseq Exp $
#include <vcl.h>
#pragma hdrstop
#include "defines.h"
//---------------------------------------------------------------------------
USEFORM("Fbase.cpp", Fbase1);
USEFORM("Fmain.cpp", Fmain1);
USEFORM("Fpending.cpp", Fpending1);
USEFORM("Fcomplete.cpp", Fcomplete1);
USEFORM("Foptions.cpp", Foptions1);
USEFORM("Fmdetail.cpp", Fmdetail1);
USEFORM("Farchive.cpp", Farchive1);
USEFORM("Fdetails.cpp", Fdetails1);
USEFORM("Fcodes.cpp", Fcodes1);
USEFORM("Frange.cpp", Frange1);
USEFORM("Fprint.cpp", Fprint1);
USEFORM("Ffind.cpp", Ffind1);
USEFORM("Fentries.cpp", Fentries1);
USEFORM("Fcreated.cpp", Fcreated1);
USEFORM("Fschedule.cpp", Fschedule1);
USEFORM("Femployee.cpp", Femployee1);
USEFORM("Fimport.cpp", Fimport1);
USEFORM("Fmail.cpp", Fmail1);
//---------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		Application->Title = "CapPlan";
		Application->CreateForm(__classid(TFmain1), &Fmain1);
		if(Application->Terminated == false) {
			Application->CreateForm(__classid(TFoptions1), &Foptions1);
			Application->CreateForm(__classid(TFpending1), &Fpending1);
			Application->CreateForm(__classid(TFarchive1), &Farchive1);
			Application->CreateForm(__classid(TFdetails1), &Fdetails1);
			Application->CreateForm(__classid(TFemployee1), &Femployee1);
			Application->CreateForm(__classid(TFimport1), &Fimport1);
			Application->CreateForm(__classid(TFprint1), &Fprint1);
			Application->CreateForm(__classid(TFrange1), &Frange1);
			Application->CreateForm(__classid(TFfind1), &Ffind1);
			Application->CreateForm(__classid(TFcodes1), &Fcodes1);
			Application->CreateForm(__classid(TFentries1), &Fentries1);
			Application->CreateForm(__classid(TFcreated1), &Fcreated1);
			Application->CreateForm(__classid(TFschedule1), &Fschedule1);
			Application->CreateForm(__classid(TFcomplete1), &Fcomplete1);
			Application->CreateForm(__classid(TFmail1), &Fmail1);
			Application->Run();
		}
	}
	catch (Exception &exception)
	{
		MSGEXC(AnsiString(exception.Message).c_str());
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			MSGEXC(AnsiString(exception.Message).c_str());
		}
	}
	return 0;
}
//---------------------------------------------------------------------------

