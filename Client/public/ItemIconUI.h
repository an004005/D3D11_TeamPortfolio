#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CItemIconUI final : public CUI
{
private:
	CItemIconUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItemIconUI(const CItemIconUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;

	virtual void	Imgui_RenderProperty() override;

public:
	void	Set_IconIndex(const _float2 & fIconIndex);

public:
	static CItemIconUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END