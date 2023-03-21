#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_ItemWindow : public CCanvas
{
protected:
	CCanvas_ItemWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_ItemWindow(const CCanvas_ItemWindow& rhs);

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
	void	Set_ItemWindow(const _float2 & fItemPos, const _float2 & fIconPos, const _int iIndex);

private:
	_int		m_iItemInfoIndex = { 0 };
	_float2		m_vPosssss = { 0.0f, 0.0f };

public:
	static CCanvas_ItemWindow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END