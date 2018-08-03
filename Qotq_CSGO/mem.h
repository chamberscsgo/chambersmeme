#pragma once
#include "SDK.h"
#include "Enums.h"
class AnimationLayer;

//	what's this? what's this? organization everywhere. what's this? what's this? classes everywhere

class AnimationLayer
{
public:
	char  pad_0000 [ 20 ];
	// These should also be present in the padding, don't see the use for it though
	//float	m_flLayerAnimtime;
	//float	m_flLayerFadeOuttime;
	uint32_t m_nOrder; //0x0014
	uint32_t m_nSequence; //0x0018
	float_t m_flPrevCycle; //0x001C
	float_t m_flWeight; //0x0020
	float_t m_flWeightDeltaRate; //0x0024
	float_t m_flPlaybackRate; //0x0028
	float_t m_flCycle; //0x002C
	void *m_pOwner; //0x0030 // player's thisptr
	char  pad_0038 [ 4 ]; //0x0034
}; //Size: 0x0038

class CBasePlayerAnimState
{
public:// Class has no vtable.
	byte    pad_0x0 [ 0x5F ];
	void*    base_entity;
	void*    active_weapon;
	void*    last_active_weapon;
	float    last_client_side_animation_update_time;
	int      last_client_side_animation_update_framecount;
	float    eye_pitch;
	float    eye_yaw;
	float    pitch;
	float    goal_feet_yaw;
	float    current_feet_yaw;
	float    current_torso_yaw;
	float    unknown_velocity_lean;
	float    lean_amount;
	byte    pad_0x94 [ 0x4 ];
	float    feet_cycle;
	float    feet_yaw_rate;
	byte    pad_0x9C [ 0x1 ];
	float    duck_amount;
	float    landing_duck_additive;
	byte    pad_0xAC [ 0x1 ];
	Vector    origin;
	Vector    last_origin;
	float    velocity_x;
	float    velocity_y;
	byte    pad_0xD0 [ 0x1C ];
	float    speed_2d;
	float    up_velocity;
	float    speed_normalized;
	float    feet_speed_forwards_or_sideways;
	float    feet_speed_unknown_forwards_or_sideways;
	float    time_since_started_moving;
	float    time_since_stopped_moving;
	bool    on_ground;
	bool    in_hit_ground_animation;
	byte    pad_0x10D [ 0xA ];
	float    last_origin_z;
	float    head_height_from_hitting_ground_animation;
	float    stop_to_full_running_fraction;
};

class CCSPlayerAnimState
{
public:

	virtual ~CCSPlayerAnimState ( ) = 0;
	virtual void Update ( float_t eyeYaw , float_t eyePitch ) = 0;
};

class CCSGOPlayerAnimState
{
public:

	Vector GetVecVelocity ( )
	{
		// Only on ground velocity
		return *( Vector* ) ( ( uintptr_t ) this + 0xC8 );
	}

	float GetVelocity ( )
	{
		return *( float* ) ( ( uintptr_t ) this + 0xEC );
	}

	char pad_0x0000 [ 0x344 ]; //0x0000
}; //Size=0x344