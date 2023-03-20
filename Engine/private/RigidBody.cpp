#include "stdafx.h"
#include "..\public\RigidBody.h"
#include "GameObject.h"
#include "ImguiUtils.h"
#include "PhysX_Manager.h"
#include "Transform.h"
#include "JsonLib.h"

CRigidBody::CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CRigidBody::CRigidBody(const CRigidBody& rhs)
	: CComponent(rhs)
{
}

HRESULT CRigidBody::Initialize(void* pArg)
{
	FAILED_CHECK(CComponent::Initialize(pArg));

	CreateActor();
	return S_OK;
}

void CRigidBody::BeginTick()
{
	CComponent::BeginTick();
	if (auto pOwner = TryGetOwner())
	{
		m_pActor->setGlobalPose(physx::PxTransform{ CPhysXUtils::ToFloat4x4(pOwner->GetTransform()->Get_WorldMatrix()) });
	}

	CPhysX_Manager::GetInstance()->GetScene()->addActor(*m_pActor);
}

void CRigidBody::Imgui_RenderProperty()
{
	CComponent::Imgui_RenderProperty();
	if (ImGui::Button("ReCreateActor(for change shape or scale)"))
	{
		CreateActor();
		if (!IsOnPhysX())
			CPhysX_Manager::GetInstance()->GetScene()->addActor(*m_pActor);
	}

	ImGui::Checkbox("bTrigger", &m_bTrigger);
	ImGui::Checkbox("bKinematic", &m_bKinematic);
	ImGui::InputFloat("Density", &m_fDensity);

	static array<const char*, CT_END> CollTypeNames {
		"Player", "Monster", "PlayerAtk", "MonsterAtk", "PsychickObj", "Trigger4Player", "Trigger4Monster", "Static", "MonsterPart", "MonsterRange", "PlayerRange"
	};
	if (ImGui::BeginCombo("CollType", CollTypeNames[m_eColliderType]))
	{
		for (int i = 0; i < CT_END; ++i)
		{
			const bool bSelected = false;
			if (ImGui::Selectable(CollTypeNames[i], bSelected))
				m_eColliderType = static_cast<ECOLLIDER_TYPE>(i);
		}

		ImGui::EndCombo();
	}

	static array<const char*, TYPE_END> ShapeTypeNames {
		"Box", "Sphere", "Capsule"
	};
	if (ImGui::BeginCombo("ShapeType", ShapeTypeNames[m_eShapeType]))
	{
		for (int i = 0; i < TYPE_END; ++i)
		{
			const bool bSelected = false;
			if (ImGui::Selectable(ShapeTypeNames[i], bSelected))
				m_eShapeType = static_cast<SHAPE>(i);
		}

		ImGui::EndCombo();
	}

	ImGui::Indent( 20.f );
	if (ImGui::CollapsingHeader("Origin Trasnform"))
	{
		static GUIZMO_INFO tInfo;
		CImguiUtils::Render_Guizmo(&m_OriginTransformMatrix, tInfo, true, true);
	}
	ImGui::Unindent( 20.f );

	CImguiUtils::FileDialog_FileSelector("Save RigidBody to", ".json", "../Bin/Resources/Objects/",
	[this](const string& filePath)
	{
		Json json;
		SaveToJson(json);
		std::ofstream file(filePath);
		file << json;
	});

	UpdateChange();
}

void CRigidBody::SaveToJson(Json& json)
{
	CComponent::SaveToJson(json);

	json["RigidBody"]["bKinematic"] = m_bKinematic;
	json["RigidBody"]["bTrigger"] = m_bTrigger;
	json["RigidBody"]["Density"] = m_fDensity;
	json["RigidBody"]["ColliderType"] = m_eColliderType;
	json["RigidBody"]["OriginTransform"] = m_OriginTransformMatrix;
	json["RigidBody"]["ShapeType"] = m_eShapeType;
}

void CRigidBody::LoadFromJson(const Json& json)
{
	if (json.contains("RigidBody") == false)
		return;
	CComponent::LoadFromJson(json);
	m_bKinematic = json["RigidBody"]["bKinematic"];
	m_bTrigger = json["RigidBody"]["bTrigger"];
	m_fDensity = json["RigidBody"]["Density"];
	m_eColliderType = json["RigidBody"]["ColliderType"];
	m_OriginTransformMatrix = json["RigidBody"]["OriginTransform"];
	m_eShapeType = json["RigidBody"]["ShapeType"];
}

void CRigidBody::SetPxWorldMatrix(const _float4x4& WorldMatrix)
{
	m_pActor->setGlobalPose(physx::PxTransform{ CPhysXUtils::ToFloat4x4(WorldMatrix) });
}

_float4x4 CRigidBody::GetPxWorldMatrix()
{
	const physx::PxMat44 shapePose(physx::PxShapeExt::getGlobalPose(*m_pShape, *m_pActor));
	return CPhysXUtils::ToFloat4x4(shapePose);
}

void CRigidBody::Activate(_bool bActive)
{
	if (bActive)
	{
		if (m_pActor->getScene() == nullptr)
			CPhysX_Manager::GetInstance()->AddActor(*m_pActor);
	}
	else
	{
		if (m_pActor->getScene())
			CPhysX_Manager::GetInstance()->RemoveActor(*m_pActor);
	}
}

void CRigidBody::Update_Tick(CTransform* pTransform)
{
	if (m_bTrigger)
	{
		SetPxWorldMatrix(pTransform->Get_WorldMatrix_f4x4());
	}
	else if (m_bKinematic && IsOnPhysX())
	{
		m_pActor->setKinematicTarget(physx::PxTransform{ CPhysXUtils::ToFloat4x4(pTransform->Get_WorldMatrix_f4x4()) });
	}
}

void CRigidBody::Update_Tick(_fmatrix matrix)
{
	if (m_bTrigger)
	{
		SetPxWorldMatrix(matrix);
	}
	else if (m_bKinematic && IsOnPhysX())
	{
		m_pActor->setKinematicTarget(physx::PxTransform{ CPhysXUtils::ToFloat4x4(matrix) });
	}
}

void CRigidBody::Update_AfterPhysX(CTransform* pTransform)
{
	if (IsOnPhysX() == false)
		return;

	if (!m_bKinematic && !m_bTrigger)
	{
		pTransform->Set_WorldMatrix(GetPxWorldMatrix());
	}
}

void CRigidBody::AddForce(_float3 vForce)
{
	if (false == (m_bTrigger && m_bKinematic))
	{
		physx::PxVec3 PxForce = physx::PxVec3(vForce.x, vForce.y, vForce.z);
		m_pActor->addForce(PxForce, physx::PxForceMode::eFORCE);
	}
}

void CRigidBody::AddTorque(_float3 vTorque)
{
	if (false == (m_bTrigger && m_bKinematic))
	{
		physx::PxVec3 PxToque = physx::PxVec3(vTorque.x, vTorque.y, vTorque.z);
		m_pActor->addTorque(PxToque, physx::PxForceMode::eFORCE);
	}
}

void CRigidBody::AddVelocity(_float3 vVelocity)
{
	if (false == (m_bTrigger && m_bKinematic))
	{
		physx::PxVec3 PxForce = physx::PxVec3(vVelocity.x, vVelocity.y, vVelocity.z);
		m_pActor->addForce(PxForce, physx::PxForceMode::eVELOCITY_CHANGE);
	}
}

void CRigidBody::Set_Kinetic(_bool bKinematic)
{
	m_bKinematic = bKinematic;
	CreateActor();
}

void CRigidBody::SetOriginTransform(const _float4x4& OriginMatrix)
{
	m_OriginTransformMatrix = OriginMatrix;
	physx::PxTransform relativePose(CPhysXUtils::ToFloat4x4(m_OriginTransformMatrix));
	m_pShape->setLocalPose(relativePose);
}

void CRigidBody::UpdateChange()
{
	Activate(false);
	m_pActor->detachShape(*m_pShape);
	physx::PxRigidBodyExt::updateMassAndInertia(*m_pActor, m_fDensity);
	SetUpActor();
	m_pActor->attachShape(*m_pShape);
	Activate(true);
}

_float4x4 CRigidBody::Get_OriginMatrix()
{
	return m_OriginTransformMatrix;
}

physx::PxBoxGeometry CRigidBody::Get_BoxGeometry()
{
	if (m_eShapeType != TYPE_BOX)
		return physx::PxBoxGeometry();

	return m_pShape->getGeometry().box();
}

physx::PxSphereGeometry CRigidBody::Get_SphereGeometry()
{
	if (m_eShapeType != TYPE_SPHERE)
		return physx::PxSphereGeometry();

	return m_pShape->getGeometry().sphere();
}

physx::PxCapsuleGeometry CRigidBody::Get_CapsuleGeometry()
{
	if (m_eShapeType != TYPE_CAPSULE)
		return physx::PxCapsuleGeometry();

	return m_pShape->getGeometry().capsule();
}

physx::PxTransform CRigidBody::Get_PxTransform()
{
	return physx::PxShapeExt::getGlobalPose(*m_pShape, *m_pActor);
}

_bool CRigidBody::IsOnPhysX()
{
	return m_pActor != nullptr && m_pActor->getScene() != nullptr;
}

void CRigidBody::ReleaseActor()
{
	if (m_pActor)
	{
		m_pActor->userData = nullptr;
		if (m_pActor->getScene())
		{
			auto pScene = CPhysX_Manager::GetInstance()->GetScene();
			pScene->removeActor(*m_pActor);
		}

		if (m_pShape)
		{
			m_pActor->detachShape(*m_pShape);
			m_pShape->release();
			m_pShape = nullptr;
		}

		m_pActor->release();
		m_pActor = nullptr;
	}
}

void CRigidBody::CreateActor()
{
	ReleaseActor();

	auto pPhysics = CPhysX_Manager::GetInstance()->GetPhysics();
	auto pxMat = CPhysXUtils::ToFloat4x4(_float4x4::Identity);

	if (auto pOwner = TryGetOwner())
		pxMat = CPhysXUtils::ToFloat4x4(pOwner->GetTransform()->Get_WorldMatrix());

	auto pMtrl = CPhysX_Manager::GetInstance()->FindMaterial("NoBounce");
	m_pActor = pPhysics->createRigidDynamic(physx::PxTransform{pxMat});

	_matrix OriginMatrix = m_OriginTransformMatrix;
	_float3 vScale =_float3(XMVectorGetX(XMVector3Length(OriginMatrix.r[0])), 
			XMVectorGetX(XMVector3Length(OriginMatrix.r[1])), 
			XMVectorGetX(XMVector3Length(OriginMatrix.r[2])));

	switch (m_eShapeType)
	{
		case TYPE_BOX:
			m_pShape = pPhysics->createShape(physx::PxBoxGeometry(0.5f * vScale.x, 0.5f * vScale.y, 0.5f * vScale.z), *pMtrl);
			break;
		case TYPE_SPHERE:
			m_pShape = pPhysics->createShape(physx::PxSphereGeometry(0.5f * vScale.x), *pMtrl);
			break;
		case TYPE_CAPSULE:
			m_pShape = pPhysics->createShape(physx::PxCapsuleGeometry(0.5f * vScale.x, 1.f * vScale.y), *pMtrl);
			break;
		case TYPE_END:
			FALLTHROUGH;
		default:
			NODEFAULT;
	}

	SetUpActor();

	m_pActor->attachShape(*m_pShape);
	physx::PxRigidBodyExt::updateMassAndInertia(*m_pActor, m_fDensity);
}

void CRigidBody::SetUpActor()
{
	if (m_bTrigger)
	{
		m_pActor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
	}
	else
	{
		m_pActor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, m_bKinematic);
	}
	m_pActor->userData = this;

	if (m_bTrigger)
	{
		m_pShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
		m_pShape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, false);
		m_pShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
	}
	else
	{
		m_pShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
		m_pShape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
		m_pShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);
	}

	m_pShape->setSimulationFilterData(physx::PxFilterData{ static_cast<physx::PxU32>(m_eColliderType), 0, 0, 0 });
	m_pShape->setQueryFilterData(physx::PxFilterData{static_cast<physx::PxU32>(GetCollTypeBit(m_eColliderType)), 0, 0, 0});

	_matrix RemoveScaleOriginMatrix = m_OriginTransformMatrix;
	RemoveScaleOriginMatrix.r[0] = XMVector3Normalize(RemoveScaleOriginMatrix.r[0]);
	RemoveScaleOriginMatrix.r[1] = XMVector3Normalize(RemoveScaleOriginMatrix.r[1]);
	RemoveScaleOriginMatrix.r[2] = XMVector3Normalize(RemoveScaleOriginMatrix.r[2]);

	physx::PxTransform relativePose(CPhysXUtils::ToFloat4x4(RemoveScaleOriginMatrix));
	m_pShape->setLocalPose(relativePose);
}

void CRigidBody::Free()
{
	CComponent::Free();
	ReleaseActor();
}

CRigidBody* CRigidBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRigidBody*		pInstance = new CRigidBody(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRigidBody");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CRigidBody::Clone(void* pArg)
{
	CRigidBody*		pInstance = new CRigidBody(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CRigidBody");
		Safe_Release(pInstance);
	}
	return pInstance;
}
