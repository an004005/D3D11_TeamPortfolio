#include "stdafx.h"
#include "AI_CH0300.h"
#include "GameInstance.h"
#include "Shader.h"
#include "AnimationInstance.h"
#include "Animation.h"
#include "FSMComponent.h"
#include "Model.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "ScarletWeapon.h"
#include "RigidBody.h"
#include "EffectSystem.h"
#include "ControlledRigidBody.h"
#include "PhysX_Manager.h"
#include "Material.h"
#include "AI_CH0300_AnimInstance.h"
#include "PlayerInfoManager.h"

#include "Enemy.h"

CAI_CH0300::CAI_CH0300(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CScarletCharacter(pDevice, pContext)
{
}

CAI_CH0300::CAI_CH0300(const CAI_CH0300& rhs)
	:CScarletCharacter(rhs)
{
}

HRESULT CAI_CH0300::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAI_CH0300::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Event()))
		return E_FAIL;

	if (FAILED(SetUp_EffectEvent()))
		return E_FAIL;

	if (FAILED(SetUp_AttackDesc()))
		return E_FAIL;

	if (FAILED(SetUp_Sound()))
		return E_FAIL;

	m_pTransformCom->SetTransformDesc({ 5.f, XMConvertToRadians(180.f) });

	m_pModel->FindMaterial(L"MI_ch0300_HOOD_0")->SetActive(false);

	return S_OK;
}

void CAI_CH0300::BeginTick()
{
	__super::BeginTick();

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	for (auto& iter : pGameInstance->GetLayer(LEVEL_NOW, PLATERTEST_LAYER_PLAYER)->GetGameObjects())
	{
		if (L"Player" == iter->GetPrototypeTag())
		{
			NULL_CHECK(m_pPlayer = iter);
			Safe_AddRef(m_pPlayer);
			break;
		}
	}
}

void CAI_CH0300::Tick(_double TimeDelta)
{
	m_fTimeDelta = TimeDelta;

	__super::Tick(TimeDelta);
	m_pModel->Tick(TimeDelta);

	m_pTransformCom->SetTransformDesc({ 5.f, XMConvertToRadians(360.f) });

	DistanceCheck();

	m_pASM->Tick(TimeDelta);
	BehaviorCheck();
}

void CAI_CH0300::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (m_bVisible && (nullptr != m_pRenderer))
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND_TOON, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);
	}
}

void CAI_CH0300::AfterPhysX()
{
	__super::AfterPhysX();
}

HRESULT CAI_CH0300::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pModel->Render(m_pTransformCom);

	return S_OK;
}

HRESULT CAI_CH0300::Render_ShadowDepth()
{
	m_pModel->Render_ShadowDepth(m_pTransformCom);
	return S_OK;
}

void CAI_CH0300::TakeDamage(DAMAGE_PARAM tDamageParams)
{
}

void CAI_CH0300::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	m_pASM->Imgui_RenderState();

	if (ImGui::CollapsingHeader("Effect_Attach"))
	{
		static string szAnimName = "";
		static _float fEffectPlayTime = 0.f;

		static _float fPrePlayTime = 0.f;
		_float fCurPlayTime = m_pModel->GetPlayAnimation()->GetPlayTime();

		static char szSeachAnim[MAX_PATH] = "";
		ImGui::InputText("Anim Search", szSeachAnim, MAX_PATH);

		if (ImGui::BeginListBox("Animation List"))
		{
			const string strSearch = szSeachAnim;
			const _bool bSearch = strSearch.empty() == false;

			for (auto& Pair : m_pModel->Get_AnimList())
			{
				if (bSearch)
				{
					if (Pair.first.find(strSearch) == string::npos)
						continue;
				}

				const bool bSelected = szAnimName == Pair.first;
				if (bSelected)
					ImGui::SetItemDefaultFocus();

				if (ImGui::Selectable(Pair.first.c_str(), bSelected))
					szAnimName = Pair.first;
			}
			ImGui::EndListBox();
		}

		// ?ㅼ쭏?곸씤 ?댄럺??遺숈씠湲?
		static char cEffectName[MAX_PATH]{};
		static string szEffectName = "";
		ImGui::InputText("Effect Search", cEffectName, MAX_PATH);
		szEffectName = cEffectName;

		static char cBoneName[MAX_PATH]{};
		static string szBoneName = "";
		ImGui::InputText("Bone Search", cBoneName, MAX_PATH);
		szBoneName = cBoneName;

		ImGui::InputFloat("EffectTime", &fEffectPlayTime);

		if ((m_pModel->GetPlayAnimation()->GetName() == szAnimName) &&
			(fPrePlayTime <= fEffectPlayTime) &&
			(fCurPlayTime > fEffectPlayTime))
		{
			//CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ELEC_ATTACK, s2ws(szEffectName), LAYER_PLAYEREFFECT)->Start_AttachPivot(this, pivot1, szBoneName, true, m_bEffectUpdate);
		}

		fPrePlayTime = fCurPlayTime;

		if ("" != szAnimName)
			m_pModel->Get_AnimList()[szAnimName]->Imgui_RenderProperty();
	}
}

_float4x4 CAI_CH0300::GetBoneMatrix(const string& strBoneName, _bool bPivotapply)
{
	if (m_pModel->Get_BonePtr(strBoneName) == nullptr)
		return XMMatrixIdentity();

	return m_pModel->GetBoneMatrix(strBoneName, bPivotapply);
}

_float4x4 CAI_CH0300::GetPivotMatrix()
{
	if (m_pModel == nullptr)
		return XMMatrixIdentity();

	return m_pModel->GetPivotMatrix();
}

HRESULT CAI_CH0300::SetUp_Components(void* pArg)
{
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRenderer));

	if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		if (json.contains("Model"))
		{
			string ProtoModel = json["Model"];
			m_ModelName = CGameUtils::s2ws(ProtoModel);
			FAILED_CHECK(__super::Add_Component(LEVEL_NOW, m_ModelName.c_str(), m_ModelName.c_str(),
				(CComponent**)&m_pModel));
		}
	}

	NULL_CHECK(m_pASM = CAI_CH0300_AnimInstance::Create(m_pModel, this));

	return S_OK;
}

HRESULT CAI_CH0300::SetUp_Event()
{
	return S_OK;
}

HRESULT CAI_CH0300::SetUp_EffectEvent()
{
	return S_OK;
}

HRESULT CAI_CH0300::SetUp_Sound()
{
	return S_OK;
}

HRESULT CAI_CH0300::SetUp_AttackDesc()
{
	return S_OK;
}

void CAI_CH0300::BehaviorCheck()
{
	string strCurState = m_pASM->GetCurStateName();
	_float4 vPlayerPos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);

	if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
	{
		// 타게팅 된 적이 없으면 플레이어 추적
		if ("WALK" == strCurState)
		{
			m_pTransformCom->LookAt_Smooth(vPlayerPos, m_fTimeDelta);
			m_pTransformCom->Go_Straight(m_fTimeDelta);
		}

		if ("RUN" == strCurState)
		{
			m_pTransformCom->LookAt_Smooth(vPlayerPos, m_fTimeDelta);
			m_pTransformCom->Go_Straight(m_fTimeDelta * 2.f);
		}
	}
	else
	{
		// 타게팅 된 적이 있으면 적 추적
		_float4 vEnemyPos = CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
		if ("WALK" == strCurState)
		{
			m_pTransformCom->LookAt_Smooth(vEnemyPos, m_fTimeDelta);
			m_pTransformCom->Go_Straight(m_fTimeDelta);
		}

		if ("RUN" == strCurState)
		{
			m_pTransformCom->LookAt_Smooth(vEnemyPos, m_fTimeDelta);
			m_pTransformCom->Go_Straight(m_fTimeDelta * 2.f);
		}

		if ("ATK_A1" == strCurState)
		{
			m_pTransformCom->LookAt_Smooth(vEnemyPos, m_fTimeDelta);
		}
	}
}

void CAI_CH0300::DistanceCheck()
{
	_float4 vPlayerDistance = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)
		- m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	m_fDistance_toPlayer = vPlayerDistance.Length();

	CGameObject* pEnemy = nullptr;
	if (nullptr != (pEnemy = CPlayerInfoManager::GetInstance()->Get_TargetedMonster()))
	{
		_float4 vPlayerDistance = pEnemy->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)
			- m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

		m_fDistance_toEnemy = vPlayerDistance.Length();
	}
	else
	{
		m_fDistance_toEnemy = -1.f;
	}
}

CAI_CH0300* CAI_CH0300::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAI_CH0300* pInstance = new CAI_CH0300(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAI_CH0300");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CAI_CH0300::Clone(void* pArg)
{
	CAI_CH0300* pInstance = new CAI_CH0300(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAI_CH0300");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CAI_CH0300::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pModel);
	Safe_Release(m_pASM);
	Safe_Release(m_pPlayer);
}
