//
// Copyright (C) 1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Broadcast Architecture DDK.
// For detailed information regarding Broadcast
// Architecture, see the DDK.
//
// scope.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "scope.h"
#include "scopeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDerivedTestGraph g_Graph;
BOOL BuildGraph();

/////////////////////////////////////////////////////////////////////////////
// CScopeApp

BEGIN_MESSAGE_MAP(CScopeApp, CWinApp)
	//{{AFX_MSG_MAP(CScopeApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScopeApp construction

CScopeApp::CScopeApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CScopeApp object

CScopeApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CScopeApp initialization

BOOL CScopeApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// here we check if vidsvr or TVX is running
	HWND hViewer = FindWindow(NULL, "TV Viewer");
	if (hViewer)
	{
		int iChoice = AfxMessageBox("TV Viewer is currently running. This application may behave unpredictably.\nClose TV Viewer? (Select CANCEL to quit the Scope application)", MB_YESNOCANCEL);
		if (iChoice == IDYES)
		{
			SendMessage(hViewer, WM_CLOSE, 0, 0);
		}
		else if (iChoice == IDCANCEL)
		{
			return(FALSE);
		}
	}

	if (!BuildGraph())
	{
		AfxMessageBox("Unable to build the necessary DirectShow filter graph.\nThis application will now terminate.");
		return(FALSE);
	}

	CScopeDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

BOOL BuildGraph()
{
	#define ADDCAT(x,y) \
		{ \
			if (bOk) bOk &= g_Graph.AddFilterByCategoryAndName(x, y); \
			if (!bOk) \
			{ \
				strMsg.Format("Unable to instantiate the DirectShow filter named \"%s\".\nPlease verify that TV Viewer is installed.\n(Check using Control Panel, Add/Remove Programs, Windows Setup)", y); \
				AfxMessageBox(strMsg); \
				return(FALSE); \
			} \
		}
	#define ADDCAT2(x,y,s) \
		{ \
			if (bOk) bOk &= g_Graph.AddFilterByCategoryAndName(x, y); \
			if (!bOk) \
			{ \
				strMsg.Format("Unable to instantiate the DirectShow filter named \"%s\".\n%s", y, s); \
				AfxMessageBox(strMsg); \
				return(FALSE); \
			} \
		}
	#define ADD(x,y) \
		{ \
			if (bOk) bOk &= g_Graph.AddFilterByCLSID(x, y); \
			if (!bOk) \
			{ \
				strMsg.Format("Unable to instantiate the DirectShow filter named \"%s\".\nPlease verify that TV Viewer is installed.\n(Check using Control Panel, Add/Remove Programs, Windows Setup)", y); \
				AfxMessageBox(strMsg); \
				return(FALSE); \
			} \
		}
	#define ADD2(x,y,s) \
		{ \
			if (bOk) bOk &= g_Graph.AddFilterByCLSID(x, y); \
			if (!bOk) \
			{ \
				strMsg.Format("Unable to instantiate the DirectShow filter named \"%s\".\n%s", y, s); \
				AfxMessageBox(strMsg); \
				return(FALSE); \
			} \
		}
	#define CONNECT(f1,p1,f2,p2) \
		{ \
			if (bOk) bOk &= g_Graph.ConnectFilters(f1,p1,f2,p2); \
			if (!bOk) \
			{ \
				strMsg.Format("Unable to connect the \"%s\" pin of the filter \"%s\" to the \"%s\" pin of the filter \"%s\".", p1, f1, p2, f2); \
				AfxMessageBox(strMsg); \
				return(FALSE); \
			} \
		}

	CString strMsg;
	BOOL bOk = TRUE;
	//
	// let's instantiate some category-based filters
	//
	ADDCAT(AM_KSCATEGORY_TVTUNER, "ATI TVTuner");
	ADDCAT(AM_KSCATEGORY_CROSSBAR, "ATI Crossbar");
	ADDCAT(AM_KSCATEGORY_CROSSBAR, "Bt829 Crossbar");
	ADDCAT(AM_KSCATEGORY_CAPTURE, "Bt829 Capture");
	ADDCAT(AM_KSCATEGORY_VBICODEC, "CC Decoder") ;
	ADDCAT(AM_KSCATEGORY_VBICODEC, "NABTS");
	ADDCAT2(AM_KSCATEGORY_VBICODEC, "Generic", "Please install the Generic VBI codec (codec.sys) in your machine.\n(Use the Add New Hardware cpl, select \"Sound, video and game controllers,\" and point to codec.inf.)");
	ADDCAT(AM_KSCATEGORY_MSTEE, "Tee");
	//
	// let's get some "normal" filters
	//
	ADD(CLSID_VBISurfaces, "VBI Surface Allocator");
	ADD(CLSID_Line21Decoder, "Line 21 Decoder");
	ADD(CLSID_OverlayMixer, "Overlay Mixer");
	ADD(CLSID_VideoRenderer, "Video Renderer");
	ADD2(CLSID_VBIScope, "VBI Scope", "Please install the VBI Scope filter (vbiscope.ax) in your machine.\n(Copy vbiscope.ax to your system dir and run \"regsvr32 vbiscope.ax\".)");

	//
	// wire em up
	//
	g_Graph.SetNameExactMatch(TRUE);
	CONNECT("Bt829 Capture", "VP","Overlay Mixer", "Input0");
	g_Graph.SetNameExactMatch(FALSE);
	//
	// tuner to ati crossbar
	//
	CONNECT("ATI TVTuner", "Analog Video", "ATI Crossbar", "Video Tuner In");
	CONNECT("ATI TVTuner", "Analog Audio", "ATI Crossbar", "Audio Tuner In");
	//
	// ati crossbar to bt829 crossbar
	//
	CONNECT("ATI Crossbar", "Composite", "Bt829 Crossbar", "Composite");
	CONNECT("ATI Crossbar", "SVideo", "Bt829 Crossbar", "SVideo");
	CONNECT("ATI Crossbar", "Tuner", "Bt829 Crossbar", "Tuner");
	//
	// bt829 crossbar to bt829 capture
	//
	CONNECT("Bt829 Crossbar", "Video Decoder Out", "Bt829 Capture", "Analog Video In");
	//
	// bt829 capture to surface allocator
	//
	CONNECT("Bt829 Capture", "VPVBI","VBI Surface Allocator", "VBI Notify");

	//
	// bt829 capture to mstee
	//
	g_Graph.SetNameExactMatch(TRUE);
	CONNECT("Bt829 Capture", "VBI","Tee", "Communication Transform");
	g_Graph.SetNameExactMatch(FALSE);

	//
	// mstee to generic to scope
	//
	CONNECT("Tee", "Splitter","Generic", "VBI");
	CONNECT("Generic", "VBI", "VBI Scope", "Scope Input Pin");

	//
	// mstee to nabts codec
	//
	CONNECT("Tee", "Splitter","NABTS", "VBI");
	//
	// mstee to cc codec to l21 decoder to mixer
	//
	CONNECT("Tee", "Splitter","CC Decoder", "VBI");
	CONNECT("CC Decoder", "CC","Line 21 Decoder", "XForm In");
	CONNECT("Line 21 Decoder", "XForm Out","Overlay Mixer", "Input2");

	CONNECT("Overlay Mixer", "Output", "Video Renderer", "Input");

	return(TRUE);
}