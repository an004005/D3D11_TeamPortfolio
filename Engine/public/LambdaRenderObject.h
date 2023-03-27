#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CLambdaRenderObject : public CGameObject
{
private:
	CLambdaRenderObject(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CLambdaRenderObject(const CLambdaRenderObject& rhs);
	virtual ~CLambdaRenderObject() = default;

public:
	virtual HRESULT Render() override;
	void SetRenderFunction(const std::function<void()>& RenderFunction);

private:
	std::function<void()> m_RenderFunction = nullptr;

public:
	static CLambdaRenderObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END