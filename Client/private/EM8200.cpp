#include "stdafx.h"
#include "../public/EM8200.h"
#include "FSMComponent.h"
#include "RigidBody.h"
#include "JsonStorage.h"
#include "EM8200_AnimInstance.h"
#include "EM8200_Controller.h"
#include "ImguiUtils.h"
#include "MathUtils.h"
#include "Material.h"
#include "EMCable.h"

CEM8200::CEM8200(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy(pDevice, pContext)
{
	m_eEnemyName = EEnemyName::EM8200;
}

CEM8200::CEM8200(const CEM8200& rhs)
	: CEnemy(rhs)
{
}

HRESULT CEM8200::Initialize(void* pArg)
{
	{
		m_iMaxHP = 10000;
		m_iHP = 10000; // ★
		m_iCrushGauge = 10000;
		m_iMaxCrushGauge = 10000;
		m_bHasCrushGauge = false;

		m_iAtkDamage = 50;
		iEemeyLevel = 2;
	}

	FAILED_CHECK(CEnemy::Initialize(pArg));

	m_eEnemyName = EEnemyName::EM8200;
	m_bHasCrushGauge = false;
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(180.f));

	return S_OK;
}

void CEM8200::SetUpComponents(void* pArg)
{
	CEnemy::SetUpComponents(pArg);
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Prototype_Model_em8200", L"Model", (CComponent**)&m_pModelCom));

	// 컨트롤러, prototype안 만들고 여기서 자체생성하기 위함
	m_pController = CEM8200_Controller::Create();
	m_pController->Initialize(nullptr);
	m_Components.emplace(L"Controller", m_pController);
	Safe_AddRef(m_pController);
	m_pController->SetOwner(this);

	// ASM
	m_pASM = CEM8200_AnimInstance::Create(m_pModelCom, this);

	m_HeavyAttackPushTimeline.SetCurve("Simple_Decrease");
}

void CEM8200::SetUpSound()
{
	CEnemy::SetUpSound();
}

void CEM8200::SetUpAnimationEvent()
{
	CEnemy::SetUpAnimationEvent();
}

void CEM8200::SetUpFSM()
{
	CEnemy::SetUpFSM();

	// 스테이트 짜기 
}

void CEM8200::BeginTick()
{
	CEnemy::BeginTick();
}

void CEM8200::Tick(_double TimeDelta)
{
	CEnemy::Tick(TimeDelta);

	m_pController->SetTarget(m_pTarget);
	if (m_bDead == false)
		m_pController->Tick(TimeDelta);
	else
		m_pController->Invalidate();

	m_bRun = m_pController->IsRun();
	m_fTurnRemain = m_pController->GetTurnRemain();
	m_vMoveAxis = m_pController->GetMoveAxis();
	m_vMoveAxis.Normalize();
	m_eInput = m_pController->GetAIInput();

	m_pFSM->Tick(TimeDelta);
	m_pASM->Tick(TimeDelta);

	const _float fMoveSpeed = m_bRun ? 4.f : 2.f;

	if (m_vMoveAxis.LengthSquared() > 0.f)
	{
		const _float fYaw = m_pTransformCom->GetYaw_Radian();
		_float3 vVelocity;
		XMStoreFloat3(&vVelocity, fMoveSpeed * XMVector3TransformNormal(XMVector3Normalize(m_vMoveAxis), XMMatrixRotationY(fYaw)));
		m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
	}

	// Tick의 제일 마지막에서 실행한다.
	ResetHitData();
}

void CEM8200::Late_Tick(_double TimeDelta)
{
	CEnemy::Late_Tick(TimeDelta);
}

void CEM8200::AfterPhysX()
{
	CEnemy::AfterPhysX();
}

HRESULT CEM8200::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CEM8200::Imgui_RenderProperty()
{
	CEnemy::Imgui_RenderProperty();
	if (ImGui::CollapsingHeader("ASM"))
	{
		m_pASM->Imgui_RenderState();
	}
}

void CEM8200::SetUpUI()
{
	__super::SetUpUI();
}

_bool CEM8200::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("FullBody") == false;
}

void CEM8200::Play_LightHitAnim()
{
}

void CEM8200::Play_MidHitAnim()
{
}

CEM8200* CEM8200::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEM8200* pInstance = new CEM8200(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM8200");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEM8200::Clone(void* pArg)
{
	CEM8200* pInstance = new CEM8200(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM8200");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM8200::Free()
{
	CEnemy::Free();
	Safe_Release(m_pASM);
	Safe_Release(m_pController);
}
