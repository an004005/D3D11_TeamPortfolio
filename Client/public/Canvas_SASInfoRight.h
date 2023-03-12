#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_SASInfoRight : public CCanvas
{
protected:
	CCanvas_SASInfoRight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_SASInfoRight(const CCanvas_SASInfoRight& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;

private:
	_float2 m_vFontPos = { 0.0f, 0.0f };

public:
	static CCanvas_SASInfoRight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END