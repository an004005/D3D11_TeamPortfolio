#include "stdafx.h"
#include "Trigger.h"
#include "GameInstance.h"
#include "RigidBody.h"
#include "SkummyPandou.h"
#include "SkummyPool.h"
#include "FlowerLeg.h"
#include "BuddyLumi.h"
#include "BronJon.h"
#include "ImGuizmo.h"
#include "JsonLib.h"
#include "JsonStorage.h"
#include "Canvas_Tutorial.h"

CTrigger::CTrigger(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CTrigger::CTrigger(const CTrigger & rhs)
	: CGameObject(rhs)
{
}

HRESULT Client::CTrigger::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT Client::CTrigger::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	if (m_pRigidBodyCom == nullptr)
	{
		SetUp_Components(pArg);
	}
	return S_OK;
}

void Client::CTrigger::BeginTick()
{
	__super::BeginTick();
}

void Client::CTrigger::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	m_pRigidBodyCom->Update_Tick(m_pTransformCom);
}

void Client::CTrigger::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

void CTrigger::SaveToJson(OUT Json & json)
{
	__super::SaveToJson(json);

	json["Usage"] = m_eUsage;

	if (m_eUsage == CREATE)
	{
		//wstring -> string
		map<string, vector<_float4x4>> tmp;

		for (auto iter : m_ProtoWorldMatrixes)
		{
			string str = ws2s(iter.first);
			tmp.emplace(str, iter.second);
		}

		json["Create"] = tmp;
	}
	else if(m_eUsage == TUTORIAL)
	{
		json["Tutorial"] = m_eTutorial;

	}
	else
	{
		MSG_BOX("No_Usage");
	}
}

void CTrigger::LoadFromJson(const Json & json)
{
	SetUp_Components(const_cast<Json*>(&json));

	__super::LoadFromJson(json);

	if (json.contains("Usage"))
	{
		USAGE eUsage = json["Usage"];
		m_eUsage = eUsage;

		SetUp_InitInfo(json);
	}
}

void CTrigger::AfterPhysX()
{
	__super::AfterPhysX();
	m_pRigidBodyCom->Update_AfterPhysX(m_pTransformCom);
}

HRESULT Client::CTrigger::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}



void Client::CTrigger::Imgui_RenderProperty()
{
	ImGui::Begin("Trigger");

	__super::Imgui_RenderProperty();

	if (m_eUsage == CREATE)
	{
		if (ImGui::BeginListBox("Trigger List"))
		{
			for (auto iter : m_ProtoWorldMatrixes)
			{
				const bool bSelected = (iter.first == m_PotoTag);

				if (bSelected)
					ImGui::SetItemDefaultFocus();

				const string str = ws2s(iter.first);

				if (ImGui::Selectable(str.c_str(), bSelected))
				{
					m_PotoTag = iter.first;
				}
			}

			ImGui::EndListBox();
		}


		auto iter = m_ProtoWorldMatrixes.find(m_PotoTag);

		if (iter != m_ProtoWorldMatrixes.end())
		{
			_int iCount = 0;
			for (auto matrix : (*iter).second)
			{
				const string str = "WorldMatrix " + to_string(iCount++);
				ImGui::Selectable(str.c_str(), false);
			}
		}
	}
	else if (m_eUsage == TUTORIAL)
	{
	}

	

	ImGui::End();
}

void CTrigger::Set_ForCreate(const wstring& ProtoTag, const _float4x4 WorldMatrix)
{
	m_eUsage = CREATE;

	auto& iter = m_ProtoWorldMatrixes.find(ProtoTag);

	if (iter == m_ProtoWorldMatrixes.end())
	{
		vector<_float4x4> WorldMatrixes;
		WorldMatrixes.emplace_back(WorldMatrix);
		m_ProtoWorldMatrixes.emplace(ProtoTag, WorldMatrixes);
	}
	else
	{
		iter->second.emplace_back(WorldMatrix);
	}
}

void CTrigger::Set_ForTutorial(CCanvas_Tutorial::TUTORIAL eTutorial)
{
	m_eUsage = TUTORIAL;

	m_eTutorial = eTutorial;

}

HRESULT CTrigger::SetUp_Components(void * pArg)
{
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"), TEXT("Com_Ridgid"),
		(CComponent**)&m_pRigidBodyCom, pArg));

	m_pRigidBodyCom->Set_Trigger(true);
	m_pRigidBodyCom->Set_ColliderType(CT_TRIGGER_FOR_PLAYER);
	m_pRigidBodyCom->UpdateChange();
	return S_OK;
}

void CTrigger::SetUp_InitInfo(const Json & json)
{
	switch (m_eUsage)
	{
	case CREATE:
		SetUp_Create(json);
		break;
	case TUTORIAL:
		SetUp_Tutorial(json);
		break;
	case USAGE_END:
		assert(!"No Usage In Data");
		break;
	default:
		assert(!"What the hell?");
		break;
	}
}

void CTrigger::SetUp_Create(const Json & json)
{
	map<string, vector<_float4x4>> tmp = json["Create"];

	//수정시 데이터를 불러오기 위해
	if (LEVEL_NOW == LEVEL_BATCH)
	{

		//맵을 그냥 복사하는건데 같은 테그가 있을리가?
		for (auto it : tmp)
			m_ProtoWorldMatrixes.emplace(s2ws(it.first), it.second);
		
	}

	//생성 후 보관
	for (auto iter : tmp)
	{
		for (auto matrix : iter.second)
		{
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			CMonster* pMonster = nullptr;
			Json jsonWorldMatrix;

			CTransform::ModifyTransformJson(jsonWorldMatrix, matrix);
			pMonster = dynamic_cast<CMonster*>(pGameInstance->Clone_GameObject_NoLayer(LEVEL_NOW, s2ws(iter.first).c_str(), &jsonWorldMatrix));

			assert(pMonster != nullptr);
			m_pMonsters.emplace_back(pMonster);
		}

	}

	m_pRigidBodyCom->SetOnTriggerIn([this](CGameObject* pGameObject) {

		for_each(m_pMonsters.begin(), m_pMonsters.end(), [](CMonster* pMonster) {
			CGameInstance* pGameinstance = CGameInstance::GetInstance();
			pGameinstance->Add_InLayer(TEXT("Layer_Monster"), pMonster);
			pMonster->SetActive();
		});

		//끝나고 트리거 삭제
		m_bDelete = true;
		m_pMonsters.clear();
	});
	
}

void CTrigger::SetUp_Tutorial(const Json & json)
{
	m_eTutorial = json["Tutorial"];
	Json jsonTutorial = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Tutorial.json");
	m_pTutorial = CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_UI", L"Canvas_Tutorial", &jsonTutorial);

	m_pRigidBodyCom->SetOnTriggerIn([this](CGameObject* pGameObject) {

		dynamic_cast<CCanvas_Tutorial*>(m_pTutorial)->Set_Tutorial(m_eTutorial);

		//끝나고 트리거 삭제
		m_bDelete = true;
	});
}

CTrigger * CTrigger::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTrigger* pInstance = new CTrigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTrigger");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* Client::CTrigger::Clone(void* pArg /*= nullptr*/)
{
	CTrigger* pInstance = new CTrigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTrigger");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void Client::CTrigger::Free()
{
	__super::Free();
	Safe_Release(m_pRigidBodyCom);

	for (auto it : m_pMonsters)
		Safe_Release(it);
}
