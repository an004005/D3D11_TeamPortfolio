#pragma once
#include "Client_Defines.h"
#include "Bullet.h"

BEGIN(Engine)
class CGameInstance;
class CRigidBody;
class CFSMComponent;
END

BEGIN(Client)

class CWaterBall : public CBullet
{
private:
	CWaterBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWaterBall(const CWaterBall& rhs);
	virtual ~CWaterBall() = default;

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
	void Set_ShootDir(_vector vDest) { m_vDir = XMVector3Normalize(vDest); }

private:
	CFSMComponent*	m_pFSMCom = nullptr;

private:
	_vector			m_vDir = { 0.f, 0.f, 0.f, 0.f };

public:
	static CWaterBall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END