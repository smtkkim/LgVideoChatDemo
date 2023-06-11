#include "UserDB.h"

#define DB_DEBUG  1

// make table named tbl_videochat
#define SQL_CREATE_TBL		\
	" CREATE TABLE IF NOT EXISTS tbl_videochat( \n \
	id INTEGER PRIMARY KEY AUTO_INCREMENT, \n \
	unique_id  TEXT    NOT NULL, \n \
	passwd     TEXT    NOT NULL, \n \
	username   TEXT    NOT NULL, \n \
	email      TEXT    NOT NULL, \n \
	phone      TEXT    NOT NULL, \n \
	address    TEXT    NOT NULL \n \
	);"
/*
CREATE TABLE IF NOT EXISTS tbl_videochat (
id INTEGER PRIMARY KEY AUTO_INCREMENT,
unique_id  TEXT    NOT NULL,
passwd     TEXT    NOT NULL,
username   TEXT    NOT NULL,
email      TEXT    NOT NULL,
phone      TEXT    NOT NULL,
address    TEXT    NOT NULL
);

SELECT * FROM tbl_videochat;
*/

// register user info
#define SQL_INSERT_USER		"INSERT INTO tbl_videochat (unique_id, passwd, username, email, phone, address) VALUES (?, ?, ?, ?, ?, ?)"
/*
INSERT INTO tbl_videochat (unique_id, passwd, username, email, phone, address)
VALUES ('robin', 'lge1234', 'robin kim', 'robin.kim@lge.com', '01082916918', "Yangchun-gu Seoul")

INSERT INTO tbl_videochat (unique_id, passwd, username, email, phone, address)
VALUES ('alice', 'lge1234', 'alice lee', 'alice@lge.com', '01012345678', "Yongsan-gu Seoul")

INSERT INTO tbl_videochat (unique_id, passwd, username, email, phone, address)
VALUES ('bob', 'lge1234', 'bob park', 'bob@lge.com', '01012345678', "Gangnam-gu Seoul")

INSERT INTO tbl_videochat (unique_id, passwd, username, email, phone, address)
VALUES ('eve', 'lge1234', 'eve jeon', 'eve@lge.com', '01012345678', "Gangseo-gu Seoul")
*/

// update user info
#define SQL_UPDATE_USER		"UPDATE tbl_videochat SET passwd = ?, username = ?, email = ?, phone = ?, address = ? WHERE unique_id = ?"

// check if same unique_id registered
#define SQL_COUNT_USER		"SELECT COUNT(*) FROM tbl_videochat WHERE unique_id = ?"

// find the password for unique_id
#define SQL_USER_PASSWD		"SELECT passwd from tbl_videochat WHERE unique_id = ?"

// delete user
#define SQL_DELETE_USER		"DELETE FROM tbl_videochat WHERE unique_id = ?"
/*
DELETE FROM tbl_videochat WHERE unique_id = 'test'
*/




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

int CUserDB::RegisterUserId(std::string& unique_id, std::string& passwd, std::string& username, std::string& email, std::string& phone, std::string& address)
{
#if DB_DEBUG
	printf("+[%s]\n", __func__);
#endif

	std::lock_guard<std::mutex> guard(mMutex);

	sql::PreparedStatement* pstmt;
	sql::ResultSet* res;

	try
	{
		pstmt = con->prepareStatement(SQL_INSERT_USER);
		pstmt->setString(1, unique_id);
		pstmt->setString(2, passwd);
		pstmt->setString(3, username);
		pstmt->setString(4, email);
		pstmt->setString(5, phone);
		pstmt->setString(6, address);
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



