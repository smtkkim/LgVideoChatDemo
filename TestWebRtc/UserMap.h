

#ifndef _USER_MAP_H_
#define _USER_MAP_H_

#include "SipMutex.h"
#include <string>
#include <map>


class CUserInfo
{
public:
	std::string m_strIp;
	int					m_iPort;
};

// key = user id
typedef std::map< std::string, CUserInfo > USER_MAP;

// key = client ip + port
typedef std::map< std::string, std::string > USER_KEY_MAP;


class CUserMap
{
public:
	CUserMap();
	~CUserMap();

	bool Insert( const char * pszUserId, const char * pszIp, int iPort );
	bool Select( const char * pszUserId, CUserInfo & clsUserInfo );
	bool SelectUserId( const char * pszIp, int iPort, std::string & strUserId );
	bool Delete( const char * pszIp, int iPort, std::string & strUserId );
	int GetSize();
	bool GetAllUserId( std::string& id );

private:
	USER_MAP			m_clsMap;
	USER_KEY_MAP	m_clsKeyMap;
	CSipMutex			m_clsMutex;

	void GetKey( const char * pszIp, int iPort, std::string & strKey );
};

extern CUserMap gclsUserMap;

#endif
