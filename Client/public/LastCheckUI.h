#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CLastCheckUI final : public CUI
{
private:
	CLastCheckUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLastCheckUI(const CLastCheckUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	BeginTick() override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;

private:
	void	Open(const _double& dTImeDelta);
	void	Shut(const _double& dTImeDelta);
	_bool	Change(const _double& dTImeDelta, const _bool& bTutorial);

private:
	_bool	m_bOnTutorial = { false };
	_bool	m_bSizeGrowLess = { false };
	_bool	m_bOffTutorial = { false };
	_bool	m_bEnd = { false };

	_float	m_fAlpha = { 0.0f };
	_float	m_fStartSizeX = { 0.0f };
	_float	m_fChangeSizeX = { 0.0f };

	_bool	m_bInput = { false };

public:
	static CLastCheckUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END