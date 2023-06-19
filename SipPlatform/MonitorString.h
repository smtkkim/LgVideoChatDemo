

#ifndef _MONITOR_STRING_H_
#define _MONITOR_STRING_H_

// 자료구조 모니터링
#define MR_COL_SEP							"|"
#define MR_ROW_SEP							"\n"

#include <string>


class CMonitorString
{
public:
	CMonitorString();
	~CMonitorString();

	void AddCol( const char * pszValue );
	void AddCol( const std::string & strValue );
	void AddCol( const std::string & strIp, int iPort );
	void AddCol( int iValue );
	void AddCol( time_t iTime );

	void AddRow( const char * pszValue );
	void AddRow( const std::string & strValue );
	void AddRow( const std::string & strIp, int iPort );
	void AddRow( int iValue );
	void AddRow( time_t iTime );

	void Clear( );

	const char * GetString();
	int GetLength();

private:
	std::string m_strBuf;
};

#endif
