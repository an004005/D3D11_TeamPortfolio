#include "stdafx.h"
#include "..\public\TipTrigger.h"
#include "Player.h"
#include "Canvas_Tutorial.h"
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
	json["TutorialType"] = m_eTutorialType;
}

void CTipTrigger::LoadFromJson(const Json& json)
{
	CTriggerEx::LoadFromJson(json);
	m_eTutorialType = json["TutorialType"];
}

void CTipTrigger::Imgui_RenderProperty()
{
	CTriggerEx::Imgui_RenderProperty();

	_int iType = m_eTutorialType;
	ImGui::InputInt("Tutorial Type", &iType);
	m_eTutorialType = (CCanvas_Tutorial::TUTORIAL)iType;
}

void CTipTrigger::TriggerInEvent(CGameObject* pObject)
{
	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Tutorial.json");
	auto pTips = dynamic_cast<CCanvas_Tutorial*>(CGameInstance::GetInstance()->Clone_GameObject_Get(LEVEL_NOW, PLAYERTEST_LAYER_FRONTUI, L"Canvas_Tutorial", &json));
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
