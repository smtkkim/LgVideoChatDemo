#ifndef _USER_DB_H_
#define _USER_DB_H_


#include <mutex>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>



class CUserDB
{
public:
	CUserDB();
	~CUserDB();

	int CreateTable();
	int RegisterUserId(std::string& unique_id, std::string& passwd, std::string& username, std::string& email, std::string& phone, std::string& address);
	int DeleteUserId(std::string& unique_id);
	int CountUserId(std::string& id);
	int GetUserPasswd(std::string& id, std::string& passwd);

private:
	sql::Driver* driver;
	sql::Connection* con;
	std::mutex mMutex;
};



#endif