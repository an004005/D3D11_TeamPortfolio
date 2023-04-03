#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

enum class AXIS { LEFT, RIGHT, UP, DOWN, RIGHT_ROTATION, LEFT_ROTATION, AXIS_END };

class CSA_AxisUI final : public CUI
{
private:
	CSA_AxisUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSA_AxisUI(const CSA_AxisUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;

private:
	void			Type_Tick(const _double & TimeDelta);

	void			Left_Tick(const _double& TimeDelta);
	void			Right_Tick(const _double& TimeDelta);
	void			Up_Tick(const _double& TimeDelta);
	void			Down_Tick(const _double& TimeDelta);

	void			Right_Rotation_Tick(const _double& TimeDelta);
	void			Left_Rotation_Tick(const _double& TimeDelta);

public:
	const void	Set_AxisInitialize(const AXIS eAxis, const _float fSpeed = 100.0f, const _double dMoveTime = 0.15) {
		m_eAxis = eAxis;
		m_fSpeed = fSpeed;
		m_fMoveTime = dMoveTime;
	}
	const void	Set_Move(const _bool bMove = false) {
		m_bMove = bMove;
	}

private:
	// 외부에서 초기화시 넣어주어야 하는 데이터
	AXIS			m_eAxis = { AXIS::AXIS_END };
	_float		m_fSpeed = { 0.0f };
	_double	m_fMoveTime = { 0.0 };

	_bool		m_bMove = { false };

	_double	m_fMove_TimeAcc = { 0.0 };
	_float		m_fStartX = { 0.0f };
	_float		m_fStartY = { 0.0f };

public:
	static CSA_AxisUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END