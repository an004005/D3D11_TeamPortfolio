#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

typedef struct tagQuestInfo
{
	wstring wsQuest0 = L"";
	//wstring wsQuest1 = L"";

	//  L"Chake" 글씨에 따라서 길이를 조절하고 싶은 경우 추가하기
	// Quest 가 2개 이상 생길 경우 첫 Y Pos 를 지정할 수 있도록 하면 된다.

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

public:
	void	Add_Quest(const _int iIndex);
	void	Set_SuccessQuest() {
		m_bVisible = true;
		m_fQuestMove = true;
		m_fSuccessQuest = true;
	}

private:

	void		Move_Tick(const _double& TimeDelta);
	void		Success(const _double& TimeDelta);
	void		Success_Tick();

private:
	wstring m_wsQuest = L"";
	_int	m_iQuestIndex = { 0 };

	_float2		m_fBackGround_StartPos = { 0.0f, 0.0f };

	_bool		m_fQuestMove = { false };
	_bool		m_fSuccessQuest = { false };

	_double	m_dMove_TimeAcc = { 0.0 };

public:
	static CCanvas_Quest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END