#pragma once
#include "Client_Defines.h"
#include "Camera.h"
#include "Timeline.h"

BEGIN(Client)

class CFPSCamera : public CCamera
{
private:
	CFPSCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFPSCamera(const CFPSCamera& rhs);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	void SetWorldMatrix(_fmatrix WorldMatrix);
	_matrix GetOffsetMatrix() const { return XMLoadFloat4x4(&m_OffsetMatrix); }
	void SetOffsetMatrix(_float4x4 OffsetMatrix) { m_OffsetMatrix = OffsetMatrix; }

	void StartDeathCamMove(_float4 vLookAt);
	void SetRespawn();
	void Tick_DeathCamMove(_float fValue);

	void PlayShake(_float fDuration, _float fMagnitude);

private:
	_float4x4 m_OffsetMatrix = _float4x4::Identity;

	CTimeline m_DeathTimeline;
	_float4 m_vDeathCamMoveStartPos;
	_float4 m_vDeathCamMoveEndPos;
	_float4 m_vLookAt;

	// shake
	_float m_fShakeTime;
	_float m_fCurShakeTime;
	_float m_fMagnitude;
	// shake

public:
	static CFPSCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
};

END