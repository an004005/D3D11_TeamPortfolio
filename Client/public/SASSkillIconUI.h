#pragma once

#include "Client_Defines.h"
#include "UI.h"

enum SASSKILL { ONE, TWO, THREE, FOUR, SASSKILL_END };
enum SUPERPOWERS { PSYCHOKINESIS0, PSYCHOKINESIS1, IGNITION, RESHUFFLE, CLAIRVOYANCE, TELEPORTATION, TRANSPARENCY, DISCHARGE, COPY, HIGHSPEED, SUPERPOWERS_END };

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
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;

private:
	void			Key_Input();
	void			SASSkillIcon_Tick();

private:
	SASSKILL		m_eSASSkill = SASSKILL_END;
	SASSKILL		m_ePreSASSkill = SASSKILL_END;
	SUPERPOWERS		m_eSuperPowers = SUPERPOWERS_END;

public:
	static CSASSkillIconUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END