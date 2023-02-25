#include "stdafx.h"
#include "..\public\PhysXStaticModel.h"
#include "ImguiUtils.h"
#include "PhysX_Manager.h"
#include "Transform.h"
#include "JsonLib.h"
#include "GameUtils.h"

using namespace physx;

_float4x4 CPhysXStaticModel::s_DefaultPivot = _float4x4::CreateScale({ 0.01f, 0.01f, 0.01f });

CPhysXStaticModel::CPhysXStaticModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CPhysXStaticModel::CPhysXStaticModel(const CPhysXStaticModel& rhs)
	: CComponent(rhs)
	, m_PxMeshes(rhs.m_PxMeshes)
{
	
}

HRESULT CPhysXStaticModel::Initialize_Prototype(const char* pModelFilePath, _float4x4 PivotMatrix)
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

		pxMesh.pShape->setSimulationFilterData(physx::PxFilterData{ static_cast<physx::PxU32>(ECOLLIDER_TYPE::CT_STATIC), 0, 0, 0 });
		pxMesh.pShape->setQueryFilterData(physx::PxFilterData{static_cast<physx::PxU32>(GetCollTypeBit(ECOLLIDER_TYPE::CT_STATIC)), 0, 0, 0});

		m_PxMeshes.push_back(pxMesh);
	}


	CloseHandle(hFile);
	return S_OK;
}

HRESULT CPhysXStaticModel::Initialize(void* pArg)
{
	FAILED_CHECK(CComponent::Initialize(pArg));

	auto pPhysics = CPhysX_Manager::GetInstance()->GetPhysics();

	auto pxMat = CPhysXUtils::ToFloat4x4(_float4x4::Identity);
	m_pActor = pPhysics->createRigidStatic(physx::PxTransform{ pxMat });
	m_pActor->userData = this;

	for (auto& pxMesh : m_PxMeshes)
	{
		m_pActor->attachShape(*pxMesh.pShape);
	}

	CPhysX_Manager::GetInstance()->AddActor(*m_pActor);

	return S_OK;
}

void CPhysXStaticModel::Imgui_RenderProperty()
{
	CComponent::Imgui_RenderProperty();
}

void CPhysXStaticModel::SetPxWorldMatrix(const _float4x4& WorldMatrix)
{
	// 디버그에서만 이동가능, 릴리즈에서는 이동못하게 막는다.
#ifdef _DEBUG
	m_pActor->setGlobalPose(physx::PxTransform{ CPhysXUtils::ToFloat4x4(WorldMatrix) });
#endif
}

void CPhysXStaticModel::Free()
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

CPhysXStaticModel* CPhysXStaticModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* pModelFilePath, _float4x4 PivotMatrix)
{
	CPhysXStaticModel*		pInstance = new CPhysXStaticModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, PivotMatrix)))
	{
		MSG_BOX("Failed to Created : CPhysXStaticModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CPhysXStaticModel::Clone(void* pArg)
{
	CPhysXStaticModel*		pInstance = new CPhysXStaticModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPhysXStaticModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}
