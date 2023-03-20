#include "stdafx.h"
#include "..\public\Main_FaceUI.h"
#include "GameInstance.h"

CMain_FaceUI::CMain_FaceUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CMain_FaceUI::CMain_FaceUI(const CMain_FaceUI& rhs)
	: CUI(rhs)
{
}

HRESULT CMain_FaceUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMain_FaceUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMain_FaceUI::Tick(_double TimeDelta)
{
	if (false == m_bChange) return;

	__super::Tick(TimeDelta);

	if (0.0 == m_dChange_TimeAcc)
	{
		m_tParams.Float2s[1].y = 1;
	}

	m_dChange_TimeAcc += TimeDelta;
	if (0.3 < m_dChange_TimeAcc)
	{
		m_bChange = false;
		m_dChange_TimeAcc = 0.0;
		m_tParams.Float2s[1].y = 11;
		m_tParams.Float2s[0].y = m_fFaceNum;
	}
}

void CMain_FaceUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CMain_FaceUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CMain_FaceUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CMain_FaceUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CMain_FaceUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CMain_FaceUI::Set_Face(const _float & fFace)
{
	//[0] 유이토 [1] 카사네 [2] 하나비 [3] 겜마 [4] 츠구미 [5] 루카 [6] 카케로 [7] 시덴 [8] 쿄카 [9] 아라시
	m_bChange = true;
	m_fFaceNum = fFace;
	
	//m_tParams.Float2s[0].y = fFace;
}

CMain_FaceUI * CMain_FaceUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CMain_FaceUI*		pInstance = new CMain_FaceUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMain_FaceUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CMain_FaceUI::Clone(void * pArg)
{
	CMain_FaceUI*		pInstance = new CMain_FaceUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMain_FaceUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMain_FaceUI::Free()
{
	__super::Free();

}
