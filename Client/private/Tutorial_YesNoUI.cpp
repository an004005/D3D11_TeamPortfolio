#include "stdafx.h"
#include "..\public\Tutorial_YesNoUI.h"
#include "GameInstance.h"
#include "GameUtils.h"

CTutorial_YesNoUI::CTutorial_YesNoUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CTutorial_YesNoUI::CTutorial_YesNoUI(const CTutorial_YesNoUI& rhs)
	: CUI(rhs)
{
}

HRESULT CTutorial_YesNoUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTutorial_YesNoUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	// 0: 테두리 마름모 /1: 작은 마름로 /2: 화살표 /3: 닫기(보이지 않음)/ 4: No (-52) /5:Yes (-19)/ 6: 페이지 전환(보이지 않음) [Tutorial_ZNextPage]
	static _int iObjectCount = 0;
	m_iObjectNumber = iObjectCount;
	++iObjectCount;

	if(2 == m_iObjectNumber)
		m_tParams.Float4s[0].w = 0.0f;

	if (7 == iObjectCount)
		iObjectCount = 0;

	return S_OK;
}

void CTutorial_YesNoUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Object_Tick(TimeDelta);
}

void CTutorial_YesNoUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CTutorial_YesNoUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CTutorial_YesNoUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CTutorial_YesNoUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CTutorial_YesNoUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CTutorial_YesNoUI::Object_Tick(const _double & TimeDelta)
{
	if (1 == m_iObjectNumber || 2 == m_iObjectNumber)
		Icon_Alpha(TimeDelta);

	switch (m_iObjectNumber)
	{
	case 0:
		ObjectZero_Tick(TimeDelta);
		break;

	case 1:
		ObjectOne_Tick(TimeDelta);
		break;

	case 2:
		ObjectTwo_Tick(TimeDelta);
		break;

	case 3:
		InvisibleBox(); 
		break;

	case 4:
		ObjectNo_Tick();
		break;

	case 5:
		ObjectYes_Tick();
		break;

	case 6:
		NextPage();
		break;

	default:
		//assert(!"Wrong No ObjectNumber");
		break;
	}
}

void CTutorial_YesNoUI::ObjectZero_Tick(const _double & TimeDelta)
{
	if (false == m_bZero_Shader)
		return;

	if (false == m_bGlowUp)
	{
		if (0.80f >= m_tParams.Floats[0])
		{
			m_tParams.Float4s[0] = { 1.0f, 0.594f, 0.469f, 1.0f };
			m_tParams.Floats[0] += _float(TimeDelta);
		}
		else
		{
			m_bGlowUp = true;
			m_bGlowDown = true;
		}
	}

	if (true == m_bGlowDown)
	{
		if (0.7f <= m_tParams.Floats[0])
		{
			m_tParams.Floats[0] -= _float(TimeDelta);
		}
		else
		{
			m_tParams.Floats[0] = 0.7f;
			m_tParams.Float4s[0] = { 1.0f, 0.469f, 0.471f, 1.0f };

			m_bGlowUp = false;
			m_bGlowDown = false;
			m_bZero_Shader = false;
		}
	}
}

void CTutorial_YesNoUI::ObjectOne_Tick(const _double & TimeDelta)
{
	if (1 <= m_iOne_RenderCount)
		return;

	m_dOne_TimeAcc += TimeDelta;
	if (0.5 < m_dOne_TimeAcc)
	{
		m_tParams.Float4s[0].w = 0.0f;
	}

	if (1.0 < m_dOne_TimeAcc)
	{
		m_dOne_TimeAcc = 0.0;
		++m_iOne_RenderCount;
		m_tParams.Float4s[0].w = 1.0f;
	}
}

void CTutorial_YesNoUI::ObjectTwo_Tick(const _double & TimeDelta)
{
	if (false == m_bTwo_Rneder)
		return;

	if (1.0f > m_tParams.Float4s[0].w)
	{
		m_tParams.Float4s[0].w += _float(TimeDelta);
	}
	else
	{
		m_tParams.Float4s[0].w = 1.0f;
		m_bTwo_Rneder = false;
		m_bTowRnederEnd = true;
	}
}

void CTutorial_YesNoUI::Icon_Alpha(const _double & TimeDelta)
{
	if (false == m_bOneTwo_Alpha)
		return;

	if (1.0f >= m_tParams.Float4s[0].w)
	{
		m_fAlpha -= _float(TimeDelta);
		m_tParams.Float4s[0].w = m_fAlpha;
	}

	if (0.0f > m_tParams.Float4s[0].w)
	{
		m_fAlpha = 1.0f;
		m_bOneTwo_Alpha = false;

		m_bAlphaEnd = true;
	}
}

void CTutorial_YesNoUI::ObjectYes_Tick()
{
	_bool	bCursor = CUI::IsCursorOn(CGameUtils::GetClientCursor());
	_bool	bKeyDown = CGameInstance::GetInstance()->KeyDown(CInput_Device::DIM_LB);
	_bool	bKeyUp = CGameInstance::GetInstance()->KeyUp(CInput_Device::DIM_LB);
	if (true == bCursor && true == bKeyDown)
	{
		m_bYes = true;
	}

	if (true == bCursor && true == bKeyUp)
	{
		m_bYes = false;
	}
}

void CTutorial_YesNoUI::ObjectNo_Tick()
{
	_bool	bCursor = CUI::IsCursorOn(CGameUtils::GetClientCursor());
	_bool	bKeyDown = CGameInstance::GetInstance()->KeyDown(CInput_Device::DIM_LB);
	_bool	bKeyUp = CGameInstance::GetInstance()->KeyUp(CInput_Device::DIM_LB);
	if (true == bCursor && true == bKeyDown)
	{
		m_bNo = true;
	}

	if (true == bCursor && true == bKeyUp)
	{
		m_bNo = false;
	}
}

void CTutorial_YesNoUI::InvisibleBox()
{
	_bool	bCursor = CUI::IsCursorOn(CGameUtils::GetClientCursor());
	_bool	bKeyDown = CGameInstance::GetInstance()->KeyDown(CInput_Device::DIM_LB);
	if (true == bCursor && true == bKeyDown)
	{
		m_bInvisible = true;
	}
}

void CTutorial_YesNoUI::NextPage()
{
	_bool	bCursor = CUI::IsCursorOn(CGameUtils::GetClientCursor());
	_bool	bKeyDown = CGameInstance::GetInstance()->KeyDown(CInput_Device::DIM_LB);
	if (true == bCursor && true == bKeyDown)
	{
		m_bNextPage = !m_bNextPage;
	}
}
		
CTutorial_YesNoUI * CTutorial_YesNoUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CTutorial_YesNoUI*		pInstance = new CTutorial_YesNoUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTutorial_YesNoUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CTutorial_YesNoUI::Clone(void * pArg)
{
	CTutorial_YesNoUI*		pInstance = new CTutorial_YesNoUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTutorial_YesNoUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTutorial_YesNoUI::Free()
{
	__super::Free();

}
