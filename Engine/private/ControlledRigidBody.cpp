#include "stdafx.h"
#include "..\public\ControlledRigidBody.h"
#include "PhysX_Manager.h"
#include "JsonLib.h"

using namespace physx;

class CEngineControllerHitReport : public physx::PxUserControllerHitReport
{
public:
	virtual ~CEngineControllerHitReport() = default;
	virtual void onShapeHit(const physx::PxControllerShapeHit& hit) override;
	virtual void onControllerHit(const physx::PxControllersHit& hit) override{}
	virtual void onObstacleHit(const physx::PxControllerObstacleHit& hit) override{}
} g_EngineControllerHitReport;

CControlledRigidBody::CControlledRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CControlledRigidBody::CControlledRigidBody(const CControlledRigidBody& rhs)
	: CComponent(rhs)

{
}

HRESULT CControlledRigidBody::Initialize(void* pArg)
{
	FAILED_CHECK(CComponent::Initialize(pArg));
	m_tDesc.reportCallback = &g_EngineControllerHitReport;
	m_tDesc.material = CPhysX_Manager::GetInstance()->FindMaterial("Default");
	m_tDesc.upDirection = { 0.f, 1.f, 0.f };

	if (pArg == nullptr)
	{
		m_tDesc.radius = 0.5f;
		m_tDesc.height = 1.f;
		m_tDesc.contactOffset = 0.1f;
		m_tDesc.density = 100.f;
		m_tDesc.slopeLimit = cosf(XMConvertToRadians(45.f));
		m_tDesc.stepOffset = 0.1f;
		m_tDesc.maxJumpHeight = 3.f;
	}

	// 이 컨트롤러와 충돌하는 타입 선택(컨트롤러의 이동은 scene query기반)
	m_MoveFilterData.word0 = CTB_PLAYER | CTB_MONSTER | CTB_PSYCHICK_OBJ | CTB_STATIC;
	m_Filters.mFilterData = &m_MoveFilterData;

	CreateController();

	return S_OK;
}

void CControlledRigidBody::Imgui_RenderProperty()
{
	CComponent::Imgui_RenderProperty();

	static array<const char*, CT_MONSTER + 1> CollTypeNames {
		"Player", "Monster"
	};
	if (ImGui::BeginCombo("CollType", CollTypeNames[m_eColliderType]))
	{
		for (int i = 0; i < CT_MONSTER + 1; ++i)
		{
			const bool bSelected = false;
			if (ImGui::Selectable(CollTypeNames[i], bSelected))
				m_eColliderType = static_cast<ECOLLIDER_TYPE>(i);
		}

		ImGui::EndCombo();
	}

	if (ImGui::Button("Update Changes"))
	{
		CreateController();
	}
}

void CControlledRigidBody::SaveToJson(Json& json)
{
	CComponent::SaveToJson(json);
	json["ControlledRigidBody"]["ColliderType"] = m_eColliderType;
	json["ControlledRigidBody"]["radius"] = m_tDesc.radius;
	json["ControlledRigidBody"]["height"] = m_tDesc.height;
	json["ControlledRigidBody"]["contactOffset"] = m_tDesc.contactOffset;
	json["ControlledRigidBody"]["density"] = m_tDesc.density;
	json["ControlledRigidBody"]["slopeLimit"] = m_tDesc.slopeLimit;
	json["ControlledRigidBody"]["stepOffset"] = m_tDesc.stepOffset;
	json["ControlledRigidBody"]["maxJumpHeight"] = m_tDesc.maxJumpHeight;

}

void CControlledRigidBody::LoadFromJson(const Json& json)
{
	if (json.contains("ControlledRigidBody") == false)
		return;
	CComponent::LoadFromJson(json);
	m_eColliderType = json["ControlledRigidBody"]["ColliderType"];
	m_tDesc.radius = json["ControlledRigidBody"]["radius"];
	m_tDesc.height = json["ControlledRigidBody"]["height"];
	m_tDesc.contactOffset = json["ControlledRigidBody"]["contactOffset"];
	m_tDesc.density = json["ControlledRigidBody"]["density"];
	m_tDesc.slopeLimit = json["ControlledRigidBody"]["slopeLimit"];
	m_tDesc.stepOffset = json["ControlledRigidBody"]["stepOffset"];
	m_tDesc.maxJumpHeight = json["ControlledRigidBody"]["maxJumpHeight"];
}

void CControlledRigidBody::SetPosition(const _float4& vPos)
{
	m_pController->setPosition({(_double)vPos.x, (_double)vPos.y, (_double)vPos.z});
}

_float4 CControlledRigidBody::GetPosition()
{
	const auto vPos = m_pController->getPosition();
	return _float4{(_float)vPos.x, (_float)vPos.y, (_float)vPos.z, 1.f};
}

_float4 CControlledRigidBody::GetFootPosition()
{
	const auto vPos = m_pController->getFootPosition();
	return _float4{(_float)vPos.x, (_float)vPos.y, (_float)vPos.z, 1.f};
}

void CControlledRigidBody::Move(_float4 vVelocity, _float fTimeDelta, _float minDist)
{
	// disp : direction * speed * delta(delta 시간 동안의 이동량)
	vVelocity *= fTimeDelta;

	const physx::PxVec3 vDisp{vVelocity.x, vVelocity.y, vVelocity.z};
	m_pController->move(vDisp, minDist, fTimeDelta, m_Filters);
}

void CControlledRigidBody::MoveDisp(_float4 vPosDelta, _float fTimeDelta, _float minDist)
{
	const physx::PxVec3 vDisp{vPosDelta.x, vPosDelta.y, vPosDelta.z};
	m_pController->move(vDisp, minDist, fTimeDelta, m_Filters);
}

void CControlledRigidBody::CreateController()
{
	ReleaseController();

	m_pController = CPhysX_Manager::GetInstance()->GetControllerManager()->createController(m_tDesc);

	PxShape* shape;
	m_pController->getActor()->getShapes(&shape, 1);
	shape->setSimulationFilterData(physx::PxFilterData{ static_cast<physx::PxU32>(m_eColliderType), 0, 0, 0 });
	shape->setQueryFilterData(physx::PxFilterData{static_cast<physx::PxU32>(GetCollTypeBit(m_eColliderType)), 0, 0, 0});
	m_pController->getActor()->userData = this;

	CPhysX_Manager::GetInstance()->AddActor(*m_pController->getActor());
}

void CControlledRigidBody::ReleaseController()
{
	if (m_pController)
	{
		CPhysX_Manager::GetInstance()->RemoveActor(*m_pController->getActor());
		m_pController->release();
	}
}

void CControlledRigidBody::Free()
{
	CComponent::Free();
	ReleaseController();
}

CControlledRigidBody* CControlledRigidBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CControlledRigidBody*		pInstance = new CControlledRigidBody(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CControlledRigidBody");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CControlledRigidBody::Clone(void* pArg)
{
	CControlledRigidBody*		pInstance = new CControlledRigidBody(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CControlledRigidBody");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CEngineControllerHitReport::onShapeHit(const physx::PxControllerShapeHit& hit)
{
	PxRigidDynamic* actor = hit.actor->is<PxRigidDynamic>();
	if(actor)
	{
		if(actor->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC)
			return;
		const PxVec3 upVector = hit.controller->getUpDirection();
		const PxF32 dp = hit.dir.dot(upVector);
//		shdfnd::printFormatted("%f\n", fabsf(dp));
		if(fabsf(dp)<1e-3f)
//		if(hit.dir.y==0.0f)
		{
			const PxTransform globalPose = actor->getGlobalPose();
			const PxVec3 localPos = globalPose.transformInv(toVec3(hit.worldPos));
			CPhysXUtils::AddForceAtLocalPos(*actor, hit.dir*hit.length*200.0f, localPos, PxForceMode::eACCELERATION);
		}
	}
}