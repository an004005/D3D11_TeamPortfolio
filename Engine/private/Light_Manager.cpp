#include "stdafx.h"
#include "..\public\Light_Manager.h"
#include "Light.h"
#include "GameInstance.h"
#include "Imgui/ImGuizmo.h"
#include "Camera.h"

IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
{
}

const LIGHTDESC * CLight_Manager::Get_LightDesc(_uint iIndex)
{
	if (iIndex >= m_Lights.size())
		return nullptr;

	return m_Lights[iIndex]->Get_LightDesc();	
}

HRESULT CLight_Manager::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc)
{
	CLight*			pLight = CLight::Create(pDevice, pContext, LightDesc);

	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.push_back(pLight);

	return S_OK;
}

void CLight_Manager::Render_Light(CVIBuffer_Rect * pVIBuffer, CShader * pShader)
{
	if (m_pShadowCam)
	{
		m_DirectionalLightView = m_pShadowCam->GetViewMatrix();
		m_DirectionalLightProj = m_pShadowCam->GetProjMatrix();
	}

	for (auto& pLight : m_Lights)
	{
		if (pLight->GetType() == LIGHTDESC::TYPE_DIRECTIONAL && m_pShadowCam)
			pLight->SetDirection(m_pShadowCam->GetTransform()->Get_State(CTransform::STATE_LOOK));

		pLight->Render(pVIBuffer, pShader);
	}
}

void CLight_Manager::Clear()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

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
	return _float4();
}

void CLight_Manager::Free()
{
	Clear();
}
