#include "stdafx.h"
#include "EM0220_AnimInstance.h"
#include "Model.h"
#include "EM0220.h"

HRESULT CEM0220_AnimInstance::Initialize(CModel * pModel, CGameObject * pGameObject)
{
	FAILED_CHECK(__super::Initialize(pModel, pGameObject));

	m_pASM_Base = CASMBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0200_102_AL_wait02"))

		.Build();
	
	m_mapAnimSocket.insert({ "FullBody",{} });
	return S_OK;
}

void CEM0220_AnimInstance::UpdateTargetState(_double TimeDelta)
{
}


CEM0220_AnimInstance * CEM0220_AnimInstance::Create(CModel * pModel, CGameObject * pGameObject)
{
	CEM0220_AnimInstance*		pInstance = new CEM0220_AnimInstance();

	if (FAILED(pInstance->Initialize(pModel, pGameObject)))
	{
		MSG_BOX("Failed to Created : CEM0220_AnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM0220_AnimInstance::Free()
{
	__super::Free();
	Safe_Release(m_pASM_Base);
}
