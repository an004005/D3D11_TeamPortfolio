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
	void SetPivotMatrix(_float4x4 pivotMatrix) {
		m_PivotMatrix = pivotMatrix;
	}
private:
	void HpBack_Tick(const _double & TimeDelta);

private:
	class CEffectGroup* m_pGroup = nullptr;;
	class CEffectGroup* m_pMonsterName = nullptr;
	_float m_fHpBack = { 1.0f };
	_float m_fCurrentHpBack = { 1.0f };
	_float4x4	m_PivotMatrix = XMMatrixIdentity();

	_int		m_iMonsterLevel = { 0 };
	EEnemyName		m_eMonsterName = EEnemyName::ENEMY_NAME_END;

public:
	static CMonsterShildUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END