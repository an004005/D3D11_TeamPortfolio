#include "stdafx.h"
#include "..\public\SASSkillFullCircleUI.h"
#include "GameInstance.h"

// 알파값이 줄어들면서, 원이 커진다.

CSASSkillFullCircleUI::CSASSkillFullCircleUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas_SASSkill(pDevice, pContext)
{
}

CSASSkillFullCircleUI::CSASSkillFullCircleUI(const CSASSkillFullCircleUI& rhs)
	: CCanvas_SASSkill(rhs)
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

	CCanvas_SASSkill::UIMove_Initialize();
	
	m_vOriginSize = { m_fSizeX * m_vScale.x, m_fSizeY * m_vScale.y };
	m_vCurrentSize = { 140.0f, 140.0f };	// 최대 크기를 이야기 한다.
	m_fSpeed = 50.0f; 

	return S_OK;
}

void CSASSkillFullCircleUI::Tick(_double TimeDelta)
{
	
	if (CGameInstance::GetInstance()->KeyDown(DIK_0))
	{
		m_bGrow = !m_bGrow;
	}

	if (true == m_bGrow)
	{
		if (m_vOriginSize.x < m_vCurrentSize.x)
		{
			m_vCurrentSize -= {_float(TimeDelta) * m_fSpeed, _float(TimeDelta) * m_fSpeed};
			m_pTransformCom->Set_Scaled(_float3(m_vCurrentSize.x, m_vCurrentSize.y, 1.f));
		}
		else
		{
			m_bGrow = false;
			m_vCurrentSize = { 140.0f, 140.0f };
			m_pTransformCom->Set_Scaled(_float3(m_vOriginSize.x, m_vOriginSize.y, 1.f));
		}
	}
	else
	{
		CUI::Tick(TimeDelta);
	}
}

void CSASSkillFullCircleUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CSASSkillFullCircleUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CSASSkillFullCircleUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CSASSkillFullCircleUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CSASSkillFullCircleUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

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

CCanvas_SASSkill * CSASSkillFullCircleUI::Clone(void * pArg)
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
