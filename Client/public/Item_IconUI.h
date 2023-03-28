#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CItem_IconUI final : public CUI
{
private:
	CItem_IconUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_IconUI(const CItem_IconUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	BeginTick() override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;

public:
	void	Set_ItemIconUse() {
		m_bUse = true;
	}
	void	Set_IconIndex(const _float2 & fIconIndex);

private:
	_bool	m_bUse = { false };
	_float2	m_fStartSize = { 0.0f, 0.0f };
	_double	m_dSizeChange_TimeAcc = { 0.0 };

public:
	static CItem_IconUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END