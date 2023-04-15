#include "stdafx.h"
#include "..\public\Canvas_Alarm.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "NextMapNameUI.h"
#include "Boss_AppearUI.h"
#include "Boss_AppearBackUI.h"
#include "LevelUpUI.h"
#include "MapNameUI.h"
#include "ShaderUI.h"

CCanvas_Alarm::CCanvas_Alarm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_Alarm::CCanvas_Alarm(const CCanvas_Alarm& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_Alarm::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_Alarm::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	return S_OK;
}

void CCanvas_Alarm::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	Appeart_Tick();
	MapName_Tick(TimeDelta);
	LevelUpDead_Tick();
}

void CCanvas_Alarm::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_Alarm::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CCanvas_Alarm::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

	if (ImGui::Button("Open Next Map Name UI"))
	{
		Set_OpenNextRoomName(L"천세의 방");
	}

	if (ImGui::Button("Boss Appeart"))
	{
		Set_Appeart();
	}

	if (ImGui::Button("Level Up"))
	{
		Set_LevelUp(20);
	}

	if (ImGui::Button("Map Name"))
	{
		Set_MapName(3.0f);
	}
}

void CCanvas_Alarm::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_Alarm::LoadFromJson(const Json& json)
{
	CCanvas::LoadFromJson(json);
}

void CCanvas_Alarm::Set_OpenNextRoomName(const wstring wsNextRoomName)
{
	dynamic_cast<CNextMapNameUI*>(Find_ChildUI(L"NextMapName"))->Set_NextRoolName(wsNextRoomName);

}

void CCanvas_Alarm::Set_LevelUp(const _uint iLevel)
{
	dynamic_cast<CLevelUpUI*>(Find_ChildUI(L"LevelUp"))->Set_LevelUp(iLevel);
	dynamic_cast<CLevelUpUI*>(Find_ChildUI(L"LevelUpBack"))->Set_LevelUpBack();
	dynamic_cast<CLevelUpUI*>(Find_ChildUI(L"LevelUpBackGround"))->Set_LevelUpBackGround();
	m_bLevelUpDead = true;
}

void CCanvas_Alarm::Set_Appeart()
{
	m_bCheck_Appeart = true;
	dynamic_cast<CBoss_AppearUI*>(Find_ChildUI(L"Boss_Appear"))->Set_Appear();
	dynamic_cast<CBoss_AppearBackUI*>(Find_ChildUI(L"Boss_AppearBackBackGround"))->Set_AppearBackGround();
}

void CCanvas_Alarm::Appeart_Tick()
{
	if (false == m_bCheck_Appeart)
		return;

	_int iAppearTextureNum = dynamic_cast<CBoss_AppearUI*>(Find_ChildUI(L"Boss_Appear"))->Get_TextureNum();

	if (29 == iAppearTextureNum)
	{
		dynamic_cast<CBoss_AppearBackUI*>(Find_ChildUI(L"Boss_AppearBackBackGround"))->Set_AppearEnd();
		m_bCheck_Appeart = false;
	}
}

void CCanvas_Alarm::MapName_Tick(const _double& dTimeDelta)
{
	if (m_bMapName == false) return;

	if (0.0 == m_bMapName_TimeAcc)
	{
		Find_ChildUI(L"MapName")->SetVisible(true);
		dynamic_cast<CMapNameUI*>(Find_ChildUI(L"MapName_BackGround"))->Set_AppearBackGround();
		dynamic_cast<CShaderUI*>(Find_ChildUI(L"MapName"))->Set_Float2s(_float2(m_fMapNameIndex, 0.0f));
	}

	m_bMapName_TimeAcc += dTimeDelta;
	if (5.0 < m_bMapName_TimeAcc)
	{
		CGameObject::SetDelete();
	}

	_float fAlpha = dynamic_cast<CShaderUI*>(Find_ChildUI(L"MapName"))->Get_Float4s_W();
	if (3.0 < m_bMapName_TimeAcc)
	{
		dynamic_cast<CMapNameUI*>(Find_ChildUI(L"MapName_BackGround"))->Set_AppearEnd();

		fAlpha -= _float(dTimeDelta) * 0.5f;
		dynamic_cast<CShaderUI*>(Find_ChildUI(L"MapName"))->Set_Float4s_W(fAlpha);
	}
	else
	{
		fAlpha += _float(dTimeDelta) * 0.5f;
		if (0.9f < fAlpha)
			fAlpha = 0.9f;
		dynamic_cast<CShaderUI*>(Find_ChildUI(L"MapName"))->Set_Float4s_W(fAlpha);
	}
}

void CCanvas_Alarm::LevelUpDead_Tick()
{
	if (false == m_bLevelUpDead) return;

	if (false == dynamic_cast<CLevelUpUI*>(Find_ChildUI(L"LevelUpBack"))->Get_LevelUpBack())
	{
		m_bLevelUpDead = false;
		CGameObject::SetDelete();	// Level 에서 ImGui 로 버튼 눌러서 지운거면 오류 난다!
	}
}

CCanvas_Alarm* CCanvas_Alarm::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCanvas_Alarm* pInstance = new CCanvas_Alarm(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_Alarm");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas* CCanvas_Alarm::Clone(void* pArg)
{
	CCanvas_Alarm* pInstance = new CCanvas_Alarm(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_Alarm");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_Alarm::Free()
{
	CCanvas::Free();
}