#pragma once
#include "afxwin.h"


// CPromptDlg dialog

class CPromptDlg : public CDialog
{
	DECLARE_DYNAMIC(CPromptDlg)

public:
	CPromptDlg(CWnd* pParent, CString strPrompt);   // standard constructor
	virtual ~CPromptDlg();

// Dialog Data
	enum { IDD = IDD_PROMPT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	void InitPrompt();

	DECLARE_MESSAGE_MAP()
private:
	CEdit m_editNewFeature;
	CString m_strPrompt;
};
