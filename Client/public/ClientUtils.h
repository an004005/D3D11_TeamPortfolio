#pragma once
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
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

typedef struct tagDamageParam
{
	_int iDamage;
	_float3 vFrom;
	_bool bHead = false;
	_bool bFullBlinded = false;
	class CPlayer* pCauser = nullptr;
	_float3 vHitPosition;
} DAMAGE_PARAM;


enum
{
	MOVE_AXIS_CNT = static_cast<_uint>(EMoveAxis::AXIS_END),
	BASE_AXIS_CNT = static_cast<_uint>(EBaseAxis::AXIS_END),
};

class CClientUtils
{
public:
	static EMoveAxis MoveAxisToEnum(_float3 vMoveAxis);
	static EBaseAxis MoveAxisToBaseEnum(_float3 vMoveAxis);
	static const string& AxisEnumToStr(EMoveAxis eAxis);
	static EBaseAxis GetDamageFromAxis(CTransform* pTransform, _fvector vFrom);

public:
	static const _tchar* const s_DebugLayer;
};

END
