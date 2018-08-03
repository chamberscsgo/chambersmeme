#pragma once
#include "SDK.h"

#include <vector>

struct HitMarkerInfo
{
	float m_flExpTime;
	int m_iDmg;
};

struct EventInfo
{
	std::string m_szMessage;
	float m_flExpTime;
};

class PlayerHurtEvent : public IGameEventListener2 , public singleton<PlayerHurtEvent>
{
public:

	void FireGameEvent ( IGameEvent *event );
	int  GetEventDebugID ( void );

	void RegisterSelf ( );
	void UnregisterSelf ( );

	void Paint ( void );

private:

	std::vector<HitMarkerInfo> hitMarkerInfo;
	std::vector<EventInfo> eventInfo;
};