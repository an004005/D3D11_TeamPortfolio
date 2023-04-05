#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CDistanceUI final : public CUI
{
private:
	CDistanceUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDistanceUI(const CDistanceUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	LoadFromJson(const Json& json) override;

private:
	class CPlayer* m_pPlayer = { nullptr };

	_float	m_fDistance = { 0.0f };
	_float4 m_vArrivalPoint = { 0.0f, 0.0f, 0.0f, 1.0f };

public:
	static CDistanceUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END