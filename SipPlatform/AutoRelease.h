

#ifndef _AUTO_RELEASE_H_
#define _AUTO_RELEASE_H_

#include <stdio.h>


template< class T_MAP, class T_DATA >
class CAutoRelease
{
	T_MAP		* m_pclsMap;

public:
	T_DATA	* m_pclsData;

	CAutoRelease( T_MAP & clsMap ) : m_pclsData(NULL)
	{
		m_pclsMap = &clsMap;
	}

	~CAutoRelease()
	{
		if( m_pclsData )
		{
			m_pclsMap->Release( m_pclsData );
		}
	}
};

#endif
