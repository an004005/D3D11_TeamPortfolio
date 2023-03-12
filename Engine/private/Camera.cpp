#include "stdafx.h"
#include "..\public\Camera.h"
#include "GameInstance.h"
#include "JsonLib.h"
#include "Camera_Manager.h"
#include "CurveManager.h"

CCamera::CCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CCamera::CCamera(const CCamera & rhs) 
	: CGameObject(rhs)
{
}

HRESULT CCamera::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pArg == nullptr)
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&_float4(0.f, 10.f, -10.f, 1.f)));
		m_pTransformCom->LookAt(XMLoadFloat4(&_float4(0.f, 0.f, 0.f, 1.f)));
	}

	m_fNear = 0.1f;
	return S_OK;
}

void CCamera::Tick(_double TimeDelta)
{
	// Camera Manager로 기능 이전
	// if (IsMainCamera())
	// {
 // 		m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
	// 	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fFOV), m_fWidth / m_fHeight, m_fNear, m_fFar));
	// }
}

void CCamera::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();
	//ImGui::InputFloat("FOV", &m_fFOV);
	ImGui::SliderFloat("FOV", &m_fFOV, 0.f, 150.f, "%.1f");
	ImGui::InputFloat("Width", &m_fWidth);
	ImGui::InputFloat("Height", &m_fHeight);
	ImGui::InputFloat("Near", &m_fNear);
	ImGui::InputFloat("Far", &m_fFar);
	ImGui::Separator();
	if (ImGui::Button("Set MainCamera"))
	{
		SetMainCamera();
	}
}

void CCamera::SaveToJson(Json& json)
{
	CGameObject::SaveToJson(json);
	json["FOV"] = m_fFOV;
	json["Width"] = m_fWidth;
	json["Height"] = m_fHeight;
	json["Near"] = m_fNear;
	json["Far"] = m_fFar;
}

void CCamera::LoadFromJson(const Json& json)
{
	CGameObject::LoadFromJson(json);
	m_fFOV = json["FOV"];
	m_fWidth = json["Width"];
	m_fHeight = json["Height"];
	m_fNear = json["Near"];
	m_fFar = json["Far"];
}

void CCamera::SetMainCamera()
{
	CCamera_Manager::GetInstance()->SetMainCamera(this);
}

void CCamera::SetMainCamera(CCamera* pCamera)
{
	CCamera_Manager::GetInstance()->SetMainCamera(pCamera);
}

CCamera* CCamera::GetMainCamera()
{
	return CCamera_Manager::GetInstance()->GetMainCam();
}

_bool CCamera::IsMainCamera() const
{
	return CCamera_Manager::GetInstance()->GetMainCam() == this;
}

_float4x4 CCamera::GetProjMatrix() const
{
	return XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fFOV), m_fWidth / m_fHeight, m_fNear, m_fFar);
}

void CCamera::Free()
{
	__super::Free();
}
