#pragma once
#include <vector>

enum eLoadType
{
	eTypeDownload,
	eTypeUpload,
};

enum eLoadResult
{
	eLoadSucceed,
	eLoadFailed,
};

#define  WM_DOWNLOAD_THREAD_FINISH     WM_USER + 0x0001
#define  WM_UPLOAD_THREAD_FINISH       WM_USER + 0x0002

class CUpAndDownLoad
{
public:
	CUpAndDownLoad(void);
	~CUpAndDownLoad(void);
protected:
	CString MakeRequestHeaders(CString );
	CString MakePreFileData(CString strBoundary, CString strFileName);
	CString MakePostFileData(CString strBoundary);
	bool Download();
	bool Upload();
	void StopThread(int typeThead);

	static DWORD WINAPI ThreadDownloadProc(LPVOID pParamer);
	static DWORD WINAPI ThreadUploadProc(LPVOID pParamer);
public:
	void DownloadFile(const HWND hParent, const CString& strUrlOnServer, const CString& strUrlOnLocal, int eType);
	void UploadFile(const HWND hParent, const CString& strUrlOnServer, const CString& strUrlOnLocal, int eType);
	void UploadFile(const HWND hParent, const CString& strUrlOnServer, std::vector<CString>& vUrlOnLocal, int eType);
private:
	HANDLE m_hDownloadThread;
	HANDLE m_hUploadThread;
	bool m_bDownloading;
	bool m_bUploading;
	CString m_strDownloadServerUrl;
	CString m_strDownloadLocalUrl;
	CString m_strUploadServerUrl;
	CString m_strUploadLocalUrl;
	std::vector<CString> m_vStrUploadLocalUrl;
	HWND m_hParent; //�������ػ��ϴ��ĳ�����
	int m_nDownloadFileType;//���ص��ĸ��ļ������ڷ��ظ�m_hParent�����ж�
	int m_nUploadFileType;//�ϴ����ĸ��ļ������ڷ��ظ�m_hParent�����ж�
};
