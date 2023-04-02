#include "stdafx.h"
#include "RedString.h"
#include <GameInstance.h>
#include <GameUtils.h>
#include <PhysX_Manager.h>
#include <Renderer.h>
#include "VIBuffer_Line_Instancing.h"
#include "RigidBody.h"
#include "JsonLib.h"
#include "ImguiUtils.h"

CRedString::CRedString(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CRedString::CRedString(const CRedString& rhs)
	: CGameObject(rhs)
{
}

HRESULT CRedString::Initialize_Prototype()
{
	FAILED_CHECK(CGameObject::Initialize_Prototype());

	if (nullptr == CGameInstance::GetInstance()->Find_Prototype(LEVEL_STATIC, L"Prototype_Component_Shader_VtxLineInstancing"))
	{
		FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxLineInstancing"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxLineInstancing.hlsl"), VTXLINE_POS_INSTANCE_DECLARATION::Elements, VTXLINE_POS_INSTANCE_DECLARATION::iNumElements)));
	}

	return S_OK;
}

HRESULT CRedString::Initialize(void* pArg)
{
	FAILED_CHECK(CGameObject::Initialize(pArg));

	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxLineInstancing"), TEXT("Shader"), (CComponent**)&m_pShaderCom));

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
		pRigidBody->SetPxWorldMatrix(_float4x4::CreateTranslation(vPos.x, vPos.y, vPos.z));
		pRigidBody->BeginTick();

		Point.second = pRigidBody;
	}
#endif

	CreateString();

	return S_OK;
}

void CRedString::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

#ifdef _DEBUG
	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	for (auto& point : m_Points)
	{
		_float4 vPos = XMVector3TransformCoord(point.first, WorldMatrix);
		vPos.w = 1.f;
		_matrix RigidBodyWorldMatrix = _float4x4::Identity;
		RigidBodyWorldMatrix.r[3] = vPos;
		point.second->Update_Tick(RigidBodyWorldMatrix);
	}
	if (m_bOnTool && m_fCreateCoolTime < 0.f)
	{
		CreateString();
		m_bOnTool = false;
		m_fCreateCoolTime = m_fMaxCreateCoolTime;
	}
	else if (m_fCreateCoolTime >= 0.f)
		m_fCreateCoolTime -= (_float)TimeDelta;
#endif
}

void CRedString::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);
	if (m_bVisible && m_pBuffer != nullptr)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CRedString::Render()
{
	FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)));

	m_pShaderCom->Set_RawValue("g_Radius", &m_fRadius, sizeof(_float));

	m_pShaderCom->Begin(0);
	m_pBuffer->Render();
	return S_OK;
}

void CRedString::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();

	ImGui::Text("First Point is created on Root");

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

	ImGui::InputFloat("Curve Smooth", &m_fCurveSmooth);
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

	ImGui::InputFloat("Radius", &m_fRadius);

	if (ImGui::Button("Create String"))
		CreateString();

	ImGui::InputFloat("ReCreate CoolTime", &m_fMaxCreateCoolTime);
}

void CRedString::LoadFromJson(const Json& json)
{
	CGameObject::LoadFromJson(json);
	for (auto& Point : json["Points"])
	{
		_float4 vPos = Point;
		m_Points.push_back({vPos, nullptr});
	}

	m_fRadius = json["Radius"];
	m_fCurveSmooth = json["CurveSmooth"];

}

void CRedString::SaveToJson(Json& json)
{
	CGameObject::SaveToJson(json);
	json["Points"] = Json::array();
	for (auto& Point : m_Points)
	{
		json["Points"].push_back(Point.first);
	}
	json["Radius"] = m_fRadius;
	json["CurveSmooth"] = m_fCurveSmooth;
}

void CRedString::CreateString()
{
	if (m_Points.size() < 4)
		return;

	Safe_Release(m_pBuffer);

	vector<_float4> SplinePoints;
	SplinePoints.reserve(m_Points.size() * 3);

	for (size_t i = 1; i < m_Points.size() - 2; ++i)
	{
		_float fDelta = 0.1f;
		_vector vTangent = (m_Points[i + 1].first - m_Points[i - 1].first) / (2 * fDelta);
		_vector vAcceleration = (m_Points[i + 1].first - 2 * m_Points[i].first + m_Points[i - 1].first) / (fDelta * fDelta);
		float curvature = XMVectorGetX(XMVector3LengthEst(XMVector3Cross(vTangent, vAcceleration)));
		curvature /= XMVectorGetX(XMVector3LengthEst(vTangent));

		_float fLength = XMVectorGetX(XMVector3LengthEst(m_Points[i].first - m_Points[i + 1].first));

		_uint iSegmentCnt = fLength / (1.f / curvature * m_fCurveSmooth) + 1;

		// IM_LOG("Curve : %f", curvature);

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

	vector<VTXLINE_POS_INSTANCE> InstanceData;
	InstanceData.reserve(SplinePoints.size());

	for (size_t i = 1; i < SplinePoints.size() - 2; ++i)
	{
		VTXLINE_POS_INSTANCE tInstance;

		_vector vPrev = SplinePoints[i - 1];
		_vector vStart = SplinePoints[i];
		_vector vEnd = SplinePoints[i + 1];
		_vector vNext = SplinePoints[i + 2];
	
		_float fScale = XMVectorGetX(XMVector3LengthEst(vStart - vEnd));
	
		_matrix WorldMatrix = XMMatrixIdentity();
		WorldMatrix.r[3] = vStart;
		WorldMatrix.r[2] = XMVector3Normalize(vEnd - vStart) * fScale; // look
		WorldMatrix.r[0] = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), WorldMatrix.r[2])) * fScale;
		WorldMatrix.r[1] = XMVector3Normalize(XMVector3Cross(WorldMatrix.r[2], WorldMatrix.r[0])) * fScale;

		tInstance.vRight = WorldMatrix.r[0];
		tInstance.vUp = WorldMatrix.r[1];
		tInstance.vLook = WorldMatrix.r[2];
		tInstance.vPosition = WorldMatrix.r[3];

		_matrix WorldInv = XMMatrixInverse(nullptr, WorldMatrix);

		tInstance.vStartTangent = XMVector3TransformNormal(vEnd - vPrev, WorldInv);
		tInstance.vEndTangent = XMVector3TransformNormal(vNext - vStart, WorldInv);
	
		InstanceData.push_back(tInstance);
	}

	m_pBuffer = CVIBuffer_Line_Instancing::Create(m_pDevice, m_pContext, InstanceData);
}

void CRedString::DeletePoint(_uint idx)
{
	if (idx >= m_Points.size())
		return;

	Safe_Release(m_Points[idx].second);
	m_Points.erase(m_Points.begin() + idx);
}

_bool CRedString::PickPoint()
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

void CRedString::AddPoint_ForTool(_float4 vPos)
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

	_matrix WorldInv = m_pTransformCom->Get_WorldMatrix_Inverse();
	vPos = XMVector3TransformCoord(vPos, WorldInv);

	if (m_Points.empty() == false && m_Points.back().first == vPos)
	{
		Safe_Release(pRigidBody);
		return;
	}

	m_Points.push_back({vPos, pRigidBody});
}

CRedString* CRedString::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRedString*		pInstance = new CRedString(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRedString");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CRedString::Clone(void* pArg)
{
	CRedString*		pInstance = new CRedString(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRedString");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRedString::Free()
{
	CGameObject::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBuffer);
	for (auto Point : m_Points)
		Safe_Release(Point.second);
	m_Points.clear();
}
