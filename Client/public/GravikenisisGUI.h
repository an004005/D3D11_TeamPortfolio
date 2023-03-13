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
	virtual void    Tick(_double TimeDelta) override;

	virtual void   Imgui_RenderProperty() override;

public:
	void SetfRatio(const _float & fRatio);

private:
	class CEffectGroup* m_pGroup = nullptr;

public:
	static CGravikenisisGUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END