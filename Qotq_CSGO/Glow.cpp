#include "Glow.h"

void CGlow::Start ( ) {
	auto GlowObjectManager = g_pGlowObjectManager;
	CGlowObjectManager::GlowObjectDefinition_t* glowEntity;
	auto local = g_pEntityList->GetClientEntity ( g_pEngine->GetLocalPlayer ( ) );

	for ( int i = 0; i < GlowObjectManager->size; i++ )
	{
		glowEntity = &GlowObjectManager->m_GlowObjectDefinitions [ i ];
		auto Entity = glowEntity->getEntity ( );

		//checks
		if ( !Entity ) continue;
		if ( Entity->GetTeam ( ) == local->GetTeam ( ) && Entity->GetIndex ( ) != g_pEngine->GetLocalPlayer ( ) ) continue;

		auto EntityClass = Entity->GetClientClass ( );
		if ( EntityClass->m_ClassID == ClassId_CCSPlayer ) {
			int hp_red = 255 - ( Entity->GetHealth ( ) * 2.55 );
			int hp_green = Entity->GetHealth ( ) * 2.55;
			glowEntity->set ( Color ( hp_red , hp_green , 0 , 255 ) );
		}
	}
}