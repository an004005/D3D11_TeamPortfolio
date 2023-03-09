#include "stdafx.h"
#include "..\public\ParticleSystem.h"
#include <ImguiUtils.h>
#include <MathUtils.h>

#include "VIBuffer_Point_Instancing.h"
#include "GameInstance.h"
#include "GameUtils.h"
#include "JsonLib.h"

CParticleSystem::CParticleSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CParticleSystem::CParticleSystem(const CParticleSystem& rhs)
	: CGameObject(rhs)
{
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

		m_pPointInstanceBuffer = CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, m_iInstanceNum);
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

			m_pPointInstanceBuffer = CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, m_iInstanceNum);
		}
	}

	return S_OK;
}

void CParticleSystem::Tick(_double TimeDelta)
{
	const _float fTimeDelta = (_float)TimeDelta;

	if (m_bLoop == false)
		m_fDuration -= fTimeDelta;
	if (m_fDuration < 0.f)
		m_bDelete = true;

	if(m_eBufferType == EBufferType::MESH)
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
}

void CParticleSystem::Late_Tick(_double TimeDelta)
{
	if (m_bVisible)
	{
		if(m_eBufferType == EBufferType::MESH)
		{
			m_pMeshInstanceBuffer->SetInstanceBuffer(&m_MeshList);
		}
		else
		{
			m_pPointInstanceBuffer->SetInstanceBuffer(&m_PointList);
			// particle은 작기 때문에 먼저 해도 문제없을 듯
		}
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}
}

HRESULT CParticleSystem::Render()
{
	if (m_pShader == nullptr || m_pPointInstanceBuffer == nullptr && m_pMeshInstanceBuffer == nullptr)
		return S_OK;

	FAILED_CHECK(SetParams());
	FAILED_CHECK(Begin());

	if (m_eBufferType == EBufferType::MESH)
		m_pMeshInstanceBuffer->Render();
	else
		m_pPointInstanceBuffer->Render();

	return S_OK;
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
}

void CParticleSystem::LoadFromJson(const Json& json)
{
	CGameObject::LoadFromJson(json);

	// Json& json = *static_cast<Json*>(json);
	CShader::LoadShaderParam(m_tParam, json);

	// m_iPassIdx = json["Pass"];
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

	if(m_eBufferType == EBufferType::MESH)
	{
		if (json.contains("MeshBufferProtoTag"))
		{
			m_MeshBufferProtoTag = json["MeshBufferProtoTag"];

			FAILED_CHECK(Add_Component(LEVEL_NOW, CGameUtils::s2ws(m_MeshBufferProtoTag).c_str(), TEXT("Mesh_Instance_Buffer"), (CComponent**)&m_pMeshInstanceBuffer));
			FAILED_CHECK(Add_Component(LEVEL_NOW, CGameUtils::s2ws(m_ShaderProtoTag).c_str(), TEXT("Mesh_Instance_Shader"), (CComponent**)&m_pShader));
		}
	}
	
	// else
	// {
	// 	m_pPointInstanceBuffer = CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, m_iInstanceNum);
	// }

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


}

void CParticleSystem::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();

	ImGui::Separator();
	CShader::Imgui_RenderShaderParams(m_tParam);
	ImGui::Separator();

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
	ImGui::Separator();

	if (ImGui::RadioButton("Point Instance", m_eBufferType == EBufferType::POINT))
		m_eBufferType = EBufferType::POINT;
	ImGui::SameLine();
	if (ImGui::RadioButton("Mesh Instance", m_eBufferType == EBufferType::MESH))
		m_eBufferType = EBufferType::MESH;
	ImGui::Separator();

	ImGui::InputFloat("Duration", &m_fDuration);
	ImGui::Checkbox("Loop", &m_bLoop);
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
		ImGui::InputFloat2("Mesh Size", (float*)&m_fSize);

		CImguiUtils::InputFloat3(&m_vScaleVariation, "Mesh ScaleVariation");
	}
	else
		ImGui::InputFloat2("P Size", (float*)&m_fSize);




	ImGui::Separator();
	ImGui::Separator();

	ImGui::NewLine();

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
		m_eShape = ESpawnShape::SPHERE;
	ImGui::SameLine();
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

	const _int bSizeDecreaseByLife = m_bSizeDecreaseByLife ? 1 : 0;
	if (FAILED(m_pShader->Set_RawValue("g_bSizeDecreaseByLife", &bSizeDecreaseByLife, sizeof(_int))))
		return E_FAIL;


	if (m_bPointInstance)
	{
		const _int bRotate = m_bRotate ? 1 : 0;
		if (FAILED(m_pShader->Set_RawValue("g_bRotate", &bRotate, sizeof(_int))))
			return E_FAIL;

		if (FAILED(m_pShader->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4))))
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

		_float4 vNewPos = data.vPosition + data.vLook  * fTimeDelta;
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

		_matrix RotationMatrix = XMMatrixIdentity();

		if (m_bRotate == true)
		{
			// RotationMatrix = XMMatrixRotationX(XMConvertToRadians(data.vRotPos.x * fTimeDelta * 5.f))
			// 	* XMMatrixRotationY(XMConvertToRadians(data.vRotPos.y * fTimeDelta * 5.f))
			// 	* XMMatrixRotationZ(XMConvertToRadians(data.vRotPos.z * fTimeDelta * 5.f));

			_float4 FRight = data.vRotPos * fTimeDelta;
			_float4 FUp = data.vRotPos * fTimeDelta;
			_float4 FLook = data.vRotPos * fTimeDelta;

			data.vRotRight =  FRight;
			data.vRotUp =  FUp;
			data.vRotLook = FLook;
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



		MeshData.vPosition = vPos;
		MeshData.vVelocity = vDir * fSpeed;
		MeshData.vSize = _float3(m_fSize.x, m_fSize.x, m_fSize.x);
		MeshData.fLifeTime = fLife;
		MeshData.fCurLifeTime = 0.f;

		MeshData.vRight = _float4(MeshData.vSize.x, 0.f, 0.f, 0.f);
		MeshData.vUp = _float4(0.f, MeshData.vSize.y, 0.f, 0.f);
		MeshData.vLook = _float4(0.f, 0.f, MeshData.vSize.z, 0.f);

		
		m_MeshList.push_back(MeshData);
	}
}

void CParticleSystem::UpdateMeshes(_float fTimeDelta)
{
	for (auto& data : m_MeshList)
	{
		data.fCurLifeTime += fTimeDelta;

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

		data.vColor = _float4(data.fCurLifeTime, data.fLifeTime, 0.f, 0.f);
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
	CGameObject::Free();

	// for(auto& iter : m_PointList)
	// {
	// 	Safe_Release(iter);
	// }
	// m_PointList.clear();
	//
	// for (auto& iter : m_MeshList)
	// {
	// 	Safe_Release(iter);
	// }
	// m_MeshList.clear();

	if (m_bCloned == true)
	{
		Safe_Release(m_pRendererCom);
		Safe_Release(m_pPointInstanceBuffer);
		Safe_Release(m_pMeshInstanceBuffer);
		Safe_Release(m_pShader);
	}

	for (auto e : m_tParam.Textures)
		Safe_Release(e.first);
}
