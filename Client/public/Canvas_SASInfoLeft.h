#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_SASInfoLeft : public CCanvas
{
protected:
	CCanvas_SASInfoLeft(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_SASInfoLeft(const CCanvas_SASInfoLeft& rhs);

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
	void	Set_Render();

private:
	_float2 m_vFontPos = { 0.0f, 0.0f };

public:
	static CCanvas_SASInfoLeft* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END