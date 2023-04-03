#include "stdafx.h"
#include "..\public\Imgui_Batch.h"
#include "ImguiUtils.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "GameInstance.h"
#include "PhysX_Manager.h"
#include "GameObject.h"

namespace nlohmann
{
	template <>
	struct adl_serializer<BATCH_PRESET>
	{
		static void to_json(json& j, const BATCH_PRESET& value)
		{
			j["Layer"] = value.strLayer;
			j["JsonPath"] = value.strJsonPath;
			j["ProtoTag"] = value.strProtoTag;
		}

		static void from_json(const json& j, BATCH_PRESET& value)
		{
			value.strLayer = j["Layer"];
			value.strJsonPath = j["JsonPath"];
			value.strProtoTag = j["ProtoTag"];
		}
	};
}

CImgui_Batch::CImgui_Batch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImguiObject(pDevice, pContext)
{
}

HRESULT CImgui_Batch::Initialize(void* pArg)
{
	m_szTabname = "BatchTool";

	m_BatchPresets.clear();

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Batch/BatchPresets.json");
	for (auto batchPreset : json["Presets"])
	{
		BATCH_PRESET tPreset = batchPreset;
		m_BatchPresets.push_back(tPreset);
	}

	return S_OK;
}

void CImgui_Batch::Imgui_RenderTab()
{
	static size_t iSelectedPresetIdx = 0;
	static _bool bClickClone = false;
	static _bool bClockSelect = false;

	if (ImGui::BeginListBox("Batch Presets"))
	{
		for (size_t i = 0; i < m_BatchPresets.size(); ++i)
		{
			const bool bSelected = iSelectedPresetIdx == i;
			if (bSelected)
				ImGui::SetItemDefaultFocus();

			string strViewName;
			if (m_BatchPresets[i].strJsonPath.empty())
			{
				strViewName = m_BatchPresets[i].strLayer + "/" + m_BatchPresets[i].strProtoTag;
			}
			else
			{
				string jsonName = CGameUtils::GetFileName(m_BatchPresets[i].strJsonPath);
				strViewName = m_BatchPresets[i].strLayer + "/" + jsonName + "/" + m_BatchPresets[i].strProtoTag;
			}
			
			if (ImGui::Selectable(strViewName.c_str(), bSelected))
				iSelectedPresetIdx = i;
		}
		ImGui::EndListBox();
	}
	if (iSelectedPresetIdx < m_BatchPresets.size())
	{
		ImGui::Text("Select Preset ProtoTag : %s", m_BatchPresets[iSelectedPresetIdx].strProtoTag.c_str());
		ImGui::Text("Select Preset Layer : %s", m_BatchPresets[iSelectedPresetIdx].strLayer.c_str());
		if (m_BatchPresets[iSelectedPresetIdx].strJsonPath.empty())
			ImGui::Text("Preset Json is Empty.");
		else
			ImGui::Text("Preset Json Path : %s", m_BatchPresets[iSelectedPresetIdx].strJsonPath.c_str());
	}
	// else
	// {
	// 	static char szProtoTag[MAX_PATH]{};
	// 	static char szLayerTag[MAX_PATH]{};
	// 	static char szJsonPath[MAX_PATH]{};
	// 	ImGui::InputText("Custom ProtoTag", szProtoTag, MAX_PATH);
	// 	ImGui::InputText("Custom LayerTag", szLayerTag, MAX_PATH);
	// 	ImGui::InputText("Custom JsonPath", szJsonPath, MAX_PATH);
	// }

	ImGui::Checkbox("Click Clone Once", &bClickClone);
	ImGui::Checkbox("Click Select Obj(only this batch)", &bClockSelect);

	if (CGameInstance::GetInstance()->KeyDown(CInput_Device::DIM_LB))
	{
		_float4 vPos;
		if (bClickClone && CGameUtils::GetPosFromRayCast(vPos))
		{
			CloneBatchPreset(vPos, iSelectedPresetIdx);
			bClickClone = false;
		}

		CGameObject* pObj = nullptr;
		if (bClockSelect && GetObjFromRayCast(pObj, CTB_PSYCHICK_OBJ))
		{
			for (auto& batchObj : m_BatchObjList)
			{
				if (pObj == batchObj.pObj)
				{
					m_SelectedBatchObj = batchObj;
					bClockSelect = false;
					break;
				}
			}
		}
	}


	if (ImGui::BeginListBox("Batched Objects"))
	{
		for (auto& BatchObj : m_BatchObjList)
		{
			const bool bSelected = m_SelectedBatchObj.pObj == BatchObj.pObj;
			if (bSelected)
				ImGui::SetItemDefaultFocus();

			char szAddressName[256];
			sprintf_s(szAddressName, "%s[%p]", BatchObj.tPreset.strProtoTag.c_str(), BatchObj.pObj);

			if (ImGui::Selectable(szAddressName, bSelected))
				m_SelectedBatchObj = BatchObj;
		}
		ImGui::EndListBox();
	}

	if (m_SelectedBatchObj.pObj && CGameInstance::GetInstance()->Check_ObjectAlive(m_SelectedBatchObj.pObj) == false)
	{
		m_SelectedBatchObj.pObj = nullptr;
	}

	if (m_SelectedBatchObj.pObj)
	{
		ImGui::Text("Select BatchObject ProtoTag : %s", m_SelectedBatchObj.tPreset.strProtoTag.c_str());
		ImGui::Text("Select BatchObject Layer : %s", m_SelectedBatchObj.tPreset.strLayer.c_str());
		if (m_SelectedBatchObj.tPreset.strJsonPath.empty())
			ImGui::Text("BatchObject Json is Empty.");
		else
			ImGui::Text("BatchObject Json Path : %s", m_SelectedBatchObj.tPreset.strJsonPath.c_str());

		if (ImGui::Button("Delete"))
		{
			m_BatchObjList.remove_if([this](const BATCH_OBJ& batchObj)
			{
				return batchObj.pObj == m_SelectedBatchObj.pObj;
			});
			m_SelectedBatchObj.pObj->SetDelete();
			m_SelectedBatchObj.pObj = nullptr;
		}

		if (m_SelectedBatchObj.pObj && ImGui::CollapsingHeader("Select Object Properties"))
		{
			ImGui::Indent(20.f);
			m_SelectedBatchObj.pObj->Imgui_RenderProperty();
			m_SelectedBatchObj.pObj->Imgui_RenderComponentProperties();
			ImGui::Unindent(20.f);
		}
	}

	SaveBatchFile();
	LoadBatchFile();
}

void CImgui_Batch::LoadBatchFile()
{
	CImguiUtils::FileDialog_FileSelector("Select BatchFile to Load", ".json", "../Bin/Resources/Batch/BatchFiles/", [this](const string& filePath)
	{
		for (auto batchObj : m_BatchObjList)
			batchObj.pObj->SetDelete();
		m_BatchObjList.clear();

		std::ifstream ifs(filePath);
		Json json = Json::parse(ifs);

		for (auto BatchObjJson : json["BatchObjs"])
		{
			BATCH_OBJ tBatchObj;
			BATCH_PRESET tPreset = BatchObjJson["Preset"];
			wstring wstrLayer = s2ws(tPreset.strLayer);
			wstring wstrProtoTag = s2ws(tPreset.strProtoTag);
			_float4x4 WorldMatrix = BatchObjJson["WorldMatrix"];

			if (tPreset.strJsonPath.empty() == false)
			{
				Json jsonObj = CJsonStorage::GetInstance()->FindOrLoadJson(tPreset.strJsonPath);
				auto pObj = CGameInstance::GetInstance()->Clone_GameObject_Get(wstrLayer.c_str(), wstrProtoTag.c_str(), &jsonObj);
				pObj->GetTransform()->Set_WorldMatrix(WorldMatrix);
				tBatchObj.pObj = pObj;
			}
			else
			{
				Json jsonObj = BatchObjJson["JsonData"];
				auto pObj = CGameInstance::GetInstance()->Clone_GameObject_Get(wstrLayer.c_str(), wstrProtoTag.c_str(), &jsonObj);
				pObj->GetTransform()->Set_WorldMatrix(WorldMatrix);
				tBatchObj.pObj = pObj;
			}
			tBatchObj.tPreset = tPreset;
			m_BatchObjList.push_back(tBatchObj);
		}
	});
}

void CImgui_Batch::SaveBatchFile()
{
	CImguiUtils::FileDialog_FileSelector("Select BatchFile to Save", ".json", "../Bin/Resources/Batch/BatchFiles/", [this](const string& filePath)
	{
		Json json;
		json["BatchObjs"] = Json::array();
		for (auto& BatchObj : m_BatchObjList)
		{
			Json BatchObjJson;
			BatchObjJson["Preset"] = BatchObj.tPreset;
			BatchObjJson["WorldMatrix"] = BatchObj.pObj->GetTransform()->Get_WorldMatrix_f4x4();
			if (BatchObj.tPreset.strJsonPath.empty())
			{
				BatchObjJson["JsonData"] = Json::object();
				BatchObj.pObj->SaveToJson(BatchObjJson["JsonData"]);
			}
			json["BatchObjs"].push_back(BatchObjJson);
		}

		std::ofstream file(filePath);
		file << json;
	});
}

void CImgui_Batch::RunBatchFile(const string& strBatchFilePath)
{
	std::ifstream ifs(strBatchFilePath);
	Json json = Json::parse(ifs);

	for (auto BatchObjJson : json["BatchObjs"])
	{
		BATCH_PRESET tPreset = BatchObjJson["Preset"];
		wstring wstrLayer = s2ws(tPreset.strLayer);
		wstring wstrProtoTag = s2ws(tPreset.strProtoTag);
		_float4x4 WorldMatrix = BatchObjJson["WorldMatrix"];

		if (tPreset.strJsonPath.empty() == false)
		{
			Json jsonObj = CJsonStorage::GetInstance()->FindOrLoadJson(tPreset.strJsonPath);
			auto pObj = CGameInstance::GetInstance()->Clone_GameObject_Get(wstrLayer.c_str(), wstrProtoTag.c_str(), &jsonObj);
			pObj->GetTransform()->Set_WorldMatrix(WorldMatrix);
		}
		else
		{
			Json jsonObj = BatchObjJson["JsonData"];
			auto pObj = CGameInstance::GetInstance()->Clone_GameObject_Get(wstrLayer.c_str(), wstrProtoTag.c_str(), &jsonObj);
			pObj->GetTransform()->Set_WorldMatrix(WorldMatrix);
		}
	}
}

_bool CImgui_Batch::GetObjFromRayCast(CGameObject*& pObj, _uint iTargetType)
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
	params.fDistance = 3000.f;
	params.iTargetType = iTargetType;
	params.bSingle = true;

	if (CGameInstance::GetInstance()->RayCast(params))
	{
		for (int i = 0; i < t.getNbAnyHits(); ++i)
		{
			auto p = t.getAnyHit(i);
			if (auto pOwner = CPhysXUtils::GetOnwer(p.actor))
			{
				pObj = pOwner;
				return true;
			}
		}
	}

	return false;
}

void CImgui_Batch::CloneBatchPreset(_float4 vPos, size_t iPresetIdx)
{
	wstring wstrLayer = s2ws(m_BatchPresets[iPresetIdx].strLayer);
	string strJsonPath = m_BatchPresets[iPresetIdx].strJsonPath;
	wstring wstrProtoTag = s2ws(m_BatchPresets[iPresetIdx].strProtoTag);
	auto pProtoType = CGameInstance::GetInstance()->Find_Prototype(LEVEL_NOW, wstrProtoTag.c_str());
	if (pProtoType == nullptr)
		pProtoType = CGameInstance::GetInstance()->Find_Prototype(LEVEL_STATIC, wstrProtoTag.c_str());

	if (pProtoType == nullptr)
	{
		MSG_BOX("ProtoType is not exist");
	}
	else if (wstrLayer.empty())
	{
		MSG_BOX("Layer is empty");
	}
	else if (strJsonPath.empty() == false && CGameUtils::FileExist(s2ws(strJsonPath).c_str()) == false)
	{
		MSG_BOX("Json file is not exist");
	}
	else
	{
		BATCH_OBJ tBatchObj;
		tBatchObj.tPreset = m_BatchPresets[iPresetIdx];

		if (strJsonPath.empty() == false)
		{
			Json json = CJsonStorage::GetInstance()->FindOrLoadJson(strJsonPath);
			tBatchObj.pObj = CGameInstance::GetInstance()->Clone_GameObject_Get(wstrLayer.c_str(), wstrProtoTag.c_str(), &json);
		}
		else
		{
			tBatchObj.pObj = CGameInstance::GetInstance()->Clone_GameObject_Get(wstrLayer.c_str(), wstrProtoTag.c_str());
		}
		tBatchObj.pObj->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, vPos);
		m_BatchObjList.push_back(tBatchObj);

		m_SelectedBatchObj.pObj = tBatchObj.pObj;
		m_SelectedBatchObj.tPreset = m_BatchPresets[iPresetIdx];
	}
}

CImgui_Batch* CImgui_Batch::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	auto inst = new CImgui_Batch(pDevice, pContext);
	if (FAILED(inst->Initialize(pArg)))
	{
		return nullptr;
	}

	return inst;
}

void CImgui_Batch::Free()
{
	CImguiObject::Free();
}
