#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CGameInstance;
class CShader;
class CModel;
class CRenderer;
class CFSMComponent;
class CRigidBody;
END

BEGIN(Client)

class CSkMpBullet : public CGameObject
{
private:
	CSkMpBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkMpBullet(const CSkMpBullet& rhs);
	virtual ~CSkMpBullet() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

	virtual void AfterPhysX();

public:
	void					Set_ShootDir(_vector vDest) { m_vDir = XMVector3Normalize(vDest); }

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CFSMComponent*			m_pFSM = nullptr;

	CRigidBody*				m_pCollider = nullptr;

private:
	HRESULT SetUp_Components(void* pArg);

private:
	wstring					m_ModelName;

	_bool					m_bIdle = false;
	_bool					m_bShoot = false;
	_bool					m_bCollision = false;
	_bool					m_bDead = false;

	_float					m_fTimeAcc = 0.f;
	_vector					m_vDir = { 0.f, 0.f, 0.f, 0.f };

private:
	CGameObject*			m_pSkummyPool = nullptr;

public:
	static CSkMpBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END