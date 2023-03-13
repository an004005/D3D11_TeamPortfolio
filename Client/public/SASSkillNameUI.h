#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CSASSkillNameUI final : public CUI
{
private:
	CSASSkillNameUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSASSkillNameUI(const CSASSkillNameUI& rhs);

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
	void	Set_ColleagueName(const ESASType & eESASType, const _bool bUsable);
	void	Set_SuperPower(const ESASType & eESASType, const _bool bUsable);

public:
	static CSASSkillNameUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END