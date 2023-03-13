#include "stdafx.h"
#include "..\public\SASSkillLightUI.h"
#include "GameInstance.h"

//m_tParams.Float4s[0] = { 226.0f, 158.0f, 1.0f, 0.7f };	// 색상 조정 a: 0.0f 미사용 0.7f 사용
                                                               
CSASSkillLightUI::CSASSkillLightUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CSASSkillLightUI::CSASSkillLightUI(const CSASSkillLightUI& rhs)
	: CUI(rhs)
{
}

HRESULT CSASSkillLightUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSASSkillLightUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CSASSkillLightUI::BeginTick()
{

}

void CSASSkillLightUI::Tick(_double TimeDelta)
{
	if (false == m_bOnSkil)
		return; 
	
	CUI::Tick(TimeDelta);

	if (false == m_bBrightness && 0.3f >= m_fStrength)
		m_bBrightness = true;
	if (true == m_bBrightness && 0.7f <= m_fStrength)
		m_bBrightness = false;

	if (true == m_bBrightness)
		m_fStrength += _float(TimeDelta) * 5.0f;
	else
		m_fStrength -= _float(TimeDelta) * 5.0f;

	m_tParams.Float4s[0].w = { m_fStrength };
}

void CSASSkillLightUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CSASSkillLightUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CSASSkillLightUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CSASSkillLightUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CSASSkillLightUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CSASSkillLightUI::Set_LightType(const ESASType & eESASType, const _bool bUsable)
{
	m_bOnSkil = bUsable;

	if (false == bUsable)
	{
		m_fStrength = 0.5f;
		m_tParams.Float4s[0] = { 0.0f, 0.0f, 0.0f, 0.0f };
		return;
	}

	switch (eESASType)
	{
	case Client::ESASType::SAS_FIRE: // 발화
		m_tParams.Float4s[0] = { 0.970f, 0.309f, 0.418f, m_fStrength };
		break;
	case Client::ESASType::SAS_PENETRATE:	// 투시
		m_tParams.Float4s[0] = { 0.388f, 0.992f, 0.455f, m_fStrength };
		break;
	case Client::ESASType::SAS_HARDBODY:	// 경질화
		m_tParams.Float4s[0] = { 1.0f, 0.824f, 0.427f, m_fStrength };
		break;
	case Client::ESASType::SAS_TELEPORT:	// 순간이동
		m_tParams.Float4s[0] = { 0.071f, 0.439f, 0.753f, m_fStrength };
		break;
	case Client::ESASType::SAS_ELETRIC:		// 전기
		m_tParams.Float4s[0] = { 1.0f, 0.713f, 0.212f, m_fStrength };
		break;
	case Client::ESASType::SAS_SUPERSPEED:	// 초고속
		m_tParams.Float4s[0] = { 1.0f, 0.515f, 0.821f, m_fStrength };
		break;
	case Client::ESASType::SAS_COPY:		// 복제
		m_tParams.Float4s[0] = { 0.572f, 0.650f, 1.0f, m_fStrength };
		break;
	case Client::ESASType::SAS_INVISIBLE:	// 투명
		m_tParams.Float4s[0] = { 0.428f, 0.969f, 1.0f, m_fStrength };
		break;
	case Client::ESASType::SAS_GRAVIKENISIS:	// 염력
		m_tParams.Float4s[0] = { 0.816f, 0.275f, 0.973f, m_fStrength };
		break;
	case Client::ESASType::SAS_NOT:	// 없어요
		m_tParams.Float4s[0] = { 0.0f, 0.0f, 0.0f, 0.0f };
		break;
	default:
		Assert("Not Light Type : CSASSkillLightUI");
		break;
	}
}

CSASSkillLightUI * CSASSkillLightUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CSASSkillLightUI*		pInstance = new CSASSkillLightUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSASSkillLightUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CSASSkillLightUI::Clone(void * pArg)
{
	CSASSkillLightUI*		pInstance = new CSASSkillLightUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSASSkillLightUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSASSkillLightUI::Free()
{
	CUI::Free();

}
