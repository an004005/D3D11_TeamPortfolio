#include "stdafx.h"
#include "..\public\Canvas_BossAppear.h"
#include "GameInstance.h"

CCanvas_BossAppear::CCanvas_BossAppear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_BossAppear::CCanvas_BossAppear(const CCanvas_BossAppear& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_BossAppear::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_BossAppear::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	return S_OK;
}

void CCanvas_BossAppear::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

}

void CCanvas_BossAppear::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_BossAppear::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CCanvas_BossAppear::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

}

void CCanvas_BossAppear::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_BossAppear::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

CCanvas_BossAppear * CCanvas_BossAppear::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_BossAppear*		pInstance = new CCanvas_BossAppear(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_BossAppear");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_BossAppear::Clone(void * pArg)
{
	CCanvas_BossAppear*		pInstance = new CCanvas_BossAppear(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_BossAppear");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_BossAppear::Free()
{
	CCanvas::Free();
}
