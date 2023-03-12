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
	else
	{
		MSG_BOX("No_Usage");
	}
}

void CTrigger::LoadFromJson(const Json & json)
{
	SetUp_Components(const_cast<Json*>(&json));

	__super::LoadFromJson(json);

	USAGE eUsage = json["Usage"];
	m_eUsage = eUsage;

	SetUp_InitInfo(json);
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
	

	ImGui::End();
}

void CTrigger::Set_ForCreate(const wstring& ProtoTag, const _float4x4 WorldMatrix)
{
	if (m_eUsage != USAGE_END || m_eUsage != CREATE)
	{
		MSG_BOX("Diffent Usage");
		return;
	}
	
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

HRESULT CTrigger::SetUp_Components(void * pArg)
{
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"), TEXT("Com_Ridgid"),
		(CComponent**)&m_pRigidBodyCom, pArg));

	m_pRigidBodyCom->Set_Trigger();
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

	for (auto iter : tmp)
	{
		wstring wstr = s2ws(iter.first);
		m_ProtoWorldMatrixes.emplace(wstr, iter.second);
	}

	m_pRigidBodyCom->SetOnTriggerIn([this](CGameObject* pGameObject) {

		for (auto proto : m_ProtoWorldMatrixes)
		{
			for (auto matrix : proto.second)
			{
				CGameInstance* pGameInstance = CGameInstance::GetInstance();
				CGameObject* pObject = nullptr;

				Json jsonWorldMatrix;
				CTransform::ModifyTransformJson(jsonWorldMatrix, matrix);
				pObject = pGameInstance->Clone_GameObject_Get(TEXT("Layer_AssortedObj"), proto.first.c_str(), &jsonWorldMatrix);
			}
		}

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
}
