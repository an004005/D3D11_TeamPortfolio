#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_Tutorial : public CCanvas
{
	enum TUTORIAL { LOCKON, FIGHTINGSTYLE, SPECIALATTACK, ADDRUSHATTACK, ADDPSYCHOKINESISATTACK, STRENGTHENATTACK, TUTORIAL_END };

protected:
	CCanvas_Tutorial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_Tutorial(const CCanvas_Tutorial& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;

public:
	void	Set_Tutorial(const TUTORIAL & TUTORIAL) {
		m_arrTutorial[TUTORIAL] = true;
	}

private:
	void	Tutorial_Tick();
	void	Lockon();

	void	Check_Tick();

	void	KeyInput_Yes();
	void	KeyInput_No();

private:
	TUTORIAL	m_eTutorial = { TUTORIAL_END };
	_bool		m_arrTutorial[TUTORIAL_END] = {};

public:
	static CCanvas_Tutorial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END