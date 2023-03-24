#include "stdafx.h"
#include "..\public\SimpleTrigger.h"
#include "Player.h"
#include "JsonStorage.h"

CSimpleTrigger::CSimpleTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTriggerEx(pDevice, pContext)
{
}

CSimpleTrigger::CSimpleTrigger(const CSimpleTrigger& rhs)
	: CTriggerEx(rhs)	
{
}


void CSimpleTrigger::SaveToJson(Json& json)
{
	CTriggerEx::SaveToJson(json);
	json["ProtoTag"] = ws2s(m_strProtoTag);
	json["JsonPath"] = m_strJsonPath;
	json["Layer"] = ws2s(m_strLayer);
}

void CSimpleTrigger::LoadFromJson(const Json& json)
{
	CTriggerEx::LoadFromJson(json);
	m_strProtoTag = s2ws(json["ProtoTag"]);
	m_strJsonPath = json["JsonPath"];
	m_strLayer = s2ws(json["Layer"]);
}

void CSimpleTrigger::Imgui_RenderProperty()
{
	CTriggerEx::Imgui_RenderProperty();
	static char szLayer[MAX_PATH]{};
	static char szJsonPath[MAX_PATH]{};
	static char szProtoTag[MAX_PATH]{};

	ImGui::Text("Write Explain of this Trigger in ObjectTag please~");
	ImGui::InputText("Layer", szLayer, MAX_PATH);
	ImGui::InputText("ProtoTag", szProtoTag, MAX_PATH);
	ImGui::InputText("JsonPath", szJsonPath, MAX_PATH);

	m_strLayer = s2ws(szLayer);
	m_strJsonPath = szJsonPath;
	m_strProtoTag = s2ws(szProtoTag);
}

void CSimpleTrigger::TriggerInEvent(CGameObject* pObject)
{
	if (m_strLayer.empty() || m_strProtoTag.empty())
	{
		IM_WARN("Simple Trigger has not layer or prototag");
		SetDelete();
		return;
	}

	auto pProtoType = CGameInstance::GetInstance()->Find_Prototype(LEVEL_NOW, m_strProtoTag.c_str());
	if (pProtoType == nullptr)
	{
		IM_WARN("Simple Trigger, %s is not Exist", ws2s(m_strProtoTag).c_str());
		SetDelete();
		return;
	}

	void* pArg = nullptr;
	Json json = CJsonStorage::GetInstance()->FindOrLoadJson(m_strJsonPath);
	if (json.empty() == false)
		pArg = &json;

	CGameInstance::GetInstance()->Clone_GameObject(LEVEL_NOW, m_strLayer.c_str(), m_strProtoTag.c_str(), pArg);
	SetDelete();
}

_bool CSimpleTrigger::CheckTriggeredObject(CGameObject* pObject)
{
	return dynamic_cast<CPlayer*>(pObject) != nullptr;
}

void CSimpleTrigger::Free()
{
	CTriggerEx::Free();
}

CSimpleTrigger* CSimpleTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSimpleTrigger* pInstance = new CSimpleTrigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSimpleTrigger");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CSimpleTrigger::Clone(void* pArg)
{
	CSimpleTrigger* pInstance = new CSimpleTrigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSimpleTrigger");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}
