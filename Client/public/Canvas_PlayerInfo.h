#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_PlayerInfo : public CCanvas
{
protected:
	CCanvas_PlayerInfo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_PlayerInfo(const CCanvas_PlayerInfo& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;

public:
	void	Set_Type(const _uint iLevel);
	_float2 mm = { 0.0f, 0.0f };

public:
	static CCanvas_PlayerInfo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END