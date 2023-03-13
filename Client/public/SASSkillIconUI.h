#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CFSMComponent;
END

BEGIN(Client)

class CSASSkillIconUI final : public CUI
{
private:
	CSASSkillIconUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSASSkillIconUI(const CSASSkillIconUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;

public:
	void	Set_IconType(const ESASType & eESASType, const _bool bUsable);

public:
	static CSASSkillIconUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END