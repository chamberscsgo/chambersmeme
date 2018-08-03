#pragma once
#include "SDK.h"
#pragma pack(push, 1)
class CCSWeaponInfo
{
public:
	byte _0x0000 [ 20 ];
	int max_clip;
	byte _0x0018 [ 12 ];
	int max_reserved_ammo; //0x0024 
	byte _0x0028 [ 88 ];
	char* bullet_type; //0x0080 
	byte _0x0083 [ 4 ];
	char* hud_name; //0x0088 
	char* weapon_name;
	byte _0x0090 [ 60 ];
	int weapon_type;
	int price;
	int reward;
	byte _0x00D8 [ 20 ];
	bool full_auto;
	byte _0x00ED [ 3 ];
	int damage;
	float armor_ratio;
	int bullets;
	float penetration;
	byte _0x0100 [ 8 ];
	float range;
	float range_modifier;
	byte _0x0110 [ 16 ];
	bool silencer;
	byte _0x0121 [ 15 ];
	float max_speed;
	float max_speed_alt;
	byte _0x0138 [ 76 ];
	int recoil_seed;
	byte _0x0188 [ 32 ];
	//char* bullet_type; //0x0080 
};
#pragma pack(pop)

class C_BaseWeapon
{
public:
	int OwnerEntity ( );
	float FireReadyTime ( );
	bool PinPulled ( );
	float ThrowTime ( );
	CCSWeaponInfo* GetCSWeaponData ( );
	bool IsInThrow ( CUserCmd * cmd );
	float GetInaccuracy ( );
	float UpdateInaccuracy ( );
	float GetSpread ( );
	float NextPAtt ( );
	short GetItemDefinitionIndex ( );
	int Clip1 ( );
	bool IsReloading ( );
	int Clip2 ( );
	bool IsGun ( );
	int GetXUIDLow ( );
	int GetXUIDHigh ( );
	int GetEntityQuality ( );
	int GetAccountID ( );
	int GetItemIDHigh ( );
	int GetFallbackPaintKit ( );
	int GetFallbackStatTrak ( );
	int GetFallbackSeed ( );
	int GetWeaponType ( );
	float GetFallbackWear ( );
	std::string GetWeaponName ( );
};

