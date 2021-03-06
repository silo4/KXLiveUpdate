#include "StdAfx.h"
#include "KXLog.h"
#include <direct.h>
#include <time.h>
#include <algorithm>

#define _MAX_LOGLINE		   250000
//#define KXLOG_FILECOUNT_NAME   "KXLogFileCount.xml"
//#define KXLOG_DIR_NAME		   "KXLogDirName.xml"
#define KXLIVE_LOG_DIR         L"\\log\\"
#define KX_LOG_PRE_NAME		   L"KXLiveUpdate_"
//#define _MAX_DIR_COUNT         5  

CKXLog* CKXLog::s_kxLog = NULL;

wstring CKXLog::GetTodayDate()
{
	SYSTEMTIME st = {0};
	GetLocalTime(&st);
	wstring strFormat, strTemp;
	strTemp += L"%d";
	strTemp += (st.wMonth > 9 ? L"%d" : L"0%d");
	strTemp += (st.wDay > 9 ? L"%d" : L"0%d");

	WCHAR date[128];
	//sprintf(date, strTemp.data(), st.wYear, st.wMonth, st.wDay);
	swprintf_s(date, strTemp.data(), st.wYear, st.wMonth, st.wDay);
	wstring today(date);
	
	return today;
}

wstring CKXLog::GetNowTime()
{
	SYSTEMTIME st = {0};
	GetLocalTime(&st);
	WCHAR time[128];
	//sprintf(time, "%d:%d:%d",st.wHour, st.wMinute, st.wSecond);
	swprintf_s(time, L"%d:%d:%d",st.wHour, st.wMinute, st.wSecond);

	wstring now(time);

	return now;
}

CKXLog::CKXLog() :m_line_count(0)
{
	wstring str_exepath;
#ifdef WIN32
	wstring date = KXLIVE_LOG_DIR/* + GetTodayDate()*/; 
	create_tracedir(date.data(), get_fullexepath(str_exepath));
#else
	create_tracedir("/log", get_fullexepath(str_exepath));
#endif

//
//	char buffer[MAX_PATH]= {0};
//#ifdef WIN32
//	char *path = _getcwd(buffer, MAX_PATH);
//#else
//	char* path = getcwd(buffer, MAX_PATH);
//#endif
	
#ifdef WIN32
	m_strFileName = str_exepath + date + L"\\";
#else
	m_strFileName +=  "/log/";
#endif
	m_strFileName += KX_LOG_PRE_NAME;
	//m_strFileName +=".log";
	//m_file_count = get_current_file_count();

	init_trace_file();

	set_not_today_log();
}

CKXLog::~CKXLog()
{
	if (m_of_file.is_open())
		m_of_file.close();

	/*char newFile[1024] = {0};
	sprintf(newFile, "%s%s.log", m_strFileName.data(), GetTodayDate().data());
	if(m_strFileName.compare(newFile) != 0)
	{
		rename(m_strFileName.data(), newFile);
	}*/
	m_vDirname.clear();
	m_vFilename.clear();
	//save_current_file_count();
}

void CKXLog::write_log(const string& str_log)
{
	if (m_of_file.is_open())
	{
		m_line_count ++;
		if ((m_line_count > _MAX_LOGLINE) || (m_strCurFileDate.compare(GetTodayDate()) != 0) )
		{
			init_trace_file();
		}
		string strDate = ws2s(GetTodayDate());
		string strTime = ws2s(GetNowTime());
		m_of_file << "[" << strDate << "--" << strTime << "]";

		m_of_file << str_log;
		m_of_file << "\n";
		m_of_file.flush();
	}
}


void CKXLog::init_trace_file()
{	
	WCHAR newFile[1024] = {0};

//	sprintf(newFile, "%s%s.log", m_strFileName.data(), GetTodayDate().data());
	swprintf_s(newFile, L"%s%s.log", m_strFileName.data(), GetTodayDate().data());
	
	m_strCurFileDate = GetTodayDate();

	if (m_of_file.is_open())
	{
		m_of_file.close();
	}
	
	//rename(m_strFileName.data(), newFile);
	_wrename(m_strFileName.data(), newFile);

	m_of_file.open(newFile, /*std::ios_base::trunc|*/std::ios_base::out|std::ios_base::app);

	m_line_count = 0;

}

const wchar_t* CKXLog::get_fullexepath(wstring& _str_path) const
{
	WCHAR buffer[MAX_PATH]= {0};
#ifdef WIN32
	//char *path = _getcwd(buffer, MAX_PATH);//谁启用本应用程序，就会返回启用者的目录
	//获取自己所在目录
	CString path; 
	GetModuleFileName(NULL, path.GetBufferSetLength(MAX_PATH+1),MAX_PATH);
	path.ReleaseBuffer(); 
	int pos = path.ReverseFind('\\'); 
	path = path.Left(pos); 
	pos = path.ReverseFind('\\');
	path = path.Left(pos); 
#else
	char* path = getcwd(buffer, MAX_PATH);
#endif
	//_str_path = CStringA(path);
	_str_path = path.GetBuffer(0);

	return _str_path.c_str();
}

bool CKXLog::create_tracedir(const wchar_t* _pdir, const wchar_t* _pparent) const
{
	if (_pparent == NULL || wcslen(_pparent) == 0)
		return false;

	wstring str_dir = _pparent;
	str_dir += _pdir;
#ifdef WIN32
	//_mkdir(str_dir.c_str());
	//wstring wStrDir = s2ws(str_dir);
	_wmkdir(str_dir.c_str());
#else 
	mkdir(str_dir.c_str(), S_IRWXU);
#endif

	return true;
}

void CKXLog::set_not_today_log()
{
	CFileFind tempFind;
	CString strTempFileFind, strTheTempDirName;
	wstring strDirName; 
	get_fullexepath(strDirName);
	strDirName += KXLIVE_LOG_DIR;
	strTheTempDirName = strDirName.c_str();
	strTempFileFind.Format(L"%s\\*.*", strTheTempDirName);

	BOOL IsFinded = tempFind.FindFile(strTempFileFind);

	while (IsFinded)
	{
		IsFinded = tempFind.FindNextFile();
		if (!tempFind.IsDots()) 
		{
			CString strFoundFileName, strTempFindName;
			strTempFindName = tempFind.GetFileName();
			strFoundFileName.Format(L"%s%s", strTheTempDirName, strTempFindName);
			if (!tempFind.IsDirectory())
			{
				CString strTemp, strFileName;
				wstring strTodayLogName = GetTodayDate() + L".log";
				strTemp = strTodayLogName.c_str();
				strFileName =  strTempFindName;
				int nIndex = strFileName.Find('_');
				strFileName = strFileName.Right(strFileName.GetLength() - nIndex - 1);
				if (strFileName.Compare(strTemp) != 0)
				{
					m_vFilename.push_back(strFoundFileName);
				}
			}
		}
	}

	tempFind.Close();
}

void CKXLog::delete_log()
{
	for (std::vector<CString>::iterator iter = m_vFilename.begin(); iter != m_vFilename.end(); iter++)
	{
		DeleteFile(*iter);
	}
}

std::wstring CKXLog::s2ws( string& strSrc) const
{
	LPCSTR pszSrc = strSrc.c_str();
	int nLen = strSrc.size();

	int nSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, 0, 0);

	if(nSize <= 0) 
		return NULL;

	WCHAR *pwszDst = new WCHAR[nSize+1];

	if( NULL == pwszDst)
		return NULL;

	MultiByteToWideChar(CP_ACP, 0,(LPCSTR)pszSrc, nLen, pwszDst, nSize);
	pwszDst[nSize] = 0;

	if( pwszDst[0] == 0xFEFF) // skip Oxfeff
	{
		for(int i = 0; i < nSize; i ++) 
		{
			pwszDst[i] = pwszDst[i+1]; 
		}
	}

	wstring wcharString(pwszDst);

	delete pwszDst;
	pwszDst = NULL;

	return wcharString;
}


std::string CKXLog::ws2s(wstring& ws) const
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, ws.c_str(), -1, NULL, 0, NULL, NULL);
	if (nLen<= 0)
	{
		return string("");
	}

	char* pszDst = new char[nLen];

	if (NULL == pszDst) 
	{
		return string("");
	}

	WideCharToMultiByte(CP_ACP, 0, ws.c_str(), -1, pszDst, nLen, NULL, NULL);

	pszDst[nLen -1] = 0;

	string strTemp(pszDst);
	delete [] pszDst;
	return strTemp;
}

/*

void CKXLog::save_current_file_count()
{
	string str_exepath;
	get_fullexepath(str_exepath);
	str_exepath += KXLIVE_LOG_DIR + GetTodayDate() + "\\"; 
	string strPath = str_exepath + KXLOG_FILECOUNT_NAME;
	CString strFile;
	strFile = strPath.c_str();
	CMarkup xml;
	if (xml.Load(strFile))
	{
		if(xml.FindChildElem(L"item"))
		{
			xml.IntoElem();
			xml.FindChildElem(L"filecount");
			xml.SetChildData(m_file_count);
			xml.OutOfElem();
		}
	}else
	{
		xml.SetDoc(L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
		xml.AddElem(L"KXLog");
		xml.IntoElem();
		xml.AddElem(L"item");
		xml.IntoElem();
		xml.AddElem(L"filecount");
		CString strTemp;
		strTemp.Format(L"%d", m_file_count);
		xml.SetElemContent(strTemp);
		xml.OutOfElem();
	}

	xml.Save(strFile);
}

int CKXLog::get_current_file_count()
{
	string str_exepath;
	get_fullexepath(str_exepath);
	str_exepath += KXLIVE_LOG_DIR + GetTodayDate() + "\\"; 
	string strPath = str_exepath + KXLOG_FILECOUNT_NAME;
	CString strFile;
	strFile = strPath.c_str();
	CMarkup xml;
	int nFileCount = 0;
	if (xml.Load(strFile))
	{
		if(xml.FindChildElem(L"item"))
		{
			xml.IntoElem();
			xml.FindChildElem(L"filecount");
			CString strFc = xml.GetChildData();
			nFileCount = _ttoi(strFc);
			xml.OutOfElem();
		}
	}

	return nFileCount;
}

void CKXLog::save_current_log_dir()
{
	GetAllDirnames();
	//删除最老的一个目录
	bool bSuc = delete_oldest_log_dir();

	string str_exepath;
	get_fullexepath(str_exepath);
	str_exepath += KXLIVE_LOG_DIR; 
	string strPath = str_exepath + KXLOG_DIR_NAME;
	CString strFile;
	strFile = strPath.c_str();
	CMarkup xml;
	if (xml.Load(strFile))
	{
		if (bSuc)//目录删除成功，说明XML需要先删除最老结点，最顶部
		{
			if(xml.FindChildElem(L"item"))
			{
				xml.IntoElem();
				xml.FindChildElem(L"dirname");
				xml.RemoveChildElem();
				xml.OutOfElem();
			}
		}
		//插入最新结点，在最底部
		vector<string>::iterator iter = find( m_vDirname.begin( ), m_vDirname.end( ), GetTodayDate());
		//如果XML已有最新结点就不用再插入了
		if (iter == m_vDirname.end())
		{
			xml.ResetMainPos();
			if(xml.FindChildElem(L"item"))
			{
				xml.IntoElem();
				CString strTemp;
				strTemp = GetTodayDate().c_str();
				xml.AddChildElem(L"dirname", strTemp);
				xml.OutOfElem();
			}
		}
	}else
	{
		xml.SetDoc(L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
		xml.AddElem(L"KXLog");
		xml.IntoElem();
		xml.AddElem(L"item");
		xml.IntoElem();
		xml.AddElem(L"dirname");
		CString strTemp;
		strTemp = GetTodayDate().c_str();
		xml.SetElemContent(strTemp);
		xml.OutOfElem();
	}

	xml.Save(strFile);
}

bool CKXLog::delete_oldest_log_dir()
{
	if (m_vDirname.size() < _MAX_DIR_COUNT)
	{
		return false;
	}

	return DeleteDir(m_vDirname[0]);
}

bool CKXLog::DeleteDir(string strDir)//删除该目录及目录下所有文件
{
	if (strDir.compare("") == 0)
	{
		return false;
	}
	string strDirName;
	get_fullexepath(strDirName);
	strDirName += KXLIVE_LOG_DIR + strDir; 
	
	CFileFind tempFind;
	//char strTempFileFind[MAX_PATH];
	//sprintf(strTempFileFind,"%s\\*.*", strDirName);
	CString strTempFileFind, strTheTempDirName;
	strTheTempDirName = strDirName.c_str();
	strTempFileFind.Format(L"%s\\*.*", strTheTempDirName);

	BOOL IsFinded = tempFind.FindFile(strTempFileFind);

	while (IsFinded)
	{
		IsFinded = tempFind.FindNextFile();
		if (!tempFind.IsDots()) 
		{
			//char strFoundFileName[MAX_PATH];
			//strcpy(strFoundFileName, tempFind.GetFileName().GetBuffer(MAX_PATH));
			CString strFoundFileName;
			strFoundFileName.Format(L"%s", tempFind.GetFileName());
			if (tempFind.IsDirectory())
			{
				//char strTempDir[MAX_PATH];
				//sprintf(strTempDir,"%s\\%s", strDirName, strFoundFileName);
				CString strCstr;
				strCstr.Format(L"%s\\%s", strTheTempDirName, strFoundFileName);
				string strTempDir = CStringA(strCstr);
				DeleteDir(strTempDir);
			}
			else
			{
				//char strTempFileName[MAX_PATH];
				//sprintf(strTempFileName,"%s\\%s", strDirName, strFoundFileName);
				CString strCstr;
				strCstr.Format(L"%s\\%s", strTheTempDirName, strFoundFileName);
				//string strTempFileName = CStringA(strCstr);
				DeleteFile(strCstr);
			}
		}
	}

	tempFind.Close();

	if(!RemoveDirectory(strTheTempDirName))
	{
		return false;
	}

	return true;
}

void CKXLog::GetAllDirnames()
{
	string str_exepath;
	get_fullexepath(str_exepath);
	str_exepath += KXLIVE_LOG_DIR; 
	string strPath = str_exepath + KXLOG_DIR_NAME;
	CString strFile;
	strFile = strPath.c_str();
	CMarkup xml;
	bool bSuc = xml.Load(strFile);
	if (!bSuc)
	{
		return;
	}
	if (xml.FindChildElem(L"item"))
	{
		xml.IntoElem();
		while(xml.FindChildElem(L"dirname"))
		{
			CString strTemp = xml.GetChildData();
			string strDir = CStringA(strTemp);
			m_vDirname.push_back(strDir);
		}
		xml.OutOfElem();
	}

}

*/