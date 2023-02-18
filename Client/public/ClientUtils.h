#pragma once
#include "Client_Defines.h"
#include "Struct.pb.h"

BEGIN(Engine)
class CTransform;
END

#ifdef _DEBUG
#define DEBUG_RAY(vOrigin, vDir, fLife) CClientUtils::CloneDebugRay(vOrigin, vDir, fLife);
#define DEBUG_RAY(vOrigin, vDir) CClientUtils::CloneDebugRay(vOrigin, vDir, 1.f);
#define DEBUG_SPHERE(vOrigin, fRadius, fLife) CClientUtils::CloneDebugSphere(vOrigin, fRadius, fLife);
#define DEBUG_SPHERE(vOrigin, fRadius) CClientUtils::CloneDebugSphere(vOrigin, fRadius, 1.f);
#else
#define DEBUG_RAY(vOrigin, vDir, fLife) 
#define DEBUG_RAY(vOrigin, vDir) 
#define DEBUG_SPHERE(vOrigin, fRadius, fLife) 
#define DEBUG_SPHERE(vOrigin, fRadius) 
#endif

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
	BASE_AXIS_CNT = static_cast<_uint>(EBaseAxis::AXIS_END)
};

class CClientUtils
{
public:
	static EMoveAxis MoveAxisToEnum(_float3 vMoveAxis);
	static EBaseAxis MoveAxisToBaseEnum(_float3 vMoveAxis);
	static const string& AxisEnumToStr(EMoveAxis eAxis);
	static void CloneDebugRay(_float3 vOrigin, _float3 vDir, _float fLife = 1.f);
	static void CloneDebugSphere(_float3 vOrigin, _float fRadius, _float fLife = 1.f);
	static EBaseAxis GetDamageFromAxis(CTransform* pTransform, _fvector vFrom);

	// ���� ��Ŷ
	static _float2 ToClientVector2(const Protocol::Vector2& vector2);
	static void ToProtocolVector2(const _float2& vector2, Protocol::Vector2* out);
	static _float4 ToClientVector4(const Protocol::Vector3& vector3);
	static _float3 ToClientVector3(const Protocol::Vector3& vector3);
	static void ToProtocolVector3(const _float4& vector4, Protocol::Vector3* out);
	static void ToProtocolVector3(const _float3& vector3, Protocol::Vector3* out);
	static _float4x4 ToClientMatrix(const Protocol::Matrix& matrix);
	static void ToProtocolMatrix(const _float4x4& matrix, Protocol::Matrix* out);

public:
	static const _tchar* const s_DebugLayer;
	static const _tchar* const s_DebugSpherePrototype;
	static const _tchar* const s_DebugRayPrototype;

};

END
