#pragma once
#include "Component.h"
#include "Client_Defines.h"

BEGIN(Client)
class CPsychokinesis : public CComponent
{
private:
	CPsychokinesis(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPsychokinesis(const CPsychokinesis& rhs);
	virtual ~CPsychokinesis() = default;

	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
	HRESULT Render() override;
	void Imgui_RenderProperty() override;

public:
	static CPsychokinesis* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent* Clone(void* pArg = nullptr) override;
	void Free() override;

};


END