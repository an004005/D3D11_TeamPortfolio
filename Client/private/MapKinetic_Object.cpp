#include "stdafx.h"
#include "..\public\MapKinetic_Object.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "RigidBody.h"
#include "Monster.h"
#include "Player.h"
#include "ScarletMap.h"
#include "ImguiUtils.h"
#include "GameUtils.h"
#include "Model.h"
#include "Material.h"
#include "GravikenisisMouseUI.h"

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
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&InitPos));
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
	m_bThrow = false;

	// ���̳��� ������ �ٵ� ���Ϳ� �浹�ߴ���?
	m_pCollider->SetOnTriggerIn([this](CGameObject* pGameObject)
	{
		if (!m_bThrow)
			return;

		if (auto pMonster = dynamic_cast<CMonster*>(pGameObject))
		{
			DAMAGE_PARAM tParam;
			tParam.eAttackType = EAttackType::ATK_HEAVY;
			tParam.iDamage = 200;
			tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

			pMonster->TakeDamage(tParam);
			m_bHit = true;

			// �浹�� �߻��ϸ� �÷��̾��� Ű��ƽ �޺� ���¸� 1�� �÷��ش�.
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

	m_pTransformCom->SetTransformDesc({ 1.f, XMConvertToRadians(18.f) });

	//������ ���� �÷��̾ ������������ üũ
   CGameInstance* pGameInstance = CGameInstance::GetInstance();
   CLayer* pLayer = pGameInstance->GetLayer(LEVEL_NOW, TEXT("Layer_Player"));

   if (pLayer != nullptr)
   {
	   CGravikenisisMouseUI* pGravikenisisMouse = nullptr;
	   pGravikenisisMouse = dynamic_cast<CGravikenisisMouseUI*>(CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_UI"), TEXT("Prototype_GameObject_GravikenisisMouseUI")));

	   assert(pGravikenisisMouse != nullptr);
	   pGravikenisisMouse->Set_Owner(this);
   }

	return S_OK;
}

void CMapKinetic_Object::BeginTick()
{
	__super::BeginTick();

	m_pCollider->UpdateChange();
}

void CMapKinetic_Object::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	OutlineMaker();

	if (m_bThrow)
		m_bUsable = false;

	if (!m_bUsable)
	{
		m_fDeadTimer += (_float)TimeDelta;

		if (m_bHit && m_fDeadTimer >= 1.f)
			this->SetDelete();
		else if (m_fDeadTimer >= 5.f)
			this->SetDelete();
	}

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

	if(m_eCurModelTag != Tag_End)
		FAILED_CHECK(m_pModelComs[m_eCurModelTag]->Render(WorldMatrix));

	return S_OK;
}

void CMapKinetic_Object::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);

	for (auto tag : json["ModelTags"])
		m_pModelTags.push_back(s2ws(tag));
	
	if (json.contains("KineticType"))
		m_eType = json["KineticType"];
	if (json.contains("LocalMatrix"))
		m_LocalMatrix = json["LocalMatrix"];
}

void CMapKinetic_Object::SaveToJson(Json & json)
{
	__super::SaveToJson(json);

	for (auto tag : m_pModelTags)
		json["ModelTags"].push_back(ws2s(tag));

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

	if (ImGui::BeginListBox("ModelTagList"))
	{
		_uint i = 0;
		for (auto tag : m_pModelTags)
		{
			const bool bSelected = (tag == m_strModelTag);

			if (bSelected)
				ImGui::SetItemDefaultFocus();

			char pStr[MAX_PATH];
			strcpy(pStr, CGameUtils::GetFileName(ws2s(tag)).c_str());

			if (ImGui::Selectable(pStr, bSelected))
			{
				m_strModelTag = tag;
				m_eCurModelTag = (KineticModeltag)i;
			}

			++i;
		}
		ImGui::EndListBox();
	}
}

const wstring & CMapKinetic_Object::Get_ModelTag()
{
	if ((size_t)m_eCurModelTag >= m_pModelTags.size())
		return L"";

	return m_pModelTags[m_eCurModelTag];
}

void CMapKinetic_Object::Add_Physical(_float3 vForce, _float3 vTorque)
{
	m_pCollider->Set_Kinetic(false);
	m_pCollider->UpdateChange();

	m_pCollider->AddForce(vForce);
	m_pCollider->AddTorque(vTorque);
}

void CMapKinetic_Object::Set_Kinetic(_bool bKinetic)
{
	m_pCollider->Set_Kinetic(bKinetic);
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

void CMapKinetic_Object::OutlineMaker()
{
	if (m_bOutline != m_bBeforeOutline)
	{
		if (m_bOutline)
		{
			for (auto& Model : m_pModelComs)
			{
				for (auto& iter : Model->GetMaterials())
				{
					if (0 == iter->GetParam().Ints.size())
					{
						iter->GetParam().Ints.push_back(0);
					}
					else
					{
						iter->GetParam().Ints[0] = 1;
					}
				}
			}
		}
		else
		{
			for (auto& Model : m_pModelComs)
			{
				for (auto& iter : Model->GetMaterials())
				{
					if (0 == iter->GetParam().Ints.size())
					{
						iter->GetParam().Ints.push_back(0);
					}
					else
					{
						iter->GetParam().Ints[0] = 0;
					}
				}
			}
		}
	}

	m_bBeforeOutline = m_bOutline;
}

HRESULT CMapKinetic_Object::SetUp_Components(void* pArg)
{
	for (_int i = 0; i < m_pModelTags.size(); ++i)
	{
		CModel* pModel = nullptr;
		const wstring comtag = L"Com_Model" + to_wstring(i);
		FAILED_CHECK(__super::Add_Component(LEVEL_NOW, m_pModelTags[i].c_str(), comtag.c_str(),
			(CComponent**)&pModel));

		m_pModelComs.push_back(pModel);
	}

	m_eCurModelTag = Tag_default;

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

	for(auto Model : m_pModelComs)
		Safe_Release(Model);
	
	Safe_Release(m_pCollider);
}
