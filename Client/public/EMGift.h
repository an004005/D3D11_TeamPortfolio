#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CModel;
END

BEGIN(Client)

class CEMGift : public CGameObject
{
private:
	CEMGift(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEMGift(const CEMGift& rhs);
	virtual ~CEMGift() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

public:
	void Select_Gift(EDeBuffType eGiftBuff);
	void DoThrow(_fvector vTargetPos);

private:
	void SetUpComponents();

	void CollisionCheck(CTransform* pTransform, DAMAGE_PARAM mParam, _float fRad, _uint eColTypes);
	void CheckHitTarget(CGameObject* pTarget, CComponent* pHitBox, DAMAGE_PARAM mParam);
	void ClearDamagedTarget();
	_bool CheckDamagedTarget(CGameObject* pTarget);
private:
	array<CModel*, static_cast<size_t>(EDeBuffType::DEBUFF_END)> m_pModelCom ;
	CRenderer* m_pRendererCom = nullptr;

private:
	set<CGameObject*> m_DamagedTargetList;

	EDeBuffType			m_eGiftBuff = EDeBuffType::DEBUFF_END;
	_bool					m_DoThrow = false;

	_double				m_dLife = 3.0;
	_float4					m_BeforePos = _float4(0.f, 0.f, 0.f, -1.f);
	_bool					m_bHitCheck = false;

	_float3					m_vDirection;
	DAMAGE_PARAM m_eDamageParam;
public:
	static CEMGift* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END
