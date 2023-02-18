#include "stdafx.h"
#include "Imgui_MapEditor.h"
#include "GameUtils.h"
#include "ImguiUtils.h"
#include "Layer.h"
#include "JsonLib.h"
#include "Floors.h"
#include "Indicator.h"
#include "Navigation.h"
#include "Cell.h"
#include "MathUtils.h"

CImgui_MapEditor::CImgui_MapEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImguiObject(pDevice, pContext)
{
}

HRESULT CImgui_MapEditor::Initialize(void* pArg)
{
	m_szTabname = "MapEditor";
	m_szWindowName = "CloneEditor";
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

	return S_OK;
}

void CImgui_MapEditor::Imgui_RenderTab()
{
#ifdef _DEBUG

	if (m_pFloor == nullptr)
	{
		m_pFloor = dynamic_cast<CFloors*>(m_pGameInstance->Clone_GameObject_Get(
			L"Layer_Utils", L"Prototype_GameObject_Floors"));
	}
	if (m_pNavigation == nullptr)
	{
		m_pNavigation = dynamic_cast<CNavigation*>(m_pGameInstance->Clone_GameObject_Get(
			L"Layer_Utils", L"Prototype_GameObject_Navigation"));
		m_Indicators[0] = dynamic_cast<CIndicator*>(m_pGameInstance->Clone_GameObject_Get(L"Layer_Utils", L"Prototype_GameObject_Indicator"));
		m_Indicators[1] = dynamic_cast<CIndicator*>(m_pGameInstance->Clone_GameObject_Get(L"Layer_Utils", L"Prototype_GameObject_Indicator"));
		m_Indicators[2] = dynamic_cast<CIndicator*>(m_pGameInstance->Clone_GameObject_Get(L"Layer_Utils", L"Prototype_GameObject_Indicator"));
	}

	if (m_pFloor == nullptr || m_pNavigation == nullptr || m_pColls == nullptr)
		return;

	CImguiUtils::FileDialog_FileSelector("Save Navigation", ".json", "../Bin/Resources/Level/Navi/", 
		[this](const string& filePath)
	{
		Json json;
		m_pNavigation->SaveToJson(json);
		std::ofstream file(filePath);
		file << json;
	});

	ImGui::SameLine();
	CImguiUtils::FileDialog_FileSelector("Load Navigation", ".json", "../Bin/Resources/Level/Navi/", 
		[this](const string& filePath)
	{
		std::ifstream ifs(filePath);
		Json json = Json::parse(ifs);
		m_pNavigation->LoadJson(json);
	});

	CImguiUtils::FileDialog_FileSelector("Save Floors", ".json", "../Bin/Resources/Level/Floor/", 
		[this](const string& filePath)
	{
		Json json;
		m_pFloor->SaveToJson(json);
		std::ofstream file(filePath);
		file << json;
	});
	ImGui::SameLine();
	CImguiUtils::FileDialog_FileSelector("Load Floors", ".json", "../Bin/Resources/Level/Floor/", 
		[this](const string& filePath)
	{
		std::ifstream ifs(filePath);
		Json json = Json::parse(ifs);
		m_pFloor->LoadJson(json);
	});

	ImGui::Separator();

	if (ImGui::Button("Navigation Quick Save"))
	{
		Json json;
		m_pNavigation->SaveToJson(json);
		std::ofstream file("../Bin/Resources/Level/Navi/navi.json");
		file << json;
	}
	if (ImGui::Button("Floor Quick Save"))
	{
		Json json;
		m_pFloor->SaveToJson(json);
		std::ofstream file("../Bin/Resources/Level/Floor/floors.json");
		file << json;
	}

	ImGui::Separator();

	ImGui::RadioButton("Floor", (int*)&m_eMode, FLOOR_MODE);
	ImGui::RadioButton("CellCreate", (int*)&m_eMode, CELL_CREATE);
	ImGui::RadioButton("CellSelect", (int*)&m_eMode, CELL_SELECT);
	ImGui::RadioButton("CellPointFix", (int*)&m_eMode, CELL_POINT_FIX);
	ImGui::RadioButton("Collider", (int*)&m_eMode, COLLIDER);

	if (m_eMode != CELL_CREATE)
	{
		m_CellPointsStore.clear();
		m_Indicators[0]->SetVisible(false);
		m_Indicators[1]->SetVisible(false);
		m_Indicators[2]->SetVisible(false);
	}
	if (m_eMode != CELL_SELECT)
	{
		m_pSelectCell = nullptr;
	}
	if (m_eMode != CELL_POINT_FIX)
	{
		m_CellPointPairs.clear();
	}
	ImGui::Separator();



	ImGui::Separator();

	switch (m_eMode)
	{
	case FLOOR_MODE:
		m_pFloor->Imgui_RenderProperty();
		break;
	case CELL_CREATE:
		if (ImGui::Button("Delete Recent Cell"))
		{
			m_pNavigation->DeleteRecentCell();
		}

		if (m_CellPointsStore.size() == 3)
		{
			if (Adjust_CellPoints())
			{
				m_pNavigation->AddCell({m_CellPointsStore[0], m_CellPointsStore[1], m_CellPointsStore[2]});
			}

			m_CellPointsStore.clear();
			m_Indicators[0]->SetVisible(false);
			m_Indicators[1]->SetVisible(false);
			m_Indicators[2]->SetVisible(false);
		}
		else
		{
			if (m_pGameInstance->KeyDown(CInput_Device::DIM_LB))
			{
				_float3 vPos;
				if (m_pNavigation->FindNearCellPoint(vPos) 
					|| m_pFloor->PickFloorGetPos(vPos))
				{
					m_Indicators[m_CellPointsStore.size()]->SetVisible(true);
					m_Indicators[m_CellPointsStore.size()]->SetPosition(vPos);
					m_CellPointsStore.push_back(vPos);
				}
			}

			if (m_pGameInstance->KeyDown(CInput_Device::DIM_RB) && !m_CellPointsStore.empty())
			{
				m_CellPointsStore.pop_back();
				m_Indicators[m_CellPointsStore.size()]->SetVisible(false);
			}
		}
		break;
	case CELL_SELECT:
		if (m_pGameInstance->KeyDown(CInput_Device::DIM_LB))
		{
			m_pSelectCell = m_pNavigation->PickCell();
		}

		if (m_pSelectCell)
		{
			m_pNavigation->SetCheckCell(m_pSelectCell->GetIdx());
			m_pSelectCell->Imgui_Render();
			if (ImGui::Button("Delete Cell"))
			{
				m_pNavigation->DeleteCell(m_pSelectCell->GetIdx());
				m_pSelectCell = nullptr;
			}
		}

		break;
	case CELL_POINT_FIX:
		if (m_pGameInstance->KeyDown(CInput_Device::DIM_LB))
		{
			m_CellPointPairs.clear();
			m_pNavigation->GetAllNearCellPoints(m_CellPointPairs);
		}

		if (!m_CellPointPairs.empty())
		{
			static GUIZMO_INFO tInfo;

			_float3 vPosOrigin = m_CellPointPairs.back().first->Get_Point(m_CellPointPairs.back().second);
			_float3 vPosNew = vPosOrigin;
			CImguiUtils::Render_Guizmo_Translate(vPosNew, tInfo, true, true);

			if (vPosNew != vPosOrigin)
			{
				for (auto& CellPointPair : m_CellPointPairs)
				{
					CellPointPair.first->Set_Point(CellPointPair.second, vPosNew);
				}
			}
		}

		break;
	case PROPERTY_VIEW:
		m_pGameInstance->Imgui_ObjectViewer(LEVEL_NOW,m_pSelectedObject);
		if (m_pSelectedObject)
		{
			ImGui::Separator();
			ImGui::Text("%s", typeid(*m_pSelectedObject).name());
			m_pSelectedObject->Imgui_RenderProperty();
			m_pSelectedObject->Imgui_RenderComponentProperties();
		}
		break;
	default:
		NODEFAULT
	}

	ImGui::Separator();

	m_pGameInstance->Imgui_ObjectViewer(LEVEL_NOW, m_pSelectedObject);
	
	if (m_pSelectedObject)
	{
		ImGui::Separator();
		ImGui::Text("%s", typeid(*m_pSelectedObject).name());
		m_pSelectedObject->Imgui_RenderProperty();
		m_pSelectedObject->Imgui_RenderComponentProperties();
	}

#endif
}

void CImgui_MapEditor::Imgui_RenderWindow()
{
	// todo  피킹 기능, 한번 만 생성 버튼, 계속 누르는 대로 생성하는 버튼 옵션 추가
	ImGui::Text("ProtoViewer");
	m_pGameInstance->Imgui_ProtoViewer(m_pSelectedProto);
	ImGui::Separator();

	ImGui::Text("Layer Selector");
	ImGui::InputText("Selected Layer", m_szLayerName, MAX_PATH);

	m_pLayerName = nullptr;
	m_pGameInstance->Imgui_LayerCombo(LEVEL_NOW, m_pLayerName);
	if (m_pLayerName)
		CGameUtils::wc2c(m_pLayerName, m_szLayerName);

	// ImGui::IsWindowAppearing()

	// if (m_pSelectedTerrain && m_pSelectedProto && m_pGameInstance->KeyUp(CInput_Device::DIM_LB))
	// {
	// 	_float4 vPickPos;
	// 	if (m_pSelectedTerrain->PickTerrain(vPickPos))
	// 	{
	// 		_tchar szLayerName[256];
	// 		CGameUtils::c2wc(m_szLayerName, szLayerName);
	//
	// 		m_pGameInstance->Clone_GameObject(szLayerName, m_pSelectedProto);
	// 		if (CGameObject* pObj = m_pGameInstance->GetLastClonedObject())
	// 		{
	// 			 pObj->GetComponentChecked<CTransform>(CGameObject::m_pTransformComTag)
	// 				->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPickPos));
	// 		}
	//
	// 		m_pSelectedProto = nullptr;
	// 	}
	// }

	if (ImGui::Button("Clone At Origin"))
	{
		if (m_pSelectedProto)
		{
			_tchar szLayerName[256];
			CGameUtils::c2wc(m_szLayerName, szLayerName);
			m_pSelectedObject = m_pGameInstance->Clone_GameObject_Get(szLayerName, m_pSelectedProto);
	
			m_pSelectedProto = nullptr;
		}
	}

	ImGui::Text("Except Layers : Layer_Terrain, Layer_Camera");
	CImguiUtils::FileDialog_FileSelector("Save Current Level Layers", ".json", "..\\Bin\\Resources\\Level\\",
	                                     [this](const string& FilePath)
	                                     {
		                                     SaveLevel(FilePath);
	                                     });

	CImguiUtils::FileDialog_FileSelector("Load Level Layers", ".json", "..\\Bin\\Resources\\Level\\",
	                                     [this](const string& FilePath)
	                                     {
		                                     LoadLevel(FilePath);
	                                     });
}

CImgui_MapEditor* CImgui_MapEditor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	auto inst = new CImgui_MapEditor(pDevice, pContext);
	if (FAILED(inst->Initialize(pArg)))
	{
		return nullptr;
	}

	return inst;
}

void CImgui_MapEditor::SaveLevel(const string& strPath)
{
	Json Layers = Json::object();

	for (auto LayerPair : CGameInstance::GetInstance()->GetLayers(LEVEL_NOW))
	{
		if (0 == lstrcmp(LayerPair.first, L"Layer_Terrain")) continue;
		if (0 == lstrcmp(LayerPair.first, L"Layer_Camera")) continue;
		Json jsonLayer = Json::array();

		for (auto pObj : LayerPair.second->GetGameObjects())
		{
			Json jsonObj = Json::object();
			pObj->SaveToJson(jsonObj);
			jsonLayer.push_back(jsonObj);
		}

		char szLayerTag[256];
		CGameUtils::wc2c(LayerPair.first, szLayerTag);
		Layers[szLayerTag] = jsonLayer;
	}

	std::ofstream file(strPath);
	file << Layers;
}

void CImgui_MapEditor::LoadLevel(const string& strPath)
{
	std::ifstream ifs(strPath);
	Json Layers = Json::parse(ifs);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	for (auto itr = Layers.begin(); itr != Layers.end(); ++itr)
	{
		auto LayerName = itr.key();
		auto LayerArr = itr.value();
		assert(LayerArr.is_array());

		_tchar szLayerName[TAG_LEN];
		CGameUtils::c2wc(LayerName.c_str(), szLayerName);

		for (auto& Object : LayerArr)
		{
			assert(Object.is_structured());
			auto PrototypeTag = Object["Prototype_GameObject"].get<string>();
			_tchar szPrototypeTag[TAG_LEN];
			CGameUtils::c2wc(PrototypeTag.c_str(), szPrototypeTag);

			pGameInstance->Clone_GameObject(szLayerName, szPrototypeTag, &Object);
		}
	}
}


_bool CImgui_MapEditor::Adjust_CellPoints()
{
	Assert(m_CellPointsStore.size() == 3);

   //함수가 실행됐다는 건 배열이 다 찼다는 의미.
   const _vector vPointA = XMLoadFloat3(&m_CellPointsStore[0]);
   const _vector vPointB = XMLoadFloat3(&m_CellPointsStore[1]);
   const _vector vPointC = XMLoadFloat3(&m_CellPointsStore[2]);

   const _vector vAB = vPointB - vPointA;
   const _vector vBC = vPointC - vPointB;

   const _float3 vCross = XMVector3Cross(vAB, vBC);

	// 수직 안되
   if (CMathUtils::FloatCmp(vCross.y, 0.f))
	   return false;

	// 반시계면 변경
	if (vCross.y < 0)
	{
		_float3 tmp = m_CellPointsStore[1];
		m_CellPointsStore[1] = m_CellPointsStore[2];
		m_CellPointsStore[2] = tmp;
	}

   return true;
}

void CImgui_MapEditor::Free()
{
	CImguiObject::Free();
	Safe_Release(m_pGameInstance);
	// Safe_Release(m_pFloor); // Clone_Get으로는 safe_add 안됨, release하지 않는다.
	// Safe_Release(m_pNavigation);
}

