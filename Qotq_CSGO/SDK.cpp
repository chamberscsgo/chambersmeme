#include "SDK.h"

HWND m_hwndWindow = NULL;
WNDPROC oldWindowProc = nullptr;
//reso externals
CUtils* g_pUtils;
CGlow* g_pGlow;
CGlowObjectManager* g_pGlowObjectManager;
CSDK* g_pSDK;
DrawManager* Draw;
vgui::HFont F_Arial;
vgui::HFont F_ESP;
Offsets NetVars;
COffsets g_Offsets;
bool pressedKey [ 256 ] = {};
INetChannel* g_pNetChannel;
bool menuOpen = false;
bool bSendPacket;
bool aimbotting;
int aaside = -1;
bool in_thirdperson;
QAngle real_angles;
SSettings g_Settings;
ShitMenu qualitymenu;

IBaseClientDLL* g_pCHLClient = nullptr;
IClientMode* g_pClientMode = nullptr;
IPanel* g_pVGuiPanel = nullptr;
ISurface* g_pVGuiSurface = nullptr;
IVEngineClient* g_pEngine = nullptr;
ICvar* g_pICvar = nullptr;
IClientEntityList* g_pEntityList = nullptr;
CGlobalVarsBase* g_pGlobalVars = nullptr;
IEngineTrace* g_pEngineTrace = nullptr;
IVDebugOverlay* g_pDebugOverlay = nullptr;
CInput* g_pCInput = nullptr;
IMemAlloc* g_pMemAlloc = nullptr;
IVModelInfoClient* g_pMdlInfo = nullptr;
IVModelRender* g_pMdlRender = nullptr;
C_LocalPlayer * g_LocalPlayer;
IVRenderView* g_pRenderView = nullptr;
IPhysicsSurfaceProps *g_PhysSurface = nullptr;
IMaterialSystem* g_pMatSystem = nullptr;
IGameEventManager2* g_pGameEvents = nullptr;
IMoveHelper* g_pMoveHelper = nullptr;
IMDLCache* g_pMDLCache = nullptr;
IPrediction* g_pIPrediction = nullptr;
CGameMovement* g_pGameMovement = nullptr;
CClientState* g_pClientState = nullptr;
void* g_pUpdateClientSideAnimation = nullptr;;
IInputSystem* g_pInputSystem = nullptr;

template< typename T >
T* GetInterface ( char* mod , char* interf , bool exact = false )
{
	typedef T* ( *CreateInterfaceFn )( const char* szName , int iReturn );
	CreateInterfaceFn CreateInterface = ( CreateInterfaceFn ) GetProcAddress ( GetModuleHandleA ( mod ) , ( "CreateInterface" ) );

	std::string sInterface , sInterfaceVersion = "0";

	if ( !exact )
	{
		for ( auto i = 0; i <= 99; i++ )
		{
			sInterface = interf;
			sInterface += sInterfaceVersion;
			sInterface += std::to_string ( i );

			T* funcPtr = CreateInterface ( sInterface.c_str ( ) , NULL );

			if ( funcPtr != NULL )
			{
				g_pUtils->Print ( "%s -> 0x%x" , sInterface.c_str ( ) , funcPtr );
				return funcPtr;
			}

			if ( i == 99 && sInterfaceVersion == "0" )
			{
				sInterfaceVersion = "00";
				i = 0;
			}
			else if ( i == 99 && sInterfaceVersion == "00" )
			{
				g_pUtils->Print ( "%s -> Cant get pointer!" , interf );
				return nullptr;
			}
		}
	}
	else
	{
		T* ptr = CreateInterface ( interf , 0 );

		if ( ptr )
		{
			g_pUtils->Print ( "%s -> 0x%x" , interf , ptr );
			return ptr;
		}
		else
		{
			g_pUtils->Print ( "%s ->  Cant get pointer!" , interf );
			return nullptr;
		}
	}

	return nullptr;
}

void InstallQotq ( void )
{
	while ( !( m_hwndWindow = FindWindowA ( "Valve001" , NULL ) ) )
		std::this_thread::sleep_for ( std::chrono::milliseconds ( 500 ) );

	while ( !GetModuleHandleA ( "serverbrowser.dll" ) )
		std::this_thread::sleep_for ( std::chrono::milliseconds ( 500 ) );

	g_pUtils->Print ( "-------------------------------------------------------------------------------" );
	g_pUtils->Print ( "-------------------------------QotqCheetos-------------------------------------" );
	g_pUtils->Print ( "-------------------------------------------------------------------------------" );
	g_pUtils->Print ( "Got serverbrowser.dll (game is read)" );
	g_pUtils->Print ( " " ); //don't ask dude
	g_pUtils->Print ( "-------------------------------------------------------------------------------" );
	//-----------------------------------------------------------------------------
	// Gettig interfaces.
	//-----------------------------------------------------------------------------
	g_pCHLClient = GetInterface<IBaseClientDLL> ( "client.dll" , "VClient" );
	g_pClientMode = **reinterpret_cast< IClientMode*** >( ( *( uintptr_t** ) g_pCHLClient ) [ 10 ] + 0x5 );
	g_pVGuiPanel = GetInterface<IPanel> ( "vgui2.dll" , "VGUI_Panel" );
	g_pVGuiSurface = GetInterface<ISurface> ( "vguimatsurface.dll" , "VGUI_Surface" );
	g_pEngine = GetInterface<IVEngineClient> ( "engine.dll" , "VEngineClient" );
	g_pICvar = GetInterface<ICvar> ( "vstdlib.dll" , "VEngineCvar" );
	g_pEntityList = GetInterface<IClientEntityList> ( "client.dll" , "VClientEntityList" );
	g_pGlobalVars = **( CGlobalVarsBase*** ) ( ( *( DWORD** ) ( g_pCHLClient ) ) [ 0 ] + 0x1B );
	g_pEngineTrace = GetInterface<IEngineTrace> ( "engine.dll" , "EngineTraceClient" );
	g_pDebugOverlay = GetInterface<IVDebugOverlay> ( "engine.dll" , "VDebugOverlay" );
	//g_pCInput = *(CInput**)((*(DWORD**)g_pCHLClient)[15] + 0x1);
	g_pMdlRender = GetInterface<IVModelRender> ( "engine.dll" , "VEngineModel" );
	g_PhysSurface = GetInterface<IPhysicsSurfaceProps> ( "vphysics.dll" , "VPhysicsSurfaceProps" );
	g_pMdlInfo = GetInterface<IVModelInfoClient> ( "engine.dll" , "VModelInfoClient" );
	g_pRenderView = GetInterface<IVRenderView> ( "engine.dll" , "VEngineRenderView" );
	g_pMatSystem = GetInterface<IMaterialSystem> ( "materialsystem.dll" , "VMaterialSystem" );
	g_pGameEvents = GetInterface<IGameEventManager2> ( "engine.dll" , "GAMEEVENTSMANAGER002" , true );
	g_pMDLCache = GetInterface<IMDLCache> ( "datacache.dll" , "MDLCache" );
	g_pIPrediction = GetInterface<IPrediction> ( "client.dll" , "VClientPrediction" );
	g_pGameMovement = GetInterface<CGameMovement> ( "client.dll" , "GameMovement" );
	g_pClientState = **( CClientState*** ) ( ( *( uintptr_t** ) g_pEngine ) [ 12 ] + 0x10 );
	g_pInputSystem = GetInterface<IInputSystem> ( "inputsystem.dll" , "InputSystemVersion" );
	g_pMemAlloc = *( IMemAlloc** ) ( GetProcAddress ( GetModuleHandle ( "tier0.dll" ) , "g_pMemAlloc" ) );

	g_pGlowObjectManager = *( CGlowObjectManager** ) ( g_pUtils->FindPatternIDA ( "client.dll" , "0F 11 05 ? ? ? ? 83 C8 01" ) + 3 );
	g_pMoveHelper = **( IMoveHelper*** ) ( g_pUtils->FindPatternIDA ( "client.dll" , "8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01" ) + 2 );
	g_pCInput = *( CInput** ) ( g_pUtils->FindPatternIDA ( "client.dll" , "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10" ) + 1 );
	auto D3DDevice9 = **( IDirect3DDevice9*** ) ( g_pUtils->FindPatternIDA ( "shaderapidx9.dll" , "A1 ? ? ? ? 50 8B 08 FF 51 0C" ) + 1 );
	g_LocalPlayer = **( C_LocalPlayer*** ) ( g_pUtils->FindPatternIDA ( "client.dll" , "8B 0D ? ? ? ? 83 FF FF 74 07" ) + 2 );
	g_pUtils->Print ( "g_LocalPlayer -> 0x%x" , g_LocalPlayer );
	g_pUtils->Print ( "-------------------------------------------------------------------------------" );

	g_pUtils->Print ( "-------------------------------------------------------------------------------" );
	g_pUtils->Print ( "Dumping Netvars:" );


	//-----------------------------------------------------------------------------
	// Netvars.
	//-----------------------------------------------------------------------------
#pragma region netvars
	//pasted from my old paste which i pasted from somewhere, dont remember.
	g_Offsets.NetVars = std::shared_ptr<CNetVars> ( new CNetVars ( ) );
	g_Offsets.NetVars->GetOffset ( ( "DT_BaseEntity" ) , ( "m_CollisionGroup" ) , &NetVars.m_rgflCoordinateFrame , -0x30 );
	g_Offsets.NetVars->GetOffset ( ( "DT_BaseEntity" ) , ( "m_Collision" ) , &NetVars.m_Collision );
	g_Offsets.NetVars->GetOffset ( ( "DT_BaseEntity" ) , ( "m_vecOrigin" ) , &NetVars.m_vecOrigin );
	g_Offsets.NetVars->GetOffset ( ( "DT_BaseEntity" ) , ( "m_iTeamNum" ) , &NetVars.m_iTeamNum );
	g_Offsets.NetVars->GetOffset ( ( "DT_BaseEntity" ) , ( "m_bSpotted" ) , &NetVars.m_bSpotted );
	g_Offsets.NetVars->GetOffset ( ( "DT_BaseEntity" ) , ( "m_vecMaxs" ) , &NetVars.m_vecMaxs );
	g_Offsets.NetVars->GetOffset ( ( "DT_BaseEntity" ) , ( "m_vecMins" ) , &NetVars.m_vecMins );
	g_Offsets.NetVars->GetOffset ( ( "DT_BaseEntity" ) , ( "m_flSimulationTime" ) , &NetVars.m_flSimulationTime );
	g_Offsets.NetVars->GetOffset ( ( "DT_BasePlayer" ) , ( "m_aimPunchAngle" ) , &NetVars.m_vecPunchAngles );
	g_Offsets.NetVars->GetOffset ( ( "DT_BasePlayer" ) , ( "m_viewPunchAngle" ) , &NetVars.m_vecViewPunchAngles );
	g_Offsets.NetVars->GetOffset ( ( "DT_BasePlayer" ) , ( "m_vecVelocity[0]" ) , &NetVars.m_vecVelocity );
	g_Offsets.NetVars->GetOffset ( ( "DT_BasePlayer" ) , ( "m_iObserverMode" ) , &NetVars.m_iObserverMode );
	g_Offsets.NetVars->GetOffset ( ( "DT_BasePlayer" ) , ( "m_hObserverTarget" ) , &NetVars.m_hObserverTarget );
	g_Offsets.NetVars->GetOffset ( ( "DT_BasePlayer" ) , ( "m_lifeState" ) , &NetVars.m_lifeState );
	g_Offsets.NetVars->GetOffset ( ( "DT_BasePlayer" ) , ( "m_fFlags" ) , &NetVars.m_fFlags );
	g_Offsets.NetVars->GetOffset ( ( "DT_BasePlayer" ) , ( "m_iHealth" ) , &NetVars.m_iHealth );
	g_Offsets.NetVars->GetOffset ( ( "DT_BasePlayer" ) , ( "m_hActiveWeapon" ) , &NetVars.m_hActiveWeapon );
	g_Offsets.NetVars->GetOffset ( ( "DT_BasePlayer" ) , ( "m_hMyWeapons" ) , &NetVars.m_hMyWeapons );
	g_Offsets.NetVars->GetOffset ( ( "DT_BasePlayer" ) , ( "m_hViewModel[0]" ) , &NetVars.m_hViewModel );
	g_Offsets.NetVars->GetOffset ( ( "DT_BasePlayer" ) , ( "m_nTickBase" ) , &NetVars.m_nTickBase );
	g_Offsets.NetVars->GetOffset ( ( "DT_BasePlayer" ) , ( "m_nHitboxSet" ) , &NetVars.m_nHitboxSet );
	g_Offsets.NetVars->GetOffset ( ( "DT_BasePlayer" ) , ( "m_hObserverTarget" ) , &NetVars.m_hObserverTarget );
	g_Offsets.NetVars->GetOffset ( ( "DT_BasePlayer" ) , ( "m_flSimulationTime" ) , &NetVars.m_flSimulationTime );
	g_Offsets.NetVars->GetOffset ( ( "DT_BasePlayer" ) , ( "m_flPoseParameter" ) , &NetVars.m_flPoseParameter );
	g_Offsets.NetVars->GetOffset ( ( "DT_BasePlayer" ) , ( "m_flNextAttack" ) , &NetVars.m_flNextAttack );

	g_Offsets.NetVars->GetOffset ( ( "DT_BaseAnimating" ) , ( "m_bClientSideAnimation" ) , &NetVars.m_bClientSideAnimation );
	g_Offsets.NetVars->GetOffset ( ( "DT_BaseAnimating" ) , ( "m_nForceBone" ) , &NetVars.m_nForceBone );

	g_Offsets.NetVars->GetOffset ( ( "DT_CSPlayer" ) , ( "m_vecViewOffset[0]" ) , &NetVars.m_vecViewOffset );
	g_Offsets.NetVars->GetOffset ( ( "DT_CSPlayer" ) , ( "deadflag" ) , &NetVars.deadflag );
	g_Offsets.NetVars->GetOffset ( ( "DT_CSPlayer" ) , ( "m_vecViewOffset[0]" ) , &NetVars.m_vecViewOffset );
	g_Offsets.NetVars->GetOffset ( ( "DT_CSPlayer" ) , ( "m_iAccount" ) , &NetVars.m_iAccountID );
	g_Offsets.NetVars->GetOffset ( ( "DT_CSPlayer" ) , ( "m_bIsScoped" ) , &NetVars.m_bIsScoped );
	g_Offsets.NetVars->GetOffset ( ( "DT_CSPlayer" ) , ( "m_angEyeAngles[0]" ) , &NetVars.m_angEyeAngles );
	g_Offsets.NetVars->GetOffset ( ( "DT_CSPlayer" ) , ( "m_iShotsFired" ) , &NetVars.m_iShotsFired );
	g_Offsets.NetVars->GetOffset ( ( "DT_CSPlayer" ) , ( "m_flFlashMaxAlpha" ) , &NetVars.m_flFlashMaxAlpha );
	g_Offsets.NetVars->GetOffset ( ( "DT_CSPlayer" ) , ( "m_hMyWearables" ) , &NetVars.m_hMyWearables );
	g_Offsets.NetVars->GetOffset ( ( "DT_CSPlayer" ) , ( "m_bHasHelmet" ) , &NetVars.m_bHasHelmet );
	g_Offsets.NetVars->GetOffset ( ( "DT_CSPlayer" ) , ( "m_bHasHeavyArmor" ) , &NetVars.m_bHasHeavyArmor );

	g_Offsets.NetVars->GetOffset ( ( "DT_BaseAttributableItem" ) , ( "m_iItemDefinitionIndex" ) , &NetVars.m_iItemDefinitionIndex );
	g_Offsets.NetVars->GetOffset ( ( "DT_BaseAttributableItem" ) , ( "m_iItemIDHigh" ) , &NetVars.m_iItemIDHigh );
	g_Offsets.NetVars->GetOffset ( ( "DT_BaseAttributableItem" ) , ( "m_iEntityQuality" ) , &NetVars.m_iEntityQuality );
	g_Offsets.NetVars->GetOffset ( ( "DT_BaseAttributableItem" ) , ( "m_szCustomName" ) , &NetVars.m_szCustomName );
	g_Offsets.NetVars->GetOffset ( ( "DT_BaseAttributableItem" ) , ( "m_nFallbackPaintKit" ) , &NetVars.m_nFallbackPaintKit );
	g_Offsets.NetVars->GetOffset ( ( "DT_BaseAttributableItem" ) , ( "m_flFallbackWear" ) , &NetVars.m_flFallbackWear );
	g_Offsets.NetVars->GetOffset ( ( "DT_BaseAttributableItem" ) , ( "m_nFallbackSeed" ) , &NetVars.m_nFallbackSeed );
	g_Offsets.NetVars->GetOffset ( ( "DT_BaseAttributableItem" ) , ( "m_nFallbackStatTrak" ) , &NetVars.m_nFallbackStatTrak );
	g_Offsets.NetVars->GetOffset ( ( "DT_BaseAttributableItem" ) , ( "m_OriginalOwnerXuidLow" ) , &NetVars.m_OriginalOwnerXuidLow );
	g_Offsets.NetVars->GetOffset ( ( "DT_BaseAttributableItem" ) , ( "m_OriginalOwnerXuidHigh" ) , &NetVars.m_OriginalOwnerXuidHigh );

	g_Offsets.NetVars->GetOffset ( ( "DT_BaseViewModel" ) , ( "m_nModelIndex" ) , &NetVars.m_nModelIndex );
	g_Offsets.NetVars->GetOffset ( ( "DT_BaseViewModel" ) , ( "m_hOwner" ) , &NetVars.m_hOwner );
	g_Offsets.NetVars->GetOffset ( ( "DT_BaseViewModel" ) , ( "m_hWeapon" ) , &NetVars.m_hWeapon );

	g_Offsets.NetVars->GetOffset ( ( "DT_BaseCombatWeapon" ) , ( "m_flNextPrimaryAttack" ) , &NetVars.m_flNextPrimaryAttack );
	g_Offsets.NetVars->GetOffset ( ( "DT_BaseCombatWeapon" ) , ( "m_flNextSecondaryAttack" ) , &NetVars.m_flNextSecondaryAttack );
	g_Offsets.NetVars->GetOffset ( ( "DT_BaseCombatWeapon" ) , ( "m_flPostponeFireReadyTime" ) , &NetVars.m_flPostponeFireReadyTime );
	g_Offsets.NetVars->GetOffset ( ( "DT_BaseCombatWeapon" ) , ( "m_iClip1" ) , &NetVars.m_iClip1 );
	g_Offsets.NetVars->GetOffset ( ( "DT_BaseCombatWeapon" ) , ( "m_iPrimaryReserveAmmoCount" ) , &NetVars.m_iClip2 );

	g_Offsets.NetVars->GetOffset ( ( "CBaseCSGrenade" ) , ( "m_bPinPulled" ) , &NetVars.m_bPinPulled );
	g_Offsets.NetVars->GetOffset ( ( "CBaseCSGrenade" ) , ( "m_fThrowTime" ) , &NetVars.m_fThrowTime );

	g_Offsets.NetVars->GetOffset ( ( "DT_PlantedC4" ) , ( "m_flTimerLength" ) , &NetVars.m_flTimerLength );
	g_Offsets.NetVars->GetOffset ( ( "DT_PlantedC4" ) , ( "m_bBombTicking" ) , &NetVars.m_bBombTicking );
	g_Offsets.NetVars->GetOffset ( ( "DT_PlantedC4" ) , ( "m_flC4Blow" ) , &NetVars.m_flC4Blow );
	g_Offsets.NetVars->GetOffset ( ( "DT_PlantedC4" ) , ( "m_bBombDefused" ) , &NetVars.m_bBombDefused );
	g_Offsets.NetVars->GetOffset ( ( "DT_PlantedC4" ) , ( "m_hBombDefuser" ) , &NetVars.m_hBombDefuser );
	g_Offsets.NetVars->GetOffset ( ( "DT_CSPlayer" ) , ( "m_flLowerBodyYawTarget" ) , &NetVars.m_flLowerBodyYawTarget );

	g_Offsets.NetVars->GetOffset ( ( "DT_CSGameRulesProxy" ) , ( "m_bIsValveDS" ) , &NetVars.m_bIsValveDS );
	g_Offsets.NetVars->GetOffset ( ( "DT_CSGameRulesProxy" ) , ( "m_bBombDropped" ) , &NetVars.m_bBombDropped );
	g_Offsets.NetVars->GetOffset ( ( "DT_CSGameRulesProxy" ) , ( "m_bBombPlanted" ) , &NetVars.m_bBombPlanted );
	g_Offsets.NetVars->GetOffset ( ( "DT_CSGameRulesProxy" ) , ( "m_bFreezePeriod" ) , &NetVars.m_bFreezePeriod );

	g_Offsets.NetVars->GetOffset ( ( "DT_CSPlayerResource" ) , ( "m_bHasDefuser" ) , &NetVars.m_bHasDefuser );
	g_Offsets.NetVars->GetOffset ( ( "DT_CSPlayerResource" ) , ( "m_iPing" ) , &NetVars.m_iPing );
	std::this_thread::sleep_for ( std::chrono::milliseconds ( 500 ) );
#pragma endregion
	//-----------------------------------------------------------------------------
	// Hooking.
	//-----------------------------------------------------------------------------

	g_pUtils->Print ( "Hooking:" );
	CVMTHookManager* VGuiHook = new CVMTHookManager ( ( PDWORD* ) g_pVGuiPanel );
	oPaintTraverse = ( PaintTraverse ) ( VGuiHook->HookMethod ( ( DWORD ) Hooks::PaintTraverseHook , 41 ) );
	g_pUtils->Print ( "PaintTraverse(41) -> 0x%x" , oPaintTraverse );

	CVMTHookManager* ClientModeHook = new CVMTHookManager ( ( PDWORD* ) g_pClientMode );
	oCreateMove = ( CreateMove ) ( ClientModeHook->HookMethod ( ( DWORD ) Hooks::CreateMoveHook , 24 ) );
	g_pUtils->Print ( "CreateMove(24) -> 0x%x" , oCreateMove );
	oDoPostScreenEffects = ( DoPostScreenEffects ) ( ClientModeHook->HookMethod ( ( DWORD ) Hooks::DoPostScreenEffectsHook , 44 ) );
	g_pUtils->Print ( "DoPostScreenEffects(44) -> 0x%x" , oDoPostScreenEffects );
	oOverrideView = ( OverrideView ) ( ClientModeHook->HookMethod ( ( DWORD ) Hooks::OverrideViewHook , 18 ) );
	g_pUtils->Print ( "OverrideView(18) -> 0x%x" , oOverrideView );

	CVMTHookManager* ClientHook = new CVMTHookManager ( ( PDWORD* ) g_pCHLClient );
	oFrameStageNotify = ( FrameStageNotify ) ( ClientHook->HookMethod ( ( DWORD ) Hooks::FrameStageNotifyHook , 37 ) );
	g_pUtils->Print ( "FrameStageNotify(37) -> 0x%x" , oFrameStageNotify );

	CVMTHookManager* SurfaceHook = new CVMTHookManager ( ( PDWORD* ) g_pVGuiSurface );
	oPlaySound = ( PlaySoundT ) ( SurfaceHook->HookMethod ( ( DWORD ) Hooks::PlaySoundHook , 82 ) );
	g_pUtils->Print ( "PlaySound(82) -> 0x%x" , oPlaySound );

	CVMTHookManager* RenderViewHook = new CVMTHookManager ( ( PDWORD* ) g_pRenderView );
	oSceneEnd = ( SceneEnd ) ( RenderViewHook->HookMethod ( ( DWORD ) Hooks::SceneEndHook , 9 ) );
	g_pUtils->Print ( "SceneEnd(9) -> 0x%x" , oSceneEnd );

	CVMTHookManager* MdlRenderHook = new CVMTHookManager ( ( PDWORD* ) g_pMdlRender );
	oDrawModelExecute = ( DrawModelExecute ) ( MdlRenderHook->HookMethod ( ( DWORD ) Hooks::DrawModelExecuteHook , 21 ) );
	g_pUtils->Print ( "DrawModelExecute(21) -> 0x%x" , oDrawModelExecute );

	//PlayerDeath::Get ( ).RegisterSelf ( );
	PlayerHurtEvent::Get ( ).RegisterSelf ( );
	//BulletImpactEvent::Get ( ).RegisterSelf ( );
}

void CSDK::InitFonts ( ) {
	F_Arial = g_pVGuiSurface->CreateFont_ ( );
	F_ESP = g_pVGuiSurface->CreateFont_ ( );

	g_pVGuiSurface->SetFontGlyphSet ( F_Arial , "System" , 16 , 300 , 0 , 0 , FONTFLAG_OUTLINE );
	g_pVGuiSurface->SetFontGlyphSet ( F_ESP , "System" , 10 , 180 , 0 , 0 , FONTFLAG_OUTLINE );
}








