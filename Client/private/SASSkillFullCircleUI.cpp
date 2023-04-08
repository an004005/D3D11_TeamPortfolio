#include "stdafx.h"
#include "..\public\SASSkillFullCircleUI.h"
#include "GameInstance.h"

// ���İ��� �پ��鼭, ���� Ŀ����.

CSASSkillFullCircleUI::CSASSkillFullCircleUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CSASSkillFullCircleUI::CSASSkillFullCircleUI(const CSASSkillFullCircleUI& rhs)
	: CUI(rhs)
{
}

HRESULT CSASSkillFullCircleUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSASSkillFullCircleUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	m_vOriginSize = { m_fSizeX * m_vScale.x, m_fSizeY * m_vScale.y };
	m_vCurrentSize = { 150.0f, 150.0f };	// �ִ� ũ�⸦ �̾߱� �Ѵ�.
	m_fSpeed = 50.0f; 

	m_bVisible = true;

	return S_OK;
}

void CSASSkillFullCircleUI::Tick(_double TimeDelta)
{
	if (false == m_bGrow)
		return;

	CUI::Tick(TimeDelta);

	if (m_vOriginSize.x < m_vCurrentSize.x)
	{
		m_vCurrentSize -= {_float(TimeDelta)* m_fSpeed, _float(TimeDelta)* m_fSpeed};
		m_pTransformCom->Set_Scaled(_float3(m_vCurrentSize.x, m_vCurrentSize.y, 1.f));
	}
	else
	{
		m_bGrow = false;
		m_vCurrentSize = { 140.0f, 140.0f };
		m_pTransformCom->Set_Scaled(_float3(m_vOriginSize.x, m_vOriginSize.y, 1.f));
	}
}

CSASSkillFullCircleUI * CSASSkillFullCircleUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CSASSkillFullCircleUI*		pInstance = new CSASSkillFullCircleUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSASSkillFullCircleUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CSASSkillFullCircleUI::Clone(void * pArg)
{
	CSASSkillFullCircleUI*		pInstance = new CSASSkillFullCircleUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSASSkillFullCircleUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSASSkillFullCircleUI::Free()
{
	CUI::Free();

}
