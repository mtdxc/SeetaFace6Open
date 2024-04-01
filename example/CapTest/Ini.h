// Ini.h: interface for the CIni class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INI_H__82D19E50_EEFD_4348_A098_5D16E166B8C5__INCLUDED_)
#define AFX_INI_H__82D19E50_EEFD_4348_A098_5D16E166B8C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define USE_CSTRING
#include <tchar.h>
#ifdef USE_CSTRING
#include <atlstr.h>
typedef CString String;
#else
#include <string>
typedef std::string String;
#endif 

String AppPath(LPCTSTR fName=NULL);
LPCTSTR AppPath(LPTSTR path, LPCTSTR fName);
class CIni  
{
public:
	CIni(LPCTSTR path);
	virtual ~CIni();

	int GetInt(LPCTSTR sec, LPCTSTR key, int def);
	String GetStr(LPCTSTR sec, LPCTSTR key, LPCTSTR def);
	BOOL SetInt(LPCTSTR sec, LPCTSTR key, int value);
	BOOL SetStr(LPCTSTR sec, LPCTSTR key, LPCTSTR value);
protected:
	TCHAR cfgpath[MAX_PATH];
};

int GetIniInt(LPCTSTR sec, LPCTSTR key, int def);
String GetIniStr(LPCTSTR sec, LPCTSTR key, LPCTSTR def);
BOOL SetIniInt(LPCTSTR sec, LPCTSTR key, int value);
BOOL SetIniStr(LPCTSTR sec, LPCTSTR key, LPCTSTR value);
BOOL SetIniFloat(LPCTSTR sec, LPCTSTR key, float value);
float GetIniFloat(LPCTSTR sec, LPCTSTR key, float def);
#endif // !defined(AFX_INI_H__82D19E50_EEFD_4348_A098_5D16E166B8C5__INCLUDED_)
