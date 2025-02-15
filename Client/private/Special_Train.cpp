#include "stdafx.h"
#include "..\public\Special_Train.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "GameInstance.h"
#include "Model.h"
#include "Animation.h"
#include "RigidBody.h"
#include "VFX_Manager.h"
#include "Material.h"

CSpecial_Train::CSpecial_Train(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CSpecialObject(pDevice, pContext)
{
}

CSpecial_Train::CSpecial_Train(const CSpecial_Train & rhs)
	: CSpecialObject(rhs)
{
}

HRESULT CSpecial_Train::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CSpecial_Train::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components(pArg));

	//m_pModelComs.front()->SetPlayAnimation("AS_mg02_372_train_cap_L");
	m_eSpecialObejctType = SPECIAL_TRAIN;

	//m_pCollider->CallOnTriggerIn([this](CGameObject* pGameObject)
	//{

	//});

	return S_OK;
}

void CSpecial_Train::BeginTick()
{
	__super::BeginTick();

	m_pModelComs.front()->SetPlayAnimation("AS_mg02_372_train_cap_L");
	m_pModelComs.front()->Play_Animation(0.f);

	m_pCollider->UpdateChange();
}

void CSpecial_Train::Tick(_double TimeDelta)
{
	if (m_bDeadCheck)
	{
		SpecialRimLightFix(false);

		m_fDeadTimer -= (_float)TimeDelta;

		if (1.f >= m_fDeadTimer)
		{
			Set_Dissolve(true);
		}

		if (0.f >= m_fDeadTimer)
		{
			this->SetDelete();
		}
	}

	__super::Tick(TimeDelta);

	if ((m_pModelComs.front()->GetPlayAnimation()->GetName() == "AS_mg02_372_train_start_L") && (m_pModelComs.front()->GetPlayAnimation()->IsFinished()))
	{
		m_pModelComs.front()->Reset_SpecialLocalMove();
		m_Effect.Reset();
		Train_Set_Animation("AS_mg02_372_train_end_L");
	}
	else if (m_pModelComs.front()->GetPlayAnimation()->IsFinished())
	{
		m_pModelComs.front()->Reset_SpecialLocalMove();
		m_bActivate = false;
	}

	if (m_bActivate)
	{
		if (m_fLerpTime < m_fDuration)
		{
			m_pModelComs.front()->GetPlayAnimation()->Update_Bones(TimeDelta, EAnimUpdateType::BLEND, m_fLerpTime / m_fDuration);
			m_fLerpTime += (_float)TimeDelta;
		}
		else
		{
			m_pModelComs.front()->GetPlayAnimation()->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL);
		}

		m_pModelComs.front()->Compute_CombindTransformationMatrix();
	}

	// 스파크 이펙트 생성
	if ((m_pModelComs.front()->GetPlayAnimation()->GetName() == "AS_mg02_372_train_start_L") &&
		(0.5f <= m_pModelComs.front()->GetPlayAnimation()->GetPlayRatio()) &&
		m_Effect.IsNotDo())
	{
		Train_SparkEffect_Set();
	}

	if ((m_pModelComs.front()->GetPlayAnimation()->GetName() == "AS_mg02_372_train_end_L") &&
		(0.2f <= m_pModelComs.front()->GetPlayAnimation()->GetPlayRatio()) &&
		m_Effect.IsNotDo())
	{
		Train_SparkEffect_Release();
	}
	// ~스파크 이펙트 생성

	if ((m_pModelComs.front()->GetPlayAnimation()->GetName() == "AS_mg02_372_train_start_L") ||
		(m_pModelComs.front()->GetPlayAnimation()->GetName() == "AS_mg02_372_train_end_L"))
	{
		Train_Collision_On();
	}
	else
	{
		Train_Collision_Off();
	}

	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();
	_float4 vLocal = m_pModelComs.front()->GetSpecialLocalMove(matWorld);
	m_pTransformCom->LocalMove(_float3(vLocal.x, vLocal.y, vLocal.z));
	IM_LOG("%f %f %f", vLocal.x, vLocal.y, vLocal.z);

	m_pCollider->Update_Tick(m_pTransformCom);
}

void CSpecial_Train::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

void CSpecial_Train::AfterPhysX()
{
	__super::AfterPhysX();

	m_pCollider->Update_AfterPhysX(m_pTransformCom);
}

HRESULT CSpecial_Train::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

void CSpecial_Train::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);

	for (auto tag : json["ModelTags"])
		m_pModelTags.push_back(s2ws(tag));

	if (json.contains("LocalMatrix"))
		m_LocalMatrix = json["LocalMatrix"];
}

void CSpecial_Train::SaveToJson(Json & json)
{
	__super::SaveToJson(json);

	for (auto tag : m_pModelTags)
		json["ModelTags"].push_back(ws2s(tag));

	json["LocalMatrix"] = m_LocalMatrix;
}

void CSpecial_Train::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	if (ImGui::CollapsingHeader("Anim Fast Modifier"))
	{
		static string szMonsterAnimName = "";
		if (ImGui::BeginListBox("Animation List"))
		{
			static char szSeachAnim[MAX_PATH] = "";
			ImGui::InputText("Anim Search", szSeachAnim, MAX_PATH);

			const string strSearch = szSeachAnim;
			const _bool bSearch = strSearch.empty() == false;

			for (auto& Pair : m_pModelComs.front()->Get_AnimList())
			{
				if (bSearch)
				{
					if (Pair.first.find(strSearch) == string::npos)
						continue;
				}

				const bool bSelected = szMonsterAnimName == Pair.first;
				if (bSelected)
					ImGui::SetItemDefaultFocus();

				if (ImGui::Selectable(Pair.first.c_str(), bSelected))
					szMonsterAnimName = Pair.first;
			}
			ImGui::EndListBox();
		}

		if ("" != szMonsterAnimName)
			m_pModelComs.front()->Get_AnimList()[szMonsterAnimName]->Imgui_RenderProperty();
	}

}

void CSpecial_Train::Train_Set_Animation(const string & szAnimName)
{
	m_fLerpTime = 0.f;
	m_pModelComs.front()->SetPlayAnimation(szAnimName);
}

void CSpecial_Train::Train_Collision_On()
{
	DAMAGE_PARAM tParam;
	ZeroMemory(&tParam, sizeof(DAMAGE_PARAM));
	tParam.eAttackSAS = ESASType::SAS_END;
	tParam.eAttackType = EAttackType::ATK_SPECIAL_END;
	tParam.eDeBuff = EDeBuffType::DEBUFF_END;
	tParam.eKineticAtkType = EKineticAttackType::KINETIC_ATTACK_DEFAULT;
	tParam.iDamage = 9999;

	if(Collision_Check_Capsule(m_pCollider, tParam, true) && m_Slow.IsNotDo())
		CGameInstance::GetInstance()->SetTimeRatioCurve("HitLack_Heavy");
}

void CSpecial_Train::Train_Collision_Off()
{
	DAMAGE_PARAM tParam;
	ZeroMemory(&tParam, sizeof(DAMAGE_PARAM));

	Collision_Check_Capsule(m_pCollider, tParam, false);
}

void CSpecial_Train::Train_SetDeadTimer()
{
	m_fDeadTimer = 3.f;
	m_bDeadCheck = true;
}

void CSpecial_Train::Train_SparkEffect_Set()
{
	// 위치가 본에 잘 안 붙음 -> 확인 필

	m_pSpark[0] = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, L"Special_G_Train_Break_Particles");
	//m_pSpark[0]->Start_AttachPivot(this, XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixTranslation(1.f, 0.f, -10.f), "M_BehindWheel_02", true, true);
	m_pSpark[0]->Start_Attach_Transform(this, m_pTransformCom,
		XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationZ(XMConvertToRadians(10.f)) * XMMatrixTranslation(1.5f, 0.f, -10.f),
		true, true);

	m_pSpark[1] = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, L"Special_G_Train_Break_Particles");
	m_pSpark[1]->Start_Attach_Transform(this, m_pTransformCom,
		XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationZ(XMConvertToRadians(10.f)) * XMMatrixTranslation(-1.5f, 0.f, -10.f),
		true, true);

	m_pSpark[2] = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, L"Special_G_Train_Break_Particles");
	m_pSpark[2]->Start_Attach_Transform(this, m_pTransformCom,
		XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationZ(XMConvertToRadians(10.f)) * XMMatrixTranslation(1.5f, 0.f, -12.f),
		true, true);

	m_pSpark[3] = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, L"Special_G_Train_Break_Particles");
	m_pSpark[3]->Start_Attach_Transform(this, m_pTransformCom,
		XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationZ(XMConvertToRadians(10.f)) * XMMatrixTranslation(-1.5f, 0.f, -12.f),
		true, true);

	m_pSpark[4] = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, L"Special_G_Train_Break_Particles");
	m_pSpark[4]->Start_Attach_Transform(this, m_pTransformCom,
		XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(10.f)) * XMMatrixTranslation(1.5f, 0.f, -14.f),
		true, true);

	m_pSpark[5] = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, L"Special_G_Train_Break_Particles");
	m_pSpark[5]->Start_Attach_Transform(this, m_pTransformCom,
		XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(10.f)) * XMMatrixTranslation(-1.5f, 0.f, -14.f),
		true, true);

}

void CSpecial_Train::Train_SparkEffect_Release()
{
	for (_uint i = 0; i < 6; ++i)
	{
		if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pSpark[i]))
		{
			m_pSpark[i]->Delete_Particles();
			m_pSpark[i] = nullptr;
		}
	}
}

_float4 CSpecial_Train::GetActionPoint()
{
	_matrix	SocketMatrix = m_pModelComs.front()->GetBoneMatrix("Reference") * m_pTransformCom->Get_WorldMatrix();

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	return SocketMatrix.r[3];
}

HRESULT CSpecial_Train::SetUp_Components(void * pArg)
{
	/*for (_int i = 0; i < m_pModelTags.size(); ++i)
	{
		CModel* pModel = nullptr;
		const wstring comtag = L"Com_Model" + to_wstring(i);
		FAILED_CHECK(__super::Add_Component(LEVEL_NOW, m_pModelTags[i].c_str(), comtag.c_str(),
			(CComponent**)&pModel));

		m_pModelComs.push_back(pModel);
	}*/

	CModel* pModel = nullptr;
	m_pModelTags.push_back(L"Model_Train");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_Train", L"Model_Train", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", L"Collider", (CComponent**)&m_pCollider, pArg));

	//m_eCurModelTag = Tag_default;

	//FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", L"Collider", (CComponent**)&m_pCollider, pArg));

	return S_OK;
}

CSpecial_Train * CSpecial_Train::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpecial_Train* pInstance = new CSpecial_Train(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpecial_Train");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CSpecial_Train::Clone(void * pArg)
{
	CSpecial_Train* pInstance = new CSpecial_Train(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpecial_Train");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSpecial_Train::Free()
{
	__super::Free();
}
