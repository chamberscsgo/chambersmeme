#pragma once
#include "SDK.h"
#include "mem.h"

class C_BaseEntity {
public:
	float SpawnTime ( );
	bool IsPlayer ( );
	bool IsWeapon ( );
	ICollideable* GetCollideable ( );
	int & GetForceBone ( );
	const model_t* GetModel ( );
	bool PVS_SetupBones ( );
	bool SetupBones ( matrix3x4_t *pBoneToWorldOut , int nMaxBones , int boneMask , float currentTime );
	int SetObserverMode ( int mode );
	int GetObserverTarget ( );
	float &LowerBodyYaw ( );
	std::array<float , 24> &PoseParams ( );
	float NextAttack ( );
	int GetNumAnimOverlays ( );
	AnimationLayer *GetAnimOverlays ( );
	AnimationLayer *GetAnimOverlay ( int i );
	int GetSequenceActivity ( int sequence );
	CBasePlayerAnimState *GetBasePlayerAnimState ( );
	CCSPlayerAnimState *GetPlayerAnimState ( );
	static void UpdateAnimationState ( CCSGOPlayerAnimState *state , QAngle angle );
	static void ResetAnimationState ( CCSGOPlayerAnimState *state );
	void CreateAnimationState ( CCSGOPlayerAnimState *state );
	void UpdateClientSideAnimation ( );

	ClientClass* GetClientClass ( );
	int GetIndex ( );
	void SetAbsOrigin ( Vector vec );
	void SetAbsAngles ( QAngle vec );
	void SetFlags ( int flags );
	QAngle & GetAbsAngles ( );
	int &GetFlags ( );
	void InvalidatePhysicsRecursive ( int nChangeFlags );
	void InvalidateBoneCache ( );
	Vector GetViewOffset ( );
	Vector GetEyePos ( );
	int GetHealth ( );
	int GetTeam ( );
	int GetPing ( );
	Vector GetMins ( );
	Vector GetMaxs ( );
	Vector GetVelocity ( );
	QAngle &GetEyeAngles ( );
	int GetFiredShots ( );
	bool GetDormant ( );
	Vector GetOrigin ( );
	QAngle GetPunchAngles ( );
	QAngle GetVPunchAngles ( );
	CBaseHandle hActiveWeapon ( );
	C_BaseWeapon* GetActiveWeapon ( );
	bool &DoClientSideAnimation ( );
	bool GetScoped ( );
	float &SimTime ( );
	const matrix3x4_t& GetCoordinateFrame ( );
	Vector GetHitboxPos ( int hitbox_id );
	bool GetHitboxPos ( int hitbox , Vector &output );
	Vector GetBonePos ( int bone );
	bool CanSeePlayer ( C_BaseEntity* player , int hitbox );
	bool CanSeePlayer ( C_BaseEntity* player , const Vector& pos );
	bool bSpotted ( );
	QAngle &visuals_Angles ( );
	bool GetPointsFromHitbox ( C_BaseEntity* pBaseEntity , const int& iHitbox , std::vector<Vector>& points );
	int GetArmor ( );
	bool HeavyArmor ( );
	bool Hashelmet ( );
	int DrawModel ( int flags , uint8_t alpha );
	int GetTickBase ( );
	int GetMoney ( );
	int GetHitboxSet ( );
	MoveType_t GetMoveType ( );
};