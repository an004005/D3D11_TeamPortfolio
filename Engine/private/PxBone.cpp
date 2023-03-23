#include "stdafx.h"
#include "..\public\PxBone.h"
#include "PhysX_Manager.h"


HRESULT CPxBone::Initialize(const Json& jBone, CBone* pParent)
{
	FAILED_CHECK(CBone::Initialize(jBone, pParent));

	return S_OK;
}

void CPxBone::CreateJointsRecur(physx::PxRigidDynamic* pParentActor, const _float4x4& PivotMatrix,
	const _float4x4& WorldMatrix)
{
	_matrix BoneMatrix = XMLoadFloat4x4(&m_CombindTransformMatrix) * XMLoadFloat4x4(&PivotMatrix)  * XMLoadFloat4x4(&WorldMatrix);

	_matrix NoScaleBoneMatrix = BoneMatrix;
	NoScaleBoneMatrix.r[0] = XMVector3Normalize(NoScaleBoneMatrix.r[0]);
	NoScaleBoneMatrix.r[1] = XMVector3Normalize(NoScaleBoneMatrix.r[1]);
	NoScaleBoneMatrix.r[2] = XMVector3Normalize(NoScaleBoneMatrix.r[2]);

	auto pPhysics = CPhysX_Manager::GetInstance()->GetPhysics();
	auto pMtrl = CPhysX_Manager::GetInstance()->FindMaterial("SmallBounce");
	m_pActor = pPhysics->createRigidDynamic(physx::PxTransform{ CPhysXUtils::ToFloat4x4(_float4x4::Identity) });
	m_pActor->setGlobalPose(physx::PxTransform{ CPhysXUtils::ToFloat4x4(NoScaleBoneMatrix) });
	physx::PxRigidBodyExt::updateMassAndInertia(*m_pActor, 10.f);


	if (pParentActor)
	{
		auto ParentWorld = pParentActor->getGlobalPose();
		const _vector vParentPos = CPhysXUtils::ToFloat4x4(physx::PxMat44{ ParentWorld }).Translation();
		const _vector vPos = BoneMatrix.r[3];
		_float fDistanceHalf = XMVectorGetX(XMVector3Length(vParentPos - vPos)) * 0.5f;
		if (fDistanceHalf < 0.00001f)
		{
			fDistanceHalf = 0.01f;
		}

		physx::PxShape*	pShape = pPhysics->createShape(physx::PxSphereGeometry(fDistanceHalf * 0.5f), *pMtrl);
		pShape->setSimulationFilterData(physx::PxFilterData{ static_cast<physx::PxU32>(CT_STATIC), 0, 0, 0 });
		m_pActor->attachShape(*pShape);
		pShape->release();

		_float4 vCenter = XMVectorSetW((vParentPos + vPos) * 0.5f, 1.f);

		auto LocalFrame0 = physx::PxTransform{ ParentWorld.transformInv(physx::PxVec3{ vCenter.x, vCenter.y, vCenter.z }) };
		auto LocalFrame1 = physx::PxTransform{ m_pActor->getGlobalPose().transformInv(physx::PxVec3{ vCenter.x, vCenter.y, vCenter.z }) };


		if (m_strName == "cable_02_a" || m_strName == "cable_02_b" || m_strName == "cable_02_c")
		{
			auto joint = physx::PxFixedJointCreate(*pPhysics, pParentActor, LocalFrame0, m_pActor, LocalFrame1);
			joint->setConstraintFlag(physx::PxConstraintFlag::ePROJECTION, true);
			joint->setProjectionLinearTolerance(0.01f);
		}
		else
		{
			auto joint = physx::PxD6JointCreate(*pPhysics, pParentActor, LocalFrame0, m_pActor, LocalFrame1);
			joint->setConstraintFlag(physx::PxConstraintFlag::ePROJECTION, true);
			joint->setProjectionLinearTolerance(0.01f);
			// joint->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eLIMITED);
			// joint->setTwistLimit(physx::PxJointAngularLimitPair(-XMConvertToRadians(20.f), XMConvertToRadians(20.f), 0.01f));

			joint->setMotion(physx::PxD6Axis::eSWING1, physx::PxD6Motion::eLIMITED);
			joint->setMotion(physx::PxD6Axis::eSWING2, physx::PxD6Motion::eLIMITED);
			joint->setSwingLimit(physx::PxJointLimitCone(XMConvertToRadians(30.f), XMConvertToRadians(30.f), 0.01f));

			joint->setDistanceLimit(physx::PxJointLinearLimit(0.f, physx::PxSpring(1.f, 1.f)));
		}
	}
	else
	{
		physx::PxShape*	pShape = pPhysics->createShape(physx::PxSphereGeometry(0.001f), *pMtrl);
		pShape->setSimulationFilterData(physx::PxFilterData{ static_cast<physx::PxU32>(CT_TRIGGER_FOR_MONSTER), 0, 0, 0 });
		m_pActor->attachShape(*pShape);
		pShape->release();

		m_pActor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
	}


	CPhysX_Manager::GetInstance()->AddActor(*m_pActor);

	for (auto pChild : m_Children)
	{
		static_cast<CPxBone*>(pChild)->CreateJointsRecur(m_pActor, PivotMatrix, WorldMatrix);
	}
}

void CPxBone::ReleaseJointsRecur()
{
	ReleaseActor();
	for (auto pChild : m_Children)
	{
		static_cast<CPxBone*>(pChild)->ReleaseJointsRecur();
	}
}

_matrix CPxBone::GetCombindMatrix_WithPivot(_fmatrix WorldMatrixInv)
{
	_matrix BoneMatrix;
	if (m_pActor && (m_pActor->getRigidBodyFlags() & physx::PxRigidBodyFlag::eKINEMATIC) == false)
		BoneMatrix = CPhysXUtils::ToFloat4x4(m_pActor->getGlobalPose());
	else
		BoneMatrix = m_NotControlledWorld;

	return BoneMatrix * WorldMatrixInv;
}

void CPxBone::SetWorldMatrix(const _float4x4& WorldMatrix, const _float4x4& PivotMatrix)
{
	m_NotControlledWorld = XMLoadFloat4x4(&m_CombindTransformMatrix) * XMLoadFloat4x4(&PivotMatrix) * XMLoadFloat4x4(&WorldMatrix);
	m_pActor->setKinematicTarget(physx::PxTransform{ CPhysXUtils::ToFloat4x4(m_NotControlledWorld) });
}

void CPxBone::ReleaseActor()
{
	if (m_pActor)
	{
		if (m_pActor->getScene())
		{
			auto pScene = CPhysX_Manager::GetInstance()->GetScene();
			pScene->removeActor(*m_pActor);
		}

		m_pActor->release();
		m_pActor = nullptr;
	}
}

CPxBone* CPxBone::Create(const Json& jBone, CBone* pParent)
{
	CPxBone*		pInstance = new CPxBone();

	if (FAILED(pInstance->Initialize(jBone, pParent)))
	{
		MSG_BOX("Failed to Created : CPxBone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPxBone::Free()
{
	CBone::Free();
	ReleaseActor();
}
