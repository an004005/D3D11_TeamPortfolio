#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CGravikenisisGUI final : public CGameObject
{
protected:
	CGravikenisisGUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGravikenisisGUI(const CGravikenisisGUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void			BeginTick() override;
	virtual void			Tick(_double TimeDelta) override;

private:
	class CEffectGroup* m_pKenisis = { nullptr };
	class CEffectGroup* m_pBanKenisis = { nullptr };
	class CEffectGroup* m_pAppealCircle = { nullptr };

	_bool	m_bOnG = { false };
	_double	m_dReset_TimeAcc = { 0.0 };

public:
	static CGravikenisisGUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END