#include "stdafx.h"
#include "..\public\SASSkillIconUI.h"
#include "GameInstance.h"
#include "FSMComponent.h"

// [0] 초능력 사용 가능(이미지) / [1] 초능력 사용 불 가능(검정색) -> 그냥 Float4s[0] 로 변경해도 되는데 이게 직관적인 것 같아서요
//m_tParams.Ints[0] = { 1 };

// x: [0] 염력 [1] 염력 [2] 발화 [3] 경질 [4] 투시 [5] 순간이동 [6] 투명 [7] 방전 [8] 복제 [9] 초고속 [10] 사용 불가
// y: [0] 초능력 사용 불 가능 (그냥) / [1] 초능력 사용 가능(불들어오는거)
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
	case Client::ESASType::SAS_FIRE: // 발화
		fSasType = 2.0f;
		break;
	case Client::ESASType::SAS_PENETRATE:	// 투시
		fSasType = 4.0f;
		break;
	case Client::ESASType::SAS_HARDBODY:	// 경질화
		fSasType = 3.0f;
		break;
	case Client::ESASType::SAS_TELEPORT:	// 순간이동
		fSasType = 5.0f;
		break;
	case Client::ESASType::SAS_ELETRIC:		// 전기
		fSasType = 7.0f;
		break;
	case Client::ESASType::SAS_SUPERSPEED:	// 초고속
		fSasType = 9.0f;
		break;
	case Client::ESASType::SAS_COPY:		// 복제
		fSasType = 8.0f;
		break;
	case Client::ESASType::SAS_INVISIBLE:	// 투명
		fSasType = 6.0f;
		break;
	case Client::ESASType::SAS_GRAVIKENISIS:	// 염력
		fSasType = 1.0f;
		break;
	case Client::ESASType::SAS_NOT:	// 없어요
		fSasType = 10.0f;
		break;
	default:
		Assert("Not Icon Type : CSASSkillIconUI");
		break;
	}

	if (bUsable)	// 사용 가능 
	{
		m_tParams.Ints[0] = 0;
		m_tParams.Float2s[0] = { fSasType, 1.0f };
	}
	else			// 사용 불 가능
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
