#pragma once
#include "Client_Defines.h"
#include "ImguiObject.h"
#include "ScarletCharacter.h"
#include "HelperClasses.h"

BEGIN(Client)
class CAI_CH0300_AnimInstance;

class CAI_CH0300 : public CScarletCharacter
{
private:
	CAI_CH0300(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAI_CH0300(const CAI_CH0300& rhs);
	virtual ~CAI_CH0300() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void BeginTick();
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);
	virtual void AfterPhysX() override;
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth() override;

public:
	virtual void TakeDamage(DAMAGE_PARAM tDamageParams);
	virtual void Imgui_RenderProperty() override;

public:
	virtual _float4x4 GetBoneMatrix(const string& strBoneName, _bool bPivotapply = true) override;
	virtual _float4x4 GetPivotMatrix() override;

private:
	HRESULT SetUp_Components(void* pArg);
	HRESULT SetUp_Event();
	HRESULT SetUp_EffectEvent();
	HRESULT SetUp_Sound();
	HRESULT SetUp_AttackDesc();

	unordered_map<string, std::function<void()>>	m_mapCollisionEvent;

private:
	_double	m_fTimeDelta = 0.f;

private:
	CRenderer*	m_pRenderer = nullptr;
	CModel*		m_pModel = nullptr;
	CAI_CH0300_AnimInstance* m_pASM = nullptr;

private:
	CGameObject* m_pPlayer = nullptr;

private:
	wstring		m_ModelName;

private:
	void		BehaviorCheck();

private:
	void		DistanceCheck();	// 플레이어와의 거리 탐지

private:
	_float		m_fDistance_toPlayer = 0.f;
	_float		m_fDistance_toEnemy = -1.f;

public:
	_float		Get_Distance_Player() const { return m_fDistance_toPlayer; }
	_float		Get_Distance_Enemy() const { return m_fDistance_toEnemy; }

public:
	static CAI_CH0300* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END