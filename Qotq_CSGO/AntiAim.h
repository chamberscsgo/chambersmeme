#pragma once
#include "singleton.h"
#include "SDK.h"

class AA : public singleton<AA>
{
public:
	void Run ( CUserCmd* cmd );
	void PerformLag ( );
	void FixMovement ( CUserCmd *usercmd , QAngle &wish_angle );

	//	breaker
	bool m_bBreakLowerBody = false;
	void UpdateLBYBreaker ( CUserCmd* cmd );

	CUserCmd *usercmd = nullptr;
private:
	bool did_just_hit_ground;
	bool did_just_hit_ground_last_tick;
	int chokedticks;
	float ctime;
	float lasttime;
	int left;
	void PerformManual ( bool isf );
	void PerformSpin ( bool isf );
	void PerformStatic ( bool isf );
	void PerformJitter ( bool isf );
	void Perform180z ( bool isf );
};