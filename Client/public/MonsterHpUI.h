#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CMonsterHpUI final : public CGameObject
{
protected:
	CMonsterHpUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterHpUI(const CMonsterHpUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	BeginTick();
	virtual void    Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void   Imgui_RenderProperty() override;

public:
	void Set_MonsterInfo(const _int iLevel, const EEnemyName eName);

	void Set_HpRatio(_float fHpRatio);
private:
	void HpBack_Tick(const _double & TimeDelta);

private:
	class CEffectGroup* m_pGroup = nullptr;
	class CEffectGroup* m_pMonsterName = nullptr;

	_float		m_fHpBack = { 1.0f };
	_float		m_fCurrentHpBack = { 1.0f };

	_int		m_iMonsterLevel = { 0 };
	EEnemyName		m_eMonsterName = EEnemyName::ENEMY_NAME_END;

public:
	static CMonsterHpUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END