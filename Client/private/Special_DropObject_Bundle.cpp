#include "stdafx.h"
#include "..\public\Special_DropObject_Bundle.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "GameInstance.h"
#include "Model.h"
#include "RigidBody.h"
#include "MathUtils.h"
#include "Animation.h"
#include "ImguiUtils.h"
#include "Special_DropObject_Single.h"
#include "MapObject.h"
#include "Enemy.h"

CSpecial_DropObject_Bundle::CSpecial_DropObject_Bundle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CSpecialObject(pDevice, pContext)
{
}

CSpecial_DropObject_Bundle::CSpecial_DropObject_Bundle(const CSpecial_DropObject_Bundle & rhs)
	:CSpecialObject(rhs)
{
}

HRESULT CSpecial_DropObject_Bundle::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CSpecial_DropObject_Bundle::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components(pArg));

	m_pTransformCom->SetTransformDesc({ 0.1f, XMConvertToRadians(90.f) });

	m_eSpecialObejctType = SPECIAL_DROPOBJECT_BUNDLE;

	m_pCollider->SetOnTriggerIn([this](CGameObject* pGameObject)
	{
		if (!m_bThrow)
			return;

		if (auto pStatic = dynamic_cast<CMapObject*>(pGameObject))
		{
			m_bDecompose = true;
			m_bDeadCheck = true;
			m_fDeadTime = 3.f;
		}

		if (auto pMonster = dynamic_cast<CEnemy*>(pGameObject))
		{
			DAMAGE_PARAM tParam;
			ZeroMemory(&tParam, sizeof(DAMAGE_PARAM));
			tParam.eAttackSAS = ESASType::SAS_END;
			tParam.eAttackType = EAttackType::ATK_SPECIAL_END;
			tParam.eDeBuff = EDeBuffType::DEBUFF_END;
			tParam.eKineticAtkType = EKineticAttackType::KINETIC_ATTACK_DEFAULT;
			tParam.iDamage = 1000;
			tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

			static_cast<CEnemy*>(pMonster)->TakeDamage(tParam);

			m_bDecompose = true;
			m_bDeadCheck = true;
			m_fDeadTime = 3.f;
		}
	});

	Create_ChildObject();

	return S_OK;
}

void CSpecial_DropObject_Bundle::BeginTick()
{
	__super::BeginTick();

	m_pCollider->Set_Kinetic(true);
	m_pCollider->UpdateChange();
}

void CSpecial_DropObject_Bundle::Tick(_double TimeDelta)
{
	if (m_bDeadCheck)
	{
		m_fDeadTime -= (_float)TimeDelta;

		if (0.f >= m_fDeadTime)
		{
			for (auto& iter : m_pObject_Single)
				iter->SetDelete();

			this->SetDelete();
		}
	}

	if (!m_bDecompose)
	{
		__super::Tick(TimeDelta);

		m_pCollider->Update_Tick(m_pTransformCom);

		for (auto& iter : m_pObject_Single)
			static_cast<CSpecial_DropObject_Single*>(iter)->Sync_position(m_pTransformCom);
	}
	else
	{
		if (Decompose.IsNotDo())
		{
			Safe_Release(m_pCollider);

			for (auto& iter : m_pObject_Single)
			{
				static_cast<CSpecial_DropObject_Single*>(iter)->Set_Kinetic(false);
				static_cast<CSpecial_DropObject_Single*>(iter)->Activate(true);
			}
		}

		m_pCollider->Activate(false);

		for (auto& iter : m_pObject_Single)
			iter->Tick(TimeDelta);
	}
}

void CSpecial_DropObject_Bundle::Late_Tick(_double TimeDelta)
{
	//__super::Late_Tick(TimeDelta);

	for (auto& iter : m_pObject_Single)
		iter->Late_Tick(TimeDelta);
}

void CSpecial_DropObject_Bundle::AfterPhysX()
{
	if (!m_bDecompose)
	{
		__super::AfterPhysX();
		m_pCollider->Update_AfterPhysX(m_pTransformCom);
	}
	else
	{
		for (auto& iter : m_pObject_Single)
			iter->AfterPhysX();
	}
}

HRESULT CSpecial_DropObject_Bundle::Render()
{
	//FAILED_CHECK(__super::Render());

	return S_OK;
}

void CSpecial_DropObject_Bundle::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);

	if (json.contains("ModelTags"))
	{
		for (auto tag : json["ModelTags"])
			m_pModelTags.push_back(s2ws(tag));
	}

	if (json.contains("LocalMatrix"))
		m_LocalMatrix = json["LocalMatrix"];

	for (_float4x4 MatChild : json["ChildMatrix"])
		m_vecChildMatrix.push_back(MatChild);
}

void CSpecial_DropObject_Bundle::SaveToJson(Json & json)
{
	__super::SaveToJson(json);

	for (auto tag : m_pModelTags)
		json["ModelTags"].push_back(ws2s(tag));

	json["LocalMatrix"] = m_LocalMatrix;

	for (_float4x4 MatChild : m_vecChildMatrix)
		json["ChildMatrix"].push_back(MatChild);
}

void CSpecial_DropObject_Bundle::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	static int iChildNum = 0;
	ImGui::InputInt("ChildNum", &iChildNum, 1);

	if (ImGui::CollapsingHeader("ChildGizmo"))
	{
		static GUIZMO_INFO tp;
		CImguiUtils::Render_Guizmo(&m_vecChildMatrix[iChildNum], tp, true, true);

		_uint iChildIdx = 0;
		for (auto& iter : m_pObject_Single)
		{
			static_cast<CSpecial_DropObject_Single*>(iter)->Set_ChildMatrix(m_vecChildMatrix[iChildIdx++]);
		}
	}
}

void CSpecial_DropObject_Bundle::Set_Kinetic(_bool bKinetic)
{
	m_pCollider->Set_Kinetic(bKinetic);
	m_pCollider->UpdateChange();
}

void CSpecial_DropObject_Bundle::DropObject_Floating()
{
	m_pCollider->AddVelocity({0.f, 5.f, 0.f});
	m_pCollider->AddTorque({0.f, 0.f, 0.f});
}

void CSpecial_DropObject_Bundle::DropObject_Drop()
{
	m_bThrow = true;
	m_pCollider->AddVelocity({ 0.f, -30.f, 0.f });
	m_pCollider->AddTorque({ 0.f, 0.f, 0.f });
}

HRESULT CSpecial_DropObject_Bundle::SetUp_Components(void * pArg)
{
	FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", L"Collider", (CComponent**)&m_pCollider, pArg));

	return S_OK;
}

void CSpecial_DropObject_Bundle::Create_ChildObject()
{
	wstring wstrLayer = L"Layer_Kinetic";
	string strJsonPath = "../Bin/Resources/Objects/KineticSpecial_DropObject_Single.json";
	wstring wstrProtoTag = L"Prototype_GameObject_Special_DropObject_Single";

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
		_uint iChildIdx = 0;
		for (auto& iter : m_pObject_Single)
		{
			if (strJsonPath.empty() == false)
			{
				Json json = CJsonStorage::GetInstance()->FindOrLoadJson(strJsonPath);
				iter = CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, wstrProtoTag.c_str(), &json);
			}
			else
			{
				iter = CGameInstance::GetInstance()->Clone_GameObject_Get(wstrLayer.c_str(), wstrProtoTag.c_str());
			}
			
			static_cast<CSpecial_DropObject_Single*>(iter)->Set_ChildMatrix(m_vecChildMatrix[iChildIdx++]);
			static_cast<CSpecial_DropObject_Single*>(iter)->Sync_position(m_pTransformCom);
		}
	}
}

CSpecial_DropObject_Bundle * CSpecial_DropObject_Bundle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpecial_DropObject_Bundle* pInstance = new CSpecial_DropObject_Bundle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpecial_DropObject_Bundle");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CSpecial_DropObject_Bundle::Clone(void * pArg)
{
	CSpecial_DropObject_Bundle* pInstance = new CSpecial_DropObject_Bundle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpecial_DropObject_Bundle");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSpecial_DropObject_Bundle::Free()
{
	__super::Free();
	
	for (auto& iter : m_pObject_Single)
	{
		if (CGameInstance::GetInstance()->Check_ObjectAlive(iter))
			Safe_Release(iter);
	}
}
