#include "stdafx.h"
#include "..\public\Light.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

CLight::CLight(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLight::Initialize(const LIGHTDESC & LightDesc)
{
	m_LightDesc = LightDesc;

	return S_OK;
}

HRESULT CLight::Render(CVIBuffer_Rect * pVIBuffer, CShader * pShader)
{
	_uint		iPassIndex = 1;

	if (LIGHTDESC::TYPE_DIRECTIONAL == m_LightDesc.eType)
	{
		if (FAILED(pShader->Set_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
			return E_FAIL;

		iPassIndex = 1;
	}

	else if (LIGHTDESC::TYPE_POINT == m_LightDesc.eType)
	{
		if (FAILED(pShader->Set_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(pShader->Set_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float))))
			return E_FAIL;

		iPassIndex = 2;
	}
	
	if (FAILED(pShader->Set_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Set_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Set_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4))))
		return E_FAIL;

	pShader->Begin(iPassIndex);

	pVIBuffer->Render();

	return S_OK;
}

void CLight::Imgui_Render()
{
	if (m_LightDesc.eType == LIGHTDESC::TYPE_DIRECTIONAL)
	{
		ImGui::Text("Directional Light");
		ImGui::Checkbox("Enable", &m_LightDesc.isEnable);
		ImGui::InputFloat4("Direction", (float*)&m_LightDesc.vDirection);

		ImGui::ColorEdit4("Diffuse", (float*)&m_LightDesc.vDiffuse, ImGuiColorEditFlags_PickerHueWheel);
		ImGui::ColorEdit4("Ambient", (float*)&m_LightDesc.vAmbient, ImGuiColorEditFlags_PickerHueWheel);
		ImGui::ColorEdit4("Speclar", (float*)&m_LightDesc.vSpecular, ImGuiColorEditFlags_PickerHueWheel);
	}
	else if (m_LightDesc.eType == LIGHTDESC::TYPE_POINT)
	{
		ImGui::Text("Point Light");
		ImGui::Checkbox("Enable", &m_LightDesc.isEnable);
		ImGui::InputFloat3("Pos", (float*)&m_LightDesc.vPosition);
		ImGui::InputFloat("Range", &m_LightDesc.fRange);

		ImGui::ColorEdit4("Diffuse", (float*)&m_LightDesc.vDiffuse, ImGuiColorEditFlags_PickerHueWheel);
		ImGui::ColorEdit4("Ambient", (float*)&m_LightDesc.vAmbient, ImGuiColorEditFlags_PickerHueWheel);
		ImGui::ColorEdit4("Speclar", (float*)&m_LightDesc.vSpecular, ImGuiColorEditFlags_PickerHueWheel);
	}
}

CLight * CLight::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc)
{
	CLight*		pInstance = new CLight(pDevice, pContext);

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX("CLight Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
