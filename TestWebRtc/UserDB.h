#ifndef _USER_DB_H_
#define _USER_DB_H_


#include <mutex>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <openssl/sha.h>
#include <sstream>

class CUserDB
{
public:
	CUserDB();
	~CUserDB();

	int CreateTable();
	int RegisterUserId(std::string& unique_id, std::string& passwd, std::string& username, std::string& email, std::string& phone, std::string& address, uint64_t utc);
	int GetUserInfo(std::string& unique_id, std::string& username, std::string& email, std::string& phone, std::string& address);
	int DeleteUserId(std::string& unique_id);
	int CountUserId(std::string& id);
	int GetUserPasswd(std::string& id, std::string& passwd);
    int GetUserSalt(std::string& id, std::string& salt);
    std::string saltStr(std::string& id, const std::string pwd);
	std::string sha256(const std::string str);
    std::string RandomString();
	int ClearWrongPasswdCnt(std::string& id);
	int IncreaseWrongPasswdCnt(std::string& id);
	int GetWrongPasswdCnt(std::string& id);
	int UpdateWrongPasswdLockTime(std::string& id, uint64_t utc_time);
	uint64_t GetWrongPasswdLockTime(std::string& id);
	uint64_t GetPasswdUpdatedTime(std::string& id);
	int updateGOtp(std::string& id, std::string& otp_string);
	int CountGOtp(std::string& otp_string);
	int GetGOtpKey(std::string& id, std::string& otp_key);
    int UpdateEmail(std::string& id, std::string& new_email);

private:
	sql::Driver* driver;
	sql::Connection* con;
	std::mutex mMutex;
};



#endif