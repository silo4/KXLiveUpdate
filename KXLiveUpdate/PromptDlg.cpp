// PromptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "KXLiveUpdate.h"
#include "PromptDlg.h"


// CPromptDlg dialog

IMPLEMENT_DYNAMIC(CPromptDlg, CDialog)

CPromptDlg::CPromptDlg(CWnd* pParent /*=NULL*/, CString strPrompt)
	: CDialog(CPromptDlg::IDD, pParent)
{
	m_strPrompt = strPrompt;
}

CPromptDlg::~CPromptDlg()
{
}

void CPromptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_editNewFeature);
}


BEGIN_MESSAGE_MAP(CPromptDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CPromptDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPromptDlg::OnBnClickedCancel)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CPromptDlg message handlers

void CPromptDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CPromptDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CPromptDlg::InitPrompt()
{
	if (m_strPrompt == L"")
	{
		return;
	}

	m_editNewFeature.SetReadOnly(TRUE);
	m_editNewFeature.SetWindowText(m_strPrompt.GetBuffer(0));
}

BOOL CPromptDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	
	InitPrompt();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CPromptDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if (pWnd->GetDlgCtrlID() == IDC_EDIT1)
	{
		pDC->SetBkMode(TRANSPARENT);
		HBRUSH hBr = CreateSolidBrush(RGB(255,255,255));   
		return (HBRUSH)hBr;  
	}

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}
