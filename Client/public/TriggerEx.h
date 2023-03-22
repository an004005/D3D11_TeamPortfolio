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
	// Ʈ���ŵ� ������Ʈ�� �� ���� ���ϴ� ������Ʈ���� Ȯ���ϴ� �Լ�
	virtual _bool CheckTriggeredObject(CGameObject* pObject) { return true; }

	// Ʈ���ŵ� ������Ʈ�� ������ ����Ǵ� �Լ�
	virtual void TriggerInEvent(CGameObject* pObject) = 0;

	// Ʈ���ŵ� ������Ʈ�� ������ ����Ǵ� �Լ�
	virtual void TriggerOutEvent(CGameObject* pObject){}


private:
	CRigidBody*			m_pRigidBodyCom = nullptr;

	// Ʈ���� ���� ���� ������Ʈ��
	set<CGameObject*> m_TriggeredObjects;


public:
	void Free() override;
};

END