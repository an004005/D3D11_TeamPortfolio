#include "stdafx.h"
#include "..\public\SASSkillIconUI.h"
#include "GameInstance.h"
#include "FSMComponent.h"

// [0] �ʴɷ� ��� ����(�̹���) / [1] �ʴɷ� ��� �� ����(������) -> �׳� Float4s[0] �� �����ص� �Ǵµ� �̰� �������� �� ���Ƽ���
//m_tParams.Ints[0] = { 1 };

// x: [0] ���� [1] ���� [2] ��ȭ [3] ���� [4] ���� [5] �����̵� [6] ���� [7] ���� [8] ���� [9] �ʰ�� [10] ��� �Ұ�
// y: [0] �ʴɷ� ��� �� ���� (�׳�) / [1] �ʴɷ� ��� ����(�ҵ����°�)
//m_tParams.Float2s[0] = _float2(1.0f, 0.0f);

CSASSkillIconUI::CSASSkillIconUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CSASSkillIconUI::CSASSkillIconUI(const CSASSkillIconUI& rhs)
	: CUI(rhs)
{
}

HRESULT CSASSkillIconUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSASSkillIconUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CSASSkillIconUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

}

void CSASSkillIconUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CSASSkillIconUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CSASSkillIconUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();


}

void CSASSkillIconUI::Set_IconType(const ESASType & eESASType, const _bool bUsable)
{
	_float fSasType = 10.0f;

	switch (eESASType)
	{
	case Client::ESASType::SAS_FIRE: // ��ȭ
		fSasType = 2.0f;
		break;
	case Client::ESASType::SAS_PENETRATE:	// ����
		fSasType = 4.0f;
		break;
	case Client::ESASType::SAS_HARDBODY:	// ����ȭ
		fSasType = 3.0f;
		break;
	case Client::ESASType::SAS_TELEPORT:	// �����̵�
		fSasType = 5.0f;
		break;
	case Client::ESASType::SAS_ELETRIC:		// ����
		fSasType = 7.0f;
		break;
	case Client::ESASType::SAS_SUPERSPEED:	// �ʰ��
		fSasType = 9.0f;
		break;
	case Client::ESASType::SAS_COPY:		// ����
		fSasType = 8.0f;
		break;
	case Client::ESASType::SAS_INVISIBLE:	// ����
		fSasType = 6.0f;
		break;
	case Client::ESASType::SAS_GRAVIKENISIS:	// ����
		fSasType = 1.0f;
		break;
	case Client::ESASType::SAS_NOT:	// �����
		fSasType = 10.0f;
		break;
	default:
		Assert("Not Icon Type : CSASSkillIconUI");
		break;
	}

	if (bUsable)	// ��� ���� 
	{
		m_tParams.Ints[0] = 0;
		m_tParams.Float2s[0] = { fSasType, 1.0f };
	}
	else			// ��� �� ����
	{
		m_tParams.Ints[0] = 1;
		m_tParams.Float2s[0] = { fSasType, 0.0f };
	}
}

CSASSkillIconUI * CSASSkillIconUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CSASSkillIconUI*		pInstance = new CSASSkillIconUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSASSkillIconUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CSASSkillIconUI::Clone(void * pArg)
{
	CSASSkillIconUI*		pInstance = new CSASSkillIconUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSASSkillIconUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSASSkillIconUI::Free()
{
	CUI::Free();

}
