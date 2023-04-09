#include "stdafx.h"
#include "..\public\Special_TelephonePole.h"
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

CSpecial_TelephonePole::CSpecial_TelephonePole(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CSpecialObject(pDevice, pContext)
{
}

CSpecial_TelephonePole::CSpecial_TelephonePole(const CSpecial_TelephonePole & rhs)
	: CSpecialObject(rhs)
{
}

HRESULT CSpecial_TelephonePole::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CSpecial_TelephonePole::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components(pArg));

	m_pTransformCom->SetTransformDesc({ 0.5f, XMConvertToRadians(90.f) });

	m_eSpecialObejctType = SPECIAL_TELEPHONEPOLE;

	m_pCollider->SetOnTriggerOut([this](CGameObject* pGameObject)
	{
		// Capsule Collision Check를 위함
		if (auto pTarget = dynamic_cast<CEnemy*>(pGameObject))
		{
			pTarget->Set_CollisionDuplicate(false);
		}
	});


	return S_OK;
}

void CSpecial_TelephonePole::BeginTick()
{
	__super::BeginTick();

	m_PivotMatrix = XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixTranslation(0.f, 2.f, 0.f);
	// m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(CMathUtils::RandomFloat(-20.f, 20.f)));

	m_pCollider->Set_Trigger(true);
	m_pCollider->UpdateChange();
}

void CSpecial_TelephonePole::Tick(_double TimeDelta)
{
	if (m_bDeadCheck)
	{
		m_bOutline = false;
		SpecialRimLightFix(false);

		m_fDeadTime -= (_float)TimeDelta;

		if (1.f >= m_fDeadTime)
		{
			Set_Dissolve(true);
		}

		if (0.f >= m_fDeadTime)
			this->SetDelete();
	}

	__super::Tick(TimeDelta);

	m_pCollider->Update_Tick(m_pTransformCom);
}

void CSpecial_TelephonePole::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

void CSpecial_TelephonePole::AfterPhysX()
{
	__super::AfterPhysX();

	m_pCollider->Update_AfterPhysX(m_pTransformCom);
}

HRESULT CSpecial_TelephonePole::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

void CSpecial_TelephonePole::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);

	for (auto tag : json["ModelTags"])
		m_pModelTags.push_back(s2ws(tag));

	if (json.contains("LocalMatrix"))
		m_LocalMatrix = json["LocalMatrix"];
}

void CSpecial_TelephonePole::SaveToJson(Json & json)
{
	__super::SaveToJson(json);

	for (auto tag : m_pModelTags)
		json["ModelTags"].push_back(ws2s(tag));

	json["LocalMatrix"] = m_LocalMatrix;
}

void CSpecial_TelephonePole::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	if (ImGui::CollapsingHeader("pivot1"))
	{
		static GUIZMO_INFO tp1;
		CImguiUtils::Render_Guizmo(&pivot1, tp1, true, true);
	}
	if (ImGui::CollapsingHeader("pivot2"))
	{
		static GUIZMO_INFO tp2;
		CImguiUtils::Render_Guizmo(&pivot2, tp2, true, true);
	}
}

void CSpecial_TelephonePole::TelephonePole_Bend(_float4 vPlayerPos, _float fForce)
{
	/*
	전봇대의 위치와 플레이어의 위치를 차연산하여 방향벡터를 구한 뒤
	해당 벡터로 Right벡터를 구한다
	Right벡터로 fForce만큼 회전하게 하여 끌어당겨지는 것처럼 묘사할 것
	-> 회전하는 기준 축은 끝동으로 잡기
	*/
	
	_float4 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4 vLookDir = vPlayerPos - vMyPos;
	vLookDir.w = 0.f;

	_float4 vAxis = XMVector3Cross(_float4(0.f, 1.f, 0.f, 0.f), vLookDir);

	m_pTransformCom->Turn_Fixed(vAxis, XMConvertToRadians(fForce));
}

void CSpecial_TelephonePole::TelephonePole_PullOut(_float4 vPlayerPos, _float fForce)
{
	_float4 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4 vLookDir = vPlayerPos - vMyPos;
	vLookDir.w = 0.f;

	_float4 vAxis = XMVector3Cross(_float4(0.f, 1.f, 0.f, 0.f), vLookDir);

	m_pTransformCom->Turn_Fixed(vAxis, XMConvertToRadians(pow(fForce, 7.f) * 7.f));

	m_pTransformCom->Move(pow(fForce, 10.f), _float3(0.f, 1.f, 0.f));

	if (false == CGameInstance::GetInstance()->Check_ObjectAlive(m_pBrakeParticle))
	{
		m_pBrakeParticle = CVFX_Manager::GetInstance()->GetParticle(PS_SAS, L"Special_G_TelephonePole_Particles");
		m_pBrakeParticle->Start_AttachPosition(this, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + XMVectorSet(0.f, 2.f, 0.f, 0.f), { 0.f, 1.f, 0.f, 0.f }, false);

		m_pBrakeSmoke = CVFX_Manager::GetInstance()->GetParticle(PS_SAS, L"Special_G_TelephonePole_Dark_Smoke");
		m_pBrakeSmoke->Start_AttachPosition(this, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + XMVectorSet(0.f, 2.f, 0.f, 0.f), { 0.f, -1.f, 0.f, 0.f }, false);
	}
	else
	{
		m_pBrakeSmoke->Delete_Particles();
		m_pBrakeParticle->Delete_Particles();
	}
}

void CSpecial_TelephonePole::TelephonePole_AttachLerp(CModel * pModel, CTransform* pTransform, _float fRatio)
{
	if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pBrakeSmoke))
	{
		m_pBrakeSmoke->Delete_Particles();
		m_pBrakeSmoke = nullptr;
	}

	pModel->GetPlayAnimation()->Update_Bones_SyncRatio(0.f);
	pModel->Compute_CombindTransformationMatrix();

	_matrix	SocketMatrix = m_PivotMatrix * pModel->GetBoneMatrix("Waist") * pTransform->Get_WorldMatrix();

	_float fScale_Right = XMVectorGetX(XMVector3Length(m_CatchMatrix.r[0]));
	_float fScale_Up = XMVectorGetX(XMVector3Length(m_CatchMatrix.r[1]));
	_float fScale_Look = XMVectorGetX(XMVector3Length(m_CatchMatrix.r[2]));

	if (1.f < fRatio)
	{
		fRatio = 1.f;
	}

	_vector vSrcScale, vSrcRot, vSrcPos;
	_vector vDstScale, vDstRot, vDstPos;
	XMMatrixDecompose(&vSrcScale, &vSrcRot, &vSrcPos, m_CatchMatrix);
	XMMatrixDecompose(&vDstScale, &vDstRot, &vDstPos, SocketMatrix);

	_vector vLerpScale = XMVectorLerp(vSrcScale, vDstScale, fRatio);
	_vector vLerpRot = XMVectorLerp(vSrcRot, vDstRot, fRatio);
	_vector vLerpTrans = XMVectorLerp(vSrcPos, vDstPos, fRatio);

	vLerpScale = XMVectorSet(fScale_Right, fScale_Up, fScale_Look, 1.f);

	_matrix matLerp = XMMatrixAffineTransformation(vLerpScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vLerpRot, vLerpTrans);

	m_pTransformCom->Set_WorldMatrix(matLerp);
	m_pTransformCom->Set_Scaled(_float3(fScale_Right, fScale_Up, fScale_Look));
}

void CSpecial_TelephonePole::TelephonePole_Swing(CModel * pModel, CTransform* pTransform, _float fPlayTime)
{
	if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pBrakeParticle))
	{
		m_pBrakeParticle->Delete_Particles();
		m_pBrakeParticle = nullptr;
	}

	pModel->GetPlayAnimation()->Update_Bones_SyncRatio(fPlayTime);
	pModel->Compute_CombindTransformationMatrix();

	_matrix	SocketMatrix = m_PivotMatrix * pModel->GetBoneMatrix("Waist") * pTransform->Get_WorldMatrix();

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	m_pTransformCom->Set_WorldMatrix(SocketMatrix);
}

void CSpecial_TelephonePole::TelephonePole_Collision_On()
{
	DAMAGE_PARAM tParam;
	ZeroMemory(&tParam, sizeof(DAMAGE_PARAM));
	tParam.eAttackSAS = ESASType::SAS_END;
	tParam.eAttackType = EAttackType::ATK_SPECIAL_END;
	tParam.eDeBuff = EDeBuffType::DEBUFF_END;
	tParam.eKineticAtkType = EKineticAttackType::KINETIC_ATTACK_DEFAULT;
	tParam.iDamage = 500;

	_bool bCol = Collision_Check_Capsule(m_pCollider, tParam, true);

	if (bCol)
	{
		_float4 vColPos = m_pCollider->GetPxWorldMatrix().Translation();
		vColPos.w = 1.f;

		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, L"Special_G_HBeam")
			->Start_AttachOnlyPos(vColPos, false);

		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, L"Special_G_HBeam_Particles")->
			Start_AttachPosition(this, vColPos, XMVectorSet(0.f, 1.f, 0.f, 0.f), false);

		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, L"Special_G_TelephonePole_BreakMeshes")->
			Start_AttachPosition(this, vColPos, XMVectorSet(0.f, 1.f, 0.f, 0.f), false);
	}
}

void CSpecial_TelephonePole::TelephonePole_Collision_Off()
{
	DAMAGE_PARAM tParam;
	ZeroMemory(&tParam, sizeof(DAMAGE_PARAM));

	Collision_Check_Capsule(m_pCollider, tParam, false);
}

void CSpecial_TelephonePole::TelephonePole_SetDeadTimer()
{
	m_bDeadCheck = true;
	m_fDeadTime = 3.f;
}

void CSpecial_TelephonePole::SetCatchPoint()
{
	m_CatchMatrix = m_pTransformCom->Get_WorldMatrix();
}

void CSpecial_TelephonePole::SetUp_BoneMatrix(CModel * pModel, _fmatrix WorldMatrix)
{
	_matrix	SocketMatrix = pModel->GetBoneMatrix("Eff01") * WorldMatrix;

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	m_pTransformCom->Set_WorldMatrix(SocketMatrix);
}

HRESULT CSpecial_TelephonePole::SetUp_Components(void * pArg)
{
	CModel* pModel = nullptr;
	m_pModelTags.push_back(L"Model_TelephonePole");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_TelephonePole", L"Model_TelephonePole", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", L"Collider", (CComponent**)&m_pCollider, pArg));

	return S_OK;
}

CSpecial_TelephonePole * CSpecial_TelephonePole::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpecial_TelephonePole* pInstance = new CSpecial_TelephonePole(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpecial_TelephonePole");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CSpecial_TelephonePole::Clone(void * pArg)
{
	CSpecial_TelephonePole* pInstance = new CSpecial_TelephonePole(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpecial_TelephonePole");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSpecial_TelephonePole::Free()
{
	__super::Free();
}
