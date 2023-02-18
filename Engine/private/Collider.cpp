#include "stdafx.h"
#include "..\public\Collider.h"
#include "DebugDraw.h"
#include "PipeLine.h"
#include "BoundingCapsule.h"
#include "JsonLib.h"
#include "ImguiUtils.h"

const array<const string, CCollider::TYPE_END> CCollider::s_EnumNames{
	"AABB", "OBB", "SPHERE", "CAPSULE"
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
	if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		m_eType = static_cast<CCollider::TYPE>(json["Collider"]["Type"]);
		m_OriginTransformMatrix = json["Collider"]["OriginTransform"];
	}
	else
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
	m_eType = eType;

	Safe_Delete(m_pAABB_Original);
	Safe_Delete(m_pAABB);
	Safe_Delete(m_pOBB_Original);
	Safe_Delete(m_pOBB);
	Safe_Delete(m_pSphere_Original);
	Safe_Delete(m_pSphere);
	Safe_Delete(m_pCapsule_Origin);
	Safe_Delete(m_pCapsule);

	switch (m_eType)
	{
	case CCollider::TYPE_AABB:
		m_pAABB_Original = new BoundingBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f));
		Update_Origin(m_OriginTransformMatrix);
		m_pAABB = new BoundingBox(*m_pAABB_Original);
		break;

	case CCollider::TYPE_OBB:
		m_pOBB_Original = new BoundingOrientedBox();
		Update_Origin(m_OriginTransformMatrix);
		m_pOBB = new BoundingOrientedBox(*m_pOBB_Original);
		break;

	case CCollider::TYPE_SPHERE:
		m_pSphere_Original = new BoundingSphere(_float3(0.f, 0.f, 0.f), 0.5f);
		Update_Origin(m_OriginTransformMatrix);
		m_pSphere = new BoundingSphere(*m_pSphere_Original);
		break;

	case CCollider::TYPE_CAPSULE:
		m_pCapsule_Origin = new CBoundingCapsule(_float3(0.f, 0.5f, 0.f), _float3(0.f, -0.5f, 0.f), 0.5f);
		Update_Origin(m_OriginTransformMatrix);
		m_pCapsule = new CBoundingCapsule(*m_pCapsule_Origin);
		break;
	case TYPE_END:
		FALLTHROUGH
	default: 
		NODEFAULT
	}
}

void CCollider::Update_Origin(_fmatrix TransformMatrix)
{
	m_OriginTransformMatrix = TransformMatrix;

	switch (m_eType)
	{
	case CCollider::TYPE_AABB:
		Assert(m_pAABB_Original != nullptr);
		m_pAABB_Original->Center = _float3(0.f, 0.f, 0.f);
		m_pAABB_Original->Extents = _float3(0.5f, 0.5f, 0.5f);
		m_pAABB_Original->Transform(*m_pAABB_Original, Remove_Rotation(TransformMatrix));		
		break;

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
		m_pCapsule_Origin->m_vTip = _float3(0.f, 0.5f, 0.f);
		m_pCapsule_Origin->m_vBase = _float3(0.f, -0.5f, 0.f);
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
	switch (m_eType)
	{
	case CCollider::TYPE_AABB:
		m_pAABB_Original->Transform(*m_pAABB, Remove_Rotation(TransformMatrix));
		break;

	case CCollider::TYPE_OBB:
		m_pOBB_Original->Transform(*m_pOBB, TransformMatrix);
		break;

	case CCollider::TYPE_SPHERE:
		m_pSphere_Original->Transform(*m_pSphere, TransformMatrix);
		break;

	case CCollider::TYPE_CAPSULE:
		m_pCapsule_Origin->Transform(*m_pCapsule, TransformMatrix);
		break;

	case TYPE_END:
		FALLTHROUGH
	default: 
		NODEFAULT
	}
}

void CCollider::Get2D_AABB_YPlane(_float2& vLeftTop, _float2& vRightBot)
{
	switch (m_eType)
	{
	case CCollider::TYPE_AABB:
		vLeftTop.x = m_pAABB->Center.x - m_pAABB->Extents.x;
		vRightBot.x = m_pAABB->Center.x + m_pAABB->Extents.x;
		vLeftTop.y = m_pAABB->Center.z + m_pAABB->Extents.z;
		vRightBot.y = m_pAABB->Center.z - m_pAABB->Extents.z;
		break;

	case CCollider::TYPE_OBB:
		{
		_float fDiangLength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_pOBB->Extents)));
		vLeftTop.x = m_pOBB->Center.x - fDiangLength;
		vRightBot.x = m_pOBB->Center.x + fDiangLength;
		vLeftTop.y = m_pOBB->Center.z + fDiangLength;
		vRightBot.y = m_pOBB->Center.z - fDiangLength;
		}
		break;

	case CCollider::TYPE_SPHERE:
		vLeftTop.x = m_pSphere->Center.x - m_pSphere->Radius;
		vRightBot.x = m_pSphere->Center.x + m_pSphere->Radius;
		vLeftTop.y = m_pSphere->Center.z + m_pSphere->Radius;
		vRightBot.y = m_pSphere->Center.z - m_pSphere->Radius;
		break;

		// 서있는 캡슐은 항상 서있다고 생각하고 구현(어렵다...)
	case CCollider::TYPE_CAPSULE:
		vLeftTop.x = m_pCapsule->m_vBase.x - m_pCapsule->m_fRadius;
		vRightBot.x = m_pCapsule->m_vBase.x + m_pCapsule->m_fRadius;
		vLeftTop.y = m_pCapsule->m_vBase.z + m_pCapsule->m_fRadius;
		vRightBot.y = m_pCapsule->m_vBase.z - m_pCapsule->m_fRadius;
		break;

	case TYPE_END:
		FALLTHROUGH
	default: 
		NODEFAULT
	}
}

_bool CCollider::Collision(CCollider * pTargetCollider) 
{
	m_bColl = false;

	switch (m_eType)
	{
	case CCollider::TYPE_AABB:
		if (TYPE_AABB == pTargetCollider->m_eType)
			m_bColl = m_pAABB->Intersects(*pTargetCollider->m_pAABB);
		else if (TYPE_OBB == pTargetCollider->m_eType)
			m_bColl = m_pAABB->Intersects(*pTargetCollider->m_pOBB);
		else if (TYPE_SPHERE == pTargetCollider->m_eType)
			m_bColl = m_pAABB->Intersects(*pTargetCollider->m_pSphere);
		break;
	case CCollider::TYPE_OBB:
		if (TYPE_AABB == pTargetCollider->m_eType)
			m_bColl = m_pOBB->Intersects(*pTargetCollider->m_pAABB);
		else if (TYPE_OBB == pTargetCollider->m_eType)
			m_bColl = m_pOBB->Intersects(*pTargetCollider->m_pOBB);
		else if (TYPE_SPHERE == pTargetCollider->m_eType)
			m_bColl = m_pOBB->Intersects(*pTargetCollider->m_pSphere);
		break;
	case CCollider::TYPE_SPHERE:
		if (TYPE_AABB == pTargetCollider->m_eType)
			m_bColl = m_pSphere->Intersects(*pTargetCollider->m_pAABB);
		else if (TYPE_OBB == pTargetCollider->m_eType)
			m_bColl = m_pSphere->Intersects(*pTargetCollider->m_pOBB);
		else if (TYPE_SPHERE == pTargetCollider->m_eType)
			m_bColl = m_pSphere->Intersects(*pTargetCollider->m_pSphere);
		break;
	case CCollider::TYPE_CAPSULE:
		if (pTargetCollider->m_eType == TYPE_CAPSULE)
			m_bColl = m_pCapsule->Intersects(*pTargetCollider->m_pCapsule);
		if (pTargetCollider->m_eType == TYPE_SPHERE)
			m_bColl = m_pCapsule->Intersects(*pTargetCollider->m_pSphere);
		else
		{
			IM_WARN("Capsule can collider only capsule now");
			m_bColl = false;
		}
		break;
	case TYPE_END:
		FALLTHROUGH
	default: 
		NODEFAULT
	}

	return m_bColl;
}

_bool CCollider::Collision(_fvector vOrigin, _fvector vDir, _float& fDistance)
{
	m_bColl = false;

	switch (m_eType)
	{
	case TYPE_AABB:
		m_bColl = m_pAABB->Intersects(vOrigin, vDir, fDistance);
		break;
	case TYPE_OBB:
		m_bColl = m_pOBB->Intersects(vOrigin, vDir, fDistance);
		break;
	case TYPE_SPHERE:
		m_bColl = m_pSphere->Intersects(vOrigin, vDir, fDistance);
		break;
	case TYPE_CAPSULE:
		m_bColl = m_pCapsule->Intersects(vOrigin, vDir, fDistance);
		break;
	case TYPE_END:
		FALLTHROUGH
	default:
		NODEFAULT
	}


	return m_bColl;
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
	case CCollider::TYPE_AABB:
		DX::Draw(m_pBatch, *m_pAABB, XMLoadFloat4(&m_vColor));
		break;
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

_matrix CCollider::Remove_Rotation(_fmatrix TransformMatrix)
{
	_matrix			ResultMatrix = TransformMatrix;

	ResultMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[0]));
	ResultMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[1]));
	ResultMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[2]));

	return ResultMatrix;
}

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

	Safe_Delete(m_pAABB_Original);
	Safe_Delete(m_pAABB);
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

}
