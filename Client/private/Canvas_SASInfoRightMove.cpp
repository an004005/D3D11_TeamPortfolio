#include "stdafx.h"
#include "..\public\Canvas_SASInfoRightMove.h"
#include "GameInstance.h"
#include "FSMComponent.h"
#include "UI_Manager.h"
#include "MathUtils.h"
#include "PlayerInfoManager.h"

#include "Canvas_SASInfoRight.h"
#include "SASInfoRightHpUI.h"
#include "SASInfoRightHpBackUI.h"
#include "SASInfoRightHpBothEndsUI.h"
#include "ShaderUI.h"

CCanvas_SASInfoRightMove::CCanvas_SASInfoRightMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_SASInfoRightMove::CCanvas_SASInfoRightMove(const CCanvas_SASInfoRightMove& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_SASInfoRightMove::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_SASInfoRightMove::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	CUI_Manager::GetInstance()->Add_MoveCanvas(L"Canvas_SASInfoRightMove", this);
	m_vMaxDestination = { 3.0f, -7.0f };
	CCanvas::UIMove_FSM();

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	return S_OK;
}

void CCanvas_SASInfoRightMove::BeginTick()
{
	CCanvas::BeginTick();

}

void CCanvas_SASInfoRightMove::Tick(_double TimeDelta)
{
	// ���� ����� �ƴ϶�� Tick �� ���� �ʴ´�.
	if (false == m_bMember)
	{
		if (false == m_bTempOff && true == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::HANABI))
		{
			m_bMember = true;

			for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
				iter->second->SetVisible(true);

			dynamic_cast<CCanvas_SASInfoRight*>(CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_SASInfoRight"))->Set_Render();
		}
	}

	CCanvas::Tick(TimeDelta);

	m_pUIMoveFSM->Tick(TimeDelta);
	CCanvas::UIHit(TimeDelta);

	SASRightHp_Tick();
	RendomTexture_Tick(TimeDelta);
	HillBar_Tick(TimeDelta);
}

void CCanvas_SASInfoRightMove::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

	// UITEST
	static _float fHp;
	ImGui::InputFloat("Hp", &fHp);
	static _float fMaxHp;
	ImGui::InputFloat("MaxHp", &fMaxHp);

	if (ImGui::Button("Save Hp"))
	{
		CPlayerInfoManager::GetInstance()->Get_HanabiStat().iHP = fHp;
		CPlayerInfoManager::GetInstance()->Get_HanabiStat().iMaxHP = fMaxHp;
	}
}

void CCanvas_SASInfoRightMove::Set_HillBar()
{
	m_bHill = true;
	m_bHpHill = true;

	dynamic_cast<CSASInfoRightHpBackUI*>(Find_ChildUI(L"SASInfoRight_HpBack0"))->Set_Speed(true);
	dynamic_cast<CSASInfoRightHpBackUI*>(Find_ChildUI(L"SASInfoRight_HpBack1"))->Set_Speed(true);
	dynamic_cast<CSASInfoRightHpBackUI*>(Find_ChildUI(L"SASInfoRight_HpBack2"))->Set_Speed(true);
}

void CCanvas_SASInfoRightMove::SASRightHp_Tick()
{
	if (true == m_bHpHill) return;

	_float fHp = static_cast<_float>(CPlayerInfoManager::GetInstance()->Get_HanabiStat().iHP);
	_float fMaxHP = static_cast<_float>(CPlayerInfoManager::GetInstance()->Get_HanabiStat().iMaxHP);
	m_fHPRatio = fHp / fMaxHP;

	dynamic_cast<CSASInfoRightHpUI*>(Find_ChildUI(L"SASInfoRight_Hp0"))->Set_PlayerHp(m_fHPRatio);
	dynamic_cast<CSASInfoRightHpUI*>(Find_ChildUI(L"SASInfoRight_Hp1"))->Set_PlayerHp(m_fHPRatio);
	dynamic_cast<CSASInfoRightHpUI*>(Find_ChildUI(L"SASInfoRight_Hp2"))->Set_PlayerHp(m_fHPRatio);

	dynamic_cast<CSASInfoRightHpBackUI*>(Find_ChildUI(L"SASInfoRight_HpBack0"))->Set_PlayerHp(m_fHPRatio);
	dynamic_cast<CSASInfoRightHpBackUI*>(Find_ChildUI(L"SASInfoRight_HpBack1"))->Set_PlayerHp(m_fHPRatio);
	dynamic_cast<CSASInfoRightHpBackUI*>(Find_ChildUI(L"SASInfoRight_HpBack2"))->Set_PlayerHp(m_fHPRatio);

	dynamic_cast<CSASInfoRightHpBothEndsUI*>(Find_ChildUI(L"SASInfoRight_EndHp"))->Set_PlayerHp(m_fHPRatio);
	dynamic_cast<CSASInfoRightHpBothEndsUI*>(Find_ChildUI(L"SASInfoRight_EndHpBack"))->Set_PlayerHp(m_fHPRatio);
	dynamic_cast<CSASInfoRightHpBothEndsUI*>(Find_ChildUI(L"SASInfoRight_StartHp"))->Set_PlayerHp(m_fHPRatio);
	dynamic_cast<CSASInfoRightHpBothEndsUI*>(Find_ChildUI(L"SASInfoRight_StartHpBack"))->Set_PlayerHp(m_fHPRatio);
}

void CCanvas_SASInfoRightMove::RendomTexture_Tick(const _double & dTimeDelta)
{
	m_dRendomTexture_TimeAcc += dTimeDelta;
	if (3.0 < m_dRendomTexture_TimeAcc)
		m_dRendomTexture_TimeAcc = 0.0;

	if (0.0 != m_dRendomTexture_TimeAcc)
		return;

	// ü�¿� ���� �������� �̹����� ����ϴ� ������ �޶�����. (m_fHPRatio ����)
	// 0.05~0.95 : 2 / 0.05~0.65 : 1

	_int iCount;
	_int iRendomCount;			// �����̴� Hp �� �׸��� ���� 
	_float fObjectMaxNumber;	// 3���� �����̴� Hp�� �׸��� ��ü
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

	for (_int i = 0; i < iCount; i++) // ü�¿� ���� �ٸ� Count �� �޾ƿ´�.
	{
		_int iRandimArrayNumber = _int(CMathUtils::RandomFloat(0.0f, _float(iCount + 1)));	// ���� ���� �迭�� ��� ���ؼ�
		_int iRandomObjectNumber = _int(CMathUtils::RandomFloat(0.0f, fObjectMaxNumber));	// ���� ��ü ��ȣ�� �ѱ�� ���ؼ�
		arrObjectNumber[iRandimArrayNumber] = iRandomObjectNumber;
	}

	for (_int i = 0; i < _int(fObjectMaxNumber); i++)
	{
		_tchar szChildTag[MAX_PATH] = TEXT("");
		if (-1 == arrObjectNumber[i])
		{
			wsprintf(szChildTag, TEXT("SASInfoRight_Hp%d"), i);
			dynamic_cast<CSASInfoRightHpUI*>(Find_ChildUI(szChildTag))->RendomHpImage(2);

			wsprintf(szChildTag, TEXT("SASInfoRight_HpBack%d"), i);
			dynamic_cast<CSASInfoRightHpBackUI*>(Find_ChildUI(szChildTag))->RendomHpImage(2);
		}
		else
		{
			_int iRandomTexture = _int(CMathUtils::RandomFloat(0.0f, 2.0f)); // 0 or 1

			wsprintf(szChildTag, TEXT("SASInfoRight_Hp%d"), i);
			dynamic_cast<CSASInfoRightHpUI*>(Find_ChildUI(szChildTag))->RendomHpImage(iRandomTexture);

			wsprintf(szChildTag, TEXT("SASInfoRight_HpBack%d"), i);
			dynamic_cast<CSASInfoRightHpBackUI*>(Find_ChildUI(szChildTag))->RendomHpImage(iRandomTexture);
		}
	}
}

void CCanvas_SASInfoRightMove::HillBar_Tick(const _double& TimeDelta)
{
	if (false == m_bHill)	return;

	Find_ChildUI(L"HillBar")->SetVisible(true);

	_float fHp = static_cast<_float>(CPlayerInfoManager::GetInstance()->Get_HanabiStat().iHP);
	_float fMaxHp = static_cast<_float>(CPlayerInfoManager::GetInstance()->Get_HanabiStat().iMaxHP);
	_float fCurrentHill = dynamic_cast<CShaderUI*>(Find_ChildUI(L"HillBar"))->Get_Floats0();

	if (0.2f < fCurrentHill)
	{
		dynamic_cast<CSASInfoRightHpBackUI*>(Find_ChildUI(L"SASInfoRight_HpBack0"))->Set_Speed(false);
		dynamic_cast<CSASInfoRightHpBackUI*>(Find_ChildUI(L"SASInfoRight_HpBack1"))->Set_Speed(false);
		dynamic_cast<CSASInfoRightHpBackUI*>(Find_ChildUI(L"SASInfoRight_HpBack2"))->Set_Speed(false);
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
			if (true == dynamic_cast<CSASInfoRightHpUI*>(Find_ChildUI(L"SASInfoRight_Hp0"))->Get_MaxHp() &&
				true == dynamic_cast<CSASInfoRightHpUI*>(Find_ChildUI(L"SASInfoRight_Hp1"))->Get_MaxHp() &&
				true == dynamic_cast<CSASInfoRightHpUI*>(Find_ChildUI(L"SASInfoRight_Hp2"))->Get_MaxHp())
			{
				m_dMaxHillChake_TimeAcc = 0.0;
				m_bHill = false;
				Find_ChildUI(L"HillBar")->SetVisible(false);
				dynamic_cast<CShaderUI*>(Find_ChildUI(L"HillBar"))->Set_Floats0(0.0f);
			}
		}
	}
}

CCanvas_SASInfoRightMove * CCanvas_SASInfoRightMove::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_SASInfoRightMove*		pInstance = new CCanvas_SASInfoRightMove(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_SASInfoRightMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_SASInfoRightMove::Clone(void * pArg)
{
	CCanvas_SASInfoRightMove*		pInstance = new CCanvas_SASInfoRightMove(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_SASInfoRightMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_SASInfoRightMove::Free()
{
	CCanvas::Free();
}
