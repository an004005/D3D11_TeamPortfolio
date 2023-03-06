#include "stdafx.h"
#include "..\public\MapKinetic_Object.h"
#include "GameInstance.h"
#include "PhysXDynamicModel.h"
#include "JsonStorage.h"
#include "RigidBody.h"

#include "Monster.h"
#include "Player.h"

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

	FAILED_CHECK(SetUp_Components(pArg));

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, 1.f, 10.f, 0.f));
	m_pTransformCom->SetTransformDesc({ 1.f, XMConvertToRadians(180.f) });

	m_pDynamic_RigidBody->Activate(true);
	m_pDynamic_RigidBody->SetPxWorldMatrix(m_pTransformCom->Get_WorldMatrix_f4x4());

	m_pKinetic_RigidBody->Activate(false);
	m_pKinetic_RigidBody->SetPxWorldMatrix(m_pTransformCom->Get_WorldMatrix_f4x4());
	m_pKinetic_RigidBody->Set_Kinetic(true);

	// 다이나믹 리지드 바디가 몬스터와 충돌했는지?
	m_pDynamic_RigidBody->SetOnTriggerIn([this](CGameObject* pGameObject)
	{
		if (!m_bThrow)
			return;

		if (auto pMonster = dynamic_cast<CMonster*>(pGameObject))
		{
			DAMAGE_PARAM tParam;
			tParam.iDamage = 1;
			tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

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

	// 키네틱 리지드 바디가 몬스터와 충돌했는지?
	m_pKinetic_RigidBody->SetOnTriggerIn([this](CGameObject* pGameObject)
	{
		if (!m_bThrow)
			return;

		if (auto pMonster = dynamic_cast<CMonster*>(pGameObject))
		{
			DAMAGE_PARAM tParam;
			tParam.iDamage = 1;
			tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

			pMonster->TakeDamage(tParam);
		}
	});

	if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		if (json.contains("InitPos"))
		{
			_float4 InitPos = json["InitPos"];
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&InitPos));
		}
	}

	return S_OK;
}

void CMapKinetic_Object::BeginTick()
{
	m_pKinetic_RigidBody->Update_Tick(m_pTransformCom);
	m_pDynamic_RigidBody->Update_Tick(m_pTransformCom);
	m_pKinetic_RigidBody->Update_AfterPhysX(m_pTransformCom);
	m_pDynamic_RigidBody->Update_AfterPhysX(m_pTransformCom);
}

void CMapKinetic_Object::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	if (m_bKinetic)
		m_pKinetic_RigidBody->Update_Tick(m_pTransformCom);
	else
		m_pDynamic_RigidBody->Update_Tick(m_pTransformCom);
}

void CMapKinetic_Object::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

void CMapKinetic_Object::AfterPhysX()
{
	//m_pRigidBody->Update_AfterPhysX(m_pTransformCom);

	if (m_bKinetic)
		m_pKinetic_RigidBody->Update_AfterPhysX(m_pTransformCom);
	else
		m_pDynamic_RigidBody->Update_AfterPhysX(m_pTransformCom);
}

HRESULT CMapKinetic_Object::Render()
{
	if (m_bVisible == false) return S_OK;

	FAILED_CHECK(__super::Render());

	FAILED_CHECK(m_pModelCom->Render(m_pTransformCom));

	return S_OK;
}

void CMapKinetic_Object::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);
	m_strModelTag = s2ws(json["ModelTag"]);
}

void CMapKinetic_Object::SaveToJson(Json & json)
{
	__super::SaveToJson(json);
	json["ModelTag"] = ws2s(m_strModelTag);
}

void CMapKinetic_Object::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	if (ImGui::Button("Kinetic Object Reset"))
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, 1.f, 10.f, 0.f));
		m_pTransformCom->SetTransformDesc({ 1.f, XMConvertToRadians(180.f) });

		m_pDynamic_RigidBody->Activate(true);
		m_pDynamic_RigidBody->SetPxWorldMatrix(m_pTransformCom->Get_WorldMatrix_f4x4());

		m_pKinetic_RigidBody->Activate(false);
		m_pKinetic_RigidBody->SetPxWorldMatrix(m_pTransformCom->Get_WorldMatrix_f4x4());
		m_pKinetic_RigidBody->Set_Kinetic(true);
	}
	//m_pPxModel->SetPxWorldMatrix(m_pTransformCom->Get_WorldMatrix());
}

void CMapKinetic_Object::Add_Physical(_float3 vForce, _float3 vTorque)
{
	//m_pRigidBody->AddForce(vForce);
	//m_pRigidBody->AddTorque(vTorque);

	m_pDynamic_RigidBody->AddForce(vForce);
	m_pDynamic_RigidBody->AddTorque(vTorque);
}

void CMapKinetic_Object::Set_Kinetic(_bool bKinetic)
{
	//m_pRigidBody->Set_Kinetic(is);

	if (bKinetic == m_bKinetic)
		return;

	if (false == bKinetic)
	{
		m_pKinetic_RigidBody->Activate(false);
		m_pDynamic_RigidBody->Activate(true);
		m_pDynamic_RigidBody->SetPxWorldMatrix(m_pTransformCom->Get_WorldMatrix_f4x4());
	}
	else if (true == bKinetic)
	{
		m_pDynamic_RigidBody->Activate(false);
		m_pKinetic_RigidBody->Activate(true);
		m_pKinetic_RigidBody->SetPxWorldMatrix(m_pTransformCom->Get_WorldMatrix_f4x4());
	}

	m_bKinetic = bKinetic;
}

wstring CMapKinetic_Object::MakePxModelProtoTag()
{
	_tchar szDriveName[MAX_PATH]{};
	_tchar szDirName[MAX_PATH]{};
	_tchar szFileName[MAX_PATH]{};
	_tchar szExtName[MAX_PATH]{};
	_wsplitpath_s(m_strModelTag.c_str(), szDriveName, MAX_PATH, szDirName, MAX_PATH, szFileName, MAX_PATH, szExtName, MAX_PATH);

	wstring PxModelTag = szDriveName;
	PxModelTag += szDirName;

	const wstring strFileName = wstring(L"PhysX_") + szFileName;
	PxModelTag += strFileName;
	PxModelTag += szExtName;

	return PxModelTag;
}

HRESULT CMapKinetic_Object::SetUp_Components(void* pArg)
{
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, m_strModelTag.c_str(), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom));

	FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", L"Kinetic", (CComponent**)&m_pKinetic_RigidBody, pArg));
	FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", L"Dynamic", (CComponent**)&m_pDynamic_RigidBody, pArg));

	// todo : 임시로 모든 CMapNonAnim_Object 에 PxModel을 가지도록 설정 추후 수정 바람
	//FAILED_CHECK(__super::Add_Component(LEVEL_NOW, PxModelTag.c_str(), TEXT("Com_PxModel"),
	//	(CComponent**)&m_pPxModel));

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
	Safe_Release(m_pPxModel);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pDynamic_RigidBody);
	Safe_Release(m_pKinetic_RigidBody);
}
