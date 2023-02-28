#include "stdafx.h"
#include "Imgui_AnimModifier.h"

#include "Animation.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Model.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "Layer.h"

/************************
 * CModelPreviwer
 ************************/

CModelPreviwer::CModelPreviwer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CModelPreviwer::CModelPreviwer(const CModelPreviwer& rhs)
	: CGameObject(rhs)
{
}

HRESULT CModelPreviwer::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CModelPreviwer::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	/* For.Com_Renderer */
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom));
	m_pTransformCom->SetSpeed(1.f);

	if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		if (json.contains("Model"))
		{
			string ProtoModel = json["Model"];
			m_ModelName = CGameUtils::s2ws(ProtoModel);
			FAILED_CHECK(__super::Add_Component(LEVEL_NOW, m_ModelName.c_str(), m_ModelName.c_str(),
				(CComponent**)&m_pModel));
		}
	}

	m_pModel->Add_EventCaller("Test", []() {IM_LOG("Hello World!")});

	return S_OK;
}

void CModelPreviwer::Late_Tick(_double TimeDelta)
{
	if (m_bVisible)
	{
		m_pModel->Play_Animation(TimeDelta);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND_TOON, this);

		if (m_bLocalMoveAccess) 
		{
			_vector vTest = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix());
			m_pTransformCom->LocalMove(vTest);
		}

		{
			_vector vOpTest = m_pModel->GetOptionalMoveVector(m_pTransformCom->Get_WorldMatrix());
			m_pTransformCom->LocalMove(vOpTest);
		}
	}
}

HRESULT CModelPreviwer::Render()
{
	if (m_pAttachPreview)
	{
		m_pTransformCom->Set_WorldMatrix(m_pAttachPreview->GetModel()->GetBoneMatrix(m_AttachBoneName) * m_pAttachPreview->GetTransform()->Get_WorldMatrix());
	}

	m_pModel->Render(m_pTransformCom);
	return S_OK;
}

void CModelPreviwer::SetAttachTo(string BoneName, CModelPreviwer* pAttachPreview)
{
	m_AttachBoneName = BoneName;
	m_pAttachPreview = pAttachPreview;
}

void CModelPreviwer::Imgui_RenderProperty()
{
	ImGui::Checkbox("TestModelLocalMove", &m_bLocalMoveAccess);
}

CAnimation* CModelPreviwer::GetPlayAnimation()
{
	return m_pModel->GetPlayAnimation();
}

void CModelPreviwer::Reset()
{
	if (m_pModel->GetPlayAnimation())
	{
		m_pModel->GetPlayAnimation()->Reset();
	}
}

CModelPreviwer* CModelPreviwer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
 	CModelPreviwer*		pInstance = new CModelPreviwer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CModelPreviwer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CModelPreviwer::Clone(void* pArg)
{
	CModelPreviwer*		pInstance = new CModelPreviwer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModelPreviwer");
		Safe_Release(pInstance); 
	}
	return pInstance;
}

void CModelPreviwer::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModel);
}


/************************
 * CImgui_AnimModifier
 ************************/

CImgui_AnimModifier::CImgui_AnimModifier(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImguiObject(pDevice, pContext)
{
}

HRESULT CImgui_AnimModifier::Initialize(void* pArg)
{
	m_szTabname = "AnimModifier";
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

	return S_OK;
}

void CImgui_AnimModifier::Imgui_RenderTab()
{
	m_pGameInstance->Imgui_ObjectViewer(LEVEL_NOW,m_pSelectedObject);
	
	if (m_pSelectedObject)
	{
		ImGui::Separator();
		ImGui::Text("%s", typeid(*m_pSelectedObject).name());
		m_pSelectedObject->Imgui_RenderProperty();
		m_pSelectedObject->Imgui_RenderComponentProperties();
	}

	if (ImGui::Button("Reset All Animation"))
	{
		for (auto layer : m_pGameInstance->GetLayers(LEVEL_NOW))
		{
			for (auto pObj : layer.second->GetGameObjects())
			{
				if (auto pPreview = dynamic_cast<CModelPreviwer*>(pObj))
				{
					pPreview->Reset();
				}
			}
		}
	}

	if (const auto pPreview = dynamic_cast<CModelPreviwer*>(m_pSelectedObject))
	{
		m_pPreview = pPreview;
	}
	else
	{
		m_pPreview = nullptr;
	}

	if (m_pPreview)
	{
		if (ImGui::Button("visible this"))
		{
			m_pPreview->SetVisible(!m_pPreview->IsVisible());
		}
	}

	RootMotionMaker();

	static Json AnimModifiers = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Meshes/Scarlet_Nexus/AnimationModifier.json");

	if (m_pPreview && m_pPreview->GetPlayAnimation())
	{
		m_pPreview->GetPlayAnimation()->Imgui_RenderProperty();
		if (ImGui::Button("Save Current Anim Data To File"))
		{
			Json json;
			m_pPreview->GetPlayAnimation()->SaveModifiedData(json);
			string animName = m_pPreview->GetPlayAnimation()->GetName();
	
			AnimModifiers[animName] = json;
			CJsonStorage::GetInstance()->UpdateJson("../Bin/Resources/Meshes/Scarlet_Nexus/AnimationModifier.json", AnimModifiers);
			CJsonStorage::GetInstance()->SaveJson("../Bin/Resources/Meshes/Scarlet_Nexus/AnimationModifier.json");
		}
	}
}

CImgui_AnimModifier* CImgui_AnimModifier::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	auto inst = new CImgui_AnimModifier(pDevice, pContext);
	if (FAILED(inst->Initialize(pArg)))
	{
		Safe_Release(inst);
		return nullptr;
	}

	return inst;
}

void CImgui_AnimModifier::RootMotionMaker()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	if (ImGui::CollapsingHeader("RootMotionMaker"))
	{
		static _float4 vPeekingPos;
		static _float4 vModelPos;
		static _float2 fMoveTime;
		static _bool   bPeekMode;

		ImGui::Checkbox("PeekMode", &bPeekMode);
		
		vPeekingPos = pGameInstance->GetPeekingPos();
		if(nullptr != m_pPreview)
			vModelPos = m_pPreview->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
		ImGui::InputFloat4("PeekingPos", &vPeekingPos.x);
		ImGui::InputFloat4("ModelPos", &vModelPos.x);
		ImGui::InputFloat2("MoveTime", &fMoveTime.x);

		static _float4 vOptionalRootVector;

		if (pGameInstance->KeyDown(CInput_Device::DIM_LB) && bPeekMode)
		{
			vOptionalRootVector = vPeekingPos - vModelPos;
			bPeekMode = false;
		}

		ImGui::InputFloat4("RootVector", &vOptionalRootVector.x);

		if (ImGui::Button("Add RootMotion"))
		{
			OPTIONAL_ROOTMOTION Root;
			ZeroMemory(&Root, sizeof(OPTIONAL_ROOTMOTION));
			Root.fStartTime = fMoveTime.x;
			Root.fEndTime = fMoveTime.y;
			XMStoreFloat4(&Root.vOptionalRootVector, vOptionalRootVector);

			m_pPreview->GetModel()->Add_OptionalRootMotion(Root);
		}
		if (ImGui::Button("Delete RootMotion"))
		{
			m_pPreview->GetModel()->Delete_OptionalRootMotion();
		}
	}
}

void CImgui_AnimModifier::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
}

