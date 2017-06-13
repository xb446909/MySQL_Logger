#pragma once

int __stdcall InitDatabase(const char* szIniPath);
int __stdcall CloseDatabase();
int __stdcall InsertLog(const char* szLog);