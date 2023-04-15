#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CMonsterShildUI final : public CGameObject
{
protected:
	CMonsterShildUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterShildUI(const CMonsterShildUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void    Tick(_double TimeDelta) override;

	virtual void   Imgui_RenderProperty() override;

public:
	void Set_MonsterInfo(const _int iLevel, const EEnemyName eName);
	void SetShild(const _float & fHP, const _float & fShild);

	void	Broken_Tick(const _double & TimeDelta);		// UITODO : 크러시 게이지가 다 깍이면 호출해 주기!!

	void SetPivotBoneName(const string& BoneName) { m_strPivotBoneName = BoneName; }
	void SetPivot(_float4x4 Pivot) { m_Pivot = Pivot; }

private:
	void HpBack_Tick(const _double & TimeDelta);

private:
	class CEffectGroup* m_pGroup = nullptr;;
	class CEffectGroup* m_pMonsterName = nullptr;
	_float m_fHpBack = { 1.0f };
	_float m_fCurrentHpBack = { 1.0f };

	EEnemyName		m_eMonsterName = EEnemyName::ENEMY_NAME_END;
	_float					m_fMonsterLevel = { 0.0f };
	_float					m_fNameIndex = { 0.0f };

	_float					m_fShild = { 0.0f };
	_bool					m_bBroken = { false };
	_double				m_dBroken_TimeAcc = { 0.0 };

	string m_strPivotBoneName = "Target";
	_float4x4 m_Pivot = XMMatrixTranslation(0.f, 0.5f, 0.f);

public:
	static CMonsterShildUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END