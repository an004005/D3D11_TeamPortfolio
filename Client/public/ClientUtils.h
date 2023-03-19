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
	SAS_FIRE, // ��
	SAS_PENETRATE, // ����
	SAS_HARDBODY, // ����ȭ
	SAS_TELEPORT, // �ڷ���Ʈ
	SAS_ELETRIC, // ����
	SAS_SUPERSPEED, // �ʰ��
	SAS_COPY, // ����
	SAS_INVISIBLE, // ����ȭ
	SAS_GRAVIKENISIS, // ���� 
	SAS_NOT, 
	SAS_END
};

enum class EDeBuffType
{
	DEBUFF_FIRE, // ȭ��
	DEBUFF_OIL, // �⸧
	DEBUFF_THUNDER, // ����
	DEBUFF_WATER, // ��
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
	_float4 vHitFrom; // �������� ��ġ
	_float4 vHitPosition; // ���� ��Ʈ ��ġ
	_float4 vHitNormal; // ���� ��Ʈ ��ġ�� ���
	_float4 vSlashVector; // �� ������ ���� ����(vHitPosition ����)
	ESASType eAttackSAS = ESASType::SAS_END; // ���� Ÿ��(���ʹ� SAS_END ����)
	EDeBuffType eDeBuff = EDeBuffType::DEBUFF_END; // ���ݿ� ����� ���� ����
	EAttackType eAttackType = EAttackType::ATK_LIGHT;
	CComponent* pContactComponent = nullptr;
	class CScarletCharacter* pCauser = nullptr; // ������ ������
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
