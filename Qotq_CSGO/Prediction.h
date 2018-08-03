#pragma once
#include "SDK.h"

class PredictionSystem : public singleton<PredictionSystem>
{

public:
	void Start ( CUserCmd *userCMD , C_BaseEntity* player );
	void End ( C_BaseEntity* player );

	void FixTicks ( CUserCmd * ucmd );
	int CorrectTick;
private:

	float m_flOldCurTime;
	float m_flOldFrametime;

	CMoveData moveData;

	int *predictionRandomSeed;
	C_BaseEntity *predictionPlayer;
};

#define FIXED_CTIME_CM() ((PredictionSystem::Get().CorrectTick + 1) * g_pGlobalVars->interval_per_tick)
#define FIXED_CTIME() (PredictionSystem::Get().CorrectTick * g_pGlobalVars->interval_per_tick)