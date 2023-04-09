#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CSASSkillFullCircleUI final : public CUI
{
private:
	CSASSkillFullCircleUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSASSkillFullCircleUI(const CSASSkillFullCircleUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;

public:
	void	Set_Grow() {
		m_bGrow = true;
	}

	void	Set_ChangeX(const _bool	bX) {
		m_bChangeX = bX;
	}

private:
	void	Grow_Tick(const _double & TimeDelta);
	void	Object_Tick();

private:
	_bool m_bGrow = { false };
	_float m_fSpeed = { 0.0f };
	_float2 m_vOriginSize = { 0.0f, 0.0f };
	_float2 m_vCurrentSize = { 0.0f, 0.0f };

	_bool m_bChangeX = { false };
	_uint iObjectNum = { 0 };

public:
	static CSASSkillFullCircleUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END