#include "stdafx.h"
#include "..\public\CapsuleLight.h"
#include "Light.h"
#include "GameInstance.h"
#include "JsonLib.h"

CCapsuleLight::CCapsuleLight(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CCapsuleLight::CCapsuleLight(const CCapsuleLight & rhs)
	: CGameObject(rhs)
{
}

void CCapsuleLight::BeginTick()
{
	CGameObject::BeginTick();
	if (m_pCapsuleLight == nullptr)
	{
		m_strLightTag = CreateCapsuleLightTag();
		LIGHTDESC tDesc;
		ZeroMemory(&tDesc, sizeof(LIGHTDESC));
		tDesc.eType = LIGHTDESC::TYPE_CAPSULE;
		tDesc.vDiffuse = _float4{ 1.f, 1.f, 1.f, 1.f };
		tDesc.fRange = 3.f;
		tDesc.isEnable = true;
		
		m_pCapsuleLight = CGameInstance::GetInstance()->Add_Light(m_strLightTag, m_pDevice, m_pContext, tDesc);
		Safe_AddRef(m_pCapsuleLight);
	}
}

void CCapsuleLight::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	const _vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	const _vector vLook = XMVector3NormalizeEst(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	m_pCapsuleLight->Get_LightDesc()->vCapsuleStart = vPos + vLook * m_fLength;
	m_pCapsuleLight->Get_LightDesc()->vCapsuleEnd = vPos - vLook * m_fLength;
}

void CCapsuleLight::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);
	m_strLightTag = CreateCapsuleLightTag();
	LIGHTDESC tDesc;
	ZeroMemory(&tDesc, sizeof(LIGHTDESC));
	tDesc.eType = LIGHTDESC::TYPE_CAPSULE;
	tDesc.vDiffuse = json["Color"];
	tDesc.fRange = json["Range"];
	tDesc.isEnable = json["Active"];
	
	m_pCapsuleLight = CGameInstance::GetInstance()->Add_Light(m_strLightTag, m_pDevice, m_pContext, tDesc);
	Safe_AddRef(m_pCapsuleLight);
}

void CCapsuleLight::SaveToJson(Json & json)
{
	__super::SaveToJson(json);
	json["Color"] = m_pCapsuleLight->Get_LightDesc()->vDiffuse;
	json["Range"] = m_pCapsuleLight->Get_LightDesc()->fRange;
	json["Active"] = m_pCapsuleLight->Get_LightDesc()->isEnable;
	json["Length"] = m_fLength;
}

void CCapsuleLight::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();
	ImGui::ColorEdit4("Color", (float*)&m_pCapsuleLight->Get_LightDesc()->vDiffuse, ImGuiColorEditFlags_PickerHueWheel);
	ImGui::InputFloat("Range", &m_pCapsuleLight->Get_LightDesc()->fRange);
	ImGui::InputFloat("Length", &m_fLength);
	ImGui::Checkbox("Active", &m_pCapsuleLight->Get_LightDesc()->isEnable);
}

void CCapsuleLight::Active(_bool bActive)
{
	m_pCapsuleLight->Get_LightDesc()->isEnable = bActive;
}

void CCapsuleLight::SetRange(_float fRange)
{
	m_pCapsuleLight->Get_LightDesc()->fRange = fRange;
}

void CCapsuleLight::SetColor(_float4 vColor)
{
	m_pCapsuleLight->Get_LightDesc()->vDiffuse = vColor;
}

void CCapsuleLight::SetLength(_float fLength)
{
	m_fLength = fLength;
}

string CCapsuleLight::CreateCapsuleLightTag()
{
	static _uint s_CapsuleLightID = 0;
	return "CapsuleLight_" + to_string(s_CapsuleLightID++);
}

CCapsuleLight * CCapsuleLight::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCapsuleLight* pInstance = new CCapsuleLight(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCapsuleLight");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCapsuleLight::Clone(void * pArg)
{
	CCapsuleLight*		pInstance = new CCapsuleLight(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCapsuleLight");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCapsuleLight::Free()
{
	CGameObject::Free();
	if (m_strLightTag.empty() == false)
		CGameInstance::GetInstance()->Delete_Light(m_strLightTag);
	Safe_Release(m_pCapsuleLight);
}
