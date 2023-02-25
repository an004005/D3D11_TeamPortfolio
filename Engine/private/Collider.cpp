#include "stdafx.h"
#include "..\public\Collider.h"
#include "DebugDraw.h"
#include "PipeLine.h"
#include "BoundingCapsule.h"
#include "JsonLib.h"
#include "ImguiUtils.h"
#include "PhysX_Manager.h"

const array<const string, CCollider::TYPE_END> CCollider::s_EnumNames{
	"OBB", "SPHERE", "CAPSULE"
};

CCollider::CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
#ifdef _DEBUG
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif // _DEBUG
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif // _DEBUG

}

HRESULT CCollider::Initialize_Prototype()
{
#ifdef _DEBUG

	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);

	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void*		pShaderByteCode;
	size_t			iShaderByteCodeSize;

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeSize);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeSize, &m_pInputLayout)))
		return E_FAIL;

#endif

	return S_OK;
}

HRESULT CCollider::Initialize(void * pArg)
{
	__super::Initialize(pArg);
	if (!pArg)
	{
		m_eType = CCollider::TYPE_SPHERE;
		m_OriginTransformMatrix = _float4x4::Identity;
	}

	ChangeType(m_eType);
	return S_OK;
}

void CCollider::SaveToJson(Json& json)
{
	json["Collider"]["Type"] = static_cast<_uint>(m_eType);
	json["Collider"]["OriginTransform"] = m_OriginTransformMatrix;
}

void CCollider::LoadFromJson(const Json& json)
{
	CComponent::LoadFromJson(json);
	m_eType = static_cast<CCollider::TYPE>(json["Collider"]["Type"]);
	m_OriginTransformMatrix = json["Collider"]["OriginTransform"];
}

void CCollider::Imgui_RenderProperty()
{
	static GUIZMO_INFO tInfo;
	_float4x4 OriginMatrix = m_OriginTransformMatrix;
	CImguiUtils::Render_Guizmo(&OriginMatrix, tInfo, true, true);

	Update_Origin(OriginMatrix);

	CImguiUtils::FileDialog_FileSelector("Save this Collider As File", ".json", "..\\Bin\\Resources\\", 
		[this](const string& fileName)
		{
			Json json = Json::object();
			SaveToJson(json);
			std::ofstream file(fileName);
			file << json;
		});
}

void CCollider::ChangeType(CCollider::TYPE eType)
{
	if (m_pActor)
	{
		if (m_pShape)
		{
			m_pActor->detachShape(*m_pShape);
			m_pShape->release();
			m_pShape = nullptr;
		}

		if (m_pActor->getScene())
		{
			auto pScene = CPhysX_Manager::GetInstance()->GetScene();
			pScene->removeActor(*m_pActor);
		}
		m_pActor->release();
		m_pActor = nullptr;
	}

	m_eType = eType;

	Safe_Delete(m_pOBB_Original);
	Safe_Delete(m_pOBB);
	Safe_Delete(m_pSphere_Original);
	Safe_Delete(m_pSphere);
	Safe_Delete(m_pCapsule_Origin);
	Safe_Delete(m_pCapsule);


	auto pPhysics = CPhysX_Manager::GetInstance()->GetPhysics();
	auto pMtrl = CPhysX_Manager::GetInstance()->FindMaterial("Default");

	auto pxMat = CPhysXUtils::ToFloat4x4(m_OriginTransformMatrix);
	m_pActor = pPhysics->createRigidDynamic(physx::PxTransform{pxMat});

	switch (m_eType)
	{
	case CCollider::TYPE_OBB:
		m_pOBB_Original = new BoundingOrientedBox();
		Update_Origin(m_OriginTransformMatrix);
		m_pOBB = new BoundingOrientedBox(*m_pOBB_Original);

		{
			const _float3 vExtents = m_pOBB->Extents;
			m_pShape = pPhysics->createShape(physx::PxBoxGeometry(vExtents.x, vExtents.y, vExtents.z), *pMtrl);
		}
		break;

	case CCollider::TYPE_SPHERE:
		m_pSphere_Original = new BoundingSphere(_float3(0.f, 0.f, 0.f), 0.5f);
		Update_Origin(m_OriginTransformMatrix);
		m_pSphere = new BoundingSphere(*m_pSphere_Original);

		m_pShape = pPhysics->createShape(physx::PxSphereGeometry(m_pSphere->Radius), *pMtrl);

		break;

	case CCollider::TYPE_CAPSULE:
		m_pCapsule_Origin = new CBoundingCapsule(_float3(1.f, 0.f, 0.f), _float3(-1.f, 0.f, 0.f), 0.5f);
		Update_Origin(m_OriginTransformMatrix);
		m_pCapsule = new CBoundingCapsule(*m_pCapsule_Origin);

		// m_pShape = pPhysics->createShape(physx::PxCapsuleGeometry(m_pCapsule->m_fRadius, m_pCapsule->GetHalfHeight()), *pMtrl);

		{
			m_pShape = pPhysics->createShape(physx::PxCapsuleGeometry(m_pCapsule->m_fRadius, m_pCapsule_Origin->GetHalfHeight()), *pMtrl);
			physx::PxTransform relativePose(physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0,0,1)));
			m_pShape->setLocalPose(relativePose);
			// physx::PxRigidBodyExt::updateMassAndInertia(*m_pActor, capsuleDensity);
		}

		break;

	case TYPE_END:
		FALLTHROUGH
	default:
		NODEFAULT
	}



	m_pActor->attachShape(*m_pShape);
	physx::PxRigidBodyExt::updateMassAndInertia(*m_pActor, 100.0f);
	CPhysX_Manager::GetInstance()->GetScene()->addActor(*m_pActor);
	
}

void CCollider::Update_Origin(_fmatrix TransformMatrix)
{
	m_OriginTransformMatrix = TransformMatrix;

	switch (m_eType)
	{
	case CCollider::TYPE_OBB:
		{
			Assert(m_pOBB_Original != nullptr);
			_float3 vScale = { XMVectorGetX(XMVector3Length(TransformMatrix.r[0])), XMVectorGetX(XMVector3Length(TransformMatrix.r[1])), XMVectorGetX(XMVector3Length(TransformMatrix.r[2])) };

			m_pOBB_Original->Center = _float3(0.f, 0.f, 0.f);
			m_pOBB_Original->Extents = _float3(vScale.x * 0.5f, vScale.y * 0.5f, vScale.z * 0.5f);
			m_pOBB_Original->Orientation = _float4(0.f, 0.f, 0.f, 1.f);

			_matrix RemoveScale = TransformMatrix;
			RemoveScale.r[0] = XMVector3Normalize(RemoveScale.r[0]);
			RemoveScale.r[1] = XMVector3Normalize(RemoveScale.r[1]);
			RemoveScale.r[2] = XMVector3Normalize(RemoveScale.r[2]);
			m_pOBB_Original->Transform(*m_pOBB_Original, RemoveScale);
		}
		break;

	case CCollider::TYPE_SPHERE:
		Assert(m_pSphere_Original != nullptr);
		m_pSphere_Original->Center = _float3(0.f, 0.f, 0.f);
		m_pSphere_Original->Radius = 0.5f;
		m_pSphere_Original->Transform(*m_pSphere_Original, TransformMatrix);
		break;

	case CCollider::TYPE_CAPSULE:
		Assert(m_pCapsule_Origin != nullptr);
		m_pCapsule_Origin->m_vTip = _float3(1.f, 0.f, 0.f);
		m_pCapsule_Origin->m_vBase = _float3(-1.f, 0.f, 0.f);
		m_pCapsule_Origin->m_fRadius = 0.5f;
		m_pCapsule_Origin->Transform(*m_pCapsule_Origin, TransformMatrix);
		break;

	case TYPE_END:
		FALLTHROUGH
	default: 
		NODEFAULT
	}
}

void CCollider::Update(_fmatrix TransformMatrix)
{
	const physx::PxMat44 shapePose(physx::PxShapeExt::getGlobalPose(*m_pShape, *m_pActor));

	// _float4x4 mw;
	// mw(0,0) = shapePose(0,0); mw(0,1) = shapePose(0,1); mw(0,2) = shapePose(0,2); mw(0,3) = shapePose(0,3);
	// mw(1,0) = shapePose(1,0); mw(1,1) = shapePose(1,1); mw(1,2) = shapePose(1,2); mw(1,3) = shapePose(1,3);
	// mw(2,0) = shapePose(2,0); mw(2,1) = shapePose(2,1); mw(2,2) = shapePose(2,2); mw(2,3) = shapePose(2,3);
	// mw(3,0) = shapePose(3,0); mw(3,1) = shapePose(3,1); mw(3,2) = shapePose(3,2); mw(3,3) = shapePose(3,3);

	XMMATRIX mWorld = CPhysXUtils::ToFloat4x4(shapePose);

	switch (m_eType)
	{
	case CCollider::TYPE_OBB:
		m_pOBB_Original->Transform(*m_pOBB, mWorld);
		break;

	case CCollider::TYPE_SPHERE:
		m_pSphere_Original->Transform(*m_pSphere, mWorld);
		break;

	case CCollider::TYPE_CAPSULE:
		m_pCapsule_Origin->Transform(*m_pCapsule, mWorld);
		break;

	case TYPE_END:
		FALLTHROUGH
	default: 
		NODEFAULT
	}
}

#ifdef _DEBUG
HRESULT CCollider::Render()
{
	m_vColor = m_bColl == true ? _float4(1.f, 0.f, 0.f, 1.f) : _float4(0.f, 1.f, 0.f, 1.f);

	m_pEffect->SetWorld(XMMatrixIdentity());

	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	m_pEffect->SetView(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

	RELEASE_INSTANCE(CPipeLine);

	m_pEffect->Apply(m_pContext);

	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pBatch->Begin();

	switch (m_eType)
	{
	case CCollider::TYPE_OBB:
		DX::Draw(m_pBatch, *m_pOBB, XMLoadFloat4(&m_vColor));
		break;
	case CCollider::TYPE_SPHERE:
		DX::Draw(m_pBatch, *m_pSphere, XMLoadFloat4(&m_vColor));
		break;
	case CCollider::TYPE_CAPSULE:
		DX::Draw(m_pBatch, *m_pCapsule, XMLoadFloat4(&m_vColor));
		break;
	case TYPE_END:
		FALLTHROUGH
	default: 
		NODEFAULT
	}

	m_pBatch->End();

	return S_OK;
}
#endif // _DEBUG

CCollider * CCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCollider*		pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CCollider::Clone(void * pArg)
{
	CCollider*		pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

	Safe_Delete(m_pOBB_Original);
	Safe_Delete(m_pOBB);
	Safe_Delete(m_pSphere_Original);
	Safe_Delete(m_pSphere);
	Safe_Delete(m_pCapsule_Origin);
	Safe_Delete(m_pCapsule);

#ifdef _DEBUG

	Safe_Release(m_pInputLayout);

	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
#endif // _DEBUG

	if (m_pActor)
	{
		if (m_pShape)
		{
			m_pActor->detachShape(*m_pShape);
			m_pShape->release();
		}

		if (m_pActor->getScene())
		{
			auto pScene = CPhysX_Manager::GetInstance()->GetScene();
			pScene->removeActor(*m_pActor);
		}
		m_pActor->release();
	}

}
