

#include "HttpStatusCode.h"
#include "MemoryDebug.h"


const char * GetReasonPhrase( int iSipCode )
{
	switch( iSipCode )
	{
	case HTTP_TRYING:											 return "Trying";
	case HTTP_SWITCHING_PROTOCOLS:         return "Switching Protocols";
	case HTTP_RINGING:                     return "Ringing";
	case HTTP_CALL_IS_BEING_FORWARDED:     return "Call Is Being Forwarded";
	case HTTP_QUEUED:                      return "Queued";
	case HTTP_SESSION_PROGRESS:            return "Session Progress";
	case HTTP_OK:                          return "OK";
	case HTTP_ACCEPTED:                    return "Accepted";
	case HTTP_MULTIPLE_CHOICES:            return "Multiple Choices";
	case HTTP_MOVED_PERMANENTLY:           return "Moved Permanently";
	case HTTP_MOVED_TEMPORARILY:           return "Moved Temporarily";
	case HTTP_USE_PROXY:                   return "Use Proxy";
	case HTTP_ALTERNATIVE_SERVICE:         return "Alternative Service";
	case HTTP_BAD_REQUEST:                 return "Bad Request";
	case HTTP_UNAUTHORIZED:                return "Unauthorized";
	case HTTP_PAYMENT_REQUIRED:            return "Payment Required";
	case HTTP_FORBIDDEN:                   return "Forbidden";
	case HTTP_NOT_FOUND:                   return "Not Found";
	case HTTP_METHOD_NOT_ALLOWED:          return "Method Not Allowed";
	case HTTP_NOT_ACCEPTABLE:							 return "Not Acceptable";
	case HTTP_PROXY_AUTHENTICATION_REQUIRED:	return "Proxy Authentication Required";
	case HTTP_REQUEST_TIME_OUT:            return "Request Timeout";
	case HTTP_CONFLICT:										 return "Conflict";
	case HTTP_GONE:                        return "Gone";
	case HTTP_LENGTH_REQUIRED:						 return "Length Required";
	case HTTP_CONDITIONAL_REQUEST_FAILED:	 return "Conditional Request Failed";
	case HTTP_REQUEST_ENTITY_TOO_LARGE:    return "Request Entity Too Large";
	case HTTP_REQUEST_URI_TOO_LARGE:       return "Request-URI Too Large";
	case HTTP_UNSUPPORTED_MEDIA_TYPE:      return "Unsupported Media Type";
	case HTTP_UNSUPPORTED_URI_SCHEME:      return "Unsupported Uri Scheme";
	case HTTP_BAD_EXTENSION:               return "Bad Extension";
	case HTTP_EXTENSION_REQUIRED:          return "Extension Required";
	case HTTP_SESSION_INTERVAL_TOO_SMALL:  return "Session Interval Too Small";
	case HTTP_INTERVAL_TOO_BRIEF:          return "Interval Too Brief";
	case HTTP_TEMPORARILY_UNAVAILABLE:     return "Temporarily not available";
	case HTTP_CALL_TRANSACTION_DOES_NOT_EXIST: return "Call Leg/Transaction Does Not Exist";
	case HTTP_LOOP_DETECTED:               return "Loop Detected";
	case HTTP_TOO_MANY_HOPS:               return "Too Many Hops";
	case HTTP_ADDRESS_INCOMPLETE:          return "Address Incomplete";
	case HTTP_AMBIGUOUS:                   return "Ambiguous";
	case HTTP_BUSY_HERE:                   return "Busy Here";
	case HTTP_REQUEST_TERMINATED:          return "Request Cancelled";
	case HTTP_NOT_ACCEPTABLE_HERE:         return "Not Acceptable Here";
	case HTTP_BAD_EVENT:                   return "Bad Event";
	case HTTP_REQUEST_PENDING:             return "Request Pending";
	case HTTP_UNDECIPHERABLE:              return "Undecipherable";
	case HTTP_INTERNAL_SERVER_ERROR:       return "Internal Server Error";
	case HTTP_NOT_IMPLEMENTED:             return "Not Implemented";
	case HTTP_BAD_GATEWAY:                 return "Bad Gateway";
	case HTTP_SERVICE_UNAVAILABLE:         return "Service Unavailable";
	case HTTP_SERVER_TIME_OUT:             return "Server Timeout";
	case HTTP_VERSION_NOT_SUPPORTED:       return "SIP Version not supported";
	case HTTP_MESSAGE_TOO_LARGE:           return "Message Too Large";
	case HTTP_BUSY_EVRYWHERE:              return "Busy Everywhere";
	case HTTP_DECLINE:                     return "Decline";
	case HTTP_DOES_NOT_EXIST_ANYWHERE:     return "Does not exist anywhere";
	case HTTP_CONNECT_ERROR:							 return "Connect error";
	case HTTP_RING_TIMEOUT_ERROR:					 return "Ring Timeout error";
	}

	return "Not Acceptable";
}
