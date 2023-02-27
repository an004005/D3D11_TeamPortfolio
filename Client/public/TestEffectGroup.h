#pragma once
#include "Client_Defines.h"
#include "EffectGroup.h"

BEGIN(Client)
class CTestEffectGroup :	public CEffectGroup
{
private:
	CTestEffectGroup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTestEffectGroup(const CEffectGroup& rhs);
	virtual ~CTestEffectGroup() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;

	virtual void BeginTick() override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Tick(_double TimeDelta) override;

	virtual void Imgui_RenderProperty() override;

public:
	void Tick_CenterScale(_float fValue);
	void Tick_IntroDissolve(_float fValue);
	void Tick_OutroTime(_float fValue);

private:
	class CEffectSystem* m_pCenter = nullptr;
	class CEffectSystem* m_pIntro = nullptr;
	class CEffectSystem* m_pOutro = nullptr;

public:
	static CTestEffectGroup*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};



END
