#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Canvas_SASSkill.h"

BEGIN(Client)

class CSASSkillFullCircleUI final : public CUI
{
private:
	CSASSkillFullCircleUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSASSkillFullCircleUI(const CSASSkillFullCircleUI& rhs);

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

private:
	CCanvas_SASSkill* m_pCanvas = { nullptr };

	_bool m_bGrow = { false };
	_float m_fSpeed = { 0.0f };
	_float2 m_vOriginSize = { 0.0f, 0.0f };
	_float2 m_vCurrentSize = { 0.0f, 0.0f };

public:
	static CSASSkillFullCircleUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END