#include "stdafx.h"
#include "..\public\GravikenisisGUI.h"
#include "GameInstance.h"
#include "EffectSystem.h"
#include "Player.h"
#include "PlayerInfoManager.h"
#include "SpecialObject.h"

CGravikenisisGUI::CGravikenisisGUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CGravikenisisGUI::CGravikenisisGUI(const CGravikenisisGUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CGravikenisisGUI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGravikenisisGUI::Initialize(void * pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CGravikenisisGUI::BeginTick()
{
	// 마우스 게이지
	m_pKenisis = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"PsychokinesisGauge_G", PLAYERTEST_LAYER_FRONTUI);
	Safe_AddRef(m_pKenisis);
	Assert(m_pKenisis != nullptr);
	m_pKenisis->Start_NoAttach(this, true, true);
	m_pKenisis->Set_GroupVisible(true);
	m_pKenisis->GetSecondEffect()->GetParams().Floats[0] = 0.0f;
	m_pKenisis->GetThirdEffect()->GetParams().Floats[0] = 0.0f;

	// 게이지 부족 UI
	m_pBanKenisis = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"Psychokinesis_BanG", PLAYERTEST_LAYER_FRONTUI);
	Safe_AddRef(m_pBanKenisis);
	Assert(m_pBanKenisis != nullptr);
	m_pBanKenisis->Start_NoAttach(this, true, true);
	m_pBanKenisis->Set_GroupVisible(false);

	// 어필 원
	m_pAppealCircle = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"Psychokinesis_Circle", PLAYERTEST_LAYER_FRONTUI);
	Safe_AddRef(m_pAppealCircle);
	Assert(m_pAppealCircle != nullptr);
	m_pAppealCircle->Start_NoAttach(this, true, true);
	m_pAppealCircle->Set_GroupVisible(true);

	// Player
	list<CGameObject*> plsGameObject = CGameInstance::GetInstance()->GetLayer(LEVEL_NOW, L"Layer_Player")->GetGameObjects();

	for (auto iter : plsGameObject)
	{
		if (iter->GetPrototypeTag() == L"Player")
		{
			m_pPlayer = dynamic_cast<CPlayer*>(iter);
			break;
		}
	}
}

void CGravikenisisGUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameObject* pSpecialObject = CPlayerInfoManager::GetInstance()->Get_SpecialObject();
	if (pSpecialObject == nullptr)
	{
		m_pBanKenisis->Set_GroupVisible(false);
		m_pAppealCircle->Set_GroupVisible(false);
		m_pKenisis->Set_GroupVisible(false);
		return;
	}

	// 객체 따라 다니기
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, dynamic_cast<CSpecialObject*>(pSpecialObject)->GetPxPostion());

	// 염력 게이지에 채우기
	SetfRatio(m_pPlayer->Get_KineticCharge());

	// 염력이 가득 찼을 때의 처리
	_float fMaxKineticCharge = CPlayerInfoManager::GetInstance()->Get_MaxKineticCharge();
	if (1.0f < m_pPlayer->Get_KineticCharge())	// 염력 게이지가 다 찼을 때
	{
		m_pKenisis->GetFourthEffect()->GetParams().Ints[0] = 0;
		m_bOnG = true;
	}

	// 염력 게이지가 가득 차게 된 후 리셋하는 과정
	if (true == m_bOnG)
	{
		if(0.0 == m_dReset_TimeAcc)
			m_pAppealCircle->Set_GroupVisible(true);

		m_dReset_TimeAcc += TimeDelta;
		if (3.0 < m_dReset_TimeAcc)
		{
			m_pAppealCircle->Set_GroupVisible(false);
			m_pBanKenisis->Set_GroupVisible(false);
			m_pKenisis->Set_GroupVisible(false);
		}

		if (10.0 < m_dReset_TimeAcc)
		{
			m_dReset_TimeAcc = 0.0;
			m_pKenisis->GetFourthEffect()->GetParams().Ints[0] = 1;
			m_bOnG = false;
		}
		return;
	}

	// 염력이 부족할 때 금지 표시
	_uint iKineticEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iKineticEnergy;
	if (40 > iKineticEnergy)	// 염력 부족할 때
	{
		m_pBanKenisis->Set_GroupVisible(true);
		m_pKenisis->Set_GroupVisible(false);
	}
	else
	{
		// 염력 게이지가 차는 중에는 어필원이 뜨지 않는다.
		if (0.01f < m_pPlayer->Get_KineticCharge())
			m_pAppealCircle->Set_GroupVisible(false);
		else
			m_pAppealCircle->Set_GroupVisible(true);

		m_pBanKenisis->Set_GroupVisible(false);
		m_pKenisis->Set_GroupVisible(true);
	}
}

void CGravikenisisGUI::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	static _float fRatio;
	ImGui::DragFloat("Ratio", &fRatio);
	SetfRatio(fRatio);
}

void CGravikenisisGUI::SetfRatio(const _float & fRatio)
{
	m_pKenisis->GetSecondEffect()->GetParams().Floats[0] = fRatio;
	m_pKenisis->GetThirdEffect()->GetParams().Floats[0] = fRatio;
}

CGravikenisisGUI * CGravikenisisGUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGravikenisisGUI*      pInstance = new CGravikenisisGUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGravikenisisGUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CGravikenisisGUI::Clone(void * pArg)
{
	CGravikenisisGUI*      pInstance = new CGravikenisisGUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGravikenisisGUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGravikenisisGUI::Free()
{
	__super::Free();

	if (m_pKenisis != nullptr && m_pKenisis->IsDeleted() == false)
		m_pKenisis->SetDelete();

	Safe_Release(m_pKenisis);

	if (m_pBanKenisis != nullptr && m_pBanKenisis->IsDeleted() == false)
		m_pBanKenisis->SetDelete();

	Safe_Release(m_pBanKenisis);

	if (m_pAppealCircle != nullptr && m_pAppealCircle->IsDeleted() == false)
		m_pAppealCircle->SetDelete();

	Safe_Release(m_pAppealCircle);
}