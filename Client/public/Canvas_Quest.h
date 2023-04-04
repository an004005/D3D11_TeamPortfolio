#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

typedef struct tagQuestInfo
{
	wstring wsQuest0 = L"";
	wstring wsQuest1 = L"";

}QUESTINFO;

class CCanvas_Quest : public CCanvas
{
protected:
	CCanvas_Quest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_Quest(const CCanvas_Quest& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void   Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;

private:
	void		Quest_Initialize();

private:
	vector<QUESTINFO>	m_vecQuestInfo;
	size_t	m_iQuestIndex = { 0 };

	_float2		m_fBackGround_StartPos = { 0.0f, 0.0f };

	_bool		m_fQuestMove = { false };
	_bool		m_fSuccessfulQuest = { false };

public:
	static CCanvas_Quest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END