#include "stdafx.h"
#include "..\public\Light_Manager.h"
#include "Light.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Graphic_Device.h"

IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
{
}

CLight* CLight_Manager::Find_Light(const string& strLightTag)
{
	auto itr = m_Lights.find(strLightTag);
	if (itr == m_Lights.end())
		return nullptr;
	return itr->second;
}

CLight* CLight_Manager::Add_Light(const string& strLightTag, ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc)
{
	CLight*			pLight = CLight::Create(pDevice, pContext, LightDesc);

	if (nullptr == pLight)
		return nullptr;

	m_Lights.emplace(strLightTag, pLight);

	return pLight;
}

void CLight_Manager::Delete_Light(const string& strLightTag)
{
	auto pLight = Find_Light(strLightTag);
	if (pLight)
	{
		m_Lights.erase(strLightTag);
		Safe_Release(pLight);
	}
}

void CLight_Manager::Render_Light(CVIBuffer_Rect * pVIBuffer, CShader * pShader)
{
	if (m_pShadowCam)
	{
		m_ShadowLightView = m_pShadowCam->GetViewMatrix();
		m_ShadowLightProj = m_pShadowCam->GetProjMatrix();
	}

	for (auto& Light : m_Lights)
	{
		if (Light.second->GetType() == LIGHTDESC::TYPE_DIRECTIONAL)
		{
			if (m_pShadowCam)
				Light.second->SetDirection(m_pShadowCam->GetTransform()->Get_State(CTransform::STATE_LOOK));

			m_vDirectionalLightDir = XMVector3Normalize(Light.second->Get_LightDesc()->vDirection);
			m_vDirectionalLightColor = Light.second->Get_LightDesc()->vDiffuse;
		}


		Light.second->Render(pVIBuffer, pShader);
	}

	for (auto& Light : m_TempLights)
	{
		Light.first->Render(pVIBuffer, pShader);
	}
}

void CLight_Manager::Clear()
{
	for (auto pLight : m_Lights)
		Safe_Release(pLight.second);

	m_Lights.clear();

	for (auto Light : m_TempLights)
		Safe_Release(Light.first);
	m_TempLights.clear();

	Safe_Release(m_pShadowCam);
	m_pShadowCam = nullptr;
}

void CLight_Manager::AddLifePointLight(_float fLife, const _float4& vPos, _float fRange, const _float4& vColor)
{
	LIGHTDESC tDesc;
	ZeroMemory(&tDesc, sizeof(LIGHTDESC));
	tDesc.eType = LIGHTDESC::TYPE_POINT;
	tDesc.vPosition = vPos;
	tDesc.fRange = fRange;
	tDesc.vDiffuse = vColor;

	auto pLight = CLight::Create(
		CGraphic_Device::GetInstance()->GetDevice(),
		CGraphic_Device::GetInstance()->GetContext(),
		tDesc);
	m_TempLights.push_back({pLight, fLife});
}

void CLight_Manager::AddLifeCapsuleLight(_float fLife, const _float4& vStart, const _float4& vEnd, _float fRange, const _float4& vColor)
{
	LIGHTDESC tDesc;
	ZeroMemory(&tDesc, sizeof(LIGHTDESC));
	tDesc.eType = LIGHTDESC::TYPE_CAPSULE;
	tDesc.vCapsuleStart = vStart;
	tDesc.vCapsuleEnd = vEnd;
	tDesc.fRange = fRange;
	tDesc.vDiffuse = vColor;

	auto pLight = CLight::Create(
		CGraphic_Device::GetInstance()->GetDevice(),
		CGraphic_Device::GetInstance()->GetContext(),
		tDesc);
	m_TempLights.push_back({pLight, fLife});
}

void CLight_Manager::Tick(_double TimeDelta)
{
	for (auto& Light : m_TempLights)
		Light.second -= (_float)TimeDelta;

	m_TempLights.remove_if([](const pair<class CLight*, _float>& Light)
	{
		return Light.second <= 0.f;
	});
}

void CLight_Manager::SetShadowCam(CCamera* pShadowCam)
{
	Assert(pShadowCam != nullptr);
	Safe_Release(m_pShadowCam);
	m_pShadowCam = pShadowCam;
	Safe_AddRef(m_pShadowCam);
}

_float4 CLight_Manager::GetShadowCamLook()
{
	if (m_pShadowCam)
		return m_pShadowCam->GetTransform()->Get_State(CTransform::STATE_LOOK);
	return _float4(0.f, -1.f, 0.f, 0.f);
}

// void CLight_Manager::CascadeUpdate(_float4x4 CamWorlMatrix, _float4* vFustumCorners)
// {
// 	for (auto& iter : m_Lights)
// 	{
// 		if (LIGHTDESC::TYPE_DIRECTIONAL == iter.second->Get_LightDesc()->eType)
// 		{
// 			iter.second->Update(CamWorlMatrix, vFustumCorners);
// 		}
// 	}
// }

void CLight_Manager::Imgui_Render()
{
	if (m_pShadowCam)
	{
		if (ImGui::CollapsingHeader("Shadow Cam"))
		{
			m_pShadowCam->Imgui_RenderProperty();
			m_pShadowCam->Imgui_RenderComponentProperties();
		}
	}
	else
	{
		ImGui::Text("Shadow Cam not Set");
	}

	static string strSelectedLightTag;
	if (ImGui::BeginListBox("Lights"))
	{
		for (auto& Light : m_Lights)
		{
			const bool bSelected = strSelectedLightTag == Light.first;
			if (bSelected)
				ImGui::SetItemDefaultFocus();
			if (ImGui::Selectable(Light.first.c_str(), bSelected))
				strSelectedLightTag = Light.first;
		}

		ImGui::EndListBox();
	}


	if (ImGui::CollapsingHeader("Show Selected Light Properties"))
	{
		if (auto pLight = Find_Light(strSelectedLightTag))
		{
			pLight->Imgui_Render();
		}
	}

}

void CLight_Manager::Free()
{
	Clear();
}
