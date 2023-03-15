#include "stdafx.h"
#include "..\public\Boss_AppearBackUI.h"
#include "GameInstance.h"

// g_float_0 : 0 -> 1 마스크의 r 값에 따라 보여지는 정도가 달라진다.
// g_vec4_0 : 텍스처 색을 빨간색에서 검정색으로 변경

CBoss_AppearBackUI::CBoss_AppearBackUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CBoss_AppearBackUI::CBoss_AppearBackUI(const CBoss_AppearBackUI& rhs)
	: CUI(rhs)
{
}

HRESULT CBoss_AppearBackUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_AppearBackUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;


	return S_OK;
}

void CBoss_AppearBackUI::Tick(_double TimeDelta)
{
	// [0] 외부에서 true 로 변경하면 아래 코드를 실행한다.
	if (false == m_bAppearBackGround)
		return;

	__super::Tick(TimeDelta);

	// [3] 그러다가 외부에서 true 을 시켰다는 것은 UI가 끝나감을 알린다. 그래서 안에서 알파값이 줄어들도록 했다.
	if (true == m_bAppearEnd)
	{
		m_tParams.Ints[0] = 0;
		m_tParams.Floats[0] -= _float(TimeDelta) * 0.5f;

		if (0.0f > m_tParams.Floats[0])
		{
			m_tParams.Floats[0] = 0.0f;
			m_bAppearBackGround = false;
			m_bAppearEnd = false;
			m_bVisible = false;
		}

		return;
	}

	// [2] 아래서 알파값이 증가해서 다 보여지게 되면 이미지를 다른 이미지로 변경한다.
	if (1.0f < m_tParams.Floats[0])
	{
		m_tParams.Ints[0] = 1;
		m_tParams.Floats[0] = 1.0f;
	}

	// [1] 가장 처음에 걸리는 if 문으로 알파값이 증가하면서 보여진다.
	if (0 == m_tParams.Ints[0])
		m_tParams.Floats[0] += _float(TimeDelta) * 0.5f;
}

void CBoss_AppearBackUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CBoss_AppearBackUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CBoss_AppearBackUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CBoss_AppearBackUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CBoss_AppearBackUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CBoss_AppearBackUI * CBoss_AppearBackUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CBoss_AppearBackUI*		pInstance = new CBoss_AppearBackUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBoss_AppearBackUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CBoss_AppearBackUI::Clone(void * pArg)
{
	CBoss_AppearBackUI*		pInstance = new CBoss_AppearBackUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBoss_AppearBackUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoss_AppearBackUI::Free()
{
	__super::Free();

}
