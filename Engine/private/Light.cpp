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

	// _vector vPos = XMLoadFloat4(&m_LightDesc.vPosition);
	// _vector vLook = XMLoadFloat4(&m_LightDesc.vDirection);
	// _vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	// _vector vUp = XMVector3Cross(vLook, vRight);
	//
	// _matrix TransformMatrix = XMMatrixIdentity();
	// TransformMatrix.r[0] = XMVector3Normalize(vRight);
	// TransformMatrix.r[1] = XMVector3Normalize(vUp);
	// TransformMatrix.r[2] = XMVector3Normalize(vLook);
	// TransformMatrix.r[3] = vPos;
	//
	// XMStoreFloat4x4(&m_TransformState[D3DTS_VIEW], XMMatrixInverse(nullptr, TransformMatrix));
	// XMStoreFloat4x4(&m_TransformState_TP[D3DTS_VIEW], XMMatrixTranspose(XMMatrixInverse(nullptr, TransformMatrix)));
	//
	// D3D11_VIEWPORT			ViewPortDesc;
	// ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	//
	// _uint		iNumViewports = 1;
	//
	// m_pContext->RSGetViewports(&iNumViewports, &ViewPortDesc);
	//
	// _matrix		ProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), ViewPortDesc.Width / ViewPortDesc.Height, 0.2f, m_LightDesc.fRange);
	// XMStoreFloat4x4(&m_TransformState[D3DTS_PROJ], ProjMatrix);
	// XMStoreFloat4x4(&m_TransformState_TP[D3DTS_PROJ], XMMatrixTranspose(ProjMatrix));

	return S_OK;
}

HRESULT CLight::Render(CVIBuffer_Rect * pVIBuffer, CShader * pShader)
{
	_uint		iPassIndex = 1;

	if (m_LightDesc.isEnable == false)
		return S_OK;

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
	else if (LIGHTDESC::TYPE_CAPSULE == m_LightDesc.eType)
	{
		if (FAILED(pShader->Set_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float))))
			return E_FAIL;
		if (FAILED(pShader->Set_RawValue("g_vCapsuleStart", &m_LightDesc.vCapsuleStart, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(pShader->Set_RawValue("g_vCapsuleEnd", &m_LightDesc.vCapsuleEnd, sizeof(_float4))))
			return E_FAIL;
		
		iPassIndex = 5;
	}
	// else if (LIGHTDESC::TYPE_FOV == m_LightDesc.eType)
	// {
	// 	if (FAILED(pShader->Set_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4))))
	// 		return E_FAIL;
	// 	if (FAILED(pShader->Set_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float))))
	// 		return E_FAIL;
	// 	if (FAILED(pShader->Set_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
	// 		return E_FAIL;
	//
	// 	// iPassIndex = 11; // TODO :: ¼öÁ¤
	// }
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

	}
	else if (m_LightDesc.eType == LIGHTDESC::TYPE_POINT)
	{
		ImGui::Text("Point Light");
		ImGui::Checkbox("Enable", &m_LightDesc.isEnable);
		ImGui::InputFloat3("Pos", (float*)&m_LightDesc.vPosition);
		ImGui::InputFloat("Range", &m_LightDesc.fRange);
	}
	else if (m_LightDesc.eType == LIGHTDESC::TYPE_CAPSULE)
	{
		ImGui::Text("Capsule Light");
		ImGui::Checkbox("Enable", &m_LightDesc.isEnable);
		ImGui::InputFloat("Capsule Radius", &m_LightDesc.fRange);
		ImGui::InputFloat4("Capsule Start", (float*)&m_LightDesc.vCapsuleStart);
		ImGui::InputFloat4("Capsule End", (float*)&m_LightDesc.vCapsuleEnd);
	}

	ImGui::ColorEdit4("Diffuse", (float*)&m_LightDesc.vDiffuse, ImGuiColorEditFlags_PickerHueWheel);
	ImGui::ColorEdit4("Ambient", (float*)&m_LightDesc.vAmbient, ImGuiColorEditFlags_PickerHueWheel);
	ImGui::ColorEdit4("Specular", (float*)&m_LightDesc.vSpecular, ImGuiColorEditFlags_PickerHueWheel);
}

// void CLight::Update(_fmatrix CamWorldMatrix, _float4* vFrustumCorners)
// {
	// if (m_LightDesc.eType == LIGHTDESC::TYPE_DIRECTIONAL)
	// {
	// 	_vector vCenterPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	// 	_vector vCorner[8];
	// 	for (_uint i = 0; i < 8; ++i)
	// 	{
	// 		vCorner[i] = XMVector3TransformCoord(XMLoadFloat4(&vFrustumCorners[i]), CamWorldMatrix);
	// 		vCenterPos += vCorner[i];
	// 	}
	// 	vCenterPos /= 8.0f;
	// 	float fRadius = 0.0f;
	//
	// 	for (_uint i = 0; i < 8; ++i)
	// 	{
	// 		float fDistance = XMVectorGetX(XMVector3Length(vCorner[i] - vCenterPos));
	// 		fRadius = (fRadius < fDistance) ? fDistance : fRadius;
	// 	}
	//
	// 	_vector vMaxExtents = XMVectorSet(fRadius, fRadius, fRadius, 0.f);
	// 	_vector vMinExtents = -vMaxExtents;
	//
	// 	XMStoreFloat4(&m_LightDesc.vPosition, vCenterPos + XMLoadFloat4(&m_LightDesc.vDirection) * XMVectorGetZ(vMinExtents));
	//
	// 	_vector vPos = XMLoadFloat4(&m_LightDesc.vPosition);
	// 	_vector vLook = XMLoadFloat4(&m_LightDesc.vDirection);
	// 	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	// 	_vector vUp = XMVector3Cross(vLook, vRight);
	//
	// 	_matrix TransformMatrix = XMMatrixIdentity();
	// 	TransformMatrix.r[0] = XMVector3Normalize(vRight);
	// 	TransformMatrix.r[1] = XMVector3Normalize(vUp);
	// 	TransformMatrix.r[2] = XMVector3Normalize(vLook);
	// 	TransformMatrix.r[3] = vPos;
	// 	XMStoreFloat4x4(&m_TransformState[D3DTS_VIEW], XMMatrixInverse(nullptr, TransformMatrix));
	// 	XMStoreFloat4x4(&m_TransformState_TP[D3DTS_VIEW], XMMatrixTranspose(XMMatrixInverse(nullptr, TransformMatrix)));
	//
	// 	_vector vCascadeExtents = vMaxExtents - vMinExtents;
	// 	_matrix ProjMatrix = XMMatrixOrthographicLH(XMVectorGetX(vCascadeExtents), XMVectorGetY(vCascadeExtents), 0.f, XMVectorGetZ(vCascadeExtents));
	//
	// 	XMStoreFloat4x4(&m_TransformState[D3DTS_PROJ], ProjMatrix);
	// 	XMStoreFloat4x4(&m_TransformState_TP[D3DTS_PROJ], XMMatrixTranspose(ProjMatrix));
	// }
// }

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
