

#ifndef _TLS_FUNCTION_H_
#define _TLS_FUNCTION_H_

#ifdef WIN32
#define USE_TLS_FREE
#endif

#include "SipTcp.h"
#include "openssl/rsa.h"
#include "openssl/crypto.h"
#include "openssl/x509.h"
#include "openssl/pem.h"
#include "openssl/ssl.h"
#include "openssl/err.h"

bool SSLServerStart( const char * szCertFile, const char * szCipherList );
bool SSLServerStop();
void SSLFinal();

bool SSLClientStart();
bool SSLClientStop();

SSL_CTX * SSLClientStart( const char * szCertFile );

bool SSLConnect( Socket iFd, SSL ** ppsttSsl );
bool SSLConnect( SSL_CTX * pCtx, Socket iFd, SSL ** ppsttSsl );
bool SSLAccept( Socket iFd, SSL ** ppsttSsl, bool bCheckClientCert, int iVerifyDepth, int iAcceptTimeout );
int SSLSend( SSL * ssl, const char * szBuf, int iBufLen );
int SSLRecv( SSL * ssl, char * szBuf, int iBufLen );
bool SSLClose( SSL * ssl );

void SSLServerSetHttp2();

#if 0
void SSLPrintLogServerCipherList();
void SSLPrintLogClientCipherList();
#endif

#endif
