#include "Aim.h"
#include "AutoWall.h"
#include "LagComp.h"

void CAimbot::run( CUserCmd* cmd ) {
	usercmd = cmd;
	aimbotting = false;
	local = g_pEntityList->GetClientEntity( g_pEngine->GetLocalPlayer( ) );

	if ( !g_Settings.aimbot.enabled )
		return;

	if ( !local || !g_pEngine->IsInGame( ) || local->GetHealth( ) < 1 || !g_pEngine->IsConnected( ) )
		return;

	if ( !find_target( ) )
		return;

	if ( !can_shoot( ) )
		return;

	if ( !hitchance_viable( ) )
		return;

	populate_hitboxes( );

	aim_at_enemy( );

	//norecoil
	auto weapon = local->GetActiveWeapon( );
	if ( weapon )
		weapon->UpdateInaccuracy( );
	usercmd->viewangles -= local->GetPunchAngles( ) * 2;
}

void CAimbot::aim_at_enemy( ) {
	//init our vars
	Vector vec_best_hitbox_pos;
	QAngle qa_best_aim_angle;
	int i_best_tick = usercmd->tick_count;
	float fl_best_damage = 0.f;
	C_BaseEntity* target_entity = NULL;

	//temporary lambda while awall is broken
	auto can_trace_to = [ & ] ( C_BaseEntity* ent, Vector vec_startpos, Vector vec_endpos ) {
		Ray_t ray;
		trace_t trace;
		CTraceFilter filter;
		filter.pSkip = ent;

		ray.Init( vec_startpos, vec_endpos );
		g_pEngineTrace->TraceRay( ray, MASK_SHOT_HULL, &filter, &trace );

		return trace.fraction > 0.97f || trace.hit_entity == 0;
	};

	for ( auto& player : vec_entities )
	{
		Vector playerBestPoint;
		float playerBestDamage = 0.f;
		int besttick;

		C_BaseEntity* pPlayer = player;
		if ( !pPlayer )
			continue;

		LagRecord trec = CBacktrack::Get( ).CreateRecord( pPlayer );

		auto awall_points = [ & ] ( int tick, int& best_tick, Vector& bestpoint, float& damage ) -> void {
			bool dohead = true;

			if ( !g_Settings.aimbot.headonly ) {
				std::vector<Vector> points;
				for ( auto& hitbox : vec_body_hitboxes )
				{
					if ( !pPlayer->GetPointsFromHitbox( local, hitbox, points ) )
						continue;
				}

				for ( auto& point : points )
				{
					float tempdamage = Autowall::Get( ).CanHit( point );

					if ( tempdamage > damage )
					{
						best_tick = tick;
						bestpoint = point;
						damage = tempdamage;
						if ( tempdamage > pPlayer->GetHealth( ) ) {
							dohead = false;
						}
					}
				}
			}

			//check head hitboxes
			if ( g_Settings.aimbot.headonly || ( !g_Settings.aimbot.headonly && pPlayer->GetFlags( ) & FL_ONGROUND ) )
				if ( dohead ) {
					std::vector<Vector> headPoints;
					for ( auto& hitbox : vec_head_hitboxes )
					{
						if ( !pPlayer->GetPointsFromHitbox( local, hitbox, headPoints ) )
							continue;
					}

					for ( auto& point : headPoints )
					{
						float tempdamage = Autowall::Get( ).CanHit( point );
						if ( tempdamage > damage )
						{
							best_tick = tick;
							bestpoint = point;
							damage = tempdamage;
						}
					}
				}
		};

		auto is_lethal = [ & ] ( LagRecord& record, C_BaseEntity* entity ) -> bool {
			if ( playerBestDamage >= pPlayer->GetHealth( ) ) {
				target_entity = pPlayer;
				vec_best_hitbox_pos = playerBestPoint;
				fl_best_damage = playerBestDamage;
				i_best_tick = besttick;
				CBacktrack::Get( ).RestoreRecord( trec, pPlayer );
				return true;
			}
			return false;
		};

		//	best tick
		int temp_tick = CBacktrack::Get( ).GetBestTick( pPlayer );
		awall_points( temp_tick, besttick, playerBestPoint, playerBestDamage );

		if ( is_lethal( trec, pPlayer ) )
			break;

		//	first tick
		int temp_tick_2 = CBacktrack::Get( ).GetFirstTick( pPlayer );
		awall_points( temp_tick_2, besttick, playerBestPoint, playerBestDamage );

		if ( is_lethal( trec, pPlayer ) )
			break;

		CBacktrack::Get( ).RestoreRecord( trec, pPlayer );

		if ( playerBestDamage > fl_mindmg )
			if ( playerBestDamage > fl_best_damage )
			{
				target_entity = pPlayer;
				vec_best_hitbox_pos = playerBestPoint;
				fl_best_damage = playerBestDamage;
				i_best_tick = besttick;
			}
	}

	if ( !target_entity )
		return;

	if ( !sanity_check( target_entity ) )
		return;

	former_target_ent = target_entity;

	//set aim angle to equal the angle between our eyepos and the vector of the hitbox
	qa_best_aim_angle = Math::CalcAngle2( local->GetEyePos( ), vec_best_hitbox_pos );
	qa_best_aim_angle.Normalize( ); //sanity
	qa_best_aim_angle.Clamp( ); //sanity

	static int vars_delaybla = 2;
	static int delay = 0;
	//pasted cause it helps
	if ( !( usercmd->buttons & IN_ATTACK ) )
	{
		if ( delay < vars_delaybla )
		{
			delay++;
			return;
		}
		delay = 0;

		usercmd->buttons |= IN_ATTACK;
	}

	if ( usercmd->buttons & IN_ATTACK )
	{
		usercmd->tick_count = i_best_tick;
		usercmd->viewangles = qa_best_aim_angle;
		Resolver::Get( ).shots [ target_entity->GetIndex( ) ]++;
		aimbotting = true;
	}
}

bool CAimbot::find_target( ) {
	vec_entities.clear( );
	for ( int i = 0; i < g_pGlobalVars->maxClients; i++ )
	{
		auto ent = reinterpret_cast< C_BaseEntity* >( g_pEntityList->GetClientEntity( i ) );
		if ( !sanity_check( ent ) )
			continue;
		vec_entities.emplace_back( ent );
	}
	if ( vec_entities.size( ) > 0 )
		return true;

	return false;
}

void CAimbot::populate_hitboxes( ) {
	vec_head_hitboxes.clear( );
	vec_body_hitboxes.clear( );

	if ( g_Settings.aimbot.headonly ) {
		vec_head_hitboxes.emplace_back( HITBOX_HEAD );
	}
	else {
		vec_head_hitboxes.emplace_back( HITBOX_HEAD );
		vec_head_hitboxes.emplace_back( HITBOX_NECK );
		vec_body_hitboxes.emplace_back( HITBOX_PELVIS );
		vec_body_hitboxes.emplace_back( HITBOX_STOMACH );
		vec_body_hitboxes.emplace_back( HITBOX_LOWER_CHEST );
		vec_body_hitboxes.emplace_back( HITBOX_CHEST );
		vec_body_hitboxes.emplace_back( HITBOX_UPPER_CHEST );
		vec_body_hitboxes.emplace_back( HITBOX_RIGHT_THIGH );
		vec_body_hitboxes.emplace_back( HITBOX_LEFT_THIGH );
		vec_body_hitboxes.emplace_back( HITBOX_RIGHT_CALF );
		vec_body_hitboxes.emplace_back( HITBOX_LEFT_CALF );
		vec_body_hitboxes.emplace_back( HITBOX_RIGHT_FOOT );
		vec_body_hitboxes.emplace_back( HITBOX_LEFT_FOOT );
		vec_body_hitboxes.emplace_back( HITBOX_RIGHT_HAND );
		vec_body_hitboxes.emplace_back( HITBOX_LEFT_HAND );
		vec_body_hitboxes.emplace_back( HITBOX_RIGHT_UPPER_ARM );
		vec_body_hitboxes.emplace_back( HITBOX_RIGHT_FOREARM );
		vec_body_hitboxes.emplace_back( HITBOX_LEFT_UPPER_ARM );
		vec_body_hitboxes.emplace_back( HITBOX_LEFT_FOREARM );
	}
}

bool CAimbot::can_shoot( ) {
	auto weapon = reinterpret_cast< C_BaseWeapon* >( local->GetActiveWeapon( ) );

	if ( !weapon )
		return false;

	if ( weapon->GetWeaponType( ) == WEAPONTYPE_KNIFE || weapon->GetWeaponType( ) == WEAPONTYPE_C4/* || weapon->GetWeaponType() == WEAPONTYPE_GRENADE*/ )
		return false;

	if ( weapon->Clip1( ) < 1 )
		return false;

	if ( weapon->IsReloading( ) )
		return false;

	if ( weapon->NextPAtt( ) > g_pGlobalVars->curtime )
		return false;

	if ( local->NextAttack( ) > g_pGlobalVars->curtime )
		return false;

	return true;
}

bool CAimbot::hitchance_viable( ) {
	auto weapon = reinterpret_cast< C_BaseWeapon* >( local->GetActiveWeapon( ) );

	if ( !weapon )
		return false;

	float fl_hitchance;
	float fl_hc_amount;
	int i_weapindex = weapon->GetItemDefinitionIndex( );
	int i_weaptype = weapon->GetCSWeaponData( )->weapon_type;

	if ( i_weapindex == WEAPON_SSG08 ) {
		fl_mindmg = 30;
		fl_hc_amount = 80;
	}
	else if ( i_weapindex == WEAPON_G3SG1 || i_weapindex == WEAPON_SCAR20 ) {
		fl_mindmg = 10;
		fl_hc_amount = 50;
	}
	else if ( i_weapindex == WEAPON_AWP ) {
		fl_mindmg = 60;
		fl_hc_amount = 80;
	}
	else if ( i_weapindex == WEAPON_DEAGLE || i_weapindex == WEAPON_REVOLVER ) {
		fl_mindmg = 40;
		fl_hc_amount = 65;
	}
	else if ( i_weaptype == WEAPONTYPE_PISTOL ) {
		fl_mindmg = 10;
		fl_hc_amount = 25;
	}
	else if ( i_weaptype == WEAPONTYPE_RIFLE ) {
		fl_mindmg = 20;
		fl_hc_amount = 40;
	}
	else {
		fl_mindmg = 10;
		fl_hc_amount = 30;
	}

	//Inaccuracy method
	float fl_inaccuracy = weapon->GetInaccuracy( );
	if ( fl_inaccuracy == 0 ) fl_inaccuracy = 0.0000001;
	fl_inaccuracy = 1 / fl_inaccuracy;
	fl_hitchance = fl_inaccuracy;

	if ( fl_hc_amount * 1.5 > fl_hitchance )
		return false;

	return true;
}

bool CAimbot::sanity_check( C_BaseEntity* ent ) {
	if ( !ent )
		return false;

	if ( ent->GetDormant( ) )
		return false;

	if ( ent->GetHealth( ) < 1 )
		return false;

	if ( ent->GetFlags( ) & FL_FROZEN )
		return false;

	if ( ent->GetClientClass( )->m_ClassID != 35 )
		return false;

	if ( ent->GetTeam( ) == local->GetTeam( ) )
		return false;

	return true;
}
