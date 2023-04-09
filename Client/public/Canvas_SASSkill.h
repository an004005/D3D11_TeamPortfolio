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
	void	Set_Grow(const _uint iNumber, const _bool bVisible);

public:
	void	InputAlt();
	void	InputCtrl();

private:
	void	InputX_Tick(const _double & dTimeDelta);
	void FullCircle();
	
private:
	_bool			m_bChangeX = { false };				// 8���� ��ų �� 4���� �ٲ㼭 Ȯ���� �� �ִ�. flase->Left, true->Right

	_bool			m_bChangeXButton = { false };		// Ÿ�̸� ����ϱ� ���ؼ�
	_double		m_dChangeX_TimcAcc = { 0.0 };		// X�� �����ϸ鼭, ��� X Ű�� ���� ������ �뵵

	_bool			m_bMember0 = { false };
	_bool			m_bMember1 = { false };

	//_bool			m_bCtrl = { false };
	//_bool			m_bAlt = { false };
	//_bool			m_bKeyInput[2] = { false };			// [0]Ctrl, [1]Alt �� ���ÿ� ������ �ʵ��� �ϱ� ���ؼ�

public:
	static CCanvas_SASSkill* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END