#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CDebugSphere : public CGameObject
{
private:
	CDebugSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDebugSphere(const CDebugSphere& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	const BoundingSphere& GetSphere() { return m_Sphere; }

private:
	_float m_fLife = 1.f;
	BoundingSphere m_Sphere;

	class CRenderer* m_pRendererCom = nullptr;

	_bool						m_isColl = false;
	PrimitiveBatch<VertexPositionColor>*				m_pBatch = nullptr;
	BasicEffect*										m_pEffect = nullptr;
	ID3D11InputLayout*									m_pInputLayout = nullptr;	
	_float4												m_vColor;

public:
	static CDebugSphere* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END