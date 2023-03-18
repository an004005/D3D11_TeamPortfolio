#include "stdafx.h"
#include "..\public\Canvas_Main.h"
#include "GameInstance.h"
#include "UI_Manager.h"

CCanvas_Main::CCanvas_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_Main::CCanvas_Main(const CCanvas_Main& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_Main::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_Main::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	return S_OK;
}

void CCanvas_Main::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	KeyInput();
}

void CCanvas_Main::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_Main::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CCanvas_Main::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

}

void CCanvas_Main::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_Main::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

void CCanvas_Main::KeyInput()
{
	if (CGameInstance::GetInstance()->KeyDown(DIK_ESCAPE))
	{
		m_bMainUI = !m_bMainUI;

		for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
			iter->second->SetVisible(m_bMainUI);

		CUI_Manager::GetInstance()->Set_TempOff(m_bMainUI);	// m_bMainUI 와 반대로 동작한다.
	}
}

CCanvas_Main * CCanvas_Main::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_Main*		pInstance = new CCanvas_Main(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_Main");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_Main::Clone(void * pArg)
{
	CCanvas_Main*		pInstance = new CCanvas_Main(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_Main");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_Main::Free()
{
	CCanvas::Free();
}
