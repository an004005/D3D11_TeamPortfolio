#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_WorldMap : public CCanvas
{
protected:
	CCanvas_WorldMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_WorldMap(const CCanvas_WorldMap& rhs);

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
	void	KeyInput();

private:
	class CCanvas_MapBar* m_pCanvasMapBar = { nullptr };
	_bool	m_bMapUI = { false };

public:
	static CCanvas_WorldMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END