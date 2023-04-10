#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CGravikenisisMouseUI final : public CGameObject
{
protected:
	CGravikenisisMouseUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGravikenisisMouseUI(const CGravikenisisMouseUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void			BeginTick() override;
	virtual void		    Tick(_double TimeDelta) override;

private:
	class CEffectGroup* m_pKenisis = { nullptr };
	class CEffectGroup* m_pBanKenisis = { nullptr };
	class CEffectGroup* m_pAppealCircle = { nullptr };

public:
	static CGravikenisisMouseUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END