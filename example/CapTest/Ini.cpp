// Ini.cpp: implementation of the CIni class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Ini.h"

LPCTSTR AppPath(LPTSTR tcPath, LPCTSTR fName)
{
	if(!tcPath) 
		return tcPath;
	tcPath[0] = 0;
	int nLen = GetModuleFileName( NULL, tcPath, MAX_PATH );
	while(nLen)
	{
		if(tcPath[nLen] == _T('\\'))
		{
			if(fName)
				_tcscpy(tcPath+nLen+1, fName);
			else
				tcPath[nLen]=0;
			break;
		}
		nLen--;
	}
	if(!nLen && fName)
		_tcscpy(tcPath, fName);
	return tcPath;
}

String AppPath(LPCTSTR fName)
{
	TCHAR tcPath[MAX_PATH] = {0};
	AppPath(tcPath, fName);
	return tcPath;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIni::CIni(LPCTSTR path)
{
	cfgpath[0] = 0;
	if(path) _tcscpy(cfgpath, path);
	if(!_tcschr(cfgpath,_T('\\'))||_tcschr(cfgpath,_T('/')))
		AppPath(cfgpath, path);
}

CIni::~CIni()
{

}

/**
@brief Int配置项获取

@param sec Sec名
@param key 键名
@param def 缺省值
@return Int值
*/
int CIni::GetInt(LPCTSTR sec, LPCTSTR key, int def)
{
	TCHAR ret[MAX_PATH] = {0};
	GetPrivateProfileString(sec, key, _T(""), ret, sizeof ret, cfgpath);
	if(!ret[0])
	{//返回为空字符串
		_stprintf_s(ret, MAX_PATH, _T("%d"), def);
		WritePrivateProfileString(sec, key, ret, cfgpath);
		return def;
	}
	return _ttoi(ret);
}

/**
@brief Int配置项获取

@param sec Sec名
@param key 键名
@param value 值
@return Int值
*/
BOOL CIni::SetInt(LPCTSTR sec,LPCTSTR key,int value)
{
	//CString szValue; szValue.Format(_T("%d"), value);
	TCHAR szValue[20]={0};
	_stprintf_s(szValue, 20, _T("%d"), value);
	return WritePrivateProfileString(sec, key, szValue, cfgpath);
}

/**
@brief Str配置项获取

@param sec Sec名
@param key 键名
@param def 缺省值
@return Str值
*/
String CIni::GetStr(LPCTSTR sec, LPCTSTR key, LPCTSTR def)
{
	TCHAR ret[MAX_PATH] = {0};
	GetPrivateProfileString(sec, key, _T(""), ret, sizeof ret, cfgpath);
	if(ret[0])
		return ret;
	else if(def && def[0])
		WritePrivateProfileString(sec, key, def, cfgpath);
	return def;
}

/**
@brief Str配置项设置

@param sec Sec名
@param key 键名
@param value 缺省值
@return Str值
*/
BOOL CIni::SetStr(LPCTSTR sec, LPCTSTR key, LPCTSTR value)
{
	return WritePrivateProfileString(sec, key, value, cfgpath);
}

static CIni gIni(_T("config.ini"));
int GetIniInt(LPCTSTR sec, LPCTSTR key, int def)
{
	return gIni.GetInt(sec,key, def);
}

String GetIniStr(LPCTSTR sec, LPCTSTR key, LPCTSTR def)
{
	return gIni.GetStr(sec,key,def);
}

BOOL SetIniInt(LPCTSTR sec, LPCTSTR key, int value)
{
	return gIni.SetInt(sec, key, value);
}

BOOL SetIniStr(LPCTSTR sec, LPCTSTR key, LPCTSTR value)
{
	return gIni.SetStr(sec, key, value);
}

BOOL SetIniFloat(LPCTSTR sec, LPCTSTR key, float value) {
	char szFloat[32] = { 0 }; sprintf(szFloat, "%f", value);
	return gIni.SetStr(sec, key, szFloat);
}
float GetIniFloat(LPCTSTR sec, LPCTSTR key, float def) {
	char szFloat[32] = { 0 }; sprintf(szFloat, "%f", def);
	String ret = gIni.GetStr(sec, key, szFloat);
	return atof(ret.c_str());
}
