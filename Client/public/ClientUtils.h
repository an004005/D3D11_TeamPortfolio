#pragma once
#include "Client_Defines.h"

#define PLAYERTEST_LAYER_TERRAIN	TEXT("Layer_Terrain")
#define PLATERTEST_LAYER_PLAYER		TEXT("Layer_Player")
#define PLAYERTEST_LAYER_CAMERA		TEXT("Layer_Camera")
#define PLAYERTEST_LAYER_BATCH		TEXT("Layer_Batch")
#define PLAYERTEST_LAYER_MAP		TEXT("Layer_Map")
#define LAYER_KINETIC				TEXT("Layer_Kinetic")
#define PLAYERTEST_LAYER_MONSTER	TEXT("Layer_Monster")
#define PLAYERTEST_LAYER_POSTVFX	TEXT("Layer_PostVFX")
#define PLAYERTEST_LAYER_FRONTUI	TEXT("Layer_FrontUI")
#define LAYER_PLAYEREFFECT			TEXT("Layer_PlayerEffect")
#define LAYER_MAPKINETICOBJECT		TEXT("Layer_MapKineticObject")
#define LAYER_AI					TEXT("Layer_AI")
#define LAYER_MAP_DECO				TEXT("Layer_MapDecorate")



#define LAYER_TRIGGER				TEXT("Layer_Trigger")

BEGIN(Engine)
class CTransform;
class CComponent;
class CRigidBody;
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

enum class EKineticAttackType
{
	KINETIC_ATTACK_DEFAULT,
	KINETIC_ATTACK_TRAIN,
	
	KINETIC_ATTACK_END
};

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

enum class ESimpleAxis
{
	NORTH,//look
	SOUTH,//back
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
	ATK_SPECIAL_LOOP,
	ATK_SPECIAL_END,
	ATK_TO_AIR,
	ATK_DOWN,
	ATK_END
};

enum class EEnemyName
{ 
	EM0110, // 말 (오거 사바트) 0.0f
	EM0200, // 플라워레그 (바스 포즈) 1.0f
	EM0210, // 투명 (소스 포즈) 2.0f
	EM0220, // 베이스 포스 3.0f
	EM0320, // 보스1 (경건 페리) 4.0f
	EM0400, // 버디러미 (버디 러미) 5.0f
	EM0650, // 스쿠미풀 (바일 풀) 6.0f
	EM0700, // 초파리 (방도 팡뒤) 7.0f
	EM0750, // 파리 (스커미 팡뒤) 8.0f
	EM0800, // 브론존 (브론 욘) 10.0f
	EM0900, // 문어 (바리스타 산타) 9.0f
	EM1100, //  물 (와이너리 치너리) 12.0f
	EM1200, //  나오미 (나오미 랜들) 11.0f
	EM8200, // 중대장 (카렌 트래버스) 13.0f
	ENEMY_NAME_END
};

typedef struct tagDamageParam
{
	_uint iDamage;
	_float4 vHitFrom; // 공격자의 위치
	_float4 vHitPosition; // 공격 히트 위치
	_float4 vHitNormal; // 공격 히트 위치의 노멀
	_float4 vSlashVector; // 검 공격의 베는 방향(vHitPosition 기준)
	EKineticAttackType eKineticAtkType = EKineticAttackType::KINETIC_ATTACK_END; // 충돌한 염력체 타입
	ESASType eAttackSAS = ESASType::SAS_END; // 공격 타입(몬스터는 SAS_END 고정)
	EDeBuffType eDeBuff = EDeBuffType::DEBUFF_END; // 공격에 디버프 포함 여부
	EAttackType eAttackType = EAttackType::ATK_LIGHT;
	CComponent* pContactComponent = nullptr;
	class CScarletCharacter* pCauser = nullptr; // 공격자 포인터
} DAMAGE_PARAM;

typedef struct tagEnemyStat
{
	_int iMaxHP = 100;
	_int iMaxCrushGage = 100;
	_bool bHasCrushGage = false;
	_int iAtkDamage = 10;
	_uint iLevel = 1;
} ENEMY_STAT;

enum
{
	MOVE_AXIS_CNT = static_cast<_uint>(EMoveAxis::AXIS_END),
	BASE_AXIS_CNT = static_cast<_uint>(EBaseAxis::AXIS_END),
	TURN_CNT = static_cast<_uint>(EBaseTurn::TURN_END),
	SAS_CNT = static_cast<_uint>(ESASType::SAS_END),
	DEBUFF_CNT = static_cast<_uint>(EDeBuffType::DEBUFF_END),
	ATK_TYPE_CNT = static_cast<_uint>(EAttackType::ATK_END),
	SIMPLE_AXIS_CNT = static_cast<_uint>(ESimpleAxis::AXIS_END),
	ENEMY_CNT = static_cast<_uint>(EEnemyName::ENEMY_NAME_END)
};

class CClientUtils
{
public:
	static EMoveAxis MoveAxisToEnum(_float3 vMoveAxis);
	static EBaseAxis MoveAxisToBaseEnum(_float3 vMoveAxis);
	static const string& AxisEnumToStr(EMoveAxis eAxis);
	static EBaseAxis GetDamageFromAxis(CTransform* pTransform, _fvector vFrom, ESimpleAxis* pSimpleAxis = nullptr);
	static EBaseTurn TurnDeltaToEnum(_float fTurnDelta);
	static string GetEnemyProtoTag(EEnemyName eName);
	static _float4 GetDirFromAxis(EBaseAxis eAxis);

public:
	static const _tchar* const s_DebugLayer;
};

END
