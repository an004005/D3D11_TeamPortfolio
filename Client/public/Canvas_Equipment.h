#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_Equipment : public CCanvas
{
protected:
	CCanvas_Equipment(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_Equipment(const CCanvas_Equipment& rhs);

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
	void	ChildUIPick();

private:
	_int	m_iItmeInfoPickCount = { 0 };
	_int	m_iItmeWindowPickCount = { 0 };

	_float2	m_Posssss = { 0.0f, 0.0f };

public:
	static CCanvas_Equipment* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END