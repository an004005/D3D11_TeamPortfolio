#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CNextMapNameUI final : public CUI
{
private:
	CNextMapNameUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNextMapNameUI(const CNextMapNameUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;

public:
	void   Set_NextRoolName(const _tchar * pNextRoomName) {
		m_bVisible = true;
		m_pNextRoomName = pNextRoomName;
	}

private:
	const _tchar * m_pNextRoomName = {};

public:
	static CNextMapNameUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END