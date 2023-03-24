#include "stdafx.h"
#include "EM0700_AnimInstance.h"
#include "Model.h"
#include "EM0700.h"

HRESULT CEM0700_AnimInstance::Initialize(CModel * pModel, CGameObject * pGameObject)
{
	FAILED_CHECK(__super::Initialize(pModel, pGameObject));

	m_pASM_Base = CASMBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0700_107_AL_fly_wait"))

			.AddTransition("Idle to Walk", "Walk")
				.Predicator([this] { return m_bMove; })
				.Duration(0.2f)

		.AddState("Walk")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0700_112_AL_walk_L"))

		.AddTransition("Walk to Idle", "Idle")
			.Predicator([this] { return !m_bMove; })
			.Duration(0.2f)
		.Build();

	m_mapAnimSocket.insert({ "FullBody",{} });
	return S_OK;
}

void CEM0700_AnimInstance::UpdateTargetState(_double TimeDelta)
{
}

CEM0700_AnimInstance * CEM0700_AnimInstance::Create(CModel * pModel, CGameObject * pGameObject)
{
	CEM0700_AnimInstance*		pInstance = new CEM0700_AnimInstance();

	if (FAILED(pInstance->Initialize(pModel, pGameObject)))
	{
		MSG_BOX("Failed to Created : CEM0700_AnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM0700_AnimInstance::Free()
{
	__super::Free();
	Safe_Release(m_pASM_Base);
}
