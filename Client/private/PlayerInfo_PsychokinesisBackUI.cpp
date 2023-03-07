#include "stdafx.h"
#include "..\public\PlayerInfo_PsychokinesisBackUI.h"
#include "GameInstance.h"

// m_tParams.Ints[0] : 텍스처 선택
// m_tParams.Floats[0] : 게이지 정도
// m_tParams.Floats[1] : 프레임 Ints 에 따라 달라진다. (0 : 0.08, 1 : 0.03)

CPlayerInfo_PsychokinesisBackUI::CPlayerInfo_PsychokinesisBackUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CPlayerInfo_PsychokinesisBackUI::CPlayerInfo_PsychokinesisBackUI(const CPlayerInfo_PsychokinesisBackUI& rhs)
	: CUI(rhs)
{
}

HRESULT CPlayerInfo_PsychokinesisBackUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerInfo_PsychokinesisBackUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	m_bVisible = false;

	return S_OK;
}

void CPlayerInfo_PsychokinesisBackUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	if (m_fCurrentPsychokinesisGauge > m_fPsychokinesisGauge)
	{
		m_bVisible = true;
		m_fCurrentPsychokinesisGauge -= _float(TimeDelta) * 0.25f;
		m_tParams.Floats[0] = m_fCurrentPsychokinesisGauge;
	}
	else
	{
		m_bVisible = false;
		m_tParams.Floats[0] = m_fPsychokinesisGauge;
	}
}

void CPlayerInfo_PsychokinesisBackUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CPlayerInfo_PsychokinesisBackUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CPlayerInfo_PsychokinesisBackUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CPlayerInfo_PsychokinesisBackUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CPlayerInfo_PsychokinesisBackUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CPlayerInfo_PsychokinesisBackUI::Set_PsychokinesisGauge(const _uint iLevel, const _uint iType, const _float & fGauge)
{
	// iType : (0)일반(물결) (1)공격 (2) 드라이브
	m_fPsychokinesisGauge = fGauge;

	// 드라이브 레벨에서 텍스처 프레임이 달라진다.
	if (2 == iType)
		m_tParams.Floats[1] = 0.03f;
	else
		m_tParams.Floats[1] = 0.08f;

	// 레벨에 따른 게이지 바 길이가 달라진다.
	if (0 == iLevel)
		m_fMaxLevelGauge = 2.0f;
	else if (1 == iLevel)
		m_fMaxLevelGauge = 1.35f;
	else
		m_fMaxLevelGauge = 1.0f;
}

CPlayerInfo_PsychokinesisBackUI * CPlayerInfo_PsychokinesisBackUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayerInfo_PsychokinesisBackUI*		pInstance = new CPlayerInfo_PsychokinesisBackUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayerInfo_PsychokinesisBackUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CPlayerInfo_PsychokinesisBackUI::Clone(void * pArg)
{
	CPlayerInfo_PsychokinesisBackUI*		pInstance = new CPlayerInfo_PsychokinesisBackUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayerInfo_PsychokinesisBackUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerInfo_PsychokinesisBackUI::Free()
{
	CUI::Free();

}
