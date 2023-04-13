#include "stdafx.h"
#include "..\public\StoryTrigger.h"
#include "Player.h"
#include "JsonStorage.h"
#include "Canvas_LeftTalk.h"
#include "Canvas_MainTalk.h"

CStoryTrigger::CStoryTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTriggerEx(pDevice, pContext)
{
}

CStoryTrigger::CStoryTrigger(const CStoryTrigger& rhs)
	: CTriggerEx(rhs)
{
}

void CStoryTrigger::SaveToJson(Json& json)
{
	CTriggerEx::SaveToJson(json);

	json["TalkIndex"] = m_vecTalkIndex;
	json["QuestIndex"] = m_iQuest;
	json["Talk"] = m_bTalk;
}

void CStoryTrigger::LoadFromJson(const Json& json)
{
	CTriggerEx::LoadFromJson(json);

	vector<_int> vecTalkTemp = json["TalkIndex"];
	m_vecTalkIndex = vecTalkTemp;
	m_iQuest = json["QuestIndex"];
	m_bTalk = json["Talk"];
}

void CStoryTrigger::Imgui_RenderProperty()
{
	CTriggerEx::Imgui_RenderProperty();

	static _int iTalk = 0;
	ImGui::InputInt("Talk", &iTalk);
	ImGui::InputInt("Quest", &m_iQuest);
	ImGui::Checkbox("true : MainTalk / false : LeftTalk", &m_bTalk);
	ImGui::SameLine();
	if (ImGui::Button("PushBack"))
	{
		m_vecTalkIndex.push_back(iTalk);
	}

	for (size_t i = 0; i < m_vecTalkIndex.size(); i++)
	{
		ImGui::Text("Talk : %d / Quest %d", m_vecTalkIndex[i], m_iQuest);
	}
}

void CStoryTrigger::TriggerInEvent(CGameObject* pObject)
{
	if (false == m_bTalk)
	{
		Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_LeftTalk.json");
		auto pTips = dynamic_cast<CCanvas_LeftTalk*>(CGameInstance::GetInstance()->Clone_GameObject_Get(LEVEL_NOW, PLAYERTEST_LAYER_FRONTUI, L"Canvas_LeftTalk", &json));

		for (size_t i = 0; i < m_vecTalkIndex.size(); i++)
		{
			if(m_vecTalkIndex.size() - 1) pTips->Add_Talk(m_vecTalkIndex[i], true, m_iQuest); // 마지막 Talk 에 Quest 넣기
			else pTips->Add_Talk(m_vecTalkIndex[i], true, -1);
		}
	}
	else
	{
		Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_MainTalk.json");
		auto pTips = dynamic_cast<CCanvas_MainTalk*>(CGameInstance::GetInstance()->Clone_GameObject_Get(LEVEL_NOW, PLAYERTEST_LAYER_FRONTUI, L"Canvas_MainTalk", &json));

		for (size_t i = 0; i < m_vecTalkIndex.size(); i++)
			pTips->Add_Talk(static_cast<size_t>(m_vecTalkIndex[i]));
	}

	SetDelete();
}

_bool CStoryTrigger::CheckTriggeredObject(CGameObject* pObject)
{
	return dynamic_cast<CPlayer*>(pObject) != nullptr;
}

void CStoryTrigger::Free()
{
	CTriggerEx::Free();

	m_vecTalkIndex.clear();
}

CStoryTrigger* CStoryTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStoryTrigger* pInstance = new CStoryTrigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStoryTrigger");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CStoryTrigger::Clone(void* pArg)
{
	CStoryTrigger* pInstance = new CStoryTrigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStoryTrigger");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}
