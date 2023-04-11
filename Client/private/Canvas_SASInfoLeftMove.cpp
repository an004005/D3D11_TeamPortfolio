#include "stdafx.h"
#include "..\public\Canvas_SASInfoLeftMove.h"
#include "GameInstance.h"
#include "FSMComponent.h"
#include "UI_Manager.h"
#include "MathUtils.h"
#include "PlayerInfoManager.h"

#include "Canvas_SASInfoLeft.h"
#include "SASInfoLeftHpUI.h"
#include "SASInfoLeftHpBackUI.h"
#include "SASInfoLeftHpBothEndsUI.h"
#include "ShaderUI.h"

CCanvas_SASInfoLeftMove::CCanvas_SASInfoLeftMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_SASInfoLeftMove::CCanvas_SASInfoLeftMove(const CCanvas_SASInfoLeftMove& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_SASInfoLeftMove::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_SASInfoLeftMove::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	CUI_Manager::GetInstance()->Add_MoveCanvas(L"Canvas_SASInfoLeftMove", this);
	m_vMaxDestination = { -3.0f, -7.0f };
	CCanvas::UIMove_FSM();

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	return S_OK;
}

void CCanvas_SASInfoLeftMove::Tick(_double TimeDelta)
{
	// 아직 멤버가 아니라면 Tick 을 돌지 않는다.
	if (false == m_bMember)
	{
		if (false == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::TSUGUMI)) 
			return;
		
		m_bMember = true;

		for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
			iter->second->SetVisible(true);
	}

	CCanvas::Tick(TimeDelta);

	m_pUIMoveFSM->Tick(TimeDelta);
	CCanvas::UIHit(TimeDelta);

	SASRightHp_Tick();
	RendomTexture_Tick(TimeDelta);
	HillBar_Tick(TimeDelta);
}

void CCanvas_SASInfoLeftMove::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

	// UITEST
	static _float fHp;
	ImGui::InputFloat("Hp", &fHp);
	static _float fMaxHp;
	ImGui::InputFloat("MaxHp", &fMaxHp);

	if (ImGui::Button("Save Hp"))
	{
		CPlayerInfoManager::GetInstance()->Get_TsugumiStat().iHP = fHp;
		CPlayerInfoManager::GetInstance()->Get_TsugumiStat().iMaxHP = fMaxHp;
	}
}

void CCanvas_SASInfoLeftMove::Set_HillBar()
{
	m_bHill = true;
	m_bHpHill = true;

	dynamic_cast<CSASInfoLeftHpBackUI*>(Find_ChildUI(L"SASInfoLeft_HpBack0"))->Set_Speed(true);
	dynamic_cast<CSASInfoLeftHpBackUI*>(Find_ChildUI(L"SASInfoLeft_HpBack1"))->Set_Speed(true);
	dynamic_cast<CSASInfoLeftHpBackUI*>(Find_ChildUI(L"SASInfoLeft_HpBack2"))->Set_Speed(true);
}

void CCanvas_SASInfoLeftMove::SASRightHp_Tick()
{
	if (true == m_bHpHill) return;

	_float fHp = static_cast<_float>(CPlayerInfoManager::GetInstance()->Get_TsugumiStat().iHP);
	_float fMaxHp = static_cast<_float>(CPlayerInfoManager::GetInstance()->Get_TsugumiStat().iMaxHP);
	m_fHPRatio = fHp / fMaxHp;

	dynamic_cast<CCanvas_SASInfoLeft*>(CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_SASInfoLeft"))->Set_Render();

	dynamic_cast<CSASInfoLeftHpUI*>(Find_ChildUI(L"SASInfoLeft_Hp0"))->Set_PlayerHp(m_fHPRatio);
	dynamic_cast<CSASInfoLeftHpUI*>(Find_ChildUI(L"SASInfoLeft_Hp1"))->Set_PlayerHp(m_fHPRatio);
	dynamic_cast<CSASInfoLeftHpUI*>(Find_ChildUI(L"SASInfoLeft_Hp2"))->Set_PlayerHp(m_fHPRatio);

	dynamic_cast<CSASInfoLeftHpBackUI*>(Find_ChildUI(L"SASInfoLeft_HpBack0"))->Set_PlayerHp(m_fHPRatio);
	dynamic_cast<CSASInfoLeftHpBackUI*>(Find_ChildUI(L"SASInfoLeft_HpBack1"))->Set_PlayerHp(m_fHPRatio);
	dynamic_cast<CSASInfoLeftHpBackUI*>(Find_ChildUI(L"SASInfoLeft_HpBack2"))->Set_PlayerHp(m_fHPRatio);

	dynamic_cast<CSASInfoLeftHpBothEndsUI*>(Find_ChildUI(L"SASInfoLeft_EndHp"))->Set_PlayerHp(m_fHPRatio);
	dynamic_cast<CSASInfoLeftHpBothEndsUI*>(Find_ChildUI(L"SASInfoLeft_EndHpBack"))->Set_PlayerHp(m_fHPRatio);
	dynamic_cast<CSASInfoLeftHpBothEndsUI*>(Find_ChildUI(L"SASInfoLeft_StartHp"))->Set_PlayerHp(m_fHPRatio);
	dynamic_cast<CSASInfoLeftHpBothEndsUI*>(Find_ChildUI(L"SASInfoLeft_StartHpBack"))->Set_PlayerHp(m_fHPRatio);
}

void CCanvas_SASInfoLeftMove::RendomTexture_Tick(const _double& dTimeDelta)
{
	m_dRendomTexture_TimeAcc += dTimeDelta;
	if (3.0 < m_dRendomTexture_TimeAcc)
		m_dRendomTexture_TimeAcc = 0.0;

	if (0.0 != m_dRendomTexture_TimeAcc)
		return;

	// 체력에 따라서 랜덤으로 이미지를 출력하는 개수가 달라진다. (m_fHPRatio 기준)
	// 0.05~0.95 : 2 / 0.05~0.65 : 1

	_int iCount;
	_int iRendomCount;			// 움직이는 Hp 를 그리는 개수 
	_float fObjectMaxNumber;	// 3개중 움직이는 Hp를 그리는 객체
	if (0.95f < m_fHPRatio)
	{
		iCount = 2;
		fObjectMaxNumber = 3.0f;
		iRendomCount = _int(CMathUtils::RandomFloat(0.0f, 3.0f));
	}
	else if (0.65f < m_fHPRatio)
	{
		iCount = 1;
		fObjectMaxNumber = 2.0f;
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

	_int arrObjectNumber[3] = { -1, -1, -1 };

	for (_int i = 0; i < iCount; i++) // 체력에 따라 다른 Count 를 받아온다.
	{
		_int iRandimArrayNumber = _int(CMathUtils::RandomFloat(0.0f, _float(iCount + 1)));	// 랜덤 으로 배열에 담기 위해서
		_int iRandomObjectNumber = _int(CMathUtils::RandomFloat(0.0f, fObjectMaxNumber));	// 랜덤 객체 번호로 넘기기 위해서
		arrObjectNumber[iRandimArrayNumber] = iRandomObjectNumber;
	}

	for (_int i = 0; i < _int(fObjectMaxNumber); i++)
	{
		_tchar szChildTag[MAX_PATH] = TEXT("");
		if (-1 == arrObjectNumber[i])
		{
			wsprintf(szChildTag, TEXT("SASInfoLeft_Hp%d"), i);
			dynamic_cast<CSASInfoLeftHpUI*>(Find_ChildUI(szChildTag))->RendomHpImage(2);

			wsprintf(szChildTag, TEXT("SASInfoLeft_HpBack%d"), i);
			dynamic_cast<CSASInfoLeftHpBackUI*>(Find_ChildUI(szChildTag))->RendomHpImage(2);
		}
		else
		{
			_int iRandomTexture = _int(CMathUtils::RandomFloat(0.0f, 2.0f)); // 0 or 1

			wsprintf(szChildTag, TEXT("SASInfoLeft_Hp%d"), i);
			dynamic_cast<CSASInfoLeftHpUI*>(Find_ChildUI(szChildTag))->RendomHpImage(iRandomTexture);

			wsprintf(szChildTag, TEXT("SASInfoLeft_HpBack%d"), i);
			dynamic_cast<CSASInfoLeftHpBackUI*>(Find_ChildUI(szChildTag))->RendomHpImage(iRandomTexture);
		}
	}
}

void CCanvas_SASInfoLeftMove::HillBar_Tick(const _double& TimeDelta)
{
	if (false == m_bHill)	return;

	Find_ChildUI(L"HillBar")->SetVisible(true);

	_float fHp = static_cast<_float>(CPlayerInfoManager::GetInstance()->Get_TsugumiStat().iHP);
	_float fMaxHp = static_cast<_float>(CPlayerInfoManager::GetInstance()->Get_TsugumiStat().iMaxHP);
	_float fCurrentHill = dynamic_cast<CShaderUI*>(Find_ChildUI(L"HillBar"))->Get_Floats0();

	if (0.2f < fCurrentHill)
	{
		dynamic_cast<CSASInfoLeftHpBackUI*>(Find_ChildUI(L"SASInfoLeft_HpBack0"))->Set_Speed(false);
		dynamic_cast<CSASInfoLeftHpBackUI*>(Find_ChildUI(L"SASInfoLeft_HpBack1"))->Set_Speed(false);
		dynamic_cast<CSASInfoLeftHpBackUI*>(Find_ChildUI(L"SASInfoLeft_HpBack2"))->Set_Speed(false);
	}

	if (fCurrentHill < (fHp / fMaxHp))
	{
		fCurrentHill += _float(TimeDelta) * 0.3f;
		dynamic_cast<CShaderUI*>(Find_ChildUI(L"HillBar"))->Set_Floats0(fCurrentHill);
	}
	else
	{
		m_bHpHill = false;

		m_dMaxHillChake_TimeAcc += TimeDelta;
		if (1.0 < m_dMaxHillChake_TimeAcc)
		{
			if (true == dynamic_cast<CSASInfoLeftHpUI*>(Find_ChildUI(L"SASInfoLeft_Hp0"))->Get_MaxHp() &&
				true == dynamic_cast<CSASInfoLeftHpUI*>(Find_ChildUI(L"SASInfoLeft_Hp1"))->Get_MaxHp() &&
				true == dynamic_cast<CSASInfoLeftHpUI*>(Find_ChildUI(L"SASInfoLeft_Hp2"))->Get_MaxHp())
			{
				m_dMaxHillChake_TimeAcc = 0.0;
				m_bHill = false;
				Find_ChildUI(L"HillBar")->SetVisible(false);
				dynamic_cast<CShaderUI*>(Find_ChildUI(L"HillBar"))->Set_Floats0(0.0f);
			}
		}
	}
}

CCanvas_SASInfoLeftMove * CCanvas_SASInfoLeftMove::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_SASInfoLeftMove*		pInstance = new CCanvas_SASInfoLeftMove(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_SASInfoLeftMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_SASInfoLeftMove::Clone(void * pArg)
{
	CCanvas_SASInfoLeftMove*		pInstance = new CCanvas_SASInfoLeftMove(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_SASInfoLeftMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_SASInfoLeftMove::Free()
{
	CCanvas::Free();
}
