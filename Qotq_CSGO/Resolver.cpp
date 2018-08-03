#include "SDK.h"

float Resolver::Bruteforce( C_BaseEntity *ent )
{
	auto record = CBacktrack::Get( ).m_PlayerTrack [ ent->GetIndex( ) ];

	int avg = 1;
	int count = 1;

	float prevlby = 0.f;
	bool done = false;
	for ( auto &r : record )
	{
		if ( !done ) {
			done = true;
			continue;
		}

		if ( r.first.m_flLowerBodyYawTarget == prevlby ) {
			prevlby = r.first.m_flLowerBodyYawTarget;
			continue;
		}

		avg += GetDelta( static_cast< int >( prevlby ), static_cast< int >( r.first.m_flLowerBodyYawTarget ) );

		prevlby = r.first.m_flLowerBodyYawTarget;

		count++;
	}

	avg /= count;

	return avg;
}

void Resolver::Run( ) {
	if ( g_Settings.aimbot.resolver == 1 )
		Default_Resolver( );
	else if ( g_Settings.aimbot.resolver == 2 )
		Experimental_Resolver( );
	else if ( g_Settings.aimbot.resolver == 3 )
		Tertiary_Resolver( );
}

void Resolver::Experimental_Resolver( )
{
	if ( g_pEngine->IsInGame( ) && g_pEngine->IsConnected( ) ) {

		auto local = g_pEntityList->GetClientEntity( g_pEngine->GetLocalPlayer( ) );
		for ( int i = 0; i < g_pGlobalVars->maxClients; i++ ) {
			auto e = g_pEntityList->GetClientEntity( i );
			if ( !e || e->GetHealth( ) < 1 || e->GetTeam( ) == local->GetTeam( ) )
				continue;

			auto info = &secondary [ i ];

			//	dormancy
			if ( e->GetDormant( ) ) {
				info->dormant_pos = e->GetOrigin( );
				shots [ i ] = 0;
				hit_shots [ i ] = 0;
				continue;
			}
			else {
				info->seen_pos = e->GetOrigin( );
			}

			//	calculate misses
			int misses = shots [ i ] - hit_shots [ i ];

			//	do nospread
			static ConVar* weapon_accuracy_nospread = g_pICvar->FindVar( "weapon_accuracy_nospread" );
			if ( weapon_accuracy_nospread->GetInt( ) == 1 ) {
				resolve_air( e );
				resolvertype [ i ] = "Nospread";
				continue;
			}

			//	fakewalk check				
			auto is_fakewalking = [ & ] ( ) -> bool {
				//	temp
				return ( e->GetVelocity( ).Length2D( ) >= 6 ) && ( e->GetVelocity( ).Length2D( ) <= 64 ) && !( e->GetFlags( ) & FL_DUCKING );
			};

			//	movement shit
			if ( e->GetVelocity( ).Length2D( ) > 0.1 && !is_fakewalking( ) ) {
				e->GetEyeAngles( ).yaw = e->LowerBodyYaw( );
				info->last_moving_lby = e->LowerBodyYaw( );
				info->dormant_pos = e->GetOrigin( );
				resolvertype [ i ] = "Moving";
			}
			else {

				//	deal with lby updates
				float prev_lby;
				if ( info->last_updated_lby != e->LowerBodyYaw( ) ) {
					prev_lby = info->last_updated_lby;
					info->last_updated_lby = e->LowerBodyYaw( );
					info->last_update_time = e->SimTime( );
					CBacktrack::Get( ).SetOverwriteTick( e, QAngle( 0, e->LowerBodyYaw( ), 0 ), e->SimTime( ) );
					info->relative_delta = GetDelta( info->last_updated_lby, prev_lby );
				}

				//	last moving lby
				if ( misses == 0 && info->seen_pos == info->dormant_pos ) {
					e->GetEyeAngles( ).yaw = info->last_moving_lby;
					resolvertype [ i ] = "Last Moving";
				}

				//	delta > 120
				else if ( fabsf( info->relative_delta ) >= 120 && misses <= 2 ) {
					//	just force -180
					e->GetEyeAngles( ).yaw = e->LowerBodyYaw( ) - 180;
					resolvertype [ i ] = "Delta > 120";
				}

				//	delta < 120 or we missed
				else {
					//	find estimated breaker side
					auto side = info->relative_delta < 0 ? -1 : 1;

					//	change side
					//if (misses % 3 == 2) info->force_side *= -1;

					//	multiply side by force_side to deal with dumping.... not pp but we should override anyways so w/e
					side *= info->force_side;

					//	bruteforce 120/90/60 * estimated side for breaker
					switch ( misses % 3 ) {
					case 0: e->GetEyeAngles( ).yaw = info->last_updated_lby + 120 * side; break;
					case 1: e->GetEyeAngles( ).yaw = info->last_updated_lby + 90 * side; break;
					case 2: e->GetEyeAngles( ).yaw = info->last_updated_lby + 60 * side; break;
					}
					resolvertype [ i ] = "Delta < 120";
				}
			}
			//	override
			if ( g_pInputSystem->IsButtonDown( ButtonCode_t::KEY_LALT ) ) e->GetEyeAngles( ).yaw += 180;
			ClampYaw( e->GetEyeAngles( ).yaw );
			info->last_velocity_length2d = e->GetVelocity( ).Length2D( );
		}
	}
}

void Resolver::Default_Resolver( )
{
	if ( g_pEngine->IsInGame( ) && g_pEngine->IsConnected( ) ) {

		auto local = g_pEntityList->GetClientEntity( g_pEngine->GetLocalPlayer( ) );
		for ( int i = 0; i < g_pGlobalVars->maxClients; i++ ) {
			auto e = g_pEntityList->GetClientEntity( i );
			if ( !e || e->GetHealth( ) < 1 || e->GetTeam( ) == local->GetTeam( ) )
				continue;

			//	dormancy
			if ( e->GetDormant( ) ) {
				shots [ i ] = 0;
				hit_shots [ i ] = 0;
				primary.ischoking [ i ] = false;
				primary.is_spinning [ i ] = false;
				primary.spin_ticks [ i ] = 0;
				primary.last_lby [ i ] = 0;
				primary.last_lby_for_delt [ i ] = 0;
				continue;
			}

			//	declare finalang, calculate misses
			QAngle final_angle;
			int misses = shots [ i ] - hit_shots [ i ];

			//	do nospread
			static ConVar* weapon_accuracy_nospread = g_pICvar->FindVar( "weapon_accuracy_nospread" );
			if ( weapon_accuracy_nospread->GetInt( ) == 1 ) {
				resolve_air( e );
				resolvertype [ i ] = "Nospread";
				continue;
			}

			//	fakewalk check				
			auto is_fakewalking = [ & ] ( ) -> bool {
				AnimationLayer anim_layers [ 15 ];
				bool
					bFakewalking = false,
					stage1 = false,			// stages needed cause we are iterating all layers, eitherwise won't work :)
					stage2 = false,
					stage3 = false;

				for ( int i = 0; i < e->GetNumAnimOverlays( ); i++ )
				{
					anim_layers [ i ] = e->GetAnimOverlays( ) [ i ];
					if ( anim_layers [ i ].m_nSequence == 26 && anim_layers [ i ].m_flWeight < 0.4f )
						stage1 = true;
					if ( anim_layers [ i ].m_nSequence == 7 && anim_layers [ i ].m_flWeight > 0.001f )
						stage2 = true;
					if ( anim_layers [ i ].m_nSequence == 2 && anim_layers [ i ].m_flWeight == 0 )
						stage3 = true;
				}

				if ( stage1 && stage2 )
					if ( stage3 || ( e->GetFlags( ) & FL_DUCKING ) ) // since weight from stage3 can be 0 aswell when crouching, we need this kind of check, cause you can fakewalk while crouching, thats why it's nested under stage1 and stage2
						bFakewalking = true;
					else
						bFakewalking = false;
				else
					bFakewalking = false;

				return bFakewalking;
			};

			//	fix fw for extrap
			primary.is_fw [ i ] = is_fakewalking( );

			//	set last_lby
			auto &llby = primary.last_lby [ i ];

			//	movement shit
			if ( e->GetVelocity( ).Length2D( ) > 0.f && !primary.is_fw [ i ] ) {
				if ( e->GetVelocity( ).Length2D( ) > 120.f ) {
					primary.last_lby_for_delt [ i ] = e->LowerBodyYaw( );
				}
				//	set lby
				final_angle.yaw = e->LowerBodyYaw( );
				llby = e->LowerBodyYaw( );
				//	set moving ticks
				primary.moving_ticks [ i ] [ 1 ] = primary.moving_ticks [ i ] [ 0 ];
				primary.moving_ticks [ i ] [ 0 ] = true;
				resolvertype [ i ] = "Moving";
			}
			else {
				//	set moving ticks
				primary.moving_ticks [ i ] [ 1 ] = primary.moving_ticks [ i ] [ 0 ];
				primary.moving_ticks [ i ] [ 0 ] = false;

				//	if we just stopped moving, set the delta
				if ( primary.moving_ticks [ i ] [ 1 ] == true && primary.moving_ticks [ i ] [ 0 ] == false && primary.last_velocity [ i ] != e->GetVelocity( ).Length2D( ) ) {
					primary.delt [ i ] = GetDelta( primary.last_lby_for_delt [ i ], e->LowerBodyYaw( ) );
				}

				//	check delta between lbys for spinbot checks
				auto oldspin = primary.former_lbys_for_spin [ i ] [ 1 ] != primary.former_lbys_for_spin [ i ] [ 0 ];
				auto newspin = primary.former_lbys_for_spin [ i ] [ 0 ] != e->LowerBodyYaw( );
				primary.spin_ticks [ i ]++;

				//	check delta between calculated deltas and set spinning bool accordingly
				if ( oldspin && newspin )
					primary.is_spinning [ i ] = true;
				else
					primary.is_spinning [ i ] = false;

				//	set spin check lby array
				primary.former_lbys_for_spin [ i ] [ 1 ] = primary.former_lbys_for_spin [ i ] [ 0 ];
				primary.former_lbys_for_spin [ i ] [ 0 ] = e->LowerBodyYaw( );

				//	decide which method we resolve with on standing or fwalking players
				if ( primary.is_spinning [ i ] && primary.spin_ticks [ i ] > 1 ) {
					final_angle.yaw = e->LowerBodyYaw( );
					resolvertype [ i ] = "Spin";
				}
				else if ( misses < 2 ) {
					switch ( misses % 2 ) {
					case 0: final_angle.yaw = e->LowerBodyYaw( ) - primary.delt [ i ]; resolvertype [ i ] = "Moment Delta (-)"; break;
					case 1: final_angle.yaw = e->LowerBodyYaw( ) + primary.delt [ i ]; resolvertype [ i ] = "Moment Delta (+)"; break;
					}
				}
				else {
					resolvertype [ i ] = "Brute";
					switch ( misses % 3 ) {
					case 0: final_angle.yaw = e->LowerBodyYaw( ) - Bruteforce( e ); resolvertype [ i ] = "Brute"; break;
					case 1: final_angle.yaw = e->LowerBodyYaw( ) - primary.delt [ i ]; resolvertype [ i ] = "Moment Delta (-)"; break;
					case 2: final_angle.yaw = e->LowerBodyYaw( ) + primary.delt [ i ]; resolvertype [ i ] = "Moment Delta (+)"; break;
					}
				}
			}

			final_angle.pitch = e->GetEyeAngles( ).pitch;
			e->GetEyeAngles( ) = final_angle;
			primary.last_velocity [ i ] = e->GetVelocity( ).Length2D( );
			ClampYaw( e->GetEyeAngles( ).yaw );
		}
	}
}

void Resolver::Tertiary_Resolver( ) {
	//	antipaste
}

void Resolver::resolve_air( C_BaseEntity* ent ) {
	auto local = g_pEntityList->GetClientEntity( g_pEngine->GetLocalPlayer( ) );
	QAngle angle = Math::CalcAngle2( local->GetEyePos( ), ent->GetOrigin( ) );

	int idx = ent->GetIndex( );
	switch ( ( shots [ idx ] - hit_shots [ idx ] ) % 4 ) {
	case 0:
		ent->GetEyeAngles( ).yaw = angle.yaw + 180; break;
	case 1:
		ent->GetEyeAngles( ).yaw = angle.yaw + 90; break;
	case 2:
		ent->GetEyeAngles( ).yaw = angle.yaw - 90; break;
	case 3:
		ent->GetEyeAngles( ).yaw = angle.yaw; break;
	}

	//probs fuckt
	if ( ent->GetEyeAngles( ).pitch < -89 || ent->GetEyeAngles( ).pitch > 89 ) {
		if ( nospread.hit_pitch [ idx ] != 0 ) {
			ent->GetEyeAngles( ).pitch = nospread.hit_pitch [ idx ];
			nospread.last_pitch [ idx ] = nospread.hit_pitch [ idx ];
			nospread.hit_pitch [ idx ] = 0;
		}
		else {
			switch ( ( shots [ idx ] - hit_shots [ idx ] ) % 2 ) {
			case 0:
				ent->GetEyeAngles( ).pitch = 89;
				nospread.last_pitch [ idx ] = 89;
				break;
			case 1:
				ent->GetEyeAngles( ).pitch = -89;
				nospread.last_pitch [ idx ] = -89;
				break;
			}
		}
	}

	ClampYaw( ent->GetEyeAngles( ).yaw );
}
