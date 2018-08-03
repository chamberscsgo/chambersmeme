#include "ShitMenu.h"

#define MAX_OPTS 21
#define CONFIG_POS 20

//high level proprietary software made by a team of professionals from around the globe

std::string bcts( ButtonCode_t code ) {
	return g_pInputSystem->ButtonCodeToString( code );
}

void ShitMenu::DrawShit( ) {
	autonum = 0;

	Draw->String( F_Arial, false, indent, 5, Color( 255, 255, 255, 255 ), "chambers' meme (open)" );

	DrawLine( "Aimbot: ", g_Settings.aimbot.enabled );
	DrawLine( "Head Only: ", g_Settings.aimbot.headonly );
	DrawLine( "Animfix: ", g_Settings.aimbot.animfix );
	DrawLine( "Resolver: ", g_Settings.aimbot.resolver, g_Settings.opts.resolvers, 4 );

	DrawLine( "AA: ", g_Settings.aa.enabled );
	DrawLine( "Pitch: ", g_Settings.aa.pitch, g_Settings.opts.pitches, 5 );
	DrawLine( "Real: ", g_Settings.aa.real, g_Settings.opts.reals, 6 );
	DrawLine( "Real Off: ", g_Settings.aa.realoff );
	DrawLine( "Fake: ", g_Settings.aa.fake, g_Settings.opts.fakes, 6 );
	DrawLine( "Fake Off: ", g_Settings.aa.fakeoff );

	DrawLine( "Lag: ", g_Settings.aa.doFakelag );
	DrawLine( "Choke: ", g_Settings.aa.choke );

	DrawLine( "Skeleton: ", g_Settings.visuals.skeleton, g_Settings.opts.bones, 4 );
	DrawLine( "Glow: ", g_Settings.visuals.glow );
	DrawLine( "Info: ", g_Settings.visuals.esp, g_Settings.opts.infos, 4 );

	DrawLine( "Bhop: ", g_Settings.misc.bhop );
	DrawLine( "Strafe: ", g_Settings.misc.autostrafe );
	std::string tpstr = "TP (" + bcts( ( ButtonCode_t ) g_Settings.misc.tpcode ) + "): ";
	DrawLine( tpstr, g_Settings.misc.thirdperson );
	DrawLine( "Anti-ut: ", g_Settings.misc.anti_ut );

	std::string btestr = "CStrafe (" + bcts( ( ButtonCode_t ) g_Settings.misc.btcode ) + "): ";
	DrawLine( btestr, g_Settings.misc.cstrafe );

	DrawLine( "Config: ", g_Settings.misc.config );
	DrawConfig( "Save Config (Enter)" );

	Draw->String( F_Arial, false, indent - 10, cur * 15 + 20, Color( 255, 0, 0, 255 ), "-" );
}

void ShitMenu::DoInt( int &val, int incr, int min, int max, int pos ) {
	if ( cur != pos )
		return;

	static float lastTime = 0;
	if ( GetTickCount( ) + 200 < lastTime ) {
		lastTime = GetTickCount( );
	}
	if ( GetTickCount( ) > lastTime ) {

		if ( g_pInputSystem->IsButtonDown( KEY_LEFT ) ) {
			if ( val == min )
				return;
			val -= incr;
			if ( cur == CONFIG_POS ) Config->Load( );
			lastTime = GetTickCount( ) + 150;
		}
		else if ( g_pInputSystem->IsButtonDown( KEY_RIGHT ) ) {
			if ( val == max )
				return;
			val += incr;
			if ( cur == CONFIG_POS ) Config->Load( );
			lastTime = GetTickCount( ) + 150;
		}
	}
}

void ShitMenu::DoBool( bool &val, int pos ) {
	if ( cur != pos )
		return;

	static float lastTime = 0;
	if ( GetTickCount( ) + 200 < lastTime ) {
		lastTime = GetTickCount( );
	}
	if ( GetTickCount( ) > lastTime ) {

		if ( g_pInputSystem->IsButtonDown( KEY_LEFT ) ) {
			val = !val;
			lastTime = GetTickCount( ) + 150;
		}
		else if ( g_pInputSystem->IsButtonDown( KEY_RIGHT ) ) {
			val = !val;
			lastTime = GetTickCount( ) + 150;
		}

	}
}

void ShitMenu::DoConfig( int pos ) {
	if ( cur != pos )
		return;

	static float lastTime = 0;
	if ( GetTickCount( ) + 200 < lastTime ) {
		lastTime = GetTickCount( );
	}
	if ( GetTickCount( ) > lastTime ) {

		if ( g_pInputSystem->IsButtonDown( KEY_ENTER ) ) {
			Config->Save( );
			lastTime = GetTickCount( ) + 150;
		}
	}
}

void ShitMenu::HandleInput( ) {

	if ( g_pInputSystem->IsButtonDown( KEY_UP ) ) {
		if ( cur == 0 )
			return;

		static float lastTime = 0;
		if ( GetTickCount( ) + 200 < lastTime ) {
			lastTime = GetTickCount( );
		}

		if ( GetTickCount( ) > lastTime ) {
			cur--;
			lastTime = GetTickCount( ) + 100;
		}
	}
	if ( g_pInputSystem->IsButtonDown( KEY_DOWN ) ) {
		if ( cur == MAX_OPTS )
			return;
		static float lastTime = 0;
		if ( GetTickCount( ) + 200 < lastTime ) {
			lastTime = GetTickCount( );
		}
		if ( GetTickCount( ) > lastTime ) {
			cur++;
			lastTime = GetTickCount( ) + 150;
		}
	}

	DoBool( g_Settings.aimbot.enabled, 0 );
	DoBool( g_Settings.aimbot.headonly, 1 );
	DoBool( g_Settings.aimbot.animfix, 2 );
	DoInt( g_Settings.aimbot.resolver, 1, 0, 3, 3 );

	DoBool( g_Settings.aa.enabled, 4 );
	DoInt( g_Settings.aa.pitch, 1, 0, 4, 5 );
	DoInt( g_Settings.aa.real, 1, 0, 5, 6 );
	DoInt( g_Settings.aa.realoff, 10, -180, 180, 7 );
	DoInt( g_Settings.aa.fake, 1, 0, 5, 8 );
	DoInt( g_Settings.aa.fakeoff, 10, -180, 180, 9 );

	DoBool( g_Settings.aa.doFakelag, 10 );
	DoInt( g_Settings.aa.choke, 1, 0, 14, 11 );

	DoInt( g_Settings.visuals.skeleton, 1, 0, 3, 12 );
	DoBool( g_Settings.visuals.glow, 13 );
	DoInt( g_Settings.visuals.esp, 1, 0, 3, 14 );

	DoBool( g_Settings.misc.bhop, 15 );
	DoBool( g_Settings.misc.autostrafe, 16 );
	DoBool( g_Settings.misc.thirdperson, 17 );
	DoBool( g_Settings.misc.anti_ut, 18 );
	DoBool( g_Settings.misc.cstrafe, 19 );

	DoInt( g_Settings.misc.config, 1, 0, 5, 20 );
	DoConfig( 21 );
}

void ShitMenu::DrawLine( std::string str, bool value ) {
	std::string onstr = ( value ) ? "On" : "Off";
	std::string options [ 2 ] { "On", "Off" };
	std::string finalstr = str + onstr;
	Draw->String( F_Arial, false, indent, autonum * 15 + 20, Color( 0, 255, 0, 255 ), finalstr.c_str( ) );

	if ( cur == autonum ) {
		for ( int j = 0; j < 2; j++ ) {
			Color col = ( j == !value ) ? Color( 255, 0, 0, 255 ) : Color( 0, 255, 0, 255 );
			Draw->String( F_Arial, false, indent + 120, autonum * 15 + 20 + j * 15, col, options [ j ].c_str( ) );
		}
	}

	autonum++;
}

void ShitMenu::DrawLine( std::string str, int value ) {
	std::string finalstr = str + std::to_string( value );
	Draw->String( F_Arial, false, indent, autonum * 15 + 20, Color( 0, 255, 0, 255 ), finalstr.c_str( ) );
	autonum++;
}

void ShitMenu::DrawLine( std::string str, int value, std::string *opts, int numopts ) {
	std::string finalstr = str + opts [ value ];
	Draw->String( F_Arial, false, indent, autonum * 15 + 20, Color( 0, 255, 0, 255 ), finalstr.c_str( ) );
	if ( cur == autonum ) {
		for ( int j = 0; j < numopts; j++ ) {
			Color col = ( j == value ) ? Color( 255, 0, 0, 255 ) : Color( 0, 255, 0, 255 );
			Draw->String( F_Arial, false, indent + 120, autonum * 15 + 20 + j * 15, col, opts [ j ].c_str( ) );
		}
	}
	autonum++;
}

void ShitMenu::DrawConfig( std::string str ) {
	Draw->String( F_Arial, false, indent, autonum * 15 + 20, Color( 0, 255, 0, 255 ), str.c_str( ) );
	autonum++;
}

