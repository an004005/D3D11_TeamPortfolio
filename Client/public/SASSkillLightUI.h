#pragma once

#include "Client_Defines.h"
#include "Canvas_SASSkill.h"

BEGIN(Client)

class CSASSkillLightUI final : public CCanvas_SASSkill
{
private:
	CSASSkillLightUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSASSkillLightUI(const CSASSkillLightUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;

private:
	void			SkilInfo_Initialize();
	
	void			ChangeSkill();
	void			ChangeSkill_TickShader(const _float & fTimeDelta);

private:
	CCanvas_SASSkill::OBJECTCOUNT	m_eObjectCount = CCanvas_SASSkill::ONE;
	
	_double		m_dLight_TimeAcc = { 0.0f };

public:
	static CSASSkillLightUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas_SASSkill* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END