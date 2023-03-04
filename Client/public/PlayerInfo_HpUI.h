#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CPlayerInfo_HpUI final : public CUI
{
private:
	CPlayerInfo_HpUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerInfo_HpUI(const CPlayerInfo_HpUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	BeginTick() override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;

public:
	void	Set_PlayerHp(const _float & fHp) {
		m_fHp = fHp;
	}
	void	Set_PlayerMaxHp(const _float & fMaxHp) {
		m_fMaxHp = fMaxHp;
	}

private:
	void	Object_Tick(const _double & dTimeDelta);

	void	Zero_Tick(const _double & dTimeDetla);
	void	One_Tick(const _double & dTimeDetla);
	void	Two_Tick(const _double & dTimeDetla);
	void	Three_Tick(const _double & dTimeDetla);
	void	Four_Tick(const _double & dTimeDetla);
	void	Five_Tick(const _double & dTimeDetla);

private:
	void	RendomHpImage(const _double & dTimeDetla);
	void	NotDraw(const _float & _fMinHp);

private:
	_int	m_iObjectNumber = { 0 };		// 6개 객체를 구별하기 위한 변수
	
	_float	m_fHp = { 0.0f };
	_float	m_fMaxHp = { 0.0f };
	_float	m_fCurrentHp = { 0.0f };

private:
	_double	m_dImageChange_TimeAcc = { 0.0 };
	_int	m_iRandomNumebr = { 0 };

public:
	static CPlayerInfo_HpUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END