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

public:
	static CLastCheckUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END