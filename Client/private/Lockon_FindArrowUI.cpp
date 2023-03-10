#include "stdafx.h"
#include "..\public\Lockon_FindArrowUI.h"
#include "GameInstance.h"

// 몬스터 입장에서 플레이어를 발견하게 되면 뜬다?

CLockon_FindArrowUI::CLockon_FindArrowUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CLockon_FindArrowUI::CLockon_FindArrowUI(const CLockon_FindArrowUI& rhs)
	: CUI(rhs)
{
}

HRESULT CLockon_FindArrowUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLockon_FindArrowUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	static _int iObjectCount;
	++iObjectCount;
	if (1 == iObjectCount)
		m_eObjectType = LEFT;
	else if (2 == iObjectCount)
		m_eObjectType = RIGHT;

	m_fMaxSize = 128.0f;
	m_fLastSize = 70.0f;

	return S_OK;
}

void CLockon_FindArrowUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
	FirstDiscovery(TimeDelta);

}

void CLockon_FindArrowUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CLockon_FindArrowUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CLockon_FindArrowUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

	if (ImGui::Button("find"))
	{
		Set_Find();
	}
}

void CLockon_FindArrowUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CLockon_FindArrowUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CLockon_FindArrowUI::FirstDiscovery(const _double & TimeDelta)
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

void CLockon_FindArrowUI::Move(const _double & TimeDelta)
{
	if (false == m_bMove)
		return;

	
}

CLockon_FindArrowUI * CLockon_FindArrowUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CLockon_FindArrowUI*		pInstance = new CLockon_FindArrowUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLockon_FindArrowUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CLockon_FindArrowUI::Clone(void * pArg)
{
	CLockon_FindArrowUI*		pInstance = new CLockon_FindArrowUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLockon_FindArrowUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLockon_FindArrowUI::Free()
{
	__super::Free();

}
