#include "stdafx.h"
#include "..\public\PlayerInfo_GaugeBackGround.h"
#include "GameInstance.h"

// m_tParams.Floats[0] : 게이지 정도

CPlayerInfo_GaugeBackGround::CPlayerInfo_GaugeBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CPlayerInfo_GaugeBackGround::CPlayerInfo_GaugeBackGround(const CPlayerInfo_GaugeBackGround& rhs)
	: CUI(rhs)
{
}

HRESULT CPlayerInfo_GaugeBackGround::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerInfo_GaugeBackGround::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	m_tParams.Floats[0] = 0.5f;
	m_bVisible = true;

	return S_OK;
}

void CPlayerInfo_GaugeBackGround::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);
}

void CPlayerInfo_GaugeBackGround::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CPlayerInfo_GaugeBackGround::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CPlayerInfo_GaugeBackGround::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CPlayerInfo_GaugeBackGround::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CPlayerInfo_GaugeBackGround::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CPlayerInfo_GaugeBackGround::Set_PsychokinesisGauge(const _uint iLevel)
{
	// 레벨에 따른 게이지 바 길이가 달라진다.
	if (0 == iLevel)
		m_tParams.Floats[0] = 0.5f;
	else if (1 == iLevel)
		m_tParams.Floats[0] = 0.735f;
	else
		m_tParams.Floats[0] = 1.0f;
}

CPlayerInfo_GaugeBackGround * CPlayerInfo_GaugeBackGround::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayerInfo_GaugeBackGround*		pInstance = new CPlayerInfo_GaugeBackGround(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayerInfo_GaugeBackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CPlayerInfo_GaugeBackGround::Clone(void * pArg)
{
	CPlayerInfo_GaugeBackGround*		pInstance = new CPlayerInfo_GaugeBackGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayerInfo_GaugeBackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerInfo_GaugeBackGround::Free()
{
	CUI::Free();

}
