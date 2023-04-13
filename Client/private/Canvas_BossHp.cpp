#include "stdafx.h"
#include "..\public\Canvas_BossHp.h"
#include "GameInstance.h"

#include "ShaderUI.h"

CCanvas_BossHp::CCanvas_BossHp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_BossHp::CCanvas_BossHp(const CCanvas_BossHp& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_BossHp::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_BossHp::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	return S_OK;
}

void CCanvas_BossHp::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

}

void CCanvas_BossHp::Set_NoMove()
{
	Find_ChildUI(L"Boss_HpBackGround")->SetVisible(true);
	Find_ChildUI(L"Boss_Name")->SetVisible(true);
}

void CCanvas_BossHp::Set_LevelName(const _float iLevel, const _float fName)
{
	dynamic_cast<CShaderUI*>(Find_ChildUI(L"Boss_Name"))->Set_Float2s(_float2(iLevel, fName));
}

CCanvas_BossHp * CCanvas_BossHp::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_BossHp*		pInstance = new CCanvas_BossHp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_BossHp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_BossHp::Clone(void * pArg)
{
	CCanvas_BossHp*		pInstance = new CCanvas_BossHp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_BossHp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_BossHp::Free()
{
	CCanvas::Free();
}
