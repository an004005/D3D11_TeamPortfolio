#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CLockon_FindUI final : public CUI
{
private:
	CLockon_FindUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLockon_FindUI(const CLockon_FindUI& rhs);

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
	void	Set_Find() {	// 한 번만 호출!
		m_bFind = true;
		m_bSizeChange = false;
		m_fSizeX = m_fMaxSize;
		m_fSizeY = m_fMaxSize;
		m_tParams.Float4s[0] = { 1.0f, 1.0f, 1.0f, 1.0f };
	}

private:
	void	FirstDiscovery(const _double TimeDelta);

private:
	_bool	m_bFind = { false };
	_bool	m_bSizeChange = { false };

	_float	m_fMaxSize = { 0.0f };
	_float	m_fLastSize = { 0.0f };

public:
	static CLockon_FindUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END