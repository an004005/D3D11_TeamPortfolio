#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CMain_SkillNameUI final : public CUI
{
private:
	CMain_SkillNameUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMain_SkillNameUI(const CMain_SkillNameUI& rhs);

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
	static CMain_SkillNameUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END