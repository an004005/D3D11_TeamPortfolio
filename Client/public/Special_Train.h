#pragma once
#include "SpecialObject.h"
#include "Client_Defines.h"
#include "HelperClasses.h"

BEGIN(Client)
class CParticleGroup;

class CSpecial_Train : public CSpecialObject
{
private:
	CSpecial_Train(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpecial_Train(const CSpecial_Train& rhs);
	virtual ~CSpecial_Train() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
	void BeginTick() override;
	void Tick(_double TimeDelta) override;
	void Late_Tick(_double TimeDelta) override;
	virtual void AfterPhysX();
	HRESULT Render() override;

	virtual void LoadFromJson(const Json& json) override;
	virtual void SaveToJson(Json& json) override;

	virtual void Imgui_RenderProperty() override;

public:
	void	Activate_Animation(_bool bActivate) { m_bActivate = bActivate; }
	void	Train_Set_Animation(const string& szAnimName);

	void	Train_Collision_On();
	void	Train_Collision_Off();

	void	Train_SetDeadTimer();

	void	Train_SparkEffect_Set();
	void	Train_SparkEffect_Release();

	_float4	GetActionPoint();

private:
	HRESULT	SetUp_Components(void* pArg);

private:
	_vector m_vLerpVector = XMVectorSet(0.f, 0.f, 0.f, 0.f);	// 보간안쓸라고 만든거
	_bool	m_bActivate = false;
	_float	m_fDeadTimer = 0.f;
	_bool	m_bDeadCheck = false;
	CDoOnce	m_Effect;
	CDoOnce m_Slow;

private:
	_float	m_fLerpTime = 0.2f;
	_float	m_fDuration = 0.2f;

private:
	CParticleGroup* m_pSpark[6] = { nullptr, };

public:
	static CSpecial_Train* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)override;
	void Free() override;
};

END