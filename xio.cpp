#include "xgui.h"

/*
 *  xDateTime
 */
CTime::CTime() : wxDateTime()
{
}

CTime::CTime(int year, int month, int day, int hour, int minute, int second) :
	wxDateTime(day, (Month)month, year, hour, minute, second, 0)
{
}

int CTime::GetDayOfWeek()
{
	return GetWeekDay();
}

CTime CTime::GetCurrentTime()
{
	wxDateTime dt = wxDateTime::Now();
	return *((CTime*)&dt);
}

COleDateTime::COleDateTime() : wxDateTime()
{
}

COleDateTime::COleDateTime(int year, int month, int day, int hour, int minute, int second) :
	wxDateTime(day, (Month)month, year, hour, minute, second, 0)
{
}

int COleDateTime::GetDayOfWeek()
{
	return GetWeekDay();
}

COleDateTime COleDateTime::GetCurrentTime()
{
	wxDateTime dt = wxDateTime::Now();
	return *((COleDateTime*)&dt);
}

/*
 *  xFile
 */
CFile::CFile()
{
}

CFile::CFile(const wxString& path, int mode)
{
	Open(path, mode);
}

BOOL CFile::Open(const wxString& path, int mode)
{
	m_strFileName = path;
	int wxmode = wxFile::read;

	if(mode & modeReadWrite)
		wxmode = wxFile::read_write;
	if(mode & modeWrite)
		wxmode = wxFile::read_write;

	if(mode & modeCreate)
	{
		wxFile::Create(path, true, wxS_IRUSR | wxS_IWUSR | wxS_IRGRP | wxS_IROTH);
		Close();
	}

	return wxFile::Open(path, (OpenMode)wxmode);
}

long CFile::SeekToEnd()
{
	return Seek(0, wxFromEnd);
}

long CFile::Seek(long off, unsigned int from)
{
	return wxFile::Seek((wxSeekMode)off, (wxSeekMode)from);
}

DWORD CFile::GetPosition() const
{
	return (DWORD)Tell();
}

unsigned int CFile::GetLength()
{
	return (unsigned int)Length();
}

wxString CFile::GetFilePath()
{
	wxFileName fn((wxString&)m_strFileName);
	return fn.GetFullName();
}

wxString CFile::GetFileName()
{
	wxFileName fn((wxString&)m_strFileName);
	return fn.GetName();
}

CFile::~CFile()
{
	Close();
}

void CFile::Remove(const wxString& fname)
{
	wxRemoveFile(fname);
}

void CFile::Rename(const wxString& fname, const wxString& newname)
{
	wxRenameFile(fname, newname);
}

BOOL CFile::GetStatus(const wxString& fname, CFileStatus& rStatus)
{
	wxFileName fn(fname);
	CTime ac, mod, cr;
	if(fn.GetTimes((wxDateTime*)&ac, (wxDateTime*)&mod, (wxDateTime*)&cr))
	{
		rStatus.m_atime = ac;
		rStatus.m_mtime = mod;
		rStatus.m_ctime = cr;
		return TRUE;
	}
	return FALSE;
}

BOOL CFile::SetStatus(const wxString& fname, const CFileStatus& rStatus)
{
	wxFileName fn(fname);
	if(fn.SetTimes((wxDateTime*)&rStatus.m_atime,
		(wxDateTime*)&rStatus.m_mtime,
		(wxDateTime*)&rStatus.m_ctime))
	{
		return TRUE;
	}
	return FALSE;
}
/*
 * xFileFind
 */
BOOL CFileFind::FindFile(const wxString& fname, DWORD)
{
	wxFileName fn(fname);

	m_strPath = fn.GetPath();
	BOOL ret = Open(m_strPath);
	if(!GetFirst(&m_prev_file, fn.GetFullName()))
		m_prev_file = wxEmptyString;
	else
		m_prev_file = m_strPath + wxFILE_SEP_PATH + m_prev_file;

	return ret;
}

BOOL CFileFind::FindNextFile()
{
	if(m_prev_file == wxEmptyString)
		return FALSE;

	m_strFile = m_prev_file;
	if(!GetNext(&m_prev_file))
		m_prev_file = wxEmptyString;
	else
		m_prev_file = m_strPath + wxFILE_SEP_PATH + m_prev_file;

	return TRUE;
}

BOOL CFileFind::GetLastWriteTime(CTime& dtm)
{
	wxFileName fn(m_strFile);
	bool ret = false;
	if(fn.IsOk())
	{
		ret = fn.GetTimes(0, &dtm, 0);
	}

	return (BOOL)ret;
}

wxString CFileFind::GetFileTitle()
{
	wxFileName fn(m_strFile);
	return fn.GetName();
}

wxString CFileFind::GetFilePath()
{
	return m_strFile;
}
