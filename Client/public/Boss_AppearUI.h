#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Timeline.h"

BEGIN(Client)

class CBoss_AppearUI final : public CUI
{
private:
	CBoss_AppearUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_AppearUI(const CBoss_AppearUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;

public:
	_int	Get_TextureNum() {
		return m_iTexturenum;
	}

	void	Set_Appear() {
		m_bVisible = true;
		m_bAppear = true;
	}

private:
	CSimpleTimeline m_Timeline;
	
	_int	m_iTexturenum = { 0 };
	_double	m_dNextTexture_TimeAcc = { 0.0 };

	_bool	m_bAppear = { false };

public:
	static CBoss_AppearUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END