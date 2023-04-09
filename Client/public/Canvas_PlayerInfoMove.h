#pragma once

#include "Client_Defines.h"
#include "Canvas.h"

BEGIN(Client)

enum class PSYCHOKINESISTYPE { ATTACK_TYPE, DRIVE_TYPE, IDLE_TYPE };

class CCanvas_PlayerInfoMove : public CCanvas
{
protected:
	CCanvas_PlayerInfoMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_PlayerInfoMove(const CCanvas_PlayerInfoMove& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	BeginTick() override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;

public:
	void	Set_HillBar();
	void	Set_PsychokinesisType();

private:
	void	PlayerHp_Tick();
	void	RendomTexture_Tick(const _double & dTimeDelta);
	void	Arrow_Move();
	void	HillBar_Tick(const _double& TimeDelta);

	void	PsychokinesisGauge_Tick();

private:
	template <typename T>
	T Remap(T In, T Inlow, T Inhigh, T Outlow, T Outhigh);

	template <typename T>
	T Clamp(T x, T low, T high);

private:
	//class CPlayer *	m_pPlayer = { nullptr };
	_float	m_fPsychokinesisGauge = { 0.0f };

	_float	m_fPercentageHp = { 0.0f };	// 캔버스가 가지고 있는 Hp와 Hp에서 직접 가지고 있는 Hp는 다르다. Hp 제엇 직접 가지면 TimeDelta 를 곱해서 천천히 빠진다.
	_double	m_dRendomTexture_TimeAcc = { 0.0 };

	_bool	m_bHill = { false };
	_bool	m_bHpHill = { false };
	_double	m_dMaxHillChake_TimeAcc = { 0.0 };	// HP 바 들이 가득 찼는지 확인하는데 한 틱 차이로 바로 차서 true 가 되는 바람에 약간의 시간을 주기 위한 변수

public:
	static CCanvas_PlayerInfoMove* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

template<typename T>
inline T CCanvas_PlayerInfoMove::Remap(T In, T Inlow, T Inhigh, T Outlow, T Outhigh)
{
	return Outlow + (In - Inlow) * (Outhigh - Outlow) / (Inhigh - Inlow);
}

template<typename T>
inline T CCanvas_PlayerInfoMove::Clamp(T x, T low, T high)
{
	assert(low <= high);
	return min(max(x, low), high);
}