#include "stdafx.h"
#include "..\public\Lockon_FindUI.h"
#include "GameInstance.h"

// 몬스터 입장에서 플레이어를 발견하게 되면 뜬다?

CLockon_FindUI::CLockon_FindUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CLockon_FindUI::CLockon_FindUI(const CLockon_FindUI& rhs)
	: CUI(rhs)
{
}

HRESULT CLockon_FindUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLockon_FindUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	m_fMaxSize = 128.0f;
	m_fLastSize = 70.0f;

	return S_OK;
}

void CLockon_FindUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
	FirstDiscovery(TimeDelta);

}

void CLockon_FindUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CLockon_FindUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CLockon_FindUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

	if (ImGui::Button("find"))
	{
		Set_Find();
	}
}

void CLockon_FindUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CLockon_FindUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CLockon_FindUI::FirstDiscovery(const _double TimeDelta)
{
	if (false == m_bFind)
		return;

	if (false == m_bSizeChange)
	{
		if (m_fMaxSize >= m_fSizeX)		// Max Size
		{
			m_fSizeX -= _float(TimeDelta) * 25.0f;
			m_fSizeY -= _float(TimeDelta) * 25.0f;

			if (0.5f < m_tParams.Float4s[0].y)
			{
				m_tParams.Float4s[0].y -= _float(TimeDelta) * 0.3f;
				m_tParams.Float4s[0].z -= _float(TimeDelta) * 0.3f;
			}											  
		}
	}

	if (m_fLastSize >= m_fSizeX)
	{
		m_bSizeChange = true;
		m_fSizeX = m_fLastSize;
		m_fSizeY = m_fLastSize;

		m_tParams.Float4s[0].y += _float(TimeDelta);
		m_tParams.Float4s[0].z += _float(TimeDelta);

		if (1.0f < m_tParams.Float4s[0].y)
		{
			m_bFind = false;
			m_tParams.Float4s[0] = { 1.0f, 1.0f, 1.0f, 1.0f };
		}
	}
}

CLockon_FindUI * CLockon_FindUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CLockon_FindUI*		pInstance = new CLockon_FindUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLockon_FindUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CLockon_FindUI::Clone(void * pArg)
{
	CLockon_FindUI*		pInstance = new CLockon_FindUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLockon_FindUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLockon_FindUI::Free()
{
	__super::Free();

}
