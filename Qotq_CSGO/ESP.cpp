#include "ESP.h"
#include <iomanip> // setprecision
#include <sstream> // stringstream
void CESP::Start( ) {
	int x, y;
	g_pVGuiSurface->GetScreenSize( x, y );
	Draw->FilledRect( x / 2 - 4, y / 2 - 1, 8, 2, Color( 255, 0, 0 ) );
	Draw->FilledRect( x / 2 - 1, y / 2 - 4, 2, 8, Color( 255, 0, 0 ) );

	for ( int i = 0; i <= g_pEngine->GetMaxClients( ); i++ ) {
		auto pLocal = g_pEntityList->GetClientEntity( g_pEngine->GetLocalPlayer( ) );
		auto pEntity = g_pEntityList->GetClientEntity( i );

		if ( !pEntity )
			continue;
		if ( pEntity->GetTeam( ) == pLocal->GetTeam( ) )
			continue;
		if ( pEntity->GetDormant( ) )
			continue;
		if ( pEntity->GetHealth( ) < 1 )
			continue;

		RECT EspRect;
		player_info_t info;
		EspRect = GetBox( pEntity );
		if ( IsRectEmpty( &EspRect ) ) continue;
		g_pEngine->GetPlayerInfo( pEntity->GetIndex( ), &info );
		if ( g_Settings.visuals.esp == 1 || g_Settings.visuals.esp == 3 )		PlayerBox( pEntity, EspRect );
		if ( g_Settings.visuals.esp > 1 )										PlayerName( pEntity, EspRect, info );
		if ( g_Settings.visuals.esp > 1 )										PlayerWeapon( pEntity, EspRect );
		if ( g_Settings.visuals.esp > 1 )										PlayerHealth( pEntity, EspRect );
		if ( g_Settings.visuals.esp > 1 )										PlayerResolver( pEntity, EspRect );
		if ( g_Settings.visuals.skeleton > 0 )									PlayerSkeleton( pEntity );
	}
}
void CESP::PlayerBox( C_BaseEntity* pEntity, RECT EspRect ) {
	int hp_red = 255 - ( pEntity->GetHealth( ) * 2.55 );
	int hp_green = pEntity->GetHealth( ) * 2.55;
	Color health_color = Color( hp_red, hp_green, 0, 255 );
	Draw->OutlinedRect( EspRect.left, EspRect.top, EspRect.right - EspRect.left, EspRect.bottom - EspRect.top, health_color );
	Draw->OutlinedRect( EspRect.left - 1, EspRect.top - 1, EspRect.right - EspRect.left + 2, EspRect.bottom - EspRect.top + 2, Color::Black );
	Draw->OutlinedRect( EspRect.left + 1, EspRect.top + 1, EspRect.right - EspRect.left - 2, EspRect.bottom - EspRect.top - 2, Color::Black );
}
void CESP::PlayerResolver( C_BaseEntity* pEntity, RECT EspRect ) {
	int hp_red = 255 - ( pEntity->GetHealth( ) * 2.55 );
	int hp_green = pEntity->GetHealth( ) * 2.55;
	Color health_color = Color( hp_red, hp_green, 0, 255 );

	stringstream stream;
	stream << fixed << setprecision( 2 ) << Resolver::Get( ).secondary [ pEntity->GetIndex( ) ].relative_delta;

	Draw->String( F_ESP, false, EspRect.right + 2, EspRect.top + 42, health_color, Resolver::Get( ).resolvertype [ pEntity->GetIndex( ) ].c_str( ) );
	Draw->String( F_ESP, false, EspRect.right + 2, EspRect.top + 42 + 14, health_color, std::string( "DLT: " + stream.str( ) ).c_str( ) );
	Draw->String( F_ESP, false, EspRect.right + 2, EspRect.top + 42 + 14 * 2, health_color, std::string( "LC: " + std::to_string( ( int ) CBacktrack::Get( ).m_PlayerTrack [ pEntity->GetIndex( ) - 1 ].size( ) ) ).c_str( ) );
}
void CESP::PlayerHealth( C_BaseEntity* pEntity, RECT EspRect ) {
	int hp_red = 255 - ( pEntity->GetHealth( ) * 2.55 );
	int hp_green = pEntity->GetHealth( ) * 2.55;
	Color health_color = Color( hp_red, hp_green, 0, 255 );

	Draw->String( F_ESP, false, EspRect.right + 2, EspRect.top + 14, health_color, "HP:%d", pEntity->GetHealth( ) );
}
void CESP::PlayerWeapon( C_BaseEntity* pEntity, RECT EspRect ) {
	auto weapon = pEntity->GetActiveWeapon( );
	int hp_red = 255 - ( pEntity->GetHealth( ) * 2.55 );
	int hp_green = pEntity->GetHealth( ) * 2.55;
	Color health_color = Color( hp_red, hp_green, 0, 255 );
	auto weapondata = weapon->GetCSWeaponData( );
	if ( weapondata )
		Draw->String( F_ESP, false, EspRect.right + 2, EspRect.top + 28, health_color, "%s[%d/%d]", weapon->GetWeaponName( ).c_str( ), weapon->Clip1( ), weapondata->max_clip );

}
void CESP::PlayerName( C_BaseEntity* pEntity, RECT EspRect, player_info_t info ) {
	std::string name = info.szName;
	int hp_red = 255 - ( pEntity->GetHealth( ) * 2.55 );
	int hp_green = pEntity->GetHealth( ) * 2.55;
	Color health_color = Color( hp_red, hp_green, 0, 255 );
	Draw->String( F_ESP, false, EspRect.right + 2, EspRect.top, health_color, name.c_str( ) );
}
void CESP::PlayerSkeleton( C_BaseEntity* pEntity ) {
	studiohdr_t *studioHdr = g_pMdlInfo->GetStudioModel( pEntity->GetModel( ) );
	if ( studioHdr )
	{
		static matrix3x4_t boneToWorldOut [ 128 ];
		//if (pEntity->PVS_SetupBones())
		if ( pEntity->SetupBones( boneToWorldOut, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, g_pGlobalVars->curtime ) )
		{
			int hp_red = 255 - ( pEntity->GetHealth( ) * 2.55 );
			int hp_green = pEntity->GetHealth( ) * 2.55;
			Color health_color = Color( hp_red, hp_green, 0, 255 );

			for ( int i = 0; i < studioHdr->numbones; i++ )
			{
				mstudiobone_t *bone = studioHdr->pBone( i );
				if ( !bone || !( bone->flags & BONE_USED_BY_HITBOX ) || bone->parent == -1 )
					continue;

				Vector bonePos1;
				if ( !WorldToScreen( Vector( boneToWorldOut [ i ] [ 0 ] [ 3 ], boneToWorldOut [ i ] [ 1 ] [ 3 ], boneToWorldOut [ i ] [ 2 ] [ 3 ] ), bonePos1 ) )
					continue;

				Vector bonePos2;
				if ( !WorldToScreen( Vector( boneToWorldOut [ bone->parent ] [ 0 ] [ 3 ], boneToWorldOut [ bone->parent ] [ 1 ] [ 3 ], boneToWorldOut [ bone->parent ] [ 2 ] [ 3 ] ), bonePos2 ) )
					continue;

				Draw->Line( ( int ) bonePos1.x, ( int ) bonePos1.y, ( int ) bonePos2.x, ( int ) bonePos2.y, health_color );
			}
		}
	}
}
void CESP::PlayerCross( C_BaseEntity* pEntity ) {
	int hp_red = 255 - ( pEntity->GetHealth( ) * 2.55 );
	int hp_green = pEntity->GetHealth( ) * 2.55;
	Color health_color = Color( hp_red, hp_green, 0, 255 );

	Vector bonePos;
	if ( !WorldToScreen( pEntity->GetBonePos( 8 ), bonePos ) )
		return;

	Draw->FilledRect( ( int ) bonePos.x - 4, ( int ) bonePos.y - 4, 8, 8, health_color );
}

bool CESP::WorldToScreen( const Vector& in, Vector& out ) {
	static ptrdiff_t ptrViewMatrix;
	if ( !ptrViewMatrix )
	{
		ptrViewMatrix = static_cast< ptrdiff_t >( g_pUtils->FindPatternIDA( "client.dll", "0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9" ) );
		ptrViewMatrix += 0x3;
		ptrViewMatrix = *reinterpret_cast< uintptr_t* >( ptrViewMatrix );
		ptrViewMatrix += 176;
	}
	const matrix3x4_t& worldToScreen = *( matrix3x4_t* ) ptrViewMatrix;

	int ScrW, ScrH;

	g_pEngine->GetScreenSize( ScrW, ScrH );

	float w = worldToScreen [ 3 ] [ 0 ] * in [ 0 ] + worldToScreen [ 3 ] [ 1 ] * in [ 1 ] + worldToScreen [ 3 ] [ 2 ] * in [ 2 ] + worldToScreen [ 3 ] [ 3 ];
	out.z = 0;
	if ( w > 0.01 )
	{
		float inverseWidth = 1 / w;
		out.x = ( ScrW / 2 ) + ( 0.5 * ( ( worldToScreen [ 0 ] [ 0 ] * in [ 0 ] + worldToScreen [ 0 ] [ 1 ] * in [ 1 ] + worldToScreen [ 0 ] [ 2 ] * in [ 2 ] + worldToScreen [ 0 ] [ 3 ] ) * inverseWidth ) * ScrW + 0.5 );
		out.y = ( ScrH / 2 ) - ( 0.5 * ( ( worldToScreen [ 1 ] [ 0 ] * in [ 0 ] + worldToScreen [ 1 ] [ 1 ] * in [ 1 ] + worldToScreen [ 1 ] [ 2 ] * in [ 2 ] + worldToScreen [ 1 ] [ 3 ] ) * inverseWidth ) * ScrH + 0.5 );
		return true;
	}
	return false;
}

RECT CESP::GetBox( C_BaseEntity* pEntity ) {
	RECT rect {};
	auto collideable = pEntity->GetCollideable( );

	if ( !collideable )
		return rect;

	auto min = collideable->OBBMins( );
	auto max = collideable->OBBMaxs( );

	auto trans = pEntity->GetCoordinateFrame( );

	Vector points [ ] = {
		Vector( min.x, min.y, min.z ),
		Vector( min.x, max.y, min.z ),
		Vector( max.x, max.y, min.z ),
		Vector( max.x, min.y, min.z ),
		Vector( max.x, max.y, max.z ),
		Vector( min.x, max.y, max.z ),
		Vector( min.x, min.y, max.z ),
		Vector( max.x, min.y, max.z )
	};

	Vector pointsTransformed [ 8 ];
	for ( int i = 0; i < 8; i++ ) {
		Math::VectorTransform( points [ i ], trans, pointsTransformed [ i ] );
	}

	Vector screen_points [ 8 ] = {};

	for ( int i = 0; i < 8; i++ ) {
		if ( !WorldToScreen( pointsTransformed [ i ], screen_points [ i ] ) )
			return rect;
	}

	auto left = screen_points [ 0 ].x;
	auto top = screen_points [ 0 ].y;
	auto right = screen_points [ 0 ].x;
	auto bottom = screen_points [ 0 ].y;

	for ( int i = 1; i < 8; i++ ) {
		if ( left > screen_points [ i ].x )
			left = screen_points [ i ].x;
		if ( top < screen_points [ i ].y )
			top = screen_points [ i ].y;
		if ( right < screen_points [ i ].x )
			right = screen_points [ i ].x;
		if ( bottom > screen_points [ i ].y )
			bottom = screen_points [ i ].y;
	}
	return RECT { ( long ) left, ( long ) bottom, ( long ) right, ( long ) top };

}
CESP* g_pESP;