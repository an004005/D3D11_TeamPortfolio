#pragma once
#include "Client_Defines.h"
#include "Component.h"
#include "TimerHelper.h"

BEGIN(Engine)
class CModel;
class CRenderer;
END

BEGIN(Client)

typedef struct tagGhostTrailInfo
{
	CModel* pPoseModel = nullptr;
	_float4x4 WorldMatrix;
	_float fLife = 0.f;
}GHOST_TRAIL_INFO;

class CSuperSpeedTrail : public CComponent
{
private:
	CSuperSpeedTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSuperSpeedTrail(const CSuperSpeedTrail& rhs);
	virtual ~CSuperSpeedTrail() = default;

public:
	virtual void BeginTick() override;
	void Tick(_double TimeDelta);

public:
	void SetOwnerModel(CModel* pModel);
	void SetTrailCoolTime(_float fCoolTime) { m_fTrailCoolTime = fCoolTime; }
	void SetMinMoveDist(_float fMinDist) { m_fMinMoveDistance = fMinDist; }
	void SetTrailLife(_float fLife) { m_fTrailLife = fLife; }

	void SetActive(_bool bActive);

private:
	CModel* m_pOwnerModel = nullptr;

	CCoolTimeHelper m_TrailGenerateTimer;
	_float m_fTrailCoolTime = 0.15f;
	_float m_fMinMoveDistance = 0.25f;
	_float m_fTrailLife = 1.f;
	_float4 m_vPrePos;

	list<GHOST_TRAIL_INFO> m_pGhostTrails;
	_bool m_bActive = false;

public:
	static CSuperSpeedTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END