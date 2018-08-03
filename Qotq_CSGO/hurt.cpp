#include "SDK.h"

void PlayerHurtEvent::FireGameEvent ( IGameEvent *event )
{
	auto local = g_pEntityList->GetClientEntity ( g_pEngine->GetLocalPlayer ( ) );
	if ( !local || !event )
		return;

	if ( g_pEngine->GetPlayerForUserID ( event->GetInt ( "attacker" ) ) == g_pEngine->GetLocalPlayer ( ) && g_pEngine->GetPlayerForUserID ( event->GetInt ( "userid" ) ) != g_pEngine->GetLocalPlayer ( ) )
	{
		g_pEngine->ExecuteClientCmd ( "play buttons\\arena_switch_press_02.wav" ); // No other fitting sound. Probs should use a resource
	}


	if ( g_Settings.aimbot.resolver != 0 ) {
		auto player = ( C_BaseEntity* ) g_pEntityList->GetClientEntity ( event->GetInt ( "userid" ) );
		if ( !player )
			return;

		int idx = player->GetIndex ( );

		if ( !player->GetDormant ( ) )
		{
			int local_id = g_pEngine->GetLocalPlayer ( );
			int attacker = g_pEngine->GetPlayerForUserID ( event->GetInt ( "attacker" ) );

			if ( attacker == local_id )
			{
				Resolver::Get ( ).hit_shots [ idx ]++;
				Resolver::Get ( ).nospread.hit_pitch [ idx ] = Resolver::Get ( ).nospread.last_pitch [ idx ];
			}
		}
	}
}

int PlayerHurtEvent::GetEventDebugID ( void )
{
	return EVENT_DEBUG_ID_INIT;
}

void PlayerHurtEvent::RegisterSelf ( )
{
	g_pGameEvents->AddListener ( this , "player_hurt" , false );
}

void PlayerHurtEvent::UnregisterSelf ( )
{
	g_pGameEvents->RemoveListener ( this );
}