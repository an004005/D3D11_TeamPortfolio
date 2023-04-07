#include "stdafx.h"
#include "Imgui_CamAnimEditor.h"
#include "GameInstance.h"
#include "AnimCam.h"
#include "Camera_Manager.h"
#include "Animation.h"

CModelTester::CModelTester(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CModelTester::CModelTester(const CModelTester& rhs)
	: CGameObject(rhs)
{
}

HRESULT CModelTester::Initialize_Prototype()
{
	return CGameObject::Initialize_Prototype();
}

HRESULT CModelTester::Initialize(void* pArg)
{
	/* For.Com_Renderer */
	CGameObject::Initialize(pArg);
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom));
	return S_OK;
}

void CModelTester::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);

	if (m_bVisible)
		m_pRendererCom->Add_RenderGroup(m_eRenderGroup, this);
}

HRESULT CModelTester::Render()
{
	m_pModel->Render(m_pTransformCom);
	return S_OK;
}

void CModelTester::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();

	if (auto pAnim = m_pModel->GetPlayAnimation())
	{
		pAnim->Imgui_RenderProperty();
	}
}

void CModelTester::SetModel(const string& strModelTag)
{
	Safe_Release(m_pModel);
	Delete_Component(L"Model");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, s2ws(strModelTag).c_str(), TEXT("Model"),
		(CComponent**)&m_pModel));
}

void CModelTester::SetPlayAnimation(const string& strAnim)
{
	m_pModel->SetPlayAnimation(strAnim);
}

void CModelTester::PlayAnimation(_float fRatio)
{
	m_pModel->Play_Animation_Sync(fRatio);
}

CModelTester* CModelTester::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CModelTester* pInstance = new CModelTester(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CModelTester");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CModelTester::Clone(void* pArg)
{
	CModelTester* pInstance = new CModelTester(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModelTester");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CModelTester::Free()
{
	CGameObject::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModel);
}

CImgui_CamAnimEditor::CImgui_CamAnimEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImguiObject(pDevice, pContext)
{
}

HRESULT CImgui_CamAnimEditor::Initialize(void* pArg)
{
	m_szWindowName = "Cam Anim Editor Window";
	m_szTabname = "Cam Anim Editor Tab";

	if (CGameInstance::GetInstance()->Find_Prototype(LEVEL_STATIC, L"Prototype_ModelTester") == nullptr)
		CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Prototype_ModelTester", CModelTester::Create(m_pDevice, m_pContext));

	m_pAnimCam = dynamic_cast<CAnimCam*>(CCamera_Manager::GetInstance()->Add_Camera("EditorCam", LEVEL_NOW, L"Layer_CamAnimEditor", L"Prototype_AnimCam", nullptr));
	Assert(m_pAnimCam != nullptr);

	return S_OK;
}

void CImgui_CamAnimEditor::Imgui_RenderWindow()
{
	if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pAnimCam) == false)
		return;

	if (m_pCurAnim)
	{
		m_pCurAnim->Imgui_Render();
	}
}

void CImgui_CamAnimEditor::Imgui_RenderTab()
{
	if (m_pAnimCam)
	{
		m_pAnimCam->Imgui_RenderComponentProperties();
		ImGui::Separator();
		m_pAnimCam->Imgui_RenderProperty();


		for (auto& eventName : m_pAnimCam->GetRequestedEvents())
		{
			auto itr = m_AnimEvents.find(eventName);
			if (itr != m_AnimEvents.end())
			{
				for (auto& AnimEvent : itr->second)
				{
					AnimEvent.first->SetPlayAnimation(AnimEvent.second);
				}
			}
		}
	}

	if (ImGui::Button("Start Anim") && m_pCurAnim && m_pAnimCam)
	{
		m_pAnimCam->StartCamAnim(m_pCurAnim,
			_float4x4::Identity,
			m_pAnimCam->GetTransform()->Get_WorldMatrix_f4x4());
	}



	ImGui::Separator();
	static char szCamAnimName[MAX_PATH]{};
	ImGui::InputText("CamAnimName", szCamAnimName, MAX_PATH);

	if (ImGui::Button("Create New CamAnim"))
	{
		auto pCamAnim = CCamera_Manager::GetInstance()->GetCamAnim(szCamAnimName);
		if (pCamAnim == nullptr)
		{
			pCamAnim = CCamAnimation::Create();
			pCamAnim->SetName(szCamAnimName);
			CCamera_Manager::GetInstance()->AddCamAnim(pCamAnim);
		}

	}
	if (ImGui::Button("Load CamAnim"))
	{
		auto pCamAnim = CCamera_Manager::GetInstance()->GetCamAnim(szCamAnimName);
		if (pCamAnim)
		{
			m_pCurAnim = pCamAnim;
		}
	}

	if (ImGui::Button("Save All CamAnim"))
	{
		CCamera_Manager::GetInstance()->SaveCamAnims();
	}

	ImGui::Separator();

	static char szModelTag[MAX_PATH]{};
	static _bool bToonModel = true;
	ImGui::InputText("ModelTag", szModelTag, MAX_PATH);
	ImGui::Checkbox("Model Is Toon", &bToonModel);

	if (ImGui::Button("Add Model"))
	{
		auto pModelTester = dynamic_cast<CModelTester*>(CGameInstance::GetInstance()->Clone_GameObject_Get(LEVEL_NOW, L"Layer_CamAnimEditor", L"Prototype_ModelTester"));
		pModelTester->SetModel(string(szModelTag));
		if (bToonModel == false)
			pModelTester->SetRenderGroup(CRenderer::RENDER_NONALPHABLEND);
		m_Models.emplace(string(szModelTag), pModelTester);
	}

	static string strSelectModel;
	if (ImGui::BeginListBox("ModelTesters"))
	{
		for (auto model : m_Models)
		{
			const bool bSelected = strSelectModel == model.first;
			if (bSelected)
				ImGui::SetItemDefaultFocus();

			if (ImGui::Selectable(model.first.c_str(), bSelected))
				strSelectModel = model.first;
		}
		ImGui::EndListBox();
	}
	if (ImGui::CollapsingHeader("Selected Model Tester Properties") && strSelectModel.empty() == false)
	{
		m_Models.find(strSelectModel)->second->Imgui_RenderProperty();
		m_Models.find(strSelectModel)->second->Imgui_RenderComponentProperties();

		if (nullptr != m_pAnimCam)
			m_Models.find(strSelectModel)->second->PlayAnimation(m_pCurAnim->GetPlayRatio());
	}

	static char szAnimName[MAX_PATH]{};
	static char szEventName[MAX_PATH]{};
	ImGui::InputText("Animation Name of Selected Tester", szAnimName, MAX_PATH);
	ImGui::InputText("Cam Event Name", szEventName, MAX_PATH);

	if (ImGui::Button("Add Event of Model Anim"))
	{
		string strAnimName(szAnimName);
		string strEventName(szEventName);
		auto pModelTester = m_Models.find(strSelectModel)->second;

		if (m_AnimEvents.find(strEventName) == m_AnimEvents.end())
		{
			list<pair<CModelTester*, string>> tmp;
			m_AnimEvents.emplace(strEventName, tmp);
		}

		auto eventList = m_AnimEvents.find(strEventName);

		eventList->second.push_back({ pModelTester, strAnimName });
	}

}

CImgui_CamAnimEditor* CImgui_CamAnimEditor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	auto inst = new CImgui_CamAnimEditor(pDevice, pContext);
	if (FAILED(inst->Initialize(pArg)))
	{
		return nullptr;
	}

	return inst;
}

void CImgui_CamAnimEditor::Free()
{
	CImguiObject::Free();
}
