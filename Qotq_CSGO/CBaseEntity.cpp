#include "CBaseEntity.h"
using namespace VT;
bool C_BaseEntity::IsPlayer ( )
{
	return VFunc<bool ( __thiscall* )( C_BaseEntity* )> ( this , 152 )( this );
}

float C_BaseEntity::SpawnTime ( ) {
	return *( float* ) ( ( uintptr_t ) this + 0xAD4 );
}

bool C_BaseEntity::IsWeapon ( )
{
	return VFunc<bool ( __thiscall* )( C_BaseEntity* )> ( this , 160 )( this );
}

Vector C_BaseEntity::GetViewOffset ( )
{
	return *reinterpret_cast< Vector* >( ( uintptr_t ) this + NetVars.m_vecViewOffset );
}

void C_BaseEntity::SetAbsOrigin ( Vector vec )
{
	using SetAbsOriginFn = void ( __thiscall* )( void* , const Vector &origin );
	static SetAbsOriginFn SetAbsOrigin;

	if ( !SetAbsOrigin )
		SetAbsOrigin = reinterpret_cast< SetAbsOriginFn >( g_pUtils->FindPatternIDA ( "client.dll" , "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8 ?? ??" ) );

	SetAbsOrigin ( this , vec );
}

void C_BaseEntity::SetFlags ( int flags )
{
	*reinterpret_cast< int* >( uintptr_t ( this ) + 0x100 ) = flags;
}

void C_BaseEntity::SetAbsAngles ( QAngle vec )
{
	using SetAbsAnglesFn = void ( __thiscall* )( void* , const QAngle &angles );
	static SetAbsAnglesFn SetAbsAngles;

	if ( !SetAbsAngles )
		SetAbsAngles = reinterpret_cast< SetAbsAnglesFn >( ( uintptr_t ) g_pUtils->FindPatternIDA ( "client.dll" , "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8" ) );

	SetAbsAngles ( this , vec );
}

QAngle & C_BaseEntity::GetAbsAngles ( ) {
	return VFunc<QAngle&( __thiscall* )( void* )> ( this , 11 )( this );
}

int C_BaseEntity::GetHealth ( )
{
	return *reinterpret_cast< int* >( ( uintptr_t ) this + NetVars.m_iHealth );
}

int C_BaseEntity::GetTeam ( )
{
	return *reinterpret_cast< int* >( ( uintptr_t ) this + NetVars.m_iTeamNum );
}

int C_BaseEntity::GetPing ( )
{
	return *reinterpret_cast< int* >( ( uintptr_t ) this + NetVars.m_iPing );
}

Vector C_BaseEntity::GetMins ( )
{
	return *reinterpret_cast< Vector* >( ( uintptr_t ) this + NetVars.m_vecMins );
}

Vector C_BaseEntity::GetMaxs ( )
{
	return *reinterpret_cast< Vector* >( ( uintptr_t ) this + NetVars.m_vecMaxs );
}

Vector C_BaseEntity::GetVelocity ( ) {
	return *reinterpret_cast< Vector* >( ( uintptr_t ) this + NetVars.m_vecVelocity );
}

int C_BaseEntity::GetFiredShots ( ) {
	return *reinterpret_cast< int* >( ( uintptr_t ) this + NetVars.m_iShotsFired );
}

QAngle& C_BaseEntity::GetEyeAngles ( ) {
	return *reinterpret_cast< QAngle* >( ( uintptr_t ) ( this ) + NetVars.m_angEyeAngles );
}

bool C_BaseEntity::GetDormant ( )
{
	return *reinterpret_cast< bool* >( ( uintptr_t ) this + 0xE9 );
}

ICollideable* C_BaseEntity::GetCollideable ( )
{
	typedef ICollideable*( __thiscall *OrigFn )( void* );
	return VFunc<OrigFn> ( this , 3 )( this );
}

int &C_BaseEntity::GetForceBone ( ) {
	return *reinterpret_cast< int* >( ( uintptr_t ) this + NetVars.m_nForceBone );
}

const model_t* C_BaseEntity::GetModel ( )
{
	void *pRenderable = ( void* ) ( this + 0x4 );
	typedef const model_t*( __thiscall *OrigFn )( void* );
	return VFunc<OrigFn> ( pRenderable , 8 )( pRenderable );
}

bool C_BaseEntity::PVS_SetupBones ( ) {
	if ( !this || this->GetHealth ( ) < 1 || this->GetDormant ( ) )
		return false;

	*( int* ) ( ( uintptr_t ) this + 0xA30 , g_pGlobalVars->framecount );
	*( int* ) ( ( uintptr_t ) this + 0xA28 , 0 );
	*( int* ) ( &GetForceBone ( ) + 0x20 , 0 );
	*( int* ) ( &GetForceBone ( ) + 0x4 , -1 );

	//	Skip call to AccumulateLayers
	*( int* ) ( this + 2600 ) |= 0xA;

	return true;
}

bool C_BaseEntity::SetupBones ( matrix3x4_t *pBoneToWorldOut , int nMaxBones , int boneMask , float currentTime )
{
	//DWORD m_nForceBone, m_dwOcclusionArray, m_bDidCheckForOcclusion, InvalidateBoneCache;

	//g_Offsets.NetVars->GetOffset(("DT_CSPlayer"), ("m_nForceBone"), &m_nForceBone);
	//m_dwOcclusionArray = *(uintptr_t*)(g_pUtils->FindPatternIDA("client.dll", "A1 ? ? ? ? 83 EC 30 56 57 8B F9") + 0x1);
	//m_bDidCheckForOcclusion = *(uintptr_t*)(g_pUtils->FindPatternIDA("client.dll", "A1 ? ? ? ? 83 EC 30 56 57 8B F9") + 0xE);
	//InvalidateBoneCache = *(uintptr_t*)(g_pUtils->FindPatternIDA("client.dll", "80 3D ? ? ? ? 00 74 16 A1"));

	//*(int*)((uintptr_t)this + m_nForceBone) = 0;
	//*(int*)((uintptr_t)this + m_bDidCheckForOcclusion) = reinterpret_cast<int*>(m_dwOcclusionArray)[1];
	//reinterpret_cast<void(__fastcall*)(void*)>(InvalidateBoneCache);

	void *pRenderable = ( void* ) ( this + 0x4 );
	typedef bool ( __thiscall *OrigFn )( void* , matrix3x4_t* , int , int , float );
	return VFunc<OrigFn> ( pRenderable , 13 )( pRenderable , pBoneToWorldOut , nMaxBones , boneMask , currentTime );
}

int C_BaseEntity::SetObserverMode ( int mode )
{
	return *reinterpret_cast< int* >( uintptr_t ( this ) + NetVars.m_iObserverMode ) = mode;
}

int C_BaseEntity::GetObserverTarget ( ) {
	return *reinterpret_cast< int* >( uintptr_t ( this ) + NetVars.m_hObserverTarget );
}

float &C_BaseEntity::LowerBodyYaw ( )
{
	return *reinterpret_cast< float* >( uintptr_t ( this ) + NetVars.m_flLowerBodyYawTarget );
}

std::array<float , 24> &C_BaseEntity::PoseParams ( )
{
	return *( std::array<float , 24>* )( ( uintptr_t ) this + NetVars.m_flPoseParameter );
}

float C_BaseEntity::NextAttack ( ) {
	return *reinterpret_cast< float* >( ( uintptr_t ) this + NetVars.m_flNextAttack );
}

ClientClass* C_BaseEntity::GetClientClass ( )
{
	void *pNetworked = ( void* ) ( this + 0x8 );
	typedef ClientClass*( __thiscall *OrigFn )( void* );
	return VFunc<OrigFn> ( pNetworked , 2 )( pNetworked );
}

int C_BaseEntity::GetIndex ( )
{
	return *reinterpret_cast< int* >( ( uintptr_t ) this + 0x64 );
}

int &C_BaseEntity::GetFlags ( )
{
	return *reinterpret_cast< int* >( ( uintptr_t ) this + NetVars.m_fFlags );
}

// Invalidates the abs state of all children
void C_BaseEntity::InvalidatePhysicsRecursive ( int nChangeFlags )
{
	static auto sig = g_pUtils->FindPatternIDA ( "client.dll" , "55  8B  EC  83  E4  F8  83  EC  0C  53  8B  5D  08  8B  C3  56  83  E0  04" );

	if ( sig )
		return ( ( void ( __thiscall* )( C_BaseEntity* , int ) )sig )( this , nChangeFlags );
}

void C_BaseEntity::InvalidateBoneCache ( )
{
	static auto sig = ( DWORD ) g_pUtils->FindPatternIDA ( "client.dll" , "80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81" );
	if ( sig ) {
		unsigned long g_iModelBoneCounter = **( unsigned long** ) ( sig + 10 );
		*( unsigned int* ) ( ( DWORD ) this + 0x2914 ) = 0xFF7FFFFF; // m_flLastBoneSetupTime = -FLT_MAX;
		*( unsigned int* ) ( ( DWORD ) this + 0x2680 ) = ( g_iModelBoneCounter - 1 ); // m_iMostRecentModelBoneCounter = g_iModelBoneCounter - 1;
	}
}

int C_BaseEntity::GetNumAnimOverlays ( )
{
	return *( int* ) ( ( DWORD ) this + 0x297C );
}

AnimationLayer *C_BaseEntity::GetAnimOverlays ( )
{
	// to find offset: use 9/12/17 dll
	// sig: 55 8B EC 51 53 8B 5D 08 33 C0
	return *( AnimationLayer** ) ( ( DWORD ) this + 10608 );
}

AnimationLayer *C_BaseEntity::GetAnimOverlay ( int i )
{
	if ( i < 15 )
		return &GetAnimOverlays ( ) [ i ];
}

int C_BaseEntity::GetSequenceActivity ( int sequence )
{
	auto hdr = g_pMdlInfo->GetStudioModel ( this->GetModel ( ) );

	if ( !hdr )
		return -1;

	// sig for stuidohdr_t version: 53 56 8B F1 8B DA 85 F6 74 55
	// sig for C_BaseAnimating version: 55 8B EC 83 7D 08 FF 56 8B F1 74 3D
	// c_csplayer VFunc 242, follow calls to find the function.
	static auto sig = ( DWORD ) g_pUtils->FindPatternIDA ( "client.dll" , "55 8B EC 83 7D 08 FF 56 8B F1 74 3D" );
	static auto get_sequence_activity = reinterpret_cast< int ( __fastcall* )( void* , studiohdr_t* , int ) >( sig );

	return get_sequence_activity ( this , hdr , sequence );
}

CBasePlayerAnimState *C_BaseEntity::GetBasePlayerAnimState ( )
{
	static int basePlayerAnimStateOffset = 0x3884;
	return *( CBasePlayerAnimState** ) ( ( DWORD ) this + basePlayerAnimStateOffset );
}

CCSPlayerAnimState *C_BaseEntity::GetPlayerAnimState ( )
{
	return *( CCSPlayerAnimState** ) ( ( DWORD ) this + 0x3870 );
}

void C_BaseEntity::UpdateAnimationState ( CCSGOPlayerAnimState *state , QAngle angle )
{
	typedef void ( __vectorcall *fnUpdateAnimState )( PVOID , PVOID , float , float , float , PVOID );
	static auto UpdateAnimState = ( fnUpdateAnimState ) g_pUtils->FindPatternIDA ( "client.dll" , "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24" );
	if ( !UpdateAnimState )
		return;

	UpdateAnimState ( state , NULL , NULL , angle.yaw , angle.pitch , NULL );

	return;

	__asm
	{
		mov ecx , state

		movss xmm1 , dword ptr [ angle + 4 ]
		movss xmm2 , dword ptr [ angle ]

		call UpdateAnimState
	}
}

void C_BaseEntity::ResetAnimationState ( CCSGOPlayerAnimState *state )
{
	using ResetAnimState_t = void ( __thiscall* )( CCSGOPlayerAnimState* );
	static auto ResetAnimState = ( ResetAnimState_t ) g_pUtils->FindPatternIDA ( ( "client.dll" ) , "56 6A 01 68 ? ? ? ? 8B F1" );
	if ( !ResetAnimState )
		return;

	ResetAnimState ( state );
}

void C_BaseEntity::CreateAnimationState ( CCSGOPlayerAnimState *state )
{
	using CreateAnimState_t = void ( __thiscall* )( CCSGOPlayerAnimState* , C_BaseEntity* );
	static auto CreateAnimState = ( CreateAnimState_t ) g_pUtils->FindPatternIDA ( ( "client.dll" ) , "55 8B EC 56 8B F1 B9 ? ? ? ? C7 46" );
	if ( !CreateAnimState )
		return;

	CreateAnimState ( state , this );
}

void C_BaseEntity::UpdateClientSideAnimation ( )
{
	typedef void ( __thiscall *o_updateClientSideAnimation )( void* );
	return VFunc<o_updateClientSideAnimation> ( this , 218 )( this );
}

Vector C_BaseEntity::GetOrigin ( )
{
	return *reinterpret_cast< Vector* >( ( uintptr_t ) this + 0x134 );
}

const matrix3x4_t& C_BaseEntity::GetCoordinateFrame ( )
{
	return *reinterpret_cast< matrix3x4_t* >( ( uintptr_t ) this + NetVars.m_rgflCoordinateFrame );
}

Vector C_BaseEntity::GetEyePos ( )
{
	Vector vec;
	VFunc < void ( __thiscall* )( void* , Vector& ) > ( this , 277 )( this , vec );
	return vec;
}

Vector C_BaseEntity::GetHitboxPos ( int hitbox_id )
{
	matrix3x4_t boneMatrix [ MAXSTUDIOBONES ];

	if ( PVS_SetupBones ( ) )
		if ( SetupBones ( boneMatrix , MAXSTUDIOBONES , BONE_USED_BY_HITBOX , 0.0f ) ) {
			auto studio_model = g_pMdlInfo->GetStudioModel ( GetModel ( ) );
			if ( studio_model ) {
				auto hitbox = studio_model->pHitboxSet ( 0 )->pHitbox ( hitbox_id );
				if ( hitbox ) {
					auto
						min = Vector {} ,
						max = Vector {};

					Math::VectorTransform ( hitbox->bbmin , boneMatrix [ hitbox->bone ] , min );
					Math::VectorTransform ( hitbox->bbmax , boneMatrix [ hitbox->bone ] , max );

					return ( min + max ) / 2.0f;
				}
			}
		}
	return Vector {};
}

bool C_BaseEntity::GetHitboxPos ( int hitbox , Vector &output )
{
	if ( hitbox >= HITBOX_MAX )
		return false;

	const model_t *model = this->GetModel ( );
	if ( !model )
		return false;

	studiohdr_t *studioHdr = g_pMdlInfo->GetStudioModel ( model );
	if ( !studioHdr )
		return false;

	matrix3x4_t matrix [ MAXSTUDIOBONES ];
	if ( !this->SetupBones ( matrix , MAXSTUDIOBONES , 0x100 , 0 ) )
		return false;

	mstudiobbox_t *studioBox = studioHdr->pHitboxSet ( 0 )->pHitbox ( hitbox );
	if ( !studioBox )
		return false;

	Vector min , max;

	Math::VectorTransform ( studioBox->bbmin , matrix [ studioBox->bone ] , min );
	Math::VectorTransform ( studioBox->bbmax , matrix [ studioBox->bone ] , max );

	output = ( min + max ) * 0.5f;

	return true;
}

Vector C_BaseEntity::GetBonePos ( int bone )
{
	matrix3x4_t boneMatrix [ MAXSTUDIOBONES ];

	if ( SetupBones ( boneMatrix , MAXSTUDIOBONES , BONE_USED_BY_ANYTHING , 0.0f ) ) {
		return Vector ( boneMatrix [ bone ] [ 0 ] [ 3 ] , boneMatrix [ bone ] [ 1 ] [ 3 ] , boneMatrix [ bone ] [ 2 ] [ 3 ] );
	}
	return Vector {};
}

bool C_BaseEntity::CanSeePlayer ( C_BaseEntity* player , int hitbox )
{
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = this;

	auto endpos = player->GetHitboxPos ( hitbox );

	ray.Init ( GetEyePos ( ) , endpos );
	g_pEngineTrace->TraceRay ( ray , MASK_SHOT | CONTENTS_GRATE , &filter , &tr );

	return ( C_BaseEntity* ) tr.hit_entity == player || tr.fraction > 0.97f;
}

bool C_BaseEntity::CanSeePlayer ( C_BaseEntity* player , const Vector& pos )
{
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = this;

	auto start = GetEyePos ( );
	auto dir = ( pos - start ).Normalized ( );

	ray.Init ( start , pos );
	g_pEngineTrace->TraceRay ( ray , MASK_SHOT | CONTENTS_GRATE , &filter , &tr );

	return ( C_BaseEntity* ) tr.hit_entity == player || tr.fraction > 0.97f;
}

bool C_BaseEntity::bSpotted ( )
{
	return *reinterpret_cast< bool* >( ( uintptr_t ) this + 0x939 );
}

QAngle &C_BaseEntity::visuals_Angles ( )
{
	return *reinterpret_cast< QAngle* >( ( uintptr_t ) this + NetVars.deadflag + 4 );
}

bool IsVisible ( const Vector & start , const Vector & end , C_BaseEntity * pPlayer , C_BaseEntity * pLocalPlayer )
{
	Ray_t ray;
	trace_t tr;
	ray.Init ( start , end );

	CTraceFilter filter;
	filter.pSkip = pLocalPlayer;

	g_pEngineTrace->TraceRay ( ray , MASK_SHOT , &filter , &tr );

	if ( ( C_BaseEntity* ) tr.hit_entity == pPlayer || tr.fraction > 0.97f )
	{
		return true;
	}
	return false;
}

bool C_BaseEntity::GetPointsFromHitbox ( C_BaseEntity* pBaseEntity , const int& iHitbox , std::vector<Vector>& points )
{
	if ( !this )
		return false;

	matrix3x4_t cCachedBones [ 128 ];
	if ( this->PVS_SetupBones ( ) )
		if ( !this->SetupBones ( cCachedBones , 128 , 0x100 , 0.f ) )
			return false;


	const auto* pModel = this->GetModel ( );
	if ( !pModel )
		return false;

	auto* pStudioHdr = g_pMdlInfo->GetStudioModel ( pModel );
	if ( !pStudioHdr )
		return false;
	mstudiohitboxset_t *s = pStudioHdr->pHitboxSet ( 0 );
	if ( !s )
		return false;
	auto* pHitbox = s->pHitbox ( iHitbox );
	if ( !pHitbox )
		return false;

	auto& mat = cCachedBones [ pHitbox->bone ];

	auto c = Math::CalcAngle2 ( pBaseEntity->GetEyePos ( ) , this->GetEyePos ( ) );

	float cyaw = c.yaw * ( 3.1415926f / 180.f );
	auto cs = cosf ( cyaw );
	auto sn = sinf ( cyaw );

	auto mod = pHitbox->m_flRadius != -1.f ? pHitbox->m_flRadius : 0.f;

	Vector mins;
	Math::VectorTransform ( pHitbox->bbmin - Vector ( mod , mod , mod ) , mat , mins );
	Vector maxs;
	Math::VectorTransform ( pHitbox->bbmax + Vector ( mod , mod , mod ) , mat , maxs );
	auto center = ( mins + maxs ) * 0.5f;

	auto fscale = ( iHitbox == HITBOX_HEAD ) ? 0.5f : 0.2f;
	float rs = mod * fscale;

	switch ( iHitbox )
	{
	case HITBOX_HEAD:
	{
		points.push_back ( center );
		if ( !IsVisible ( pBaseEntity->GetEyePos ( ) , center , this , pBaseEntity ) )
		{
			points.push_back ( Vector ( center.x , center.y , center.z + rs ) );
			points.push_back ( Vector ( center.x + cs + rs * sn , center.y + sn - rs * cs , center.z ) );
			points.push_back ( Vector ( center.x + cs - rs * sn , center.y + sn + rs * cs , center.z ) );
		}
		break;
	}
	case HITBOX_PELVIS:
	{
		points.push_back ( center );
		if ( !IsVisible ( pBaseEntity->GetEyePos ( ) , center , this , pBaseEntity ) )
		{
			points.push_back ( Vector ( center.x , center.y , center.z + rs ) );
			points.push_back ( Vector ( center.x + cs + rs * sn , center.y + sn - rs * cs , center.z ) );
			points.push_back ( Vector ( center.x + cs - rs * sn , center.y + sn + rs * cs , center.z ) );
		}
		break;
	}
	default:
	{
		points.push_back ( center );
		break;
	}
	}
	//g_pUtils->Print("points: about to return true");

	return true;
}

int C_BaseEntity::GetArmor ( )
{
	return *reinterpret_cast< int* >( ( uintptr_t ) this + NetVars.m_ArmorValue );
}

bool C_BaseEntity::Hashelmet ( )
{
	return *reinterpret_cast< bool* >( ( uintptr_t ) this + NetVars.m_bHasHelmet );
}

bool C_BaseEntity::HeavyArmor ( )
{
	return *reinterpret_cast< bool* >( ( uintptr_t ) this + NetVars.m_bHasHeavyArmor );
}

//int C_BaseEntity::DrawModel(int flags, uint8_t alpha) {
//	typedef bool(__thiscall *OrigFn)(void*, int, uint8_t);
//	return VFunc<OrigFn>(GetClientRenderable(), 9)(GetClientRenderable(), flags, alpha);
//}

int C_BaseEntity::GetTickBase ( )
{
	return *reinterpret_cast< int* >( ( uintptr_t ) this + NetVars.m_nTickBase );
}

QAngle C_BaseEntity::GetPunchAngles ( )
{
	return *reinterpret_cast< QAngle* >( ( uintptr_t ) this + NetVars.m_vecPunchAngles );
}

QAngle C_BaseEntity::GetVPunchAngles ( )
{
	return *reinterpret_cast< QAngle* >( ( uintptr_t ) this + NetVars.m_vecViewPunchAngles );
}

CBaseHandle C_BaseEntity::hActiveWeapon ( )
{
	return *reinterpret_cast< CBaseHandle* >( ( uintptr_t ) this + NetVars.m_hActiveWeapon );
}

C_BaseWeapon* C_BaseEntity::GetActiveWeapon ( )
{
	return ( C_BaseWeapon* ) g_pEntityList->GetClientEntityFromHandle ( ( CBaseHandle ) hActiveWeapon ( ) );
}

float &C_BaseEntity::SimTime ( ) {
	return *reinterpret_cast< float* >( ( uintptr_t ) this + NetVars.m_flSimulationTime );
}

bool &C_BaseEntity::DoClientSideAnimation ( )
{
	return *reinterpret_cast< bool* >( ( uintptr_t ) this + NetVars.m_bClientSideAnimation );
}

bool C_BaseEntity::GetScoped ( )
{
	return *reinterpret_cast< bool* >( ( uintptr_t ) this + NetVars.m_bIsScoped );
}

int C_BaseEntity::GetHitboxSet ( )
{
	return *( int* ) ( ( DWORD ) this + NetVars.m_nHitboxSet );
}

MoveType_t C_BaseEntity::GetMoveType ( ) {
	return *reinterpret_cast< MoveType_t* >( ( uintptr_t ) this + 0x258 );
}

int C_BaseEntity::GetMoney ( )
{
	return *reinterpret_cast< int* >( ( uintptr_t ) this + NetVars.m_iAccountID );
}