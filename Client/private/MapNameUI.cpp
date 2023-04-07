#include "stdafx.h"
#include "..\public\MapNameUI.h"
#include "GameInstance.h"

CMapNameUI::CMapNameUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CMapNameUI::CMapNameUI(const CMapNameUI& rhs)
	: CUI(rhs)
{
}

HRESULT CMapNameUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapNameUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	m_fSpeed = 0.4f;
	m_fAlpha = 0.9f;

	return S_OK;
}

void CMapNameUI::Tick(_double TimeDelta)
{
	// [0] �ܺο��� true �� �����ϸ� �Ʒ� �ڵ带 �����Ѵ�.
	if (false == m_bAppearBackGround)
		return;

	__super::Tick(TimeDelta);

	// [3] �׷��ٰ� �ܺο��� true �� ���״ٴ� ���� UI�� �������� �˸���. �׷��� �ȿ��� ���İ��� �پ�鵵�� �ߴ�.
	if (true == m_bAppearEnd)
	{
		m_tParams.Floats[0] -= _float(TimeDelta) * m_fSpeed;

		if (0.0f > m_tParams.Floats[0])
		{
			m_tParams.Floats[0] = 0.0f;
			m_bAppearBackGround = false;
			m_bAppearEnd = false;
			m_bVisible = false;
		}

		return;
	}

	// [2] �Ʒ��� ���İ��� �����ؼ� �� �������� �Ǹ� �̹����� �ٸ� �̹����� �����Ѵ�.
	if (m_fAlpha < m_tParams.Floats[0])
	{
		m_tParams.Floats[0] = m_fAlpha;
	}

	// [1] ���� ó���� �ɸ��� if ������ ���İ��� �����ϸ鼭 ��������.
	if (0 == m_tParams.Ints[0])
		m_tParams.Floats[0] += _float(TimeDelta) * m_fSpeed;
}

void CMapNameUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CMapNameUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CMapNameUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CMapNameUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CMapNameUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CMapNameUI * CMapNameUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CMapNameUI*		pInstance = new CMapNameUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapNameUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CMapNameUI::Clone(void * pArg)
{
	CMapNameUI*		pInstance = new CMapNameUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapNameUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMapNameUI::Free()
{
	__super::Free();

}
