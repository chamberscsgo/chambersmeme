#include "death.h"

void PlayerDeath::FireGameEvent ( IGameEvent *event )
{
	auto local = g_pEntityList->GetClientEntity ( g_pEngine->GetLocalPlayer ( ) );
	if ( !local || !event )
		return;

}

int PlayerDeath::GetEventDebugID ( void )
{
	return EVENT_DEBUG_ID_INIT;
}

void PlayerDeath::RegisterSelf ( )
{
	g_pGameEvents->AddListener ( this , "player_death" , false );
}

void PlayerDeath::UnregisterSelf ( )
{
	g_pGameEvents->RemoveListener ( this );
}