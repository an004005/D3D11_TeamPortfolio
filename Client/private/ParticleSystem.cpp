#include "stdafx.h"
#include "ParticleSystem.h"
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ParticleSystem"), TEXT("Shader"),
		(CComponent**)&m_pShader)))
		return E_FAIL;

	m_pBuffer = CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, m_iInstanceNum);


	if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
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
		m_bSizeDecreaseByLife = json["bSizeDecreaseByLife"];
		m_bSizeIncreaseByLife = json["bSizeIncreaseByLife"];
		m_eShape = json["Shape"];
		m_fConeAngleDeg = json["ConeDirDegree"];
		m_fConeOriginRadius = json["ConeOriginRadius"];
		m_fIncreaseMaxSize = json["IncreaseMaxSize"];
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

	UpdatePoints(fTimeDelta);

	m_fCurSpawnTick -= fTimeDelta;
	if (m_fCurSpawnTick <= 0.f)
	{
		m_fCurSpawnTick = m_fSpawnTickTime;
		AddPoint();
	}
}

void CParticleSystem::Late_Tick(_double TimeDelta)
{
	if (m_bVisible)
	{
		m_pBuffer->SetInstanceBuffer(&m_Points);
		// particle은 작기 때문에 먼저 해도 문제없을 듯
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
	}
}

HRESULT CParticleSystem::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShader, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	const _int bLocal = m_bLocal ? 1 : 0;
	if (FAILED(m_pShader->Set_RawValue("g_bLocal", &bLocal, sizeof(_int))))
		return E_FAIL;
	const _int bSizeDecreaseByLife = m_bSizeDecreaseByLife ? 1 : 0;
	if (FAILED(m_pShader->Set_RawValue("g_bSizeDecreaseByLife", &bSizeDecreaseByLife, sizeof(_int))))
		return E_FAIL;
	const _int bSizeIncreaseByLife = m_bSizeIncreaseByLife ? 1 : 0;
	if (FAILED(m_pShader->Set_RawValue("g_bSizeIncreaseByLife", &bSizeIncreaseByLife, sizeof(_int))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_fIncreaseMaxSize", &m_fIncreaseMaxSize, sizeof(_float))))
		return E_FAIL;
	

	m_pShader->Begin_Params(m_tParam);
	m_pBuffer->Render();
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
	json["bSizeDecreaseByLife"] = m_bSizeDecreaseByLife;
	json["bSizeIncreaseByLife"] = m_bSizeIncreaseByLife;
	json["Shape"] = m_eShape;
	json["ConeDirDegree"] = m_fConeAngleDeg;
	json["ConeOriginRadius"] = m_fConeOriginRadius;
	json["IncreaseMaxSize"] = m_fIncreaseMaxSize;
}

void CParticleSystem::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();

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

	ImGui::InputFloat2("P Size", (float*)&m_fSize);

	ImGui::Checkbox("From Origin", &m_bFromOrigin);
	ImGui::Checkbox("Is Local", &m_bLocal);
	ImGui::Checkbox("Size Decrease by life", &m_bSizeDecreaseByLife);
	ImGui::Checkbox("Size Increase by life", &m_bSizeIncreaseByLife);
	if (m_bSizeIncreaseByLife)
	{
		ImGui::InputFloat("Increase MaxSize", &m_fIncreaseMaxSize);
	}

	if (ImGui::RadioButton("Shape Sphere", m_eShape == ESpawnShape::SPHERE))
		m_eShape = ESpawnShape::SPHERE;
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

	ImGui::Separator();
	CShader::Imgui_RenderShaderParams(m_tParam);

	CImguiUtils::FileDialog_FileSelector("Save ParticleSystem to", ".json", "../Bin/Resources/VFX/ParticleSystem/", [this](const string& filePath)
	{
		Json json;
		SaveToJson(json);
		std::ofstream file(filePath);
		file << json;
	});
}

void CParticleSystem::AddPoint()
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
	for (_uint i = 0; i < m_iBurstCnt; ++i)
	{
		if (m_Points.size() >= m_iInstanceNum)
			return;

		VTXMATRIX pointData;

		_float fLife = m_fLife;
		_float fSpeed = m_fSpeed;
		_float4 vDir;
		_float4 vPos;

		pointData.vUp.x = m_fSize.x;
		pointData.vUp.y = m_fSize.y;

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
		pointData.vLook = vDir;
		pointData.vLook.w = fSpeed;
		pointData.vPosition = vPos;
		pointData.vPosition.w = 0.f;
		m_Points.push_back(pointData);
	}
}

void CParticleSystem::UpdatePoints(_float fTimeDelta)
{
	for (auto& data : m_Points)
	{
		data.vPosition.w += fTimeDelta;

		_float4 vNewPos = data.vPosition + data.vLook * data.vLook.w * fTimeDelta;
		vNewPos.w = data.vPosition.w;
		data.vPosition = vNewPos;
	}

	m_Points.remove_if([this](const VTXMATRIX& data)
	{
		return data.vPosition.w >= data.vUp.w;
	});

	_vector vCamPos = CGameInstance::GetInstance()->Get_CamPosition();

	if (m_bLocal)
	{
		const _matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		for (auto& instanceData : m_Points)
		{
			_vector vPos = instanceData.vPosition;
			vPos = XMVector3TransformCoord(vPos, WorldMatrix);
			instanceData.vUp.z = XMVectorGetX(XMVector3Length(vCamPos - vPos));
		}
	}
	else
	{
		for (auto& instanceData : m_Points)
		{
			instanceData.vUp.z = XMVectorGetX(XMVector3Length(vCamPos - XMLoadFloat4(&instanceData.vPosition)));
		}
	}

	m_Points.sort([](const VTXMATRIX& left, const VTXMATRIX& right)->_bool
	{
		return left.vUp.z > right.vUp.z;
	});
}

void CParticleSystem::Free()
{
	CGameObject::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pShader);

	for (auto e : m_tParam.Textures)
		Safe_Release(e.first);
}
