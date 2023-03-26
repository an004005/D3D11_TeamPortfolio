#include "stdafx.h"
#include "..\public\PointLight.h"
#include "Light.h"
#include "GameInstance.h"
#include "JsonLib.h"


CPointLight::CPointLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CPointLight::CPointLight(const CPointLight& rhs)
	: CGameObject(rhs)
{
}

void CPointLight::BeginTick()
{
	CGameObject::BeginTick();
	if (m_pPointLight == nullptr)
	{
		m_strLightTag = CreatePointLightTag();
		LIGHTDESC tDesc;
		ZeroMemory(&tDesc, sizeof(LIGHTDESC));
		tDesc.eType = LIGHTDESC::TYPE_POINT;
		tDesc.vDiffuse = _float4{ 1.f, 1.f, 1.f, 1.f };
		tDesc.fRange = 3.f;
		tDesc.isEnable = true;
		
		m_pPointLight = CGameInstance::GetInstance()->Add_Light(m_strLightTag, m_pDevice, m_pContext, tDesc);
		Safe_AddRef(m_pPointLight);
	}
}

void CPointLight::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);
	XMStoreFloat4(&m_pPointLight->Get_LightDesc()->vPosition, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
}

void CPointLight::LoadFromJson(const Json& json)
{
	CGameObject::LoadFromJson(json);

	m_strLightTag = CreatePointLightTag();
	LIGHTDESC tDesc;
	ZeroMemory(&tDesc, sizeof(LIGHTDESC));
	tDesc.eType = LIGHTDESC::TYPE_POINT;
	tDesc.vDiffuse = json["Color"];
	tDesc.fRange = json["Range"];
	tDesc.isEnable = json["Active"];
	
	m_pPointLight = CGameInstance::GetInstance()->Add_Light(m_strLightTag, m_pDevice, m_pContext, tDesc);
	Safe_AddRef(m_pPointLight);
}

void CPointLight::SaveToJson(Json& json)
{
	CGameObject::SaveToJson(json);
	json["Color"] = m_pPointLight->Get_LightDesc()->vDiffuse;
	json["Range"] = m_pPointLight->Get_LightDesc()->fRange;
	json["Active"] = m_pPointLight->Get_LightDesc()->isEnable;
}

void CPointLight::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();
	ImGui::ColorEdit4("Color", (float*)&m_pPointLight->Get_LightDesc()->vDiffuse, ImGuiColorEditFlags_PickerHueWheel);
	ImGui::InputFloat("Range", &m_pPointLight->Get_LightDesc()->fRange);
	ImGui::Checkbox("Active", &m_pPointLight->Get_LightDesc()->isEnable);
}

void CPointLight::Active(_bool bActive)
{
	m_pPointLight->Get_LightDesc()->isEnable = bActive;
}

void CPointLight::SetRange(_float fRange)
{
	m_pPointLight->Get_LightDesc()->fRange = fRange;
}

void CPointLight::SetColor(_float4 vColor)
{
	m_pPointLight->Get_LightDesc()->vDiffuse = vColor;
}

CPointLight* CPointLight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPointLight* pInstance = new CPointLight(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPointLight");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPointLight::Clone(void* pArg)
{
	CPointLight*		pInstance = new CPointLight(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPointLight");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPointLight::Free()
{
	CGameObject::Free();
	if (m_strLightTag.empty() == false)
		CGameInstance::GetInstance()->Delete_Light(m_strLightTag);
	Safe_Release(m_pPointLight);
}

string CPointLight::CreatePointLightTag()
{
	static _uint s_PointLightID = 0;
	return "PointLight_" + to_string(s_PointLightID++);
}
