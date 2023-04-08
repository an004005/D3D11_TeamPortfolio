#pragma once

#include "Canvas.h"
#include "Client_Defines.h"
#include <queue>

typedef struct tagMainTalkInfo
{
	wstring wsName = L"";
	wstring wsTalk0 = L"";
	wstring wsTalk1 = L"";

}MAINTALKINFO;

BEGIN(Client)

class CCanvas_MainTalk : public CCanvas
{
protected:
	CCanvas_MainTalk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_MainTalk(const CCanvas_MainTalk& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void   Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;

public:
	void	Add_Talk(const size_t iIndex);

private:
	void	Show_Talk();

private:
	std::queue<MAINTALKINFO>	m_qCurrentTalk;
	MAINTALKINFO	m_tTalkInfo = {};

	_bool		m_bRunning = false;

	_bool		m_bNextTalk = { false };
	_double	m_dTalk_TimeAcc = { 0.0 };

public:
	static CCanvas_MainTalk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END