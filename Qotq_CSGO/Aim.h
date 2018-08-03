#pragma once
#include "SDK.h"
#include "singleton.h"

class CAimbot : public singleton<CAimbot>
{
public:
	void run ( CUserCmd* cmd );
	std::vector<C_BaseEntity*> vec_entities;
	C_BaseEntity* former_target_ent;
	float fl_mindmg;
	bool can_shoot ( );
	std::vector<int> vec_head_hitboxes;
	std::vector<int> vec_body_hitboxes;
private:
	bool find_target ( );
	void aim_at_enemy ( );
	bool hitchance_viable ( );
	void populate_hitboxes ( );
	bool sanity_check ( C_BaseEntity* ent );
	CUserCmd* usercmd;
	C_BaseEntity* local;
};