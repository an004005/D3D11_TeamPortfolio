#include "stdafx.h"
#include "..\public\UI_Manager.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "Canvas.h"

#include "DefaultUI.h"

#include "Canvas_PlayerInfo.h"

CUI_Manager::CUI_Manager(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Manager::CUI_Manager(const CUI_Manager & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Manager::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CUI_Manager::Initialize(void * pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUI_Manager::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CUI_Manager::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

}

HRESULT CUI_Manager::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CUI_Manager::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	//static char	szPrototypeTag[MAX_PATH];
	//ImGui::InputText("Prototype Tag", szPrototypeTag, sizeof(char[MAX_PATH]));

	if (ImGui::Button("Create Canvas"))
	{
		//_tchar pPrototypeTag[MAX_PATH];
		//CGameUtils::c2wc(szPrototypeTag, pPrototypeTag);
		//Create_Canvas();
	}
	ImGui::SameLine();
	if (ImGui::Button("Create UI"))
	{
		//_tchar pPrototypeTag[MAX_PATH];
		//CGameUtils::c2wc(szPrototypeTag, pPrototypeTag);
		Create_UI();
	}

	if (ImGui::BeginListBox("UI List"))
	{
		list<CUI*>::iterator iter = m_pUIs.begin();

		for (auto& UI : m_pUIs)
		{
			const bool bSelected = (UI == m_pUI);

			if (bSelected)
				ImGui::SetItemDefaultFocus();

			wstring wstrUITag = UI->GetPrototypeTag();

			char pStr[128];
			wc2c(wstrUITag.c_str(), pStr);
			
			_uint iCount = (_uint)count_if(m_pUIs.begin(), iter, [&](CUI* pUI) {
				return wstrUITag == pUI->GetPrototypeTag();
			});

			sprintf_s(pStr, sizeof(pStr), "%s %d", pStr, iCount);

			if (ImGui::Selectable(pStr, bSelected))
			{
				m_pUI = UI;
			}

			++iter;
		}

		ImGui::EndListBox();
	}

	if (ImGui::BeginListBox("Canvas List"))
	{
		list<CUI*>::iterator iter = m_pCanvases.begin();

		for (auto& Canvas : m_pCanvases)
		{
			const bool bSelected = (Canvas == m_pCanvas);

			if (bSelected)
				ImGui::SetItemDefaultFocus();

			wstring wstrUITag = Canvas->GetPrototypeTag();

			char pStr[128];
			wc2c(wstrUITag.c_str(), pStr);

			_uint iCount = (_uint)count_if(m_pCanvases.begin(), iter, [&](CUI* pUI) {
				return wstrUITag == pUI->GetPrototypeTag();
			});

			sprintf_s(pStr, sizeof(pStr), "%s %d", pStr, iCount);

			if (ImGui::Selectable(pStr, bSelected))
			{
				m_pCanvas = Canvas;
			}

			++iter;
		}

		ImGui::EndListBox();
	}

	if (ImGui::Button("Add UI In Canvas"))
	{
		//if(m_pCanvas != nullptr && m_pUI != nullptr)
		//	dynamic_cast<CCanvas*>(m_pCanvas)->Add_ChildUI(m_pUI);
	}
	ImGui::Separator();

	Imgui_UI();

	if (m_pUI)
	{
		ImGui::Separator();
		ImGui::Text("%s", typeid(*m_pUI).name());
		m_pUI->Imgui_RenderProperty();
		m_pUI->Imgui_RenderComponentProperties();
	}
}

void CUI_Manager::SaveToJson(Json & json)
{
	__super::SaveToJson(json);

	json["PositionX"] = m_pUI->Get_Position().x;
	json["PositionY"] = m_pUI->Get_Position().y;
	json["SizeX"] =	m_pUI->Get_Size().x;
	json["SizeY"] = m_pUI->Get_Size().y;
}

void CUI_Manager::LoadFromJson(const Json & json, const _tchar* pPrototypeTag)
{
	//CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/UI/UI_Dafault.json");
	//CUI* pUI = dynamic_cast<CUI*>(pGameInstance->Clone_GameObject_Get(L"Layer_UI", pPrototypeTag, (void*)&json));
	//assert(pUI != nullptr && "유아이 생성 실패!");

	//m_pUIs.emplace_back(pUI);

	//return;
}

HRESULT CUI_Manager::Create_UI()
{

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/UI/UI_Dafault.json");
	CUI* pUI = dynamic_cast<CUI*>(pGameInstance->Clone_GameObject_Get(L"Layer_UI", L"UI_Default", (void*)&json));

	if (pUI == nullptr)
	{
		MSG_BOX("유아이 생성 실패!");
		return E_FAIL;
	}

	m_pUIs.emplace_back(pUI);
	
	return S_OK;
}

HRESULT CUI_Manager::Create_Canvas(const _tchar* pPrototypeTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CUI* pUI = dynamic_cast<CUI*>(pGameInstance->Clone_GameObject_Get(L"Layer_UI", pPrototypeTag));
	assert(pUI != nullptr && "캔버스 생성 실패!");

	m_pCanvases.emplace_back(pUI);
	return S_OK;
}

void CUI_Manager::Imgui_UI()
{
	//static _float fPosition[2];
	//static _float fSize[2];
	//ImGui::InputFloat2("Position", fPosition);
	//ImGui::InputFloat2("Size", fSize);

	//// 전체적인 값을 변경할 때
	//if (ImGui::Button("Set All"))
	//{
	//	m_pTransformCom->Set_Scaled(_float3(m_pUI->Set_Position(fPosition[0]), m_fSizeY, 1.f));
	//	_float fPositionX = (_float)g_iWinSizeX * m_fX;
	//	_float fPositionY = (_float)g_iWinSizeY * m_fY;
	//	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(fPositionX, fPositionY, 0.f, 1.f));
	//}

	//ImGui::Separator();

	//// 값을 곱해서 위치를 변경할 때
	//static _float fValue_RePosition[2];
	//ImGui::InputFloat2("Value Position", fValue_RePosition);

	//if (ImGui::Button("Value Reposition"))
	//{
	//	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet((_float)g_iWinSizeX * fValue_RePosition[0], (_float)g_iWinSizeY * fValue_RePosition[1], 0.f, 1.f));
	//}
	//ImGui::SameLine();
	//if (ImGui::Button("Set Value Reposition"))
	//{
	//	m_fX = fValue_RePosition[0];
	//	m_fY = fValue_RePosition[1];
	//}

	//static _float fValue_ReSize[2];
	//ImGui::InputFloat2("Value ReSize", fValue_ReSize);

	//if (ImGui::Button("Value Resize"))
	//{
	//	m_pTransformCom->Set_Scaled(_float3(m_fSizeX * fValue_ReSize[0], m_fSizeY * fValue_ReSize[1], 1.f));
	//}
	//ImGui::SameLine();
	//if (ImGui::Button("Set Value Resize"))
	//{
	//	m_fSizeX *= fValue_ReSize[0];
	//	m_fSizeY *= fValue_ReSize[1];
	//}

	//static _float fReSize[2];
	//ImGui::DragFloat2("ReSize", fReSize);

	//// 사이즈를 직접적으로 조정할 때
	//static _bool	bReSize;
	//if (ImGui::Button("ReSize"))
	//{
	//	bReSize = !bReSize;
	//}
	//ImGui::SameLine();
	//if (true == bReSize)
	//{
	//	m_pTransformCom->Set_Scaled(_float3(fReSize[0], fReSize[1], 1.f));
	//}
	//if (ImGui::Button("Set Resize"))
	//{
	//	m_fSizeX = fReSize[0];
	//	m_fSizeY = fReSize[1];
	//}
}

CUI_Manager * CUI_Manager::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CUI_Manager*		pInstance = new CUI_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Manager");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Manager::Clone(void * pArg)
{
	CUI_Manager*		pInstance = new CUI_Manager(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Manager");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Manager::Free()
{
	__super::Free();
}
