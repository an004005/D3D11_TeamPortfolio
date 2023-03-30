#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CMain_SkillIconUI final : public CUI
{
private:
	CMain_SkillIconUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMain_SkillIconUI(const CMain_SkillIconUI& rhs);

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
	void	Set_Icon(const _float & fIcon);

public:
	static CMain_SkillIconUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END