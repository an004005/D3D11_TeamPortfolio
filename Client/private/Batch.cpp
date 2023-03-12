#include "stdafx.h"
#include "..\public\ScarletMap.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "Batch.h"
#include "Trigger.h"
#include "PhysX_Manager.h"
#include "ScarletCharacter.h"
#include "MainApp.h"

CBatch::CBatch(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CBatch::CBatch(const CBatch & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBatch::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CBatch::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components());

	return S_OK;
}

void CBatch::BeginTick()
{
}

void CBatch::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	RayPicking();
}

void CBatch::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CBatch::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

void CBatch::Imgui_RenderProperty()
{
	if (LEVEL_NOW != LEVEL_BATCH) return;

	__super::Imgui_RenderProperty();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	const char*	BatchTypes[BATCHTYPE::TYPE_END] = { "MONSTER", "OTEHR" };
	ImGui::Combo("ProtoFilter", &m_Filter, BatchTypes, IM_ARRAYSIZE(BatchTypes));

	ImGui::Separator();

	if (ImGui::BeginListBox("GameObjectProto List"))
	{
		for (auto& info : m_ProtosInfo)
		{
			//필터 적용
			if (m_Filter != info.second) continue;

			const bool bSelected = (info.first == m_pProtoTag);

			if (bSelected)
				ImGui::SetItemDefaultFocus();

			char pStr[MAX_PATH];
			strcpy(pStr, CGameUtils::GetFileName(ws2s(info.first)).c_str());

			if (ImGui::Selectable(pStr, bSelected))
			{
				m_pProtoTag = info.first;
			}
		}

		ImGui::EndListBox();
	}

	
	ImGui::Separator();

	if (ImGui::Checkbox("Picking On", &m_bPick));
	
	ImGui::Separator();
	if (ImGui::TreeNode("Trigger"))
	{
		if (ImGui::Button("Create_Trigger"))
		{
			CTrigger* pTrigger = nullptr;
			pTrigger = dynamic_cast<CTrigger*>(pGameInstance->Clone_GameObject_Get(TEXT("Layer_Trigger"), TEXT("Prototype_GameObject_Trigger")));

			assert(pTrigger != nullptr);
			m_pTriggers.emplace_back(pTrigger);
		}

		ImGui::SameLine();

		if (ImGui::Button("Delete_Trigger"))
		{
			if (m_pTrigger != nullptr)
			{
				m_pTriggers.erase(remove(m_pTriggers.begin(), m_pTriggers.end(), m_pTrigger), m_pTriggers.end());
				m_pTrigger->SetDelete();
				m_pTrigger = nullptr;
			}
		}


		ImGui::Separator();

		if (ImGui::BeginListBox("Trigger List"))
		{
			for (size_t i = 0; i < m_pTriggers.size(); ++i)
			{
				const bool bSelected = (m_pTriggers[i] == m_pTrigger);

				if (bSelected)
					ImGui::SetItemDefaultFocus();

				char pStr[MAX_PATH]{};
				sprintf_s(pStr, sizeof(pStr), "Trigger %zd", i);

				if (ImGui::Selectable(pStr, bSelected))
				{
					m_pTrigger = m_pTriggers[i];
				}

			}

			ImGui::EndListBox();
		}

		if (ImGui::Button("Set_Monster"))
		{
			if (m_pTrigger != nullptr && m_pGameObject != nullptr)
			{
				m_pTrigger->Set_ForCreate(m_pGameObject->GetPrototypeTag(), m_pGameObject->GetTransform()->Get_WorldMatrix());
			}
		}


		ImGui::Separator();

		ImGui::BeginChild("Selected Trigger", { 400.f, 250.f });

		if (m_pTrigger)
		{
			ImGui::Separator();
			ImGui::Text("%s", typeid(*m_pTrigger).name());
			m_pTrigger->Imgui_RenderProperty();
			m_pTrigger->Imgui_RenderComponentProperties();
		}

		ImGui::EndChild();

		ImGui::TreePop();
	}

	ImGui::Separator();

	if (ImGui::TreeNode("GameObject"))
	{
		/*	if (ImGui::Button("Create_Object"))
			{
				if (m_pProtoTag != L"")
				{
					CGameObject* pGameObject = nullptr;
					pGameObject = pGameInstance->Clone_GameObject_Get(TEXT("Layer_AssortedObj"), m_pProtoTag.c_str());

					assert(pGameObject != nullptr);
					m_pGameObjects.emplace_back(pGameObject);
				}

			}*/

		ImGui::Separator();


		if (ImGui::BeginListBox("GameObject List"))
		{
			for (size_t i = 0; i < m_pGameObjects.size(); ++i)
			{
				const bool bSelected = (m_pGameObjects[i] == m_pGameObject);

				if (bSelected)
					ImGui::SetItemDefaultFocus();

				char pStr[MAX_PATH]{};
				string str = ws2s(m_pGameObjects[i]->GetPrototypeTag());
				sprintf_s(pStr, sizeof(pStr), "%s %zd", str.c_str(), i);

				if (ImGui::Selectable(pStr, bSelected))
				{
					m_pGameObject = m_pGameObjects[i];
				}
			}

			ImGui::EndListBox();
		}

		ImGui::Separator();

		ImGui::BeginChild("Selected Object", { 400.f, 250.f });

		if (m_pGameObject)
		{
			ImGui::Separator();
			ImGui::Text("%s", typeid(*m_pGameObject).name());
			m_pGameObject->Imgui_RenderProperty();
			m_pGameObject->Imgui_RenderComponentProperties();
		}

		ImGui::EndChild();

		ImGui::TreePop();
	}

	


}

void CBatch::SaveToJson(OUT Json & json)
{
	__super::SaveToJson(json);

	json["Triggers"] = Json::array();

	for (auto trigger : m_pTriggers)
	{
		Json jsonTrigger;
		trigger->SaveToJson(jsonTrigger);
		json["Triggers"].push_back(jsonTrigger);
	}
}

void CBatch::LoadFromJson(const Json & json)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	__super::LoadFromJson(json);

	if (json.contains("ProtosInfo"))
	{
		for (auto pGameObj : json["ProtosInfo"])
		{
			pair<wstring, BATCHTYPE> objdesc = pGameObj;
			m_ProtosInfo.emplace_back(objdesc);
		}
	}

	if (json.contains("Triggers"))
	{
		for (auto trigger : json["Triggers"])
		{
			CTrigger* pTrigger = nullptr;
			pTrigger = dynamic_cast<CTrigger*>(pGameInstance->Clone_GameObject_Get(TEXT("Layer_Trigger"), TEXT("Prototype_GameObject_Trigger"), &trigger));

			assert(pTrigger != nullptr);
			m_pTriggers.emplace_back(pTrigger);
		}
	}
	
}

HRESULT CBatch::SetUp_Components()
{
	return S_OK;
}

_float4 CBatch::SetUp_InitPosition()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	_matrix CamWorldMatrix = pGameInstance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);

	_vector vCamLook = CamWorldMatrix.r[2];
	_vector vCamPos = CamWorldMatrix.r[3];

	_float4 vInitPos;
	XMStoreFloat4(&vInitPos, XMVectorSetW(vCamPos + XMVector3Normalize(vCamLook) * 20.f, 1.f));

	return vInitPos;
}

void CBatch::RayPicking()
{
	if (m_bPick == false) return;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (pGameInstance->KeyDown(CInput_Device::DIM_LB))
	{
		_float4 vOrigin;
		_float4 vDir;
		CGameUtils::GetPickingRay(vOrigin, vDir);

		physx::PxRaycastHit hitBuffer[1];
		physx::PxRaycastBuffer t(hitBuffer, 1);

		RayCastParams params;
		params.rayOut = &t;
		params.vOrigin = vOrigin;
		params.vDir = vDir;
		params.fDistance = 1000.f;
		params.iTargetType = CTB_STATIC;
		params.bSingle = true;

		if (pGameInstance->RayCast(params))
		{
			IM_LOG("Hit Ray");
			for (int i = 0; i < t.getNbAnyHits(); ++i)
			{
				auto p = t.getAnyHit(i);

				_float4 vPos{ p.position.x, p.position.y + 10.f, p.position.z, 1.f };

				if (m_pProtoTag != L"")
				{
					_float4x4 WorldMatrix = _float4x4::Identity;
					WorldMatrix.m[3][0] = vPos.x;
					WorldMatrix.m[3][1] = vPos.y;
					WorldMatrix.m[3][2] = vPos.z;

					Json jsonTransform;
					CTransform::ModifyTransformJson(jsonTransform, WorldMatrix);

					CGameObject* pGameObject = nullptr;
					pGameObject = pGameInstance->Clone_GameObject_Get(TEXT("Layer_AssortedObj"), m_pProtoTag.c_str(), &jsonTransform);

					//pGameObject->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
					//assert(pGameObject != nullptr);
					m_pGameObjects.emplace_back(pGameObject);

					return;
				}
			}
		}
	}
}


CBatch * CBatch::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBatch*		pInstance = new CBatch(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBatch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBatch::Clone(void* pArg)
{
	CBatch*		pInstance = new CBatch(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBatch");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBatch::Free()
{
	__super::Free();
}
