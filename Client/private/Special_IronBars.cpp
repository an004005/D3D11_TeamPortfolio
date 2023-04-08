#include "stdafx.h"
#include "Special_IronBars.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "GameInstance.h"
#include "Model.h"
#include "RigidBody.h"
#include "MathUtils.h"
#include "Animation.h"
#include "ImguiUtils.h"

#include "Special_IronBars_Door.h"
#include "Special_IronBars_Bars.h"
#include "Special_IronBars_SingleBars.h"
#include "Special_IronBars_MultiBars.h"

#include "PlayerInfoManager.h"

CSpecial_IronBars::CSpecial_IronBars(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CSpecialObject(pDevice, pContext)
{
}

CSpecial_IronBars::CSpecial_IronBars(const CSpecial_IronBars& rhs)
	: CSpecialObject(rhs)
{
}

HRESULT CSpecial_IronBars::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CSpecial_IronBars::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components(pArg));

	m_pTransformCom->SetTransformDesc({ 0.1f, XMConvertToRadians(90.f) });

	m_eSpecialObejctType = SPECIAL_IRONBARS;

	m_vecSingleBarBoneName = {
		"pr_grille_crash1_1",
		"pr_grille_crash1_2",
		"pr_grille_crash1_3",
		"pr_grille_crash2_1",
		"pr_grille_crash2_2",
		"pr_grille_crash3_1",
		"pr_grille_crash3_2",
		"pr_grille_crash3_3"
	};

	Create_Child();

	return S_OK;
}

void CSpecial_IronBars::BeginTick()
{
	__super::BeginTick();
}

void CSpecial_IronBars::Tick(_double TimeDelta)
{
	if (m_bDeadCheck)
	{
		static_cast<CSpecial_IronBars_Door*>(m_pDoor)->SetOutline(false);
		static_cast<CSpecial_IronBars_Bars*>(m_pBars)->SetOutline(false);

		for (_uint i = 0; i < 8; ++i)
		{
			static_cast<CSpecial_IronBars_SingleBars*>(m_pSingleBar[i])->SetOutline(false);
		}

		static_cast<CSpecial_IronBars_MultiBars*>(m_pMultiBars)->SetOutline(false);


		m_fDeadTime -= (_float)TimeDelta;

		if (0.f >= m_fDeadTime)
		{
			m_pDoor->SetDelete();
			m_pBars->SetDelete();

			for (_uint i = 0; i < 8; ++i)
			{
				m_pSingleBar[i]->SetDelete();
			}

			m_pMultiBars->SetDelete();

			this->SetDelete();
		}
	}
	else
	{
		if (CPlayerInfoManager::GetInstance()->Get_SpecialObject() == this)
		{
			static_cast<CSpecial_IronBars_Door*>(m_pDoor)->SetOutline(true);
			static_cast<CSpecial_IronBars_Bars*>(m_pBars)->SetOutline(true);

			for (_uint i = 0; i < 8; ++i)
			{
				static_cast<CSpecial_IronBars_SingleBars*>(m_pSingleBar[i])->SetOutline(true);
			}

			static_cast<CSpecial_IronBars_MultiBars*>(m_pMultiBars)->SetOutline(true);
		}
		else
		{
			static_cast<CSpecial_IronBars_Door*>(m_pDoor)->SetOutline(false);
			static_cast<CSpecial_IronBars_Bars*>(m_pBars)->SetOutline(false);

			for (_uint i = 0; i < 8; ++i)
			{
				static_cast<CSpecial_IronBars_SingleBars*>(m_pSingleBar[i])->SetOutline(false);
			}

			static_cast<CSpecial_IronBars_MultiBars*>(m_pMultiBars)->SetOutline(false);
		}
	}

	__super::Tick(TimeDelta);

	m_pDoor->Tick(TimeDelta);
	m_pBars->Tick(TimeDelta);

	static_cast<CSpecial_IronBars_Door*>(m_pDoor)->Set_SyncMatrix(m_DoorSyncMatrix);
	static_cast<CSpecial_IronBars_Door*>(m_pDoor)->Sync_Position(m_pTransformCom);

	static_cast<CSpecial_IronBars_Bars*>(m_pBars)->Set_SyncMatrix(m_BarsSyncMatrix);
	static_cast<CSpecial_IronBars_Bars*>(m_pBars)->Sync_Position(m_pTransformCom);

	for (_uint i = 0; i < 8; ++i)
	{
		if (!m_bDecompose)
			static_cast<CSpecial_IronBars_SingleBars*>(m_pSingleBar[i])->Attach_BoneMatrix(static_cast<CSpecial_IronBars_Bars*>(m_pBars)->Get_Model(), m_pBars->GetTransform(), m_vecSingleBarBoneName[i]);
		
		m_pSingleBar[i]->Tick(TimeDelta);
	}

	m_pMultiBars->Tick(TimeDelta);
}

void CSpecial_IronBars::Late_Tick(_double TimeDelta)
{
	m_pDoor->Late_Tick(TimeDelta);

	if (!m_bDecompose)
	{
		m_pBars->Late_Tick(TimeDelta);
	}
	else if (m_bDecompose && !m_bBundleCompose)
	{
		for (_uint i = 0; i < 8; ++i)
		{
			m_pSingleBar[i]->Late_Tick(TimeDelta);
		}
	}
	else
	{
		m_pMultiBars->Late_Tick(TimeDelta);
	}
}

void CSpecial_IronBars::AfterPhysX()
{
	__super::AfterPhysX();

	m_pDoor->AfterPhysX();
	m_pBars->AfterPhysX();

	for (_uint i = 0; i < 8; ++i)
	{
		m_pSingleBar[i]->AfterPhysX();
	}

	m_pMultiBars->AfterPhysX();
}

HRESULT CSpecial_IronBars::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

void CSpecial_IronBars::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);

	if (json.contains("LocalMatrix"))
		m_LocalMatrix = json["LocalMatrix"];

	if (json.contains("DoorSyncMatrix"))
		m_DoorSyncMatrix = json["DoorSyncMatrix"];

	if (json.contains("BarsSyncMatrix"))
		m_BarsSyncMatrix = json["BarsSyncMatrix"];
}

void CSpecial_IronBars::SaveToJson(Json & json)
{
	__super::SaveToJson(json);

	json["LocalMatrix"] = m_LocalMatrix;

	json["DoorSyncMatrix"] = m_DoorSyncMatrix;

	json["BarsSyncMatrix"] = m_BarsSyncMatrix;
}

void CSpecial_IronBars::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	if (ImGui::CollapsingHeader("Door"))
	{
		static GUIZMO_INFO Door;
		CImguiUtils::Render_Guizmo(&m_DoorSyncMatrix, Door, true, true);
	}
	if (ImGui::CollapsingHeader("Bars"))
	{
		static GUIZMO_INFO Bars;
		CImguiUtils::Render_Guizmo(&m_BarsSyncMatrix, Bars, true, true);
	}

	if (ImGui::Button("Open"))	IronBars_AnimActive(true);
	if (ImGui::Button("Close"))	IronBars_AnimActive(false);

	if (ImGui::Button("Kinetic_On"))	IronBars_Decompose(true);
	if (ImGui::Button("Kinetic_Off"))	IronBars_Decompose(false);
}

void CSpecial_IronBars::IronBars_AnimActive(_bool bActive)
{
	static_cast<CSpecial_IronBars_Door*>(m_pDoor)->Set_Activate(bActive);
	static_cast<CSpecial_IronBars_Bars*>(m_pBars)->Set_Activate(bActive);
}

void CSpecial_IronBars::IronBars_Decompose(_bool bDecompose)
{
	m_bDecompose = bDecompose;
}

void CSpecial_IronBars::IronBars_SetTrigger(_bool bTrigger)
{
	for (_uint i = 0; i < 8; ++i)
	{
		static_cast<CSpecial_IronBars_SingleBars*>(m_pSingleBar[i])->Set_Trigger(bTrigger);
	}
}

void CSpecial_IronBars::IronBars_DecomposeEffect()
{
	for (_uint i = 0; i < 8; ++i)
	{
		static_cast<CSpecial_IronBars_SingleBars*>(m_pSingleBar[i])->DecomposeEffect();
	}
}

void CSpecial_IronBars::IronBars_AttachAnim(CModel* pModel, CTransform* pTransform, _float4 vPoint)
{
	for (_uint i = 0; i < 8; ++i)
	{
		static_cast<CSpecial_IronBars_SingleBars*>(m_pSingleBar[i])->Attack_BoneMatrix_SetPoint(pModel, pTransform, "Waist", vPoint);
	}
}

void CSpecial_IronBars::IronBars_LookAtTarget(_float4 vTargetPos, _float fRatio)
{
	for (_uint i = 0; i < 8; ++i)
	{
		static_cast<CSpecial_IronBars_SingleBars*>(m_pSingleBar[i])->Calculate_TargetDir(vTargetPos, fRatio);
	}
}

void CSpecial_IronBars::IronBars_Shooting_All(_float4 vTargetPos)
{
	for (_uint i = 0; i < 8; ++i)
	{
		static_cast<CSpecial_IronBars_SingleBars*>(m_pSingleBar[i])->Shooting(vTargetPos);
	}
}

void CSpecial_IronBars::IronBars_Shooting_Single(_float4 vTargetPos, _uint iIndex)
{
	static_cast<CSpecial_IronBars_SingleBars*>(m_pSingleBar[iIndex])->Shooting(vTargetPos);
}

void CSpecial_IronBars::IronBars_Reload(_float4 vDestPos, _float4 vTargetPos, _float fRatio)
{
	static_cast<CSpecial_IronBars_SingleBars*>(m_pSingleBar[0])->Reloading(vDestPos + _float4(0.f, -2.f, 3.f, 0.f), vTargetPos, fRatio);
	static_cast<CSpecial_IronBars_SingleBars*>(m_pSingleBar[1])->Reloading(vDestPos + _float4(0.f, -2.f, 1.f, 0.f), vTargetPos, fRatio);
	static_cast<CSpecial_IronBars_SingleBars*>(m_pSingleBar[2])->Reloading(vDestPos + _float4(0.f, -1.5f, 3.5f, 0.f), vTargetPos, fRatio);
	static_cast<CSpecial_IronBars_SingleBars*>(m_pSingleBar[3])->Reloading(vDestPos + _float4(0.f, -1.5f, 0.5f, 0.f), vTargetPos, fRatio);
	static_cast<CSpecial_IronBars_SingleBars*>(m_pSingleBar[4])->Reloading(vDestPos + _float4(0.f, -1.f, 3.5f, 0.f), vTargetPos, fRatio);
	static_cast<CSpecial_IronBars_SingleBars*>(m_pSingleBar[5])->Reloading(vDestPos + _float4(0.f, -1.f, 0.5f, 0.f), vTargetPos, fRatio);
	static_cast<CSpecial_IronBars_SingleBars*>(m_pSingleBar[6])->Reloading(vDestPos + _float4(0.f, -0.5f, 1.f, 0.f), vTargetPos, fRatio);
	static_cast<CSpecial_IronBars_SingleBars*>(m_pSingleBar[7])->Reloading(vDestPos + _float4(0.f, -0.5f, 3.f, 0.f), vTargetPos, fRatio);
}

void CSpecial_IronBars::IronBars_LerpAnim(CModel* pModel, CTransform* pTransform, _float4 vPoint, _float fRatio)
{
	for (_uint i = 0; i < 8; ++i)
	{
		static_cast<CSpecial_IronBars_SingleBars*>(m_pSingleBar[i])->Lerp_BoneMatrix(pModel, pTransform, "Waist", vPoint, fRatio);
	}
}

void CSpecial_IronBars::IronBars_ChangeToBundle()
{
	if (m_bBundleCompose) return;

	m_bBundleCompose = true;

	for (_uint i = 0; i < 8; ++i)
	{
		static_cast<CSpecial_IronBars_SingleBars*>(m_pSingleBar[i])->Activate(false);
	}
}

void CSpecial_IronBars::IronBars_AttachAnim_MulitBars(CModel* pModel, CTransform* pTransform, _float4 vPoint)
{
	static_cast<CSpecial_IronBars_MultiBars*>(m_pMultiBars)->Attack_BoneMatrix_SetPoint(pModel, pTransform, "Waist", vPoint);
}

void CSpecial_IronBars::IronBars_Shooting_Finish(_float4 vTargetPos)
{
	static_cast<CSpecial_IronBars_MultiBars*>(m_pMultiBars)->Shooting(vTargetPos);

	m_bDeadCheck = true;
	m_fDeadTime = 10.f;
}

void CSpecial_IronBars::IronBars_SetDead()
{
	m_bDeadCheck = true;
	m_fDeadTime = 10.f;
}

void CSpecial_IronBars::IronBars_SingleBars_Particle()
{
	for (_uint i = 0; i < 8; ++i)
	{
		static_cast<CSpecial_IronBars_SingleBars*>(m_pSingleBar[i])->CreateKineticParticle_Mini({ 0.f, 0.f, 0.f, 0.f }, {0.1f, 0.1f, 0.1f, 0.f});
	}
}

HRESULT CSpecial_IronBars::SetUp_Components(void * pArg)
{
	FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", L"Collider", (CComponent**)&m_pCollider, pArg));

	return S_OK;
}

HRESULT CSpecial_IronBars::Create_Child()
{
	// Door
	{
		wstring wstrLayer = L"Layer_Kinetic";
		string strJsonPath = "../Bin/Resources/Objects/KineticSpecial_IronBars_Door.json";
		wstring wstrProtoTag = L"Prototype_GameObject_Special_IronBars_Door";

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
			if (strJsonPath.empty() == false)
			{
				Json json = CJsonStorage::GetInstance()->FindOrLoadJson(strJsonPath);
				m_pDoor = CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, wstrProtoTag.c_str(), &json);
			}
			else
			{
				m_pDoor = CGameInstance::GetInstance()->Clone_GameObject_Get(wstrLayer.c_str(), wstrProtoTag.c_str());
			}
		}

		static_cast<CSpecial_IronBars_Door*>(m_pDoor)->Set_SyncMatrix(m_DoorSyncMatrix);
		static_cast<CSpecial_IronBars_Door*>(m_pDoor)->Sync_Position(m_pTransformCom);
	}
	// ~Door

	// Bars
	{
		wstring wstrLayer = L"Layer_Kinetic";
		string strJsonPath = "";
		wstring wstrProtoTag = L"Prototype_GameObject_Special_IronBars_Bars";

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
			if (strJsonPath.empty() == false)
			{
				Json json = CJsonStorage::GetInstance()->FindOrLoadJson(strJsonPath);
				m_pBars = CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, wstrProtoTag.c_str(), &json);
			}
			else
			{
				m_pBars = CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, wstrProtoTag.c_str());
				//m_pBars = CGameInstance::GetInstance()->Clone_GameObject_Get(wstrLayer.c_str(), wstrProtoTag.c_str());
			}
		}

		static_cast<CSpecial_IronBars_Bars*>(m_pBars)->Set_SyncMatrix(m_BarsSyncMatrix);
		static_cast<CSpecial_IronBars_Bars*>(m_pBars)->Sync_Position(m_pTransformCom);
	}
	// ~Bars

	// SingleBars
	{
		for (_uint i = 0; i < 8; ++i)
		{
			wstring wstrLayer = L"Layer_Kinetic";
			string strJsonPath = "../Bin/Resources/Objects/KineticSpecial_IronBars_SingleBars.json";
			wstring wstrProtoTag = L"Prototype_GameObject_Special_IronBars_SingleBars";

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
				if (strJsonPath.empty() == false)
				{
					Json json = CJsonStorage::GetInstance()->FindOrLoadJson(strJsonPath);
					m_pSingleBar[i] = CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, wstrProtoTag.c_str(), &json);
				}
				else
				{
					m_pSingleBar[i] = CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, wstrProtoTag.c_str());
					//m_pBars = CGameInstance::GetInstance()->Clone_GameObject_Get(wstrLayer.c_str(), wstrProtoTag.c_str());
				}
			}

			static_cast<CSpecial_IronBars_SingleBars*>(m_pSingleBar[i])->Attach_BoneMatrix(static_cast<CSpecial_IronBars_Bars*>(m_pBars)->Get_Model(), m_pBars->GetTransform(), m_vecSingleBarBoneName[i]);
		}
	}
	// ~SingleBars

	// MultiBars
	{
		wstring wstrLayer = L"Layer_Kinetic";
		string strJsonPath = "../Bin/Resources/Objects/KineticSpecial_IronBars_MultiBars.json";
		wstring wstrProtoTag = L"Prototype_GameObject_Special_IronBars_MultiBars";

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
			if (strJsonPath.empty() == false)
			{
				Json json = CJsonStorage::GetInstance()->FindOrLoadJson(strJsonPath);
				m_pMultiBars = CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, wstrProtoTag.c_str(), &json);
			}
			else
			{
				m_pMultiBars = CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, wstrProtoTag.c_str());
				//m_pBars = CGameInstance::GetInstance()->Clone_GameObject_Get(wstrLayer.c_str(), wstrProtoTag.c_str());
			}
		}
	}
	// ~MultiBars

	return S_OK;
}

CSpecial_IronBars* CSpecial_IronBars::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpecial_IronBars* pInstance = new CSpecial_IronBars(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpecial_IronBars");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CSpecial_IronBars::Clone(void * pArg)
{
	CGameObject* pInstance = new CSpecial_IronBars(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpecial_IronBars");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSpecial_IronBars::Free()
{
	__super::Free();

	if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pDoor))
		Safe_Release(m_pDoor);

	if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pBars))
		Safe_Release(m_pBars);

	for (auto& iter : m_pSingleBar)
	{
		if (CGameInstance::GetInstance()->Check_ObjectAlive(iter))
			Safe_Release(iter);
	}

	if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pMultiBars))
		Safe_Release(m_pMultiBars);
}
