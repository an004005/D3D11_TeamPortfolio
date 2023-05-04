#pragma once

#include "Client_Defines.h"
#include "Canvas.h"

BEGIN(Client)

class CCanvas_SASSkill : public CCanvas
{
protected:
	CCanvas_SASSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_SASSkill(const CCanvas_SASSkill& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;

	virtual void	Imgui_RenderProperty() override;

public:
	void	InputAlt();
	void	InputCtrl();

private:
	void	InputX_Tick(const _double & dTimeDelta);
	void InputData();
	void FullCircle();
	
private:
	_bool			m_bChangeX = { false };				// 8개의 스킬 중 4개씩 바꿔서 확인할 수 있다. flase->Left, true->Right

	_bool			m_bChangeXButton = { false };		// 타이머 사용하기 위해서
	_double		m_dChangeX_TimcAcc = { 0.0 };		// X로 변경하면서, 잠깐 X 키에 불이 들어오는 용도

	_bool			m_bMember0 = { false };
	_bool			m_bMember1 = { false };

	// 1234 / 567  -> 이 안에서 바뀔 때는 X가 보이지 않고, 서로의 bool 값이 다른 경우에만 X 를 보이게 한다.
	_bool			m_bX = { false };						
	_bool			m_bPreX = { false };				

public:
	static CCanvas_SASSkill* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END