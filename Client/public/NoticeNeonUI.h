#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CNoticeNeonUI final : public CGameObject
{
protected:
	CNoticeNeonUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNoticeNeonUI(const CNoticeNeonUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	BeginTick();
	virtual void    Tick(_double TimeDelta) override;

	virtual void    Imgui_RenderProperty() override;
	virtual void	LoadFromJson(const Json& json) override;

public:
	_bool	doKill() {
		return m_dKill;
	}

private:
	void	Dead_Tick(const _double & TimeDelta);

private:
	class CEffectGroup* m_pNoticeNeonGroup = nullptr;

	string m_strNoticeNeon = { "" };

	_double	m_dDead_TimeAcc = { 0.0 };
	_bool	m_dKill = { false };

public:
	static CNoticeNeonUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END