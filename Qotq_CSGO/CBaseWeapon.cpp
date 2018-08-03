#include "CBaseWeapon.h"
int C_BaseWeapon::OwnerEntity ( )
{
	return *reinterpret_cast< int* >( ( uintptr_t ) this + NetVars.m_hOwner );
}
float C_BaseWeapon::FireReadyTime ( ) {
	return *reinterpret_cast< float* >( ( uintptr_t ) this + NetVars.m_flPostponeFireReadyTime );
}
bool C_BaseWeapon::PinPulled ( ) {
	return *reinterpret_cast< float* >( ( uintptr_t ) this + NetVars.m_bPinPulled );
}
float C_BaseWeapon::ThrowTime ( ) {
	return *reinterpret_cast< float* >( ( uintptr_t ) this + NetVars.m_fThrowTime );
}
CCSWeaponInfo* C_BaseWeapon::GetCSWeaponData ( )
{
	if ( !this ) return nullptr;
	typedef CCSWeaponInfo*( __thiscall* OriginalFn )( void* );
	return VT::VFunc<OriginalFn> ( this , 445 )( this );
}
bool C_BaseWeapon::IsInThrow ( CUserCmd* cmd )
{
	if ( !PinPulled ( ) || ( cmd->buttons & IN_ATTACK ) || ( cmd->buttons & IN_ATTACK2 ) )
	{
		float throwTime = ThrowTime ( );

		if ( throwTime > 0 )
			return true;
	}
	return false;
}
float C_BaseWeapon::GetInaccuracy ( )
{
	typedef float ( __thiscall* oInaccuracy )( PVOID );
	return VT::VFunc<oInaccuracy> ( this , 468 )( this );
}
float C_BaseWeapon::UpdateInaccuracy ( )
{
	typedef float ( __thiscall* oInaccuracy )( PVOID );
	return VT::VFunc<oInaccuracy> ( this , 469 )( this );
}
float C_BaseWeapon::GetSpread ( )
{
	typedef float ( __thiscall* oInaccuracy )( PVOID );
	return VT::VFunc<oInaccuracy> ( this , 437 )( this );
}
float C_BaseWeapon::NextPAtt ( ) {
	return *reinterpret_cast< float* >( ( uintptr_t ) this + NetVars.m_flNextPrimaryAttack );
}
short C_BaseWeapon::GetItemDefinitionIndex ( ) {
	return *reinterpret_cast< int* >( ( uintptr_t ) this + NetVars.m_iItemDefinitionIndex );
}
int C_BaseWeapon::Clip1 ( ) {
	return *reinterpret_cast< int* >( ( uintptr_t ) this + NetVars.m_iClip1 );
}
bool C_BaseWeapon::IsReloading ( )
{
	static auto inReload = *( uint32_t* ) ( g_pUtils->FindPatternIDA ( ( "client.dll" ) , "C6 87 ? ? ? ? ? 8B 06 8B CE FF 90" ) + 2 );
	return *( bool* ) ( ( uintptr_t ) this + inReload );
}
int C_BaseWeapon::Clip2 ( ) {
	return *reinterpret_cast< int* >( ( uintptr_t ) this + NetVars.m_iClip2 );
}
bool C_BaseWeapon::IsGun ( ) {
	if ( !this )
		return false;

	int id = this->GetItemDefinitionIndex ( );

	switch ( id )
	{
	case WEAPON_DEAGLE:
	case WEAPON_ELITE:
	case WEAPON_FIVESEVEN:
	case WEAPON_GLOCK:
	case WEAPON_AK47:
	case WEAPON_AUG:
	case WEAPON_AWP:
	case WEAPON_FAMAS:
	case WEAPON_G3SG1:
	case WEAPON_GALILAR:
	case WEAPON_M249:
	case WEAPON_M4A1:
	case WEAPON_MAC10:
	case WEAPON_P90:
	case WEAPON_UMP45:
	case WEAPON_XM1014:
	case WEAPON_BIZON:
	case WEAPON_MAG7:
	case WEAPON_NEGEV:
	case WEAPON_SAWEDOFF:
	case WEAPON_TEC9:
	case WEAPON_HKP2000:
	case WEAPON_MP7:
	case WEAPON_MP9:
	case WEAPON_NOVA:
	case WEAPON_P250:
	case WEAPON_SCAR20:
	case WEAPON_SG556:
	case WEAPON_SSG08:
	case WEAPON_M4A1_SILENCER:
	case WEAPON_USP_SILENCER:
	case WEAPON_CZ75A:
	case WEAPON_REVOLVER:
		return true;
	default:
		return false;
	}
}
int C_BaseWeapon::GetXUIDLow ( ) {
	return *reinterpret_cast< int* >( ( uintptr_t ) this + NetVars.m_OriginalOwnerXuidLow );

}
int C_BaseWeapon::GetXUIDHigh ( ) {
	return *reinterpret_cast< int* >( ( uintptr_t ) this + NetVars.m_OriginalOwnerXuidHigh );

}
int C_BaseWeapon::GetEntityQuality ( ) {
	return *reinterpret_cast< int* >( ( uintptr_t ) this + NetVars.m_iEntityQuality );

}
int C_BaseWeapon::GetItemIDHigh ( ) {
	return *reinterpret_cast< int* >( ( uintptr_t ) this + NetVars.m_iItemIDHigh );

}
int C_BaseWeapon::GetFallbackPaintKit ( ) {
	return *reinterpret_cast< int* >( ( uintptr_t ) this + NetVars.m_nFallbackPaintKit );

}
int C_BaseWeapon::GetFallbackStatTrak ( ) {
	return *reinterpret_cast< int* >( ( uintptr_t ) this + NetVars.m_nFallbackStatTrak );

}
int C_BaseWeapon::GetFallbackSeed ( ) {
	return *reinterpret_cast< int* >( ( uintptr_t ) this + NetVars.m_nFallbackSeed );

}
float C_BaseWeapon::GetFallbackWear ( ) {
	return *reinterpret_cast< float* >( ( uintptr_t ) this + NetVars.m_flFallbackWear );

}
int C_BaseWeapon::GetWeaponType ( ) {
	if ( !this )
		return WEAPONTYPE_UNKNOWN;

	int id = this->GetItemDefinitionIndex ( );

	switch ( id )
	{
	case WEAPON_DEAGLE:
	case WEAPON_ELITE:
	case WEAPON_FIVESEVEN:
	case WEAPON_GLOCK:
		return WEAPONTYPE_PISTOL;
	case WEAPON_AK47:
	case WEAPON_AUG:
		return WEAPONTYPE_RIFLE;
	case WEAPON_AWP:
		return WEAPONTYPE_SNIPER_RIFLE;
	case WEAPON_FAMAS:
		return WEAPONTYPE_RIFLE;
	case WEAPON_G3SG1:
		return WEAPONTYPE_SNIPER_RIFLE;
	case WEAPON_GALILAR:
		return WEAPONTYPE_RIFLE;
	case WEAPON_M249:
		return WEAPONTYPE_MACHINEGUN;
	case WEAPON_M4A1:
		return WEAPONTYPE_RIFLE;
	case WEAPON_MAC10:
	case WEAPON_P90:
	case WEAPON_UMP45:
		return WEAPONTYPE_SUBMACHINEGUN;
	case WEAPON_XM1014:
		return WEAPONTYPE_SHOTGUN;
	case WEAPON_BIZON:
		return WEAPONTYPE_SUBMACHINEGUN;
	case WEAPON_MAG7:
		return WEAPONTYPE_SHOTGUN;
	case WEAPON_NEGEV:
		return WEAPONTYPE_MACHINEGUN;
	case WEAPON_SAWEDOFF:
		return WEAPONTYPE_SHOTGUN;
	case WEAPON_TEC9:
		return WEAPONTYPE_PISTOL;
	case WEAPON_TASER:
		return WEAPONTYPE_SHOTGUN;
	case WEAPON_HKP2000:
		return WEAPONTYPE_PISTOL;
	case WEAPON_MP7:
	case WEAPON_MP9:
		return WEAPONTYPE_SUBMACHINEGUN;
	case WEAPON_NOVA:
		return WEAPONTYPE_SHOTGUN;
	case WEAPON_P250:
		return WEAPONTYPE_PISTOL;
	case WEAPON_SCAR20:
	case WEAPON_SG556:
	case WEAPON_SSG08:
		return WEAPONTYPE_SNIPER_RIFLE;
	case WEAPON_KNIFE:
		return WEAPONTYPE_KNIFE;
	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
	case WEAPON_SMOKEGRENADE:
	case WEAPON_MOLOTOV:
	case WEAPON_DECOY:
	case WEAPON_INCGRENADE:
		return WEAPONTYPE_GRENADE;
	case WEAPON_C4:
		return WEAPONTYPE_C4;
	case WEAPON_KNIFE_T:
		return WEAPONTYPE_KNIFE;
	case WEAPON_M4A1_SILENCER:
		return WEAPONTYPE_RIFLE;
	case WEAPON_USP_SILENCER:
	case WEAPON_CZ75A:
	case WEAPON_REVOLVER:
		return WEAPONTYPE_PISTOL;
	case WEAPON_KNIFE_BAYONET:
	case WEAPON_KNIFE_FLIP:
	case WEAPON_KNIFE_GUT:
	case WEAPON_KNIFE_KARAMBIT:
	case WEAPON_KNIFE_M9_BAYONET:
	case WEAPON_KNIFE_TACTICAL:
	case WEAPON_KNIFE_FALCHION:
	case WEAPON_KNIFE_SURVIVAL_BOWIE:
	case WEAPON_KNIFE_BUTTERFLY:
	case WEAPON_KNIFE_PUSH:
		return WEAPONTYPE_KNIFE;
	case GLOVE_STUDDED_BLOODHOUND:
	case GLOVE_T_SIDE:
	case GLOVE_CT_SIDE:
	case GLOVE_SPORTY:
	case GLOVE_SLICK:
	case GLOVE_LEATHER_WRAP:
	case GLOVE_MOTORCYCLE:
	case GLOVE_SPECIALIST:
		return WEAPONTYPE_UNKNOWN;
	default:
		return WEAPONTYPE_UNKNOWN;
	}
}
std::string C_BaseWeapon::GetWeaponName ( )
{
	if ( !this )
		return "";

	int id = this->GetItemDefinitionIndex ( );

	switch ( id )
	{
	case WEAPON_DEAGLE:
		return ( "Desert Eagle" );
	case WEAPON_ELITE:
		return ( "Dual Berettas" );
	case WEAPON_FIVESEVEN:
		return ( "Five-SeveN" );
	case WEAPON_GLOCK:
		return ( "Glock-18" );
	case WEAPON_AK47:
		return ( "AK-47" );
	case WEAPON_AUG:
		return ( "AUG" );
	case WEAPON_AWP:
		return ( "AWP" );
	case WEAPON_FAMAS:
		return ( "FAMAS" );
	case WEAPON_G3SG1:
		return ( "G3SG1" );
	case WEAPON_GALILAR:
		return ( "Galil" );
	case WEAPON_M249:
		return ( "M249" );
	case WEAPON_M4A1:
		return ( "M4A1" );
	case WEAPON_MAC10:
		return ( "MAC-10" );
	case WEAPON_P90:
		return ( "P90" );
	case WEAPON_UMP45:
		return ( "UMP-45" );
	case WEAPON_XM1014:
		return ( "XM1014" );
	case WEAPON_BIZON:
		return ( "PP-Bizon" );
	case WEAPON_MAG7:
		return ( "MAG-7" );
	case WEAPON_NEGEV:
		return ( "Negev" );
	case WEAPON_SAWEDOFF:
		return ( "Sawed-Off" );
	case WEAPON_TEC9:
		return ( "Tec-9" );
	case WEAPON_TASER:
		return ( "Taser" );
	case WEAPON_HKP2000:
		return ( "P2000" );
	case WEAPON_MP7:
		return ( "MP7" );
	case WEAPON_MP9:
		return ( "MP9" );
	case WEAPON_NOVA:
		return ( "Nova" );
	case WEAPON_P250:
		return ( "P250" );
	case WEAPON_SCAR20:
		return ( "SCAR-20" );
	case WEAPON_SG556:
		return ( "SG 553" );
	case WEAPON_SSG08:
		return ( "SSG 08" );
	case WEAPON_KNIFE:
		return ( "Knife" );
	case WEAPON_FLASHBANG:
		return ( "Flashbang" );
	case WEAPON_HEGRENADE:
		return ( "HE Grenade" );
	case WEAPON_SMOKEGRENADE:
		return ( "Smoke Grenade" );
	case WEAPON_MOLOTOV:
		return ( "Molotov" );
	case WEAPON_DECOY:
		return ( "Decoy" );
	case WEAPON_INCGRENADE:
		return ( "Incendiary Grenade" );
	case WEAPON_C4:
		return ( "C4" );
	case WEAPON_KNIFE_T:
		return ( "Knife" );
	case WEAPON_M4A1_SILENCER:
		return ( "M4A1-S" );
	case WEAPON_USP_SILENCER:
		return ( "USP-S" );
	case WEAPON_CZ75A:
		return ( "CZ75-Auto" );
	case WEAPON_REVOLVER:
		return ( "R8 Revolver" );
	default:
		return ( "Knife" );
	}

	return "";
}