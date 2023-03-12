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
	virtual void    Tick(_double TimeDelta) override;

	virtual void   Imgui_RenderProperty() override;

public:
	void SetHP(_float fRatio);

private:
	void HpBack_Tick(const _double & TimeDelta);

private:
	class CEffectGroup* m_pGroup = nullptr;

	_float m_fHpBack = { 1.0f };
	_float m_fCurrentHpBack = { 1.0f };

public:
	static CMonsterHpUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END