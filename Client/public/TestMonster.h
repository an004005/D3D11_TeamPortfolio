

#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CFSMComponent;
END

BEGIN(Client)

class CTestMonster : public CGameObject
{
private:
	CTestMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTestMonster(const CTestMonster& rhs);
	virtual ~CTestMonster() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CFSMComponent*			m_pFSM = nullptr;

public:
	_bool IsIdle() const { return m_bIdle; }
	_bool IsWalk() const { return m_bWalk; }
	_bool IsAttack() const { return m_bAttack; }
	_bool IsRush() const { return m_bRush; }
	_bool IsDead() const { return m_bDead; }

private:
	// TimeDelta 누적하여 초 단위 제어를 위해 사용
	_float	m_fTimeAcc = 0.f;
	_uint	m_iLRWalkCNT = 0;

	// Rush -> 실질적 Move Trigger
	_uint	m_iRushStart = 0;
	
	// 현재 내 위치를 찾아야 할 때 사용되었음
	_float3	m_fMyState = { 0.f, 0.f, 0.f };

	// 좀 더 깔끔하게 사용처를 나눠서 사용해 볼 것
	// 스테이트 상태
	_bool m_bIdle = false;
	_bool m_bWalk = false;
	_bool m_bAttack = false;
	_bool m_bRush = false;
	_bool m_bDead = false;

	// 스테이트 변환시 사용되었음
	_bool m_bWtoD = false;
	_bool m_bRtoW = false;
	_bool m_bWtoI = false;

	// Tick 내에서 애니메이션 변화(ex : start -> loop -> end)에 사용하였음.
	_bool m_bCondition = false;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CTestMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END
