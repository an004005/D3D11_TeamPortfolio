#include "stdafx.h"
#include "TrailSystem.h"
#include "GameInstance.h"
#include "VIBuffer_Trail_Instancing.h"
#include "MathUtils.h"
#include "GameUtils.h"
#include "ImguiUtils.h"
#include "JsonLib.h"
#include "PlayerInfoManager.h"

CTrailSystem::CTrailSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTrailSystem::CTrailSystem(const CTrailSystem& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTrailSystem::Initialize(void* pArg)
{
	CGameObject::Initialize(pArg);

	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Trail_VFX"), TEXT("Shader"), (CComponent**)&m_pShaderCom));

	m_pBuffer = CVIBuffer_Trail_Instancing::Create(m_pDevice, m_pContext, 300);
	
	if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);

		m_fLife = json["Life"];
		m_fWidth = json["Width"];
		m_fSegmentSize = json["SegmentSize"];
		m_vColor = json["Color"];
		if (json.contains("Pass"))
			m_iPass = json["Pass"];

		wstring texProtoTag = CGameUtils::s2ws(json["Texture"]);
		FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, texProtoTag.c_str(), TEXT("Texture"),(CComponent**)&m_pTex));
		//FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("../Bin/Resources/Texture/VFX/T_ef_one_lin_043.png"), TEXT("Texture"), (CComponent**)&m_pTex));
	}
	else
	{
		// FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("../Bin/Resources/Texture/VFX/AlphaGradient/ring_glow.dds"), TEXT("Texture"),(CComponent**)&m_pTex));
		FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("../Bin/Resources/Texture/VFX/T_ef_one_lin_043.png"), TEXT("Texture"), (CComponent**)&m_pTex));

		
		m_fLife = 0.5f;
		m_fWidth = 1.2f;
		m_fSegmentSize = 0.1f;
		m_vColor = _float4(1.f, 68.f / 255.f, 51.f / 255.f, 1.f);
	}

	m_pTransformCom->SetTransformDesc(CTransform::TRANSFORMDESC{ 15.f, XMConvertToRadians(180.f) });

	return S_OK;
}

void CTrailSystem::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);
	m_pBuffer->Tick(TimeDelta);

	// SAS에 따른 트레일 색상값 변경
	list<ESASType>	TrailType = CPlayerInfoManager::GetInstance()->Get_PlayerSasList();

	m_vColor = _float4(215.f / 255.f, 66.f / 255.f, 248.f / 255.f, 1.f);

	const auto FireCheck = find(TrailType.begin(), TrailType.end(), ESASType::SAS_FIRE);
	if (FireCheck != TrailType.end())
		m_vColor = _float4(1.f, 180.f / 255.f, 45.f / 255.f, 1.f);

	//const auto ElecCheck = find(TrailType.begin(), TrailType.end(), ESASType::SAS_ELETRIC);
	//if (ElecCheck != TrailType.end())
	//	m_vColor = _float4(1.f, 180.f / 255.f, 45.f / 255.f, 1.f);
	// ~SAS에 따른 트레일 색상값 변경

	if (m_vPrePoses.size() > 3)
	{
		_vector vLerpPos = m_vPrePoses[m_vPrePoses.size() - 3];
		_vector vSourPos = m_vPrePoses.back();
		_vector vDestPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

		_vector vSourRight = m_vPreRight.back();
		_vector vDestRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

		for (_uint i = 0; i < 5; i++)
		{
			_vector vBefPos = XMVectorCatmullRom(vLerpPos, vSourPos, vDestPos, vDestPos, _float(i * 0.2f));
			_vector vAfterPos = XMVectorCatmullRom(vLerpPos, vSourPos, vDestPos, vDestPos, _float((i + 1) * 0.2f));
			_vector vBefRight = XMVectorLerp(vSourRight, vDestRight, _float(i * 0.2f));
			_vector vAfterRight = XMVectorLerp(vSourRight, vDestRight, _float((i + 1) * 0.2f));

			_vector v0 = vBefPos + (vBefRight * m_fWidth);
			_vector v1 = vAfterPos + (vAfterRight * m_fWidth);
			_vector v2 = vAfterPos - (vAfterRight * m_fWidth);
			_vector v3 = vBefPos - (vBefRight * m_fWidth);

			m_TrailPointList.push_back(v0);
			m_TrailPointList.push_back(v1);
			m_TrailPointList.push_back(v2);
			m_TrailPointList.push_back(v3);

			m_pBuffer->AddData(m_pTransformCom->Get_WorldMatrix());
		}

		_vector v0 = vSourPos + (vSourRight * m_fWidth);
		_vector v1 = vDestPos + (vDestRight * m_fWidth);
		_vector v2 = vDestPos - (vDestRight * m_fWidth);
		_vector v3 = vSourPos - (vSourRight * m_fWidth);

		m_TrailPointList.push_back(v0);
		m_TrailPointList.push_back(v1);
		m_TrailPointList.push_back(v2);
		m_TrailPointList.push_back(v3);

		m_pBuffer->AddData(m_pTransformCom->Get_WorldMatrix());
	}

	// 이전 위치와 이전 Right 벡터를 저장해두어 CatMull-Rom 연산에 사용
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	m_vPrePoses.push_back(vPos);
	
	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	m_vPreRight.push_back(vRight);

	if (m_vPrePoses.size() >= 5)
	{
		while (m_vPrePoses.size() > 5)
		{
			m_vPrePoses.erase(m_vPrePoses.begin());
			m_vPreRight.erase(m_vPreRight.begin());
		}
	}

	if (m_TrailPointList.size() > 40)
	{
		while (m_TrailPointList.size() > 40)
		{
			m_TrailPointList.pop_front();
		}
	}
}

//void CTrailSystem::Tick(_double TimeDelta)
//{
//	CGameObject::Tick(TimeDelta);
//	m_pBuffer->Tick(TimeDelta);
//
//	if (m_bActive == false)
//		return;
//
//	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
//	if (m_vPrePoses.empty() || m_vPrePoses.back() != vPos)
//		m_vPrePoses.push_back(vPos);
//
//	if (m_vPrePoses.size() >= 3)
//	{
//		while (m_vPrePoses.size() > 3)
//			m_vPrePoses.erase(m_vPrePoses.begin());
//
//		_vector vCamPos = CGameInstance::GetInstance()->Get_CamPosition();
//		_float fSplineLen = XMVectorGetX(XMVector3LengthEst(m_vPrePoses[m_vPrePoses.size() - 2] - m_vPrePoses[m_vPrePoses.size() - 1]));
//		_uint iSegmentCnt = static_cast<_uint>((fSplineLen / m_fSegmentSize));
//
//		_vector p0 = m_vPrePoses[m_vPrePoses.size() - 3];
//		_vector p1 = m_vPrePoses[m_vPrePoses.size() - 2];
//		_vector p2 = m_vPrePoses[m_vPrePoses.size() - 1];
//		_vector p3 = m_vPrePoses[m_vPrePoses.size() - 1];
//		_float fPreLife = 0.f;
//		if (m_pBuffer->GetDatas()->empty() == false)
//			fPreLife = m_pBuffer->GetDatas()->back().vPosition.w;
//
//		_vector vPrePos = p1;
//
//		for (_uint i = 0; i < iSegmentCnt; ++i)
//		{
//			_float fWeight = (_float)(i + 1) / (_float)iSegmentCnt;
//			_vector vSplinePos = XMVectorCatmullRom(p0, p1, p2, p3, fWeight);
//
//			_vector vRight = XMVectorLerp(m_vPreRight, m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fWeight);
//			_vector vLook = XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f));
//			_vector vUp = XMVector3Cross(vLook, vRight);
//			vSplinePos = XMVectorSetW(vSplinePos, CMathUtils::Lerp(fPreLife, m_fLife, fWeight));//life
//
//			_matrix TrailMatrix(vRight, vUp, vLook, vSplinePos);
//			m_pBuffer->AddData(TrailMatrix);
//
//			vPrePos = vSplinePos;
//		}
//	}
//	
//	m_iSegmentCnt = m_iSegmentCnt;
//	m_vPreRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
//}

// 소드트레일 적용 전 Tick
//void CTrailSystem::Tick(_double TimeDelta)
//{
//	CGameObject::Tick(TimeDelta);
//	m_pBuffer->Tick(TimeDelta);
//
//	if (m_bActive == false)
//		return;
//
//	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
//	if (m_vPrePoses.empty() || m_vPrePoses.back() != vPos)
//		m_vPrePoses.push_back(vPos);
//
//	if (m_vPrePoses.size() >= 3)
//	{
//		while (m_vPrePoses.size() > 3)
//			m_vPrePoses.erase(m_vPrePoses.begin());
//
//		_vector vCamPos = CGameInstance::GetInstance()->Get_CamPosition();
//		_float fSplineLen = XMVectorGetX(XMVector3LengthEst(m_vPrePoses[1] - m_vPrePoses[2]));
//		_uint iSegmentCnt = static_cast<_uint>((fSplineLen / m_fSegmentSize));
//
//		_vector p0 = m_vPrePoses[0];
//		_vector p1 = m_vPrePoses[1];
//		_vector p2 = m_vPrePoses[2];
//		_vector p3 = m_vPrePoses[2];
//		_float fPreLife = 0.f;
//		if (m_pBuffer->GetDatas()->empty() == false)
//			fPreLife = m_pBuffer->GetDatas()->back().vPosition.w;
//
//		_vector vPrePos = p1;
//
//		for (_uint i = 0; i < iSegmentCnt; ++i)
//		{
//			_float fWeight = (_float)(i + 1) / (_float)iSegmentCnt;
//			_vector vSplinePos = XMVectorCatmullRom(p0, p1, p2, p3, fWeight);
//
//			_vector vRight = XMVector3Normalize(vSplinePos - vPrePos);
//			_vector vLookAtCam = XMVector3Normalize(vCamPos - vSplinePos);
//
//			float fRadian = XMConvertToDegrees(fabs(acosf(XMVectorGetX(XMVector3Dot(vLookAtCam, vRight)))));
//			if (fRadian < 5.f)
//				continue;
//
//			_vector vUp = XMVector3Cross(vRight, vLookAtCam);
//			_vector vLook = XMVector3Cross(vRight, vUp);
//			vSplinePos = XMVectorSetW(vSplinePos, CMathUtils::Lerp(fPreLife, m_fLife, fWeight));//life
//
//			_matrix TrailMatrix(vRight, vUp, vLook, vSplinePos);
//			m_pBuffer->AddData(TrailMatrix);
//
//			vPrePos = vSplinePos;
//		}
//	}
//}


void CTrailSystem::Late_Tick(_double TimeDelta)
{
	if (m_bVisible/* && m_pBuffer->IsEmpty() == false*/)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CTrailSystem::Render()
{
	FAILED_CHECK(SetUp_ShaderResources());

	//m_pShaderCom->Begin(m_iPass);
	//m_pBuffer->Render();

	if (m_bActive == false)
		return S_OK;

	if (!m_TrailPointList.empty())
	{
		_vector vPoint[4];
		_uint iCount = 0;
		for (auto& iter : m_TrailPointList)
		{
			vPoint[iCount++] = iter;

			if (iCount == 4)
			{
				FAILED_CHECK(m_pShaderCom->Set_RawValue("g_TrailPoint", &vPoint, sizeof(_vector) * 4));

				m_pShaderCom->Begin(5);
				m_pBuffer->Render();

				iCount = 0;
			}
		}
	}

	return S_OK;
}

void CTrailSystem::SaveToJson(Json& json)
{
	json["Life"] = m_fLife;
	json["Width"] = m_fWidth;
	json["SegmentSize"] = m_fSegmentSize;
	json["Color"] = m_vColor;
	json["Pass"] = m_iPass;

	char texProtoTag[MAX_PATH];
	CGameUtils::wc2c(m_pTex->GetPrototypeTag(), texProtoTag);
	json["Texture"] = texProtoTag;
}

void CTrailSystem::Imgui_RenderProperty()
{
	ImGui::InputFloat("Life", &m_fLife);
	ImGui::InputFloat("Width", &m_fWidth);
	ImGui::InputFloat("Segment Size", &m_fSegmentSize);
	ImGui::ColorEdit4("Color", (float*)&m_vColor);

	_int iPass = m_iPass;
	ImGui::InputInt("Pass", &iPass);
	m_iPass = iPass < 0 ? 0 : iPass;

	CImguiUtils::FileDialog_FileSelector("Select AlphaTexture", ".png,.dds", "../Bin/Resources/Textures/VFX/",
		[this](const string& fileName)
	{
		size_t iPos = fileName.find("\\Bin\\Resources");
		string relative = ".." + fileName.substr(iPos, fileName.size() - iPos);

		Safe_Release(m_pTex);
		FAILED_CHECK(Delete_Component(L"Texture"));
		FAILED_CHECK(Add_Component(LEVEL_NOW, CGameUtils::s2ws(relative).c_str(), TEXT("Texture"),
			(CComponent**)&m_pTex));

	});

	CImguiUtils::FileDialog_FileSelector("Save This Trail", ".json", "../Bin/Resources/VFX/Trail/",
		[this](const string& filePath)
	{
		Json json;
		SaveToJson(json);
		std::ofstream file(filePath);
		file << json;
	});


	ImGui::Separator();
	CGameObject::Imgui_RenderProperty();
}

HRESULT CTrailSystem::SetUp_ShaderResources()
{
	FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)));

	auto pData = m_pBuffer->GetDatas();

	FAILED_CHECK(m_pShaderCom->Set_MatrixArray("g_TrailData", (_float4x4*)pData->data(), static_cast<_uint>(pData->size())));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_fLife", &m_fLife, sizeof(_float)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_fWidth", &m_fWidth, sizeof(_float)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_Color", &m_vColor, sizeof(_float4)));
	FAILED_CHECK(m_pTex->Bind_ShaderResource(m_pShaderCom, "g_AlphaTex"));

	return S_OK;
}

void CTrailSystem::ClearBuffer()
{
	m_pBuffer->ClearBuffer();
	m_vPrePoses.clear();
}

CTrailSystem* CTrailSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrailSystem*		pInstance = new CTrailSystem(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTrailSystem");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTrailSystem::Clone(void* pArg)
{
	CTrailSystem*		pInstance = new CTrailSystem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTrailSystem");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTrailSystem::Free()
{
	CGameObject::Free();
	Safe_Release(m_pBuffer);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTex);
}
