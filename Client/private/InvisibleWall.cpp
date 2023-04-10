#include "stdafx.h"
#include "InvisibleWall.h"
#include "GameInstance.h"
#include "MathUtils.h"
#include "GameUtils.h"
#include "ImguiUtils.h"
#include "MapInstance_Object.h"
#include "JsonStorage.h"
#include "JsonLib.h"
#include "PhysX_Manager.h"
#include "PhysXStaticModel.h"
#include "Level_Maptool.h"
#include "RigidBody.h"
#include "VIBuffer_Invisible.h"
#include "PhysXStaticModel.h"

CInvisibleWall::CInvisibleWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CInvisibleWall::CInvisibleWall(const CInvisibleWall& rhs)
	: CGameObject(rhs)
{
}

HRESULT CInvisibleWall::Initialize_Prototype()
{
	FAILED_CHECK(CGameObject::Initialize_Prototype());

	if (nullptr == CGameInstance::GetInstance()->Find_Prototype_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxInvisible"))
	{
		FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxInvisible"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInvisibleWall.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements)));
	}

	return S_OK;
}

HRESULT CInvisibleWall::Initialize(void* pArg)
{
	FAILED_CHECK(CGameObject::Initialize(pArg));

	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxInvisible"), TEXT("Shader"), (CComponent**)&m_pShaderCom));  // ★

	Json jsonParams = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/InvisibleWallShaderParams.json");
	CShader::LoadShaderParam(m_tParams, jsonParams);
	if (m_tParams.Floats.empty())
		m_tParams.Floats.push_back(0.f);

#ifdef _DEBUG
	const _matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	for (auto& Point : m_Points)
	{
		Json json;
		json["RigidBody"]["bKinematic"] = true;
		json["RigidBody"]["bTrigger"] = false;
		json["RigidBody"]["Density"] = 10;
		json["RigidBody"]["ColliderType"] = CT_TRIGGER_FOR_MONSTER;
		json["RigidBody"]["OriginTransform"] = _float4x4::CreateScale(m_fIndicatorSize);
		json["RigidBody"]["ShapeType"] = CRigidBody::TYPE_SPHERE;
		auto pRigidBody = dynamic_cast<CRigidBody*>(CGameInstance::GetInstance()->Clone_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", &json));

		const _float4 vPos = XMVector3TransformCoord(Point.first, WorldMatrix);
		pRigidBody->BeginTick();
		pRigidBody->SetPxWorldMatrix(_float4x4::CreateTranslation(vPos.x, vPos.y, vPos.z));

		Point.second = pRigidBody;
	}
#endif

	CreateWall();

	m_Start.SetCurve("Simple_Increase");
	m_Start.SetFinishFunction([this]
	{
		if (m_pPxMesh)
		{
			m_pPxMesh->Activate(true);
			m_pPxMesh->SetPxWorldMatrix(m_pTransformCom->Get_WorldMatrix_f4x4());
		}
	});

	m_End.SetCurve("Simple_Decrease");
	m_End.SetFinishFunction([this]
	{
		if (m_pPxMesh)
			m_pPxMesh->Activate(false);
	});

	return S_OK;
}

void CInvisibleWall::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

#ifdef _DEBUG
	if (m_bOnTool)
	{
		_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		for (auto& point : m_Points)
		{
			_float4 vPos = XMVector3TransformCoord(point.first, WorldMatrix);
			vPos.w = 1.f;
			_matrix RigidBodyWorldMatrix = _float4x4::Identity;
			RigidBodyWorldMatrix.r[3] = vPos;
			point.second->Update_Tick(RigidBodyWorldMatrix);
		}
	}
	if (m_bOnTool && m_fCreateCoolTime < 0.f)
	{
		CreateWall();
		m_bOnTool = false;
		m_fCreateCoolTime = m_fMaxCreateCoolTime;
	}
	else if (m_fCreateCoolTime >= 0.f)
		m_fCreateCoolTime -= (_float)TimeDelta;
#endif

	m_Start.Tick(TimeDelta * 0.5, m_tParams.Floats[0]);
	m_End.Tick(TimeDelta * 0.5, m_tParams.Floats[0]);
	m_pShaderCom->Tick(TimeDelta);
}

void CInvisibleWall::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);
	if (m_bVisible && m_pBuffer != nullptr)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MESH_ALPHABLEND, this);
}

HRESULT CInvisibleWall::Render()
{
	FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)));

	m_pShaderCom->Begin_Params(m_tParams);
	m_pBuffer->Render();
	return S_OK;
}

void CInvisibleWall::SaveToJson(Json& json)
{
	CGameObject::SaveToJson(json);

	json["Points"] = Json::array();
	for (auto& Point : m_Points)
	{
		json["Points"].push_back(Point.first);
	}
	json["SegmentSize"] = m_fSegmentSize;
	json["Visible"] = m_bVisible;
}

void CInvisibleWall::LoadFromJson(const Json& json)
{
	CGameObject::LoadFromJson(json);

	for (auto& Point : json["Points"])
	{
		_float4 vPos = Point;
		m_Points.push_back({vPos, nullptr});
	}
	m_fSegmentSize = json["SegmentSize"];
	m_bVisible = json["Visible"];
}

void CInvisibleWall::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();

	static _bool bAddPointUsingPicking = false;
	ImGui::Checkbox("Add Point Using Picking", &bAddPointUsingPicking);
	if (bAddPointUsingPicking && CGameInstance::GetInstance()->KeyDown(CInput_Device::DIM_LB))
	{
		_float4 vPos;
		CGameUtils::GetPosFromRayCast(vPos);
		vPos.y -= 1.f;
		AddPoint_ForTool(vPos);
		bAddPointUsingPicking = false;
	}

	ImGui::Text("Point Cnt : %d", (int)m_Points.size());
	ImGui::InputInt("Point Idx", &m_iPointIdx);
	if (ImGui::CollapsingHeader("Show Point Guizmo") && m_iPointIdx >= 0 && m_iPointIdx < (int)m_Points.size())
	{
		m_bOnTool = true;

		_float4 vPoint = m_Points[m_iPointIdx].first;

		_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		vPoint = XMVector3TransformCoord(vPoint, WorldMatrix);
		_float4x4 PointWorldMatrix;
		PointWorldMatrix.m[3][0] = vPoint.x;
		PointWorldMatrix.m[3][1] = vPoint.y;
		PointWorldMatrix.m[3][2] = vPoint.z;

		static GUIZMO_INFO tInfo;
		CImguiUtils::Render_Guizmo(&PointWorldMatrix, tInfo, true, true);

		_vector vNewPos = _float4(PointWorldMatrix.m[3][0], PointWorldMatrix.m[3][1], PointWorldMatrix.m[3][2], 1.f);
		
		m_Points[m_iPointIdx].first = XMVector3TransformCoord(vNewPos, XMMatrixInverse(nullptr, WorldMatrix));
	}
	if (ImGui::Button("Delete Point"))
		DeletePoint(m_iPointIdx);

	static _bool bSelectPoint = false;
	ImGui::Checkbox("Pick Point Once", &bSelectPoint);
	if (CGameInstance::GetInstance()->KeyDown(CInput_Device::DIM_LB) && bSelectPoint)
	{
		if (PickPoint())
			bSelectPoint = false;
	}

	ImGui::InputFloat("SegmentSize", &m_fSegmentSize);

	if (ImGui::Button("Create Wall"))
		CreateWall();

	ImGui::InputFloat("ReCreate CoolTime", &m_fMaxCreateCoolTime);

	if (ImGui::Button("Active"))
		Activate(true);
	if (ImGui::Button("DeActive"))
		Activate(false);
}

void CInvisibleWall::CreateWall()
{
	if (m_Points.size() < 3)
		return;

	CreateInstanceData();
}

void CInvisibleWall::CreateInstanceData()
{
	m_Points.push_back({m_Points.front().first, nullptr});
	vector<VTXNORTEX> VtxData;
	vector<FACEINDICES32> IndexData;

	_bool bClockWise = false;
	_vector vDir1 = XMVector3Normalize(m_Points[1].first - m_Points[0].first);
	_vector vDir2 = XMVector3Normalize(m_Points[2].first - m_Points[1].first);
	_vector vCross = XMVector3Cross(vDir1, vDir2);
	if (XMVectorGetY(vCross) > 0)
		bClockWise = true;

	vector<_float4> SplinePoints;
	SplinePoints.reserve(m_Points.size() * 3);

	for (size_t i = 0; i < m_Points.size() - 1; ++i)
	{
		const _float fLength = XMVectorGetX(XMVector3LengthEst(m_Points[i].first - m_Points[i + 1].first));
		const _uint iSegmentCnt = fLength / m_fSegmentSize + 1;
	
		if (i > 0 && i < m_Points.size() - 2) // 켓멀롬
		{
			for (_uint j = 0; j < iSegmentCnt; ++j)
			{
				_float fRatio = (_float)j / (_float)iSegmentCnt;
				_vector vPos = XMVectorCatmullRom(
					m_Points[i - 1].first, 
					m_Points[i].first, 
					m_Points[i + 1].first, 
					m_Points[i + 2].first, 
					fRatio);
				XMVectorSetW(vPos, 1.f);
				SplinePoints.push_back(vPos);
			}
		}
		else// 선형 보간
		{
			for (_uint j = 0; j < iSegmentCnt; ++j)
			{
				_float fRatio = (_float)j / (_float)iSegmentCnt;
				
				_vector vPos = XMVectorLerp(
					m_Points[i].first,
					m_Points[i + 1].first,
					fRatio);
				XMVectorSetW(vPos, 1.f);
				SplinePoints.push_back(vPos);
	
				// 마지막 점
				if (i == m_Points.size() - 2 && j == iSegmentCnt - 1)
				{
					SplinePoints.push_back(m_Points[i + 1].first);
				}
			}
		}
	}

	VtxData.reserve(SplinePoints.size() * 2);
	IndexData.reserve(SplinePoints.size() * 2 - 2);
	_float fU = 0.f;

	VTXNORTEX vtxNor;
	FACEINDICES32 Index;
	ZeroMemory(&vtxNor, sizeof(VTXNORTEX));
	ZeroMemory(&Index, sizeof(FACEINDICES32));

	for (size_t i = 0; i < SplinePoints.size(); ++i)
	{
		vtxNor.vPosition = _float3(SplinePoints[i].x, SplinePoints[i].y + 10.f, SplinePoints[i].z);
		vtxNor.vTexUV = _float2(fU, 0.f);
		VtxData.push_back(vtxNor);

		vtxNor.vPosition.y -= 10.f;
		vtxNor.vTexUV.y = 1.f;
		VtxData.push_back(vtxNor);


		// 시계방향일 때
		if (bClockWise)
		{
			if (i < SplinePoints.size() - 1)
			{
				Index._0 = i * 2;
				Index._1 = Index._0 + 2;
				Index._2 = Index._0 + 3;
				IndexData.push_back(Index);

				Index._1 = Index._0 + 3;
				Index._2 = Index._0 + 1;
				IndexData.push_back(Index);

				const _float fLength = XMVectorGetX(XMVector3Length(SplinePoints[i] - SplinePoints[i + 1]));
				fU += fLength;
			}
		}
		else
		{
			if (i < SplinePoints.size() - 1)
			{
				Index._0 = i * 2;
				Index._1 = Index._0 + 1;
				Index._2 = Index._0 + 3;
				IndexData.push_back(Index);

				Index._1 = Index._0 + 3;
				Index._2 = Index._0 + 2;
				IndexData.push_back(Index);

				const _float fLength = XMVectorGetX(XMVector3Length(SplinePoints[i] - SplinePoints[i + 1]));
				fU += fLength;
			}
		}
	}


	{
		Safe_Release(m_pBuffer);
		CONTEXT_LOCK;
		m_pBuffer = CVIBuffer_Invisible::Create(m_pDevice, m_pContext, VtxData, IndexData);
	}

	m_Points.pop_back();

	Safe_Release(m_pPxMesh);
	m_pPxMesh = CPhysXStaticMesh::Create(m_pDevice, m_pContext, VtxData, IndexData, m_pTransformCom->Get_WorldMatrix_f4x4());
}

void CInvisibleWall::Activate(_bool bActive)
{
	if (bActive)
	{
		m_Start.PlayFromStart();
	}
	else
	{
		m_End.PlayFromStart();
	}
}

_bool CInvisibleWall::IsActive()
{
	return m_pPxMesh != nullptr && m_pPxMesh->IsActive();
}

_bool CInvisibleWall::PickPoint()
{
	_float4 vOrigin;
	_float4 vDir;
	CGameUtils::GetPickingRay(vOrigin, vDir);

	physx::PxRaycastHit hitBuffer[1];
	physx::PxRaycastBuffer t(hitBuffer, 1);

	RayCastParams params;
	params.rayOut = &t;
	params.vOrigin = vOrigin;
	params.vDir = vDir;
	params.fDistance = 1000.f;
	params.iTargetType = CTB_TRIGGER_FOR_MONSTER;
	params.bSingle = true;

	if (CGameInstance::GetInstance()->RayCast(params))
	{
		for (int i = 0; i < t.getNbAnyHits(); ++i)
		{
			auto p = t.getAnyHit(i);
			if (auto pRigidBody = dynamic_cast<CRigidBody*>(CPhysXUtils::GetComponent(p.actor)))
			{
				for (size_t i = 0; i < m_Points.size(); ++i)
				{
					if (m_Points[i].second == pRigidBody)
					{
						m_iPointIdx = i;
						return true;
					}
				}
			}
		}
	}

	return false;
}

void CInvisibleWall::AddPoint_ForTool(_float4 vPos)
{
	Json json;
	json["RigidBody"]["bKinematic"] = true;
	json["RigidBody"]["bTrigger"] = false;
	json["RigidBody"]["Density"] = 10;
	json["RigidBody"]["ColliderType"] = CT_TRIGGER_FOR_MONSTER;
	json["RigidBody"]["OriginTransform"] = _float4x4::CreateScale(m_fIndicatorSize);
	json["RigidBody"]["ShapeType"] = CRigidBody::TYPE_SPHERE;
	auto pRigidBody = dynamic_cast<CRigidBody*>(CGameInstance::GetInstance()->Clone_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", &json));
	pRigidBody->BeginTick();
	pRigidBody->SetPxWorldMatrix(_float4x4::CreateTranslation(vPos.x, vPos.y, vPos.z));

	_matrix WorldInv = m_pTransformCom->Get_WorldMatrix_Inverse();
	vPos = XMVector3TransformCoord(vPos, WorldInv);

	if (m_Points.empty() == false && m_Points.back().first == vPos)
	{
		Safe_Release(pRigidBody);
		return;
	}

	m_Points.push_back({vPos, pRigidBody});
}

void CInvisibleWall::DeletePoint(_uint idx)
{
	if (idx >= m_Points.size())
		return;

	Safe_Release(m_Points[idx].second);
	m_Points.erase(m_Points.begin() + idx);
}

CInvisibleWall* CInvisibleWall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInvisibleWall* pInstance = new CInvisibleWall(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CInvisibleWall");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CInvisibleWall::Clone(void* pArg)
{
    CInvisibleWall* pInstance = new CInvisibleWall(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CInvisibleWall");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CInvisibleWall::Free()
{
    CGameObject::Free();
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pBuffer);
	for (auto Point : m_Points)
		Safe_Release(Point.second);
	m_Points.clear();

	Safe_Release(m_pPxMesh);
	for (auto pTex : m_tParams.Textures)
		Safe_Release(pTex.first);
}
