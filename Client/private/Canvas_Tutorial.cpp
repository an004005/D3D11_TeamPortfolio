#include "stdafx.h"
#include "..\public\Canvas_Tutorial.h"
#include "GameInstance.h"

#include "TutorialUI.h"

CCanvas_Tutorial::CCanvas_Tutorial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_Tutorial::CCanvas_Tutorial(const CCanvas_Tutorial& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_Tutorial::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_Tutorial::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCanvas_Tutorial::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	//Tutorial_Tick();
}

void CCanvas_Tutorial::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();


}

void CCanvas_Tutorial::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_Tutorial::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

void CCanvas_Tutorial::Tutorial_Tick()
{
	switch (m_eTutorial)
	{
	case Client::CCanvas_Tutorial::LOCKON:
		Lockon();
		break;
	case Client::CCanvas_Tutorial::FIGHTINGSTYLE:
		break;
	case Client::CCanvas_Tutorial::SPECIALATTACK:
		break;
	case Client::CCanvas_Tutorial::ADDRUSHATTACK:
		break;
	case Client::CCanvas_Tutorial::ADDPSYCHOKINESISATTACK:
		break;
	case Client::CCanvas_Tutorial::STRENGTHENATTACK:
		break;
	default:
		return; // No Tutorials
		break;
	}
}

void CCanvas_Tutorial::Lockon()
{
	if (false == m_arrTutorial)
		return;

	dynamic_cast<CTutorialUI*>(Find_ChildUI(L"Tutorial0"))->Set_OnTutorial();
	dynamic_cast<CTutorialUI*>(Find_ChildUI(L"Tutorial0"))->Set_OffTutorial();
}

CCanvas_Tutorial * CCanvas_Tutorial::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_Tutorial*		pInstance = new CCanvas_Tutorial(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_Tutorial");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_Tutorial::Clone(void * pArg)
{
	CCanvas_Tutorial*		pInstance = new CCanvas_Tutorial(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_Tutorial");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_Tutorial::Free()
{
	CCanvas::Free();
}
