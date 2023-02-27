#include "stdafx.h"
#include "..\public\SASSkillFullCircleUI.h"
#include "GameInstance.h"
#include "JsonLib.h"

// 알파값이 줄어들면서, 원이 커진다.

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

	m_vFirstSize = { m_fSizeX * m_vScale.x, m_fSizeY * m_vScale.y };
	m_vCurrentSize = { 140.0f, 140.0f };	// 최대 크기를 이야기 한다.
	m_fSpeed = 50.0f; 

	return S_OK;
}

void CSASSkillFullCircleUI::BeginTick()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pCanvas = dynamic_cast<CCanvas_SASSkill*>(pGameInstance->Find_Prototype(LEVEL_NOW, TEXT("Canvas_SASSkill")));
}

void CSASSkillFullCircleUI::Tick(_double TimeDelta)
{
	IM_LOG("X %f", m_vCurrentSize.x);
	IM_LOG("Y %f", m_vCurrentSize.y);

	if (CGameInstance::GetInstance()->KeyDown(DIK_0))
	{
		m_bGrow = !m_bGrow;
	}

	if (true == m_bGrow)
	{
		if (m_vFirstSize.x < m_vCurrentSize.x)
		{
			m_vCurrentSize -= {_float(TimeDelta) * m_fSpeed, _float(TimeDelta) * m_fSpeed};
			m_pTransformCom->Set_Scaled(_float3(m_vCurrentSize.x, m_vCurrentSize.y, 1.f));
		}
		else
		{
			m_bGrow = false;
			m_vCurrentSize = { 140.0f, 140.0f };
			m_pTransformCom->Set_Scaled(_float3(m_vFirstSize.x, m_vFirstSize.y, 1.f));
		}
	}
	else
	{
		__super::Tick(TimeDelta);
	}
}

void CSASSkillFullCircleUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CSASSkillFullCircleUI::Render()
{
	if (FAILED(__super::Render()))
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
	__super::Free();

}
