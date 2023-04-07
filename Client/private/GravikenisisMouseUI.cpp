#include "stdafx.h"
#include "..\public\GravikenisisMouseUI.h"
#include "GameInstance.h"
#include "EffectSystem.h"
#include "Player.h"
#include "PlayerInfoManager.h"
#include "MapKinetic_Object.h"

CGravikenisisMouseUI::CGravikenisisMouseUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CGravikenisisMouseUI::CGravikenisisMouseUI(const CGravikenisisMouseUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CGravikenisisMouseUI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGravikenisisMouseUI::Initialize(void * pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CGravikenisisMouseUI::BeginTick()
{
	//m_pKenisis->Start_NoAttach(m_pOwner, true);
	// 마우스 게이지
	m_pKenisis = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"PsychokinesisGauge_Mouse", PLAYERTEST_LAYER_FRONTUI);
	Safe_AddRef(m_pKenisis);
	Assert(m_pKenisis != nullptr);
	m_pKenisis->Start_NoAttach(this, true, true);
	m_pKenisis->Set_GroupVisible(true);
	m_pKenisis->GetSecondEffect()->GetParams().Floats[0] = 0.0f;
	m_pKenisis->GetThirdEffect()->GetParams().Floats[0] = 0.0f;
	
	// 게이지 부족 UI
	m_pBanKenisis = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"Psychokinesis_BanMouse", PLAYERTEST_LAYER_FRONTUI);
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

void CGravikenisisMouseUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
 	CGameObject* pKineticObject = CPlayerInfoManager::GetInstance()->Get_KineticObject();
	if (pKineticObject == nullptr)
	{
		m_pBanKenisis->Set_GroupVisible(false);
		m_pAppealCircle->Set_GroupVisible(false);
		m_pKenisis->Set_GroupVisible(false);
		return;
	}

	// 객체 따라 다니기
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, dynamic_cast<CMapKinetic_Object*>(pKineticObject)->GetPxPostion());

	// 염력 게이지에 채우기
	SetfRatio(m_pPlayer->Get_KineticCharge());
	
	// 염력이 가득 찼을 때의 처리
	_float fMaxKineticCharge = CPlayerInfoManager::GetInstance()->Get_MaxKineticCharge();
	if (fMaxKineticCharge < m_pPlayer->Get_KineticCharge())	// 염력 게이지가 다 찼을 때
	{
		m_pKenisis->GetFourthEffect()->GetParams().Ints[0] = 0;
	}
	else
	{
		m_pKenisis->GetFourthEffect()->GetParams().Ints[0] = 1;
	}
	
	// 염력이 부족할 때 금지 표시
	_uint iKineticEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iKineticEnergy;
	if (20 > iKineticEnergy)	// 염력 부족할 때
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

	//// 나 염력 사용하라고 어필 하는 원

	//CGameObject* pGameObject = m_pOwner;
	//CMapKinetic_Object* kinetic = dynamic_cast<CMapKinetic_Object*>(m_pOwner);

	//_bool CameRange = kinetic->Get_CameRange();

	//if (false == dynamic_cast<CMapKinetic_Object*>(m_pOwner)->Get_CameRange())
	//{
	//	m_pKenisis->Set_GroupVisible(false);
	//	m_pBanKenisis->Set_GroupVisible(false);
	//	m_pAppealCircle->Set_GroupVisible(false);
	//	
	//	return;
	//}

	//if (0.0f == m_pPlayer->Get_KineticCharge())	// 들리고 있지 않은 상태
	//{
	//	m_pAppealCircle->Set_GroupVisible(true);
	//}
	//else	// 들리고 있는 상태
	//{
	//	m_pAppealCircle->Set_GroupVisible(false);
	//}

	//// 염력 게이지가 부족 하다면, 금지 Icon 으로 변경하고 return 시킨다.
	//if (20.0f > CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iKineticEnergy)
	//{
	//	m_pBanKenisis->Set_GroupVisible(true);

	//	m_pKenisis->Set_GroupVisible(false);
	//	m_pAppealCircle->Set_GroupVisible(false);

	//	return;
	//}

	//// 염력 사용이 가능할 때 
	//m_pKenisis->Set_GroupVisible(true);
	//m_pBanKenisis->Set_GroupVisible(false);

	//SetfRatio(m_pPlayer->Get_KineticCharge());	// 염력 게이지를 사용하는 만큼 게이지가 올라간다. (사용하지 않으면 내려간다.)
	//if (1.0f <= m_pPlayer->Get_KineticCharge())	// 최대 1초가 넘어간 객체는 게이지를 지운다.
	//{
	//	m_bDelete = true;
	//	return;
	//}
}

void CGravikenisisMouseUI::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	static _float fRatio;
	ImGui::DragFloat("Ratio", &fRatio);
	SetfRatio(fRatio);
}

void CGravikenisisMouseUI::SetfRatio(const _float & fRatio)
{
	m_pKenisis->GetSecondEffect()->GetParams().Floats[0] = fRatio;
	m_pKenisis->GetThirdEffect()->GetParams().Floats[0] = fRatio;
}

CGravikenisisMouseUI * CGravikenisisMouseUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGravikenisisMouseUI*      pInstance = new CGravikenisisMouseUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGravikenisisMouseUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CGravikenisisMouseUI::Clone(void * pArg)
{
	CGravikenisisMouseUI*      pInstance = new CGravikenisisMouseUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGravikenisisMouseUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGravikenisisMouseUI::Free()
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