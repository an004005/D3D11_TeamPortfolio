#include "stdafx.h"
#include "..\public\PlayerInfo_PsychokinesisUI.h"
#include "GameInstance.h"
#include "JsonLib.h"

// m_tParams.Ints[0] : 텍스처 선택
// m_tParams.Floats[1] : 프레임 Ints 에 따라 달라진다. (0 : 0.08, 1 : 0.03)
// m_tParams.Floats[0] : 게이지 정도

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

	m_fMaxLevelGauge = 1.0f;

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

void CPlayerInfo_PsychokinesisUI::Set_Type(const _uint iLevel)
{
	// 레벨에 따른 게이지 바 길이가 달라진다.
	if (0 == iLevel)
		m_fMaxLevelGauge = 2.0f;
	else if (1 == iLevel)
		m_fMaxLevelGauge = 1.35f;
	else
		m_fMaxLevelGauge = 1.0f;
}

void CPlayerInfo_PsychokinesisUI::Set_Gauge(const _uint iType, const _float& fGauge)
{
	// iType : (0)일반(물결) (1)공격 (2) 드라이브
	// 드라이브 타입에 따라 이미지와 속도가 달라진다.
	m_tParams.Ints[0] = iType;
	if (2 == iType)
		m_tParams.Floats[1] = 0.08f;
	else
		m_tParams.Floats[1] = 0.05f;

	m_fPsychokinesisGauge = fGauge / m_fMaxLevelGauge;
}

_float2 CPlayerInfo_PsychokinesisUI::Gauge_Position()
{
	const _float2 PivotPair = GetPivotXY(m_ePivot);
	return{ m_fX + PivotPair.x, m_fY + PivotPair.y };
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
