#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CSASSkillLightUI final : public CUI
{
private:
	CSASSkillLightUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSASSkillLightUI(const CSASSkillLightUI& rhs);

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
	void	Set_LightType(const ESASType & eESASType, const _bool bUsable);

private:
	_bool		m_bBrightness = { false };
	_float		m_fStrength = { 0.5f };

	_bool		m_bOnSkil = { false };

public:
	static CSASSkillLightUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END