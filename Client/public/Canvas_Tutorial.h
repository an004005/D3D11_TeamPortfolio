#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_Tutorial : public CCanvas
{
public:
	enum TUTORIAL { LOCKON, FIGHTINGSTYLE, SPECIALATTACK, ADDRUSHATTACK, ADDPSYCHOKINESISATTACK, STRENGTHENATTACK, TUTORIAL_END };
	enum TIPS { TIPS0, TIPS1, TIPS2, TIPS3, TIPS4, TIPS5, TIPS6, TIPS7, TIPS8, TIPS9, TIPS10, TIPS_END };

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
	void	Set_Tutorial(const TUTORIAL & eTUTORIAL) {
		m_eTutorial = eTUTORIAL;
		m_arrTutorial[eTUTORIAL] = true;
	}
	void	Set_Tips(const TIPS & eTIPS) {
		m_eTips = eTIPS;
	}
	void	Set_TipsDelete(const TIPS & eTIPS) {
		m_eTips = eTIPS;
		m_arrTips[eTIPS] = true;
	}

	void	Set_Success() {
		m_bSuccess = true;
	}

private:
	void	Tutorial_Tick();
	void	Tutorial(const TUTORIAL & eTUTORIAL, const _tchar * pChildTag);

	void	Check_Tick();

	void	KeyInput_Yes();
	void	KeyInput_No();

private:
	void	Tips_Tick();
	void	Tips(const TIPS & eTIPS, const _tchar * pChildTag);

private:
	void	Success_Tick(const _double & TimeDelta);

private:	// Tutorial
	TUTORIAL	m_eTutorial = { TUTORIAL_END };
	_bool		m_arrTutorial[TUTORIAL_END] = {};

	_int		m_iYesCount = { 0 };
	_int		m_iNoCount = { 0 };
	_bool		m_bYesPush = { false };
	_bool		m_bNoPush = { false };

	_bool		m_bTutorialOpen = { false };

	_bool		m_bCheckOpen = { false };
	_bool		m_bCheckClose = { false };

private:	// Tips
	TIPS		m_eTips = { TIPS_END };
	_bool		m_arrTips[TIPS_END] = {};

	_bool		m_iTipsOpen = { false };

private:	// Success
	_bool		m_bSuccess = { false };
	_double		m_bSuccess_TimeAcc = { 0.0 };

public:
	static CCanvas_Tutorial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END