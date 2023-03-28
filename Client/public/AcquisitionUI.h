#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CAcquisitionUI final : public CUI
{
private:
	CAcquisitionUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAcquisitionUI(const CAcquisitionUI& rhs);

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
	_bool	Get_Dead() {
		return m_bDead;
	}

private:
	wstring	m_wsText = L"";
	_bool	m_bDead = { false };
	_double	m_dDead_TimeAcc = { 0.0 };

public:
	static CAcquisitionUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END