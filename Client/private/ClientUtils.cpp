#include "stdafx.h"
#include "ClientUtils.h"
#include "GameInstance.h"
#include "JsonLib.h"
#include "Transform.h"

const _tchar* const CClientUtils::s_DebugLayer = L"DebugLayer";


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

EBaseAxis CClientUtils::GetDamageFromAxis(CTransform* pTransform, _fvector vFrom, ESimpleAxis* pSimpleAxis)
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

	if (pSimpleAxis != nullptr)
	{
		if (-90.f <= fDeg && fDeg <= 90.f)
			*pSimpleAxis = ESimpleAxis::NORTH;
		else
			*pSimpleAxis = ESimpleAxis::SOUTH;
	}

	return eAxis;
}

EBaseTurn CClientUtils::TurnDeltaToEnum(_float fTurnDelta)
{
	if (abs(fTurnDelta - 0.00001f) < 0.01f)
		return EBaseTurn::TURN_END;
	else if (fTurnDelta > 0.f)
		return EBaseTurn::TURN_LEFT;
	else
		return EBaseTurn::TURN_RIGHT;
}

string CClientUtils::GetEnemyProtoTag(EEnemyName eName)
{
	static array<string, ENEMY_CNT> s_EnemyNames{
	   "Monster_em110", // 말
		"Monster_em200", // 플라워레그
	   "Monster_em210", // 투명
	   "Monster_em220", // 베이스포스
	   "Monster_em320", // 보스1
	   "Monster_em400",  // 버디러미
	   "Monster_em650", // 스쿠미풀
	   "Monster_em700", // 초파리
	   "Monster_em750", // 파리
	   "Monster_em800", // 브론존
	   "Monster_em900", // 문어
	   "Monster_em1100", // 물
	   "Monster_em1200", // 나오미
	   "NotUsing", // 중대장
	};

	return s_EnemyNames[static_cast<_uint>(eName)];
}

_float4 CClientUtils::GetDirFromAxis(EBaseAxis eAxis)
{
	_float4 vOut = _float4::Zero;
	switch (eAxis)
	{
	case EBaseAxis::NORTH:
		vOut.z = 1.f;
		break;
	case EBaseAxis::EAST:
		vOut.x = 1.f;
		break;
	case EBaseAxis::SOUTH: 
		vOut.z = -1.f;
		break;
	case EBaseAxis::WEST: 
		vOut.x = -1.f;
		break;
	case EBaseAxis::AXIS_END:
		FALLTHROUGH
	default: 
		NODEFAULT;
	}
	return vOut;
}
