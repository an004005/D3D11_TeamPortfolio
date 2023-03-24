#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CMain_OnMouseColorUI final : public CUI
{
private:
	CMain_OnMouseColorUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMain_OnMouseColorUI(const CMain_OnMouseColorUI& rhs);

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
	void	Set_IconColor(const _float & fColor);

public:
	static CMain_OnMouseColorUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END