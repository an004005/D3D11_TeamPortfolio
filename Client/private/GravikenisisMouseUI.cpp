#include "stdafx.h"
#include "..\public\GravikenisisMouseUI.h"
#include "GameInstance.h"
#include "VFX_Manager.h"
#include "EffectSystem.h"
#include "MapKinetic_Object.h"
#include "Player.h"

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
	// 마우스 게이지
	m_pKenisis = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"PsychokinesisGauge_Mouse", TEXT("Layer_UI"));
	Safe_AddRef(m_pKenisis);
	Assert(m_pKenisis != nullptr);

	m_pKenisis->Start_NoAttach(m_pOwner, true);

	m_pKenisis->GetSecondEffect()->GetParams().Floats[0] = 0.0f;
	m_pKenisis->GetThirdEffect()->GetParams().Floats[0] = 0.0f;

	//// 게이지 부족 UI
	//m_pBanKenisis = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"Psychokinesis_BanMouse", TEXT("Layer_UI"));
	//Safe_AddRef(m_pBanKenisis);
	//Assert(m_pBanKenisis != nullptr);

	//m_pBanKenisis->Start_NoAttach(m_pOwner, true);

	//m_pBanKenisis->GetSecondEffect()->SetVisible(false);

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

	if (m_pKenisis == nullptr || m_pOwner == nullptr)
		return;

	if (false == dynamic_cast<CMapKinetic_Object*>(m_pOwner)->Get_IsTargeted())
		return;
		
	// 염력 게이지가 부족 하다면, 
	if (20.0f > m_pPlayer->Get_PlayerStat().m_iKineticEnergy)
	{
		m_pKenisis->GetFirstEffect()->SetVisible(false);
		m_pKenisis->GetSecondEffect()->SetVisible(false);
		m_pKenisis->GetThirdEffect()->SetVisible(false);

		//m_pBanKenisis->GetFirstEffect()->SetVisible(true);
		//m_pBanKenisis->GetSecondEffect()->SetVisible(true);

		return;
	}
	else
	{
		SetfRatio(m_pPlayer->Get_KineticCharge());

		m_pKenisis->GetFirstEffect()->SetVisible(true);
		m_pKenisis->GetSecondEffect()->SetVisible(true);
		m_pKenisis->GetThirdEffect()->SetVisible(true);

		//m_pBanKenisis->GetFirstEffect()->SetVisible(false);
		//m_pBanKenisis->GetSecondEffect()->SetVisible(false);
	}

	if (1.0f <= m_pPlayer->Get_KineticCharge())
	{
		m_bDelete = true;
		return;
	}
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
}