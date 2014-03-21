#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class CKXLog
{
public:
	static CKXLog* GetKXLogInstance()
	{
		if(s_kxLog == NULL)
		{
			s_kxLog = new CKXLog;
		}
		return s_kxLog;
	}

	static void DestroyKXLog()
	{
		if (s_kxLog)
		{
			delete s_kxLog;
			s_kxLog = NULL;
		}
	}
	//写入日志
	void		write_log(const string& str_log);
	wstring		GetTodayDate();
	wstring		GetNowTime();
	std::vector<CString> GetNotTodayLog() {return m_vFilename;}
	void        delete_log();
protected:
	void		init_trace_file();
	const wchar_t* get_fullexepath(std::wstring& _str_path) const;
	bool		create_tracedir(const wchar_t* _pdir, const wchar_t* _pparent) const;
	void        set_not_today_log();
	wstring		s2ws(string&) const;
	string		ws2s(wstring&) const;
	/*void        save_current_file_count();
	int         get_current_file_count();
	void		save_current_log_dir();
	bool        delete_oldest_log_dir();
	bool        DeleteDir(string strDir);
	void        GetAllDirnames();*/
protected:
	CKXLog();
	~CKXLog();

	CKXLog& operator = (const CKXLog& _other);
	CKXLog(const CKXLog& _other);
	static CKXLog* s_kxLog;
private:
	ofstream		m_of_file;
	wstring			m_strFileName;
	wstring          m_strCurFileDate;//当前正在写的日志的所用的日期
	int				m_line_count;
	int				m_file_count;
	std::vector<string>  m_vDirname;
	std::vector<CString>  m_vFilename;
};


#define  KX_LOG(strLog)  \
	CKXLog::GetKXLogInstance()->write_log(strLog);

#define  KX_LOG_GET_NOT_TODAY_LOG()  \
	CKXLog::GetKXLogInstance()->GetNotTodayLog();

#define  KX_LOG_DELETE_LOG()    \
	CKXLog::GetKXLogInstance()->delete_log();