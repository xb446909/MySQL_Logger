// DatabaseLogger.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <mysql.h>
#include "IniConfig.h"
#include <sstream>
#include "DatabaseLogger.h"

using namespace std;

#pragma comment(lib, "libmysql.lib")

#define SECTION_NAME		"MySQL Server"

int check_db(MYSQL *mysql, char *db_name);
int check_table(MYSQL* mysql, char *name);

MYSQL mysql;
char szConfigPath[MAX_PATH] = { 0 };

int __stdcall InitDatabase(const char* szIniPath)
{
	strcpy(szConfigPath, szIniPath);

	CIniConfig config(szConfigPath);

	if (!mysql_init(&mysql))
	{
		OutputDebugStringA("Initialized mysql failed!\r\n");
		return -1;
	}

	char szServerHost[512] = { 0 };
	int nPort = 0;
	char szUserName[512] = { 0 };
	char szPassword[512] = { 0 };

	config.ReadString(SECTION_NAME, "Server Host", szServerHost, "localhost");
	nPort = config.ReadInt(SECTION_NAME, "Server Port", 3306);
	config.ReadString(SECTION_NAME, "User Name", szUserName, "user");
	config.ReadString(SECTION_NAME, "Password", szPassword, "123");

	if (!mysql_real_connect(&mysql, szServerHost, szUserName, szPassword, NULL, nPort, NULL, 0))
	{
		stringstream ss;
		ss << "Failed to connect to database: Error: " << mysql_error(&mysql) << endl;
		OutputDebugStringA(ss.str().c_str());
		return -1;
	}

	if (!mysql_set_character_set(&mysql, "gb18030"))
	{
		stringstream ss;
		ss << "New client character set: " << mysql_character_set_name(&mysql) << endl;
		OutputDebugStringA(ss.str().c_str());
	}

	OutputDebugStringA("connected to database .....\n");

	char szDatabaseName[512] = { 0 };
	char szTableName[512] = { 0 };
	config.ReadString(SECTION_NAME, "Database", szDatabaseName, "HANS");
	config.ReadString(SECTION_NAME, "Table", szTableName, "log");

	for (int i = 0; szDatabaseName[i]; i++)
	{
		if (szDatabaseName[i] >= 'A'&&szDatabaseName[i] <= 'Z')
			szDatabaseName[i] += 32;
	}

	for (int i = 0; szTableName[i]; i++)
	{
		if (szTableName[i] >= 'A'&&szTableName[i] <= 'Z')
			szTableName[i] += 32;
	}

	int err = check_db(&mysql, szDatabaseName);
	if (err != 0)
	{
		OutputDebugStringA("create db is err!\n");
		mysql_close(&mysql);
		return -1;
	}

	if (mysql_select_db(&mysql, szDatabaseName)) //return 0 is success ,!0 is err  
	{
		stringstream ss;
		ss << "Failed to select database: Error: " << mysql_error(&mysql) << endl;
		OutputDebugStringA(ss.str().c_str());
		mysql_close(&mysql);
		return -1;
	}

	if ((err = check_table(&mysql, szTableName)) != 0)
	{
		OutputDebugStringA("check_tbl is err!\n");
		mysql_close(&mysql);
		return -1;
	}

	return 0;
}

int __stdcall CloseDatabase()
{
	mysql_close(&mysql);
	return 0;
}


int __stdcall InsertLog(const char * szLog)
{
	if (mysql_ping(&mysql))
	{
		stringstream ss;
		ss << "Failed to insert: Error: " << mysql_error(&mysql) << endl;
		OutputDebugStringA(ss.str().c_str());
		return -1;
	}

	CIniConfig config(szConfigPath);

	char szTableName[512] = { 0 };
	config.ReadString(SECTION_NAME, "Table", szTableName, "log");

	stringstream sql;
	sql << "INSERT INTO " << szTableName;
	sql << "(operation) VALUES('" << szLog << "');";
	int res = mysql_query(&mysql, sql.str().c_str());

	if (!res)
	{
		stringstream ss;
		ss << "Inserted " << (unsigned long)mysql_affected_rows(&mysql) << " rows." << endl;
		OutputDebugStringA(ss.str().c_str());
	}
	else
	{
		stringstream ss;
		ss << "Failed to insert: Error: " << mysql_error(&mysql) << endl;
		OutputDebugStringA(ss.str().c_str());
		return -1;
	}

	return 0;
}


int check_db(MYSQL *mysql, char *db_name)
{
	MYSQL_ROW row = NULL;
	MYSQL_RES *res = NULL;

	res = mysql_list_dbs(mysql, NULL);
	if (res)
	{
		while ((row = mysql_fetch_row(res)) != NULL)
		{
			if (strcmp(row[0], db_name) == 0)
			{
				break;
			}
		}
		//mysql_list_dbs会分配内存，需要使用mysql_free_result释放  
		mysql_free_result(res);
	}
	if (!row)  //没有这个数据库，则建立  
	{
		stringstream sql;
		sql << "CREATE DATABASE " << db_name << " CHARACTER SET 'gb18030' COLLATE 'gb18030_chinese_ci';";
		if (mysql_query(mysql, sql.str().c_str()))
		{
			stringstream ss;
			ss << "Failed to create database: Error: " << mysql_error(mysql) << endl;
			OutputDebugStringA(ss.str().c_str());
			return -1;
		}
	}
	return 0;
}


int check_table(MYSQL* mysql, char *name)
{
	if (name == NULL)
		return 0;
	MYSQL_ROW row = NULL;
	MYSQL_RES *res = NULL;
	res = mysql_list_tables(mysql, NULL);
	if (res)
	{
		while ((row = mysql_fetch_row(res)) != NULL)
		{
			if (strcmp(row[0], name) == 0)
			{
				break;
			}
		}
		mysql_free_result(res);
	}
	if (!row) //create table  
	{
		stringstream sql;
		sql << "CREATE TABLE " << name << " (operation_time DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, operation VARCHAR(10240))  CHARACTER SET 'gb18030' COLLATE 'gb18030_chinese_ci';";
		if (mysql_query(mysql, sql.str().c_str())) 
		{
			stringstream ss;
			ss << "Query failed: Error: " << mysql_error(mysql) << endl;
			OutputDebugStringA(ss.str().c_str());
		}
	}
	return 0;
}