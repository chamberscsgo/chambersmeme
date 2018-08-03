#include "LagComp.h"

#define TICK_INTERVAL			( g_pGlobalVars->interval_per_tick )


#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )		( TICK_INTERVAL *( t ) )
#define ROUND_TO_TICKS( t )		( TICK_INTERVAL * TIME_TO_TICKS( t ) )

template<class T> const T&
clamp( const T& x, const T& upper, const T& lower ) { return min( upper, max( x, lower ) ); }

#define LC_NONE				0
#define LC_ALIVE			(1<<0)

#define LC_ORIGIN_CHANGED	(1<<8)
#define LC_ANGLES_CHANGED	(1<<9)
#define LC_SIZE_CHANGED		(1<<10)
#define LC_ANIMATION_CHANGED (1<<11)

float GetLerpTime( )
{
	auto cl_updaterate = g_pICvar->FindVar( "cl_updaterate" );
	auto sv_minupdaterate = g_pICvar->FindVar( "sv_minupdaterate" );
	auto sv_maxupdaterate = g_pICvar->FindVar( "sv_maxupdaterate" );

	auto cl_interp = g_pICvar->FindVar( "cl_interp" );
	auto sv_client_min_interp_ratio = g_pICvar->FindVar( "sv_client_min_interp_ratio" );
	auto sv_client_max_interp_ratio = g_pICvar->FindVar( "sv_client_max_interp_ratio" );

	auto updateRate = cl_updaterate->GetInt( );
	auto interpRatio = cl_interp->GetFloat( );
	auto minInterpRatio = sv_client_min_interp_ratio->GetFloat( );
	auto maxInterpRatio = sv_client_max_interp_ratio->GetFloat( );
	auto minUpdateRate = sv_minupdaterate->GetInt( );
	auto maxUpdateRate = sv_maxupdaterate->GetInt( );

	auto clampedUpdateRate = clamp( updateRate, minUpdateRate, maxUpdateRate );
	auto clampedInterpRatio = clamp( interpRatio, minInterpRatio, maxInterpRatio );

	auto lerp = clampedInterpRatio / clampedUpdateRate;

	if ( lerp <= cl_interp->GetFloat( ) )
		lerp = cl_interp->GetFloat( );

	return lerp;
}

#define FLOW_OUTGOING	0		
#define FLOW_INCOMING	1
#define MAX_FLOWS		2		// in & out

//	actually think for the LC
bool CBacktrack::StartLagCompensation( C_BaseEntity * player, CUserCmd * cmd, int bestrecord )
{
	if ( !IsValid( player ) )
		return false;

	int idx = player->GetIndex( );

	//	get player track
	auto *track = &m_PlayerTrack [ idx - 1 ];

	//	get the record
	LagRecord &record = track->at( bestrecord ).first;

	//	validity check
	if ( !ValidTick( record.m_flSimulationTime ) )
		return false;

	//	restore
	RestoreRecord( record, player );

	return true;
}

//	yup.
int CBacktrack::FixTickcount( C_BaseEntity * player, int bestrecord )
{
	int idx = player->GetIndex( );

	std::deque<std::pair<LagRecord, int>> *track = &m_PlayerTrack [ idx - 1 ];
	LagRecord &record = track->at( bestrecord ).first;
	if ( record.m_flSimulationTime == 0.0f )
		return TIME_TO_TICKS( player->SimTime( ) + GetLerpTime( ) ) + 1;

	int iLerpTicks = TIME_TO_TICKS( GetLerpTime( ) );
	int iTargetTickCount = TIME_TO_TICKS( record.m_flSimulationTime ) + iLerpTicks;

	return iTargetTickCount;
}

//	fsn
void CBacktrack::FSN( )
{
	static auto sv_unlag = g_pICvar->FindVar( "sv_unlag" );
	static auto sv_maxunlag = g_pICvar->FindVar( "sv_maxunlag" );

	if ( ( g_pGlobalVars->maxClients <= 1 ) || !sv_unlag->GetBool( ) )
	{
		ClearHistory( );
		return;
	}

	int flDeadtime = g_pGlobalVars->curtime - sv_maxunlag->GetFloat( );

	for ( int i = 1; i <= g_pGlobalVars->maxClients; i++ )
	{
		auto *pPlayer = reinterpret_cast< C_BaseEntity* >( g_pEntityList->GetClientEntity( i ) );

		auto &track = m_PlayerTrack [ i - 1 ];

		if ( !IsValid( pPlayer ) )
		{
			if ( track.size( ) > 0 )
			{
				track.clear( );
			}

			continue;
		}

		*reinterpret_cast< int* > ( reinterpret_cast< uintptr_t >( pPlayer ) + 0xA30 ) = g_pGlobalVars->framecount;
		*reinterpret_cast< int* > ( reinterpret_cast< uintptr_t >( pPlayer ) + 0xA28 ) = 0;

		DisableInterpolation( pPlayer );

		Assert( track->Count( ) < 1000 ); // sanity

		// remove old records
		while ( track.size( ) )
		{
			LagRecord &tail = track.back( ).first;

			// if tail is within limits, stop
			if ( tail.m_flSimulationTime >= flDeadtime )
				continue;

			// remove tail
			track.pop_back( );
		}

		// store simtime, only for extrap
		float stored_simtime = pPlayer->SimTime( );

		// simulation time popping
		if ( track.size( ) > 0 )
		{
			auto &tr = *track.begin( );
			auto &head = tr.first;

			//	perform extrap if the simtime hasnt updated
			if ( head.m_flSimulationTime >= pPlayer->SimTime( ) )
				continue;

			//if ( head.m_flSimulationTime >= pPlayer->SimTime( ) )
			//	PerformExtrapolation( pPlayer, stored_simtime );	//	extrapolate player. to not extrap, replace with continue
			//else {
			//	m_PrevRecords [ i - 1 ] [ 1 ] = m_PrevRecords [ i - 1 ] [ 0 ];
			//	m_PrevRecords [ i - 1 ] [ 0 ] = CreateRecord( pPlayer );
			//}
		}

		//	add new record to deque
		LagRecord record = CreateRecord( pPlayer );
		record.m_flSimulationTime = stored_simtime;
		std::pair<LagRecord, int> paired;
		paired.first = record;
		if ( pPlayer->GetVelocity( ).Length2D( ) > 120 )
			paired.second = 0;	//	best case
		else
			paired.second = 2;	//	worst case
		//	resolved records are priority 1: second to moving, but better than all others
		track.push_front( paired );
	}
}

//	validity check
bool CBacktrack::ValidTick( float simTime )
{
	INetChannelInfo *nci = g_pEngine->GetNetChannelInfo( );
	if ( !nci )
		return false;

	auto lerpTime = GetLerpTime( );

	auto predictedCmdArrivalTick = g_pGlobalVars->tickcount + 1 + TIME_TO_TICKS( nci->GetAvgLatency( FLOW_INCOMING ) + nci->GetAvgLatency( FLOW_OUTGOING ) );
	auto deltaTime = clamp( lerpTime + nci->GetLatency( FLOW_OUTGOING ), 0.f, 1.f ) - TICKS_TO_TIME( predictedCmdArrivalTick + TIME_TO_TICKS( lerpTime ) - TIME_TO_TICKS( simTime ) );

	return fabs( deltaTime ) > 0.2f;
}

//	set cmd->tickcount to this
int CBacktrack::GetBestTick( C_BaseEntity * pPlayer )
{
	int tickcount = g_pGlobalVars->tickcount;

	auto &track = m_PlayerTrack [ pPlayer->GetIndex( ) - 1 ];

	//	go thru all priorities from least to best, therefore the final record should be the best possible
	for ( int priority = 0; priority < 3; priority++ ) {
		//	every record in the deque of this priority level
		for ( int i = 0; i < track.size( ); i++ ) {
			//	priority check
			if ( track [ i ].second != priority )
				continue;

			//do lagcomp
			if ( StartLagCompensation( pPlayer, NULL, i - 1 ) )
			{
				tickcount = FixTickcount( pPlayer, i - 1 );
				break;
			}
		}
	}

	return tickcount;
}

//	set cmd->tickcount to this
int CBacktrack::GetFirstTick( C_BaseEntity * pPlayer )
{
	int tickcount = g_pGlobalVars->tickcount;

	auto &track = m_PlayerTrack [ pPlayer->GetIndex( ) - 1 ];

	for ( int i = 0; i < track.size( ); i++ ) {
		//do lagcomp
		if ( StartLagCompensation( pPlayer, NULL, i - 1 ) )
		{
			tickcount = FixTickcount( pPlayer, i - 1 );
			break;
		}
	}

	return tickcount;
}

//	create a new record
LagRecord CBacktrack::CreateRecord( C_BaseEntity * entity ) {
	int idx = entity->GetIndex( );
	LagRecord record;

	//	bone cache
	static uintptr_t pInvalidateBoneCache = g_pUtils->FindPatternIDA( ( "client.dll" ), ( "80 3D ? ? ? ? 00 74 16 A1" ) );
	unsigned long g_iModelBoneCounter = **reinterpret_cast< unsigned long** > ( pInvalidateBoneCache + 10 );
	*reinterpret_cast< unsigned int* >( reinterpret_cast< uintptr_t >( entity ) + 0x2914 ) = 0xFF7FFFFF;
	*reinterpret_cast< unsigned int* >( reinterpret_cast< uintptr_t >( entity ) + 0x2680 ) = ( g_iModelBoneCounter - 1 );

	//	store simtime, angs, flags, etc.
	//entity->SetupBones( record.m_nBones, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, g_pGlobalVars->curtime );
	record.m_flSimulationTime = entity->SimTime( );
	entity->GetAbsAngles( ).Clamp( );
	record.m_vecAbsAngles = entity->GetAbsAngles( );
	record.m_vecOrigin = entity->GetOrigin( );
	record.m_fFlags = entity->GetFlags( );
	record.m_flLowerBodyYawTarget = entity->LowerBodyYaw( );

	//	store pose params
	for ( int poseparam = 0; poseparam < 24; poseparam++ )
		record.m_flPoseParameter [ poseparam ] = entity->PoseParams( ) [ poseparam ];

	//	store animlayers
	for ( int animlayer = 0; animlayer < MAX_LAYER_RECORDS; animlayer++ )
	{
		record.m_layerRecords [ animlayer ].m_flCycle = entity->GetAnimOverlay( animlayer )->m_flCycle;
		record.m_layerRecords [ animlayer ].m_nOrder = entity->GetAnimOverlay( animlayer )->m_nOrder;
		record.m_layerRecords [ animlayer ].m_nSequence = entity->GetAnimOverlay( animlayer )->m_nSequence;
		record.m_layerRecords [ animlayer ].m_flWeight = entity->GetAnimOverlay( animlayer )->m_flWeight;
	}

	return record;
}

//	restore a record: aka backtrack em
void CBacktrack::RestoreRecord( LagRecord &record, C_BaseEntity * entity ) {
	int idx = entity->GetIndex( );

	//	bone cache
	static uintptr_t pInvalidateBoneCache = g_pUtils->FindPatternIDA( ( "client.dll" ), ( "80 3D ? ? ? ? 00 74 16 A1" ) );
	unsigned long g_iModelBoneCounter = **reinterpret_cast< unsigned long** > ( pInvalidateBoneCache + 10 );
	*reinterpret_cast< unsigned int* >( reinterpret_cast< uintptr_t >( entity ) + 0x2914 ) = 0xFF7FFFFF;
	*reinterpret_cast< unsigned int* >( reinterpret_cast< uintptr_t >( entity ) + 0x2680 ) = ( g_iModelBoneCounter - 1 );

	//	set angles, flags, etc
	entity->SetAbsOrigin( record.m_vecOrigin );
	entity->SetAbsAngles( record.m_vecAbsAngles );
	entity->SimTime( ) = record.m_flSimulationTime;
	entity->GetFlags( ) = record.m_fFlags;
	entity->LowerBodyYaw( ) = record.m_flLowerBodyYawTarget;

	//	set pose params
	for ( int poseparam = 0; poseparam < 24; poseparam++ )
		entity->PoseParams( ) [ poseparam ] = record.m_flPoseParameter [ poseparam ];

	//	anim layers?
	for ( int animlayer = 0; animlayer < MAX_LAYER_RECORDS; animlayer++ )
	{
		entity->GetAnimOverlay( animlayer )->m_flCycle = record.m_layerRecords [ animlayer ].m_flCycle;
		entity->GetAnimOverlay( animlayer )->m_nOrder = record.m_layerRecords [ animlayer ].m_nOrder;
		entity->GetAnimOverlay( animlayer )->m_nSequence = record.m_layerRecords [ animlayer ].m_nSequence;
		entity->GetAnimOverlay( animlayer )->m_flWeight = record.m_layerRecords [ animlayer ].m_flWeight;
	}

	//	animfix is wonky, have a check!
	if ( g_Settings.aimbot.animfix )
		FixAnims( entity );
	else
		entity->UpdateClientSideAnimation( );
}

//	sanity for players
bool CBacktrack::IsValid( C_BaseEntity * pPlayer )
{
	auto local = g_pEntityList->GetClientEntity( g_pEngine->GetLocalPlayer( ) );
	if ( !pPlayer )
		return false;

	if ( pPlayer->GetDormant( ) || pPlayer->GetHealth( ) > 0 || pPlayer->GetFlags( ) & FL_FROZEN )
		return false;

	if ( pPlayer->GetTeam( ) == local->GetTeam( ) )
		return false;

	if ( pPlayer->GetClientClass( )->m_ClassID != 35 )
		return false;

	if ( pPlayer->GetIndex( ) == g_pEngine->GetLocalPlayer( ) )
		return false;

	return true;
}

//	high quality meme that probably got about nothing right. good times.
void CBacktrack::FixAnims( C_BaseEntity* player ) {

//	anitpaste
}

//	singular tick based extrapolation. pretty fucking garbage, but who cares?
void CBacktrack::PerformExtrapolation( C_BaseEntity* player, float &s_simtime ) {
	//	find num of ticks we want to extrap the player to
	int ticks_to_extrap = g_pGlobalVars->tickcount - TIME_TO_TICKS( player->SimTime( ) );

	//	accel
	auto index = player->GetIndex( ) - 1;
	Vector velocity;
	velocity.Zero( );	//	zero this so if track doesnt exist we dont fuck with accel
	if ( m_PlayerTrack [ index ].size( ) ) {
		velocity = m_PrevRecords [ index ] [ 1 ].m_vecVelocity - m_PrevRecords [ index ] [ 0 ].m_vecVelocity;
		velocity /= TIME_TO_TICKS( m_PrevRecords [ index ] [ 1 ].m_flSimulationTime - m_PrevRecords [ index ] [ 0 ].m_flSimulationTime );
	}

	//	default velocity
	auto default_vel = player->GetVelocity( ) * ( g_pGlobalVars->interval_per_tick * ticks_to_extrap );
	auto accel_vel = velocity * ( g_pGlobalVars->interval_per_tick * ticks_to_extrap );

	//	create the estimated extrap
	Vector final_pos = player->GetOrigin( ) + default_vel - accel_vel;	//	might need to be +accel_vel instead

	//	add gravity
	final_pos.z -= ticks_to_extrap * g_pICvar->FindVar( "sv_gravity" )->GetFloat( );

	//	create a raytrace to the final pos to check for collision
	auto mins = player->GetCollideable( )->OBBMins( );
	auto maxs = player->GetCollideable( )->OBBMaxs( );
	auto src = player->GetOrigin( );
	auto end = final_pos;
	Ray_t ray;
	ray.Init( src, end, mins, maxs );
	trace_t trace;
	CTraceFilter filter;
	filter.pSkip = player;
	g_pEngineTrace->TraceRay( ray, CONTENTS_SOLID, &filter, &trace );

	//	if we hit something, set the player location to the pos it hit at
	if ( trace.fraction != 1.f )
		final_pos = trace.endpos;

	//	set player origin and simtime, simtime gets reset after being stored
	player->SetAbsOrigin( final_pos );
	s_simtime = player->SimTime( ) + TICKS_TO_TIME( ticks_to_extrap );
}

//	for resolver, kinda useless but w/e
void CBacktrack::SetOverwriteTick( C_BaseEntity *player, QAngle angles, float correct_time ) {
	int idx = player->GetIndex( );

	LagRecord record;

	record = CreateRecord( player );

	//	overwrite
	record.m_flSimulationTime = correct_time;
	angles.Clamp( );
	record.m_vecAbsAngles = angles;

	std::pair<LagRecord, int> pr;
	pr.first = record; pr.second = 1;

	m_PlayerTrack [ idx - 1 ].push_front( pr );
}
