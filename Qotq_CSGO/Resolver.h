#pragma once
#include "SDK.h"
#include <deque>
#include "mem.h"

class Resolver : public singleton<Resolver>
{
public:
	float Bruteforce( C_BaseEntity * ent );
	void Run( );
	void Tertiary_Resolver( );
	void Experimental_Resolver( );
	void Default_Resolver( );
	void resolve_air( C_BaseEntity* ent );

	struct {
		float hit_pitch [ 65 ];
		float last_pitch [ 65 ];
	} nospread;

	struct {
		float last_lby [ 65 ];
		float last_lby_for_delt [ 65 ];
		float last_velocity [ 65 ];
		bool moving_ticks [ 65 ] [ 2 ];

		float former_lbys_for_spin [ 65 ] [ 2 ];
		bool is_spinning [ 65 ];
		int spin_ticks [ 65 ];

		float delt [ 65 ];
		float last_balance_adjust_trigger_time [ 65 ];
		AnimationLayer previous_animlayers [ 15 ];

		bool is_fw [ 65 ];

		float stored_simtime [ 65 ];
		bool ischoking [ 65 ];
	} primary;

	struct {
		float last_moving_lby;
		float last_updated_lby;

		float relative_delta;
		int force_side = 1;

		float last_simtime;
		float last_update_time;

		bool is_prediction_valid;
		bool is_lby_valid;

		float last_velocity_length2d;

		Vector dormant_pos;
		Vector seen_pos;
	} secondary [ 64 ];

	struct {
		int layer_count;
		AnimationLayer animation_layers [ 15 ];
		float last_moving_lby;
		bool fakewalking;

		float lby_timer;
		bool update_lby;
		float last_lby;
		float last_standing_lby_update;

		bool should_resolve_delta_180;
		bool suppressing_979;

		bool has_lby_breaker;
	} tertiary [ 64 ];

	int shots [ 65 ], hit_shots [ 65 ];
	std::string resolvertype [ 65 ];

	const inline float ClampYaw( float yaw )
	{
		while ( yaw > 180.f )
			yaw -= 360.f;
		while ( yaw < -180.f )
			yaw += 360.f;
		return yaw;
	}

	const inline float GetDelta( float a, float b )
	{
		float yaw = a - b;
		while ( yaw > 180.f )
			yaw -= 360.f;
		while ( yaw < -180.f )
			yaw += 360.f;
		return yaw;
	}
};