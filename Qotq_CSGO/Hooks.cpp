#include "Hooks.h"

unsigned char ( __cdecl *ReadByte ) ( uintptr_t );
int ( __cdecl *ReadInt ) ( uintptr_t );
float ( __cdecl *ReadFloat ) ( uintptr_t );
double ( __cdecl *ReadDouble ) ( uintptr_t );
short ( __cdecl *ReadShort ) ( uintptr_t );
void ( __cdecl *WriteByte ) ( uintptr_t , unsigned char );
void ( __cdecl *WriteInt ) ( uintptr_t , int );
void ( __cdecl *WriteFloat ) ( uintptr_t , float );
void ( __cdecl *WriteDouble ) ( uintptr_t , double );
void ( __cdecl *WriteShort ) ( uintptr_t , short );

CUserCmd* pcmd;

//reso hooks
CreateMove oCreateMove;
DoPostScreenEffects oDoPostScreenEffects;
PaintTraverse oPaintTraverse;
OverrideView oOverrideView;
SceneEnd oSceneEnd;
FrameStageNotify oFrameStageNotify;
WriteUsercmdDeltaToBuffer oWriteUsercmdDeltaToBuffer;
PlaySoundT oPlaySound;
DrawModelExecute oDrawModelExecute;
UpdateClientSideAnimationFn oUpdateClientSideAnimation;
SendDatagramFn oSendDatagram;

CVMTHookManager* g_pNetChannelHook = nullptr;

int32_t originalCorrectedFakewalkIdx = 0;
int32_t tickHitPlayer = 0;
int32_t tickHitWall = 0;
int32_t originalShotsMissed = 0;

void __fastcall Hooks::PaintTraverseHook ( void *thisptr , void * _EDX , vgui::VPANEL panel , bool forceRepaint , bool allowForce ) {
	static uint32_t FocusOverlayPanel;
	if ( !FocusOverlayPanel )
	{
		const char* szName = g_pVGuiPanel->GetName ( panel );

		if ( lstrcmpA ( szName , "MatSystemTopPanel" ) == 0 )
		{
			FocusOverlayPanel = panel;
			g_pSDK->InitFonts ( );
			g_pEngine->ExecuteClientCmd ( "clear" );
		}
	}

	if ( FocusOverlayPanel == panel )
	{
		auto local = g_pEntityList->GetClientEntity ( g_pEngine->GetLocalPlayer ( ) );
		time_t _tm = time ( NULL );
		struct tm * curtime = localtime ( &_tm );
		std::string timee = asctime ( curtime );
		if ( local ) {
			g_pESP->Start ( );
		}
		//do visuals here
		if ( g_pInputSystem->IsButtonDown ( KEY_INSERT ) )
		{
			static float lastTime = 0;
			if ( GetTickCount ( ) + 400 < lastTime ) {
				lastTime = GetTickCount ( );
			}
			if ( GetTickCount ( ) > lastTime ) {
				menuOpen = !menuOpen;
				lastTime = GetTickCount ( ) + 300;
			}
		}
		std::string None = ( "None" );
		if ( local && local->GetHealth ( ) > 0 ) {
			Draw->String ( F_Arial , false , 10 , 500 , Color ( 255 , 255 , 255 ) , ( "Sim: " + std::to_string ( local->SimTime ( ) ) ).c_str ( ) );
			Draw->String ( F_Arial , false , 10 , 500 + ( 14 * 1 ) , Color ( 255 , 255 , 255 ) , ( "Cur: " + std::to_string ( g_pGlobalVars->curtime ) ).c_str ( ) );
			Draw->String ( F_Arial , false , 10 , 500 + ( 14 * 2 ) , Color ( 255 , 255 , 255 ) , ( "LBY: " + std::to_string ( local->LowerBodyYaw ( ) ) ).c_str ( ) );
			Draw->String ( F_Arial , false , 10 , 500 + ( 14 * 3 ) , Color ( 255 , 255 , 255 ) , ( "PKT: " + std::string ( bSendPacket ? "True" : "False" ) ).c_str ( ) );
		}
		else {
			Draw->String ( F_Arial , false , 10 , 500 , Color ( 255 , 255 , 255 ) , std::string ( "Sim: " + None ).c_str ( ) );
			Draw->String ( F_Arial , false , 10 , 500 + ( 14 * 1 ) , Color ( 255 , 255 , 255 ) , std::string ( "Cur: " + None ).c_str ( ) );
			Draw->String ( F_Arial , false , 10 , 500 + ( 14 * 2 ) , Color ( 255 , 255 , 255 ) , std::string ( "LBY: " + None ).c_str ( ) );
			Draw->String ( F_Arial , false , 10 , 500 + ( 14 * 3 ) , Color ( 255 , 255 , 255 ) , std::string ( "PKT: " + None ).c_str ( ) );
		}


		if ( menuOpen ) {
			qualitymenu.HandleInput ( );
			qualitymenu.DrawShit ( );
		}
		else {
			Draw->String ( F_Arial , false , 400 , 5 , Color ( 255 , 255 , 255 , 255 ) , "chambers' meme (closed)" );
		}
	}

	return oPaintTraverse ( thisptr , panel , forceRepaint , allowForce );
}

template<class T , class U>
T clamp ( T in , U low , U high )
{
	if ( in <= low )
		return low;

	if ( in >= high )
		return high;

	return in;
}

bool __stdcall Hooks::CreateMoveHook ( float flInputSampleTime , CUserCmd* cmd ) {
	C_BaseEntity* localplayer = g_pEntityList->GetClientEntity ( g_pEngine->GetLocalPlayer ( ) );

	bSendPacket = true;

	if ( !cmd->command_number || !g_pEngine->IsConnected ( ) && !g_pEngine->IsInGame ( ) || !localplayer )
		return oCreateMove ( flInputSampleTime , cmd );

	pcmd = cmd;

	QAngle org_angle = cmd->viewangles;

	PredictionSystem::Get ( ).FixTicks ( cmd );

	//CBacktrack::Get().UpdateIncomingSequences();

	if ( g_Settings.misc.bhop )
		CMovement::Get ( ).Bhop ( localplayer , cmd );

	if ( g_Settings.misc.autostrafe )
		CMovement::Get ( ).AutoStrafe ( cmd );

	QAngle preAAAngle = cmd->viewangles;
	cmd->viewangles = org_angle;

	PredictionSystem::Get ( ).Start ( cmd , localplayer );
	{
		AA::Get ( ).UpdateLBYBreaker ( cmd );

		if ( g_pInputSystem->IsButtonDown ( KEY_LSHIFT ) && g_Settings.aimbot.enabled ) {
			CMovement::Get ( ).Fakewalk ( cmd );
			CMovement::Get ( ).fwing = true;
		}
		else {
			CMovement::Get ( ).fwchoked = 0;
			CMovement::Get ( ).fwing = false;
			AA::Get ( ).PerformLag ( );
		}

		AA::Get ( ).Run ( cmd );

		CAimbot::Get ( ).run ( cmd );

		AA::Get ( ).FixMovement ( cmd , preAAAngle );
	}
	PredictionSystem::Get ( ).End ( localplayer );

	if ( g_Settings.aa.enabled ) {
		//set bsendpacket
		uintptr_t *framePtr;
		__asm mov framePtr , ebp;

		if ( aimbotting && cmd->buttons & IN_ATTACK )
			*( bool* ) ( *framePtr - 0x1C ) = true;
		else
			*( bool* ) ( *framePtr - 0x1C ) = bSendPacket;


	}
	if ( !bSendPacket || !g_Settings.aa.enabled ) {
		CBasePlayerAnimState* animstate = localplayer->GetBasePlayerAnimState ( );

		if ( animstate ) {
			if ( animstate->in_hit_ground_animation && !g_pInputSystem->IsButtonDown ( KEY_SPACE ) ) {
				real_angles = cmd->viewangles;
				real_angles.pitch = 0.f;
			}
			else
				real_angles = cmd->viewangles;
		}
		else {
			real_angles = cmd->viewangles;
		}
	}

	if ( g_Settings.misc.anti_ut ) {
		cmd->forwardmove = clamp ( cmd->forwardmove , -450.f , 450.f );
		cmd->sidemove = clamp ( cmd->sidemove , -450.f , 450.f );
		cmd->upmove = clamp ( cmd->upmove , -320.f , 320.f );
		cmd->viewangles.Clamp ( );
	}

	return false;
}

int  __fastcall Hooks::DoPostScreenEffectsHook ( void *thisptr , void * _EDX , int a1 ) {
	if ( g_pEngine->GetLocalPlayer ( ) && g_Settings.visuals.glow ) g_pGlow->Start ( );
	return oDoPostScreenEffects ( thisptr , a1 );
}

void __stdcall Hooks::OverrideViewHook ( CViewSetup* pSetup ) {
	oOverrideView ( pSetup );

	if ( ( g_pInputSystem->IsButtonDown ( ( ButtonCode_t ) g_Settings.misc.tpcode ) ) && g_Settings.misc.thirdperson )
	{
		static float lastTime = 0;
		if ( GetTickCount ( ) + 400 < lastTime ) {
			in_thirdperson = !in_thirdperson;
			lastTime = GetTickCount ( ) + 300;
		}
		else if ( GetTickCount ( ) > lastTime ) {
			in_thirdperson = !in_thirdperson;
			lastTime = GetTickCount ( ) + 300;
		}
	}

	auto local = g_pEntityList->GetClientEntity ( g_pEngine->GetLocalPlayer ( ) );

	//thirdperson stuff
	static bool spoofed = false;
	if ( g_pEngine->IsConnected ( ) && g_pEngine->IsInGame ( ) )
	{
		if ( !spoofed ) {
			ConVar* sv_cheats = g_pICvar->FindVar ( "sv_cheats" );
			SpoofedConvar* sv_cheats_spoofed = new SpoofedConvar ( sv_cheats );
			sv_cheats_spoofed->SetInt ( 1 );
			spoofed = true;
		}

		CThirdperson::Get ( ).Run ( pSetup );

		if ( g_Settings.aimbot.enabled )
			pSetup->fov = 110;
	}
	else
		spoofed = false;
}

void __stdcall Hooks::FrameStageNotifyHook ( ClientFrameStage_t stage ) {
	auto local = g_pEntityList->GetClientEntity ( g_pEngine->GetLocalPlayer ( ) );

	if ( !local || !g_pEngine->IsInGame ( ) || !g_pEngine->IsConnected ( ) )
		return oFrameStageNotify ( stage );

	QAngle aim_punch_old;
	QAngle view_punch_old;

	QAngle *aim_punch = nullptr;
	QAngle *view_punch = nullptr;

	if ( !g_pEngine->IsInGame ( ) || !g_pEngine->IsConnected ( ) ) {
		//CBacktrack::Get().ClearIncomingSequences();
		return;
	}

	static int userId [ 64 ];
	static AnimationLayer
		backupLayersUpdate [ 64 ] [ 15 ] ,
		backupLayersInterp [ 64 ] [ 15 ];

	if ( stage == FRAME_RENDER_START )
	{
		for ( int i = 1; i <= g_pGlobalVars->maxClients; i++ ) //	PVS Fix
		{
			if ( i == g_pEngine->GetLocalPlayer ( ) ) continue;

			C_BaseEntity* player = g_pEntityList->GetClientEntity ( i );
			if ( !player ) continue;

			*( int* ) ( ( uintptr_t ) player + 0xA30 ) = g_pGlobalVars->framecount; //we'll skip occlusion checks now
			*( int* ) ( ( uintptr_t ) player + 0xA28 ) = 0; //clear occlusion flags
		}

		if ( g_Settings.aimbot.animfix )
			CBacktrack::Get ( ).LocalLegmeme ( local );

		if ( local->GetHealth ( ) > 0 )
			if ( *( bool* ) ( ( uintptr_t ) g_pCInput + 0xA5 ) ) //set thirdperson to look at real angs
				local->visuals_Angles ( ) = real_angles;

		//start novisualrecoil
		if ( g_Settings.aimbot.enabled )
			if ( local && local->GetHealth ( ) > 0 && g_pEngine->IsInGame ( ) && g_pEngine->IsConnected ( ) ) {
				aim_punch = &local->GetPunchAngles ( );
				view_punch = &local->GetVPunchAngles ( );

				aim_punch_old = *aim_punch;
				view_punch_old = *view_punch;

				*aim_punch = QAngle ( 0.f , 0.f , 0.f );
				*view_punch = QAngle ( 0.f , 0.f , 0.f );
			}
	}

	if ( stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START )
	{
		if ( g_Settings.aimbot.enabled ) {
			Resolver::Get ( ).Run ( );
			CBacktrack::Get ( ).FSN ( );
		}
	}

	return oFrameStageNotify ( stage );
}

void __fastcall Hooks::SceneEndHook ( void* thisptr ) {
	oSceneEnd ( thisptr );
}

bool __fastcall Hooks::WriteUsercmdDeltaToBufferHook ( IBaseClientDLL* this0 , void * _EDX , int nSlot , void* buf , int from , int to , bool isNewCmd ) {
	return true;
}

void __stdcall Hooks::PlaySoundHook ( const char *folderIme ) {
	return oPlaySound ( g_pVGuiSurface , folderIme );
}

void __stdcall Hooks::DrawModelExecuteHook ( IMatRenderContext* ctx , const DrawModelState_t& state , const ModelRenderInfo_t& pInfo , matrix3x4_t* pCustomBoneToWorld ) {
	//	this hook is very necessary.
	oDrawModelExecute ( g_pMdlRender , ctx , state , pInfo , pCustomBoneToWorld );
}

//	client, 55 8B EC 51 56 8B F1 80 BE ? ? ? ? 00 74 ? 8B 06 FF
void __fastcall Hooks::UpdateClientSideAnimation ( C_BaseEntity* entity , void* edx ) {
	oUpdateClientSideAnimation ( entity , edx );

	if ( !entity || !entity->IsPlayer ( ) || entity->GetHealth ( ) < 1 || entity->GetDormant ( ) || entity->GetIndex ( ) != g_pEngine->GetLocalPlayer ( ) )
		return;

	auto animstate = entity->GetBasePlayerAnimState ( );

	if ( !animstate )
		return;

	entity->SetAbsAngles ( QAngle ( 0.0f , animstate->goal_feet_yaw , 0.0f ) );
}

int __fastcall Hooks::SendDatagram ( void* netchan , void* , void *datagram )
{
	INetChannel* chan = ( INetChannel* ) netchan;
	bf_write* data = ( bf_write* ) datagram;

	g_pNetChannel = chan;

	int instate = chan->m_nInReliableState;
	int insequencenr = chan->m_nInSequenceNr;

	/*if (pcmd) {
		if (pcmd->buttons & IN_ATTACK || g_pInputSystem->IsButtonDown((ButtonCode_t)g_Settings.misc.btcode)) {
			CMovement::Get().Pitchonshoot(chan);
		}
		else {
			if (g_pInputSystem->IsButtonDown(KEY_N)) {
				CMovement::Get().NasaWalk(chan);
		}
	}*/

	int ret = oSendDatagram ( chan , data );

	chan->m_nInReliableState = instate;
	chan->m_nInSequenceNr = insequencenr;

	return ret;
}

