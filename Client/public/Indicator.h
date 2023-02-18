#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CCollider;
END

BEGIN(Client)

class CIndicator : public CGameObject
{
private:
	CIndicator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CIndicator(const CIndicator& rhs);
	virtual ~CIndicator() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void SetPosition(_float3 vPos);


private:
	CRenderer*				m_pRendererCom = nullptr;
	CCollider*				m_pCollider = nullptr;

	

public:
	static CIndicator* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END