#include "stdafx.h"
#include "ClientUtils.h"
#include "GameInstance.h"
#include "JsonLib.h"
#include "Transform.h"
#include "Protocol.pb.h"

const _tchar* const CClientUtils::s_DebugLayer = L"DebugLayer";
const _tchar* const CClientUtils::s_DebugRayPrototype = L"Prototype_DebugRay";
const _tchar* const CClientUtils::s_DebugSpherePrototype = L"Prototype_DebugSphere";


EMoveAxis CClientUtils::MoveAxisToEnum(_float3 vMoveAxis)
{
	if (vMoveAxis.z > 0.f) // north
	{
		if (vMoveAxis.x > 0.f) // east
			return EMoveAxis::NORTH_EAST;
		else if (vMoveAxis.x < 0.f)
			return EMoveAxis::NORTH_WEST;
		return EMoveAxis::NORTH;
	}
	else if (vMoveAxis.z < 0.f) // south
	{
		if (vMoveAxis.x > 0.f) // east
			return EMoveAxis::SOUTH_EAST;
		else if (vMoveAxis.x < 0.f)
			return EMoveAxis::SOUTH_WEST;
		return EMoveAxis::SOUTH;
	}
	else if (vMoveAxis.x > 0.f)
		return EMoveAxis::EAST;
	else if (vMoveAxis.x < 0.f)
		return EMoveAxis::WEST;

	return EMoveAxis::CENTER;
}

EBaseAxis CClientUtils::MoveAxisToBaseEnum(_float3 vMoveAxis)
{
	if (vMoveAxis.z > 0.f) // north
	{
		return EBaseAxis::NORTH;
	}
	else if (vMoveAxis.z < 0.f) // south
	{
		return EBaseAxis::SOUTH;
	}
	else if (vMoveAxis.x > 0.f)
		return EBaseAxis::EAST;
	else if (vMoveAxis.x < 0.f)
		return EBaseAxis::WEST;

	return EBaseAxis::AXIS_END;
}

const string& CClientUtils::AxisEnumToStr(EMoveAxis eAxis)
{
	static const array<const string, MOVE_AXIS_CNT> strs{
	"NORTH",
	"NORTH_EAST",
	"EAST",
	"SOUTH_EAST",
	"SOUTH",
	"SOUTH_WEST",
	"WEST",
	"NORTH_WEST",
	"CENTER",
	};
	return strs[static_cast<_uint>(eAxis)];
}

void CClientUtils::CloneDebugRay(_float3 vOrigin, _float3 vDir, _float fLife)
{
	Json json = Json::object();
	json["Origin"] = vOrigin;
	json["Dir"] = vDir;
	json["Life"] = fLife;

	CGameInstance::GetInstance()->Clone_GameObject(LEVEL_NOW, s_DebugLayer, s_DebugRayPrototype, &json);
}

void CClientUtils::CloneDebugSphere(_float3 vOrigin, _float fRadius, _float fLife)
{
	Json json = Json::object();
	json["Origin"] = vOrigin;
	json["Radius"] = fRadius;
	json["Life"] = fLife;

	CGameInstance::GetInstance()->Clone_GameObject(LEVEL_NOW, s_DebugLayer, s_DebugSpherePrototype, &json);
}

EBaseAxis CClientUtils::GetDamageFromAxis(CTransform* pTransform, _fvector vFrom)
{
	// vLook 기준
	// 시계방향으로 - 180
	// 반시계방향으로 + 180
	const _vector vLook = XMVector3Normalize(XMVectorSetY(pTransform->Get_State(CTransform::STATE_LOOK), 0.f));
	const _vector vPos = pTransform->Get_State(CTransform::STATE_TRANSLATION);
	const _vector vFromDir = XMVector3Normalize(XMVectorSetY(vFrom - vPos, 0.f));

	_float fDeg;
	if (XMVectorGetX(vFromDir) < 0.f)
		fDeg = -acosf(XMVectorGetX(XMVector3Dot(vFromDir, vLook)));
	else
		fDeg = acosf(XMVectorGetX(XMVector3Dot(vFromDir, vLook)));
	fDeg = XMConvertToDegrees(fDeg);

	EBaseAxis eAxis = EBaseAxis::NORTH;
	if (-45.f <= fDeg && fDeg <= 45.f)
		eAxis = EBaseAxis::NORTH;
	else if (-135.f < fDeg && fDeg < -45.f)
		eAxis = EBaseAxis::EAST;
	else if (45.f < fDeg && fDeg < 135.f)
		eAxis = EBaseAxis::WEST;
	else
		eAxis = EBaseAxis::SOUTH;

	return eAxis;
}

_float2 CClientUtils::ToClientVector2(const Protocol::Vector2& vector2)
{
	return _float2{vector2.x(), vector2.y()};
}

void CClientUtils::ToProtocolVector2(const _float2& vector2, Protocol::Vector2* out)
{
	out->set_x(vector2.x);
	out->set_y(vector2.y);
}

void CClientUtils::ToProtocolVector3(const _float4& vector4, Protocol::Vector3* out)
{
	out->set_x(vector4.x);
	out->set_y(vector4.y);
	out->set_z(vector4.z);
}

void CClientUtils::ToProtocolVector3(const _float3& vector3, Protocol::Vector3* out)
{
	out->set_x(vector3.x);
	out->set_y(vector3.y);
	out->set_z(vector3.z);
}

void CClientUtils::ToProtocolMatrix(const _float4x4& matrix, Protocol::Matrix* out)
{
	out->mutable_vright()->set_x(matrix._11);
	out->mutable_vright()->set_y(matrix._12);
	out->mutable_vright()->set_z(matrix._13);

	out->mutable_vup()->set_x(matrix._21);
	out->mutable_vup()->set_y(matrix._22);
	out->mutable_vup()->set_z(matrix._23);

	out->mutable_vlook()->set_x(matrix._31);
	out->mutable_vlook()->set_y(matrix._32);
	out->mutable_vlook()->set_z(matrix._33);

	out->mutable_vpos()->set_x(matrix._41);
	out->mutable_vpos()->set_y(matrix._42);
	out->mutable_vpos()->set_z(matrix._43);
}

_float4 CClientUtils::ToClientVector4(const Protocol::Vector3& vector3)
{
	return _float4{vector3.x(), vector3.y(), vector3.z(), 0.f};
}

_float3 CClientUtils::ToClientVector3(const Protocol::Vector3& vector3)
{
	return _float3{vector3.x(), vector3.y(), vector3.z()};
}

_float4x4 CClientUtils::ToClientMatrix(const Protocol::Matrix& matrix)
{
	_float4x4 out;
	out._11 = matrix.vright().x();
	out._12 = matrix.vright().y();
	out._13 = matrix.vright().z();

	out._21 = matrix.vup().x();
	out._22 = matrix.vup().y();
	out._23 = matrix.vup().z();

	out._31 = matrix.vlook().x();
	out._32 = matrix.vlook().y();
	out._33 = matrix.vlook().z();

	out._41 = matrix.vpos().x();
	out._42 = matrix.vpos().y();
	out._43 = matrix.vpos().z();
	return out;
}
