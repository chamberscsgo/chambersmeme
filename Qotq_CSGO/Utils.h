#pragma once
#include "SDK.h"

class CUtils {
public:
	void Print ( const char* fmt , ... );
	std::uint8_t * PatternScan ( void * module , const char * signature );
	DWORD FindPatternIDA ( std::string moduleName , std::string pattern );
}; extern CUtils* g_pUtils;