#include "stdafx.h"
#include "..\public\Boss_AppearUI.h"
#include "GameInstance.h"

// g_vec2_0 의 x 값을 변경하여 텍스처를 돌려준다.
// g_vec4_0 의 w 값을 변경하여 알파값을 변경한다.

CBoss_AppearUI::CBoss_AppearUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CBoss_AppearUI::CBoss_AppearUI(const CBoss_AppearUI& rhs)
	: CUI(rhs)
{
}

HRESULT CBoss_AppearUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_AppearUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	m_Timeline.SetCurve("UI_BossAppear");

	return S_OK;
}

void CBoss_AppearUI::Tick(_double TimeDelta)
{
	if (false == m_bAppear)
		return;

	__super::Tick(TimeDelta);
	m_Timeline.Tick(TimeDelta, m_tParams.Float4s[0].w);

	if (0 == m_iTexturenum)
		m_Timeline.PlayFromStart();

	// 29 보스 등장 텍스처가 끝났음을 의미합니다.
	if (29 == m_iTexturenum)
	{
		m_dNextTexture_TimeAcc += TimeDelta;
		if (1.0 < m_dNextTexture_TimeAcc)
		{
			m_tParams.Float4s[0].y -= _float(TimeDelta) * 0.5f;
			m_tParams.Float4s[0].z -= _float(TimeDelta) * 0.5f;
		}

		if (3.0 < m_dNextTexture_TimeAcc)
		{
			m_tParams.Float2s[0].x = 0.0f;
			m_tParams.Float4s[0].x = 1.0f;
			m_tParams.Float4s[0].y = 1.0f;
			m_tParams.Float4s[0].z = 1.0f;
			m_tParams.Float4s[0].w = 0.0f;

			m_iTexturenum = 0;
			m_dNextTexture_TimeAcc = 0.0;
			m_bVisible = false;
			m_bAppear = false;
		}

		return;
	}

	m_dNextTexture_TimeAcc += TimeDelta;
	if (0.1 < m_dNextTexture_TimeAcc)
	{
		m_dNextTexture_TimeAcc = 0.0;
		++m_iTexturenum;
		m_tParams.Float2s[0].x = _float(m_iTexturenum);
	}
}

void CBoss_AppearUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CBoss_AppearUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CBoss_AppearUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

	m_Timeline.Imgui_RenderEditor();

}

CBoss_AppearUI * CBoss_AppearUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CBoss_AppearUI*		pInstance = new CBoss_AppearUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBoss_AppearUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CBoss_AppearUI::Clone(void * pArg)
{
	CBoss_AppearUI*		pInstance = new CBoss_AppearUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBoss_AppearUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoss_AppearUI::Free()
{
	__super::Free();

}
