#include "stdafx.h"
#include "Special_IronBars_SingleBars.h"
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

CSpecial_IronBars_SingleBars::CSpecial_IronBars_SingleBars(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CSpecialObject(pDevice, pContext)
{
}

CSpecial_IronBars_SingleBars::CSpecial_IronBars_SingleBars(const CSpecial_IronBars_SingleBars& rhs)
	: CSpecialObject(rhs)
{
}

HRESULT CSpecial_IronBars_SingleBars::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CSpecial_IronBars_SingleBars::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components(pArg));

	m_pTransformCom->SetTransformDesc({ 0.1f, XMConvertToRadians(90.f) });

	m_eSpecialObejctType = SPECIAL_IRONBARS_SINGLEBARS;

	m_pCollider->SetOnTriggerIn([this](CGameObject* pGameObject)
		{
			if (m_bHit)
				return;

			if (auto pTarget = dynamic_cast<CEnemy*>(pGameObject))
			{
				if (m_bHit) return;

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

				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, L"Special_G_Iron_Bar_Hit_Effect")
				->Start_AttachOnlyPos(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), false);

				CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, L"Special_G_Iron_Bar_Hit_Particle")
					->Start_AttachPosition(this, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), { 0.f, 1.f, 0.f, 0.f });
			}
		});

	return S_OK;
}

void CSpecial_IronBars_SingleBars::BeginTick()
{
	__super::BeginTick();

	m_pCollider->Set_Trigger(true);
	m_pCollider->UpdateChange();
}

void CSpecial_IronBars_SingleBars::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_bHit && 0.5f >= m_fChangeKineticTimer)
	{
		m_fChangeKineticTimer += (_float)TimeDelta;

		if (0.5f <= m_fChangeKineticTimer)
		{
			Set_Trigger(true);
		}
	}

	m_pCollider->Update_Tick(m_pTransformCom);
}

void CSpecial_IronBars_SingleBars::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

void CSpecial_IronBars_SingleBars::AfterPhysX()
{
	__super::AfterPhysX();

	m_pCollider->Update_AfterPhysX(m_pTransformCom);
}

HRESULT CSpecial_IronBars_SingleBars::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

void CSpecial_IronBars_SingleBars::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);

	for (auto tag : json["ModelTags"])
		m_pModelTags.push_back(s2ws(tag));

	if (json.contains("LocalMatrix"))
		m_LocalMatrix = json["LocalMatrix"];
}

void CSpecial_IronBars_SingleBars::SaveToJson(Json & json)
{
	__super::SaveToJson(json);

	for (auto tag : m_pModelTags)
		json["ModelTags"].push_back(ws2s(tag));

	json["LocalMatrix"] = m_LocalMatrix;
}

void CSpecial_IronBars_SingleBars::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	if (ImGui::CollapsingHeader("ChildGizmo"))
	{
		static GUIZMO_INFO Local;
		CImguiUtils::Render_Guizmo(&m_LocalMatrix, Local, true, true);
	}
}

void CSpecial_IronBars_SingleBars::Attach_BoneMatrix(CModel* pModel, CTransform* pTransform, const string& strBoneName)
{
	_matrix	SocketMatrix = XMMatrixInverse(nullptr, m_LocalMatrix) * XMMatrixRotationX(XMConvertToRadians(-90.f)) * pModel->GetBoneMatrix(strBoneName) * pTransform->Get_WorldMatrix();

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	m_pTransformCom->Set_WorldMatrix(SocketMatrix);
}

void CSpecial_IronBars_SingleBars::Attack_BoneMatrix_SetPoint(CModel* pModel, CTransform* pTransform, const string& strBoneName, _float4 vPoint)
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

void CSpecial_IronBars_SingleBars::Lerp_BoneMatrix(CModel* pModel, CTransform* pTransform, const string& strBoneName, _float4 vPoint, _float fRatio)
{
	if (!m_pCollider->IsTrigger()) Set_Trigger(true);

	_matrix	SocketMatrix = XMMatrixInverse(nullptr, m_LocalMatrix)
		//* XMMatrixRotationX(XMConvertToRadians(-90.f))
		* XMMatrixTranslation(vPoint.x, vPoint.y, vPoint.z)
		* pModel->GetBoneMatrix(strBoneName)
		* pTransform->Get_WorldMatrix();

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	_float4 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	_float4 vLerpPos = XMVectorLerp(vMyPos, SocketMatrix.r[3], fRatio);
	vLerpPos.w = 1.f;


	_float3 vScale = m_pTransformCom->Get_Scaled();

	_float4 vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	_float4 vLerpLook = XMVectorLerp(vLook, SocketMatrix.r[2], fRatio);
	vLerpLook.w = 0.f;

//	vLerpLook = XMVector3TransformNormal(vLerpLook, XMMatrixRotationY(XMConvtrtToRadians(90.f)));

	_float4 vLerpRight = XMVector3Cross({ 0.f, 1.f, 0.f, 0.f }, vLerpLook);
	_float4 vLerpUp = XMVector3Cross(vLerpLook, vLerpRight);

	vLerpRight = XMVector3Normalize(vLerpRight) * vScale.x;
	vLerpUp = XMVector3Normalize(vLerpUp) * vScale.y;
	vLerpLook = XMVector3Normalize(vLerpLook) * vScale.z;

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vLerpRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vLerpUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLerpLook);

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vLerpPos);
}

void CSpecial_IronBars_SingleBars::Set_Trigger(_bool bTrigger)
{
	m_pCollider->Set_Trigger(bTrigger);
}

void CSpecial_IronBars_SingleBars::Calculate_TargetDir(_float4 vTargetPos, _float fRatio)
{
	_float4 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float3 vDir = XMVector3Normalize(vTargetPos - vMyPos);

	_float3 vScale = m_pTransformCom->Get_Scaled();

	_float4 vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));

	_float4 vLerpRight = XMVectorLerp(vRight, vDir, fRatio);
	vLerpRight.w = 0.f;

	_float4 vLerpLook = XMVector3Cross(vLerpRight, { 0.f, 1.f, 0.f, 0.f });
	_float4 vLerpUp = XMVector3Cross(vLerpLook, vLerpRight);

	vLerpRight = XMVector3Normalize(vLerpRight) * vScale.x;
	vLerpUp = XMVector3Normalize(vLerpUp) * vScale.y;
	vLerpLook = XMVector3Normalize(vLerpLook) * vScale.z;

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vLerpRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vLerpUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLerpLook);
}

void CSpecial_IronBars_SingleBars::Shooting(_float4 vTargetPos)
{
	if (m_pCollider->IsTrigger()) Set_Trigger(false);
	if (m_bHit)	m_bHit = false;

	_float4 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float3 vDir = { 0.f, 0.f, 0.f };

	if (vTargetPos.Length() == 0.f)
	{
		vDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
	}
	else
	{
		vDir = XMVector3Normalize(vTargetPos - vMyPos);
	}

	vDir *= 50.f;

	m_pCollider->AddVelocity(vDir);
}

void CSpecial_IronBars_SingleBars::Reloading(_float4 vDestPos, _float4 vTargetPos, _float fRatio)
{
	if (!m_pCollider->IsTrigger()) Set_Trigger(true);

	_float3 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	_float4 vLerpPos = XMVectorLerp(vMyPos, vDestPos, fRatio);
	vLerpPos.w = 1.f;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vLerpPos);

	Calculate_TargetDir(vTargetPos, fRatio);
}

void CSpecial_IronBars_SingleBars::DecomposeEffect()
{
	CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, L"Special_G_Iron_Bar_Particles")
		->Start_AttachPosition(this, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), { 0.f, 1.f, 0.f, 0.f });
}

void CSpecial_IronBars_SingleBars::Activate(_bool bActivate)
{
	m_pCollider->Activate(bActivate);
	m_pCollider->UpdateChange();
}

HRESULT CSpecial_IronBars_SingleBars::SetUp_Components(void * pArg)
{
	CModel* pModel = nullptr;
	m_pModelTags.push_back(L"Model_IronBars_SingleBars");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_IronBars_SingleBars", L"Model_IronBars_SingleBars", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", L"Collider", (CComponent**)&m_pCollider, pArg));

	return S_OK;
}

CSpecial_IronBars_SingleBars* CSpecial_IronBars_SingleBars::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpecial_IronBars_SingleBars* pInstance = new CSpecial_IronBars_SingleBars(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpecial_IronBars_SingleBars");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CSpecial_IronBars_SingleBars::Clone(void * pArg)
{
	CGameObject* pInstance = new CSpecial_IronBars_SingleBars(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpecial_IronBars_SingleBars");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSpecial_IronBars_SingleBars::Free()
{
	__super::Free();
}
