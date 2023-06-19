
#include <iostream>
#include <regex>
#include <random>
#include "WebRtcServer.h"
#include "HttpStatusCode.h"
#include "FileUtility.h"
#include "Directory.h"
#include "Log.h"
#include "UserMap.h"
#include "CallMap.h"
#include "MemoryDebug.h"

#include <openssl/hmac.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

extern CHttpStack gclsStack;

CWebRtcServer::CWebRtcServer() : m_bStop(false)
{
	m_clsUserDB = new CUserDB();
}

CWebRtcServer::~CWebRtcServer()
{
	delete m_clsUserDB;
}


bool CWebRtcServer::RecvHttpRequest( CHttpMessage * pclsRequest, CHttpMessage * pclsResponse )
{
	std::string strPath = m_strDocumentRoot;
	std::string strExt;

	//CLog::Print( LOG_DEBUG, "req uri[%s]", pclsRequest->m_strReqUri.c_str() );

	// 보안상 .. 을 포함한 URL 을 무시한다.
	if( strstr( pclsRequest->m_strReqUri.c_str(), ".." ) )
	{
		pclsResponse->m_iStatusCode = HTTP_NOT_FOUND;
		return true;
	}

#ifdef _DEBUG
	// 메모리 누수 검사를 위해서 exit.html 을 수신하면 프로그램을 종료한다.
	if( !strcmp( pclsRequest->m_strReqUri.c_str(), "/exit.html" ) )
	{
		pclsResponse->m_iStatusCode = HTTP_NOT_FOUND;
		m_bStop = true;
		return true;
	}
#endif

	if( !strcmp( pclsRequest->m_strReqUri.c_str(), "/" ) )
	{
		CDirectory::AppendName( strPath, "index.html" );
	}
	else
	{
#ifdef WIN32
		ReplaceString( pclsRequest->m_strReqUri, "/", "\\" );
#endif

		strPath.append( pclsRequest->m_strReqUri );
	}

	if( IsExistFile( strPath.c_str() ) == false )
	{
		pclsResponse->m_iStatusCode = HTTP_NOT_FOUND;
		return true;
	}

	// 파일별 Content-Type 을 설정한다.
	GetFileExt( strPath.c_str(), strExt );
	const char * pszExt = strExt.c_str();
	
	if( !strcmp( pszExt, "html" ) || !strcmp( pszExt, "htm" ) )
	{
		pclsResponse->m_strContentType = "text/html";
	}
	else if( !strcmp( pszExt, "css" ) )
	{
		pclsResponse->m_strContentType = "text/css";
	}
	else if( !strcmp( pszExt, "js" ) )
	{
		pclsResponse->m_strContentType = "text/javascript";
	}
	else if( !strcmp( pszExt, "png" ) || !strcmp( pszExt, "gif" ) )
	{
		pclsResponse->m_strContentType = "image/";
		pclsResponse->m_strContentType.append( pszExt );
	}
	else if( !strcmp( pszExt, "jpg" ) || !strcmp( pszExt, "jpeg" ) )
	{
		pclsResponse->m_strContentType = "image/jpeg";
	}
	else
	{
		pclsResponse->m_iStatusCode = HTTP_NOT_FOUND;
		return true;
	}

	// 파일을 읽어서 HTTP body 에 저장한다.
	FILE * fd = fopen( strPath.c_str(), "rb" );
	if( fd == NULL )
	{
		pclsResponse->m_iStatusCode = HTTP_NOT_FOUND;
		return true;
	}

	int n;
	char szBuf[8192];

	while( ( n = (int)fread( szBuf, 1, sizeof(szBuf), fd ) ) > 0 )
	{
		pclsResponse->m_strBody.append( szBuf, n );
	}

	fclose( fd );

	pclsResponse->m_iStatusCode = HTTP_OK;

	return true;
}


void CWebRtcServer::WebSocketConnected( const char * pszClientIp, int iClientPort, CHttpMessage * pclsRequest )
{
	printf( "WebSocket[%s:%d] connected\n", pszClientIp, iClientPort );
}


void CWebRtcServer::WebSocketClosed( const char * pszClientIp, int iClientPort )
{
	printf( "WebSocket[%s:%d] closed\n", pszClientIp, iClientPort );

	std::string strUserId;

	gclsUserMap.Delete( pszClientIp, iClientPort, strUserId );
	gclsCallMap.Delete( strUserId.c_str() );
}


bool CWebRtcServer::WebSocketData( const char * pszClientIp, int iClientPort, std::string & strData )
{
	printf( "WebSocket[%s:%d] recv[%s]\n", pszClientIp, iClientPort, strData.c_str() );

	STRING_VECTOR clsList;

	SplitString( strData.c_str(), clsList, '|' );

	int iCount = (int)clsList.size();

	if( iCount < 2 )
	{
		return false;
	}

	bool bReq = true;
	if( strcmp( clsList[0].c_str(), "req" ) ) bReq = false;

	const char * pszCommand = clsList[1].c_str();
	std::string strUserId;

	if (!strcmp(pszCommand, "check"))
	{
		if (iCount < 3)
		{
			printf("register request arg is not correct\n");
			return false;
		}

		std::string unique_id = clsList[2];

		if( unique_id.length() == 0)
		{
			printf("userid has not been entered");
			Send(pszClientIp, iClientPort, "res|check|410");
			return true;
		}

		int CountUserId = m_clsUserDB->CountUserId(unique_id);
		if ( CountUserId == 0 )   // check if id is alread registered
		{
			printf("available unique_id (%s)\n", unique_id.c_str());
			Send(pszClientIp, iClientPort, "res|check|200");
		}
		else if ( CountUserId >= 1 )   // check if id is alread registered
		{
			printf("same unique_id is already exist\n");
			Send(pszClientIp, iClientPort, "res|check|400");
		}
	}
	else if (!strcmp(pszCommand, "register"))
	{
		if (iCount < 8)
		{
			printf("register request arg is not correct\n");
			return false;
		}

		std::string unique_id = clsList[2];
		std::string passwd = clsList[3];
		std::string username = clsList[4];
		std::string email = clsList[5];
		std::string phone = clsList[6];
		std::string address = clsList[7];

		if( unique_id.length() == 0
			|| passwd.length() == 0
			|| username.length() == 0
			|| email.length() == 0)
		{
			printf("can not INSERT the user info to mysql");
			Send(pszClientIp, iClientPort, "res|register|410");
			return true;
		}

		if (m_clsUserDB->CountUserId(unique_id) >= 1 )   // check if id is alread registered
		{
			printf("same unique_id is already exist\n");
			Send(pszClientIp, iClientPort, "res|register|400");
			return true;
		}

    	if (validatePassword(passwd) == false ) 
		{
        	printf("password format is not valid\n");
			Send(pszClientIp, iClientPort, "res|register|420");
			return true;
		}

		uint64_t utc_time;
		getTimeUtc(&utc_time);

		if (m_clsUserDB->RegisterUserId(unique_id, passwd, username, email, phone, address, utc_time) == 0)
		{
			std::string google_totp_key;
			int count_key = 0;

			do {
				// generate Google TOTP key
				google_totp_key = generateKey(32);

				// check if same TOTP key is exist
				count_key = m_clsUserDB->CountGOtp(google_totp_key);
			} while (count_key != 0);

			// save key to database
			m_clsUserDB->updateGOtp(unique_id, google_totp_key);

			printf("user is correctly registered (%s)", google_totp_key.c_str());
			Send(pszClientIp, iClientPort, "res|register|200|%s", google_totp_key.c_str());
		}
		else
		{
			printf("can not INSERT the user info to mysql");
			Send(pszClientIp, iClientPort, "res|register|410");
			return true;
		}
	}
	else if( !strcmp( pszCommand, "login" ) )
	{
		if( iCount < 5 )
		{
			printf( "login request arg is not correct\n" );
			return false;
		}

		// passwd check
		std::string user_id = clsList[2];
		std::string otp_key;
		std::string db_user_passwd;

		if (m_clsUserDB->CountUserId(user_id) == 1)	// check if id is registered
		{
			uint64_t utc_time_now;
			getTimeUtc(&utc_time_now);

			if (m_clsUserDB->GetWrongPasswdLockTime(user_id) > utc_time_now)	// check if password lock time
			{
				// reject caused password lock time 
				Send(pszClientIp, iClientPort, "res|login|420");

				return true;
			}

			if (m_clsUserDB->GetUserPasswd(user_id, db_user_passwd) == 0)	// get the passwd
			{
				std::string entered_passwd = m_clsUserDB->sha256(m_clsUserDB->saltStr(user_id, clsList[3]));

				if (!strcmp(entered_passwd.c_str(), db_user_passwd.c_str()))
				{
					// passwd OK
					printf("password is correct\n");
					m_clsUserDB->ClearWrongPasswdCnt(user_id);
					m_clsUserDB->UpdateWrongPasswdLockTime(user_id, 0);

					// TOTP check
					std::string entered_otp = clsList[4];
					m_clsUserDB->GetGOtpKey(user_id, otp_key);

					int totp = generateTOTP(otp_key);
					printf("otp : %d\n", totp);

					if (std::stoi(entered_otp) != totp)
					{
						// OTP is wrong
						printf("OTP num is wrong(%d, %d)\n", std::stoi(entered_otp), totp);
						Send(pszClientIp, iClientPort, "res|login|440");
						return true;
					}

                    // check password updated time
                    time_t last_updated_time = m_clsUserDB->GetPasswdUpdatedTime(user_id);
                    if (NeedPasswdUpdate((time_t)utc_time_now, last_updated_time))
                    {
                        printf("Password is outdated and  needs to be changed.\n");
                        Send(pszClientIp, iClientPort, "res|login|430");
                        return true;
                    }
				}
				else
				{
					printf("password is wrong\n");
					m_clsUserDB->IncreaseWrongPasswdCnt(user_id);

					// check if passwd was wrong 3 times
					if (m_clsUserDB->GetWrongPasswdCnt(user_id) >= 3)
					{
						// too many times passwd wrong and update lock time
						m_clsUserDB->UpdateWrongPasswdLockTime(user_id, utc_time_now + 60);
					}

					Send(pszClientIp, iClientPort, "res|login|400");



					return true;
				}
			}
			else
			{
				printf("can not get the passwd from mysql\n");
				Send(pszClientIp, iClientPort, "res|login|410");
				return true;
			}
		}
		else
		{
			printf("Unregisterd user\n");
			Send(pszClientIp, iClientPort, "res|login|300");
			return true;
		}


		if( gclsUserMap.Insert( clsList[2].c_str(), pszClientIp, iClientPort ) == false )
		{
			Send( pszClientIp, iClientPort, "res|login|500" );
		}
		else
		{
			Send( pszClientIp, iClientPort, "res|login|200" );
		}
	}
	else if( !strcmp( pszCommand, "invite" ) )
	{
		if( bReq )
		{
			if( iCount < 4 )
			{
				printf( "invite request arg is not correct\n" );
				return false;
			}

			const char * pszToId = clsList[2].c_str();
			const char * pszSdp = clsList[3].c_str();

			CUserInfo clsToUser;
			std::string strUserId;

			if( gclsUserMap.SelectUserId( pszClientIp, iClientPort, strUserId ) == false )
			{
				Send( pszClientIp, iClientPort, "res|invite|403" );
			}
			else if( gclsUserMap.Select( pszToId, clsToUser ) == false )
			{
				Send( pszClientIp, iClientPort, "res|invite|404" );
			}
			else if( gclsCallMap.Insert( strUserId.c_str(), pszToId ) == false )
			{
				Send( pszClientIp, iClientPort, "res|invite|500" );
			}
			else
			{
				if( Send( clsToUser.m_strIp.c_str(), clsToUser.m_iPort, "req|invite|%s|%s", strUserId.c_str(), pszSdp ) == false )
				{
					Send( pszClientIp, iClientPort, "res|invite|500" );
				}
				else
				{
					Send( pszClientIp, iClientPort, "res|invite|180" );
				}
			}
		}
		else
		{
			if( iCount < 3 )
			{
				printf( "invite response arg is not correct\n" );
				return false;
			}

			int iStatus = atoi( clsList[2].c_str() );

			SendCall( pszClientIp, iClientPort, strData, strUserId );

			if( iStatus > 200 )
			{
				gclsCallMap.Delete( strUserId.c_str() );
			}
		}
	}
	else if (!strcmp(pszCommand, "contact"))
	{
		// send all contact list
		std::string strUserAllId;
		std::string strContactList;
		int iUserCount = gclsUserMap.GetSize();
		int ret = gclsUserMap.GetAllUserId(strUserAllId);

		strContactList = "res|contact|" + std::to_string(iUserCount) + "|" + strUserAllId;

		Send(pszClientIp, iClientPort, strContactList.c_str());
	}
	else if (!strcmp(pszCommand, "changeacc"))
	{
		if (iCount < 4)
		{
			printf("change account request arg is not correct\n");
			return false;
		}

		std::string unique_id = clsList[2];
		std::string email = clsList[3];

		if( unique_id.length() == 0
			|| email.length() == 0)
		{
			printf("can not INSERT the user info to mysql");
			Send(pszClientIp, iClientPort, "res|register|410");
			return true;
		}

		if (m_clsUserDB->UpdateEmail(unique_id, email) == 0)
		{
			printf("Email is updated (%s)", email.c_str());
			Send(pszClientIp, iClientPort, "res|register|200|%s", email.c_str());
		}
		else
		{
			printf("can not INSERT the user info to mysql");
			Send(pszClientIp, iClientPort, "res|register|410");
			return true;
		}
	}
	else if (!strcmp(pszCommand, "userinfo"))
	{
		std::string strUserInfo;
		std::string username; 
		std::string email; 
		std::string phone; 
		std::string address;

		if (iCount < 3)
		{
			printf("userinfo request arg is not correct\n");
			return false;
		}

		m_clsUserDB->GetUserInfo(clsList[2], username, email, phone, address);

		strUserInfo = "res|userinfo|" + clsList[2] + "|" + username + "|" + email + "|" + phone + "|" + address;

		Send(pszClientIp, iClientPort, strUserInfo.c_str());
	}
	else if( !strcmp( pszCommand, "bye" ) )
	{
		SendCall( pszClientIp, iClientPort, strData, strUserId );

		gclsCallMap.Delete( strUserId.c_str() );
	}

	return true;
}


bool CWebRtcServer::Send( const char * pszClientIp, int iClientPort, const char * fmt, ... )
{
	va_list	ap;
	char		szBuf[8192];
	int			iBufLen;

	va_start( ap, fmt );
	iBufLen = vsnprintf( szBuf, sizeof(szBuf)-1, fmt, ap );
	va_end( ap );

	if( gclsStack.SendWebSocketPacket( pszClientIp, iClientPort, szBuf, iBufLen ) )
	{
		printf( "WebSocket[%s:%d] send[%s]\n", pszClientIp, iClientPort, szBuf );
		return true;
	}

	return false;
}


bool CWebRtcServer::SendCall( const char * pszClientIp, int iClientPort, std::string & strData, std::string & strUserId )
{
	std::string strOtherId;
	CUserInfo clsOtherInfo;

	if( gclsUserMap.SelectUserId( pszClientIp, iClientPort, strUserId ) == false )
	{
		printf( "gclsUserMap.SelectUserId(%s:%d) error\n", pszClientIp, iClientPort );
		return false;
	}
	else if( gclsCallMap.Select( strUserId.c_str(), strOtherId ) == false )
	{
		printf( "gclsCallMap.Select(%s) error\n", strUserId.c_str() );
		return false;
	}
	else if( gclsUserMap.Select( strOtherId.c_str(), clsOtherInfo ) == false )
	{
		printf( "gclsUserMap.Select(%s) error\n", strOtherId.c_str() );
		return false;
	}

	return Send( clsOtherInfo.m_strIp.c_str(), clsOtherInfo.m_iPort, "%s", strData.c_str() );
}


//https://www.epochconverter.com/
int CWebRtcServer::getTimeUtc(uint64_t* time_utc)
{
	time_t _time;

	_time = time(&_time);
	*time_utc = (uint64_t)_time;

	return 0;
}

bool CWebRtcServer::NeedPasswdUpdate(time_t last_time, time_t current_time) {
    time_t diff = std::abs(last_time - current_time);

    // 30d * 24h * 60m * 60s
    return diff >= (30 * 24 * 60 * 60);
}

bool CWebRtcServer::validatePassword(const std::string& password) {
    // 최소 10자 이상의 길이를 가지는지 검사
    if (password.length() < 10) {
        return false;
    }

    // 문자, 특수문자, 숫자를 포함하는지 검사
    std::regex letterRegex("[a-zA-Z]");
    std::regex specialCharRegex("[!@#$%^&*-_=+]");
    std::regex digitRegex("[0-9]");

    if (!std::regex_search(password, letterRegex)) {
        return false;
    }

    if (!std::regex_search(password, specialCharRegex)) {
        return false;
    }

    if (!std::regex_search(password, digitRegex)) {
        return false;
    }

    return true;
}

std::string CWebRtcServer::generateKey(const int len)
{
	// 랜덤으로 선택할 수 있는 문자
	const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

	std::random_device rd;
	std::mt19937 generator(rd());

	std::uniform_int_distribution<> distribution(0, chars.size() - 1);
	std::string key;

	for (int i = 0; i < len; ++i) {
		key += chars[distribution(generator)];
	}

	return key;
}

// Generate TOTP
int CWebRtcServer::generateTOTP(const std::string& secret) 
{
	std::string decoded_secret = base32_decode(secret);
	long long timestamp = std::time(nullptr) / 30;

	unsigned char msg[8];
	for (int i = 7; i >= 0; i--) {
		msg[i] = static_cast<unsigned char>(timestamp);
		timestamp >>= 8;
	}

	unsigned char hash[EVP_MAX_MD_SIZE];
	unsigned int hash_len;
	HMAC(EVP_sha1(), decoded_secret.c_str(), decoded_secret.length(), msg, 8, hash, &hash_len);

	int offset = hash[hash_len - 1] & 0xf;
	int otp = (hash[offset + 0] & 0x7f) << 24 |
		(hash[offset + 1] & 0xff) << 16 |
		(hash[offset + 2] & 0xff) << 8 |
		(hash[offset + 3] & 0xff);
	otp %= 1000000;

	return otp;
}


std::string CWebRtcServer::base32_decode(const std::string& base32) 
{
	std::vector<unsigned char> bytes;
	int buffer = 0, bits = 0;

	for (char c : base32) {
		int val = base32_decode_char(c);
		if (val != -1) {
			buffer <<= 5;
			buffer |= val;
			bits += 5;

			if (bits >= 8) {
				bytes.push_back(static_cast<unsigned char>(buffer >> (bits - 8)));
				bits -= 8;
			}
		}
		else {
			// Invalid character
			return {};
		}
	}

	return std::string(bytes.begin(), bytes.end());
}

int CWebRtcServer::base32_decode_char(char c) 
{
	if (c >= 'A' && c <= 'Z')
		return c - 'A';
	else if (c >= '2' && c <= '7')
		return c - '2' + 26;
	else
		return -1; // Invalid character
}
