#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CMonsterLockonUI final : public CGameObject
{
protected:
	CMonsterLockonUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterLockonUI(const CMonsterLockonUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    BeginTick() override;
	virtual void    Tick(_double TimeDelta) override;
	virtual void   Imgui_RenderProperty() override;

private:
	class CEffectGroup* m_pTargetGroup = nullptr;
	class CEffectGroup* m_pTargetRhombusGroup = nullptr;

public:
	static CMonsterLockonUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END