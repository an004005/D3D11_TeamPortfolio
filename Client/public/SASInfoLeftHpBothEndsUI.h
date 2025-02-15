#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CSASInfoLeftHpBothEndsUI final : public CUI
{
private:
	CSASInfoLeftHpBothEndsUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSASInfoLeftHpBothEndsUI(const CSASInfoLeftHpBothEndsUI& rhs);

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

	void	Set_PlayerHp(const _float & fHp) {
		m_fHp = fHp;
	}

private:
	void	Start_Tick(const _double & dTimeDetla);
	void	End_Tick(const _double & dTimeDetla);

private:
	template <typename T>
	T Remap(T In, T Inlow, T Inhigh, T Outlow, T Outhigh);

	template <typename T>
	T Clamp(T x, T low, T high);


private:
	_int	m_iObjectNumber = { 0 };		// 2개 객체를 구별하기 위한 변수

	_float	m_fHp = { 0.0f };
	_float	m_fCurrentHp = { 0.0f };

public:
	static CSASInfoLeftHpBothEndsUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

template<typename T>
inline T CSASInfoLeftHpBothEndsUI::Remap(T In, T Inlow, T Inhigh, T Outlow, T Outhigh)
{
	return Outlow + (In - Inlow) * (Outhigh - Outlow) / (Inhigh - Inlow);
}

template<typename T>
inline T CSASInfoLeftHpBothEndsUI::Clamp(T x, T low, T high)
{
	assert(low <= high);
	return min(max(x, low), high);
}