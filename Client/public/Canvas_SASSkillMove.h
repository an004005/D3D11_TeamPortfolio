#pragma once

#include "Client_Defines.h"
#include "Canvas.h"

BEGIN(Client)

class CCanvas_SASSkillMove final : public CCanvas
{
public:
	enum SKILLINDEX { ONE0, TWO0, THREE0, FOUR0, ONE1, TWO1, THREE1, FOUR1, SKILLINDEX_END };

protected:
	CCanvas_SASSkillMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_SASSkillMove(const CCanvas_SASSkillMove& rhs);

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
	void	Set_SkillInfo(const SKILLINDEX eSKILLINDEX, const ESASType eESASType);
	void	Set_IconType(const _uint iIndex, const ESASType & eESASType);

	void	Info_Tick(); 
	void	InputCtrl_Tick();
	void	InputAlt_Tick();
	void	UseSkill_Tick();

private:
	class CPlayer *	m_pPlyaer = { nullptr };
	ESASType		m_eSASType[SKILLINDEX_END] = { ESASType::SAS_END };
	
	_bool			m_bCtrl = { false };
	_bool			m_bAlt = { false };

public:
	static CCanvas_SASSkillMove* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END