#include "stdafx.h"
#include "..\public\Light_Manager.h"
#include "Light.h"
#include "GameInstance.h"
#include "Camera.h"

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
		if (Light.second->GetType() == LIGHTDESC::TYPE_DIRECTIONAL && m_pShadowCam)
			Light.second->SetDirection(m_pShadowCam->GetTransform()->Get_State(CTransform::STATE_LOOK));

		Light.second->Render(pVIBuffer, pShader);
	}
}

void CLight_Manager::Clear()
{
	for (auto pLight : m_Lights)
		Safe_Release(pLight.second);

	m_Lights.clear();

	Safe_Release(m_pShadowCam);
	m_pShadowCam = nullptr;
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
