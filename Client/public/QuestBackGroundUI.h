#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CQuestBackGroundUI final : public CUI
{
private:
	CQuestBackGroundUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQuestBackGroundUI(const CQuestBackGroundUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;

public:
	static CQuestBackGroundUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END