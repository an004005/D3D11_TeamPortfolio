#include "stdafx.h"
#include "..\public\Alarm_Trigger.h"
#include "Player.h"
#include "JsonStorage.h"

#include "Canvas_Alarm.h"

CAlarm_Trigger::CAlarm_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTriggerEx(pDevice, pContext)
{
}

CAlarm_Trigger::CAlarm_Trigger(const CAlarm_Trigger& rhs)
	: CTriggerEx(rhs)
{
}

void CAlarm_Trigger::SaveToJson(Json& json)
{
	CTriggerEx::SaveToJson(json);

	json["eLevel"] = m_eLevel;
}

void CAlarm_Trigger::LoadFromJson(const Json& json)
{
	CTriggerEx::LoadFromJson(json);

	m_eLevel = json["eLevel"];

	switch (m_eLevel)
	{
	case Client::LEVEL_DOWNTOWN_1: // 12
		m_wsName = L"스오-류신구";
		break;
	case Client::LEVEL_DOWNTOWN_2:	// 13
		m_wsName = L"스오-류신구";
		break;
	case Client::LEVEL_DOWNTOWN_3:	// 14
		m_wsName = L"스오-류신구";
		break;
	case Client::LEVEL_TUTORIAL:	// 20
		m_wsName = L"대로";
		break;
	case Client::LEVEL_CONSTRUCTIONSITE_3F:	// 21
		m_wsName = L"키쿠치바 3층";
		break;
	case Client::LEVEL_CONSTRUCTIONSITE_2F:	// 22
		m_wsName = L"키쿠치바 2층";
		break;
	case Client::LEVEL_SUBWAY:	// 23
		m_wsName = L"폐지하선로 스오 9호선";
		break;
	case Client::LEVEL_NAOMIROOM:	// 24
		m_wsName = L"???";
		break;
	case Client::LEVEL_HOSPITAL_1F:	// 25
		m_wsName = L"구 토벌군병원";
		break;
	case Client::LEVEL_FINAL_STAGE:	// 26
		m_wsName = L"????";
		break;
	default:
		m_wsName = L"쥬신 게임아카데미";
		break;
	}
}

void CAlarm_Trigger::Imgui_RenderProperty()
{
	CTriggerEx::Imgui_RenderProperty();
	
	static _int iLevel = 0;
	ImGui::InputInt("Level", &iLevel);
	m_eLevel = LEVEL(iLevel);
}

void CAlarm_Trigger::TriggerInEvent(CGameObject* pObject)
{
	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Alarm.json");
	auto pUI = dynamic_cast<CCanvas_Alarm*>(CGameInstance::GetInstance()->Clone_GameObject_Get(LEVEL_NOW, PLAYERTEST_LAYER_FRONTUI, L"Canvas_Alarm", &json));
	assert(pUI != nullptr && "Failed to Cloned : CCanvas_Alarm");

	pUI->Set_OpenNextRoomName(m_wsName);

	SetDelete();
}

_bool CAlarm_Trigger::CheckTriggeredObject(CGameObject* pObject)
{
	return dynamic_cast<CPlayer*>(pObject) != nullptr;
}

void CAlarm_Trigger::Free()
{
	CTriggerEx::Free();

}

CAlarm_Trigger* CAlarm_Trigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAlarm_Trigger* pInstance = new CAlarm_Trigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAlarm_Trigger");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CAlarm_Trigger::Clone(void* pArg)
{
	CAlarm_Trigger* pInstance = new CAlarm_Trigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAlarm_Trigger");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}
