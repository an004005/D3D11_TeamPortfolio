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

	void	Info_Tick(const _bool bPush); // Ctrl, Alt �� ������ ���� ���� ó���� �ϱ� ���� (��ų �������� ������ �����鼭 ������ ���.)
	void	InputCtrl_Tick();
	void	InputAlt_Tick();

private:
	CFSMComponent*	m_pFSM = { nullptr };
	class CCanvas_SASSkill* m_pCanvas = { nullptr };

	SKILLINDEX		m_eSASSkill = SKILLINDEX_END;		// UI ���� ���� ��ü�� ����ϱ� ������ �����ϱ� ���ؼ�
	SKILLINDEX		m_ePreSASSkill = SKILLINDEX_END;	// ������ ����� ��ų
	SUPERPOWERS		m_eSuperPowers = SUPERPOWERS_END;	// �ʴɷ� ����

	_bool			m_bInputSkill[SKILLINDEX_END] = {};	// �÷��̾�� Ű�� ������ ��
	_bool			m_bOnSkill = { false };				// 1: �ʴɷ� ��� ����, 0: �ʴɷ� ��� �� ��

	// UIMove_FSM() -> ���������� �����ߴٰ� ���� �������� ���ư��� ���� �Լ�
	_bool			m_bUIMove = { false };				// SASSkill_UIMove() -> UI�� �̵���Ű���� �� �� �ܺο��� Set �� �Ѵ�.
	_bool			m_bIsDestination = { false };		// ��ǥ������ ����! �������� ���� ���ư���
	_float2			m_vDestination = { 0.0f, 0.0f };	// ��������

	map<wstring, CUI*> Temp;

public:
	static CCanvas_SASSkillMove* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END