
#include <iostream>
#include <iomanip>
#include <sstream>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include "UserDB.h"

#define DB_DEBUG			0
#define DB_SHA256_PASSWD	1
#define DB_OTP_ENCRYPTION	1
#define DB_SALT_LEN         16

// make table named tbl_videochat
#define SQL_CREATE_TBL		\
	" CREATE TABLE IF NOT EXISTS tbl_videochat( \n \
	id INTEGER PRIMARY KEY AUTO_INCREMENT, \n \
	unique_id  TEXT    NOT NULL, \n \
	passwd     TEXT    NOT NULL, \n \
	salt       TEXT    NOT NULL, \n \
	username   TEXT    NOT NULL, \n \
	email      TEXT    NOT NULL, \n \
	phone      TEXT    NOT NULL, \n \
	address    TEXT    NOT NULL, \n \
	passwd_update_utc INTEGER,   \n \
	passwd_wrong_cnt  INTEGER DEFAULT 0,  \n \
	passwd_lock_utc   INTEGER DEFAULT 0,  \n \
	gotp       TEXT              \n \
	);"


// register user info
#define SQL_INSERT_USER		"INSERT INTO tbl_videochat (unique_id, passwd, username, email, phone, address, passwd_update_utc, salt) VALUES (?, ?, ?, ?, ?, ?, ?, ?)"


// update user info
#if DB_OTP_ENCRYPTION
#define SQL_UPDATE_GOTP		"UPDATE tbl_videochat SET gotp = TO_BASE64(AES_ENCRYPT(?, ?)) WHERE unique_id = ?"
#else
#define SQL_UPDATE_GOTP		"UPDATE tbl_videochat SET gotp = ? WHERE unique_id = ?"
#endif

#define SQL_COUNT_GOTP		"SELECT COUNT(*) FROM tbl_videochat WHERE gotp = ?"

#if DB_OTP_ENCRYPTION
#define SQL_GOTP_KEY        "SELECT CAST(AES_DECRYPT(FROM_BASE64(gotp), ?) AS CHAR) AS decrypted_gotp FROM tbl_videochat WHERE unique_id = ?"
#else
#define SQL_GOTP_KEY        "SELECT gotp FROM tbl_videochat WHERE unique_id = ?"
#endif

#define SQL_USER_INFO		"SELECT unique_id, username, email, phone, address FROM tbl_videochat WHERE unique_id = ?"

// update user info
#define SQL_UPDATE_USER		"UPDATE tbl_videochat SET passwd = ?, username = ?, email = ?, phone = ?, address = ? WHERE unique_id = ?"

// check if same unique_id registered
#define SQL_COUNT_USER		"SELECT COUNT(*) FROM tbl_videochat WHERE unique_id = ?"

// find the password for unique_id
#define SQL_USER_PASSWD		"SELECT passwd FROM tbl_videochat WHERE unique_id = ?"

// find the salt for password for unique_id
#define SQL_USER_SALT		"SELECT salt FROM tbl_videochat WHERE unique_id = ?"

// delete user
#define SQL_DELETE_USER		"DELETE FROM tbl_videochat WHERE unique_id = ?"


#define SQL_UPDATE_PASSWD	"UPDATE tbl_videochat SET passwd = ? WHERE unique_id = ?"



#define SQL_WRONG_PASSWD_CNT_CLEAR		"UPDATE tbl_videochat SET passwd_wrong_cnt = 0 WHERE unique_id = ?"

#define SQL_WRONG_PASSWD_INC			"UPDATE tbl_videochat SET passwd_wrong_cnt = passwd_wrong_cnt + 1 WHERE unique_id = ?"

#define SQL_WRONG_PASSWD_CNT			"SELECT passwd_wrong_cnt FROM tbl_videochat WHERE unique_id = ?"

#define SQL_WRONG_PASSWD_LOCK_UPDATE	"UPDATE tbl_videochat SET passwd_lock_utc = ? WHERE unique_id = ?"

#define SQL_WRONG_PASSWD_LOCK_UTC		"SELECT passwd_lock_utc FROM tbl_videochat WHERE unique_id = ?"


#define SQL_UPDATE_PASSWD_UPDATED_TIME	"UPDATE tbl_videochat SET passwd_update_utc = ? WHERE unique_id = ?"

#define SQL_GET_PASSWD_UPDATED_TIME		"SELECT passwd_update_utc FROM tbl_videochat WHERE unique_id = ?"

#define SQL_UPDATE_EMAIL		"UPDATE tbl_videochat SET email = ? WHERE unique_id = ?"

const std::string aes_key = "lg_video_chat";


CUserDB::CUserDB()
{
	try
	{
		driver = get_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3306", "root", "lge1234");
		con->setSchema("lge");
	}
	catch (sql::SQLException& e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

		exit(0);
	}

	CreateTable();


	// method test code
	//std::string passwd;
	//CountUserId(std::string("robin"));
	//GetUserPasswd(std::string("robin"), passwd);
	//RegisterUserId(std::string("test"), std::string("lge1234"), std::string("test kim"), std::string("test@lge.com"), std::string("01012345678"), std::string("Seocho-gu Seoul"));
	//DeleteUserId(std::string("test"));
	//std::string username; std::string email; std::string phone; std::string address;
	//GetUserInfo(std::string("robin"), username, email, phone, address);
	//printf("robin : %s : %s : %s : %s", username.c_str(), email.c_str(), phone.c_str(), address.c_str());

	// https://www.convertstring.com/ko/Hash/SHA256
	//std::string sha256_passwd = sha256(std::string("lge1234") + salt);
	//printf("sha256:%s\n", sha256_passwd.c_str());	// 29e586cd7f3164b3b0448c2953eb7f052ea474bbcd771bbd8820872da9581d56

}

CUserDB::~CUserDB()
{
	delete con;
}


int CUserDB::CreateTable()
{
#if DB_DEBUG
	printf("+[%s]\n", __func__);
#endif

	std::lock_guard<std::mutex> guard(mMutex);

	sql::PreparedStatement* pstmt;
	sql::ResultSet* res;

	try
	{
		pstmt = con->prepareStatement(SQL_CREATE_TBL);
		res = pstmt->executeQuery();

		if (res->next()) {
		}
	}
	catch (sql::SQLException& e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

		return -1;
	}

	delete res;
	delete pstmt;

#if DB_DEBUG
	printf("-[%s]\n", __func__);
#endif

	return 0;
}

int CUserDB::RegisterUserId(std::string& unique_id, std::string& passwd, std::string& username, std::string& email, std::string& phone, std::string& address, uint64_t utc)
{
#if DB_DEBUG
	printf("+[%s]\n", __func__);
#endif

	std::lock_guard<std::mutex> guard(mMutex);

	sql::PreparedStatement* pstmt;
	sql::ResultSet* res;

#if DB_SHA256_PASSWD
    std::string salt = RandomString();
	std::string sha256_passwd = sha256(passwd + salt);
#if DB_DEBUG
	printf("%s:%s\n", passwd.c_str(), sha256_passwd.c_str());
#endif
#endif

	try
	{
		pstmt = con->prepareStatement(SQL_INSERT_USER);
		pstmt->setString(1, unique_id);
#if DB_SHA256_PASSWD
		pstmt->setString(2, sha256_passwd);
#else
		pstmt->setString(2, passwd);
#endif
		pstmt->setString(3, username);
		pstmt->setString(4, email);
		pstmt->setString(5, phone);
		pstmt->setString(6, address);
		pstmt->setUInt64(7, utc);
		pstmt->setString(8, salt);

		res = pstmt->executeQuery();

		if (res->next()) {
		}
	}
	catch (sql::SQLException& e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

		return -1;
	}

	delete res;
	delete pstmt;

#if DB_DEBUG
	printf("-[%s]\n", __func__);
#endif
	return 0;
}


int CUserDB::GetUserInfo(std::string& unique_id, std::string& username, std::string& email, std::string& phone, std::string& address)
{
#if DB_DEBUG
	printf("+[%s]\n", __func__);
#endif

	std::lock_guard<std::mutex> guard(mMutex);

	sql::PreparedStatement* pstmt;
	sql::ResultSet* res;

	try
	{
		pstmt = con->prepareStatement(SQL_USER_INFO);
		pstmt->setString(1, unique_id);

		res = pstmt->executeQuery();

		if (res->next()) {
			username = res->getString("username");
			email = res->getString("email");
			phone = res->getString("phone");
			address = res->getString("address");
		}
	}
	catch (sql::SQLException& e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

		return -1;
	}

	delete res;
	delete pstmt;

#if DB_DEBUG
	printf("-[%s]\n", __func__);
#endif
	return 0;
}


int CUserDB::DeleteUserId(std::string& unique_id)
{
#if DB_DEBUG
	printf("+[%s]\n", __func__);
#endif

	std::lock_guard<std::mutex> guard(mMutex);

	sql::PreparedStatement* pstmt;
	sql::ResultSet* res;

	try
	{
		pstmt = con->prepareStatement(SQL_DELETE_USER);
		pstmt->setString(1, unique_id);
		res = pstmt->executeQuery();

		if (res->next()) {
		}
	}
	catch (sql::SQLException& e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

		return -1;
	}

	delete res;
	delete pstmt;

#if DB_DEBUG
	printf("-[%s]\n", __func__);
#endif

	return 0;
}


int CUserDB::CountUserId(std::string& id)
{
#if DB_DEBUG
	printf("+[%s]\n", __func__);
#endif

	std::lock_guard<std::mutex> guard(mMutex);

	int count = 0;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* res;

	try
	{
		pstmt = con->prepareStatement(SQL_COUNT_USER);
		pstmt->setString(1, id);
		res = pstmt->executeQuery();

		if (res->next()) {
			count = res->getInt(1);
		}
	}
	catch (sql::SQLException& e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

		return -1;
	}

	delete res;
	delete pstmt;

#if DB_DEBUG
	printf("DB[CountUserId]:(%d)\n", count);
	printf("-[%s]\n", __func__);
#endif

	return count;
}



int CUserDB::GetUserPasswd(std::string& id, std::string& passwd)
{
#if DB_DEBUG
	printf("+[%s]\n", __func__);
#endif

	std::lock_guard<std::mutex> guard(mMutex);

	sql::PreparedStatement* pstmt;
	sql::ResultSet* res;

	try
	{
		pstmt = con->prepareStatement(SQL_USER_PASSWD);
		pstmt->setString(1, id);
		res = pstmt->executeQuery();

		if (res->next()) {
			passwd = res->getString("passwd");
		}
	}
	catch (sql::SQLException& e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

		return -1;
	}

	delete res;
	delete pstmt;

#if DB_DEBUG
	printf("DB[GetUserPasswd]:(%s)\n", passwd.c_str());
	printf("-[%s]\n", __func__);
#endif

	return 0;
}


int CUserDB::GetUserSalt(std::string& id, std::string& salt)
{
#if DB_DEBUG
	printf("+[%s]\n", __func__);
#endif

	std::lock_guard<std::mutex> guard(mMutex);

	sql::PreparedStatement* pstmt;
	sql::ResultSet* res;

	try
	{
		pstmt = con->prepareStatement(SQL_USER_SALT);
		pstmt->setString(1, id);
		res = pstmt->executeQuery();

		if (res->next()) {
			salt = res->getString("salt");
		}
	}
	catch (sql::SQLException& e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

		return -1;
	}

	delete res;
	delete pstmt;

#if DB_DEBUG
	printf("DB[GetUserSalt]:(%s)\n", salt.c_str());
	printf("-[%s]\n", __func__);
#endif

	return 0;
}

// load salt from DB and combine with the given string
std::string CUserDB::saltStr(std::string& id, const std::string pwd)
{
    std::string salt;
    if ((this->GetUserSalt(id, salt)) == 0) {
        return pwd + salt;
    }
    return nullptr;
}

std::string CUserDB::sha256(const std::string str)
{
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, str.c_str(), str.size());
	SHA256_Final(hash, &sha256);
	std::stringstream ss;
	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
	}
	return ss.str();
}

std::string CUserDB::RandomString()
{
    const std::string chars("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

    unsigned char random_bytes[DB_SALT_LEN];
    if (RAND_bytes(random_bytes, DB_SALT_LEN) != 1) {
        printf("[%s] RAND_bytes failed\n", __func__);
        return "_cmu_videochat";
    }

    std::string result;
    for (std::size_t i = 0; i < DB_SALT_LEN; ++i) {
        result.push_back(chars[random_bytes[i] % chars.size()]);
    }

    return result;
}

int CUserDB::ClearWrongPasswdCnt(std::string& id)
{
#if DB_DEBUG
	printf("+[%s]\n", __func__);
#endif

	std::lock_guard<std::mutex> guard(mMutex);

	int count = 0;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* res;

	try
	{
		pstmt = con->prepareStatement(SQL_WRONG_PASSWD_CNT_CLEAR);
		pstmt->setString(1, id);
		res = pstmt->executeQuery();

		if (res->next()) {
		}
	}
	catch (sql::SQLException& e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

		return -1;
	}

	delete res;
	delete pstmt;

#if DB_DEBUG
	printf("-[%s]\n", __func__);
#endif

	return count;
}

int CUserDB::IncreaseWrongPasswdCnt(std::string& id)
{
#if DB_DEBUG
	printf("+[%s]\n", __func__);
#endif

	std::lock_guard<std::mutex> guard(mMutex);

	int count = 0;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* res;

	try
	{
		pstmt = con->prepareStatement(SQL_WRONG_PASSWD_INC);
		pstmt->setString(1, id);
		res = pstmt->executeQuery();

		if (res->next()) {
		}
	}
	catch (sql::SQLException& e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

		return -1;
	}

	delete res;
	delete pstmt;

#if DB_DEBUG
	printf("-[%s]\n", __func__);
#endif

	return count;
}


int CUserDB::GetWrongPasswdCnt(std::string& id)
{
#if DB_DEBUG
	printf("+[%s]\n", __func__);
#endif

	std::lock_guard<std::mutex> guard(mMutex);

	int count = 0;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* res;

	try
	{
		pstmt = con->prepareStatement(SQL_WRONG_PASSWD_CNT);
		pstmt->setString(1, id);
		res = pstmt->executeQuery();

		if (res->next()) {
			count = res->getInt(1);
		}
	}
	catch (sql::SQLException& e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

		return -1;
	}

	delete res;
	delete pstmt;

#if DB_DEBUG
	printf("-[%s]\n", __func__);
#endif

	return count;
}



int CUserDB::UpdateWrongPasswdLockTime(std::string& id, uint64_t utc_time)
{
#if DB_DEBUG
	printf("+[%s]\n", __func__);
#endif

	std::lock_guard<std::mutex> guard(mMutex);

	int count = 0;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* res;

	try
	{
		pstmt = con->prepareStatement(SQL_WRONG_PASSWD_LOCK_UPDATE);
		pstmt->setUInt64(1, utc_time);
		pstmt->setString(2, id);
		res = pstmt->executeQuery();

		if (res->next()) {
		}
	}
	catch (sql::SQLException& e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

		return -1;
	}

	delete res;
	delete pstmt;

#if DB_DEBUG
	printf("-[%s]\n", __func__);
#endif

    // TBD: why do we return count here?
	return count;
}



uint64_t CUserDB::GetWrongPasswdLockTime(std::string& id)
{
#if DB_DEBUG
	printf("+[%s]\n", __func__);
#endif

	std::lock_guard<std::mutex> guard(mMutex);

	uint64_t utc_time = 0;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* res;

	try
	{
		pstmt = con->prepareStatement(SQL_WRONG_PASSWD_LOCK_UTC);
		pstmt->setString(1, id);
		res = pstmt->executeQuery();

		if (res->next()) {
			utc_time = res->getUInt64(1);
		}
	}
	catch (sql::SQLException& e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

		return -1;
	}

	delete res;
	delete pstmt;

#if DB_DEBUG
	printf("-[%s]\n", __func__);
#endif

	return utc_time;
}



uint64_t CUserDB::GetPasswdUpdatedTime(std::string& id)
{
#if DB_DEBUG
	printf("+[%s]\n", __func__);
#endif

	std::lock_guard<std::mutex> guard(mMutex);

	uint64_t utc_time = 0;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* res;

	try
	{
		pstmt = con->prepareStatement(SQL_GET_PASSWD_UPDATED_TIME);
		pstmt->setString(1, id);
		res = pstmt->executeQuery();

		if (res->next()) {
			utc_time = res->getUInt64(1);
		}
	}
	catch (sql::SQLException& e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

		return -1;
	}

	delete res;
	delete pstmt;

#if DB_DEBUG
	printf("-[%s]\n", __func__);
#endif

	return utc_time;
}


int CUserDB::updateGOtp(std::string& id, std::string& otp_string)
{
#if DB_DEBUG
	printf("+[%s]\n", __func__);
#endif

	std::lock_guard<std::mutex> guard(mMutex);

	sql::PreparedStatement* pstmt;
	sql::ResultSet* res;

	try
	{
		pstmt = con->prepareStatement(SQL_UPDATE_GOTP);
#if DB_OTP_ENCRYPTION
		pstmt->setString(1, otp_string);
		pstmt->setString(2, aes_key);
		pstmt->setString(3, id);
#else
		pstmt->setString(1, otp_string);
		pstmt->setString(2, id);
#endif
		res = pstmt->executeQuery();

		if (res->next()) {
		}
	}
	catch (sql::SQLException& e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

		return -1;
	}

	delete res;
	delete pstmt;

#if DB_DEBUG
	printf("-[%s]\n", __func__);
#endif

	return 0;
}


int CUserDB::CountGOtp(std::string& otp_string)
{
#if DB_DEBUG
	printf("+[%s]\n", __func__);
#endif

	std::lock_guard<std::mutex> guard(mMutex);

	int count = 0;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* res;

	try
	{
		pstmt = con->prepareStatement(SQL_COUNT_GOTP);
		pstmt->setString(1, otp_string);
		res = pstmt->executeQuery();

		if (res->next()) {
			count = res->getInt(1);
		}
	}
	catch (sql::SQLException& e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

		return -1;
	}

	delete res;
	delete pstmt;

#if DB_DEBUG
	printf("DB[CountGOtp]:(%d)\n", count);
	printf("-[%s]\n", __func__);
#endif

	return count;
}

int CUserDB::GetGOtpKey(std::string& id, std::string& otp_key)
{
#if DB_DEBUG
	printf("+[%s]\n", __func__);
#endif

	std::lock_guard<std::mutex> guard(mMutex);

	sql::PreparedStatement* pstmt;
	sql::ResultSet* res;

	try
	{
		pstmt = con->prepareStatement(SQL_GOTP_KEY);
#if DB_OTP_ENCRYPTION
		pstmt->setString(1, aes_key);
		pstmt->setString(2, id);
#else
		pstmt->setString(1, id);
#endif
		res = pstmt->executeQuery();

		if (res->next()) {
#if DB_OTP_ENCRYPTION
			otp_key = res->getString("decrypted_gotp");
#else
			otp_key = res->getString("gotp");
#endif
		}
	}
	catch (sql::SQLException& e)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

		return -1;
	}

	delete res;
	delete pstmt;

#if DB_DEBUG
	printf("DB[GetGOtpKey]:(%s)\n", otp_key.c_str());
	printf("-[%s]\n", __func__);
#endif

	return 0;
}


int CUserDB::UpdateEmail(std::string& id, std::string& new_email)
{
#if DB_DEBUG
        printf("+[%s]\n", __func__);
#endif
    
        std::lock_guard<std::mutex> guard(mMutex);
    
        sql::PreparedStatement* pstmt;
        sql::ResultSet* res;
    
        try
        {
            pstmt = con->prepareStatement(SQL_UPDATE_EMAIL);
            pstmt->setString(1, new_email);
            pstmt->setString(2, id);
            res = pstmt->executeQuery();
    
            if (res->next()) {
            }
        }
        catch (sql::SQLException& e)
        {
            std::cout << "# ERR: SQLException in " << __FILE__;
            std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
            std::cout << "# ERR: " << e.what();
            std::cout << " (MySQL error code: " << e.getErrorCode();
            std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    
            return -1;
        }
    
        delete res;
        delete pstmt;
    
#if DB_DEBUG
        printf("-[%s]\n", __func__);
#endif
    
        return 0;
}

