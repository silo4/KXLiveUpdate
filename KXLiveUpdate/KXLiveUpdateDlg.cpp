
// KXLiveUpdateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "KXLiveUpdate.h"
#include "KXLiveUpdateDlg.h"
#include "Markup.h"
#include <atlconv.h>
#include "KXLog.h"
#include "PromptDlg.h"

//#define  MSG_PLUGIN_NOTIFY_UPDATE		 WM_USER+0x0011

#define  CURRENT_VERSION                 _T("\\KXLiveLatestVersion.xml")
#define  LATEST_VERSION					 _T("\\KXLiveServerVersion.xml")
#define  LATEST_SETUP_NAME               _T("\\KXLiveSetup.exe")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CKXLiveUpdateDlg dialog

CKXLiveUpdateDlg::CKXLiveUpdateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKXLiveUpdateDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_strLatestVersionUrl = L"";/*L"http://192.168.20.137/download/KXLiveLatestVersion.xml";*/
	m_uid = 0;
	m_bFinishedDownload = true;
	m_bFinishedUpload = true;
}

CKXLiveUpdateDlg::~CKXLiveUpdateDlg()
{
	CKXLog::DestroyKXLog();
}


void CKXLiveUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CKXLiveUpdateDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CKXLiveUpdateDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CKXLiveUpdateDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &CKXLiveUpdateDlg::OnBnClickedButton1)
	ON_MESSAGE(WM_DOWNLOAD_THREAD_FINISH, OnDownloadFinished)
	ON_MESSAGE(WM_UPLOAD_THREAD_FINISH, OnUploadFinished)
	//ON_MESSAGE(MSG_PLUGIN_NOTIFY_UPDATE, OnPluginNotifyMe)
	ON_BN_CLICKED(IDC_BUTTON2, &CKXLiveUpdateDlg::OnBnClickedButton2)
	//ON_WM_COPYDATA()
	ON_MESSAGE(WM_COPYDATA, OnCopyData)
END_MESSAGE_MAP()
// CKXLiveUpdateDlg message handlers


CString GetModulePath()
{
	CString path; 
	GetModuleFileName(NULL,path.GetBufferSetLength(MAX_PATH+1),MAX_PATH);
	path.ReleaseBuffer(); 
	int pos = path.ReverseFind('\\'); 
	path = path.Left(pos); 

	return path;
}
 
BOOL CKXLiveUpdateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
#ifndef _DEBUG
	CRect rc(0, 0, 0, 0);
	MoveWindow(&rc);
	ShowWindow(SW_HIDE);
	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW);
#endif
	GetCurrentVerCfg();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CKXLiveUpdateDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CKXLiveUpdateDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CKXLiveUpdateDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CKXLiveUpdateDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//OnOK();
#ifdef _DEBUG
	DownLoadLatestVersionCfg();
#endif
	
}

void CKXLiveUpdateDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
#ifdef _DEBUG	
	OnCancel();
#endif
}

void CKXLiveUpdateDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
#ifdef _DEBUG
	vector<CString> vUpdateLog = KX_LOG_GET_NOT_TODAY_LOG();
	m_upAndDownLoad.UploadFile(GetSafeHwnd(), m_latestVersionConfig.strUploadUrl, vUpdateLog, eUploadLog);
#endif
}

void CKXLiveUpdateDlg::GetCurrentVerCfg()
{
	CMarkup xml;
	CString strPath = GetModulePath() + CURRENT_VERSION;
	xml.Load(strPath.GetBuffer(0));
	
	if(xml.FindChildElem(L"item"))
	{
		xml.IntoElem();
		bool bFind = xml.FindChildElem(L"version");
		m_curVersionConfig.strVertion = (bFind ? xml.GetChildData() : L"");
		//test
		/*bFind = xml.FindChildElem(L"description");
		m_latestVersionConfig.strDescription = (bFind ? xml.GetChildData() : L"");
		GladToUpdateNow();*/
		///////
		xml.OutOfElem();
	}
}

void CKXLiveUpdateDlg::GetLatestVerCfg()
{
	CMarkup xml;
	CString strPath = GetModulePath() + LATEST_VERSION;
	xml.Load(strPath.GetBuffer(0));

	if(xml.FindChildElem(L"item"))
	{
		xml.IntoElem();
		bool bFind = xml.FindChildElem(L"version");
		m_latestVersionConfig.strVertion = (bFind ? xml.GetChildData() : L"");
		bFind = xml.FindChildElem(L"download_url");
		m_latestVersionConfig.strDownloadUrl = (bFind ? xml.GetChildData() : L"");
		bFind = xml.FindChildElem(L"is_upload_log");
		m_latestVersionConfig.strIsUploadLog = (bFind ? xml.GetChildData() : L"");
		bFind = xml.FindChildElem(L"upload_url");
		m_latestVersionConfig.strUploadUrl = (bFind ? xml.GetChildData() : L"");
		bFind = xml.FindChildElem(L"description");
		m_latestVersionConfig.strDescription = (bFind ? xml.GetChildData() : L"");
		xml.OutOfElem();
	}
}

void CKXLiveUpdateDlg::DownLoadLatestVersionCfg()//下载最新配置文件
{
	m_bFinishedDownload = false;
	CString strDownloadLocalPath = GetModulePath() + LATEST_VERSION;
	//CString tempUrl = L"http://v.51.com/swf/KXLiveLatestVersion.xml?v=2014021701";
	m_upAndDownLoad.DownloadFile(GetSafeHwnd(), m_strLatestVersionUrl, strDownloadLocalPath, eDownloadConfig);
	//m_upAndDownLoad.DownloadFile(GetSafeHwnd(), tempUrl, strDownloadLocalPath, eDownloadConfig);
}

void CKXLiveUpdateDlg::DownLoadLatestSetup()//下载最新安装包
{
	m_bFinishedDownload = false;
	CString strDownloadLocalPath = GetModulePath() + LATEST_SETUP_NAME;
	m_upAndDownLoad.DownloadFile(GetSafeHwnd(), m_latestVersionConfig.strDownloadUrl, strDownloadLocalPath, eDownloadSetup);
}

LRESULT CKXLiveUpdateDlg::OnUploadFinished( WPARAM wParam, LPARAM lParam )
{
	if ((int)lParam == eLoadFailed)//上传失败
	{
		KX_LOG("Upload KXLiveUpdate_log failed!");
		m_bFinishedUpload = true;
		QuitSelf();
		return FALSE;
	}
	switch((int)wParam)
	{
	case eUploadLog://上传日志文件成功
		{
			KX_LOG("The KXLiveUpdate_log is uploaded succeed!");
			m_bFinishedUpload = true;
			KX_LOG_DELETE_LOG();//上传完后就删除
			QuitSelf();
		}	
		break;
	default:break;
	}

	return TRUE;
}

LRESULT CKXLiveUpdateDlg::OnDownloadFinished( WPARAM wParam, LPARAM lParam )
{
	if ((int)lParam == eLoadFailed)//下载失败
	{
		if ((int)wParam == eDownloadConfig)
		{
			KX_LOG("Download Config Failed!");
		}else
		{
			KX_LOG("Download Setup Failed!");
		}
		m_bFinishedDownload = true;
		QuitSelf();
		return FALSE;
	}
	switch((int)wParam)
	{
	case eDownloadConfig://下载配置文件成功
		{
			KX_LOG("Download config succeed!");
			m_bFinishedDownload = true;
			if (NeedToDownloadLatestSetup())
			{
				KX_LOG("NeedToDownloadLatestSetup, now begin download");
				DownLoadLatestSetup();
			}else
			{
				KX_LOG("Don't NeedToDownloadLatestSetup");
			}
			if (m_latestVersionConfig.strIsUploadLog.Trim() != L"0")//需要上传
			{
				KX_LOG("Need to upload log, now begin");
				UploadLog();
			}else
			{
				KX_LOG("Dont't need to upload log");
			}

			QuitSelf();
		}	
		break;
	case eDownloadSetup://下载安装包成功
		{
			KX_LOG("Download setup succeed!");
			
			/*if (IDOK == ::MessageBox(GetSafeHwnd(), L"        下载完成，是否开始安装？", L"提示", MB_OKCANCEL))
			{*/
				CString strExePath = GetModulePath() + LATEST_SETUP_NAME;
				::ShellExecute(NULL, L"open", strExePath, NULL, NULL, SW_SHOW);
			/*	KX_LOG("Sure to open the setup");
			}else
			{
				KX_LOG("Cancel to open the setup");
			}*/
			m_bFinishedDownload = true;
			QuitSelf();
		}
		break;
	default:break;
	}

	return TRUE;
}

bool CKXLiveUpdateDlg::NeedToDownloadLatestSetup()//版本对比
{
	GetLatestVerCfg();
	if (m_curVersionConfig.strVertion == _T("") || m_latestVersionConfig.strVertion == _T(""))
	{
		return false;
	}
	CString strLatestVersion = m_latestVersionConfig.strVertion;
	CString strCurrentVersion = m_curVersionConfig.strVertion;
	int nLatestVerIndex;
	int nCurrentVerIndex;
	char buf[128];
	sprintf(buf, "localVer: %s, serverVer: %s", strCurrentVersion, strLatestVersion);
	string s = buf;
	KX_LOG(s);

	do 
	{
		strLatestVersion.Trim();
		strCurrentVersion.Trim();
		nLatestVerIndex = strLatestVersion.Find('.');
		nCurrentVerIndex = strCurrentVersion.Find('.');

		try // _ttoi会抛出异常
		{
			int nLatestVer, nCurrentVer;
			if (nLatestVerIndex == -1 && nCurrentVerIndex == -1)//说明是最后一段版本号
			{
				nLatestVer = _ttoi(strLatestVersion);
				nCurrentVer = _ttoi(strCurrentVersion);	
			}else
			{
				nLatestVer = _ttoi(strLatestVersion.Left(nLatestVerIndex));
				nCurrentVer = _ttoi(strCurrentVersion.Left(nCurrentVerIndex));
			}
			
			if (nLatestVer > nCurrentVer)
			{
				char buf[128];
				sprintf(buf, "localVer: %d, serverVer: %d, can update", nCurrentVer, nLatestVer);
				string s = buf;
				KX_LOG(s);
				return GladToUpdateNow();
			}else if (nLatestVer < nCurrentVer)
			{
				KX_LOG("don't need to update!");
				return false;
			}
			strLatestVersion = strLatestVersion.Right(strLatestVersion.GetLength() - nLatestVerIndex - 1);
			strCurrentVersion = strCurrentVersion.Right(strCurrentVersion.GetLength() - nCurrentVerIndex - 1);
		}
		catch (CException* e)
		{
			//AfxMessageBox(L"版本号不是数字");
			KX_LOG("the version is not digit");
			return false;
		}

	} while (nLatestVerIndex != -1 && nCurrentVerIndex != -1);
	
	return false;
}

void CKXLiveUpdateDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
}

LRESULT CKXLiveUpdateDlg::OnCopyData(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your message handler code here and/or call default
	COPYDATASTRUCT* pCopyDataStruct = (COPYDATASTRUCT*)lParam;

	char arrUrl[500] = {0};
	arrUrl[pCopyDataStruct->cbData] = '\0';
	memcpy(arrUrl, pCopyDataStruct->lpData, pCopyDataStruct->cbData);
	m_strLatestVersionUrl = arrUrl;
	m_uid = (UINT32) pCopyDataStruct->dwData;
#ifdef _DEBUG	
	OutputDebugString(L"\n((((( OnCopyData )))))\n");
	OutputDebugString(m_strLatestVersionUrl.GetBuffer(0));
#endif
	KX_LOG("OnCopyData()");

	DownLoadLatestVersionCfg();
	
	return TRUE;
}

void CKXLiveUpdateDlg::UploadLog()
{
	m_bFinishedUpload = false;
	vector<CString> vUpdateLog = KX_LOG_GET_NOT_TODAY_LOG();
	CString strUploadUrlWithParam;
	strUploadUrlWithParam.Format(L"%s?uid=%u", m_latestVersionConfig.strUploadUrl, m_uid);
	string strLog = CStringA(strUploadUrlWithParam);
	KX_LOG(strLog);
	m_upAndDownLoad.UploadFile(GetSafeHwnd(), strUploadUrlWithParam, vUpdateLog, eUploadLog);
}

void CKXLiveUpdateDlg::QuitSelf()
{
	CString strTemp1 = m_bFinishedDownload ? _T("TRUE") : _T("FALSE");
	CString strTemp2 = m_bFinishedUpload ? _T("TRUE") : _T("FALSE");
	CString strOutput;
	strOutput.Format(L"QuitSelf():m_bFinishedDownload=%s, m_bFinishedUpload=%s", strTemp1, strTemp2);
	string str = CStringA(strOutput);
	KX_LOG(str);
	if (m_bFinishedDownload && m_bFinishedUpload)
	{
		//SendMessage(WM_CLOSE, 0, 0);
		CDialog::OnCancel();
		KX_LOG("QuitSelf() : CDialog::OnCancel()");
	}
}

bool CKXLiveUpdateDlg::GladToUpdateNow()
{
	//CString strTemp = L"test1\r\ntest2";
	//CPromptDlg proDlg(this, strTemp);
	CString strPrompt = m_latestVersionConfig.strDescription;
	strPrompt.Replace(L"\\n", L"\r\n");
	CPromptDlg proDlg(this, strPrompt);
	BOOL bRet = proDlg.DoModal();
	if (bRet == IDOK)
	{
		KX_LOG("Want to update now!");
		return true;
	}else
	{
		KX_LOG("Don't want to update now!");
		return false;
	}
}
