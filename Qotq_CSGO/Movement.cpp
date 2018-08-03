#include "Movement.h"
#include <algorithm>
#include <cstdint>

#define PI 3.14159265358979323846f
#define PI_F	((float)(PI)) 
#define DEG2RAD( x ) ( ( float )( x ) * ( float )( ( float )( PI ) / 180.0f ) )
#define RAD2DEG( x ) ( ( float )( x ) * ( float )( 180.0f / ( float )( PI ) ) )

inline float RandomFloat ( float min , float max )
{
	static auto ranFloat = reinterpret_cast< float ( *)( float , float ) >( GetProcAddress ( GetModuleHandle ( "vstdlib.dll" ) , "RandomFloat" ) );
	if ( ranFloat )
		return ranFloat ( min , max );
	else
		return 0.f;
}

void RotateMovement ( CUserCmd* cmd , float yaw )
{
	QAngle viewangles;
	g_pEngine->GetViewAngles ( viewangles );

	float rotation = DEG2RAD ( viewangles.yaw - yaw );

	float cos_rot = cos ( rotation );
	float sin_rot = sin ( rotation );

	float new_forwardmove = ( cos_rot * cmd->forwardmove ) - ( sin_rot * cmd->sidemove );
	float new_sidemove = ( sin_rot * cmd->forwardmove ) + ( cos_rot * cmd->sidemove );

	cmd->forwardmove = new_forwardmove;
	cmd->sidemove = new_sidemove;
}


void CMovement::Fakewalk ( CUserCmd* usercmd ) {
	fwchoked = fwchoked > 7 ? 0 : fwchoked + 1;
	usercmd->forwardmove = fwchoked < 2 || fwchoked > 5 ? 0 : usercmd->forwardmove;
	usercmd->sidemove = fwchoked < 2 || fwchoked > 5 ? 0 : usercmd->sidemove;
	bSendPacket = fwchoked < 1;
}

void CMovement::Bhop ( C_BaseEntity* pLocal , CUserCmd* cmd ) {
	auto serverbhop = g_pICvar->FindVar ( "sv_autobunnyhopping" );
	if ( serverbhop->GetInt ( ) == 0 ) {
		if ( cmd->buttons & IN_JUMP ) {
			if ( pLocal->GetFlags ( ) & FL_ONGROUND || pLocal->GetMoveType ( ) & MOVETYPE_LADDER )
			{
				cmd->buttons |= IN_JUMP;
			}
			else
			{
				cmd->buttons &= ~IN_JUMP;
			}
		}
	}
}

template<class T , class U>
T Clamp ( T in , U low , U high )
{
	if ( in <= low )
		return low;

	if ( in >= high )
		return high;

	return in;
}

bool isactive;
void CMovement::CStrafe ( CUserCmd *userCMD ) {
	//	nice 450 line antipaste. it *totally* worked....
}

void CMovement::AutoStrafe ( CUserCmd *userCMD )
{
	auto local = g_pEntityList->GetClientEntity ( g_pEngine->GetLocalPlayer ( ) );

	if ( local->GetMoveType ( ) == MOVETYPE_NOCLIP || local->GetMoveType ( ) == MOVETYPE_LADDER || local->GetHealth ( ) < 1 ) return;

	// If we're not jumping or want to manually move out of the way/jump over an obstacle don't strafe.
	if ( !g_pInputSystem->IsButtonDown ( ButtonCode_t::KEY_SPACE ) ||
		g_pInputSystem->IsButtonDown ( ButtonCode_t::KEY_A ) ||
		g_pInputSystem->IsButtonDown ( ButtonCode_t::KEY_D ) ||
		g_pInputSystem->IsButtonDown ( ButtonCode_t::KEY_S ) ||
		g_pInputSystem->IsButtonDown ( ButtonCode_t::KEY_W ) )
		return;

	if ( !( local->GetFlags ( ) & FL_ONGROUND ) ) {
		if ( g_pInputSystem->IsButtonDown ( ( ButtonCode_t ) g_Settings.misc.btcode ) && !isactive ) {
			StartCStrafe ( userCMD );
		}

		if ( g_pInputSystem->IsButtonDown ( ( ButtonCode_t ) g_Settings.misc.btcode ) && isactive ) {
			CStrafe ( userCMD );
			return;
		}
		else {
			isactive = false;
			if ( userCMD->mousedx > 1 || userCMD->mousedx < -1 ) {
				userCMD->sidemove = Clamp ( userCMD->mousedx < 0.f ? -400.f : 400.f , -400 , 400 );
			}
			else {
				if ( local->GetVelocity ( ).Length2D ( ) == 0 || local->GetVelocity ( ).Length2D ( ) == NAN || local->GetVelocity ( ).Length2D ( ) == INFINITE )
				{
					userCMD->forwardmove = 400;
					return;
				}
				userCMD->forwardmove = 5850.f / local->GetVelocity ( ).Length2D ( );
				userCMD->sidemove = Clamp ( ( userCMD->command_number % 2 ) == 0 ? -400.f : 400.f , -400 , 400 );
			}
		}
	}
}