#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)

END

BEGIN(Client)

class CCamera_Dynamic final : public CCamera
{
private:
	CCamera_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Dynamic(const CCamera_Dynamic& rhs);
	virtual ~CCamera_Dynamic() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;
	void SetActive(_bool bActive) { m_bActive = bActive; }

private:
	HRESULT SetUp_Components();
	
private:
	_float		m_fMouseSpeed = 0.03f;
	_float		m_fSpeedPerSec = 10.f;
	_bool m_bActive = true;

public:
	static CCamera_Dynamic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END