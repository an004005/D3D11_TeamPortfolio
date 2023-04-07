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
	// ���콺 ������
	m_pKenisis = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"PsychokinesisGauge_Mouse", PLAYERTEST_LAYER_FRONTUI);
	Safe_AddRef(m_pKenisis);
	Assert(m_pKenisis != nullptr);
	m_pKenisis->Start_NoAttach(this, true, true);
	m_pKenisis->Set_GroupVisible(true);
	m_pKenisis->GetSecondEffect()->GetParams().Floats[0] = 0.0f;
	m_pKenisis->GetThirdEffect()->GetParams().Floats[0] = 0.0f;
	
	// ������ ���� UI
	m_pBanKenisis = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"Psychokinesis_BanMouse", PLAYERTEST_LAYER_FRONTUI);
	Safe_AddRef(m_pBanKenisis);
	Assert(m_pBanKenisis != nullptr);
	m_pBanKenisis->Start_NoAttach(this, true, true);
	m_pBanKenisis->Set_GroupVisible(false);

	// ���� ��
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

	// ��ü ���� �ٴϱ�
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, dynamic_cast<CMapKinetic_Object*>(pKineticObject)->GetPxPostion());

	// ���� �������� ä���
	SetfRatio(m_pPlayer->Get_KineticCharge());
	
	// ������ ���� á�� ���� ó��
	_float fMaxKineticCharge = CPlayerInfoManager::GetInstance()->Get_MaxKineticCharge();
	if (fMaxKineticCharge < m_pPlayer->Get_KineticCharge())	// ���� �������� �� á�� ��
	{
		m_pKenisis->GetFourthEffect()->GetParams().Ints[0] = 0;
	}
	else
	{
		m_pKenisis->GetFourthEffect()->GetParams().Ints[0] = 1;
	}
	
	// ������ ������ �� ���� ǥ��
	_uint iKineticEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iKineticEnergy;
	if (20 > iKineticEnergy)	// ���� ������ ��
	{
		m_pBanKenisis->Set_GroupVisible(true);
		m_pKenisis->Set_GroupVisible(false);
	}
	else
	{
		// ���� �������� ���� �߿��� ���ʿ��� ���� �ʴ´�.
		if (0.01f < m_pPlayer->Get_KineticCharge())
			m_pAppealCircle->Set_GroupVisible(false);
		else
			m_pAppealCircle->Set_GroupVisible(true);

		m_pBanKenisis->Set_GroupVisible(false);
		m_pKenisis->Set_GroupVisible(true);
	}

	//// �� ���� ����϶�� ���� �ϴ� ��

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

	//if (0.0f == m_pPlayer->Get_KineticCharge())	// �鸮�� ���� ���� ����
	//{
	//	m_pAppealCircle->Set_GroupVisible(true);
	//}
	//else	// �鸮�� �ִ� ����
	//{
	//	m_pAppealCircle->Set_GroupVisible(false);
	//}

	//// ���� �������� ���� �ϴٸ�, ���� Icon ���� �����ϰ� return ��Ų��.
	//if (20.0f > CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iKineticEnergy)
	//{
	//	m_pBanKenisis->Set_GroupVisible(true);

	//	m_pKenisis->Set_GroupVisible(false);
	//	m_pAppealCircle->Set_GroupVisible(false);

	//	return;
	//}

	//// ���� ����� ������ �� 
	//m_pKenisis->Set_GroupVisible(true);
	//m_pBanKenisis->Set_GroupVisible(false);

	//SetfRatio(m_pPlayer->Get_KineticCharge());	// ���� �������� ����ϴ� ��ŭ �������� �ö󰣴�. (������� ������ ��������.)
	//if (1.0f <= m_pPlayer->Get_KineticCharge())	// �ִ� 1�ʰ� �Ѿ ��ü�� �������� �����.
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