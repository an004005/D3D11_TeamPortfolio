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
	// ���콺 ������
	m_pKenisis = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"PsychokinesisGauge_Mouse", TEXT("Layer_UI"));
	Safe_AddRef(m_pKenisis);
	Assert(m_pKenisis != nullptr);
	m_pKenisis->Start_NoAttach(m_pOwner, true);

	m_pKenisis->Set_GroupVisible(true);
	m_pKenisis->GetSecondEffect()->GetParams().Floats[0] = 0.0f;
	m_pKenisis->GetThirdEffect()->GetParams().Floats[0] = 0.0f;


	// ������ ���� UI
	m_pBanKenisis = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"Psychokinesis_BanMouse", TEXT("Layer_UI"));
	Safe_AddRef(m_pBanKenisis);
	Assert(m_pBanKenisis != nullptr);
	m_pBanKenisis->Start_NoAttach(m_pOwner, true);

	m_pBanKenisis->Set_GroupVisible(false);

	// ���� ��_1
	m_pAppealCircle_0 = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"Psychokinesis_Circle0", TEXT("Layer_UI"));
	Safe_AddRef(m_pAppealCircle_0);
	Assert(m_pAppealCircle_0 != nullptr);
	m_pAppealCircle_0->Start_NoAttach(m_pOwner, true);

	m_pAppealCircle_0->Set_GroupVisible(false);
	// ���� ��_2
	m_pAppealCircle_1 = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"Psychokinesis_Circle1", TEXT("Layer_UI"));
	Safe_AddRef(m_pAppealCircle_1);
	Assert(m_pAppealCircle_1 != nullptr);
	m_pAppealCircle_1->Start_NoAttach(m_pOwner, true);

	m_pAppealCircle_1->Set_GroupVisible(false);

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
	
	// �� ���� ����϶�� ���� �ϴ� ��
	if (true == dynamic_cast<CMapKinetic_Object*>(m_pOwner)->Get_CameRange())
	{
		if (0.0f == m_pPlayer->Get_KineticCharge())	// �鸮�� ���� ���� ����
		{
			m_pAppealCircle_0->Set_GroupVisible(true);
			m_pAppealCircle_1->Set_GroupVisible(true);
		}
		else	// �鸮�� �ִ� ����
		{
			m_pAppealCircle_0->Set_GroupVisible(false);
			m_pAppealCircle_1->Set_GroupVisible(false);
		}
	}

	// Get_IsTargeted �÷��̾�� ��ȣ�ۿ��� ���۵� ������Ʈ
	if (false == dynamic_cast<CMapKinetic_Object*>(m_pOwner)->Get_IsTargeted())	
		return;
		
	// ���� �������� ���� �ϴٸ�, ���� Icon ���� �����ϰ� return ��Ų��.
	if (20.0f > m_pPlayer->Get_PlayerStat().m_iKineticEnergy)
	{
		m_pBanKenisis->Set_GroupVisible(true);
		
		m_pKenisis->Set_GroupVisible(false);
		m_pAppealCircle_0->Set_GroupVisible(false);
		m_pAppealCircle_1->Set_GroupVisible(false);

		return;
	}

	// ���� ����� ������ �� 
	m_pBanKenisis->Set_GroupVisible(false);

	m_pKenisis->Set_GroupVisible(true);

	SetfRatio(m_pPlayer->Get_KineticCharge());	// ���� �������� ����ϴ� ��ŭ �������� �ö󰣴�. (������� ������ ��������.)
	if (1.0f <= m_pPlayer->Get_KineticCharge())	// �ִ� 1�ʰ� �Ѿ ��ü�� �������� �����.
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