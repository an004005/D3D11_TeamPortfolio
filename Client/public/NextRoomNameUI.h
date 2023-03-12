#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CNextRoomNameUI final : public CUI
{
private:
	CNextRoomNameUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNextRoomNameUI(const CNextRoomNameUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void   Tick(_double TimeDelta) override;
	virtual void   Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void   Imgui_RenderProperty() override;
	virtual void   SaveToJson(Json& json) override;
	virtual void   LoadFromJson(const Json& json) override;

public:
	void	Set_NextRoolName(const _tchar * pNextRoomName) {
		m_bVisible = true;
		m_pNextRoomName = pNextRoomName;
	}

private:
	const _tchar * m_pNextRoomName = {};

public:
	static CNextRoomNameUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END