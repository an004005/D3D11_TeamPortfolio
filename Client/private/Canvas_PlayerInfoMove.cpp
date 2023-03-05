#include "stdafx.h"
#include "..\public\Canvas_PlayerInfoMove.h"
#include "GameInstance.h"
#include "FSMComponent.h"
#include "UI_Manager.h"
#include "MathUtils.h"

#include "PlayerInfo_HpUI.h"
#include "PlayerInfo_HpBackUI.h"
#include "PlayerInfo_HpBothEndsUI.h"

CCanvas_PlayerInfoMove::CCanvas_PlayerInfoMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_PlayerInfoMove::CCanvas_PlayerInfoMove(const CCanvas_PlayerInfoMove& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_PlayerInfoMove::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_PlayerInfoMove::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	CUI_Manager::GetInstance()->Add_MoveCanvas(L"Canvas_PlayerInfoMove", this);
	CCanvas::UIMove_FSM();

	return S_OK;
}

void CCanvas_PlayerInfoMove::BeginTick()
{

}

void CCanvas_PlayerInfoMove::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	m_pUIMoveFSM->Tick(TimeDelta);

	if (CGameInstance::GetInstance()->KeyDown(DIK_0))
		Set_UIMove();

	ChildHp_Tick();
	RendomTexture(TimeDelta);	// 계속 Hp 가 출력할 전체 개수, 이미지를 계산한다.
}

void CCanvas_PlayerInfoMove::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);
	m_bVisible = true;

}

HRESULT CCanvas_PlayerInfoMove::Render()
{
	if (FAILED(CCanvas::Render()))
		return E_FAIL;

	_float2 fTemp = dynamic_cast<CPlayerInfo_HpUI*>(Find_ChildUI(L"PlayerInfo_Hp0"))->GetScreenSpaceLeftTop();
	CGameInstance::GetInstance()->Render_Font(L"Pretendard32", L"유이토 스메라기", fTemp + m_vFontPos, 0.f, m_vFontScale, { 1.0f, 0.99f, 0.87f, 1.0f });

	return S_OK;
}

void CCanvas_PlayerInfoMove::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

	// UITEST
	static _float fHp;
	ImGui::InputFloat("Hp", &fHp);
	static _float fMaxHp;
	ImGui::InputFloat("MaxHp", &fMaxHp);

	if (ImGui::Button("Save Hp"))
	{
		m_fHp = fHp / fMaxHp;
	}

	static _float fPosition[2];
	ImGui::DragFloat2("Font Position", fPosition);
	m_vFontPos = { fPosition[0], fPosition[1] };

	static _float fScele[2];
	ImGui::DragFloat2("Font fScele", fScele);
	m_vFontScale = { fScele[0], fScele[1] };
}

void CCanvas_PlayerInfoMove::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);

}

void CCanvas_PlayerInfoMove::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);

	m_vFontPos = { 0.35f, 0.35f };
}

void CCanvas_PlayerInfoMove::ChildHp_Tick()
{
	dynamic_cast<CPlayerInfo_HpUI*>(Find_ChildUI(L"PlayerInfo_Hp0"))->Set_PlayerHp(m_fHp);
	dynamic_cast<CPlayerInfo_HpUI*>(Find_ChildUI(L"PlayerInfo_Hp1"))->Set_PlayerHp(m_fHp);
	dynamic_cast<CPlayerInfo_HpUI*>(Find_ChildUI(L"PlayerInfo_Hp2"))->Set_PlayerHp(m_fHp);
	dynamic_cast<CPlayerInfo_HpUI*>(Find_ChildUI(L"PlayerInfo_Hp3"))->Set_PlayerHp(m_fHp);
	dynamic_cast<CPlayerInfo_HpUI*>(Find_ChildUI(L"PlayerInfo_Hp4"))->Set_PlayerHp(m_fHp);
	dynamic_cast<CPlayerInfo_HpUI*>(Find_ChildUI(L"PlayerInfo_Hp5"))->Set_PlayerHp(m_fHp);

	dynamic_cast<CPlayerInfo_HpBackUI*>(Find_ChildUI(L"PlayerInfo_HpBack0"))->Set_PlayerHp(m_fHp);
	dynamic_cast<CPlayerInfo_HpBackUI*>(Find_ChildUI(L"PlayerInfo_HpBack1"))->Set_PlayerHp(m_fHp);
	dynamic_cast<CPlayerInfo_HpBackUI*>(Find_ChildUI(L"PlayerInfo_HpBack2"))->Set_PlayerHp(m_fHp);
	dynamic_cast<CPlayerInfo_HpBackUI*>(Find_ChildUI(L"PlayerInfo_HpBack3"))->Set_PlayerHp(m_fHp);
	dynamic_cast<CPlayerInfo_HpBackUI*>(Find_ChildUI(L"PlayerInfo_HpBack4"))->Set_PlayerHp(m_fHp);
	dynamic_cast<CPlayerInfo_HpBackUI*>(Find_ChildUI(L"PlayerInfo_HpBack5"))->Set_PlayerHp(m_fHp);

	dynamic_cast<CPlayerInfo_HpBothEndsUI*>(Find_ChildUI(L"PlayerInfo_EndHp"))->Set_PlayerHp(m_fHp);
	dynamic_cast<CPlayerInfo_HpBothEndsUI*>(Find_ChildUI(L"PlayerInfo_EndHpBack"))->Set_PlayerHp(m_fHp);
	dynamic_cast<CPlayerInfo_HpBothEndsUI*>(Find_ChildUI(L"PlayerInfo_StartHp"))->Set_PlayerHp(m_fHp);
	dynamic_cast<CPlayerInfo_HpBothEndsUI*>(Find_ChildUI(L"PlayerInfo_StartHpBack"))->Set_PlayerHp(m_fHp);
}

void CCanvas_PlayerInfoMove::RendomTexture(const _double & dTimeDelta)
{
	m_dRendomTexture_TimeAcc += dTimeDelta;
	if (3.0 < m_dRendomTexture_TimeAcc)
		m_dRendomTexture_TimeAcc = 0.0;

	if (0.0 != m_dRendomTexture_TimeAcc)
		return;

	// 체력에 따라서 랜덤으로 이미지를 출력하는 개수가 달라진다. (m_fHp 기준)
	// 0.05~0.95 : 3 / 0.05~0.65 : 2 / 0.05~0.35 : 1

	_int iCount;
	_int iRendomCount;			// 움직이는 Hp 를 그리는 개수 
	_float fObjectMaxNumber;	// 6개중 움직이는 Hp를 그리는 객체
	if (0.95f < m_fHp)
	{
		iCount = 3;
		fObjectMaxNumber = 6.0f;
		iRendomCount = _int(CMathUtils::RandomFloat(0.0f, 4.0f));
	}
	else if (0.65f < m_fHp)
	{
		iCount = 2;
		fObjectMaxNumber = 5.0f;
		iRendomCount = _int(CMathUtils::RandomFloat(0.0f, 3.0f));
	}
	else if (0.35f < m_fHp)
	{
		iCount = 1;
		fObjectMaxNumber = 3.0f;
		iRendomCount = _int(CMathUtils::RandomFloat(0.0f, 2.0f));
	}
	else
	{
		iCount = 0;
		fObjectMaxNumber = 0.0f;
		iRendomCount = 0;
	}

	if (0 == iCount)
		return;

	_int arrObjectNumber[6] = { -1, -1, -1, -1, -1, -1 };

	for (_int i = 0; i < iCount; i++) // 체력에 따라 다른 Count 를 받아온다.
	{
		_int iRandimArrayNumber = _int(CMathUtils::RandomFloat(0.0f, _float(iCount + iCount)));	// 랜덤 으로 배열에 담기 위해서
		_int iRandomObjectNumber = _int(CMathUtils::RandomFloat(0.0f, fObjectMaxNumber));	// 랜덤 객체 번호로 넘기기 위해서
		arrObjectNumber[iRandimArrayNumber] = iRandomObjectNumber;
	}

	for (_int i = 0; i < _int(fObjectMaxNumber); i++)
	{
		_tchar szChildTag[MAX_PATH] = TEXT("");
		if (-1 == arrObjectNumber[i])
		{
			wsprintf(szChildTag, TEXT("PlayerInfo_Hp%d"), i);
			dynamic_cast<CPlayerInfo_HpUI*>(Find_ChildUI(szChildTag))->RendomHpImage(2);

			wsprintf(szChildTag, TEXT("PlayerInfo_HpBack%d"), i);
			dynamic_cast<CPlayerInfo_HpBackUI*>(Find_ChildUI(szChildTag))->RendomHpImage(2);
		}
		else
		{
			_int iRandomTexture = _int(CMathUtils::RandomFloat(0.0f, 2.0f)); // 0 or 1

			wsprintf(szChildTag, TEXT("PlayerInfo_Hp%d"), i);
			dynamic_cast<CPlayerInfo_HpUI*>(Find_ChildUI(szChildTag))->RendomHpImage(iRandomTexture);

			wsprintf(szChildTag, TEXT("PlayerInfo_HpBack%d"), i);
			dynamic_cast<CPlayerInfo_HpBackUI*>(Find_ChildUI(szChildTag))->RendomHpImage(iRandomTexture);
		}
	}
}

CCanvas_PlayerInfoMove * CCanvas_PlayerInfoMove::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_PlayerInfoMove*		pInstance = new CCanvas_PlayerInfoMove(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_PlayerInfoMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_PlayerInfoMove::Clone(void * pArg)
{
	CCanvas_PlayerInfoMove*		pInstance = new CCanvas_PlayerInfoMove(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_PlayerInfoMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_PlayerInfoMove::Free()
{
	CCanvas::Free();

}
