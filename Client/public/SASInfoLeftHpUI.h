#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CSASInfoLeftHpUI final : public CUI
{
private:
	CSASInfoLeftHpUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSASInfoLeftHpUI(const CSASInfoLeftHpUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;

public:
	void	RendomHpImage(const _int & iImageNumber);

	_bool	Get_MaxHp() {
		return m_bMaxHp;
	}
	void	Set_PlayerHp(const _float & fHp) {
		m_fHp = fHp;
	}

private:
	void	Object_Tick(const _double & dTimeDelta);

	void	Zero_Tick(const _double & dTimeDetla);
	void	One_Tick(const _double & dTimeDetla);
	void	Two_Tick(const _double & dTimeDetla);

private:
	void	NotDraw(const _float & _fMinHp);

private:
	_int	m_iObjectNumber = { 0 };	// 3개 객체를 구별하기 위한 변수

	_float	m_fHp = { 0.0f };
	_float	m_fCurrentHp = { 0.0f };
	
	_bool	m_bMaxHp = { false };

public:
	static CSASInfoLeftHpUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END