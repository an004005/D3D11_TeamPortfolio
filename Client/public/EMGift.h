#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CModel;
END

BEGIN(Client)

class CEMGift : public CGameObject
{
private:
	CEMGift(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEMGift(const CEMGift& rhs);
	virtual ~CEMGift() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel* m_pModelCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;

public:
	static CEMGift* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END
