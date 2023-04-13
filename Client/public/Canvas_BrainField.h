#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_BrainField : public CCanvas
{
protected:
	CCanvas_BrainField(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_BrainField(const CCanvas_BrainField& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;
	
	class CCanvas_BrainFieldMove* m_pCanvas_BrainFieldMove = { nullptr };
	_float2 mm = { 0.0f, 0.0f };

public:
	static CCanvas_BrainField* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END