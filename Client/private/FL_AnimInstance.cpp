#include "stdafx.h"
#include "FL_AnimInstance.h"
#include "FlowerLeg.h"
#include "Model.h"
#include "Animation.h"

HRESULT CFL_AnimInstance::Initialize(CModel * pModel, CGameObject * pGameObject)
{
	FAILED_CHECK(CAnimationInstance::Initialize(pModel, pGameObject));

	{
		m_pASM_Base = CASMBuilder()
			.InitState("Idle")
			.AddState("Idle")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0200_102_AL_wait02"))

				.AddTransition("Idle to Walk", "Walk")
					.Predicator([&]()->_bool {return !m_bIdle && m_bWalk; })
					.Duration(0.2f)

				.AddTransition("Idle to JumpStart", "JumpStart")
					.Predicator([&]()->_bool {return !m_bIdle && m_bAir; })
					.Duration(0.2f)

			.AddState("Walk")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0200_106_AL_walk02"))
				
				.AddTransition("Walk to Idle", "Idle")
					.Predicator([&]()->_bool {return !m_bWalk && m_bIdle; })
					.Duration(0.2f)

				.AddTransition("Walk to JumpStart", "JumpStart")
					.Predicator([&]()->_bool {return !m_bWalk && m_bAir; })
					.Duration(0.2f)

			.AddState("JumpStart")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0200_135_AL_dodge_B_start"))
				
				.AddTransition("JumpStart to JumpEnd", "JumpEnd")
					.Predicator([&]()->_bool {return m_bAir; })	// End를 위한 값 받기
					.Duration(0.2f)

			.AddState("JumpEnd")

			.Build();
	}
	
	m_pASM_Base->SetCurState("Idle");
	
	// Socket

	return S_OK;
}

void CFL_AnimInstance::Tick(_double TimeDelta)
{
	UpdateTargetState(TimeDelta);

	// Socket에 대한 정의

	_bool bLocalMove = true;

	m_pASM_Base->Tick(TimeDelta);
	m_pModel->SetCurAnimName(m_pASM_Base->GetCurState()->m_Animation->GetName());

	m_pModel->Compute_CombindTransformationMatrix();

	if (bLocalMove)
	{
		_matrix WorldMatrix = m_pTargetObject->GetTransform()->Get_WorldMatrix();
		m_pTargetObject->GetTransform()->LocalMove(m_pModel->GetLocalMove(WorldMatrix));
	}
}

void CFL_AnimInstance::UpdateTargetState(_double TimeDelta)
{
	CFlowerLeg* pFlowerLeg = static_cast<CFlowerLeg*>(m_pTargetObject);


}

void CFL_AnimInstance::Imgui_RenderState()
{
}

void CFL_AnimInstance::InputAnimSocket(const string & strSocName, list<CAnimation*> AnimList)
{
	m_mapAnimSocket[strSocName] = (AnimList);
}

CFL_AnimInstance * CFL_AnimInstance::Create(CModel * pModel, CGameObject * pGameObject)
{
	CFL_AnimInstance*		pInstance = new CFL_AnimInstance();

	if (FAILED(pInstance->Initialize(pModel, pGameObject)))
	{
		MSG_BOX("Failed to Created : CFL_AnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFL_AnimInstance::Free()
{
	__super::Free();
	Safe_Release(m_pASM_Base);
}
