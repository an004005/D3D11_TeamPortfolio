#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_SASSkill : public CCanvas
{
public:
	enum OBJECTCOUNT { ONE, TWO, THREE, FOUR };
	enum SKILLINDEX { ONE0, TWO0, THREE0, FOUR0, ONE1, TWO1, THREE1, FOUR1, SKILLINDEX_END };
	enum SUPERPOWERS { PSYCHOKINESIS0, PSYCHOKINESIS1, IGNITION, RESHUFFLE, CLAIRVOYANCE, TELEPORTATION, TRANSPARENCY, DISCHARGE, COPY, HIGHSPEED, NOT, SUPERPOWERS_END };

protected:
	CCanvas_SASSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_SASSkill(const CCanvas_SASSkill& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;

public:
	SKILLINDEX Get_SASSkill() {
		return m_eSASSkill;
	}
	SKILLINDEX Get_PreSASSkill() {
		return m_ePreSASSkill;
	}
	SUPERPOWERS	Get_SuperPowers() {
		return m_eSuperPowers;
	}

	_bool Get_InputSkill(SKILLINDEX eSkill) {
		return m_bInputSkill[eSkill];
	}
	_bool Get_ChangeX() {
		return m_bChangeX;
	}
	_bool Get_OnSkill() {
		return m_bOnSkill;
	}

	void Set_SASSkill(SKILLINDEX eSkill) {
		m_eSASSkill = eSkill;
	}
	void Set_PreSASSkill(SKILLINDEX eSkill) {
		m_ePreSASSkill = eSkill;
	}
	void Set_SuperPowers(SUPERPOWERS eSkill) {
		m_eSuperPowers = eSkill;
	}

	void Set_InputSkill(SKILLINDEX eSkill, _bool bInput) {
		m_bInputSkill[eSkill] = bInput;
	}
	void Set_ChangeX(_bool bChange) {
		m_bChangeX = bChange;
	}
	void Set_OnSkill(_bool bOn) {
		m_bOnSkill = bOn;
	}

private:
	SKILLINDEX		m_eSASSkill = SKILLINDEX_END;		// UI ���� ���� ��ü�� ����ϱ� ������ �����ϱ� ���ؼ�
	SKILLINDEX		m_ePreSASSkill = SKILLINDEX_END;
	SUPERPOWERS		m_eSuperPowers = SUPERPOWERS_END;	// �ʴɷ� ����

	_bool			m_bInputSkill[SKILLINDEX_END] = {};	// �÷��̾�� Ű�� ������ ��
	_bool			m_bChangeX = { false };				// 8���� ��ų �� 4���� �ٲ㼭 Ȯ���� �� �ִ�.
	_bool			m_bOnSkill = { false };				// 1: �ʴɷ� ��� ����, 0: �ʴɷ� ��� �� ����

public:
	static CCanvas_SASSkill* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END