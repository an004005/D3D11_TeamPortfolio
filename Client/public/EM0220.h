#pragma once
#include "Enemy.h"
#include "Controller.h"

BEGIN(Engine)
class CRigidBody;
class CMaterial;
END

BEGIN(Client)

// �ٸ� ���� :: ���̽� ����

class CEM0220 : public CEnemy
{
private:
	CEM0220(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEM0220(const CEM0220& rhs);
	virtual ~CEM0220() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void SetUpComponents(void* pArg) override;
	virtual void SetUpSound() override;
	virtual void SetUpAnimationEvent() override;
	virtual void SetUpFSM() override;

	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual void AfterPhysX() override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;
	virtual _bool IsWeak(CRigidBody* pHitPart);

	virtual void HitEffect(DAMAGE_PARAM& tDamageParams) override;
	virtual void CheckCrushGage(DAMAGE_PARAM& tDamageParams) override;
	virtual void CheckHP(DAMAGE_PARAM& tDamageParams) override;
	void	SetUp_Lantern();
public:
	//�ൿ ���� �Լ� ����
	_bool IsMove() const { return m_vMoveAxis != _float3::Zero; }
	_float3 GetMoveAxis() const { return m_vMoveAxis; }
	_float GetTurnRemain() const { return m_fTurnRemain; }
	_bool IsPlayingSocket() const;


private:
	void Play_HeavbyHitAnim();
	void Update_LanternMatrix();
	void CounterAttack(_double TimeDelta);
	void Create_Bullet();
	void HitWeakProcess(_double TimeDelta);

private:
	//�浹 ���� �Լ� ����

private:
	class CEM0220_Controller*		m_pController = nullptr;
	class CEM0220_AnimInstance*		m_pASM = nullptr;
	CMaterial* m_pWeak = nullptr;
	class CEMBrain* m_pBrain = nullptr;
	vector<class CEM0221*>	m_pLanterns;
private:
	//���� ������
	_float3 m_vMoveAxis;
	_float m_fTurnRemain = 0.f;

	_bool		m_bHitMove = false;

	// Attack
	_bool		m_bAttack = false;
	_double		m_dFallCount = 0.0;
	_uint		m_iFallIndex = 0;
	_bool	m_bWeakProcess = false;

	//��������
	_bool		m_Unbeatable = false;
	_int			m_iHitDamage = 0;

	CController::EHandleInput	m_eInput = CController::EHandleInput::HANDLE_END;

	//BC
	_double		m_BCLoopTime = 0.0;
	
public:
	static CEM0220* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

