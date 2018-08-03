#include "Prediction.h"

void PredictionSystem::Start ( CUserCmd *userCMD , C_BaseEntity* player )
{
	static bool bInit = false;
	if ( !bInit )
	{
		auto client = GetModuleHandle ( "client.dll" );
		predictionRandomSeed = *( int** ) ( g_pUtils->PatternScan ( client , "8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04" ) + 2 );
		predictionPlayer = *reinterpret_cast< C_BaseEntity** >( g_pUtils->PatternScan ( client , "89 3D ? ? ? ? F3 0F 10 47" ) + 2 );
		bInit = true;
	}

	*predictionRandomSeed = MD5_PseudoRandom ( userCMD->command_number ) & 0x7FFFFFFF;
	predictionPlayer = player;

	m_flOldCurTime = g_pGlobalVars->curtime;
	m_flOldFrametime = g_pGlobalVars->frametime;

	g_pGlobalVars->curtime = player->GetTickBase ( ) * g_pGlobalVars->interval_per_tick;
	g_pGlobalVars->frametime = g_pGlobalVars->interval_per_tick;

	//Here we're doing CBasePlayer::UpdateButtonState // NOTE: hard to tell when offsets changed, think of more longterm solution or just dont do this.
	moveData.m_nButtons = userCMD->buttons;
	int buttonsChanged = userCMD->buttons ^ *reinterpret_cast< int* >( uintptr_t ( player ) + 0x31E8 );
	*reinterpret_cast< int* >( uintptr_t ( player ) + 0x31DC ) = ( uintptr_t ( player ) + 0x31E8 );
	*reinterpret_cast< int* >( uintptr_t ( player ) + 0x31E8 ) = userCMD->buttons;
	*reinterpret_cast< int* >( uintptr_t ( player ) + 0x31E0 ) = userCMD->buttons & buttonsChanged;  //m_afButtonPressed ~ The changed ones still down are "pressed"
	*reinterpret_cast< int* >( uintptr_t ( player ) + 0x31E4 ) = buttonsChanged & ~userCMD->buttons; //m_afButtonReleased ~ The ones not down are "released"

	g_pGameMovement->StartTrackPredictionErrors ( player );

	memset ( &moveData , 0 , sizeof ( CMoveData ) );
	g_pMoveHelper->SetHost ( player );
	g_pIPrediction->SetupMove ( player , userCMD , g_pMoveHelper , &moveData );
	g_pGameMovement->ProcessMovement ( player , &moveData );
	g_pIPrediction->FinishMove ( player , userCMD , &moveData );
}

void PredictionSystem::End ( C_BaseEntity* player )
{
	g_pGameMovement->FinishTrackPredictionErrors ( player );
	g_pMoveHelper->SetHost ( nullptr );

	*predictionRandomSeed = -1;
	predictionPlayer = nullptr;

	g_pGlobalVars->curtime = m_flOldCurTime;
	g_pGlobalVars->frametime = m_flOldFrametime;
}

void PredictionSystem::FixTicks ( CUserCmd* ucmd ) {

	static CUserCmd* g_pLastCmd = nullptr;
	auto local = g_pEntityList->GetClientEntity ( g_pEngine->GetLocalPlayer ( ) );
	if ( !ucmd )
		return;

	if ( !g_pLastCmd || g_pLastCmd->hasbeenpredicted ) {
		CorrectTick = local->GetTickBase ( );
	}
	else {
		// Required because prediction only runs on frames, not ticks
		// So if your framerate goes below tickrate, m_nTickBase won't update every tick
		++CorrectTick;
	}

	g_pLastCmd = ucmd;
}