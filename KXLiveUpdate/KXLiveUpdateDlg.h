#pragma once
#include "UpAndDownLoad.h"

typedef struct stVersionCfg
{
	CString strVertion;
	CString strDownloadUrl;
	CString strUploadUrl;
	CString strIsUploadLog;
	CString strDescription;

	stVersionCfg():
	strVertion(L""),
	strDownloadUrl(L""),
	strUploadUrl(L""),
	strIsUploadLog(L""),
	strDescription(L"")
	{}
	~stVersionCfg()
	{};
}VersionCfg;

enum eDownloadType
{
	eDownloadConfig, //下载配置文件
	eDownloadSetup, //下载安装包
};

enum eUploadType
{
	eUploadLog, //上传日志
};

//class CPromptDlg;
// CKXLiveUpdateDlg dialog
class CKXLiveUpdateDlg : public CDialog
{
// Construction
public:
	CKXLiveUpdateDlg(CWnd* pParent = NULL);	// standard constructor
	~CKXLiveUpdateDlg();

// Dialog Data
	enum { IDD = IDD_KXLIVEUPDATE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg LRESULT OnUploadFinished(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDownloadFinished(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnPluginNotifyMe(WPARAM wParam, LPARAM lParam);
//	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg LRESULT OnCopyData(WPARAM wParam, LPARAM lParam);

	void GetCurrentVerCfg();//获取当前的版本配置
	void GetLatestVerCfg();//获取最新的版本配置
	void DownLoadLatestVersionCfg();
	void DownLoadLatestSetup();
	void UploadLog();//上传日志
	bool NeedToDownloadLatestSetup();
	bool GladToUpdateNow();//是否愿意现在更新

	void QuitSelf();

	DECLARE_MESSAGE_MAP()
private:
	CUpAndDownLoad m_upAndDownLoad;

	VersionCfg m_curVersionConfig;
	VersionCfg m_latestVersionConfig;
	CString m_strLatestVersionUrl;//最新配置文件地址
	UINT32 m_uid;

	bool m_bFinishedDownload;
	bool m_bFinishedUpload;
};

