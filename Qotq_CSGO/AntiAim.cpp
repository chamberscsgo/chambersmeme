#include "AntiAim.h"
#include "Resolver.h"

void AA::PerformLag ( ) {
	auto local = g_pEntityList->GetClientEntity ( g_pEngine->GetLocalPlayer ( ) );

	int max_ticks_calc;
	if ( g_Settings.aa.enabled && !g_Settings.aa.doFakelag )
		max_ticks_calc = 1;
	else if ( g_Settings.aa.doFakelag ) {
		if ( local->GetVelocity ( ).Length ( ) > 0.1f ) //	pp breaker fix
			max_ticks_calc = g_Settings.aa.choke;
		else
			max_ticks_calc = 1;
	}

	bool did_hit_manual_max = chokedticks == max_ticks_calc;

	did_just_hit_ground_last_tick = did_just_hit_ground;
	if ( local->GetFlags ( ) & FL_ONGROUND )
		did_just_hit_ground = true;
	else
		did_just_hit_ground = false;

	if ( chokedticks > 14 ) {
		bSendPacket = true;
		chokedticks = 0;
	}
	else if ( did_just_hit_ground && !did_just_hit_ground_last_tick && g_Settings.aa.doFakelag ) {
		bSendPacket = false;
		chokedticks++;
	}
	else if ( did_hit_manual_max ) {
		bSendPacket = true;
		chokedticks = 0;
	}
	else {
		bSendPacket = false;
		chokedticks++;
	}
}

#pragma region movefix
#define DEG2RAD( x ) ( ( float )( x ) * ( float )( ( float )( PI ) / 180.0f ) )
#define PI 3.14159265358979323846f
void SinCos ( float a , float* s , float*c )
{
	*s = sin ( a );
	*c = cos ( a );
}
void AngleVectors ( const QAngle &angles , Vector& forward , Vector& right , Vector& up )
{
	float sr , sp , sy , cr , cp , cy;

	SinCos ( DEG2RAD ( angles [ 1 ] ) , &sy , &cy );
	SinCos ( DEG2RAD ( angles [ 0 ] ) , &sp , &cp );
	SinCos ( DEG2RAD ( angles [ 2 ] ) , &sr , &cr );

	forward.x = ( cp * cy );
	forward.y = ( cp * sy );
	forward.z = ( -sp );
	right.x = ( -1 * sr * sp * cy + -1 * cr * -sy );
	right.y = ( -1 * sr * sp * sy + -1 * cr *  cy );
	right.z = ( -1 * sr * cp );
	up.x = ( cr * sp * cy + -sr * -sy );
	up.y = ( cr * sp * sy + -sr * cy );
	up.z = ( cr * cp );
}
template<class T , class U>
T clamp ( T in , U low , U high )
{
	if ( in <= low )
		return low;

	if ( in >= high )
		return high;

	return in;
}
void AA::FixMovement ( CUserCmd *usercmd , QAngle &wish_angle )
{
	Vector view_fwd , view_right , view_up , cmd_fwd , cmd_right , cmd_up;
	QAngle viewangles = usercmd->viewangles;
	auto Norm = [ & ] ( QAngle in )
	{
		auto x_rev = in.pitch / 360.f;
		if ( in.pitch > 180.f || in.pitch < -180.f )
		{
			x_rev = abs ( x_rev );
			x_rev = round ( x_rev );

			if ( in.pitch < 0.f )
				in.pitch = ( in.pitch + 360.f * x_rev );

			else
				in.pitch = ( in.pitch - 360.f * x_rev );
		}

		auto y_rev = in.yaw / 360.f;
		if ( in.yaw > 180.f || in.yaw < -180.f )
		{
			y_rev = abs ( y_rev );
			y_rev = round ( y_rev );

			if ( in.yaw < 0.f )
				in.yaw = ( in.yaw + 360.f * y_rev );

			else
				in.yaw = ( in.yaw - 360.f * y_rev );
		}

		auto z_rev = in.roll / 360.f;
		if ( in.roll > 180.f || in.roll < -180.f )
		{
			z_rev = abs ( z_rev );
			z_rev = round ( z_rev );

			if ( in.roll < 0.f )
				in.roll = ( in.roll + 360.f * z_rev );

			else
				in.roll = ( in.roll - 360.f * z_rev );
		}

		return Vector ( in.pitch , in.yaw , in.roll );
	};

	Norm ( viewangles );

	AngleVectors ( wish_angle , view_fwd , view_right , view_up );
	AngleVectors ( usercmd->viewangles , cmd_fwd , cmd_right , cmd_up );

	const float v8 = sqrtf ( ( view_fwd.x * view_fwd.x ) + ( view_fwd.y * view_fwd.y ) );
	const float v10 = sqrtf ( ( view_right.x * view_right.x ) + ( view_right.y * view_right.y ) );
	const float v12 = sqrtf ( view_up.z * view_up.z );

	const Vector norm_view_fwd ( ( 1.f / v8 ) * view_fwd.x , ( 1.f / v8 ) * view_fwd.y , 0.f );
	const Vector norm_view_right ( ( 1.f / v10 ) * view_right.x , ( 1.f / v10 ) * view_right.y , 0.f );
	const Vector norm_view_up ( 0.f , 0.f , ( 1.f / v12 ) * view_up.z );

	const float v14 = sqrtf ( ( cmd_fwd.x * cmd_fwd.x ) + ( cmd_fwd.y * cmd_fwd.y ) );
	const float v16 = sqrtf ( ( cmd_right.x * cmd_right.x ) + ( cmd_right.y * cmd_right.y ) );
	const float v18 = sqrtf ( cmd_up.z * cmd_up.z );

	const Vector norm_cmd_fwd ( ( 1.f / v14 ) * cmd_fwd.x , ( 1.f / v14 ) * cmd_fwd.y , 0.f );
	const Vector norm_cmd_right ( ( 1.f / v16 ) * cmd_right.x , ( 1.f / v16 ) * cmd_right.y , 0.f );
	const Vector norm_cmd_up ( 0.f , 0.f , ( 1.f / v18 ) * cmd_up.z );

	const float v22 = norm_view_fwd.x * usercmd->forwardmove;
	const float v26 = norm_view_fwd.y * usercmd->forwardmove;
	const float v28 = norm_view_fwd.z * usercmd->forwardmove;
	const float v24 = norm_view_right.x * usercmd->sidemove;
	const float v23 = norm_view_right.y * usercmd->sidemove;
	const float v25 = norm_view_right.z * usercmd->sidemove;
	const float v30 = norm_view_up.x * usercmd->upmove;
	const float v27 = norm_view_up.z * usercmd->upmove;
	const float v29 = norm_view_up.y * usercmd->upmove;

	usercmd->forwardmove = ( ( ( ( norm_cmd_fwd.x * v24 ) + ( norm_cmd_fwd.y * v23 ) ) + ( norm_cmd_fwd.z * v25 ) )
		+ ( ( ( norm_cmd_fwd.x * v22 ) + ( norm_cmd_fwd.y * v26 ) ) + ( norm_cmd_fwd.z * v28 ) ) )
		+ ( ( ( norm_cmd_fwd.y * v30 ) + ( norm_cmd_fwd.x * v29 ) ) + ( norm_cmd_fwd.z * v27 ) );
	usercmd->sidemove = ( ( ( ( norm_cmd_right.x * v24 ) + ( norm_cmd_right.y * v23 ) ) + ( norm_cmd_right.z * v25 ) )
		+ ( ( ( norm_cmd_right.x * v22 ) + ( norm_cmd_right.y * v26 ) ) + ( norm_cmd_right.z * v28 ) ) )
		+ ( ( ( norm_cmd_right.x * v29 ) + ( norm_cmd_right.y * v30 ) ) + ( norm_cmd_right.z * v27 ) );
	usercmd->upmove = ( ( ( ( norm_cmd_up.x * v23 ) + ( norm_cmd_up.y * v24 ) ) + ( norm_cmd_up.z * v25 ) )
		+ ( ( ( norm_cmd_up.x * v26 ) + ( norm_cmd_up.y * v22 ) ) + ( norm_cmd_up.z * v28 ) ) )
		+ ( ( ( norm_cmd_up.x * v30 ) + ( norm_cmd_up.y * v29 ) ) + ( norm_cmd_up.z * v27 ) );
}
#pragma endregion

void AA::Run ( CUserCmd* cmd ) {
	if ( !g_Settings.aa.enabled )
		return;

	auto local = g_pEntityList->GetClientEntity ( g_pEngine->GetLocalPlayer ( ) );

	if ( !local )
		return;

	if ( local->GetHealth ( ) <= 0 )
		return;

	if ( cmd->buttons & IN_USE )
		return;

	if ( g_pInputSystem->IsButtonDown ( MOUSE_LEFT ) )
		return;

	if ( local->GetMoveType ( ) == MOVETYPE_LADDER )
		return;

	auto w = local->GetActiveWeapon ( );
	if ( w->GetWeaponType ( ) == WEAPONTYPE_GRENADE && w->IsInThrow ( cmd ) )
		return;

	usercmd = cmd;

	if ( g_pInputSystem->IsButtonDown ( KEY_V ) && g_Settings.aa.enabled ) {
		static float lastTime = 0;
		if ( GetTickCount ( ) + 500 < lastTime ) {
			lastTime = GetTickCount ( );
		}
		if ( GetTickCount ( ) > lastTime ) {
			if ( left == 1 )
				left = -1;
			else
				left = 1;
			lastTime = GetTickCount ( ) + 300;
		}
	}

	if ( bSendPacket ) {
		switch ( g_Settings.aa.fake ) {
		case 0:
			break;
		case 1:
			PerformStatic ( true );
			break;
		case 2:
			PerformJitter ( true );
			break;
		case 3:
			PerformSpin ( true );
			break;
		case 4:
			Perform180z ( true );
			break;
		case 5:
			PerformManual ( true );
			break;
		}
	} //fake
	else {
		switch ( g_Settings.aa.real ) {
		case 0:
			break;
		case 1:
			PerformStatic ( false );
			break;
		case 2:
			PerformJitter ( false );
			break;
		case 3:
			PerformSpin ( false );
			break;
		case 4:
			Perform180z ( false );
			break;
		case 5:
			PerformManual ( false );
			break;
		}
	} //real


	switch ( g_Settings.aa.pitch ) { //off, down, up, fakeup, fakedown
	case 0:
		break;
	case 1:
		usercmd->viewangles.pitch = 89.f;
		break;
	case 2:
		usercmd->viewangles.pitch = -89.f;
		break;
	case 3:
		usercmd->viewangles.pitch = -881.f;
		break;
	case 4:
		usercmd->viewangles.pitch = 881.f;
		break;
	}

	if ( m_bBreakLowerBody )
		usercmd->viewangles.yaw += 110 * left;
}

void AA::UpdateLBYBreaker ( CUserCmd* cmd )
{
	auto local = g_pEntityList->GetClientEntity ( g_pEngine->GetLocalPlayer ( ) );
	if ( !local )
		return;

	m_bBreakLowerBody = false;
	static float next_lby_update_time = 0;
	float curtime = ( PredictionSystem::Get ( ).CorrectTick + 1 ) * g_pGlobalVars->interval_per_tick;
	auto animstate = local->GetBasePlayerAnimState ( );

	if ( !animstate )
		return;

	if ( !( local->GetFlags ( ) & FL_ONGROUND ) )
		return;

	if ( !g_pClientState->chokedcommands ) {
		if ( animstate->speed_2d > 0.1f )
			next_lby_update_time = curtime + 0.22f;

		if ( next_lby_update_time < curtime ) {
			next_lby_update_time = curtime + 1.1f;
			m_bBreakLowerBody = true;
			return;
		}
	}
}

#pragma region types

void AA::Perform180z ( bool isf ) {
	auto air_val = fmod ( FIXED_CTIME ( ) / 0.9f * -300 , 140 );
	auto offset = ( isf ) ? g_Settings.aa.fakeoff : g_Settings.aa.realoff;
	usercmd->viewangles.yaw += offset + air_val;
}

void AA::PerformManual ( bool isf ) {
	auto offset = ( isf ) ? g_Settings.aa.fakeoff : g_Settings.aa.realoff;
	if ( isf )
		usercmd->viewangles.yaw -= 90 * left + offset;
	else
		usercmd->viewangles.yaw += 90 * left + offset;
}

void AA::PerformSpin ( bool isf ) {
	static float angle;
	angle += 5;
	auto offset = ( isf ) ? g_Settings.aa.fakeoff : g_Settings.aa.realoff;
	if ( angle > 180.0f ) angle = -180.0f;
	else if ( angle < -180.0f ) angle = 180.0f;
	usercmd->viewangles.yaw = offset + angle;
}

void AA::PerformStatic ( bool isf ) {
	float finalang = ( ( isf ) ? g_Settings.aa.fakeoff : g_Settings.aa.realoff );
	finalang = clamp ( finalang , -180 , 180 );
	usercmd->viewangles.yaw = finalang;
}

inline float RandomFloat ( float min , float max )
{
	static auto ranFloat = reinterpret_cast< float ( *)( float , float ) >( GetProcAddress ( GetModuleHandle ( "vstdlib.dll" ) , "RandomFloat" ) );
	if ( ranFloat )
		return ranFloat ( min , max );
	else
		return 0.f;
}

void AA::PerformJitter ( bool isf ) {
	auto offset = ( isf ) ? g_Settings.aa.fakeoff : g_Settings.aa.realoff;
	float finalang = offset + RandomFloat ( -11 , 11 );
	finalang = clamp ( finalang , -180 , 180 );
	usercmd->viewangles.yaw += finalang;
}

#pragma endregion