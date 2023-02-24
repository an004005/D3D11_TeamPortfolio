#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)

END

// 플레이어 카메라는 CamSpot을 따라다닌다

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
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

public:
	void	Sync_Target(_fvector TargetPos, _fvector TargetLook, _float CamHeight, _double TimeDelta);

private:
	HRESULT SetUp_Components();

public:
	static CCamera_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END