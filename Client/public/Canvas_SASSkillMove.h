#pragma once

#include "Client_Defines.h"
#include "Canvas.h"

BEGIN(Engine)
class CFSMComponent;
END

	BEGIN(Client)

class CCanvas_SASSkillMove : public CCanvas
{
public:
	enum OBJECTCOUNT { ONE, TWO, THREE, FOUR };
	enum SKILLINDEX { ONE0, TWO0, THREE0, FOUR0, ONE1, TWO1, THREE1, FOUR1, SKILLINDEX_END };
	enum SUPERPOWERS { PSYCHOKINESIS0, PSYCHOKINESIS1, IGNITION, RESHUFFLE, CLAIRVOYANCE, TELEPORTATION, TRANSPARENCY, DISCHARGE, COPY, HIGHSPEED, NOT, SUPERPOWERS_END };

protected:
	CCanvas_SASSkillMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_SASSkillMove(const CCanvas_SASSkillMove& rhs);

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
	SKILLINDEX Get_SASSkill() {
		return m_eSASSkill;
	}
	void Set_SASSkill(SKILLINDEX eSkill) {
		m_eSASSkill = eSkill;
	}

	SKILLINDEX Get_PreSASSkill() {
		return m_ePreSASSkill;
	}
	void Set_PreSASSkill(SKILLINDEX eSkill) {
		m_ePreSASSkill = eSkill;
	}

	SUPERPOWERS	Get_SuperPowers() {
		return m_eSuperPowers;
	}
	void Set_SuperPowers(SUPERPOWERS eSkill) {
		m_eSuperPowers = eSkill;
	}

	_bool Get_InputSkill(SKILLINDEX eSkill) {
		return m_bInputSkill[eSkill];
	}
	void Set_InputSkill(SKILLINDEX eSkill, _bool bInput) {
		m_bInputSkill[eSkill] = bInput;
	}

	_bool Get_OnSkill() {
		return m_bOnSkill;
	}
	void Set_OnSkill(_bool bOn) {
		m_bOnSkill = bOn;
	}

public:
	void	UIMove_FSM();

	void	Info_Tick(const _bool bPush); // Ctrl, Alt 를 눌렀을 때에 대한 처리를 하기 위해 (스킬 아이콘이 보이지 않으면서 정보가 뜬다.)
	void	InputCtrl_Tick();
	void	InputAlt_Tick();

private:
	CFSMComponent*	m_pFSM = { nullptr };
	class CCanvas_SASSkill* m_pCanvas = { nullptr };

	SKILLINDEX		m_eSASSkill = SKILLINDEX_END;		// UI 들이 같은 객체를 사용하기 때문에 구별하기 위해서
	SKILLINDEX		m_ePreSASSkill = SKILLINDEX_END;	// 이전에 사용한 스킬
	SUPERPOWERS		m_eSuperPowers = SUPERPOWERS_END;	// 초능력 종류

	_bool			m_bInputSkill[SKILLINDEX_END] = {};	// 플레이어에서 키를 눌렀을 때
	_bool			m_bOnSkill = { false };				// 1: 초능력 사용 가능, 0: 초능력 사용 불 가

	// UIMove_FSM() -> 도착지점에 도달했다가 원래 지점으로 돌아가기 위해 함수
	_bool			m_bUIMove = { false };				// SASSkill_UIMove() -> UI를 이동시키고자 할 때 외부에서 Set 을 한다.
	_bool			m_bIsDestination = { false };		// 목표지점에 도달! 원점으로 이제 돌아가라
	_float2			m_vDestination = { 0.0f, 0.0f };	// 도착지점

	map<wstring, CUI*> Temp;

public:
	static CCanvas_SASSkillMove* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END