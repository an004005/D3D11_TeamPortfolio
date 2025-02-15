#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CItem_NameUI final : public CUI
{
private:
	CItem_NameUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_NameUI(const CItem_NameUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	BeginTick() override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;

public:
	void	Set_ItemName(const _float & fIndex);

public:
	static CItem_NameUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END