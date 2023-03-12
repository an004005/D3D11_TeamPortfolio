#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CBoss_ShildUI final : public CUI
{
private:
	CBoss_ShildUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_ShildUI(const CBoss_ShildUI& rhs);

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
	void	Set_Shild(const _float & fShild) {
		m_tParams.Floats[0] = fShild;
	}

public:
	static CBoss_ShildUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END