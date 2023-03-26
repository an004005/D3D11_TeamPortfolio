#include "stdafx.h"
#include "..\public\TipTrigger.h"
#include "Player.h"
#include "JsonStorage.h"

CTipTrigger::CTipTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTriggerEx(pDevice, pContext)
{
}

CTipTrigger::CTipTrigger(const CTipTrigger& rhs)
	: CTriggerEx(rhs)
{
}

void CTipTrigger::SaveToJson(Json& json)
{
	CTriggerEx::SaveToJson(json);
	json["TutorialOrTips"] = m_bTutorialOrTips;
	json["TipsType"] = m_eTips;
	json["TutorialType"] = m_eTutorialType;
}

void CTipTrigger::LoadFromJson(const Json& json)
{
	CTriggerEx::LoadFromJson(json);
	//m_bTutorialOrTips = json["TutorialOrTips"];
	//m_eTips = json["TipsType"];
	m_eTutorialType = json["TutorialType"];
}

void CTipTrigger::Imgui_RenderProperty()
{
	CTriggerEx::Imgui_RenderProperty();

	_int iTutorialOrTips = m_bTutorialOrTips;
	ImGui::InputInt("0_Tutorial 1_Tips", &iTutorialOrTips);
	m_bTutorialOrTips = (_bool)iTutorialOrTips;

	_int iTips = m_eTips;
	ImGui::InputInt("Tips Type", &iTips);
	m_eTips = (CCanvas_Tutorial::TIPS)iTips;

	_int iTutorial = m_eTutorialType;
	ImGui::InputInt("Tutorial Type", &iTutorial);
	m_eTutorialType = (CCanvas_Tutorial::TUTORIAL)iTutorial;
}

void CTipTrigger::TriggerInEvent(CGameObject* pObject)
{
	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Tutorial.json");
	auto pTips = dynamic_cast<CCanvas_Tutorial*>(CGameInstance::GetInstance()->Clone_GameObject_Get(LEVEL_NOW, PLAYERTEST_LAYER_FRONTUI, L"Canvas_Tutorial", &json));

	if (true == m_bTutorialOrTips)
		pTips->Set_Tips(m_eTips);
	else
		pTips->Set_Tutorial(m_eTutorialType);

	SetDelete();
}

_bool CTipTrigger::CheckTriggeredObject(CGameObject* pObject)
{
	return dynamic_cast<CPlayer*>(pObject) != nullptr;
}

void CTipTrigger::Free()
{
	CTriggerEx::Free();
}

CTipTrigger* CTipTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTipTrigger* pInstance = new CTipTrigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTipTrigger");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CTipTrigger::Clone(void* pArg)
{
	CTipTrigger* pInstance = new CTipTrigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTipTrigger");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}
