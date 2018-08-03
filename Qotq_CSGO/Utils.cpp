#pragma once
#include "Utils.h"
void CUtils::Print ( const char* fmt , ... )
{
	if ( !fmt ) return;

	va_list va_alist;
	char logBuf [ 256 ] = { 0 };

	va_start ( va_alist , fmt );
	_vsnprintf ( logBuf + strlen ( logBuf ) , sizeof ( logBuf ) - strlen ( logBuf ) , fmt , va_alist );
	va_end ( va_alist );

	if ( logBuf [ 0 ] != '\0' )
	{
		printf ( " %s\n" , logBuf );
	}
}
std::uint8_t *CUtils::PatternScan ( void* module , const char* signature )
{
	static auto pattern_to_byte = [ ] ( const char* pattern ) {
		auto bytes = std::vector<int> {};
		auto start = const_cast< char* >( pattern );
		auto end = const_cast< char* >( pattern ) + strlen ( pattern );

		for ( auto current = start; current < end; ++current ) {
			if ( *current == '?' ) {
				++current;
				if ( *current == '?' )
					++current;
				bytes.push_back ( -1 );
			}
			else {
				bytes.push_back ( strtoul ( current , &current , 16 ) );
			}
		}
		return bytes;
	};

	auto dosHeader = ( PIMAGE_DOS_HEADER ) module;
	auto ntHeaders = ( PIMAGE_NT_HEADERS ) ( ( std::uint8_t* )module + dosHeader->e_lfanew );

	auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
	auto patternBytes = pattern_to_byte ( signature );
	auto scanBytes = reinterpret_cast< std::uint8_t* >( module );

	auto s = patternBytes.size ( );
	auto d = patternBytes.data ( );

	for ( auto i = 0ul; i < sizeOfImage - s; ++i ) {
		bool found = true;
		for ( auto j = 0ul; j < s; ++j ) {
			if ( scanBytes [ i + j ] != d [ j ] && d [ j ] != -1 ) {
				found = false;
				break;
			}
		}
		if ( found ) {
			return &scanBytes [ i ];
		}
	}

	//Afterwards call server to stop dispatch of cheat and to alert us of update.
	return nullptr;
}

DWORD CUtils::FindPatternIDA ( std::string moduleName , std::string pattern )
{
	const char* pat = pattern.c_str ( );
	DWORD firstMatch = 0;
	DWORD rangeStart = ( DWORD ) GetModuleHandleA ( moduleName.c_str ( ) );
	MODULEINFO miModInfo;
	K32GetModuleInformation ( GetCurrentProcess ( ) , ( HMODULE ) rangeStart , &miModInfo , sizeof ( MODULEINFO ) );
	DWORD rangeEnd = rangeStart + miModInfo.SizeOfImage;
	for ( DWORD pCur = rangeStart; pCur < rangeEnd; pCur++ )
	{
		if ( !*pat )
			return firstMatch;

		if ( *( PBYTE ) pat == '\?' || *( BYTE* ) pCur == getByte ( pat ) )
		{
			if ( !firstMatch )
				firstMatch = pCur;

			if ( !pat [ 2 ] )
				return firstMatch;

			if ( *( PWORD ) pat == '\?\?' || *( PBYTE ) pat != '\?' )
				pat += 3;

			else
				pat += 2; //one ?
		}
		else
		{
			pat = pattern.c_str ( );
			firstMatch = 0;
		}
	}

	Print ( "Bad pattern: '%s' in '%s'" , pattern , moduleName.c_str ( ) );
	return NULL;
}
