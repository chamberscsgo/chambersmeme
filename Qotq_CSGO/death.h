#pragma once
#include "SDK.h"

class PlayerDeath : public IGameEventListener2 , public singleton<BulletImpactEvent>
{
public:

	void FireGameEvent ( IGameEvent *event );
	int  GetEventDebugID ( void );

	void RegisterSelf ( );
	void UnregisterSelf ( );
};