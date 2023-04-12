#include "stdafx.h"
#include "Special_IronBars_MultiBars.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "GameInstance.h"
#include "Model.h"
#include "RigidBody.h"
#include "MathUtils.h"
#include "Animation.h"
#include "ImguiUtils.h"
#include "Enemy.h"
#include "Material.h"
#include "PlayerInfoManager.h"

CSpecial_IronBars_MultiBars::CSpecial_IronBars_MultiBars(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CSpecialObject(pDevice, pContext)
{
}

CSpecial_IronBars_MultiBars::CSpecial_IronBars_MultiBars(const CSpecial_IronBars_MultiBars& rhs)
	: CSpecialObject(rhs)
{
}

HRESULT CSpecial_IronBars_MultiBars::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CSpecial_IronBars_MultiBars::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components(pArg));

	m_pTransformCom->SetTransformDesc({ 0.1f, XMConvertToRadians(90.f) });

	m_eSpecialObejctType = SPECIAL_IRONBARS_MULTIBARS;

	m_pCollider->SetOnTriggerIn([this](CGameObject* pGameObject)
		{
			if (m_bHit)
				return;

			if (auto pTarget = dynamic_cast<CEnemy*>(pGameObject))
			{
				m_bHit = true;
				m_fChangeKineticTimer = 0.f;

				DAMAGE_PARAM tParam;
				ZeroMemory(&tParam, sizeof(DAMAGE_PARAM));
				tParam.eAttackSAS = ESASType::SAS_END;
				tParam.eAttackType = EAttackType::ATK_SPECIAL_LOOP;
				tParam.eDeBuff = EDeBuffType::DEBUFF_END;
				tParam.eKineticAtkType = EKineticAttackType::KINETIC_ATTACK_DEFAULT;
				tParam.iDamage = 200;
				tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

				static_cast<CEnemy*>(pTarget)->TakeDamage(tParam);

				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, L"Special_Iron_Bar_Final_Hit_Effect")
					->Start_AttachOnlyPos(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), false);

				CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, L"Special_G_Iron_Bar_Hit_Particle")
					->Start_AttachPosition(this, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), { 0.f, 1.f, 0.f, 0.f });

				CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.1f, 0.3f);
			}
		});

	return S_OK;
}

void CSpecial_IronBars_MultiBars::BeginTick()
{
	__super::BeginTick();

	m_pCollider->Set_Trigger(true);
	m_pCollider->UpdateChange();
}

void CSpecial_IronBars_MultiBars::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_pCollider->Update_Tick(m_pTransformCom);
}

void CSpecial_IronBars_MultiBars::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

void CSpecial_IronBars_MultiBars::AfterPhysX()
{
	__super::AfterPhysX();

	m_pCollider->Update_AfterPhysX(m_pTransformCom);
}

HRESULT CSpecial_IronBars_MultiBars::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

void CSpecial_IronBars_MultiBars::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);

	for (auto tag : json["ModelTags"])
		m_pModelTags.push_back(s2ws(tag));

	if (json.contains("LocalMatrix"))
		m_LocalMatrix = json["LocalMatrix"];
}

void CSpecial_IronBars_MultiBars::SaveToJson(Json & json)
{
	__super::SaveToJson(json);

	for (auto tag : m_pModelTags)
		json["ModelTags"].push_back(ws2s(tag));

	json["LocalMatrix"] = m_LocalMatrix;
}

void CSpecial_IronBars_MultiBars::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	if (ImGui::CollapsingHeader("ChildGizmo"))
	{
		static GUIZMO_INFO Local;
		CImguiUtils::Render_Guizmo(&m_LocalMatrix, Local, true, true);
	}
}

void CSpecial_IronBars_MultiBars::Attack_BoneMatrix_SetPoint(CModel* pModel, CTransform* pTransform, const string& strBoneName, _float4 vPoint)
{
	if (!m_pCollider->IsTrigger()) Set_Trigger(true);

	_matrix	SocketMatrix = XMMatrixInverse(nullptr, m_LocalMatrix)
		* XMMatrixRotationX(XMConvertToRadians(-90.f))
		* XMMatrixTranslation(vPoint.x, vPoint.y, vPoint.z)
		* pModel->GetBoneMatrix(strBoneName)
		* pTransform->Get_WorldMatrix();

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	m_pTransformCom->Set_WorldMatrix(SocketMatrix);
}

void CSpecial_IronBars_MultiBars::Set_Trigger(_bool bTrigger)
{
	m_pCollider->Set_Trigger(bTrigger);
}

void CSpecial_IronBars_MultiBars::Shooting(_float4 vTargetPos)
{
	if (m_pCollider->IsTrigger()) Set_Trigger(false);
	if (m_bHit)	m_bHit = false;

	_float4 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float3 vDir = { 0.f, 0.f, 0.f };

	if (vTargetPos.Length() == 0.f)
	{
		vDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)) * -1.f;
	}
	else
	{
		vDir = XMVector3Normalize(vTargetPos - vMyPos);
	}

	vDir *= 40.f;

	m_pCollider->AddVelocity(vDir);
}

HRESULT CSpecial_IronBars_MultiBars::SetUp_Components(void * pArg)
{
	CModel* pModel = nullptr;
	m_pModelTags.push_back(L"Model_IronBars_MultiBars");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_IronBars_MultiBars", L"Model_IronBars_MultiBars", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", L"Collider", (CComponent**)&m_pCollider, pArg));

	return S_OK;
}

CSpecial_IronBars_MultiBars* CSpecial_IronBars_MultiBars::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpecial_IronBars_MultiBars* pInstance = new CSpecial_IronBars_MultiBars(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpecial_IronBars_MultiBars");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CSpecial_IronBars_MultiBars::Clone(void * pArg)
{
	CGameObject* pInstance = new CSpecial_IronBars_MultiBars(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpecial_IronBars_MultiBars");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSpecial_IronBars_MultiBars::Free()
{
	__super::Free();
}
