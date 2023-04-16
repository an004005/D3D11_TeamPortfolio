#include "stdafx.h"
#include "Weapon_Player.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "RigidBody.h"
#include "TrailSystem.h"
#include "Monster.h"
#include "PhysX_Manager.h"
#include "Material.h"
#include "Enemy.h"
#include "PlayerInfoManager.h"

CWeapon_Player::CWeapon_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CScarletWeapon(pDevice, pContext)
{
}

CWeapon_Player::CWeapon_Player(const CWeapon_Player& rhs)
	:CScarletWeapon(rhs)
{
}

HRESULT CWeapon_Player::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CWeapon_Player::Initialize(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components());

	m_pModel = m_pModelComs.front();
	m_eType = WP_0106;

	m_pCollider->SetOnTriggerOut([this](CGameObject* pGameObject)
	{
		if (auto pTarget = dynamic_cast<CEnemy*>(pGameObject))
		{
			pTarget->Set_CollisionDuplicate(false, m_eColtype);
		}
	});

	m_pCollider->UpdateChange();

	Json AttackMesh = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/Trail/PlayerSwordTrail.json");
	m_pTrail = static_cast<CTrailSystem*>(pGameInstance->Clone_GameObject_NoLayer(LEVEL_NOW, TEXT("ProtoVFX_TrailSystem"), &AttackMesh));

	return S_OK;
}

void CWeapon_Player::BeginTick()
{
	//m_fAdaptLength = -0.f;
}

void CWeapon_Player::Tick(_double TimeDelta)
{
	Assert(m_eColtype != ECOPYCOLTYPE::COPYCOL_END);	// 복제충돌타입 반드시 정의

	m_fAdaptLength = -0.7f;

	__super::Tick(TimeDelta);
	m_pCollider->Update_Tick(m_pTransformCom);

	_float4 vCurPos = { m_pCollider->GetPxWorldMatrix().Translation().x, m_pCollider->GetPxWorldMatrix().Translation().y, m_pCollider->GetPxWorldMatrix().Translation().z, 1.f };
	m_vSlashVector = vCurPos - m_vBeforePos;

	wstring strMtrlName = L"";

	switch (m_eType)
	{
		case WP_0106:
			strMtrlName = L"MI_wp0106_SWORD";
			CPlayerInfoManager::GetInstance()->SetWeaponDamage(0.f);
			break;

		case WP_0126:
			strMtrlName = L"MI_wp0126_SWORD";
			CPlayerInfoManager::GetInstance()->SetWeaponDamage(25.f);
			break;

		case WP_0190:
			strMtrlName = L"MI_wp0190_SWORD";
			CPlayerInfoManager::GetInstance()->SetWeaponDamage(50.f);
			break;

		default:
			break;
	}

	if (m_bBright)
	{
		switch (m_eSasType)
		{
			case ESASType::SAS_NOT:
				m_pModel->FindMaterial(strMtrlName.c_str())->GetParam().Ints[0] = 1;
				SetLight({ 0.2f, 0.01f, 0.5f, 1.f });
				break;

			case ESASType::SAS_FIRE:
				m_pModel->FindMaterial(strMtrlName.c_str())->GetParam().Ints[0] = 2;
				SetLight({ 0.139f, 0.035f, 0.033f, 1.f });
				break;

			case ESASType::SAS_ELETRIC:
				m_pModel->FindMaterial(strMtrlName.c_str())->GetParam().Ints[0] = 3;
				SetLight({ 0.48f, 0.5f, 0.07451f, 1.f });
				break;

			default:
				m_pModel->FindMaterial(strMtrlName.c_str())->GetParam().Ints[0] = 0;
				break;
		}
	}
	else
	{
		m_pModel->FindMaterial(strMtrlName.c_str())->GetParam().Ints[0] = 0;
	}

	m_vBeforePos = vCurPos;
}

void CWeapon_Player::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

void CWeapon_Player::AfterPhysX()
{
	m_pCollider->Update_AfterPhysX(m_pTransformCom);
}

HRESULT CWeapon_Player::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pModel->Render(m_pTransformCom);

	if (CPlayerInfoManager::GetInstance()->Get_Copy())
	{
		_float4 vRight = CPlayerInfoManager::GetInstance()->Get_PlayerWorldMatrix().r[0];
		vRight.Normalize();

		//_float4 vLeft = CPla
	}

	return S_OK;
}

void CWeapon_Player::Setup_BoneMatrix(CModel* pModel, _fmatrix Transform, const string& strBoneName)
{
	_matrix	SocketMatrix = pModel->GetBoneMatrix(strBoneName) * Transform;

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	m_pTransformCom->Set_WorldMatrix(SocketMatrix);
}

void CWeapon_Player::Change_Weapon(WEAPONTYPE eType)
{
	if (m_eType == eType) return;
	Assert(eType < WP_END);

	m_eType = eType;
	CPlayerInfoManager::GetInstance()->Set_PlayerWeapon(static_cast<_uint>(m_eType));

	m_pModel = m_pModelComs[eType];
}

void CWeapon_Player::SetLight(_float4 vColor)
{
	_float4 WeaponPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) * m_fAdaptLength);
	WeaponPos.w = 1.f;
	_float4 WeaponLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	WeaponLook.Normalize();
	WeaponLook.w = 0.f;

	CGameInstance::GetInstance()->AddLifeCapsuleLight(0.1f, WeaponPos + WeaponLook, WeaponPos - WeaponLook, 2.f, vColor/*{ 0.545098f, 0.1f, 0.6f, 1.f }*/);
}

HRESULT CWeapon_Player::SetUp_Components()
{
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRenderer));

	CModel* pModel = nullptr;

	// wp0106
	m_pModelTags.push_back(L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_106/wp0106.static_model");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, 
		L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_106/wp0106.static_model", 
		L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_106/wp0106.static_model",
		(CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	// wp0126
	m_pModelTags.push_back(L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_126/wp0126.static_model");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW,
		L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_126/wp0126.static_model",
		L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_126/wp0126.static_model",
		(CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	// wp0190
	m_pModelTags.push_back(L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_190/wp0190.static_model2");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW,
		L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_190/wp0190.static_model2", 
		L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_190/wp0190.static_model2", 
		(CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	if (m_Desc.m_pJson)
	{
		Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", L"Trigger", (CComponent**)&m_pCollider, (void*)m_Desc.m_pJson);
	}

	return S_OK;
}

CWeapon_Player* CWeapon_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Player* pInstance = new CWeapon_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon_Player");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CWeapon_Player::Clone(void* pArg)
{
	CWeapon_Player* pInstance = new CWeapon_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWeapon_Player");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CWeapon_Player::Free()
{
	m_pModel = nullptr;

	__super::Free();

	for (auto& iter : m_pModelComs)
	{
		Safe_Release(iter);
	}
}
