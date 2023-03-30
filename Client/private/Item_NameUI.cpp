#include "stdafx.h"
#include "..\public\Item_NameUI.h"
#include "GameInstance.h"

CItem_NameUI::CItem_NameUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CItem_NameUI::CItem_NameUI(const CItem_NameUI& rhs)
	: CUI(rhs)
{
}

HRESULT CItem_NameUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_NameUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CItem_NameUI::BeginTick()
{

}

void CItem_NameUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);


	
}

void CItem_NameUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CItem_NameUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CItem_NameUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CItem_NameUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CItem_NameUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CItem_NameUI::Set_ItemName(const _float & fIndex)
{
	// [0] ��ĭ [1] ȸ��(��)���� [2] ȸ��(��)���� [3] ȸ��(��)���� [4] ��ü: ȸ�� (��) ���� [5] ��ü: ȸ�� (��) ���� [6] ��ü: ȸ�� (��) ���� 
	// [7] ����ȭ �º� [8] ��ü: ����ȭ �º� [9] �극�� �ʵ� ��� [10] SAS ���޼� 
	m_tParams.Float2s[0].x = fIndex;
}

CItem_NameUI * CItem_NameUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CItem_NameUI*		pInstance = new CItem_NameUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CItem_NameUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CItem_NameUI::Clone(void * pArg)
{
	CItem_NameUI*		pInstance = new CItem_NameUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CItem_NameUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CItem_NameUI::Free()
{
	CUI::Free();

}
