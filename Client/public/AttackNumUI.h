#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CAttackNumUI final : public CGameObject
{
protected:
	CAttackNumUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAttackNumUI(const CAttackNumUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_double TimeDelta) override;

	virtual void   Imgui_RenderProperty() override;

public:
	void Set_AttackNum(const _uint iFour, const _uint iThree, const _uint iTwo, const _uint iOne);

private:
	class CEffectGroup* m_pGroup = nullptr;

public:
	static CAttackNumUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END