#include "stdafx.h"
#include "IniConfig.h"
#include <sstream>

using namespace std;


CIniConfig::CIniConfig(const char* szIniPath)
{
	strcpy(m_szIniPath, szIniPath);
}


CIniConfig::~CIniConfig()
{
}


void CIniConfig::WriteString(const char* szApp, const char* szKey, const char* szValue)
{
	WritePrivateProfileStringA(szApp, szKey, szValue, m_szIniPath);
}

void CIniConfig::ReadString(const char * szApp, const char * szKey, char * szOut, const char * szDefault)
{
	char szBuf[512] = { 0 };
	GetPrivateProfileStringA(szApp, szKey, szDefault, szBuf, 512, m_szIniPath);
	WriteString(szApp, szKey, szBuf);
	strcpy(szOut, szBuf);
}

void CIniConfig::WriteDouble(const char * szApp, const char * szKey, double dbValue)
{
	stringstream ss;
	ss << dbValue;
	WriteString(szApp, szKey, ss.str().c_str());
}

double CIniConfig::ReadDouble(const char * szApp, const char * szKey, double dbDefault)
{
	char szTemp[512] = { 0 };
	stringstream ss;
	ss << dbDefault;
	ReadString(szApp, szKey, szTemp, ss.str().c_str());
	return atof(szTemp);
}


void CIniConfig::WriteInt(const char * szApp, const char * szKey, int nValue)
{
	stringstream ss;
	ss << nValue;
	WriteString(szApp, szKey, ss.str().c_str());
}


int CIniConfig::ReadInt(const char * szApp, const char * szKey, int nDefault)
{
	int nRet = GetPrivateProfileIntA(szApp, szKey, nDefault, m_szIniPath);
	WriteInt(szApp, szKey, nRet);
	return nRet;
}
