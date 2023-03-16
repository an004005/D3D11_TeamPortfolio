#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CBoss_HpBackUI final : public CUI
{
private:
	CBoss_HpBackUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_HpBackUI(const CBoss_HpBackUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;

public:
	void	Set_BossHp(const _float & fHp);

private:
	_float	m_fHp = { 0.0f };
	_float	m_fCurrentHp = { 1.0f };

public:
	static CBoss_HpBackUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END