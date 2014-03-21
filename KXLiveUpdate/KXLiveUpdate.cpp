
// KXLiveUpdate.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "KXLiveUpdate.h"
#include "KXLiveUpdateDlg.h"
#include <tlhelp32.h>
#include <algorithm>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CKXLiveUpdateApp

BEGIN_MESSAGE_MAP(CKXLiveUpdateApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()



//检查更新程序是否启动了
bool CheckKXLiveUpdate()
{
	std::wstring ezonename(L"kxliveupdate.exe");//注意，要用小写！

	HANDLE hProcessSnap = NULL;
	HANDLE hProcess = NULL;
	PROCESSENTRY32 pe32;

	hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if( hProcessSnap == INVALID_HANDLE_VALUE )
	{
		return false;
	}

	pe32.dwSize = sizeof( PROCESSENTRY32 );

	if( !Process32First( hProcessSnap, &pe32) )
	{
		CloseHandle( hProcessSnap );         
		return false;
	}

	int process_count = 0;
	do
	{
		std::wstring exeFile = pe32.szExeFile;
		transform(exeFile.begin(),exeFile.end(), exeFile.begin(), tolower);   

		if(exeFile == ezonename)
			process_count ++;

	} while( Process32Next( hProcessSnap, &pe32 ) );

	CloseHandle(hProcessSnap);

	if(process_count > 1)
	{
		return true;
	}
	else 
	{
		return false;
	}
}


// CKXLiveUpdateApp construction

CKXLiveUpdateApp::CKXLiveUpdateApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CKXLiveUpdateApp object

CKXLiveUpdateApp theApp;


// CKXLiveUpdateApp initialization

BOOL CKXLiveUpdateApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.

	if (CheckKXLiveUpdate())
	{
		return FALSE;
	}

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CKXLiveUpdateDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
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
