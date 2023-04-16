#include "stdafx.h"
#include "..\public\Canvas_PlayerInfoMove.h"
#include "GameInstance.h"
#include "FSMComponent.h"
#include "UI_Manager.h"
#include "MathUtils.h"
#include "PlayerInfoManager.h"

#include "Player.h"
#include "PlayerInfo_HpUI.h"
#include "PlayerInfo_HpBackUI.h"
#include "PlayerInfo_HpBothEndsUI.h"
#include "PlayerInfo_PsychokinesisUI.h"
#include "PlayerInfo_PsychokinesisBackUI.h"
#include "Canvas_PlayerInfo.h"
#include "ShaderUI.h"

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
	m_vMaxDestination = { 0.0f, -7.0f };
	CCanvas::UIMove_FSM();

	Find_ChildUI(L"PlauerInfo_GaugeArrow")->SetVisible(false);
	Find_ChildUI(L"HillBar")->SetVisible(false);

	Set_PsychokinesisType();
	
	return S_OK;
}

void CCanvas_PlayerInfoMove::BeginTick()
{
	CCanvas::BeginTick();

	PlayerHp_Tick();

}

void CCanvas_PlayerInfoMove::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	m_pUIMoveFSM->Tick(TimeDelta);
	CCanvas::UIHit(TimeDelta);

	RendomTexture_Tick(TimeDelta);	// 계속 Hp 가 출력할 전체 개수, 이미지를 계산한다.
	PlayerHp_Tick();
	PsychokinesisGauge_Tick();
	//Arrow_Move(); // 계속 화살표의 좌표를 변경한다.
	HillBar_Tick(TimeDelta); // 외부에서 힐이 되는지 계속 확인 하다가 힐이 들어오면 이미지를 띄우고 Hp 를 증가 시킨다.
}

void CCanvas_PlayerInfoMove::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_PlayerInfoMove::Render()
{
	if (FAILED(CCanvas::Render()))
		return E_FAIL;

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
		CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iHP = fHp;
		CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iMaxHP = fMaxHp;
	}

	static _int iLevel;
	ImGui::InputInt("iLevel", &iLevel);
	static _int iTyoe;
	ImGui::InputInt("iTyoe", &iTyoe);
	static _float fGauge;
	ImGui::InputFloat("Gauge", &fGauge);
	static _float fMaxGauge;
	ImGui::InputFloat("MaxGauge", &fMaxGauge);

	if (ImGui::Button("Set Gauge"))
	{
		CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iKineticEnergyLevel = iLevel;
		CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iKineticEnergyType = iTyoe;
		CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iKineticEnergy = fGauge;
		CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iMaxKineticEnergy = fMaxGauge;
	}
}

void CCanvas_PlayerInfoMove::Set_HillBar()
{
	m_bHill = true;
	m_bHpHill = true;

	dynamic_cast<CPlayerInfo_HpBackUI*>(Find_ChildUI(L"PlayerInfo_HpBack0"))->Set_Speed(true);
	dynamic_cast<CPlayerInfo_HpBackUI*>(Find_ChildUI(L"PlayerInfo_HpBack1"))->Set_Speed(true);
	dynamic_cast<CPlayerInfo_HpBackUI*>(Find_ChildUI(L"PlayerInfo_HpBack2"))->Set_Speed(true);
	dynamic_cast<CPlayerInfo_HpBackUI*>(Find_ChildUI(L"PlayerInfo_HpBack3"))->Set_Speed(true);
	dynamic_cast<CPlayerInfo_HpBackUI*>(Find_ChildUI(L"PlayerInfo_HpBack4"))->Set_Speed(true);
	dynamic_cast<CPlayerInfo_HpBackUI*>(Find_ChildUI(L"PlayerInfo_HpBack5"))->Set_Speed(true);
}

void CCanvas_PlayerInfoMove::Set_PsychokinesisType()
{
	_uint iLevel = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iKineticEnergyLevel;

	
	dynamic_cast<CPlayerInfo_PsychokinesisUI*>(Find_ChildUI(L"PlayerInfo_Psychokinesis"))->Set_Gauge(0, 1.0f);
	dynamic_cast<CPlayerInfo_PsychokinesisUI*>(Find_ChildUI(L"PlayerInfo_Psychokinesis"))->Set_Type(iLevel);
	dynamic_cast<CPlayerInfo_PsychokinesisBackUI*>(Find_ChildUI(L"PlayerInfo_PsychokinesisBack"))->Set_Gauge(0, 1.0f);
	dynamic_cast<CPlayerInfo_PsychokinesisBackUI*>(Find_ChildUI(L"PlayerInfo_PsychokinesisBack"))->Set_Type(iLevel);
	dynamic_cast<CCanvas_PlayerInfo*>(CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_PlayerInfo"))->Set_Type(iLevel);
}

void CCanvas_PlayerInfoMove::PlayerHp_Tick()
{
	if (true == m_bHpHill) return;

	_uint iHp = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iHP;
	_uint iMaxHp = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iMaxHP;
	_float fRatio = _float(iHp) / iMaxHp;
	m_fPercentageHp = (1.0f < fRatio) ? 1.0f : fRatio;

	dynamic_cast<CPlayerInfo_HpUI*>(Find_ChildUI(L"PlayerInfo_Hp0"))->Set_PlayerHp(m_fPercentageHp);
	dynamic_cast<CPlayerInfo_HpUI*>(Find_ChildUI(L"PlayerInfo_Hp1"))->Set_PlayerHp(m_fPercentageHp);
	dynamic_cast<CPlayerInfo_HpUI*>(Find_ChildUI(L"PlayerInfo_Hp2"))->Set_PlayerHp(m_fPercentageHp);
	dynamic_cast<CPlayerInfo_HpUI*>(Find_ChildUI(L"PlayerInfo_Hp3"))->Set_PlayerHp(m_fPercentageHp);
	dynamic_cast<CPlayerInfo_HpUI*>(Find_ChildUI(L"PlayerInfo_Hp4"))->Set_PlayerHp(m_fPercentageHp);
	dynamic_cast<CPlayerInfo_HpUI*>(Find_ChildUI(L"PlayerInfo_Hp5"))->Set_PlayerHp(m_fPercentageHp);

	dynamic_cast<CPlayerInfo_HpBackUI*>(Find_ChildUI(L"PlayerInfo_HpBack0"))->Set_PlayerHp(m_fPercentageHp);
	dynamic_cast<CPlayerInfo_HpBackUI*>(Find_ChildUI(L"PlayerInfo_HpBack1"))->Set_PlayerHp(m_fPercentageHp);
	dynamic_cast<CPlayerInfo_HpBackUI*>(Find_ChildUI(L"PlayerInfo_HpBack2"))->Set_PlayerHp(m_fPercentageHp);
	dynamic_cast<CPlayerInfo_HpBackUI*>(Find_ChildUI(L"PlayerInfo_HpBack3"))->Set_PlayerHp(m_fPercentageHp);
	dynamic_cast<CPlayerInfo_HpBackUI*>(Find_ChildUI(L"PlayerInfo_HpBack4"))->Set_PlayerHp(m_fPercentageHp);
	dynamic_cast<CPlayerInfo_HpBackUI*>(Find_ChildUI(L"PlayerInfo_HpBack5"))->Set_PlayerHp(m_fPercentageHp);

	dynamic_cast<CPlayerInfo_HpBothEndsUI*>(Find_ChildUI(L"PlayerInfo_EndHp"))->Set_PlayerHp(m_fPercentageHp);
	dynamic_cast<CPlayerInfo_HpBothEndsUI*>(Find_ChildUI(L"PlayerInfo_EndHpBack"))->Set_PlayerHp(m_fPercentageHp);
	dynamic_cast<CPlayerInfo_HpBothEndsUI*>(Find_ChildUI(L"PlayerInfo_StartHp"))->Set_PlayerHp(m_fPercentageHp);
	dynamic_cast<CPlayerInfo_HpBothEndsUI*>(Find_ChildUI(L"PlayerInfo_StartHpBack"))->Set_PlayerHp(m_fPercentageHp);
}

void CCanvas_PlayerInfoMove::RendomTexture_Tick(const _double& dTimeDelta)
{
	m_dRendomTexture_TimeAcc += dTimeDelta;
	if (3.0 < m_dRendomTexture_TimeAcc)
		m_dRendomTexture_TimeAcc = 0.0;

	if (0.0 != m_dRendomTexture_TimeAcc)
		return;

	// 체력에 따라서 랜덤으로 이미지를 출력하는 개수가 달라진다. (m_fPercentageHp 기준)
	// 0.05~0.95 : 3 / 0.05~0.65 : 2 / 0.05~0.35 : 1

	_int iCount;
	_int iRendomCount;			// 움직이는 Hp 를 그리는 개수 
	_float fObjectMaxNumber;	// 6개중 움직이는 Hp를 그리는 객체
	if (0.95f < m_fPercentageHp)
	{
		iCount = 3;
		fObjectMaxNumber = 6.0f;
		iRendomCount = _int(CMathUtils::RandomFloat(0.0f, 4.0f));
	}
	else if (0.65f < m_fPercentageHp)
	{
		iCount = 2;
		fObjectMaxNumber = 5.0f;
		iRendomCount = _int(CMathUtils::RandomFloat(0.0f, 3.0f));
	}
	else if (0.35f < m_fPercentageHp)
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

void CCanvas_PlayerInfoMove::Arrow_Move()
{
	// 염력 게이지의 x 좌표 가져와서 화살표 에게 넘겨준다. 
	//_float2 vGauge_Position = dynamic_cast<CPlayerInfo_PsychokinesisUI*>(Find_ChildUI(L"PlayerInfo_Psychokinesis"))->Gauge_Position();


	//_float fCurHp = Clamp<_float>(m_fCurrentHp, 0.0f, 0.05f);
	//m_tParams.Floats[0] = Remap<float>(fCurHp, 0.0f, 0.05f, 0.0f, 1.0f);

	//_float2 vGauge_Position = Find_ChildUI(L"PlauerInfo_GaugeArrow")->Get_Position();
	//_float fPosition = Clamp<_float>(vGauge_Position.x, -138.0f, 138.0f);
	//_float fResult = Remap<_float>(fPosition, -138.0f, 138.0f, 0.0f, 1.0f);

	//m_fPercentageHp;

	//_float2 vGauge_Position = Find_ChildUI(L"PlayerInfo_Psychokinesis")->Get_Position();
	//Find_ChildUI(L"PlauerInfo_GaugeArrow")->Set_Position(_float2(vTemp.x, vGauge_Position.y + 15.0f));
}

void CCanvas_PlayerInfoMove::HillBar_Tick(const _double& TimeDelta)
{
	if (false == m_bHill)	return;

	Find_ChildUI(L"HillBar")->SetVisible(true);

	_float fHp = _float(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iHP);
	_float fMaxHp = _float(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iMaxHP);
	_float fCurrentHill = dynamic_cast<CShaderUI*>(Find_ChildUI(L"HillBar"))->Get_Floats0();

	if (0.2f < fCurrentHill)
	{
		dynamic_cast<CPlayerInfo_HpBackUI*>(Find_ChildUI(L"PlayerInfo_HpBack0"))->Set_Speed(false);
		dynamic_cast<CPlayerInfo_HpBackUI*>(Find_ChildUI(L"PlayerInfo_HpBack1"))->Set_Speed(false);
		dynamic_cast<CPlayerInfo_HpBackUI*>(Find_ChildUI(L"PlayerInfo_HpBack2"))->Set_Speed(false);
		dynamic_cast<CPlayerInfo_HpBackUI*>(Find_ChildUI(L"PlayerInfo_HpBack3"))->Set_Speed(false);
		dynamic_cast<CPlayerInfo_HpBackUI*>(Find_ChildUI(L"PlayerInfo_HpBack4"))->Set_Speed(false);
		dynamic_cast<CPlayerInfo_HpBackUI*>(Find_ChildUI(L"PlayerInfo_HpBack5"))->Set_Speed(false);
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
			if (true == dynamic_cast<CPlayerInfo_HpUI*>(Find_ChildUI(L"PlayerInfo_Hp0"))->Get_MaxHp() &&
				true == dynamic_cast<CPlayerInfo_HpUI*>(Find_ChildUI(L"PlayerInfo_Hp1"))->Get_MaxHp() &&
				true == dynamic_cast<CPlayerInfo_HpUI*>(Find_ChildUI(L"PlayerInfo_Hp2"))->Get_MaxHp() &&
				true == dynamic_cast<CPlayerInfo_HpUI*>(Find_ChildUI(L"PlayerInfo_Hp3"))->Get_MaxHp() &&
				true == dynamic_cast<CPlayerInfo_HpUI*>(Find_ChildUI(L"PlayerInfo_Hp4"))->Get_MaxHp() &&
				true == dynamic_cast<CPlayerInfo_HpUI*>(Find_ChildUI(L"PlayerInfo_Hp5"))->Get_MaxHp())
			{
				m_dMaxHillChake_TimeAcc = 0.0;
				m_bHill = false;
				Find_ChildUI(L"HillBar")->SetVisible(false);
				dynamic_cast<CShaderUI*>(Find_ChildUI(L"HillBar"))->Set_Floats0(0.0f);
			}
		}
	}
}

void CCanvas_PlayerInfoMove::PsychokinesisGauge_Tick()
{
	_float fRatio = _float(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iKineticEnergy / _float(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iMaxKineticEnergy));
	m_fPsychokinesisGauge = (1.0f < fRatio) ? 1.0f : fRatio;

	PSYCHOKINESISTYPE eType = PSYCHOKINESISTYPE(CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iKineticEnergyType);

	dynamic_cast<CPlayerInfo_PsychokinesisUI*>(Find_ChildUI(L"PlayerInfo_Psychokinesis"))->Set_Gauge(_uint(eType), fRatio);
	dynamic_cast<CPlayerInfo_PsychokinesisBackUI*>(Find_ChildUI(L"PlayerInfo_PsychokinesisBack"))->Set_Gauge(_uint(eType), fRatio);
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
