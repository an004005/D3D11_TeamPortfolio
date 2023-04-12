#pragma once

#include "Client_Defines.h"
#include "Camera.h"
#include "HelperClasses.h"

BEGIN(Engine)
class CControlledRigidBody;
END

// �÷��̾� ī�޶�� CamSpot�� ����ٴѴ�

BEGIN(Client)

class CCamera_Player : public CCamera
{
private:
	CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Player(const CCamera_Player& rhs);
	virtual ~CCamera_Player() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual void AfterPhysX() override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

public:
	void	Sync_Target(_fvector TargetPos, _fvector TargetLook, _float CamHeight, _double TimeDelta);
	void	Attach_Target(_fmatrix AttachMatrix);

	_vector	Get_SyncPos(_fvector TargetPos, _fvector TargetLook, _float CamHeight, _double TimeDelta);	// ���� ��ġ
	_vector	Get_SyncLook(_fvector TargetPos, _fvector TargetLook, _float CamHeight, _double TimeDelta);	// ���� ��
	
	void	Lerp_ActionPos(_fvector vPos, _fvector vLook);	// ������ ��ġ�� ķ ��ġ ����
	
	_float	Get_Magnification() { return m_fDistanceMagnification; }
	void	Set_Magnification(_float fMag) { m_fDistanceMagnification = fMag; }

private:
	HRESULT SetUp_Components();

private:
	_float	m_fCamDistance = 4.f;
	_float	m_fDistanceMagnification = 1.f;
	_float	m_fBeforeCamDistance = -1.f;

public:
	static CCamera_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END