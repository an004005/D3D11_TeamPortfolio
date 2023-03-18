#include "stdafx.h"
#include "..\public\Canvas.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "FSMComponent.h"
#include "UI_Manager.h"
#include "GameUtils.h"
#include "Player.h"

CCanvas::CCanvas(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CCanvas::CCanvas(const CCanvas& rhs)
	: CUI(rhs)
{
}

HRESULT CCanvas::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CCanvas::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_fSizeX = _float(g_iWinSizeX);
	m_fSizeY = _float(g_iWinSizeY);

	m_bVisible = false;

	//FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("Prototype_Component_LocalController"), TEXT("Com_Controller"), (CComponent**)&m_pController));

	return S_OK;
}

void CCanvas::BeginTick()
{
	CLayer* pLayer = CGameInstance::GetInstance()->GetLayer(LEVEL_NOW, L"Layer_Player");

	if (pLayer != nullptr)
	{
		list<CGameObject*> plsGameObject = pLayer->GetGameObjects();

		for (auto iter : plsGameObject)
		{
			if (iter->GetPrototypeTag() == L"Player")
			{
				m_pPlayer = dynamic_cast<CPlayer*>(iter);
				Assert(m_pPlayer != nullptr);
				break;
			}
		}

		m_Timeline0.SetCurve("UI_CanvasHit_0");
		m_Timeline1.SetCurve("UI_CanvasHit_1");
	}
}

void CCanvas::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_pPlayer != nullptr)
	{
		if (true == m_pPlayer->Get_Dash())
			Set_UIMove();
	}
	

	m_fSizeX = (_float)g_iWinSizeX;
	m_fSizeY = (_float)g_iWinSizeY;

	const _float2 PivotPair = GetPivotXY(m_ePivot);

	// 자식들의 캔버스 최신화
	const CanvasRect ThisCanvasSize{
		m_fX + PivotPair.x,
		m_fY + PivotPair.y,
		m_fSizeX * 0.5f,
		m_fSizeY * 0.5f
	};
	
	for (auto& Pair : m_mapChildUIs)
		Pair.second->SetCanvasSize(ThisCanvasSize);
	
	// map 으로 보관하고 있는 캔버스의 Tick() 을 돌린다.
	for (auto iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end();)
	{
		const auto ChildPair = *iter;

		if (ChildPair.second->IsDeleted())
		{
			ChildPair.second->Release();
			iter = m_mapChildUIs.erase(iter);
		}
		else
		{
			ChildPair.second->Tick(TimeDelta);
			++iter;
		}
	}
}

void CCanvas::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	vector<CUI*> tmpUI;
	tmpUI.reserve(m_mapChildUIs.size());

	for (const auto& Pair : m_mapChildUIs)
		tmpUI.push_back(Pair.second);

	std::sort(tmpUI.begin(), tmpUI.end(), [](const CUI* left, const CUI* right)
	{
		return left->Get_Priority() < right->Get_Priority();
	});

	for (auto pUI : tmpUI)
		pUI->Late_Tick(TimeDelta);
}

void CCanvas::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	m_Timeline1.Imgui_RenderEditor();

	ImGui::Separator();
	static char szChildProtoName[MAX_PATH]{};
	ImGui::InputText("ChildProtoTag", szChildProtoName, MAX_PATH);
	static char szChildName[MAX_PATH]{};
	ImGui::InputText("ChildName", szChildName, MAX_PATH);

	if (ImGui::Button("Add Empty UI"))	// 캔버스 안 에서 유아이를 생성한다.
	{
		wstring childTag = s2ws(szChildName);
		wstring childPrototypeTag = s2ws(szChildProtoName);

		if (Find_ChildUI(childTag.c_str()) == nullptr && childTag.empty() == false && childPrototypeTag.empty() == false)
			Add_ChildUI(LEVEL_NOW, childPrototypeTag.c_str(), childTag.c_str());
	}

	if (ImGui::BeginListBox("UI List"))	// 캔버스 안 에서 생성한 유아이를 확인한다.
	{
		for (auto& UI : m_mapChildUIs)
		{
			const bool bSelected = (UI.second == m_pUI);

			if (bSelected)
				ImGui::SetItemDefaultFocus();

			char pStr[64];
			wc2c(UI.first.c_str(), pStr);

			if (ImGui::Selectable(pStr, bSelected))
			{
				m_pUI = UI.second;
			}
		}

		ImGui::EndListBox();
	}

	ImGui::BeginChild("ChildUIProperty", { 500.f, 500.f }, true);	// 유아이의 정보를 확인 및 수정 가능하다.
	if (m_pUI)
	{
		ImGui::Separator();
		ImGui::Text("%s", typeid(*m_pUI).name());
		m_pUI->Imgui_RenderProperty();
	}
	ImGui::EndChild();

	ImGui::Separator();
}

void CCanvas::SaveToJson(Json & json)
{
	__super::SaveToJson(json);
	json["ChildrenUI"] = Json::array();

	for (auto pChild : m_mapChildUIs)
	{
		Json jsonChild;
		jsonChild["ChildTag"] = ws2s(pChild.first);
		pChild.second->SaveToJson(jsonChild);
		json["ChildrenUI"].push_back(jsonChild);
	}
}

void CCanvas::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);

	for (auto& childJson : json["ChildrenUI"])
	{
		string protoTag = childJson["Prototype_GameObject"];
		string childTag = childJson["ChildTag"];
		auto pUI = Add_ChildUI(LEVEL_NOW, s2ws(protoTag).c_str(),s2ws(childTag).c_str(), (void*)&childJson);
		Assert(pUI != nullptr);
	}
}

CUI * CCanvas::Find_ChildUI(const _tchar * pChildTag)
{
	auto	iter = m_mapChildUIs.find(pChildTag);
	if (iter == m_mapChildUIs.end())
		return nullptr;

	return iter->second;
}

CUI * CCanvas::Add_ChildUI(_uint iLevelIndex, const _tchar * pPrototypeTag, const _tchar * pChildTag, void * pArg)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	CGameObject * pGameObject = pGameInstance->Find_Prototype(LEVEL_NOW, pPrototypeTag);
	if (nullptr == pGameObject)
	{
		MSG_BOX("Not Found");
		return nullptr;
	}

	if (Find_ChildUI(pChildTag))
	{
		IM_WARN("Child Tag is Duplicated");
		return nullptr;
	}

 	CUI* pChildUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayer(iLevelIndex, pPrototypeTag, pArg));
	Assert(pChildUI != nullptr);

	m_mapChildUIs.emplace(pChildTag, pChildUI);

	return pChildUI;
}

void CCanvas::UIMove_FSM()
{
	m_pUIMoveFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
		.AddTransition("Idle to Move", "Move")
		.Predicator([this] {
		return m_bUIMove;
	})
		.AddState("Move")
		.OnStart([this]
	{
		m_fX = 0.0f;
		m_fY = 0.0f; 
		
		map<wstring, CCanvas*>& canvases = CUI_Manager::GetInstance()->Get_MoveCanvas();

		for_each(canvases.begin(), canvases.end(), [&](pair<wstring, CCanvas*> pCanvas) {
			pCanvas.second->Set_UIMove();
		});

		static _int iRandomCount;
		if (6 == iRandomCount)
			iRandomCount = 0;
		if (0 == iRandomCount)
			m_fRandomDestination = CGameUtils::GetRandFloat(0.0f, 3.0f);
		++iRandomCount;

		m_vDestination.x = m_vMaxDestination.x - m_fRandomDestination;
		m_vDestination.y = m_vMaxDestination.y - m_fRandomDestination;

	})
		.Tick([this](_double TimeDelta) {

		_vector vPosition = XMVectorSet(m_fX, m_fY, 0.0f, 1.0f);
		_vector vDestination = { m_vDestination.x, m_vDestination.y, 0.0f, 1.0f };
		_vector vDistance = vDestination - vPosition;

		vPosition += XMVector2Normalize(vDistance) * _float(TimeDelta) * 15.0f;
		m_fX = XMVectorGetX(vPosition);
		m_fY = XMVectorGetY(vPosition);

		// 목표 지점과 현재 지점을 비교한다.
		_float fDistance = XMVectorGetX(XMVector4Length(vDestination - vPosition));

		//IM_LOG("X %f", fDistance);
		if (0.3f > fDistance)
		{
			IM_LOG("X ---------------------%d");
			m_bIsDestination = true;
		}
	})
		.AddTransition("Move to Return", "Return")
		.Predicator([this] {
		return m_bIsDestination;
	})
		.AddState("Return")
		.Tick([this](_double TimeDelta) {

		_vector vPosition = XMVectorSet(m_fX, m_fY, 0.0f, 1.0f);
		_vector vDestination = { 0.0f, 0.0f, 0.0f, 1.0f };
		_vector vDistance = vDestination - vPosition;

		vPosition += XMVector2Normalize(vDistance) * _float(TimeDelta) * 15.0f;
		m_fX = XMVectorGetX(vPosition);
		m_fY = XMVectorGetY(vPosition);

		// 원래 지점과 현재 지점을 비교한다.
		_float fDistance = XMVectorGetX(XMVector2Length(vDestination - vPosition));

		//IM_LOG("Y %f", fDistance);
		if (0.3f > fDistance)
		{
			IM_LOG("Y --------------------%d");
			m_bIsDestination = false;
			m_bUIMove = false;
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		}
	})
		.AddTransition("Return to Idle", "Idle")
		.Predicator([this] {
		return m_bUIMove == false;
	})
		.Build();
}

void CCanvas::UIHit(const _double & TimeDelta)
{
	m_Timeline0.Tick(TimeDelta, m_fY);
	//m_Timeline1.Tick(TimeDelta, m_fX);

	if (m_pPlayer == nullptr)
		return;

	if (true == m_pPlayer->Get_Hit() || CGameInstance::GetInstance()->KeyDown(DIK_0))
	{	
		m_Timeline0.PlayFromStart();
		//_int iRndomNum = static_cast<_int>(CGameUtils::GetRandFloat(0.0f, 2.0f));

		//if (0 == iRndomNum)
		//{
		//	m_Timeline0.PlayFromStart();
		//}
		//else
		//{
		//	m_Timeline1.PlayFromStart();
		//}
	}

}

void CCanvas::TempOff(const _bool & bOff)
{
	CUI::TempOff(bOff);

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->TempOff(bOff);
}

CCanvas * CCanvas::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas*		pInstance = new CCanvas(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDefaultUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CCanvas::Clone(void * pArg)
{
	CCanvas*		pInstance = new CCanvas(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDefaultUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas::Free()
{
	__super::Free();

	Safe_Release(m_pUIMoveFSM);

	for (auto& Pair : m_mapChildUIs)
		Safe_Release(Pair.second);
	m_mapChildUIs.clear();

}
