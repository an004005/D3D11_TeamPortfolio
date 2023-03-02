#pragma once
#include "Camera.h"
#include "Client_Defines.h"

BEGIN(Client)

class CTestCamera : public CCamera
{
private:
	CTestCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTestCamera(const CTestCamera& rhs);
	virtual ~CTestCamera() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	void AttachTo(_fvector vPos);
	void SetMouseAxis(_float2 vMouseAxis) { m_vMouseAxis = vMouseAxis; }


private:
	_float m_fBoomLength = 5.f;
	_bool m_bActive = false;
	_float2 m_vMouseAxis;

public:
	static CTestCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END