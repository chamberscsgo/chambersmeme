#pragma once
#include "SDK.h"
class CESP {
public:
	void Start ( );
	bool WorldToScreen ( const Vector& in , Vector& out );
private:
	void PlayerBox ( C_BaseEntity* pEntity , RECT EspRect );
	void PlayerResolver ( C_BaseEntity * pEntity , RECT EspRect );
	void PlayerHealth ( C_BaseEntity* pEntity , RECT EspRect );
	void PlayerWeapon ( C_BaseEntity* pEntity , RECT EspRect );
	void PlayerMoney ( C_BaseEntity* pEntity , RECT EspRect );
	void PlayerName ( C_BaseEntity* pEntity , RECT EspRect , player_info_t info );
	void PlayerSkeleton ( C_BaseEntity * pEntity );
	void PlayerCross ( C_BaseEntity * pEntity );
	void PlayerDistance ( C_BaseEntity* pEntity , RECT EspRect , C_BaseEntity* pLocal );
	RECT GetBox ( C_BaseEntity* pEntity );
};
extern CESP* g_pESP;