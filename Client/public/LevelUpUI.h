#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Timeline.h"

BEGIN(Engine)
class CFSMComponent;
END

BEGIN(Client)

class CLevelUpUI final : public CUI
{
private:
	CLevelUpUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLevelUpUI(const CLevelUpUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;

public:
	_bool	Get_LevelUpBack() {
		return m_bLevelUpBack;
	}

	void	Set_LevelUp(const _uint iLevel) {
		m_iLevel = iLevel - 1;
		m_bVisible = true;
		m_bLevelUp = true;
	}

	void	Set_LevelUpBack() {
		m_bVisible = true;
		m_bLevelUpBack = true;
	}

	void	Set_LevelUpBackGround() {
		m_bVisible = true;
		m_bLevelUpBackGround = true;
	}

private:
	void	LevelUp_Initialize();

	void	LevelUpBack_Tick(const _double & TimeDelta);
	void	LevelUpBackGround_Tick(const _double & TimeDelta);

private:
	CFSMComponent*	m_pLevelUpFSM = { nullptr };
	CSimpleTimeline m_Timeline;

	_bool	m_bLevelUp = { false };
	_bool	m_bLevelUpBack = { false };
	_bool	m_bLevelUpBackGround = { false };

	_uint	m_iLevel = { 0 };
	_bool	m_bAlpha = { false };
	_double m_dLevelBakcGround_TimeAcc = { 0.0 };

public:
	static CLevelUpUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END