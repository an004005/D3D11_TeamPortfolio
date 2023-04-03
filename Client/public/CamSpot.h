#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)
class CCamera_Player;
class CController;

class CCamSpot : public CGameObject
{
public:
	enum ECamMod { MOD_SYNC, MOD_ATTACH, MOD_END };

private:
	CCamSpot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamSpot(const CCamSpot& rhs);
	virtual ~CCamSpot() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void BeginTick();
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);
	virtual void AfterPhysX() override;
	virtual void Imgui_RenderProperty() override;

private:
	void	MouseMove(_double TimeDelta);
	_float	m_fSensitivity = 0.1f;
	_float	m_fCamHeight = 0.f;

public:
	void	Switch_CamMod();	// CameraPos¿¡ Ä· ºÙÀÌ±â
	void	Reset_CamMod();
	void	SetUp_BoneMatrix(CModel* pModel, _fmatrix Transform);
	ECamMod	GetCamMod() { return m_eCamMod; }

public:
	_bool	Cam_Closer(_double TimeDelta, _float fRatio, _float fLimit = 0.25f);
	_bool	Cam_Away(_double TimeDelta, _float fRatio, _float fLimit = 1.f);

public:
	void	Random_Shaking(_float fShakePower);
	void	Axis_Shaking(_float4 vAxis, _float fShakePower);
	void	Axis_Sliding(_float4 vAxis, _float fSlidePower);

private:
	ECamMod	m_eCamMod = MOD_SYNC;
	_matrix	m_AttachMatrix = XMMatrixIdentity();
	_float	m_fLerpTime = 1.f;
	ECamMod	m_bCheckActionEnd = MOD_SYNC;
	_matrix m_CheckMatrix = XMMatrixIdentity();

public:
	CTransform*	GetTransform() { return this->m_pTransformCom; }
	
private:
	CGameObject* m_pTargetObject = nullptr;
	CGameObject* m_pPlayerCamera = nullptr;

private:
	HRESULT	Setup_Components(void);
	CController* m_pController = nullptr;

	_float2 m_vMouseMovement;

public:
	static CCamSpot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END