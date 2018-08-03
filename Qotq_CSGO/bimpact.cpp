#include "SDK.h"


void BulletImpactEvent::FireGameEvent ( IGameEvent *event )
{
	auto local = g_pEntityList->GetClientEntity ( g_pEngine->GetLocalPlayer ( ) );
	if ( !local || !event )
		return;
	int idx = event->GetInt ( "userid" );
	auto player = ( C_BaseEntity* ) g_pEntityList->GetClientEntity ( idx );
	if ( !player )
		return;

	if ( !player->GetDormant ( ) )
	{
		int local_id = g_pEngine->GetLocalPlayer ( );
		int attacker = g_pEngine->GetPlayerForUserID ( event->GetInt ( "attacker" ) );
		int assister = g_pEngine->GetPlayerForUserID ( event->GetInt ( "assister" ) );

		if ( attacker == local_id || assister == local_id )
		{
			if ( !event->GetBool ( "cs_win_panel_round" ) )
				event->SetInt ( "cs_win_panel_round" , 1 );
		}
	}

}

int BulletImpactEvent::GetEventDebugID ( void )
{
	return EVENT_DEBUG_ID_INIT;
}

void BulletImpactEvent::RegisterSelf ( )
{
	g_pGameEvents->AddListener ( this , "bullet_impact" , false );
}

void BulletImpactEvent::UnregisterSelf ( )
{
	g_pGameEvents->RemoveListener ( this );
}