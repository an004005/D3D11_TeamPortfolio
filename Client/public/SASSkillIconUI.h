#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CFSMComponent;
END

BEGIN(Client)

class CSASSkillIconUI final : public CUI
{
private:
	CSASSkillIconUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSASSkillIconUI(const CSASSkillIconUI& rhs);

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
	void			SASSkillIcon_Tick();
	void			ChangeSkill_Shader();

private:
	//CCanvas_SASSkill* m_pCanvas = { nullptr };
	//CCanvas_SASSkill::OBJECTCOUNT	m_eObjectCount = CCanvas_SASSkill::ONE;

	_bool	asdfasdfasdfasdf;	// SASSkill_UIMove() -> UI를 이동시키고자 할 때 외부에서 Set 을 한다.
	_bool			m_bOneCheck = { false }; // 원래 지점과 목표지점의 좌표를 받는다.
	_bool			m_bIsDestination = { false };
	_bool			m_bIsOriginGoal = { false };
	_float2			m_vDestination = { 0.0f, 0.0f };	// 도착지점
//	CFSMComponent*	m_pFSM = nullptr;

public:
	static CSASSkillIconUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END