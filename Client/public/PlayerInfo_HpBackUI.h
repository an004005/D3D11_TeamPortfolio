#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CPlayerInfo_HpBackUI final : public CUI
{
private:
	CPlayerInfo_HpBackUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerInfo_HpBackUI(const CPlayerInfo_HpBackUI& rhs);

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
	void	RendomHpImage(const _int & iImageNumber);

	void	Set_PlayerHp(const _float & fHp) {
		m_fHp = fHp;
	}
	void	Set_Speed(const _bool	bSpeed) {
		m_bSpeed = bSpeed;
	}

private:
	void	Object_Tick(const _double & dTimeDelta);

	void	Zero_Tick(const _double & dTimeDelta);
	void	One_Tick(const _double & dTimeDelta);
	void	Two_Tick(const _double & dTimeDelta);
	void	Three_Tick(const _double & dTimeDelta);
	void	Four_Tick(const _double & dTimeDelta);
	void	Five_Tick(const _double & dTimeDelta);

private:
	void	NotDraw(const _float & _fMinHp);

private:
	_int	m_iObjectNumber = { 0 };		// 6�� ��ü�� �����ϱ� ���� ����

	_float	m_fHp = { 0.0f };
	_float	m_fCurrentHp = { 0.0f };

	_bool	m_bSpeed = { false };

public:
	static CPlayerInfo_HpBackUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END