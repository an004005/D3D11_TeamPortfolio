#include "stdafx.h"
#include "..\public\PlayerInfo_PsychokinesisUI.h"
#include "GameInstance.h"
#include "JsonLib.h"

// m_tParams.Ints[0] : 텍스처 선택
// m_tParams.Floats[0] : 게이지 정도
// m_tParams.Floats[1] : 프레임 Ints 에 따라 달라진다. (0 : 0.08, 1 : 0.03)

CPlayerInfo_PsychokinesisUI::CPlayerInfo_PsychokinesisUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CPlayerInfo_PsychokinesisUI::CPlayerInfo_PsychokinesisUI(const CPlayerInfo_PsychokinesisUI& rhs)
	: CUI(rhs)
{
}

HRESULT CPlayerInfo_PsychokinesisUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerInfo_PsychokinesisUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPlayerInfo_PsychokinesisUI::BeginTick()
{


}

void CPlayerInfo_PsychokinesisUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	m_tParams.Floats[0] = m_fPsychokinesisGauge;
}

void CPlayerInfo_PsychokinesisUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CPlayerInfo_PsychokinesisUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CPlayerInfo_PsychokinesisUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CPlayerInfo_PsychokinesisUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CPlayerInfo_PsychokinesisUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CPlayerInfo_PsychokinesisUI::Set_PsychokinesisGauge(const _uint & iType, const _float & fGauge)
{
	// iType : (0)일반0  (2)일반1  (3)  (4)공격 (5)드라이브
	m_fPsychokinesisGauge = fGauge;

	// 게이지 텍스처 선택
	m_tParams.Ints[0] = iType;
	// 텍스처 프레임
	switch (iType)
	{
	case 0:
		m_tParams.Floats[1] = 0.08f;
		break;
	case 1:
		m_tParams.Floats[1] = 0.03f;
		break;
	default:
		m_tParams.Floats[1] = 0.08f;
		break;
	}
}

CPlayerInfo_PsychokinesisUI * CPlayerInfo_PsychokinesisUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CPlayerInfo_PsychokinesisUI*		pInstance = new CPlayerInfo_PsychokinesisUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayerInfo_PsychokinesisUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CPlayerInfo_PsychokinesisUI::Clone(void * pArg)
{
	CPlayerInfo_PsychokinesisUI*		pInstance = new CPlayerInfo_PsychokinesisUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayerInfo_PsychokinesisUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerInfo_PsychokinesisUI::Free()
{
	CUI::Free();

}
