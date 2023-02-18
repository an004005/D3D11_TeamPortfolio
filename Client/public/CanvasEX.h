#pragma once
#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvasEX : public CCanvas
{
protected:
	CCanvasEX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvasEX(const CCanvasEX& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Imgui_RenderProperty() override;
	virtual void SaveToJson(Json& json) override;

private:
	_bool m_bFullSize = true;

public:
	DECLARE_CREATE(CCanvasEX)
	DECLARE_CLONE(CCanvasEX)
	virtual void Free() override;
};

END