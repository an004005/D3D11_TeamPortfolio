#include "stdafx.h"
#include "..\public\ParticleSystem.h"
#include <ImguiUtils.h>
#include <MathUtils.h>

#include "VIBuffer_Point_Instancing.h"
#include "GameInstance.h"
#include "GameUtils.h"
#include "JsonLib.h"
#include "Model.h"
#include "RigidBody.h"
#include "CurveManager.h"
#include "CurveFloatMapImpl.h"

CParticleSystem::CParticleSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CParticleSystem::CParticleSystem(const CParticleSystem& rhs)
	: CGameObject(rhs)
{
	m_ModelProtoTag = "";
	m_iInstanceNum = 30000;
}

_int CParticleSystem::GetLiveParticleCnt()
{
	if(m_eBufferType == EBufferType::POINT)
	{
		return m_PointList.size();
	}
	else
	{
		return m_MeshList.size();
	}
}

void CParticleSystem::Remote_BurstCnt_Zero()
{
	m_iBurstCnt = 0;
}

HRESULT CParticleSystem::Initialize(void* pArg)
{
	CGameObject::Initialize(pArg);

	/* For.Com_Renderer */
	

	if (pArg == nullptr)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
			(CComponent**)&m_pRendererCom)))
			return E_FAIL;

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance_Particle"), TEXT("Shader"),
			(CComponent**)&m_pShader)))
			return E_FAIL;

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Point_Instance_Particle"), TEXT("Com_PointInstance"),
			(CComponent**)&m_pPointInstanceBuffer)))
			return E_FAIL;

		// m_pPointInstanceBuffer = CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, m_iInstanceNum);
		m_eBufferType = EBufferType::POINT;;
	}
	else
	{
		if (m_eBufferType == EBufferType::MESH)
		{
			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
				(CComponent**)&m_pRendererCom)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
				(CComponent**)&m_pRendererCom)))
				return E_FAIL;

			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance_Particle"), TEXT("Shader"),
				(CComponent**)&m_pShader)))
				return E_FAIL;

			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Point_Instance_Particle"), TEXT("Com_PointInstance"),
				(CComponent**)&m_pPointInstanceBuffer)))
				return E_FAIL;
			// m_pPointInstanceBuffer = CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, m_iInstanceNum);
		}
	}

	if(m_bTurn == true)
		m_pTransformCom->SetRotPerSec(XMConvertToRadians(m_fTurnAngle));

	if(m_bUseMainCurve == true)
	{
		m_Timeline.PlayFromStart();
	}

	Start_Timeline();

	return S_OK;
}

void CParticleSystem::Tick(_double TimeDelta)
{
	// if (m_bVisible == false)
	// 	return;
	m_pShader->Tick(TimeDelta);
	m_Timeline.Tick(TimeDelta);

	if (m_bCurveDir)
	{
		m_VtxStartTimeline.Tick(TimeDelta, m_fMeshVtxStartRatio);
		m_VtxRangeTimeline.Tick(TimeDelta, m_fMeshVtxRangeRatio);
	}

		const _float fTimeDelta = (_float)TimeDelta;

		if (m_bLoop == false)
			m_fDuration -= fTimeDelta;
		if (m_fDuration < 0.f)
			m_iBurstCnt = 0;

		if (m_eBufferType == EBufferType::MESH)
		{
			UpdateMeshes(fTimeDelta);
		}
		else
			UpdatePoints(fTimeDelta);

		m_fCurSpawnTick -= fTimeDelta;

		if (m_fCurSpawnTick <= 0.f)
		{
			m_fCurSpawnTick = m_fSpawnTickTime;

			if (m_eBufferType == EBufferType::MESH)
			{
				AddMesh();
			}
			else
			{
				AddPoint();
			}
		}

	if(m_bTurn == true)
	{
		if (m_fTurnAngle != 0.f)
		{
			m_pTransformCom->SetRotPerSec(XMConvertToRadians(m_fTurnAngle));
			m_pTransformCom->Turn(m_fTurnAxis, TimeDelta);
		}
		// m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), m_fTurnAxis.x);
	}

}

void CParticleSystem::Late_Tick(_double TimeDelta)
{
	
	if (m_bVisible)
	{
		if(m_eBufferType == EBufferType::MESH && m_pMeshInstanceBuffer != nullptr)
		{
			m_pMeshInstanceBuffer->SetInstanceBuffer(&m_MeshList);
		}
		else if (m_eBufferType == EBufferType::POINT && m_pPointInstanceBuffer != nullptr)
		{
			m_pPointInstanceBuffer->SetInstanceBuffer(&m_PointList);
			// particle은 작기 때문에 먼저 해도 문제없을 듯
		}

		if(m_bNonAlphaBlend == true)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		else
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND_FIRST, this);
	}
}

HRESULT CParticleSystem::Render()
{
	if (m_pShader == nullptr)
		return S_OK;

	FAILED_CHECK(SetParams());
	FAILED_CHECK(Begin());

	if (m_eBufferType == EBufferType::MESH && m_pMeshInstanceBuffer != nullptr)
		m_pMeshInstanceBuffer->Render();
	else if (m_eBufferType == EBufferType::POINT && m_pPointInstanceBuffer != nullptr)
		m_pPointInstanceBuffer->Render();
	
	return S_OK;
}

void CParticleSystem::AfterPhysX()
{
	CGameObject::AfterPhysX();
	if (m_bPhysX)
	{
		for (auto& data : m_MeshList)
		{
			_float4x4 WorldMatrix = data.pRigidBody->GetPxWorldMatrix();

			memcpy(&data.vRight, &WorldMatrix.m[0], sizeof(_float4));
			memcpy(&data.vUp, &WorldMatrix.m[1], sizeof(_float4));
			memcpy(&data.vLook, &WorldMatrix.m[2], sizeof(_float4));
			memcpy(&data.vPosition, &WorldMatrix.m[3], sizeof(_float4));
			data.vRight *= data.vSize.x;
			data.vUp *= data.vSize.y;
			data.vLook *= data.vSize.z;
		}
	}
}

void CParticleSystem::SaveToJson(Json& json)
{
	CGameObject::SaveToJson(json);
	CShader::SaveShaderParam(m_tParam, json);

	json["Duration"] = m_fDuration;
	json["Loop"] = m_bLoop;
	json["SpawnTickTime"] = m_fSpawnTickTime;
	json["BurstCnt"] = m_iBurstCnt;
	json["LifeRand"] = m_bLifeRand;
	json["LifeMinMax"] = m_fLifeMinMax;
	json["Life"] = m_fLife;
	json["SpeedRand"] = m_bSpeedRand;
	json["SpeedMinMax"] = m_fSpeedMinMax;
	json["Speed"] = m_fSpeed;
	json["pSize"] = m_fSize;
	json["SphereRadius"] = m_fSphereRadius;
	json["FromOrigin"] = m_bFromOrigin;
	json["bLocal"] = m_bLocal;
	json["bGravity"] = m_bGravity;
	json["bSizeDecreaseByLife"] = m_bSizeDecreaseByLife;
	json["bSizeIncreaseByLife"] = m_bSizeIncreaseByLife;
	json["Shape"] = m_eShape;
	json["ConeDirDegree"] = m_fConeAngleDeg;
	json["ConeOriginRadius"] = m_fConeOriginRadius;
	json["IncreaseMaxSize"] = m_fIncreaseMaxSize;
	json["GravityPowerMinMax"] = m_fGravityPowerMinMax;
	json["MeshBufferProtoTag"] = m_MeshBufferProtoTag;
	json["ShaderProtoTag"] = m_ShaderProtoTag;
	json["BufferType"] = m_eBufferType;
	json["bRotate"] = m_bRotate;
	json["RotationToTimeMin"] = m_fRotationToTime_Min;
	json["RotationToTimeMax"] = m_fRotationToTime_Max;
	json["ScaleVariation"] = m_vScaleVariation;
	json["MeshSize"] = m_vMeshSize;
	json["BilboardType"] = m_eBilboardType;

	json["bNonAlpha"] = m_bNonAlphaBlend;
	json["ModelProtoTag"] = m_ModelProtoTag;
	json["ModelCurveProtoTag"] = m_MeshCurveModelProtoTag;

	json["RandDirMax"] = m_vRandDir_Max;
	json["RandDirMin"] = m_vRandDir_Min;
	json["SphereDetail"] = m_bSphereDetail;
	json["UseMeshData"] = m_bUseMeshData;
	json["bPhysX"] = m_bPhysX;
	json["bMeshCurve"] = m_bMeshCurve;

	json["bCurveDir"] = m_bCurveDir;
	json["XDirCurveName"] = m_strXDirCurveName;
	json["YDirCurveName"] = m_strYDirCurveName;
	json["ZDirCurveName"] = m_strZDirCurveName;
	json["bTurn"] = m_bTurn;
	json["TurnAxis"] = m_fTurnAxis;
	json["TurnAngle"] = m_fTurnAngle;

	json["VtxStartTimelineCurve"] = m_VtxStartTimeline.GetCurveName();
	json["VtxRangeTimelineCurve"] = m_VtxRangeTimeline.GetCurveName();
	json["VtxStartRatioValue"] = m_fMeshVtxStartRatio;
	json["VtxRangeRatioValue"] = m_fMeshVtxRangeRatio;

	json["bUseMainCurve"] = m_bUseMainCurve;

	if(m_bUseMainCurve == true)
	{
		for(auto pCurves : m_Curves)
		{
			Json CurveJson;
			pCurves.second->SaveToJson(CurveJson);
			json["Curves"].push_back(CurveJson);
		}
	}

	if (m_iSelectFinishFunc >= FUNC_PLAYFROMSTART && m_iSelectFinishFunc < FUNC_END)
		json["FinishFunc"] = m_iSelectFinishFunc;
	else
		json["FinishFunc"] = FUNC_RESET;

	if (m_fEndTime >= 0.f)
		json["EndTime"] = m_fEndTime;
	else
		json["EndTime"] = 0.f;
}

void CParticleSystem::LoadFromJson(const Json& json)
{
	CGameObject::LoadFromJson(json);

	CShader::LoadShaderParam(m_tParam, json);

	m_fDuration = json["Duration"];
	m_bLoop = json["Loop"];
	m_fSpawnTickTime = json["SpawnTickTime"];
	m_iBurstCnt = json["BurstCnt"];
	m_bLifeRand = json["LifeRand"];
	m_fLifeMinMax = json["LifeMinMax"];
	m_fLife = json["Life"];
	m_bSpeedRand = json["SpeedRand"];
	m_fSpeedMinMax = json["SpeedMinMax"];
	m_fSpeed = json["Speed"];
	m_fSize = json["pSize"];
	m_fSphereRadius = json["SphereRadius"];
	m_bFromOrigin = json["FromOrigin"];
	m_bLocal = json["bLocal"];
	m_bGravity = json["bGravity"];
	m_ShaderProtoTag = json["ShaderProtoTag"];
	m_fGravityPowerMinMax = json["GravityPowerMinMax"];
	m_eBufferType = json["BufferType"];
	m_bRotate = json["bRotate"];
	m_fRotationToTime_Min = json["RotationToTimeMin"] ;
	m_fRotationToTime_Max = json["RotationToTimeMax"] ;
	m_vScaleVariation = json["ScaleVariation"];

	if (json.contains("bTurn"))
	{
		m_bTurn = json["bTurn"];
	}

	if(json.contains("TurnAxis"))
		m_fTurnAxis = json["TurnAxis"];

	if (json.contains("TurnAngle"))
		m_fTurnAngle = json["TurnAngle"];

	if(json.contains("bCurveDir"))
	{
		m_bCurveDir = json["bCurveDir"];

		if(m_bCurveDir == true)
		{
			m_strXDirCurveName = json["XDirCurveName"];
			m_strYDirCurveName = json["YDirCurveName"];
			m_strZDirCurveName = json["ZDirCurveName"];
		}
	}

	if(json.contains("bMeshCurve"))
	{
		m_bMeshCurve = json["bMeshCurve"];

		if (json.contains("ModelCurveProtoTag"))
		{
			m_MeshCurveModelProtoTag = json["ModelCurveProtoTag"];

			if(m_MeshCurveModelProtoTag != "")
				FAILED_CHECK(Add_Component(LEVEL_NOW, CGameUtils::s2ws(m_MeshCurveModelProtoTag).c_str(), TEXT("MeshCurveModel"), (CComponent**)&m_pMeshCurveModel));
		}
	}

	if(json.contains("bPhysX"))
	{
		m_bPhysX = json["bPhysX"];
	}

	if(json.contains("UseMeshData"))
	{
		m_bUseMeshData = json["UseMeshData"];
	}

	if(json.contains("ModelProtoTag"))
	{
		m_ModelProtoTag = json["ModelProtoTag"];

		if (m_ModelProtoTag != "")
		{
			FAILED_CHECK(Add_Component(LEVEL_NOW, CGameUtils::s2ws(m_ModelProtoTag).c_str(), TEXT("Model"), (CComponent**)&m_pModel));
		}
	}

	if (json.contains("BilboardType"))
	{
		m_eBilboardType = json["BilboardType"];
	}
	else if (m_bRotate == true)
		m_eBilboardType = EBilboardType::NONE;
	else if (m_bRotate == false)
		m_eBilboardType = EBilboardType::LOOK;
	else
	{
		m_eBilboardType = EBilboardType::ALL;
	}

	if(json.contains("bNonAlpha"))
		m_bNonAlphaBlend = json["bNonAlpha"];

	if(json.contains("SphereDetail"))
	{
		m_bSphereDetail = json["SphereDetail"];
	}

	if(json.contains("RandDirMax"))
	{
		m_vRandDir_Max = json["RandDirMax"];
	}

	if(json.contains("RandDirMin"))
	{
		m_vRandDir_Min = json["RandDirMin"];
	}

	if(json.contains("MeshSize"))
		m_vMeshSize = json["MeshSize"];

	if(json.contains("MeshDir"))
	{
		m_bCurveDir = json["MeshDir"];

	}

	if(m_eBufferType == EBufferType::MESH)
	{
		if (json.contains("MeshBufferProtoTag"))
		{
			m_MeshBufferProtoTag = json["MeshBufferProtoTag"];

			FAILED_CHECK(Add_Component(LEVEL_NOW, CGameUtils::s2ws(m_MeshBufferProtoTag).c_str(), TEXT("Mesh_Instance_Buffer"), (CComponent**)&m_pMeshInstanceBuffer));
			FAILED_CHECK(Add_Component(LEVEL_NOW, CGameUtils::s2ws(m_ShaderProtoTag).c_str(), TEXT("Mesh_Instance_Shader"), (CComponent**)&m_pShader));
		}
	}

	m_bSizeDecreaseByLife = json["bSizeDecreaseByLife"];
	if (json.contains("bSizeIncreaseByLife"))
		m_bSizeIncreaseByLife = json["bSizeIncreaseByLife"];
	if (json.contains("Shape"))
		m_eShape = json["Shape"];
	if (json.contains("ConeDirDegree"))
		m_fConeAngleDeg = json["ConeDirDegree"];
	if (json.contains("ConeOriginRadius"))
		m_fConeOriginRadius = json["ConeOriginRadius"];
	if (json.contains("IncreaseMaxSize"))
		m_fIncreaseMaxSize = json["IncreaseMaxSize"];

	if (json.contains("VtxStartTimelineCurve"))
		m_VtxStartTimeline.SetCurve(json["VtxStartTimelineCurve"]);
	if (json.contains("VtxRangeTimelineCurve"))
		m_VtxRangeTimeline.SetCurve(json["VtxRangeTimelineCurve"]);
	if(json.contains("bUseMainCurve"))
		m_bUseMainCurve = json["bUseMainCurve"];

	if (json.contains("EndTime"))
		json["EndTime"].get_to<_float>(m_fEndTime);
	else
		m_fEndTime = 0.f;


	if (m_bUseMainCurve == true)
	{
		json["FinishFunc"].get_to<_int>(m_iSelectFinishFunc);

		m_Timeline.SetTimelineLength((_double)m_fEndTime);

		if (m_iSelectFinishFunc == 0)
		{
			m_Timeline.SetFinishFunction(&m_Timeline, &CTimeline::PlayFromStart);
		}
		else if (m_iSelectFinishFunc == 1)
		{
			m_Timeline.SetFinishFunction(&m_Timeline, &CTimeline::Reset);
		}
		else if (m_iSelectFinishFunc == 2)
		{
			m_Timeline.SetFinishFunction(&m_Timeline, &CTimeline::Stop);
		}
		else if (m_iSelectFinishFunc == 3)
		{
			m_Timeline.SetFinishFunction(&m_Timeline, &CTimeline::Reverse);
		}
		else
			m_Timeline.SetFinishFunction(&m_Timeline, &CTimeline::Reset);

		if (json.contains("Curves"))
		{
			for (auto curveJson : json["Curves"])
			{
				Json JsonCurve = curveJson;
				auto pCurve = CCurveFloatImpl::Create(&JsonCurve);
				AddTargetCurve(pCurve->GetName(), pCurve);
			}
		}
		else
			m_bUseMainCurve = false;
	}

	if(json.contains("VtxStartRatioValue"))
	{
		m_fMeshVtxStartRatio = json["VtxStartRatioValue"];
	}

	if (json.contains("VtxRangeRatioValue"))
	{
		m_fMeshVtxRangeRatio = json["VtxRangeRatioValue"];
	}


}

void CParticleSystem::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();


	ImGui::SliderFloat("VtxStartRatio", &m_fMeshVtxStartRatio, 0.f, 1.f);
	ImGui::SliderFloat("VtxRangeRatio", &m_fMeshVtxRangeRatio, 0.f, 1.f);

	ImGui::Checkbox("CurveDir", &m_bCurveDir);
	static char szXDirCurveName[MAX_PATH]{};
	static char szYDirCurveName[MAX_PATH]{};
	static char szZDirCurveName[MAX_PATH]{};
	strcpy_s(szXDirCurveName, m_strXDirCurveName.c_str());
	strcpy_s(szYDirCurveName, m_strYDirCurveName.c_str());
	strcpy_s(szZDirCurveName, m_strZDirCurveName.c_str());
	ImGui::InputText("XDir CurveName", szXDirCurveName, MAX_PATH);
	ImGui::InputText("YDir CurveName", szYDirCurveName, MAX_PATH);
	ImGui::InputText("ZDir CurveName", szZDirCurveName, MAX_PATH);
	m_strXDirCurveName = szXDirCurveName;
	m_strYDirCurveName = szYDirCurveName;
	m_strZDirCurveName = szZDirCurveName;

	static _int iCurveIdx = 0;
	ImGui::InputInt("CurveID", &iCurveIdx);
	switch (iCurveIdx)
	{
	case 0:
		m_VtxStartTimeline.Imgui_RenderEditor();
		break;
	case 1:
		m_VtxRangeTimeline.SetID(iCurveIdx);
		m_VtxRangeTimeline.Imgui_RenderEditor();
		break;
	}

	if (ImGui::Button("StartAllTimeline"))
	{
		Start_Timeline();
	}

	ImGui::Separator();
	CShader::Imgui_RenderShaderParams(m_tParam);
	ImGui::Separator();
	ImGui::Checkbox("PhysX", &m_bPhysX);
	ImGui::SameLine();
	ImGui::Checkbox("MeshCurve", &m_bMeshCurve);

	{
		char BufferProtoTag[MAX_PATH];
		strcpy(BufferProtoTag, m_MeshBufferProtoTag.c_str());
		ImGui::InputText("MeshBuffer ProtoTag", BufferProtoTag, MAX_PATH);
		m_MeshBufferProtoTag = BufferProtoTag;
		ImGui::SameLine();
		if (ImGui::Button("ReCreate Mesh_Instance"))
		{
			Safe_Release(m_pMeshInstanceBuffer);
			Delete_Component(TEXT("Mesh_Instance_Buffer"));
			FAILED_CHECK(Add_Component(LEVEL_NOW, CGameUtils::s2ws(m_MeshBufferProtoTag).c_str(), TEXT("Mesh_Instance_Buffer"), (CComponent**)&m_pMeshInstanceBuffer));
		}
	}
	{
		char ShaderProtoTag[MAX_PATH];
		strcpy(ShaderProtoTag, m_ShaderProtoTag.c_str());
		ImGui::InputText("InstanceShader ProtoTag", ShaderProtoTag, MAX_PATH);
		m_ShaderProtoTag = ShaderProtoTag;
		ImGui::SameLine();
		if (ImGui::Button("ReCreate Intance_Shader"))
		{
			Safe_Release(m_pShader);
			Delete_Component(TEXT("Shader"));
			FAILED_CHECK(Add_Component(LEVEL_NOW, CGameUtils::s2ws(m_ShaderProtoTag).c_str(), TEXT("Shader"), (CComponent**)&m_pShader));
		}
	}

	{
		char ModelProtoTag[MAX_PATH];
		strcpy(ModelProtoTag, m_ModelProtoTag.c_str());
		ImGui::InputText("MeshData ProtoTag", ModelProtoTag, MAX_PATH);
		m_ModelProtoTag = ModelProtoTag;
		ImGui::SameLine();
		if (ImGui::Button("ReCreate MeshData"))
		{
			Safe_Release(m_pModel);
			Delete_Component(TEXT("Model"));
			FAILED_CHECK(Add_Component(LEVEL_NOW, CGameUtils::s2ws(m_ModelProtoTag).c_str(), TEXT("Model"), (CComponent**)&m_pModel));
		}
	}

	{
		char MeshModelProtoTag[MAX_PATH];
		strcpy(MeshModelProtoTag, m_MeshCurveModelProtoTag.c_str());
		ImGui::InputText("MeshCurve ProtoTag", MeshModelProtoTag, MAX_PATH);
		m_MeshCurveModelProtoTag = MeshModelProtoTag;
		ImGui::SameLine();
		if (ImGui::Button("ReCreate MeshCurve"))
		{
			Safe_Release(m_pMeshCurveModel);
			Delete_Component(TEXT("MeshCurveModel"));
			FAILED_CHECK(Add_Component(LEVEL_NOW, CGameUtils::s2ws(m_MeshCurveModelProtoTag).c_str(), TEXT("MeshCurveModel"), (CComponent**)&m_pMeshCurveModel));
		}
	}
	ImGui::Separator();

	if (ImGui::RadioButton("Point Instance", m_eBufferType == EBufferType::POINT))
		m_eBufferType = EBufferType::POINT;
	ImGui::SameLine();
	if (ImGui::RadioButton("Mesh Instance", m_eBufferType == EBufferType::MESH))
		m_eBufferType = EBufferType::MESH;
	ImGui::Separator();

	ImGui::InputFloat("Duration", &m_fDuration);
	ImGui::Checkbox("Particle Loop", &m_bLoop);
	ImGui::InputFloat("SpawnTickTime", &m_fSpawnTickTime);

	ImGui::InputInt("Burst Cnt", &m_iBurstCnt);
	if (m_iBurstCnt <= 0)
		m_iBurstCnt = 1;

	ImGui::Checkbox("LifeRand", &m_bLifeRand);
	if (m_bLifeRand)
		ImGui::InputFloat2("Life Min Max", (float*)&m_fLifeMinMax);
	else
		ImGui::InputFloat("Life", &m_fLife);

	ImGui::Checkbox("SpeedRand", &m_bSpeedRand);
	if (m_bSpeedRand)
		ImGui::InputFloat2("Speed Min Max", (float*)&m_fSpeedMinMax);
	else
		ImGui::InputFloat("Speed", &m_fSpeed);

	if (m_eBufferType == EBufferType::MESH)
	{
		ImGui::InputFloat3("Mesh Size", (float*)&m_vMeshSize);

		CImguiUtils::InputFloat3(&m_vScaleVariation, "Mesh ScaleVariation");
	}
	else
		ImGui::InputFloat2("P Size", (float*)&m_fSize);

	ImGui::Checkbox("IsTurn", &m_bTurn);

	if (m_bTurn == true)
	{
		CImguiUtils::InputFloat3(&m_fTurnAxis, "Turn Axis");
		ImGui::InputFloat("Turn Speed", &m_fTurnAngle);

	}

	ImGui::Separator();
	ImGui::Separator();

	ImGui::NewLine();

	ImGui::Checkbox("Render NonAlpha", &m_bNonAlphaBlend);
	ImGui::SameLine();
	ImGui::Checkbox("Use Mesh Data", &m_bUseMeshData);

	ImGui::Checkbox("From Origin", &m_bFromOrigin);
	ImGui::SameLine();
	ImGui::Checkbox("Is Local", &m_bLocal);
	ImGui::SameLine();
	ImGui::Checkbox("Is Gravity", &m_bGravity);
	if(m_bGravity == true)
	{
		ImGui::InputFloat2("Gravity Power Min Max", (float*)&m_fGravityPowerMinMax);
	}

	ImGui::Checkbox("Is Rotate", &m_bRotate);
	if(m_bRotate == true)
	{
		CImguiUtils::InputFloat3(&m_fRotationToTime_Min, "RotationToTime Min");
		CImguiUtils::InputFloat3(&m_fRotationToTime_Max, "RotationToTime Max");
	}

	ImGui::Checkbox("Size Decrease by life", &m_bSizeDecreaseByLife);
	ImGui::SameLine();
	ImGui::Checkbox("Size Increase by life", &m_bSizeIncreaseByLife);

	if (m_bSizeIncreaseByLife)
	{
		ImGui::InputFloat("Increase MaxSize", &m_fIncreaseMaxSize);
	}

	if (ImGui::RadioButton("Shape Sphere", m_eShape == ESpawnShape::SPHERE))
	{
		m_eShape = ESpawnShape::SPHERE;

		
	}

	ImGui::Checkbox("Sphere Detail", &m_bSphereDetail);
	if (m_bSphereDetail)
	{
		CImguiUtils::InputFloat3(&m_vRandDir_Min, "RandDir_Min");
		CImguiUtils::InputFloat3(&m_vRandDir_Max, "RandDir_Max");
	}

	if (ImGui::RadioButton("Shape Cone", m_eShape == ESpawnShape::CONE))
		m_eShape = ESpawnShape::CONE;

	if (m_eShape == ESpawnShape::SPHERE)
	{
		ImGui::InputFloat("Sphere Radius", &m_fSphereRadius);
	}
	else if (m_eShape == ESpawnShape::CONE)
	{
		ImGui::InputFloat("Cone Angle Deg", &m_fConeAngleDeg);
		if (m_fConeAngleDeg <= 0.f)
			m_fConeAngleDeg = 1.f;
		if (m_fConeAngleDeg > 90.f)
			m_fConeAngleDeg = 90.f;

		ImGui::InputFloat("Cone Origin Radius", &m_fConeOriginRadius);
		if (m_fConeOriginRadius < 0.f)
			m_fConeOriginRadius = 0.f;
	}

	if (ImGui::RadioButton("Bilboard All", m_eBilboardType == EBilboardType::ALL))
	{
		m_eBilboardType = EBilboardType::ALL;
	}

	if (ImGui::RadioButton("Bilboard LOOK", m_eBilboardType == EBilboardType::LOOK))
	{
		m_eBilboardType = EBilboardType::LOOK;
	}

	if (ImGui::RadioButton("NO Bilboard, Rotate", m_eBilboardType == EBilboardType::NONE))
	{
		m_eBilboardType = EBilboardType::NONE;
	}

	ImGui::Checkbox("Use Main Curve", &m_bUseMainCurve);

	if(m_bUseMainCurve == true)
	{
		static _int	iSelectCurve = -1;

		IMGUI_LEFT_LABEL(ImGui::Combo, "Target Curve", &iSelectCurve, m_szCurveTag, CURVE_END);
		ImGui::SameLine();

		if (ImGui::Button("Add Target Curve"))
		{
			if (iSelectCurve == -1 || iSelectCurve > CURVE_END)
			{
				MSG_BOX("Wrong Curve Number");
				return;
			}

			AddTargetCurve(m_szCurveTag[iSelectCurve]);

			iSelectCurve = -1;
		}

		ImGui::BulletText("Timeline_Options");
		ImGui::SliderFloat("Set_EndTime", &m_fEndTime, 0.f, 30.f);
		ImGui::SameLine();
		if (ImGui::Button("Set_End"))
		{
			m_Timeline.SetTimelineLength(m_fEndTime);
		}

		ImGui::Combo("FinishFunc", &m_iSelectFinishFunc, m_szFuncName, FUNC_END);
		ImGui::SameLine();
		if (ImGui::Button("Set_Fin-Function"))
		{
			if (m_iSelectFinishFunc == 0)
			{
				m_Timeline.SetFinishFunction(&m_Timeline, &CTimeline::PlayFromStart);
			}
			else if (m_iSelectFinishFunc == 1)
			{
				m_Timeline.SetFinishFunction(&m_Timeline, &CTimeline::Reset);
			}
			else if (m_iSelectFinishFunc == 2)
			{
				m_Timeline.SetFinishFunction(&m_Timeline, &CTimeline::Stop);
			}
			else if (m_iSelectFinishFunc == 3)
			{
				m_Timeline.SetFinishFunction(&m_Timeline, &CTimeline::Reverse);
			}
			else if (m_iSelectFinishFunc == 4)
			{
				m_Timeline.SetFinishFunction([this]
				{
					SetDelete();
				});
			}
		}

		m_Timeline.Imgui_RenderEditor();
		ImGui::BulletText("Particle Curve List");
		static const char* pCurName = "";
		if (ImGui::BeginListBox("##Particles Curve List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (auto pCurve : m_Curves)
			{
				const bool bSelected = strcmp(pCurve.first, pCurName) == 0;
				if (bSelected)
					ImGui::SetItemDefaultFocus();
				if (ImGui::Selectable(pCurve.first, bSelected))
					pCurName = pCurve.first;
			}
			ImGui::EndListBox();
		}

		auto itr = m_Curves.find(pCurName);

		if (itr != m_Curves.end())
		{
			itr->second->Imgui_RenderEditor();
		}
	}

	CImguiUtils::FileDialog_FileSelector("Save ParticleSystem to", ".json", "../Bin/Resources/VFX/ParticleSystem/", [this](const string& filePath)
	{
		Json json;
		SaveToJson(json);
		std::ofstream file(filePath);
		file << json;
	});
}

HRESULT CParticleSystem::SetParams()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShader, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	const _int bLocal = m_bLocal ? 1 : 0;
	if (FAILED(m_pShader->Set_RawValue("g_bLocal", &bLocal, sizeof(_int))))
		return E_FAIL;

	if (m_pMeshInstanceBuffer == nullptr && m_eBufferType == EBufferType::POINT)
	{
		const _int bRotate = (_int)m_eBilboardType;
		if (FAILED(m_pShader->Set_RawValue("g_bRotate", &bRotate, sizeof(_int))))
			return E_FAIL;
		if (FAILED(m_pShader->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4))))
			return E_FAIL;
		const _int bSizeDecreaseByLife = m_bSizeDecreaseByLife ? 1 : 0;
		if (FAILED(m_pShader->Set_RawValue("g_bSizeDecreaseByLife", &bSizeDecreaseByLife, sizeof(_int))))
			return E_FAIL;
		const _int bSizeIncreaseByLife = m_bSizeIncreaseByLife ? 1 : 0;
		if (FAILED(m_pShader->Set_RawValue("g_bSizeIncreaseByLife", &bSizeIncreaseByLife, sizeof(_int))))
			return E_FAIL;
		if (FAILED(m_pShader->Set_RawValue("g_fIncreaseMaxSize", &m_fIncreaseMaxSize, sizeof(_float))))
			return E_FAIL;
	}

	m_pShader->Set_Params(m_tParam);

	return S_OK;
}

HRESULT CParticleSystem::Begin()
{
	_uint iPassIndex = m_tParam.iPass;

	if (0 <= iPassIndex && iPassIndex < m_pShader->GetNumPasses())
		m_tParam.iPass = iPassIndex;
	else
		m_tParam.iPass = 0;

	return m_pShader->Begin(m_tParam.iPass);
}

void CParticleSystem::Start_Timeline()
{
	m_VtxStartTimeline.PlayFromStart();
	m_VtxRangeTimeline.PlayFromStart();
}

void CParticleSystem::AddPoint()
{/*
	 * pos.xyz = 로컬 또는 월드 위치
	 * pos.w = 현재 life(시간)
	 * up.w = 총 life
	 * up.xy = p사이즈 xy
	 * up.z = cam과의 거리(alpha 소팅용)
	 *
	 * look.xyz : 이동 방향
	 * look.w : 이동 속력
	 */
	
	for (_uint i = 0; i < (_uint)m_iBurstCnt; ++i)
	{
		if (m_PointList.size() >= m_iInstanceNum)
			return;

		VTXMATRIX pointData;

		_float fLife = m_fLife;
		_float fSpeed = m_fSpeed;
		_float4 vDir;
		_float4 vPos;

		pointData.vUp.x = m_fSize.x;
		pointData.vUp.y = m_fSize.y;

		if(m_bRotate == true)
		{
			pointData.vRotPos.x = CGameUtils::GetRandVector3(m_fRotationToTime_Min, m_fRotationToTime_Max).x;
			pointData.vRotPos.y = CGameUtils::GetRandVector3(m_fRotationToTime_Min, m_fRotationToTime_Max).y;
			pointData.vRotPos.z = CGameUtils::GetRandVector3(m_fRotationToTime_Min, m_fRotationToTime_Max).z;
		}

		if(m_bGravity == true)
			pointData.fGravityPower = CGameUtils::GetRandFloat(m_fGravityPowerMinMax.x, m_fGravityPowerMinMax.y);

		if (m_bLifeRand)
			fLife = CGameUtils::GetRandFloat(m_fLifeMinMax.x, m_fLifeMinMax.y);
		if (m_bSpeedRand)
			fSpeed = CGameUtils::GetRandFloat(m_fSpeedMinMax.x, m_fSpeedMinMax.y);

		if (m_eShape == ESpawnShape::SPHERE && m_bCurveDir == false)
		{
			if (m_bSphereDetail == false)
			{
				const _float3 vRandDir = CGameUtils::GetRandVector3Sphere(_float3::Zero, 1.f);
				vDir.x = vRandDir.x;
				vDir.y = vRandDir.y;
				vDir.z = vRandDir.z;
			}
			else
			{
				const _float3 vRandDir = CGameUtils::GetRandVector3(m_vRandDir_Min, m_vRandDir_Max);

				const _float vRandDirX = CGameUtils::GetRandFloat(m_vRandDir_Min.x, m_vRandDir_Max.x);
				const _float vRandDirY = CGameUtils::GetRandFloat(m_vRandDir_Min.y, m_vRandDir_Max.y);
				const _float vRandDirZ = CGameUtils::GetRandFloat(m_vRandDir_Min.z, m_vRandDir_Max.z);

				vDir.x = vRandDirX;
				vDir.y = vRandDirY;
				vDir.z = vRandDirZ;
			}
			if (m_bLocal)
			{
				vPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
			}
			else
			{
				vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			}

			if (m_bFromOrigin == false)
			{
				const _float3 vRanPos = CGameUtils::GetRandVector3Sphere(_float3::Zero, m_fSphereRadius);
				vPos.x += vRanPos.x;
				vPos.y += vRanPos.y;
				vPos.z += vRanPos.z;

				if(m_bSphereDetail == true)
				{
					const _float3 vRanPos = CGameUtils::GetRandVector3Sphere(m_vRandDir_Min, m_vRandDir_Max.x);
					vPos.x += vRanPos.x;
					vPos.y += vRanPos.y;
					vPos.z += vRanPos.z;
				}
			}
		}
		else if (m_eShape == ESpawnShape::CONE && m_bCurveDir == false)
		{
			_vector vYAxis_RandomDir = CMathUtils::RandomUnitVectorInCone_AxisY(m_fConeAngleDeg);
			vDir = XMVector3TransformNormal(vYAxis_RandomDir, m_pTransformCom->Get_WorldMatrix());

			_vector vRandPos = CMathUtils::RandomPosVectorInCircle_FromZero(m_fConeOriginRadius);

			if (m_bLocal)
			{
				vPos = XMVector3TransformNormal(vRandPos, m_pTransformCom->Get_WorldMatrix());
			}
			else
			{
				vPos = XMVector3TransformCoord(vRandPos, m_pTransformCom->Get_WorldMatrix());
			}
		}

		if (m_bUseMeshData == true && m_pModel != nullptr && m_bMeshCurve == false)
		{
			_uint VBSize = m_pModel->Get_NumVertices();

			const VTXMODEL* pNonAnimBuffer = m_pModel->Get_NonAnimBuffer();
			Assert(VBSize > 0);
			_uint iRandomVtx = (_uint)CGameUtils::GetRandFloat(0.f, (_float)(VBSize - 1));
			
			_float3 v3Pos = pNonAnimBuffer[iRandomVtx].vPosition;
			vPos = _float4{ v3Pos.x, v3Pos.y, v3Pos.z, 1.f };

			if (m_bLocal == false)
			{
				vPos = XMVector3TransformCoord(vPos, m_pTransformCom->Get_WorldMatrix());
			}
		}

		if (m_pMeshCurveModel != nullptr && m_bMeshCurve == true)
		{
			_uint VBSize = m_pMeshCurveModel->Get_NumVertices();
			const VTXMODEL* pNonAnimBuffer = m_pMeshCurveModel->Get_NonAnimBuffer();
			Assert(VBSize > 0);

			if (m_fMeshVtxStartRatio <= 0.f)
				m_fMeshVtxStartRatio = 0.f;
			if (m_fMeshVtxRangeRatio <= 0.f)
				m_fMeshVtxRangeRatio = 0.f;


			_float fStartIdx = m_fMeshVtxStartRatio * VBSize;
			_float fEndIdx = fStartIdx + m_fMeshVtxRangeRatio * VBSize;
			if (fEndIdx > (_float)VBSize - 1.f)
			{
				fEndIdx = (_float)VBSize - 1.f;
			}

			_uint iRandomVtx = (_uint)CGameUtils::GetRandFloat(fStartIdx, fEndIdx);
			

			_float3 v3Pos = pNonAnimBuffer[iRandomVtx].vPosition;
			vPos = _float4{ v3Pos.x, v3Pos.y, v3Pos.z, 1.f };

			if (m_bLocal == false)
			{
				vPos = XMVector3TransformCoord(vPos, m_pTransformCom->Get_WorldMatrix());
			}

			if (iRandomVtx > 0)
			{
				_float4 vPrePos = _float4{ pNonAnimBuffer[iRandomVtx - 1].vPosition.x, pNonAnimBuffer[iRandomVtx - 1].vPosition .y, pNonAnimBuffer[iRandomVtx - 1].vPosition .z, 1.f};

				if (m_bLocal == false)
				{
					vPrePos = XMVector3TransformCoord(vPrePos, m_pTransformCom->Get_WorldMatrix());
				}

				_float4 vUVDir = vPos - vPrePos;
				vUVDir.Normalize();
				vDir = vUVDir;
			}
		}
		
		pointData.vUp.w = fLife;

		pointData.vLook = vDir * fSpeed;

		pointData.vPosition = vPos;
		pointData.vPosition.w = 0.f;

		m_PointList.push_back(pointData);
	}
}

void CParticleSystem::UpdatePoints(_float fTimeDelta)
{
	for (auto& data : m_PointList)
	{
		data.vPosition.w += fTimeDelta;
		_float4 vNewPos;

		vNewPos = data.vPosition + data.vLook  * fTimeDelta;
		vNewPos.w = data.vPosition.w;

		if (m_bGravity == true)
		{
			data.vLook.y -= data.fGravityPower * fTimeDelta;

			if (data.vLook.y <= -20.f)
			{
				data.vLook.y = -20.f;
			}
		}

		data.vPosition = vNewPos;

		if (m_bRotate == true)
		{
			_float4 FRight = data.vRotPos * fTimeDelta;
			_float4 FUp = data.vRotPos * fTimeDelta;
			_float4 FLook = data.vRotPos * fTimeDelta;

			data.vRotRight =  FRight;
			data.vRotUp =  FUp;
			data.vRotLook = FLook;
		}



		if (m_bCurveDir)
		{
			_float fCurLife = data.vPosition.w;
			_float fLifeRatio = fCurLife;

			data.vLook.Normalize();

			if (auto pCurve = CCurveManager::GetInstance()->GetCurve(m_strXDirCurveName))
			{
				data.vLook.x += pCurve->GetValue(fLifeRatio) * 2.f - 1.f;
			}
			if (auto pCurve = CCurveManager::GetInstance()->GetCurve(m_strYDirCurveName))
			{
				data.vLook.y += pCurve->GetValue(fLifeRatio) * 2.f - 1.f;
			}
			if (auto pCurve = CCurveManager::GetInstance()->GetCurve(m_strZDirCurveName))
			{
				data.vLook.z += pCurve->GetValue(fLifeRatio) * 2.f - 1.f;
			}
			// data.vLook.Normalize();

			if(m_bLocal == true)
			{
				data.vLook *= CGameUtils::GetRandFloat(m_fSpeedMinMax.x, m_fSpeedMinMax.y);
			}
			else
			{
				const _matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
				data.vLook = XMVector3TransformNormal(data.vLook, WorldMatrix) * CGameUtils::GetRandFloat(m_fSpeedMinMax.x, m_fSpeedMinMax.y);
			}
		}

	}

	m_PointList.remove_if([this](const VTXMATRIX& data)
	{
		return data.vPosition.w >= data.vUp.w;
	});

	_vector vCamPos = CGameInstance::GetInstance()->Get_CamPosition();

	if (m_bLocal)
	{
		const _matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		for (auto& instanceData : m_PointList)
		{
			_vector vPos = instanceData.vPosition;
			vPos = XMVector3TransformCoord(vPos, WorldMatrix);
			instanceData.vUp.z = XMVectorGetX(XMVector3Length(vCamPos - vPos));
		}
	}
	else
	{
		for (auto& instanceData : m_PointList)
		{
			instanceData.vUp.z = XMVectorGetX(XMVector3Length(vCamPos - XMLoadFloat4(&instanceData.vPosition)));
		}
	}

	m_PointList.sort([](const VTXMATRIX& left, const VTXMATRIX& right)->_bool
	{
		return left.vUp.z > right.vUp.z;
	});
}


void CParticleSystem::AddMesh()
{
	/*
	* pos.xyz = 로컬 또는 월드 위치
	* pos.w = 현재 life(시간)
	* up.w = 총 life
	* up.xy = p사이즈 xy
	* up.z = cam과의 거리(alpha 소팅용)
	*
	* look.xyz : 이동 방향
	* look.w : 이동 속력
	*/
	for (_uint i = 0; i < (_uint)m_iBurstCnt; ++i)
	{
		if (m_MeshList.size() >= m_iInstanceNum)
			return;

		VTXINSTANCE MeshData;

		_float fLife = m_fLife;
		_float fSpeed = m_fSpeed;
		_float3 vDir;
		_float4 vPos;

		if (m_bRotate == true)
		{
			MeshData.vEulerRad.x = CMathUtils::RandomFloat(0.f, XM_PI);
			MeshData.vEulerRad.y = CMathUtils::RandomFloat(0.f, XM_PI);
			MeshData.vEulerRad.z = CMathUtils::RandomFloat(0.f, XM_PI);
		}

		if (m_bGravity == true)
			MeshData.fGravityPower = CGameUtils::GetRandFloat(m_fGravityPowerMinMax.x, m_fGravityPowerMinMax.y);

		if (m_bLifeRand)
			fLife = CGameUtils::GetRandFloat(m_fLifeMinMax.x, m_fLifeMinMax.y);
		if (m_bSpeedRand)
			fSpeed = CGameUtils::GetRandFloat(m_fSpeedMinMax.x, m_fSpeedMinMax.y);

		if (m_eShape == ESpawnShape::SPHERE)
		{
			const _float3 vRandDir = CGameUtils::GetRandVector3Sphere(_float3::Zero, 1.f);
			vDir.x = vRandDir.x;
			vDir.y = vRandDir.y;
			vDir.z = vRandDir.z;

			if (m_bLocal)
			{
				vPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
			}
			else
			{
				vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			}

			if (m_bFromOrigin == false)
			{
				const _float3 vRanPos = CGameUtils::GetRandVector3Sphere(_float3::Zero, m_fSphereRadius);
				vPos.x += vRanPos.x;
				vPos.y += vRanPos.y;
				vPos.z += vRanPos.z;
			}
		}
		if (m_eShape == ESpawnShape::CONE)
		{
			_vector vYAxis_RandomDir = CMathUtils::RandomUnitVectorInCone_AxisY(m_fConeAngleDeg);
			vDir = XMVector3TransformNormal(vYAxis_RandomDir, m_pTransformCom->Get_WorldMatrix());

			_vector vRandPos = CMathUtils::RandomPosVectorInCircle_FromZero(m_fConeOriginRadius);

			if (m_bLocal)
			{
				vPos = XMVector3TransformNormal(vRandPos, m_pTransformCom->Get_WorldMatrix());
			}
			else
			{
				vPos = XMVector3TransformCoord(vRandPos, m_pTransformCom->Get_WorldMatrix());
			}
		}

		if (m_bUseMeshData == true && m_pModel != nullptr)
		{
			_uint VBSize = m_pModel->Get_NumVertices();

			const VTXMODEL* pNonAnimBuffer = m_pModel->Get_NonAnimBuffer();
			Assert(VBSize > 0);
			_uint iRandomVtx = (_uint)CGameUtils::GetRandFloat(0.f, (_float)(VBSize - 1));

			_float3 v3Pos = pNonAnimBuffer[iRandomVtx].vPosition;
			vPos = _float4{ v3Pos.x, v3Pos.y, v3Pos.z, 1.f };
			// Fix_Dead Flower 
			if (m_bLocal)
			{
				vPos = XMVector3TransformNormal(vPos, m_pTransformCom->Get_WorldMatrix());
			}
			else
			{
				vPos = XMVector3TransformCoord(vPos, m_pTransformCom->Get_WorldMatrix());
			}
		}

		MeshData.vPosition = vPos;
		MeshData.vVelocity = vDir * fSpeed;
		MeshData.vSize = _float3(m_vMeshSize.x, m_vMeshSize.y, m_vMeshSize.z);
		MeshData.fLifeTime = fLife;
		MeshData.fCurLifeTime = 0.f;

		MeshData.vRight = _float4(MeshData.vSize.x, 0.f, 0.f, 0.f);
		MeshData.vUp = _float4(0.f, MeshData.vSize.y, 0.f, 0.f);
		MeshData.vLook = _float4(0.f, 0.f, MeshData.vSize.z, 0.f);

		if (m_bPhysX)
		{
			Json json;
			json["RigidBody"]["bKinematic"] = false;
			json["RigidBody"]["bTrigger"] = false;
			json["RigidBody"]["Density"] = 10;
			json["RigidBody"]["ColliderType"] = CT_PARTICLE;
			json["RigidBody"]["OriginTransform"] = _float4x4::CreateScale(MeshData.vSize);
			json["RigidBody"]["ShapeType"] = CRigidBody::TYPE_BOX;
			auto pRigidBody = dynamic_cast<CRigidBody*>(CGameInstance::GetInstance()->Clone_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", &json));
			pRigidBody->SetPxWorldMatrix(_float4x4::CreateTranslation(MeshData.vPosition.x, MeshData.vPosition.y, MeshData.vPosition.z));
			pRigidBody->BeginTick();


			pRigidBody->AddVelocity(MeshData.vVelocity);
			pRigidBody->AddTorque(MeshData.vVelocity * 0.00001f);

			MeshData.pRigidBody = pRigidBody;
			Assert(pRigidBody != nullptr);
		}

		
		m_MeshList.push_back(MeshData);
	}
}

void CParticleSystem::UpdateMeshes(_float fTimeDelta)
{
	for (auto& data : m_MeshList)
	{
		data.fCurLifeTime += fTimeDelta;

		if (m_bPhysX == false)
		{
			_float4 MovePos = data.vPosition + (data.vVelocity * fTimeDelta);

			if (m_bSizeDecreaseByLife == true)
			{
				data.vSize = data.vSize - (m_vScaleVariation * fTimeDelta);
			}
			else
			{
				data.vSize = data.vSize + (m_vScaleVariation * fTimeDelta);
			}

			if (m_bGravity == true)
			{
				data.vVelocity.y -= data.fGravityPower * fTimeDelta;

				if (data.vVelocity.y <= -20.f)
				{
					data.vVelocity.y = -20.f;
				}
			}

			if (m_bRotate)
			{
				_matrix rot = XMMatrixRotationRollPitchYaw(data.vEulerRad.x * fTimeDelta, data.vEulerRad.y * fTimeDelta, data.vEulerRad.z * fTimeDelta);

				data.vRight = XMVector3Normalize(XMVector4Transform(data.vRight, rot)) * data.vSize.x;
				data.vUp = XMVector3Normalize(XMVector4Transform(data.vUp, rot)) *data.vSize.y;
				data.vLook = XMVector3Normalize(XMVector4Transform(data.vLook, rot)) *data.vSize.z;
				data.vPosition = _float4(MovePos.x, MovePos.y, MovePos.z, 1.f);
			}

			data.vRight = XMVector3Normalize(data.vRight) * data.vSize.x;
			data.vUp = XMVector3Normalize(data.vUp) *data.vSize.y;
			data.vLook = XMVector3Normalize(data.vLook) * data.vSize.z;
			data.vPosition = _float4(MovePos.x, MovePos.y, MovePos.z, 1.f);
		}

		data.vColor = _float4(data.fCurLifeTime, data.fLifeTime, 0.f, 0.f);

		if (data.fCurLifeTime >= data.fLifeTime)
		{
			Safe_Release(data.pRigidBody);
		}
	}

	m_MeshList.remove_if([this](const VTXINSTANCE& data)
	{
		return data.fCurLifeTime >= data.fLifeTime;
	});

	m_MeshList.sort([](const VTXINSTANCE& left, const VTXINSTANCE& right)->_bool
	{
		return left.vUp.z > right.vUp.z;
	});
}

void CParticleSystem::AddTargetCurve(const string& strCurveName, CCurveFloatImpl* pCurve)
{
	if(pCurve == nullptr)
		pCurve = CCurveFloatImpl::Create(strCurveName);

	auto itr = m_Curves.find(strCurveName.c_str());
	if (itr != m_Curves.end())
		return;

	m_Curves.emplace(pCurve->GetName(), pCurve);

	if (strCurveName == "ObjectScale_All")
	{
		m_Timeline.SetCurve(this, &CParticleSystem::Tick_Scale_All, m_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "ObjectScale_Y")
	{
		m_Timeline.SetCurve(this, &CParticleSystem::Tick_Scale_Y, m_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_Curves.find(strCurveName.c_str())->second);
	}
	else if (!strcmp(pCurve->GetName(), "ObjectScale_X"))
	{
		m_Timeline.SetCurve(this, &CParticleSystem::Tick_Scale_X, m_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_0")
	{
		m_Timeline.SetCurve(this, &CParticleSystem::Tick_Floats_0, m_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_1")
	{
		m_Timeline.SetCurve(this, &CParticleSystem::Tick_Floats_1, m_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_2")
	{
		m_Timeline.SetCurve(this, &CParticleSystem::Tick_Floats_2, m_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_3")
	{
		m_Timeline.SetCurve(this, &CParticleSystem::Tick_Floats_3, m_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_4")
	{
		m_Timeline.SetCurve(this, &CParticleSystem::Tick_Floats_4, m_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_5")
	{
		m_Timeline.SetCurve(this, &CParticleSystem::Tick_Floats_5, m_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_6")
	{
		m_Timeline.SetCurve(this, &CParticleSystem::Tick_Floats_6, m_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_7")
	{
		m_Timeline.SetCurve(this, &CParticleSystem::Tick_Floats_7, m_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_8")
	{
		m_Timeline.SetCurve(this, &CParticleSystem::Tick_Floats_8, m_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_9")
	{
		m_Timeline.SetCurve(this, &CParticleSystem::Tick_Floats_9, m_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Color_Change")
	{
		m_Timeline.SetCurve(this, &CParticleSystem::Tick_ColorChange, m_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Ints_0")
	{
		m_Timeline.SetCurve(this, &CParticleSystem::Tick_Ints_0, m_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_Curves.find(strCurveName.c_str())->second);
	}
	else
	{
		MSG_BOX("Failed to SetCurve");
	}
}

void CParticleSystem::Tick_Scale_All(_float fValue)
{
	fValue *= 2.f;

	m_pTransformCom->Set_Scaled(_float3(fValue, fValue, fValue));
}

 void CParticleSystem::Tick_Scale_Y(_float fValue)
{
	_float3 Scale = m_pTransformCom->Get_Scaled();
	 	
	m_pTransformCom->Set_Scaled(_float3(Scale.x, fValue, Scale.x));
}

void CParticleSystem::Tick_Scale_X(_float fValue)
{
	_float3 Scale = m_pTransformCom->Get_Scaled();

	m_pTransformCom->Set_Scaled(_float3(fValue, Scale.y, Scale.x));
}

 void CParticleSystem::Tick_Floats_0(_float fValue)
{
	 m_tParam.Floats[0] = fValue;
 }

 void CParticleSystem::Tick_Floats_1(_float fValue)
{
	 m_tParam.Floats[1] = fValue;
 }

void CParticleSystem::Tick_Floats_2(_float fValue)
{
	m_tParam.Floats[2] = fValue;
}

void CParticleSystem::Tick_Floats_3(_float fValue)
{
	m_tParam.Floats[3] = fValue;
}

void CParticleSystem::Tick_Floats_4(_float fValue)
{
	m_tParam.Floats[4] = fValue;
}

void CParticleSystem::Tick_Floats_5(_float fValue)
{
	m_tParam.Floats[5] = fValue;
}

void CParticleSystem::Tick_Floats_6(_float fValue)
{
	m_tParam.Floats[6] = fValue;
}

void CParticleSystem::Tick_Floats_7(_float fValue)
{
	m_tParam.Floats[7] = fValue;
}
void CParticleSystem::Tick_Floats_8(_float fValue)
{
	m_tParam.Floats[8] = fValue;
}
void CParticleSystem::Tick_Floats_9(_float fValue)
{
	m_tParam.Floats[9] = fValue;
}

void CParticleSystem::Tick_ColorChange(_float fValue)
{
	if (m_tParam.Float4s.size() < 2)
		return;

	_float4 vChangeColor = m_tParam.Float4s[1];

	_float4 vOutColor = m_tParam.Float4s[0] * (1.f - fValue);
	_float4 vInColor = vChangeColor * (fValue);

	m_tParam.Float4s[0] = vOutColor + vInColor;
}


void CParticleSystem::Tick_Ints_0(_float fValue)
{
	m_tParam.Ints[0] = (_int)fValue;
}

CParticleSystem* CParticleSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticleSystem*		pInstance = new CParticleSystem(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CParticleSystem");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CParticleSystem::Clone(void* pArg)
{
	CParticleSystem*		pInstance = new CParticleSystem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CParticleSystem");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CParticleSystem::Free()
{
	__super::Free();

	for (auto pCurve : m_Curves)
		Safe_Release(pCurve.second);
	m_Curves.clear();

	m_PointList.clear();
	for (auto& meshInst : m_MeshList)
		Safe_Release(meshInst.pRigidBody);
	m_MeshList.clear();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pPointInstanceBuffer);
	Safe_Release(m_pMeshInstanceBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pMeshCurveModel);


	for (auto e : m_tParam.Textures)
		Safe_Release(e.first);
}


