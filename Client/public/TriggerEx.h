#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRigidBody;
END

BEGIN(Client)

class CTriggerEx abstract : public CGameObject
{
protected:
	CTriggerEx(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTriggerEx(const CTriggerEx& rhs);
	virtual ~CTriggerEx() = default;

public:
	HRESULT Initialize(void* pArg) override;
	void Tick(_double TimeDelta) override;

public:
	// 트리거된 오브젝트들 중 내가 원하는 오브젝트인지 확인하는 함수
	virtual _bool CheckTriggeredObject(CGameObject* pObject) { return true; }

	// 트리거된 오브젝트가 들어오면 실행되는 함수
	virtual void TriggerInEvent(CGameObject* pObject) = 0;

	// 트리거된 오브젝트가 나가면 실행되는 함수
	virtual void TriggerOutEvent(CGameObject* pObject){}


private:
	CRigidBody*			m_pRigidBodyCom = nullptr;

	// 트리거 내에 들어온 오브젝트들
	set<CGameObject*> m_TriggeredObjects;


public:
	void Free() override;
};

END