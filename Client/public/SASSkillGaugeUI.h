#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CSASSkillGaugeUI final : public CUI
{
private:
	CSASSkillGaugeUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSASSkillGaugeUI(const CSASSkillGaugeUI& rhs);

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
	void		Set_GaugeType(const ESASType & eESASType, const _float & fGauge);
	void		Set_GaugeBackType(const ESASType & eESASType, const _float & fGauge);

private:
	void			SkilInfo_Initialize();

	void			ChangeSkill();
	void			ChangeSkill_TickShader(const _float & fTimeDelta);

private:
	_float2		m_fMinMaxGauge = { 0.0f, 1.0f };	// 최소 게이지와 최대 게이지
	_float		m_fGaugeSpeed = { 0.0f };			// 게이지가 사용되는 속도
	
	_float		m_fSkill_TimeAcc = { 0.0f };

public:
	static CSASSkillGaugeUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END