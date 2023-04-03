#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CSA_RotationUI final : public CUI
{
private:
	CSA_RotationUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSA_RotationUI(const CSA_RotationUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;

	virtual void	Imgui_RenderProperty() override;

private:
	_float m_fAngle = 60.f;
	_float m_fRadius = 100.f;

	_float m_fSpeed = { 0.0f };
	_float	m_fAngle_Add = { 0.0f };

public:
	static CSA_RotationUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END