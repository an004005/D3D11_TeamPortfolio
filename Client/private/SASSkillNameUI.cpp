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
	//[0] ������ [1] ī��� [2] �ϳ��� [3] �׸� [4] ������ [5] ��ī [6] ī�ɷ� [7] �õ� [8] ��ī [9] �ƶ�� [10] �����

	_float fUasType;
	if (true == bUsable)
		fUasType = 0.0f;
	else
		fUasType = 1.0f;

	switch (eESASType)
	{
	case Client::ESASType::SAS_FIRE: // ��ȭ
	{
		m_tParams.Float2s[0] = { fUasType , 2.0f };
	}
	break;
	case Client::ESASType::SAS_PENETRATE:	// ����
	{
		m_tParams.Float2s[0] = { fUasType , 4.0f };
	}
	break;
	case Client::ESASType::SAS_HARDBODY:	// ����ȭ
	{
		m_tParams.Float2s[0] = { fUasType , 3.0f };
	}
	break;
	case Client::ESASType::SAS_TELEPORT:	// �����̵�
	{
		m_tParams.Float2s[0] = { fUasType , 5.0f };
	}
	break;
	case Client::ESASType::SAS_ELETRIC:		// ����
	{
		m_tParams.Float2s[0] = { fUasType , 7.0f };
	}
	break;
	case Client::ESASType::SAS_SUPERSPEED:	// �ʰ��
	{
		m_tParams.Float2s[0] = { fUasType , 9.0f };
	}
	break;
	case Client::ESASType::SAS_COPY:		// ����
	{
		m_tParams.Float2s[0] = { fUasType , 8.0f };
	}
	break;
	case Client::ESASType::SAS_INVISIBLE:	// ����
	{
		m_tParams.Float2s[0] = { fUasType , 6.0f };
	}
	break;
	case Client::ESASType::SAS_GRAVIKENISIS:	// ����
	{
		m_tParams.Float2s[0] = { fUasType , 1.0f };
	}
	break;
	case Client::ESASType::SAS_NOT:	// �����
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
	//[0] ���� [1] ���� [2] ��ȭ [3] ����ȭ [4] ���� [5] �����̵� [6] ����ȭ [7] ���� [8] ���� [9] �ʰ�� [10] �����

	_float fUasType;
	if (true == bUsable)
		fUasType = 0.0f;
	else
		fUasType = 1.0f;

	switch (eESASType)
	{
	case Client::ESASType::SAS_FIRE: // ��ȭ
	{
		m_tParams.Float2s[0] = { fUasType , 2.0f };
	}
	break;
	case Client::ESASType::SAS_PENETRATE:	// ����
	{
		m_tParams.Float2s[0] = { fUasType , 4.0f };
	}
	break;
	case Client::ESASType::SAS_HARDBODY:	// ����ȭ
	{
		m_tParams.Float2s[0] = { fUasType , 3.0f };
	}
	break;
	case Client::ESASType::SAS_TELEPORT:	// �����̵�
	{
		m_tParams.Float2s[0] = { fUasType , 5.0f };
	}
	break;
	case Client::ESASType::SAS_ELETRIC:		// ����
	{
		m_tParams.Float2s[0] = { fUasType , 7.0f };
	}
	break;
	case Client::ESASType::SAS_SUPERSPEED:	// �ʰ��
	{
		m_tParams.Float2s[0] = { fUasType , 9.0f };
	}
	break;
	case Client::ESASType::SAS_COPY:		// ����
	{
		m_tParams.Float2s[0] = { fUasType , 8.0f };
	}
	break;
	case Client::ESASType::SAS_INVISIBLE:	// ����
	{
		m_tParams.Float2s[0] = { fUasType , 6.0f };
	}
	break;
	case Client::ESASType::SAS_GRAVIKENISIS:	// ����
	{
		m_tParams.Float2s[0] = { fUasType , 1.0f };
	}
	break;
	case Client::ESASType::SAS_NOT:	// �����
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
