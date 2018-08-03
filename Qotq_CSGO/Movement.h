#pragma once
#include "SDK.h"
#include "singleton.h"
class CMovement : public singleton<CMovement> {
public:
	void Bhop ( C_BaseEntity* pLocal , CUserCmd* cmd );
	void NasaWalk ( INetChannel* netchan );
	void Pitchonshoot ( INetChannel* netchan );
	void Fakewalk ( CUserCmd* usercmd );
	void StartCStrafe ( CUserCmd * userCMD );
	void CStrafe ( CUserCmd *userCMD );
	void AutoStrafe ( CUserCmd *userCMD );

	int fwchoked = 0;
	bool fwing = false;
};