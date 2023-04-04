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

	/*FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC,
		L"Prototype_VIBuffer_Invisible",
		CVIBuffer_Invisible::Create(m_pDevice, m_pContext, m_TrailPointList, m_fHeight)));*/

	return S_OK;
}

HRESULT CInvisibleWall::Initialize(void* pArg)
{
	FAILED_CHECK(CGameObject::Initialize(pArg));

	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxInvisible"), TEXT("Shader"), (CComponent**)&m_pShaderCom));  // ★

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
		auto pRigidBody = dynamic_cast<CRigidBody*>(CGameInstance::GetInstance()->Clone_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", &json));

		const _float4 vPos = XMVector3TransformCoord(Point.first, WorldMatrix);
		pRigidBody->SetPxWorldMatrix(_float4x4::CreateTranslation(vPos.x, vPos.y, vPos.z));
		pRigidBody->BeginTick();

		Point.second = pRigidBody;
	}
#endif

	/*if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		LoadFromJson(json);
	}*/

	return S_OK;
}

void CInvisibleWall::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);
}

void CInvisibleWall::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);
	if (m_bVisible && m_pBuffer != nullptr)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CInvisibleWall::Render()
{
	FAILED_CHECK(SetUp_ShaderResources());
	m_pBuffer->Render();
	return S_OK;
}

void CInvisibleWall::SaveToJson(Json& json)
{
	CGameObject::SaveToJson(json);
	json["TrailList"] = m_TrailPointList;
	json["Width"] = m_fHeight;
	json["Pass"] = m_iPass;

	json["Points"] = Json::array();
	for (auto& Point : m_Points)
	{
		json["Points"].push_back(Point.first);
	}
}

void CInvisibleWall::LoadFromJson(const Json& json)
{
	CGameObject::LoadFromJson(json);
	m_TrailPointList = json["TrailList"];
	m_fHeight = json["Width"];
	m_iPass = json["Pass"];

	for (auto& Point : json["Points"])
	{
		_float3 vPos = Point;
		m_Points.push_back({ vPos, nullptr });
	}
}

void CInvisibleWall::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();

	if (ImGui::Button("Add Point to front of camera"))
	{
		_matrix CamWorld = CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
		CamWorld = XMMatrixInverse(nullptr, CamWorld);
		_vector vPos = CamWorld.r[3] + XMVector3Normalize(CamWorld.r[2]) * 3.5f;
		AddPoint_ForTool(vPos);
	}

	if (ImGui::Button("Add Point To Next"))
	{
		_vector vPos;
		if (m_Points.size() < 2)
		{
			_matrix CamWorld = CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
			CamWorld = XMMatrixInverse(nullptr, CamWorld);
			vPos = CamWorld.r[3] + XMVector3Normalize(CamWorld.r[2]) * 3.5f;
		}
		else
		{
			_vector vBack = m_Points[m_Points.size() - 1].first;
			_vector vPreBack = m_Points[m_Points.size() - 2].first;

			vPos = vBack + XMVector3Normalize(vBack - vPreBack);
			XMVectorSetW(vPos, 1.f);
		}
		AddPoint_ForTool(vPos);		
	}

	static _bool bSelectPoint = false;
	ImGui::Checkbox("Pick Once", &bSelectPoint);
	if (CGameInstance::GetInstance()->KeyDown(CInput_Device::DIM_LB) && bSelectPoint)
	{
		if (RayPicking())
			bSelectPoint = false;
	}

	if (!m_TrailPointList.empty())
	{
		ImGui::InputFloat3("PickingPtPos", &m_TrailPointList.back().x); // 현재 피킹한 곳의 Position(_float3)
		_int iSize;
		iSize = m_TrailPointList.size();
		ImGui::InputInt("PointListSize", &iSize);
	}

	if (ImGui::Button("Delete Point"))
		DeletePoint(m_iPointIdx);

	ImGui::InputFloat("InvWall Height", &m_fHeight);

	if (ImGui::Button("Create BufferInv"))		
		CreateInvWall();		
}

HRESULT CInvisibleWall::SetUp_ShaderResources()
{
	FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));

    return S_OK;
}

_bool CInvisibleWall::RayPicking()
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
	params.iTargetType = CTB_STATIC;
	params.bSingle = true;

	if (CGameInstance::GetInstance()->RayCast(params))
	{		
		for (int i = 0; i < t.getNbAnyHits(); ++i)
		{
			auto p = t.getAnyHit(i);			
			if (auto pStaticModel = dynamic_cast<CPhysXStaticModel*>(CPhysXUtils::GetComponent(p.actor)))
			{
				_float3 PickingPos = { 0.f, 0.f, 0.f };
				memcpy(&PickingPos, &p.position, sizeof(_float3));

				AddPoint_ForTool(PickingPos);

				if (m_bPick)
				{					
					m_TrailPointList.push_back(m_Points.back().first);
					m_iPointIdx = m_TrailPointList.size();
					m_bPick = false;
					return true;					
				}
			}				
		}
	}	

	return false;
}

void CInvisibleWall::DeletePoint(_uint idx)
{
	if (idx >= m_Points.size())
		return;

	Safe_Release(m_Points[idx].second);
	m_Points.erase(m_Points.begin() + idx);
}

void CInvisibleWall::AddPoint_ForTool(_float3 vPos)
{
	if (m_Points.empty())
		vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	Json json;
	json["RigidBody"]["bKinematic"] = true;
	json["RigidBody"]["bTrigger"] = false;
	json["RigidBody"]["Density"] = 10;
	json["RigidBody"]["ColliderType"] = CT_TRIGGER_FOR_MONSTER;
 	json["RigidBody"]["OriginTransform"] = _float4x4::CreateScale(m_fIndicatorSize);
	json["RigidBody"]["ShapeType"] = CRigidBody::TYPE_SPHERE;
	auto pRigidBody = dynamic_cast<CRigidBody*>(CGameInstance::GetInstance()->Clone_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", &json));
	pRigidBody->SetPxWorldMatrix(_float4x4::CreateTranslation(vPos.x, vPos.y, vPos.z));
	pRigidBody->BeginTick();

	/*_matrix WorldInv = m_pTransformCom->Get_WorldMatrix_Inverse();
	vPos = XMVector3TransformCoord(vPos, WorldInv);*/

	if (m_Points.empty() == false && m_Points.back().first == vPos)
	{
		Safe_Release(pRigidBody);
		return;
	}

	m_Points.push_back({ vPos, pRigidBody });
	m_bPick = true;
}

void CInvisibleWall::CreateInvWall()
{
	Safe_Release(m_pBuffer);
	m_pBuffer = CVIBuffer_Invisible::Create(m_pDevice, m_pContext, m_TrailPointList, m_fHeight);
	
	//m_TrailPointList.clear();
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
}



/*
		_float3 vPos1, vPos2;

		_uint iCnt = 0;

		_uint iPointCnt = 0;
		iPointCnt = m_TrailPointList.size();
		for (auto& iter : m_TrailPointList)
		{
			if (iCnt == (m_TrailPointList.size() - 2))
			{
				vPos1 = iter;
			}
			else if (iCnt == (m_TrailPointList.size() - 1))
			{
				vPos2 = iter;
			}

			iCnt++;
		}*/