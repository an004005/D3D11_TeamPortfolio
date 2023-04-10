#include "stdafx.h"
#include "..\public\SpawnTrigger.h"

#include <GameUtils.h>

#include "Enemy.h"
#include "InvisibleWall.h"
#include "Player.h"
#include "JsonLib.h"
#include "Layer.h"

namespace nlohmann
{
	template <>
	struct adl_serializer<ENEMY_STAT>
	{
		static void to_json(json& j, const ENEMY_STAT& value)
		{
			j["MaxHP"] = value.iMaxHP;
			j["MaxCrushGage"] = value.iMaxCrushGage;
			j["HasCrushGage"] = value.bHasCrushGage;
			j["AtkDamage"] = value.iAtkDamage;
			j["Level"] = value.iLevel;
		}

		static void from_json(const json& j, ENEMY_STAT& value)
		{
			value.iMaxHP = j["MaxHP"];
			value.iMaxCrushGage = j["MaxCrushGage"];
			value.bHasCrushGage = j["HasCrushGage"];
			value.iAtkDamage = j["AtkDamage"];
			value.iLevel = j["Level"];
		}
	};

	template <>
	struct adl_serializer<EnemyWithPos>
	{
		static void to_json(json& j, const EnemyWithPos& value)
		{
			j["EnemyTag"] = value.strEnemyTag;
			j["WorldMatrix"] = value.WorldMatrix;
			j["EnemyStat"] = value.tStat;
		}

		static void from_json(const json& j, EnemyWithPos& value)
		{
			value.strEnemyTag = j["EnemyTag"];
			value.WorldMatrix = j["WorldMatrix"];
			value.tStat = j["EnemyStat"];
		}
	};
}

CSpawnTrigger::CSpawnTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTriggerEx(pDevice, pContext)
{
}

CSpawnTrigger::CSpawnTrigger(const CSpawnTrigger& rhs)
	: CTriggerEx(rhs)
{
}

HRESULT CSpawnTrigger::Initialize(void* pArg)
{
	FAILED_CHECK(CTriggerEx::Initialize(pArg));

	m_WaitingEnemies.resize(m_SpawnEnemies.size());
	for (auto& EnemyDataList : m_SpawnEnemies)
	{
		list<CEnemy*> WaitingEnemiesList;

		for (auto& EnemyData : EnemyDataList)
		{
			auto pObj = CGameInstance::GetInstance()->Clone_GameObject_NoLayerNoBegin(LEVEL_NOW, s2ws(EnemyData.strEnemyTag).c_str());
			if (auto pEnemy = dynamic_cast<CEnemy*>(pObj))
			{
				pEnemy->SetEnemyBatchDataStat(EnemyData.tStat);
				pEnemy->GetTransform()->Set_WorldMatrix(EnemyData.WorldMatrix);
				WaitingEnemiesList.push_back(pEnemy);
			}
		}

		m_WaitingEnemies.push_back(WaitingEnemiesList);
	}

	if (CGameInstance::GetInstance()->GetLayer(LEVEL_NOW, PLAYERTEST_LAYER_MONSTER) == nullptr)
	{
		CGameInstance::GetInstance()->Add_EmptyLayer(LEVEL_NOW, PLAYERTEST_LAYER_MONSTER);
	}

	if (m_pWall == nullptr)
	{
		m_pWall = dynamic_cast<CInvisibleWall*>(CGameInstance::GetInstance()->Clone_GameObject_Get(LEVEL_NOW, LAYER_TRIGGER, L"Prototype_InvisibleWall"));
	}


	return S_OK;
}

void CSpawnTrigger::Tick(_double TimeDelta)
{
	CTriggerEx::Tick(TimeDelta);

	if (m_bStart)
	{
		if (m_iSpawnOrder >= m_WaitingEnemies.size())
		{
			if (m_WallDeActiveOnce.IsNotDo())
			{
				m_pWall->Activate(false);
			}
			if (m_pWall->IsActive() == false)
			{
				SetDelete();
			}

			return;
		}

		if (auto pEnemyLayer = CGameInstance::GetInstance()->GetLayer(LEVEL_NOW, PLAYERTEST_LAYER_MONSTER))
		{
			if (pEnemyLayer->GetGameObjects().empty())
			{
				for (auto pEnemy : m_WaitingEnemies[m_iSpawnOrder])
				{
					pEnemy->BeginTick();
					CGameInstance::GetInstance()->Add_InLayer(PLAYERTEST_LAYER_MONSTER, pEnemy);
				}
				m_WaitingEnemies[m_iSpawnOrder].clear();
				++m_iSpawnOrder;
			}
		}
	}
}

void CSpawnTrigger::SaveToJson(Json& json)
{
	CTriggerEx::SaveToJson(json);
	json["SpawnEnemies"] = m_SpawnEnemies;
	m_pWall->SaveToJson(json["InvisibleWall"]);
}

void CSpawnTrigger::LoadFromJson(const Json& json)
{
	CTriggerEx::LoadFromJson(json);
	m_SpawnEnemies = json["SpawnEnemies"];

	if (json.contains("InvisibleWall"))
	{
		Json wallJson = json["InvisibleWall"];
		m_pWall = dynamic_cast<CInvisibleWall*>(CGameInstance::GetInstance()->Clone_GameObject_Get(LEVEL_NOW, LAYER_TRIGGER, L"Prototype_InvisibleWall", &wallJson));
	}
}

void CSpawnTrigger::Imgui_RenderProperty()
{
	CTriggerEx::Imgui_RenderProperty();
	static EEnemyName eSelectEnemyEnum = EEnemyName::EM0200;
	static list<CEnemy*> TmpEnemies;

	const _int iOrder = (_int)m_SpawnEnemies.size();
	ImGui::Text("Current Order : %d", iOrder);
	if (ImGui::BeginCombo("EnemyName", CClientUtils::GetEnemyProtoTag(eSelectEnemyEnum).c_str()))
	{
		for (int i = 0; i < ENEMY_CNT; ++i)
		{
			const bool bSelected = false;
			if (ImGui::Selectable(CClientUtils::GetEnemyProtoTag(static_cast<EEnemyName>(i)).c_str(), bSelected))
				eSelectEnemyEnum = static_cast<EEnemyName>(i);
		}
		ImGui::EndCombo();
	}

	static _bool bClickClone = false;
	ImGui::Checkbox("Click Enemy Clone Once", &bClickClone);
	if (CGameInstance::GetInstance()->KeyDown(CInput_Device::DIM_LB) && bClickClone)
	{
		_float4 vPos;
		if (CGameUtils::GetPosFromRayCast(vPos))
		{
			auto pEnemy = dynamic_cast<CEnemy*>(CGameInstance::GetInstance()->Clone_GameObject_Get(PLAYERTEST_LAYER_MONSTER, s2ws(CClientUtils::GetEnemyProtoTag(eSelectEnemyEnum)).c_str()));
			Assert(pEnemy != nullptr);
			pEnemy->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, vPos);
			TmpEnemies.push_back(pEnemy);
			bClickClone = false;
		}
	}

	{
		static CEnemy* pSelectEnemy = nullptr;
		if (CGameInstance::GetInstance()->Check_ObjectAlive(pSelectEnemy) == false)
			pSelectEnemy = nullptr;

		if (ImGui::BeginListBox("Current Order Enemies"))
		{
			for (auto pEnemy : TmpEnemies)
			{
				const bool bSelected = pSelectEnemy == pEnemy;
				if (bSelected)
					ImGui::SetItemDefaultFocus();

				char szAddressName[256];
				sprintf_s(szAddressName, "%s[%p]", ws2s(pEnemy->GetPrototypeTag()).c_str(), pEnemy);

				if (ImGui::Selectable(szAddressName, bSelected))
					pSelectEnemy = pEnemy;
			}

			ImGui::EndListBox();
		}
		if (ImGui::Button("Delete Select Obj") && pSelectEnemy)
		{
			TmpEnemies.remove_if([this](CEnemy* pEnemy)
			{
				return pEnemy == pSelectEnemy;
			});
			pSelectEnemy->SetDelete();
			pSelectEnemy = nullptr;
		}
		ImGui::Indent(20.f);
		if (ImGui::CollapsingHeader("Selected Object Properties") && pSelectEnemy)
		{
			if (ImGui::BeginChild("Triggered Spawn Monster", ImVec2{500.f ,600.f}))
			{
				pSelectEnemy->Imgui_RenderProperty();
				pSelectEnemy->Imgui_RenderComponentProperties();
				ImGui::EndChild();
			}
		}
		ImGui::Unindent(20.f);
	}


	if (ImGui::Button("Fix Current Order"))
	{
		EnemyWithPos tData;
		list<EnemyWithPos> tDataList;
		for (auto pEnemy : TmpEnemies)
		{
			tData.strEnemyTag = ws2s(pEnemy->GetPrototypeTag());
			tData.WorldMatrix = pEnemy->GetTransform()->Get_WorldMatrix_f4x4();
			tData.tStat = pEnemy->GetEnemyBatchDataStat();
			tDataList.push_back(tData);
			pEnemy->SetDelete();
		}

		m_SpawnEnemies.push_back(tDataList);
		TmpEnemies.clear();
	}


	static _bool bWallEditor = false;
	ImGui::Checkbox("Open WallEditor", &bWallEditor);
	ImGui::Begin("WallEditor", &bWallEditor);
	m_pWall->Imgui_RenderProperty();
	m_pWall->Imgui_RenderComponentProperties();
	ImGui::End();
}

void CSpawnTrigger::TriggerInEvent(CGameObject* pObject)
{
	m_bStart = true;
	if (m_pWall)
	{
		m_pWall->Activate(true);
	}
}

_bool CSpawnTrigger::CheckTriggeredObject(CGameObject* pObject)
{
	return dynamic_cast<CPlayer*>(pObject) != nullptr;
}

void CSpawnTrigger::Free()
{
	CTriggerEx::Free();
	for (auto& EnemyList : m_WaitingEnemies)
	{
		for (auto pEnemy : EnemyList)
			Safe_Release(pEnemy);
	}
	m_WaitingEnemies.clear();
	m_SpawnEnemies.clear();

	m_pWall->SetDelete();
}

CSpawnTrigger* CSpawnTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpawnTrigger* pInstance = new CSpawnTrigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTrigger");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CSpawnTrigger::Clone(void* pArg)
{
	CSpawnTrigger* pInstance = new CSpawnTrigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpawnTrigger");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}
