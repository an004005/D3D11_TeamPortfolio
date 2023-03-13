#include "stdafx.h"
#include "..\public\SASSkillNameUI.h"
#include "GameInstance.h"

CSASSkillNameUI::CSASSkillNameUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CSASSkillNameUI::CSASSkillNameUI(const CSASSkillNameUI& rhs)
	: CUI(rhs)
{
}

HRESULT CSASSkillNameUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSASSkillNameUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CSASSkillNameUI::BeginTick()
{


}

void CSASSkillNameUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

}

void CSASSkillNameUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CSASSkillNameUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CSASSkillNameUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CSASSkillNameUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CSASSkillNameUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CSASSkillNameUI::Set_ColleagueName(const ESASType & eESASType, const _bool bUsable)
{
	//[0] 유이토 [1] 카사네 [2] 하나비 [3] 겜마 [4] 츠구미 [5] 루카 [6] 카케로 [7] 시덴 [8] 쿄카 [9] 아라시 [10] 없어요

	_float fUasType;
	if (true == bUsable)
		fUasType = 0.0f;
	else
		fUasType = 1.0f;

	switch (eESASType)
	{
	case Client::ESASType::SAS_FIRE: // 발화
	{
		m_tParams.Float2s[0] = { fUasType , 2.0f };
	}
	break;
	case Client::ESASType::SAS_PENETRATE:	// 투시
	{
		m_tParams.Float2s[0] = { fUasType , 4.0f };
	}
	break;
	case Client::ESASType::SAS_HARDBODY:	// 경질화
	{
		m_tParams.Float2s[0] = { fUasType , 3.0f };
	}
	break;
	case Client::ESASType::SAS_TELEPORT:	// 순간이동
	{
		m_tParams.Float2s[0] = { fUasType , 5.0f };
	}
	break;
	case Client::ESASType::SAS_ELETRIC:		// 전기
	{
		m_tParams.Float2s[0] = { fUasType , 7.0f };
	}
	break;
	case Client::ESASType::SAS_SUPERSPEED:	// 초고속
	{
		m_tParams.Float2s[0] = { fUasType , 9.0f };
	}
	break;
	case Client::ESASType::SAS_COPY:		// 복제
	{
		m_tParams.Float2s[0] = { fUasType , 8.0f };
	}
	break;
	case Client::ESASType::SAS_INVISIBLE:	// 투명
	{
		m_tParams.Float2s[0] = { fUasType , 6.0f };
	}
	break;
	case Client::ESASType::SAS_GRAVIKENISIS:	// 염력
	{
		m_tParams.Float2s[0] = { fUasType , 1.0f };
	}
	break;
	case Client::ESASType::SAS_NOT:	// 없어요
	{
		m_tParams.Float2s[0] = { fUasType , 10.0f };
	}
	break;
	default:
		Assert("Not Colleague Name : CSASSkillNameUI");
		break;
	}
}

void CSASSkillNameUI::Set_SuperPower(const ESASType & eESASType, const _bool bUsable)
{
	//[0] 염력 [1] 염력 [2] 발화 [3] 경질화 [4] 투시 [5] 순간이동 [6] 투명화 [7] 방전 [8] 복제 [9] 초고속 [10] 없어요

	_float fUasType;
	if (true == bUsable)
		fUasType = 0.0f;
	else
		fUasType = 1.0f;

	switch (eESASType)
	{
	case Client::ESASType::SAS_FIRE: // 발화
	{
		m_tParams.Float2s[0] = { fUasType , 2.0f };
	}
	break;
	case Client::ESASType::SAS_PENETRATE:	// 투시
	{
		m_tParams.Float2s[0] = { fUasType , 4.0f };
	}
	break;
	case Client::ESASType::SAS_HARDBODY:	// 경질화
	{
		m_tParams.Float2s[0] = { fUasType , 3.0f };
	}
	break;
	case Client::ESASType::SAS_TELEPORT:	// 순간이동
	{
		m_tParams.Float2s[0] = { fUasType , 5.0f };
	}
	break;
	case Client::ESASType::SAS_ELETRIC:		// 전기
	{
		m_tParams.Float2s[0] = { fUasType , 7.0f };
	}
	break;
	case Client::ESASType::SAS_SUPERSPEED:	// 초고속
	{
		m_tParams.Float2s[0] = { fUasType , 9.0f };
	}
	break;
	case Client::ESASType::SAS_COPY:		// 복제
	{
		m_tParams.Float2s[0] = { fUasType , 8.0f };
	}
	break;
	case Client::ESASType::SAS_INVISIBLE:	// 투명
	{
		m_tParams.Float2s[0] = { fUasType , 6.0f };
	}
	break;
	case Client::ESASType::SAS_GRAVIKENISIS:	// 염력
	{
		m_tParams.Float2s[0] = { fUasType , 1.0f };
	}
	break;
	case Client::ESASType::SAS_NOT:	// 없어요
	{
		m_tParams.Float2s[0] = { fUasType , 10.0f };
	}
	break;
	default:
		Assert("Not SuperPower : CSASSkillNameUI");
		break;
	}
}

CSASSkillNameUI * CSASSkillNameUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CSASSkillNameUI*		pInstance = new CSASSkillNameUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSASSkillNameUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CSASSkillNameUI::Clone(void * pArg)
{
	CSASSkillNameUI*		pInstance = new CSASSkillNameUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSASSkillNameUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSASSkillNameUI::Free()
{
	CUI::Free();

}
