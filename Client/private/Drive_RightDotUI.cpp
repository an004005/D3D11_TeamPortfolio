#include "stdafx.h"
#include "..\public\Drive_RightDotUI.h"
#include "GameInstance.h"
#include "JsonLib.h"
#include "UI_Manager.h"

//m_tParams.Float2s[0] = [x] UV.y 에 더할 값 / [y] 텍스쳐 나눌 값

CDrive_RightDotUI::CDrive_RightDotUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CDrive_RightDotUI::CDrive_RightDotUI(const CDrive_RightDotUI& rhs)
	: CUI(rhs)
{
}

HRESULT CDrive_RightDotUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDrive_RightDotUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CDrive_RightDotUI::BeginTick()
{


}

void CDrive_RightDotUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	DriveCount_Tick();
}

void CDrive_RightDotUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CDrive_RightDotUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CDrive_RightDotUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

	_int iDriveCount = _int(m_eDriveCount);
	ImGui::InputInt("Drive Count", &iDriveCount);
	m_eDriveCount = DRIVECOUNT(iDriveCount);
}

void CDrive_RightDotUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CDrive_RightDotUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CDrive_RightDotUI::DriveCount_Tick()
{
	switch (m_eDriveCount)
	{
	case Client::CDrive_RightDotUI::ONE:
	{
		m_fY = 56.0f;
		m_fSizeY = 5.0f;
		m_tParams.Float2s[0] = { 11.0f, 12.0f };
	}
		break;
	case Client::CDrive_RightDotUI::TWO:
	{
		m_fY = 57.3f;
		m_fSizeY = 10.0f;
		m_tParams.Float2s[0] = { 11.0f, 6.0f };
	}
		break;
	case Client::CDrive_RightDotUI::THREE:
	{
		m_fY = 59.3f;
		m_fSizeY = 15.0f;
		m_tParams.Float2s[0] = { 11.0f, 4.0f };
	}
		break;
	case Client::CDrive_RightDotUI::FOUR:
	{
		m_fY = 61.0f;
		m_fSizeY = 20.0f;
		m_tParams.Float2s[0] = { 11.0f, 3.0f };
	}
		break;
	case Client::CDrive_RightDotUI::FIVE:
	{
		m_fY = 62.6f;
		m_fSizeY = 25.0f;
		m_tParams.Float2s[0] = { 11.0f, 2.4f };
	}
		break;
	case Client::CDrive_RightDotUI::SIX:
	{
		m_fY = 64.5f;
		m_fSizeY = 30.0f;
		m_tParams.Float2s[0] = { 11.0f, 2.0f };
	}
		break;
	case Client::CDrive_RightDotUI::SEVEN:
	{
		m_fY = 65.5f;
		m_fSizeY = 35.0f;
		m_tParams.Float2s[0] = { 19.0f, 1.7f };
	}
		break;
	case Client::CDrive_RightDotUI::EIGHT:
	{
		m_fY = 68.0f;
		m_fSizeY = 40.0f;
		m_tParams.Float2s[0] = { 11.0f, 1.5f };
	}
		break;
	case Client::CDrive_RightDotUI::NINE:
	{
		m_fY = 69.0f;
		m_fSizeY = 45.0f;
		m_tParams.Float2s[0] = { 11.0f, 1.33f };
	}
		break;
	case Client::CDrive_RightDotUI::TEN:
	{
		m_fY = 71.5f;
		m_fSizeY = 50.0f;
		m_tParams.Float2s[0] = { 11.0f, 1.2f };
	}
		break;
	case Client::CDrive_RightDotUI::ELEVEN:
	{
		m_fY = 73.0f;
		m_fSizeY = 55.0f;
		m_tParams.Float2s[0] = { 0.0f, 1.08f };
	}
		break;
	case Client::CDrive_RightDotUI::TWELVE:
	{
		m_fY = 75.0f;
		m_fSizeY = 60.0f;
		m_tParams.Float2s[0] = { 0.0f, 1.0f };
	}
		break;
	default:
		assert(!"Wrong Drive Right UI Enum");
		break;
	}
}

CDrive_RightDotUI * CDrive_RightDotUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CDrive_RightDotUI*		pInstance = new CDrive_RightDotUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDrive_RightDotUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CDrive_RightDotUI::Clone(void * pArg)
{
	CDrive_RightDotUI*		pInstance = new CDrive_RightDotUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDrive_RightDotUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDrive_RightDotUI::Free()
{
	CUI::Free();

}
