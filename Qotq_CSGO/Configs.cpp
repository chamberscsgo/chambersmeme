#include "SDK.h"
#include <winerror.h>
#pragma warning( disable : 4091)
#include <ShlObj.h>
#include "SDK.h"
#include <string>
#include <sstream>

//	replace later. paste for now cause fuck making configs every inject

void CConfig::Setup ( )
{
	//	Aim
	SetupValue ( g_Settings.aimbot.enabled , false , ( "aimbot" ) , ( "enabled" ) );
	SetupValue ( g_Settings.aimbot.headonly , false , ( "aimbot" ) , ( "headonly" ) );
	SetupValue ( g_Settings.aimbot.animfix , false , ( "aimbot" ) , ( "animfix" ) );
	SetupValue ( g_Settings.aimbot.resolver , 0 , ( "aimbot" ) , ( "resolver" ) );

	//	Lag
	SetupValue ( g_Settings.aa.choke , 14 , ( "aa" ) , ( "choke" ) );
	SetupValue ( g_Settings.aa.doFakelag , false , ( "aa" ) , ( "doFakelag" ) );

	//	AA
	SetupValue ( g_Settings.aa.enabled , false , ( "aa" ) , ( "enabled" ) );
	SetupValue ( g_Settings.aa.fake , 0 , ( "aa" ) , ( "fake" ) );
	SetupValue ( g_Settings.aa.fakeoff , 0 , ( "aa" ) , ( "fakeoff" ) );
	SetupValue ( g_Settings.aa.pitch , 0 , ( "aa" ) , ( "pitch" ) );
	SetupValue ( g_Settings.aa.real , 0 , ( "aa" ) , ( "real" ) );
	SetupValue ( g_Settings.aa.realoff , 0 , ( "aa" ) , ( "realoff" ) );

	//	esp
	SetupValue ( g_Settings.visuals.skeleton , 0 , ( "visuals" ) , ( "skeleton" ) );
	SetupValue ( g_Settings.visuals.glow , false , ( "visuals" ) , ( "glow" ) );
	SetupValue ( g_Settings.visuals.esp , 0 , ( "visuals" ) , ( "esp" ) );

	//	misc
	SetupValue ( g_Settings.misc.anti_ut , false , ( "misc" ) , ( "anti_ut" ) );
	SetupValue ( g_Settings.misc.thirdperson , false , ( "misc" ) , ( "thirdperson" ) );
	SetupValue ( g_Settings.misc.tpcode , 0 , ( "misc" ) , ( "tpcode" ) );

	//	movement
	SetupValue ( g_Settings.misc.cstrafe , false , ( "misc" ) , ( "cstrafe" ) );
	SetupValue ( g_Settings.misc.btcode , 0 , ( "misc" ) , ( "btcode" ) );
	SetupValue ( g_Settings.misc.autostrafe , false , ( "misc" ) , ( "autostrafe" ) );
	SetupValue ( g_Settings.misc.bhop , false , ( "misc" ) , ( "bhop" ) );

}


void CConfig::SetupValue ( int &value , int def , std::string category , std::string name )
{
	value = def;
	ints.push_back ( new ConfigValue<int> ( category , name , &value ) );
}

void CConfig::SetupValue ( float &value , float def , std::string category , std::string name )
{
	value = def;
	floats.push_back ( new ConfigValue<float> ( category , name , &value ) );
}

void CConfig::SetupValue ( bool &value , bool def , std::string category , std::string name )
{
	value = def;
	bools.push_back ( new ConfigValue<bool> ( category , name , &value ) );
}

void CConfig::Save ( )
{
	static TCHAR path [ MAX_PATH ];
	std::string folder , file;

	if ( SUCCEEDED ( SHGetFolderPath ( NULL , CSIDL_MYDOCUMENTS , NULL , 0 , path ) ) )
	{


		folder = std::string ( path ) + ( "\memecheat" );
		switch ( g_Settings.misc.config )
		{
		case 0:
			file = std::string ( path ) + ( "/memecheat" ) + std::string ( "/one" );
			break;
		case 1:
			file = std::string ( path ) + ( "/memecheat" ) + std::string ( "/two" );
			break;
		case 2:
			file = std::string ( path ) + ( "/memecheat" ) + std::string ( "/three" );
			break;
		case 3:
			file = std::string ( path ) + ( "/memecheat" ) + std::string ( "/four" );
			break;
		case 4:
			file = std::string ( path ) + ( "/memecheat" ) + std::string ( "/five" );
			break;
		case 5:
			file = std::string ( path ) + ( "/memecheat" ) + std::string ( "/six" );
			break;
		}

	}

	CreateDirectory ( folder.c_str ( ) , NULL );

	for ( auto value : ints )
		WritePrivateProfileString ( value->category.c_str ( ) , value->name.c_str ( ) , std::to_string ( *value->value ).c_str ( ) , file.c_str ( ) );

	for ( auto value : floats )
		WritePrivateProfileString ( value->category.c_str ( ) , value->name.c_str ( ) , std::to_string ( *value->value ).c_str ( ) , file.c_str ( ) );

	for ( auto value : bools )
		WritePrivateProfileString ( value->category.c_str ( ) , value->name.c_str ( ) , *value->value ? "true" : "false" , file.c_str ( ) );
}

void CConfig::Load ( )
{
	static TCHAR path [ MAX_PATH ];
	std::string folder , file;

	if ( SUCCEEDED ( SHGetFolderPath ( NULL , CSIDL_MYDOCUMENTS , NULL , 0 , path ) ) )
	{
		folder = std::string ( path ) + ( "\memecheat" );
		switch ( g_Settings.misc.config )
		{
		case 0:
			file = std::string ( path ) + ( "/memecheat" ) + std::string ( "/one" );
			break;
		case 1:
			file = std::string ( path ) + ( "/memecheat" ) + std::string ( "/two" );
			break;
		case 2:
			file = std::string ( path ) + ( "/memecheat" ) + std::string ( "/three" );
			break;
		case 3:
			file = std::string ( path ) + ( "/memecheat" ) + std::string ( "/four" );
			break;
		case 4:
			file = std::string ( path ) + ( "/memecheat" ) + std::string ( "/five" );
			break;
		case 5:
			file = std::string ( path ) + ( "/memecheat" ) + std::string ( "/six" );
			break;
		}
	}

	CreateDirectory ( folder.c_str ( ) , NULL );

	char value_l [ 32 ] = { '\0' };

	for ( auto value : ints )
	{
		GetPrivateProfileString ( value->category.c_str ( ) , value->name.c_str ( ) , "" , value_l , 32 , file.c_str ( ) );
		*value->value = atoi ( value_l );
	}

	for ( auto value : floats )
	{
		GetPrivateProfileString ( value->category.c_str ( ) , value->name.c_str ( ) , "" , value_l , 32 , file.c_str ( ) );
		*value->value = ( float ) atof ( value_l );
	}

	for ( auto value : bools )
	{
		GetPrivateProfileString ( value->category.c_str ( ) , value->name.c_str ( ) , "" , value_l , 32 , file.c_str ( ) );
		*value->value = !strcmp ( value_l , "true" );
	}
}

CConfig* Config = new CConfig ( );
