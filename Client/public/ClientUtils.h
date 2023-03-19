#pragma once
#include "Client_Defines.h"

#define PLAYERTEST_LAYER_TERRAIN	TEXT("Layer_Terrain")
#define PLATERTEST_LAYER_PLAYER		TEXT("Layer_Player")
#define PLAYERTEST_LAYER_CAMERA		TEXT("Layer_Camera")
#define PLAYERTEST_LAYER_BATCH		TEXT("Layer_Batch")
#define PLAYERTEST_LAYER_MAP		TEXT("Layer_Map")
#define PLAYERTEST_LAYER_KINETIC	TEXT("Layer_Kinetic")
#define PLAYERTEST_LAYER_MONSTER	TEXT("Layer_Monster")
#define PLAYERTEST_LAYER_POSTVFX	TEXT("Layer_PostVFX")
#define PLAYERTEST_LAYER_FRONTUI	TEXT("Layer_FrontUI")

BEGIN(Engine)
class CTransform;
class CComponent;
END

// #ifdef _DEBUG
// #define DEBUG_RAY(vOrigin, vDir, fLife) CClientUtils::CloneDebugRay(vOrigin, vDir, fLife);
// #define DEBUG_RAY(vOrigin, vDir) CClientUtils::CloneDebugRay(vOrigin, vDir, 1.f);
// #define DEBUG_SPHERE(vOrigin, fRadius, fLife) CClientUtils::CloneDebugSphere(vOrigin, fRadius, fLife);
// #define DEBUG_SPHERE(vOrigin, fRadius) CClientUtils::CloneDebugSphere(vOrigin, fRadius, 1.f);
// #else
// #define DEBUG_RAY(vOrigin, vDir, fLife) 
// #define DEBUG_RAY(vOrigin, vDir) 
// #define DEBUG_SPHERE(vOrigin, fRadius, fLife) 
// #define DEBUG_SPHERE(vOrigin, fRadius) 
// #endif

#define LAYER_SAS L"Layer_SAS"

BEGIN(Client)

enum class EMoveAxis
{
	NORTH,
	NORTH_EAST,
	EAST,
	SOUTH_EAST,
	SOUTH,
	SOUTH_WEST,
	WEST,
	NORTH_WEST,
	CENTER,
	AXIS_END
};

enum class EBaseAxis
{
	NORTH,//look
	EAST,//right
	SOUTH,//back
	WEST,//left
	AXIS_END
};

enum class EBaseTurn
{
	TURN_LEFT,
	TURN_RIGHT,
	TURN_END
};

enum class ESASType
{
	SAS_FIRE, // 불
	SAS_PENETRATE, // 투시
	SAS_HARDBODY, // 경질화
	SAS_TELEPORT, // 텔레포트
	SAS_ELETRIC, // 전기
	SAS_SUPERSPEED, // 초고속
	SAS_COPY, // 복제
	SAS_INVISIBLE, // 투명화
	SAS_GRAVIKENISIS, // 염력 
	SAS_NOT, 
	SAS_END
};

enum class EDeBuffType
{
	DEBUFF_FIRE, // 화상
	DEBUFF_OIL, // 기름
	DEBUFF_THUNDER, // 감전
	DEBUFF_WATER, // 물
	DEBUFF_END
};

enum class EAttackType
{
	ATK_LIGHT,
	ATK_MIDDLE,
	ATK_HEAVY,
	ATK_TO_AIR,
	ATK_END
};

typedef struct tagDamageParam
{
	_int iDamage;
	_float4 vHitFrom; // 공격자의 위치
	_float4 vHitPosition; // 공격 히트 위치
	_float4 vHitNormal; // 공격 히트 위치의 노멀
	_float4 vSlashVector; // 검 공격의 베는 방향(vHitPosition 기준)
	ESASType eAttackSAS = ESASType::SAS_END; // 공격 타입(몬스터는 SAS_END 고정)
	EDeBuffType eDeBuff = EDeBuffType::DEBUFF_END; // 공격에 디버프 포함 여부
	EAttackType eAttackType = EAttackType::ATK_LIGHT;
	CComponent* pContactComponent = nullptr;
	class CScarletCharacter* pCauser = nullptr; // 공격자 포인터
} DAMAGE_PARAM;

enum
{
	MOVE_AXIS_CNT = static_cast<_uint>(EMoveAxis::AXIS_END),
	BASE_AXIS_CNT = static_cast<_uint>(EBaseAxis::AXIS_END),
	TURN_CNT = static_cast<_uint>(EBaseTurn::TURN_END),
	SAS_CNT = static_cast<_uint>(ESASType::SAS_END),
	DEBUFF_CNT = static_cast<_uint>(EDeBuffType::DEBUFF_END),
	ATK_TYPE_CNT = static_cast<_uint>(EAttackType::ATK_END),
};

class CClientUtils
{
public:
	static EMoveAxis MoveAxisToEnum(_float3 vMoveAxis);
	static EBaseAxis MoveAxisToBaseEnum(_float3 vMoveAxis);
	static const string& AxisEnumToStr(EMoveAxis eAxis);
	static EBaseAxis GetDamageFromAxis(CTransform* pTransform, _fvector vFrom);
	static EBaseTurn TurnDeltaToEnum(_float fTurnDelta);

public:
	static const _tchar* const s_DebugLayer;
};

END
