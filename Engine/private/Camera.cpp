#include "stdafx.h"
#include "..\public\Camera.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "JsonLib.h"
#include "ImguiUtils.h"

CCamera* CCamera::s_pMainCamera = nullptr;

CCamera::CCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
	, m_pPipeLine(CPipeLine::GetInstance())
{
	Safe_AddRef(m_pPipeLine);	
}

CCamera::CCamera(const CCamera & rhs) 
	: CGameObject(rhs)
	, m_pPipeLine(rhs.m_pPipeLine)
{
	Safe_AddRef(m_pPipeLine);
}

HRESULT CCamera::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		m_fFOV = json["FOV"];
		m_fWidth = json["Width"];
		m_fHeight = json["Height"];
		m_fNear = json["Near"];
		m_fFar = json["Far"];
		m_bUseViewPortSize = json["UseViewportSize"];
	}
	else
	{
		m_CameraDesc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
		m_CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
		m_CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&m_CameraDesc.vEye));
		m_pTransformCom->LookAt(XMLoadFloat4(&m_CameraDesc.vAt));
	}

	return S_OK;
}

void CCamera::Tick(_double TimeDelta)
{
	if (IsMainCamera())
	{
 		m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());

		if (m_bUseViewPortSize)
			m_pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fFOV), (_float)WINCX / (_float)WINCY, m_fNear, m_fFar));
		else
			m_pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fFOV), m_fWidth / m_fHeight, m_fNear, m_fFar));
	}
}

void CCamera::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();
	ImGui::InputFloat("FOV", &m_fFOV);
	ImGui::InputFloat("Width", &m_fWidth);
	ImGui::InputFloat("Height", &m_fHeight);
	ImGui::InputFloat("Near", &m_fNear);
	ImGui::InputFloat("Far", &m_fFar);
	ImGui::Checkbox("UseViewportSize", &m_bUseViewPortSize);
	ImGui::Separator();
	if (ImGui::Button("Set MainCamera"))
	{
		SetMainCamera();
	}
	CImguiUtils::FileDialog_FileSelector("Save This Camera", ".json", "../Bin/Resources/Objects/", [this](const string& filePath)
	{
		Json json;
		SaveToJson(json);
		std::ofstream file(filePath);
		file << json;
	});
}

void CCamera::SaveToJson(Json& json)
{
	CGameObject::SaveToJson(json);
	json["FOV"] = m_fFOV;
	json["Width"] = m_fWidth;
	json["Height"] = m_fHeight;
	json["Near"] = m_fNear;
	json["Far"] = m_fFar;
	json["UseViewportSize"] = m_bUseViewPortSize;
}

_float4x4 CCamera::GetProjMatrix() const
{
	if (m_bUseViewPortSize)
		return XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fFOV), (_float)WINCX / (_float)WINCY, m_fNear, m_fFar);
	else
		return XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fFOV), m_fWidth / m_fHeight, m_fNear, m_fFar);
}

void CCamera::Free()
{
	__super::Free();
	Safe_Release(m_pPipeLine);
}
