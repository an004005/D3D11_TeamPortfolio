#include "stdafx.h"
#include "..\public\SuperSpeedTrail.h"
#include <GameInstance.h>
#include <GameObject.h>

#include "Model.h"
#include "Material.h"


CSuperSpeedTrail::CSuperSpeedTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CSuperSpeedTrail::CSuperSpeedTrail(const CSuperSpeedTrail& rhs)
	: CComponent(rhs)
{
}

void CSuperSpeedTrail::BeginTick()
{
	__super::BeginTick();
	m_TrailGenerateTimer.Initialize(m_fTrailCoolTime, false);
	m_vPrePos = TryGetOwner()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
}

void CSuperSpeedTrail::Tick(_double TimeDelta)
{
	//if (m_bActive == false)
	//{
	//	for (auto& pTrail : m_pGhostTrails)
	//		Safe_Release(pTrail.pPoseModel);
	//	m_pGhostTrails.clear();
	//	return;
	//}


	if (m_pOwnerModel == nullptr)
		return;

	if (m_bActive)
		m_TrailGenerateTimer.Tick(TimeDelta);

	m_pGhostTrails.remove_if([this](const GHOST_TRAIL_INFO& tInfo)
	{
		return tInfo.fLife <= 0.f;
	});

	for (auto& tTrailInfo : m_pGhostTrails)
	{
		tTrailInfo.fLife -= (_float)TimeDelta;
		if (tTrailInfo.fLife <= 0.f)
			Safe_Release(tTrailInfo.pPoseModel);
		else
		{
			_float fLifeRatio = tTrailInfo.fLife / m_fTrailLife;
			CGameInstance::GetInstance()->LambdaRenderRequest(tTrailInfo.WorldMatrix, [tTrailInfo, fLifeRatio]()
			{
				for (auto pMtrl : tTrailInfo.pPoseModel->GetMaterials())
				{
					pMtrl->GetParam().Floats[0] = fLifeRatio;
				}
				tTrailInfo.pPoseModel->Render_NoUpdateBone(tTrailInfo.WorldMatrix);
			}, CRenderer::RENDER_ALPHABLEND);
		}
	}


	auto pOwner = TryGetOwner();
	Assert(pOwner != nullptr);

	_vector vOwnerPos = pOwner->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	_float fDistance = XMVectorGetX(XMVector3LengthEst(vOwnerPos - XMLoadFloat4(&m_vPrePos)));
	if (fDistance < m_fMinMoveDistance)
		return;

	m_vPrePos = vOwnerPos;

	if (m_TrailGenerateTimer.Use())
	{
		GHOST_TRAIL_INFO tInfo;
		_matrix WorldMatrix = pOwner->GetTransform()->Get_WorldMatrix();

		_float3 vScale{XMVectorGetX(XMVector3LengthEst(WorldMatrix.r[0])),
			XMVectorGetX(XMVector3LengthEst(WorldMatrix.r[1])),
			XMVectorGetX(XMVector3LengthEst(WorldMatrix.r[2]))};
		WorldMatrix.r[0] = XMVector3Normalize(WorldMatrix.r[0]) * vScale.x * 0.97f;
		WorldMatrix.r[1] = XMVector3Normalize(WorldMatrix.r[1]) * vScale.y * 0.97f;
		WorldMatrix.r[2] = XMVector3Normalize(WorldMatrix.r[2]) * vScale.z * 0.97f;

		tInfo.WorldMatrix = WorldMatrix;
		tInfo.fLife = m_fTrailLife;
		tInfo.pPoseModel = m_pOwnerModel->CloneModelPose();

		const _uint iSuperSpeedPass = 8; // VtxAnimModel, VtxToonAnimModel ¸ðµÎ 8¹ø
		for (auto pMtrl : tInfo.pPoseModel->GetMaterials())
		{
			pMtrl->GetParam().iPass = iSuperSpeedPass;
			if (pMtrl->GetParam().Floats.empty())
				pMtrl->GetParam().Floats.push_back(0.f);
		}

		m_pGhostTrails.push_back(tInfo);
	}
}

void CSuperSpeedTrail::SetOwnerModel(CModel* pModel)
{
	Safe_Release(m_pOwnerModel);
	m_pOwnerModel = pModel;
	Safe_AddRef(m_pOwnerModel);
}

void CSuperSpeedTrail::SetActive(_bool bActive)
{
	m_bActive = bActive;
}

CSuperSpeedTrail* CSuperSpeedTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
 	CSuperSpeedTrail*		pInstance = new CSuperSpeedTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSuperSpeedTrail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CSuperSpeedTrail::Clone(void* pArg)
{
	CSuperSpeedTrail*		pInstance = new CSuperSpeedTrail(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSuperSpeedTrail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSuperSpeedTrail::Free()
{
	__super::Free();
	Safe_Release(m_pOwnerModel);
	for (auto& pTrail : m_pGhostTrails)
	{
		Safe_Release(pTrail.pPoseModel);
	}
	m_pGhostTrails.clear();
}
