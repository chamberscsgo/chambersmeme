#pragma once
#include "SDK.h"
#include "singleton.h"
#define MAX_LAYER_RECORDS 15
#define MAX_PLAYERS 64

struct CIncomingSequence
{
	CIncomingSequence::CIncomingSequence( int instate, int outstate, int seqnr, float time )
	{
		inreliablestate = instate;
		outreliablestate = outstate;
		sequencenr = seqnr;
		curtime = time;
	}
	int inreliablestate;
	int outreliablestate;
	int sequencenr;
	float curtime;
};

struct LayerRecord
{
	int m_nSequence;
	float m_flCycle;
	float m_flWeight;
	int m_nOrder;

	LayerRecord( )
	{
		m_nSequence = 0;
		m_flCycle = 0;
		m_flWeight = 0;
		m_nOrder = 0;
	}

	LayerRecord( const LayerRecord& src )
	{
		m_nSequence = src.m_nSequence;
		m_flCycle = src.m_flCycle;
		m_flWeight = src.m_flWeight;
		m_nOrder = src.m_nOrder;
	}
};

struct LagRecord
{
	LagRecord( )
	{
		m_fFlags = 0;
		m_vecOrigin.Init( );
		m_vecAbsAngles.Init( );
		m_flSimulationTime = -1;
		m_bIsBreakingLagComp = false;
		m_flLowerBodyYawTarget = 0.f;
		m_masterCycle = 0.f;
		m_masterSequence = 0;

	}

	LagRecord( const LagRecord& src )
	{
		m_fFlags = src.m_fFlags;
		m_vecOrigin = src.m_vecOrigin;
		m_vecAbsAngles = src.m_vecAbsAngles;
		m_flSimulationTime = src.m_flSimulationTime;
		m_bIsBreakingLagComp = src.m_bIsBreakingLagComp;
		m_flLowerBodyYawTarget = src.m_flLowerBodyYawTarget;
		m_masterCycle = src.m_masterCycle;
		m_masterSequence = src.m_masterSequence;
		for ( int poseparams = 0; poseparams < 24; poseparams++ )
		{
			m_flPoseParameter [ poseparams ] = src.m_flPoseParameter [ poseparams ];
		}
	}

	// Did player die this frame
	int						m_fFlags;
	Vector					m_vecOrigin;
	Vector					m_vecVelocity;
	QAngle					m_vecAbsAngles;
	QAngle					m_angEyeAngles;
	float					m_flLowerBodyYawTarget;

	Vector					m_bbmin;
	Vector					m_bbmax;

	float					m_flSimulationTime;
	matrix3x4_t				m_nBones [ MAXSTUDIOBONES ];
	bool					m_bIsBreakingLagComp;

	float					m_flPoseParameter [ 24 ];

	float					m_masterCycle;
	int						m_masterSequence;

	LayerRecord				m_layerRecords [ MAX_LAYER_RECORDS ];
};


class VarMapEntry_t
{
public:
	unsigned short type;
	unsigned short m_bNeedsToInterpolate; // Set to false when this var doesn't
										  // need Interpolate() called on it anymore.
	void *data;
	void *watcher;
};

struct VarMapping_t
{
	VarMapping_t( )
	{
		m_nInterpolatedEntries = 0;
	}

	VarMapEntry_t* m_Entries;
	int m_nInterpolatedEntries;
	float m_lastInterpolationTime;
};

class CBacktrack : public singleton<CBacktrack>
{
public:
	bool StartLagCompensation( C_BaseEntity * player, CUserCmd * cmd, int bestrecord );
	int FixTickcount( C_BaseEntity * player, int bestrecord );
	void FSN( );
	bool ValidTick( float simTime );
	int GetBestTick( C_BaseEntity * pPlayer );
	int GetFirstTick( C_BaseEntity * pPlayer );
	LagRecord CreateRecord( C_BaseEntity * entity );
	void RestoreRecord( LagRecord & record, C_BaseEntity * entity );
	bool IsValid( C_BaseEntity * pPlayer );

	void LocalLegmeme( C_BaseEntity * player );
	void FixAnims( C_BaseEntity * player );

	void PerformExtrapolation( C_BaseEntity * player, float &s_simtime );
	void SetOverwriteTick( C_BaseEntity * player, QAngle angles, float correct_time );

	std::deque< std::pair<LagRecord, int> >	m_PlayerTrack [ MAX_PLAYERS ];
	LagRecord m_PrevRecords [ MAX_PLAYERS ] [ 2 ];

	int enemy_tick [ 65 ];

	void ClearHistory( )
	{
		for ( int i = 0; i < MAX_PLAYERS; i++ )
			m_PlayerTrack [ i ].clear( );
	}


	VarMapping_t* GetVarMap( void* pBaseEntity )
	{
		return reinterpret_cast< VarMapping_t* >( ( DWORD ) pBaseEntity + 0x24 ); //0x4C );
	}

	void DisableInterpolation( C_BaseEntity* pEntity )
	{
		VarMapping_t* map = GetVarMap( pEntity );
		if ( !map ) return;
		for ( int i = 0; i < map->m_nInterpolatedEntries; i++ )
		{
			VarMapEntry_t *e = &map->m_Entries [ i ];
			e->m_bNeedsToInterpolate = false;
		}
	}
};