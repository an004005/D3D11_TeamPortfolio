#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

typedef struct tagTalkInfo
{
	_float2		fFace;
	wstring	wsTalk0;
	wstring	wsTalk1;

}TALKINFO;

BEGIN(Client)

class CCanvas_LeftTalk : public CCanvas
{
protected:
	CCanvas_LeftTalk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_LeftTalk(const CCanvas_LeftTalk& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void   Tick(_double TimeDelta) override;

public:
	void	Add_Talk(const _int iIndex, const _int iQuest = -1);
	_bool	Get_End() {
		return m_bRunning;
	}

private:
	void	Show_Talk();

private:
	std::list<TALKINFO>	m_qCurrentTalk;

	_bool		m_bRunning = { false };
	_double	m_dTalk_TimeAcc = { 3.0 };

	_int		m_iQuestNum = { -1 };
	_int		m_iMapMove = { -1 };

	_bool	m_bSound = { false };

public:
	static CCanvas_LeftTalk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END