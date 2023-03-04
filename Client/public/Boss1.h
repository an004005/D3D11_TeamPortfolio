#pragma once
#include "Monster.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CFSMComponent;
class CAnimation;
END

BEGIN(Client)

class CBoss1 : public CMonster
{
private:
	CBoss1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss1(const CBoss1& rhs);
	virtual ~CBoss1() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

	_bool IsMove() const { return m_vMoveAxis != _float3::Zero; }
	_float3 GetMoveAxis() const { return m_vMoveAxis; }
	_float GetTurnRemain() const { return m_fTurnRemain; }
	_bool IsPlayingSocket() const;

private:
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	class CBoss1_AnimationInstance* m_pASM = nullptr;
	class CBoss1_AIController*		m_pController = nullptr;

	CScarletCharacter* m_pTarget = nullptr;

	_float3 m_vMoveAxis;
	_float m_fTurnRemain = 0.f;

	_bool m_bDown = false;
	_bool m_bMiddleDown = false;


	_bool m_bJumpAttack = false;
	_float3 m_vOnJumpMoveVelocity;
	_float m_fJumpMoveTime = 0.f;
	_int m_iJitabaCnt = 0;


	CAnimation* m_pAtk_R = nullptr;
	CAnimation* m_pAtk_L = nullptr;
	CAnimation* m_pAtk_Spin = nullptr;
	CAnimation* m_pAtk_WaterBall = nullptr;
	CAnimation* m_pMiddleDown = nullptr;
	CAnimation* m_pDownStart = nullptr;
	CAnimation* m_pDown = nullptr;

	CAnimation* m_pJumpStart = nullptr;
	CAnimation* m_pJumpEnd = nullptr;
	CAnimation* m_pJumpLand = nullptr;
	CAnimation* m_pJumpJitabata = nullptr;

	

	// for test

public:
	static CBoss1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END