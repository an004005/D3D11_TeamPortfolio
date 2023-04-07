#include "stdafx.h"
#include "..\public\Talk_FaceUI.h"
#include "GameInstance.h"

CTalk_FaceUI::CTalk_FaceUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CTalk_FaceUI::CTalk_FaceUI(const CTalk_FaceUI& rhs)
	: CUI(rhs)
{
}

HRESULT CTalk_FaceUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTalk_FaceUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTalk_FaceUI::Tick(_double TimeDelta)
{
	if (false == m_bChange) return;

	__super::Tick(TimeDelta);

	if (0.0 == m_dChange_TimeAcc)
	{
		m_tParams.Float2s[1].y = m_fFace;
	}

	m_dChange_TimeAcc += TimeDelta;
	if (2.5 < m_dChange_TimeAcc)
	{
		m_tParams.Float4s[0].w -= _float(TimeDelta) * 2.0f;
	}
	if (0.4 < m_dChange_TimeAcc)
	{
		m_tParams.Float2s[1].y = m_fFace;
	}
	if (0.2 < m_dChange_TimeAcc)
	{
		m_tParams.Float2s[1].y = 20;
	}
}

HRESULT CTalk_FaceUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	_float2 fSize = { 0.45f, 0.45f };
	if (0.5 < m_dChange_TimeAcc && 2.7 > m_dChange_TimeAcc)
	{
		CGameInstance::GetInstance()->Render_Font(L"Pretendard32", m_wsTalk0.c_str(), GetScreenSpaceLeftTop() + _float2(30.0f, 145.0f), 0.f, fSize);
		CGameInstance::GetInstance()->Render_Font(L"Pretendard32", m_wsTalk1.c_str(), GetScreenSpaceLeftTop() + _float2(30.0f, 180.0f), 0.f, fSize);
	}

	return S_OK;
}

void CTalk_FaceUI::Set_Face(const _float2 & fFace, const wstring & szTalk0, const wstring & szTalk1)
{
	// x : [0] ������ [1] ������ [2] ����
	// y : [0] ������ [1] ī��� [2] �ϳ��� [3] �׸� [4] ������ [5] ��ī [6] ī�ɷ� [7] �õ� [8] ��ī [9] �ƶ�� [10] ��Į�� ��Ʈ���� [13] �ߴ��� [18] ������
	m_bVisible = true;
	m_bChange = true;
	m_dChange_TimeAcc = 0.0;
	m_fFace = fFace.x;
	m_tParams.Float4s[0].w = 1.0f;
	m_tParams.Float2s[0].x = fFace.x;
	m_tParams.Float2s[0].y = fFace.y;
	m_wsTalk0 = szTalk0;
	m_wsTalk1 = szTalk1;
}

CTalk_FaceUI * CTalk_FaceUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CTalk_FaceUI*		pInstance = new CTalk_FaceUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTalk_FaceUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CTalk_FaceUI::Clone(void * pArg)
{
	CTalk_FaceUI*		pInstance = new CTalk_FaceUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTalk_FaceUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTalk_FaceUI::Free()
{
	__super::Free();

}
