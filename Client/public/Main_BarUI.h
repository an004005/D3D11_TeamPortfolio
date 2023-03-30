#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CMain_BarUI final : public CUI
{
private:
	CMain_BarUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMain_BarUI(const CMain_BarUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;

	virtual void	Imgui_RenderProperty() override;

public:
	void	Set_Ber(const _float & fRatio);

public:
	static CMain_BarUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END