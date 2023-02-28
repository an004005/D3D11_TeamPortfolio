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
	virtual void	BeginTick() override;
	virtual void	Tick(_double TimeDelta) override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;

public:
	_bool Get_ChangeX() {
		return m_bChangeX;
	}
	void Set_ChangeX(_bool bChange) {
		m_bChangeX = bChange;
	}

public:
	void	InputAlt(const _bool & bInput);
	void	InputCtrl(const _bool & bInput);

private:
	void	InputX_Tick(const _double & dTimeDelta);
	
private:
	class CCanvas_SASSkillMove * m_pCanvas = { nullptr };
	_bool			m_bChangeX = { false };				// 8개의 스킬 중 4개씩 바꿔서 확인할 수 있다. flase->Left, true->Right

	_bool			m_bChangeXButton = { false };		// 타이머 사용하기 위해서
	_double			m_dChangeX_TimcAcc = { 0.0 };		// X로 변경하면서, 잠깐 X 키에 불이 들어오는 용도

public:
	static CCanvas_SASSkill* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END