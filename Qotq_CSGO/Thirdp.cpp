#include "SDK.h"

void CThirdperson::Run ( CViewSetup* setup )
{
	auto local = reinterpret_cast< C_BaseEntity* >( g_pEntityList->GetClientEntity ( g_pEngine->GetLocalPlayer ( ) ) );
	if ( !local )
		return;

	static size_t lastTime = 0;

	g_pCInput->m_fCameraInThirdPerson = in_thirdperson && local && local->GetHealth ( ) > 0;
}