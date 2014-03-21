#include "StdAfx.h"
#include "UpAndDownLoad.h"
#include <afxinet.h>
#include "KXLog.h"

//定义边界值
#define  STR_HTTP_BOUNDARY       _T("---------------------------7b4a6d158c9")  

CUpAndDownLoad::CUpAndDownLoad(void)
{
	m_hDownloadThread = NULL;
	m_hUploadThread = NULL;
	m_hParent = NULL;
	m_bDownloading = false;
	m_bUploading = false;

	m_strDownloadServerUrl = _T("");
	m_strDownloadLocalUrl = _T("");
	m_strUploadServerUrl = _T("");
	m_strUploadLocalUrl = _T("");

	m_nDownloadFileType = -1;
	m_nUploadFileType = -1;
}

CUpAndDownLoad::~CUpAndDownLoad(void)
{
	m_vStrUploadLocalUrl.clear();
}

CString CUpAndDownLoad::MakeRequestHeaders(CString strBoundary)//包头   
{   
	CString strFormat;   
	CString strData;   

	strFormat = _T("Content-Type: multipart/form-data; boundary=%s\r\n");//二进制文件传送Content-Type类型为: multipart/form-data   

	strData.Format(strFormat, strBoundary);   
	return strData;   
}   

CString CUpAndDownLoad::MakePreFileData(CString strBoundary, CString strFileName)   
{  
	CString strFormat = _T("");
	CString strData = _T("");
	strFormat += _T("--%s");
	strFormat += _T("\r\n");
	strFormat += _T("Content-Disposition: form-data; name=\"file\"; filename=\"%s\"");
	strFormat += _T("\r\n");
	strFormat += _T("Content-Type: txt");
	strFormat += _T("\r\n");
	strFormat += _T("Content-Transfer-Encoding: binary");
	strFormat += _T("\r\n\r\n");

	strData.Format(strFormat, strBoundary, strFileName);

	return strData;   
}   

CString CUpAndDownLoad::MakePostFileData(CString strBoundary)//发送请求包   
{   
	CString strFormat;   
	CString strData;   

	strFormat = _T("\r\n");    
	strFormat += _T("--%s");   
	strFormat += _T("\r\n");   
	strFormat += _T("Content-Disposition: form-data; name=\"submitted\"");   
	strFormat += _T("\r\n\r\n");   
	strFormat += _T("submit");   
	strFormat += _T("\r\n");   
	strFormat += _T("--%s--");   
	strFormat += _T("\r\n");   

	strData.Format(strFormat, strBoundary, strBoundary); 

	return strData;   
}   


DWORD WINAPI CUpAndDownLoad::ThreadUploadProc( LPVOID pParamer )
{
	CUpAndDownLoad* pThis = (CUpAndDownLoad*)pParamer;
	pThis->Upload();
	return 0;
}

DWORD WINAPI CUpAndDownLoad::ThreadDownloadProc(LPVOID pParamer)
{
	CUpAndDownLoad* pThis = (CUpAndDownLoad*)pParamer;
	pThis->Download();
	
	return 0;
}

bool CUpAndDownLoad::Upload()
{
	ASSERT(m_hParent != NULL);
	//ASSERT(m_strUploadLocalUrl != _T(""));
	ASSERT(m_strUploadServerUrl != _T(""));
	ASSERT(m_vStrUploadLocalUrl.size() > 0);

	if (m_bUploading || m_hParent == NULL || m_strUploadServerUrl == _T("") || m_vStrUploadLocalUrl.size() <= 0 )
	{
		return false;
	}

	m_bUploading = true;
	CString defServerName;//服务器名   
	CString defObjectName;//服务器端接收文件的页面   
	DWORD dwType;
	INTERNET_PORT  nPort;   
	AfxParseURL(m_strUploadServerUrl, dwType, defServerName, defObjectName, nPort);

	CFile fTrack;  
	bool bSuccess = true;   

	for (std::vector<CString>::iterator iter = m_vStrUploadLocalUrl.begin(); iter != m_vStrUploadLocalUrl.end(); iter++)
	{
		CString strTemp = *iter;
		string strLog = CStringA(strTemp);
		KX_LOG(strLog);
		if (FALSE == fTrack.Open(strTemp/*m_strUploadLocalUrl*/, CFile::modeRead | CFile::shareDenyWrite))//读出文件    
		{   
			//AfxMessageBox(_T("Unable to open the file."));   
			KX_LOG("Unable to open the file.");
			StopThread(eTypeUpload);
			::PostMessage(m_hParent, WM_UPLOAD_THREAD_FINISH, m_nUploadFileType, eLoadFailed);
			return false;   
		}   

		CInternetSession Session;   
		CHttpConnection *pHttpConnection = NULL;   
		CHttpFile* pHTTP;   
		TCHAR szError[MAX_PATH];   
		CString strPreFileData = MakePreFileData(STR_HTTP_BOUNDARY, strTemp/*m_strUploadLocalUrl*/); 
		CString strPostFileData = MakePostFileData(STR_HTTP_BOUNDARY);   
		DWORD dwTotalRequestLength = strPreFileData.GetLength() + strPostFileData.GetLength() + fTrack.GetLength();//计算整个包的总长度   
		DWORD dwChunkLength = 64 * 1024;   
		void* pBuffer = malloc(dwChunkLength);   

		if (NULL == pBuffer)   
		{  
			StopThread(eTypeUpload);
			::PostMessage(m_hParent, WM_UPLOAD_THREAD_FINISH, m_nUploadFileType, eLoadFailed);
			return false;   
		}   

		try   
		{   
			pHttpConnection = Session.GetHttpConnection(defServerName, nPort);   
			pHTTP = pHttpConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, defObjectName);   
			pHTTP->AddRequestHeaders(MakeRequestHeaders(STR_HTTP_BOUNDARY));//发送包头请求   
			pHTTP->SendRequestEx(dwTotalRequestLength, HSR_SYNC | HSR_INITIATE);   

#ifdef _UNICODE   
			USES_CONVERSION;
			pHTTP->Write(W2A(strPreFileData), strPreFileData.GetLength());//写入服务器所需信息 
#else   
			pHTTP->Write((LPSTR)(LPCSTR)strPreFileData, strPreFileData.GetLength());  
#endif   

			DWORD dwReadLength = -1;   
			while (0 != dwReadLength)   
			{    
				dwReadLength = fTrack.Read(pBuffer, dwChunkLength);//文件内容   
				if (0 != dwReadLength)   
				{   
					pHTTP->Write(pBuffer, dwReadLength);//写入服务器本地文件，用二进制进行传送   
				}   
			}   

#ifdef _UNICODE   
			pHTTP->Write(W2A(strPostFileData), strPostFileData.GetLength());   
#else   
			pHTTP->Write((LPSTR)(LPCSTR)strPostFileData, strPostFileData.GetLength());   
#endif   

			pHTTP->EndRequest(HSR_SYNC);   
		}    
		catch (CException* e)   
		{   
			e->GetErrorMessage(szError, MAX_PATH);   
			e->Delete();   
			AfxMessageBox(szError);   
			bSuccess = false;   
		}   

		pHTTP->Close();   
		delete pHTTP;   
		pHTTP = NULL;

		fTrack.Close();   

		if (NULL != pBuffer)   
		{   
			free(pBuffer);   
			pBuffer = NULL;
		}  
	}

	StopThread(eTypeUpload);
	if (bSuccess)
	{
		//AfxMessageBox(L"上传成功");
		::PostMessage(m_hParent, WM_UPLOAD_THREAD_FINISH, m_nUploadFileType, eLoadSucceed);
	}else
	{
		::PostMessage(m_hParent, WM_UPLOAD_THREAD_FINISH, m_nUploadFileType, eLoadFailed);
	}

	return bSuccess;   
}

bool CUpAndDownLoad::Download()
{
	ASSERT(m_hParent != NULL);
	ASSERT(m_strDownloadServerUrl.Compare(L"") != 0);
	ASSERT(m_strDownloadLocalUrl.Compare(L"") != 0);
	
	if (m_bDownloading)
	{
		return false;
	}
	m_bDownloading = true;

	CInternetSession session;
	CHttpConnection* pHttpConnection = NULL;
	CHttpFile* pHttpFile = NULL;
	CString strServer, strObject;
	INTERNET_PORT wPort;

	DWORD dwType;
	const int nTimeOut =  2000;
	session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, nTimeOut);//重试之间的等待延时
	session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 3);//重试次数
	char* pszBuffer = NULL;

	try
	{
		AfxParseURL(m_strDownloadServerUrl, dwType, strServer, strObject, wPort);
		pHttpConnection = session.GetHttpConnection(strServer, wPort);
		pHttpFile = pHttpConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObject);
		//HRESULT hRet = URLDownloadToFile(NULL, m_strDownloadServerUrl, m_strDownloadLocalUrl, 0, 0);
		if (pHttpFile->SendRequest() == FALSE)
		//if (hRet != S_OK)
		{
			StopThread(eTypeDownload);
			::PostMessage(m_hParent, WM_DOWNLOAD_THREAD_FINISH, m_nDownloadFileType, eLoadFailed);
			return false;
		}

		DWORD dwStateCode;
		pHttpFile->QueryInfoStatusCode(dwStateCode);
		if (dwStateCode == HTTP_STATUS_OK)
		{
			HANDLE hFile = CreateFile(m_strDownloadLocalUrl, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, 
				CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE)
			{
				pHttpFile->Close();
				pHttpConnection->Close();
				session.Close();
				StopThread(eTypeDownload);
				::PostMessage(m_hParent, WM_DOWNLOAD_THREAD_FINISH, m_nDownloadFileType, eLoadFailed);
				return false;
			}

			char szInfoBuffer[1000];//返回消息
			DWORD dwFileSize = 0;//文件长度

			DWORD dwInfoBufferSize = sizeof(szInfoBuffer);
			BOOL bResult = FALSE;
			bResult = pHttpFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, (void*)szInfoBuffer, &dwInfoBufferSize, NULL);
			dwFileSize = atoi(szInfoBuffer);

			const int BUFFER_LENGTH = 1024 * 10;
			pszBuffer = new char[BUFFER_LENGTH];
			DWORD dwWrite, dwTotalWrite;
			dwWrite = dwTotalWrite = 0;
			UINT nRead = pHttpFile->Read(pszBuffer, BUFFER_LENGTH);//读取服务器上的数据
			while(nRead > 0)
			{
				WriteFile(hFile, pszBuffer, nRead, &dwWrite, NULL);//写到本地文件
				dwTotalWrite += dwWrite;
				nRead = pHttpFile->Read(pszBuffer, BUFFER_LENGTH);
			}
			delete[] pszBuffer;
			pszBuffer = NULL;
			CloseHandle(hFile);
		}else
		{
			CString strTemp;
			strTemp.Format(L"Download failed! Error = %d", dwStateCode);
			string strError = (CStringA)strTemp;
			KX_LOG(strError);
			delete[] pszBuffer;
			pszBuffer = NULL;
			if (pHttpFile != NULL)
			{
				pHttpFile->Close();
				delete pHttpFile;
				pHttpFile = NULL;
			}
			if (pHttpConnection != NULL)
			{
				pHttpConnection->Close();
				delete pHttpConnection;
				pHttpConnection = NULL;
			}
			session.Close();
			StopThread(eTypeDownload);
			::PostMessage(m_hParent, WM_DOWNLOAD_THREAD_FINISH, m_nDownloadFileType, eLoadFailed);
			return false;
		}
	}
	catch(...)
	{
		delete[] pszBuffer;
		pszBuffer = NULL;
		if (pHttpFile != NULL)
		{
			pHttpFile->Close();
			delete pHttpFile;
			pHttpFile = NULL;
		}
		if (pHttpConnection != NULL)
		{
			pHttpConnection->Close();
			delete pHttpConnection;
			pHttpConnection = NULL;
		}
		session.Close();
		StopThread(eTypeDownload);
		::PostMessage(m_hParent, WM_DOWNLOAD_THREAD_FINISH, m_nDownloadFileType, eLoadFailed);
		return false;
	}

	if (pHttpFile != NULL)
	{
		pHttpFile->Close();
	}
	if (pHttpConnection != NULL)
	{
		pHttpConnection->Close();
	}
	session.Close();

	//AfxMessageBox(L"下载成功");
	StopThread(eTypeDownload);
	::PostMessage(m_hParent, WM_DOWNLOAD_THREAD_FINISH, m_nDownloadFileType, eLoadSucceed);

	return true;
}

void CUpAndDownLoad::DownloadFile(const HWND hParent, const CString& strUrlOnServer, const CString& strUrlOnLocal, int eType )
{
	ASSERT(hParent != NULL);
	ASSERT(strUrlOnServer != _T(""));
	ASSERT(strUrlOnLocal != _T(""));
	if (strUrlOnServer == _T("") || strUrlOnLocal == _T("") || hParent == NULL)
	{
		return;
	}
	
	m_hParent = hParent;
	m_strDownloadLocalUrl = strUrlOnLocal;
	m_strDownloadServerUrl = strUrlOnServer;
	m_nDownloadFileType = eType;

	m_hDownloadThread = CreateThread(NULL, 0, ThreadDownloadProc, this, 0, NULL);
}

void CUpAndDownLoad::UploadFile(const HWND hParent, const CString& strUrlOnServer, const CString& strUrlOnLocal, int eType )
{
	ASSERT(hParent != NULL);
	ASSERT(strUrlOnServer != _T(""));
	ASSERT(strUrlOnLocal != _T(""));
	if (strUrlOnServer == _T("") || strUrlOnLocal == _T("") || hParent == NULL)
	{
		return;
	}

	m_hParent = hParent;
	m_strUploadServerUrl = strUrlOnServer;
	m_strUploadLocalUrl = strUrlOnLocal;
	m_nUploadFileType = eType;

	m_hUploadThread = CreateThread(NULL, 0, ThreadUploadProc, this, 0, NULL);
}

void CUpAndDownLoad::UploadFile( const HWND hParent, const CString& strUrlOnServer, std::vector<CString>& vUrlOnLocal, int eType )
{
	ASSERT(hParent != NULL);
	ASSERT(strUrlOnServer != _T(""));
	if (strUrlOnServer == _T("") || vUrlOnLocal.size() <=0 || hParent == NULL)
	{
		return;
	}

	m_hParent = hParent;
	m_strUploadServerUrl = strUrlOnServer;
	//m_strUploadLocalUrl = strUrlOnLocal;
	m_vStrUploadLocalUrl = vUrlOnLocal;
	m_nUploadFileType = eType;

	m_hUploadThread = CreateThread(NULL, 0, ThreadUploadProc, this, 0, NULL);
}

void CUpAndDownLoad::StopThread( int typeThead )
{
	if (typeThead == eTypeUpload)
	{
		if (m_hUploadThread)
		{
			//WaitForSingleObject(m_hUploadThread, INFINITE);
			CloseHandle(m_hUploadThread);
			m_hUploadThread = NULL;
			m_bUploading = false;
		}
	}else if (typeThead == eTypeDownload)
	{
		if (m_hDownloadThread)
		{
			//WaitForSingleObject(m_hDownloadThread, INFINITE);
			CloseHandle(m_hDownloadThread);
			m_hDownloadThread = NULL;
			m_bDownloading = false;
		}
	}
}

