#include "stdafx.h"
#include "..\public\PhysXDynamicModel.h"

#include "GameUtils.h"
#include "PhysX_Manager.h"
#include "Transform.h"

using namespace physx;

_float4x4 CPhysXDynamicModel::s_DefaultPivot = _float4x4::CreateScale({ 0.01f, 0.01f, 0.01f }) *_float4x4::CreateRotationY(XMConvertToRadians(-180.f));

CPhysXDynamicModel::CPhysXDynamicModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CPhysXDynamicModel::CPhysXDynamicModel(const CPhysXDynamicModel& rhs)
	: CComponent(rhs)
	, m_PxMeshes(rhs.m_PxMeshes)
{
}

HRESULT CPhysXDynamicModel::Initialize_Prototype(const char* pModelFilePath, _float4x4 PivotMatrix)
{
	CComponent::Initialize_Prototype();
	Assert(true == CGameUtils::CheckExt(pModelFilePath, { ".static_model" }));

	_tchar szFilePath[MAX_PATH];
	CGameUtils::c2wc(pModelFilePath, szFilePath);
	HANDLE hFile = CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		IM_ERROR("fail to open file %s", pModelFilePath);
		return E_FAIL;
	}

	DWORD dwByte = 0;
	string strName = CGameUtils::ReadStrFile(hFile); /* Read */

	_uint iNumMeshes = 0;
	ReadFile(hFile, &iNumMeshes, sizeof(_uint), &dwByte, nullptr); /* Read */

	const _matrix _PivotMatrix = PivotMatrix;
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		vector<_float3> Vertices;
		vector<FACEINDICES32> Indices;

		_uint iMaterialIndex;
		_uint iNumVertices;
		_uint iNumPrimitive;

		strName = CGameUtils::ReadStrFile(hFile); /* Read */
		ReadFile(hFile, &iMaterialIndex, sizeof(_uint), &dwByte, nullptr); /* Read */
		ReadFile(hFile, &iNumVertices, sizeof(_uint), &dwByte, nullptr); /* Read */
		ReadFile(hFile, &iNumPrimitive, sizeof(_uint), &dwByte, nullptr); /* Read */

		for (_uint i = 0; i < iNumVertices; ++i)
		{
			VTXMODEL buffer;
			ReadFile(hFile, &buffer, sizeof(VTXMODEL), &dwByte, nullptr); /* Read */

			XMStoreFloat3(&buffer.vPosition, XMVector3TransformCoord(XMLoadFloat3(&buffer.vPosition), _PivotMatrix));
			// XMStoreFloat3(&buffer.vNormal, XMVector3TransformNormal(XMLoadFloat3(&buffer.vNormal), _PivotMatrix));
			
			Vertices.push_back(buffer.vPosition);
		}

		for (_uint i = 0; i < iNumPrimitive; ++i)
		{
			FACEINDICES32 buffer;
			ReadFile(hFile, &buffer, sizeof(FACEINDICES32), &dwByte, nullptr); /* Read */
			Indices.push_back(buffer);
		}

		PxMesh pxMesh;

		PxTriangleMeshDesc meshDesc;
		meshDesc.points.count           = iNumVertices;
		meshDesc.points.stride          = sizeof(_float3);
		meshDesc.points.data            = Vertices.data();

		meshDesc.triangles.count        = iNumPrimitive;
		meshDesc.triangles.stride       = sizeof(FACEINDICES32);
		meshDesc.triangles.data         = Indices.data();

		auto pCooking = CPhysX_Manager::GetInstance()->GetCooking();
		auto pPhysics = CPhysX_Manager::GetInstance()->GetPhysics();
		auto pMtrl = CPhysX_Manager::GetInstance()->FindMaterial("Default");

		PxDefaultMemoryOutputStream writeBuffer;
		PxTriangleMeshCookingResult::Enum result;
		bool status = pCooking->cookTriangleMesh(meshDesc, writeBuffer, &result);
		if(!status)
		{
			IM_ERROR("Cooking %s Failed", pModelFilePath);
			CloseHandle(hFile);
			Assert(0);
		}

		switch (result)
		{
		case PxTriangleMeshCookingResult::eSUCCESS:
			break;
		case PxTriangleMeshCookingResult::eLARGE_TRIANGLE:
			IM_WARN("Cooking %s is Large", pModelFilePath);
			break;
		case PxTriangleMeshCookingResult::eFAILURE: 
			IM_ERROR("Cooking %s Failed", pModelFilePath);
			CloseHandle(hFile);
			Assert(0);
			break;
		default: ;
		}

		PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
		pxMesh.pPxMesh = pPhysics->createTriangleMesh(readBuffer);
		pxMesh.tMeshGeom = PxTriangleMeshGeometry(pxMesh.pPxMesh);
		pxMesh.pShape = pPhysics->createShape(pxMesh.tMeshGeom, *pMtrl);
		
		pxMesh.pShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
		pxMesh.pShape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
		pxMesh.pShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);
		pxMesh.pShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);

		pxMesh.pShape->setSimulationFilterData(physx::PxFilterData{ static_cast<physx::PxU32>(ECOLLIDER_TYPE::CT_PSYCHICK_OBJ), 0, 0, 0 });
		pxMesh.pShape->setQueryFilterData(physx::PxFilterData{static_cast<physx::PxU32>(GetCollTypeBit(ECOLLIDER_TYPE::CT_PSYCHICK_OBJ)), 0, 0, 0});

		m_PxMeshes.push_back(pxMesh);
	}


	CloseHandle(hFile);
	return S_OK;
}

HRESULT CPhysXDynamicModel::Initialize(void* pArg)
{
	FAILED_CHECK(CComponent::Initialize(pArg));

	auto pPhysics = CPhysX_Manager::GetInstance()->GetPhysics();

	auto pxMat = CPhysXUtils::ToFloat4x4(_float4x4::Identity);
	m_pActor = pPhysics->createRigidDynamic(physx::PxTransform{ pxMat });
	m_pActor->userData = this;

	for (auto& pxMesh : m_PxMeshes)
	{
		m_pActor->attachShape(*pxMesh.pShape);
	}

	CPhysX_Manager::GetInstance()->AddActor(*m_pActor);

	return S_OK;
}

void CPhysXDynamicModel::Imgui_RenderProperty()
{
	CComponent::Imgui_RenderProperty();
}

void CPhysXDynamicModel::SetPxWorldMatrix(const _float4x4& WorldMatrix)
{
	m_pActor->setGlobalPose(physx::PxTransform{ CPhysXUtils::ToFloat4x4(WorldMatrix) });
}

void CPhysXDynamicModel::Update_AfterPhysX(CTransform* pTransform)
{
	if (m_pActor->getScene() == nullptr)
		return;
	pTransform->Set_WorldMatrix(CPhysXUtils::ToFloat4x4(m_pActor->getGlobalPose()));
}

void CPhysXDynamicModel::Free()
{
	CComponent::Free();

	if (m_isCloned)
	{
		if (m_pActor)
		{
			if (m_pActor->getScene())
			{
				CPhysX_Manager::GetInstance()->RemoveActor(*m_pActor);

				for (auto& pxMesh : m_PxMeshes)
				{
					m_pActor->detachShape(*pxMesh.pShape);
				}
			}
			m_pActor->release();
			m_pActor = nullptr;
		}
	}
	else
	{
		for (auto& pxMesh : m_PxMeshes)
		{
			pxMesh.pPxMesh->release();
			pxMesh.pPxMesh = nullptr;
			pxMesh.pShape->release();
			pxMesh.pShape = nullptr;
		}
	}
}

CPhysXDynamicModel* CPhysXDynamicModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
	const char* pModelFilePath, _float4x4 PivotMatrix)
{
	CPhysXDynamicModel*		pInstance = new CPhysXDynamicModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, PivotMatrix)))
	{
		MSG_BOX("Failed to Created : CPhysXDynamicModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CPhysXDynamicModel::Clone(void* pArg)
{
	CPhysXDynamicModel*		pInstance = new CPhysXDynamicModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPhysXDynamicModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}
