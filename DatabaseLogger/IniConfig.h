#pragma once
class CIniConfig
{
public:
	CIniConfig(const char* szIniPath);
	~CIniConfig();

	void WriteInt(const char* szApp, const char* szKey, int nValue);
	int ReadInt(const char* szApp, const char* szKey, int nDefault);

	void WriteString(const char* szApp, const char* szKey, const char* szValue);
	void ReadString(const char* szApp, const char* szKey, char* szOut, const char* szDefault);

	void WriteDouble(const char* szApp, const char* szKey, double dbValue);
	double ReadDouble(const char* szApp, const char* szKey, double dbDefault);

private:
	char m_szIniPath[MAX_PATH];
};

