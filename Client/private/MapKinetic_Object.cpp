#include "stdafx.h"
#include "..\public\MapKinetic_Object.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "RigidBody.h"
#include "Monster.h"
#include "Player.h"
#include "ScarletMap.h"
#include "ImguiUtils.h"

CMapKinetic_Object::CMapKinetic_Object(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMapObject(pDevice, pContext)
{
}

CMapKinetic_Object::CMapKinetic_Object(const CMapKinetic_Object & rhs)
	: CMapObject(rhs)
{
}

HRESULT CMapKinetic_Object::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CMapKinetic_Object::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		if (json.contains("InitPos"))
		{
			_float4 InitPos = json["InitPos"];
			_float4x4 WorldMatrix = XMMatrixTranslationFromVector(InitPos);
			CTransform::ModifyTransformJson(json, WorldMatrix);
			// m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&InitPos));
		}
	}

	FAILED_CHECK(SetUp_Components(pArg));

	// m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, -30.f, 0.f, 1.f));
	m_pTransformCom->SetTransformDesc({ 1.f, XMConvertToRadians(180.f) });

	if (CScarletMap::s_bMapEditor == true)
	{
		m_pCollider->Set_Kinetic(true);
		m_pCollider->UpdateChange();
	}

	// m_pCollider->Activate(true);
	// m_pCollider->SetPxWorldMatrix(m_pTransformCom->Get_WorldMatrix_f4x4());
	// m_pCollider->Set_Kinetic(true);
	// m_pCollider->UpdateChange();

	m_bUsable = true;

	// 다이나믹 리지드 바디가 몬스터와 충돌했는지?
	m_pCollider->SetOnTriggerIn([this](CGameObject* pGameObject)
	{
		if (!m_bUsable)
			return;

		if (auto pMonster = dynamic_cast<CMonster*>(pGameObject))
		{
			DAMAGE_PARAM tParam;
			tParam.iDamage = 1;
			tParam.eAttackType = EAttackType::ATK_HEAVY;
			tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			tParam.eAttackType = EAttackType::ATK_HEAVY;

			pMonster->TakeDamage(tParam);

			// 충돌이 발생하면 플레이어의 키네틱 콤보 상태를 1로 올려준다.
			if (CGameInstance::GetInstance()->GetLayer(LEVEL_NOW, L"Layer_Player") != nullptr)
			{
				for (auto& iter : CGameInstance::GetInstance()->GetLayer(LEVEL_NOW, L"Layer_Player")->GetGameObjects())
				{
					if (L"Player" == iter->GetPrototypeTag())
					{
						static_cast<CPlayer*>(iter)->Set_KineticCombo_Kinetic();
						break;
					}
				}
			}
		}
	});


	return S_OK;
}

void CMapKinetic_Object::BeginTick()
{
	__super::BeginTick();
}

void CMapKinetic_Object::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_pCollider->Update_Tick(m_pTransformCom);
}

void CMapKinetic_Object::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

void CMapKinetic_Object::AfterPhysX()
{
	m_pCollider->Update_AfterPhysX(m_pTransformCom);
}

HRESULT CMapKinetic_Object::Render()
{
	if (m_bVisible == false) return S_OK;

	FAILED_CHECK(__super::Render());

	const _matrix WorldMatrix = m_LocalMatrix * m_pTransformCom->Get_WorldMatrix();

	FAILED_CHECK(m_pModelCom->Render(WorldMatrix));

	return S_OK;
}

void CMapKinetic_Object::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);
	m_strModelTag = s2ws(json["ModelTag"]);
	if (json.contains("KineticType"))
		m_eType = json["KineticType"];
	if (json.contains("LocalMatrix"))
		m_LocalMatrix = json["LocalMatrix"];
}

void CMapKinetic_Object::SaveToJson(Json & json)
{
	__super::SaveToJson(json);
	json["ModelTag"] = ws2s(m_strModelTag);
	json["KineticType"] = m_eType;
	json["LocalMatrix"] = m_LocalMatrix;
}

void CMapKinetic_Object::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	static array<const char*, KT_END> arrTypeName {
		"Normal"
	};
	if (ImGui::BeginCombo("KineticType", arrTypeName[m_eType]))
	{
		for (int i = 0; i < KT_END; ++i)
		{
			const bool bSelected = false;
			if (ImGui::Selectable(arrTypeName[i], bSelected))
				m_eType = static_cast<EKineticType>(i);
		}
		ImGui::EndCombo();
	}

	if (ImGui::CollapsingHeader("Local Matrix Edit"))
	{
		static GUIZMO_INFO tInfo;
		CImguiUtils::Render_Guizmo(&m_LocalMatrix, tInfo, true, true);
	}


	if (ImGui::Button("Kinetic Object Reset"))
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, -30.f, 0.f, 1.f));
		m_pTransformCom->SetTransformDesc({ 1.f, XMConvertToRadians(180.f) });

		m_pCollider->Activate(true);
		m_pCollider->SetPxWorldMatrix(m_pTransformCom->Get_WorldMatrix_f4x4());
		m_pCollider->Set_Kinetic(true);
		m_pCollider->UpdateChange();
	}
}

void CMapKinetic_Object::Add_Physical(_float3 vForce, _float3 vTorque)
{
	m_pCollider->Set_Kinetic(false);
	m_pCollider->UpdateChange();

	if(vForce.Length() > 900.f)
		m_bUsable = false;

	m_pCollider->AddForce(vForce);
	m_pCollider->AddTorque(vTorque);
}

void CMapKinetic_Object::Reset_Transform()
{
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, -30.f, 0.f, 1.f));
	m_pTransformCom->SetTransformDesc({ 1.f, XMConvertToRadians(180.f) });

	m_pCollider->Activate(true);
	m_pCollider->SetPxWorldMatrix(m_pTransformCom->Get_WorldMatrix_f4x4());
	m_pCollider->Set_Kinetic(true);
	m_pCollider->UpdateChange();
}

HRESULT CMapKinetic_Object::SetUp_Components(void* pArg)
{
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, m_strModelTag.c_str(), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom));

	FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", L"Collider", (CComponent**)&m_pCollider, pArg));
	return S_OK;
}

CMapKinetic_Object * CMapKinetic_Object::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMapKinetic_Object*		pInstance = new CMapKinetic_Object(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapKinetic_Object");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMapKinetic_Object::Clone(void * pArg)
{
	CMapKinetic_Object*		pInstance = new CMapKinetic_Object(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapKinetic_Object");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapKinetic_Object::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pCollider);
}
