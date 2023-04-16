#include "stdafx.h"
#include "EM8200_CopyRush.h"
#include <GameInstance.h>
#include <MathUtils.h>

#include "FSMComponent.h"
#include "Model.h"
#include "VFX_Manager.h"
#include "ScarletCharacter.h"
#include "Material.h"
#include "JsonStorage.h"
#include "EffectSystem.h"
#include "PhysX_Manager.h"
#include "EM8200.h"

CEM8200_CopyRush::CEM8200_CopyRush(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEM8200_CopyRush::CEM8200_CopyRush(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEM8200_CopyRush::Initialize(void* pArg)
{
	FAILED_CHECK(CGameObject::Initialize(pArg));

	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Prototype_Model_em8200", L"Model", (CComponent**)&m_pModelCom));

	m_pASM = CEM8200_CopyRushAnimInstance::Create(m_pModelCom, this);

	Json KarenMask = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/EffectSystem/Monster_Effect/Karen_Mask.json");
	m_pKarenMaskEf = (CEffectSystem*) CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_KarenMask", L"ProtoVFX_EffectSystem", &KarenMask);
	Safe_AddRef(m_pKarenMaskEf);


	m_DefaultTicPerSec = m_pModelCom->Find_Animation("AS_em8200_222_AL_atk_copy_loop")->GetTickPerSec();

	m_TPStart.SetCurve("Simple_Increase_0.2x");
	m_TPStart.SetFinishFunction([this]
	{
		m_bActive = false;
	});
	m_TPEnd.SetCurve("Simple_Decrease_0.2x");

	m_pModelCom->Add_EventCaller("Rush_Start", [this]
	{
		m_bRushStart = true;

		_matrix Pivot = XMMatrixTranslation(0.f, 1.f, -1.f);
		m_pDashEF = CVFX_Manager::GetInstance()->GetEffect(EF_MONSTER, TEXT("em8200_Dash_Attack"));
		m_pDashEF->Start_AttachPivot(this, Pivot, "Reference", true, true);
	});

	m_pModelCom->Add_EventCaller("Rush_End", [this]
	{
		m_bRushStart = false;
	});

	m_SoundStore.CloneSound("karen_fx_dash");



	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.AddTransition("Idle to Start", "Rush_Copy_Start")
				.Predicator([this]
				{
					return m_bActive;
				})

		.AddState("Rush_Copy_Start")
			.OnStart([this]
				{
					m_TPEnd.PlayFromStart();

					m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_221_AL_atk_copy_start");
				})

		.AddTransition("Rush_Copy_Start to Rush_Copy_Loop", "Rush_Copy_Loop")
			.Predicator([this]
				{
					return m_pASM->isSocketEmpty("FullBody");
				})


		// ~ Rush_Copy_Start

		// Rush_Copy_Loop Loop 
		.AddState("Rush_Copy_Loop")
			.OnStart([this]
				{
					m_pModelCom->Find_Animation("AS_em8200_222_AL_atk_copy_loop")->SetTickPerSec(GetRandomTickPerSec());
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_222_AL_atk_copy_loop");
				})

		.AddTransition("Rush_Copy_Loop to Rush_Crouch_Start", "Rush_Crouch_Start")
			.Predicator([this]
				{
					return m_pASM->isSocketEmpty("FullBody");
				})
		// ~Rush_Copy_Loop Loop

		// Rush_Crouch_Start
		.AddState("Rush_Crouch_Start")
			.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_223_AL_atk_crouch_start");
				})

		.AddTransition("Rush_Crouch_Start to Rush_Crouch_Loop", "Rush_Crouch_Loop")
			.Predicator([this]
				{
					return m_pASM->isSocketEmpty("FullBody");
				})
		// ~Rush_Crouch_Start

		// Rush_Crouch_Loop
		.AddState("Rush_Crouch_Loop")
			.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_224_AL_atk_crouch_loop");
				})

		.AddTransition("Rush_Crouch_Loop to Rush_Start", "Rush_Start")
				.Predicator([this]
					{
						return m_pASM->isSocketEmpty("FullBody");
					})
		// ~Rush_Crouch_Loop

		// Rush_Start
		.AddState("Rush_Start")
			.OnStart([this]
				{
					m_SoundStore.PlaySound("karen_fx_dash");
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_225_AL_atk_rush_start");
					m_pTransformCom->LookAt_NonY(m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION));
				})
			.Tick([this](_double TimeDelta)
			{
					if (m_bRushStart == true)
					{
						_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
						
						m_pTransformCom->LocalMove(vLook, 0.5f);
					}
					HitOverlap();

			})
		.AddTransition("Rush_Start to Rush_Loop", "Rush_Loop")
			.Predicator([this]
				{
					return m_pASM->isSocketEmpty("FullBody");
				})

		.AddState("Rush_Loop")
			.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_226_AL_atk_rush_loop");
				})
			.Tick([this](_double TimeDelta)
			{
					if (m_bRushStart == true)
					{
						_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
						m_pTransformCom->LocalMove(vLook, 0.5f);
					}
					HitOverlap();
			})
		.AddTransition("Rush_Loop to Rush_End", "Rush_End")
			.Predicator([this]
				{
					return m_pASM->isSocketEmpty("FullBody");
				})
		// ~Rush_Loop

		// Rush_End
		.AddState("Rush_End")
			.OnStart([this]
			{
				m_TPStart.PlayFromStart();
				m_bRushStart = false;
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_112_AL_brake_dash"); // ·¯½Ã ¾Øµå ¾Ö´Ô ¹Ù²Þ
				if (m_pDashEF != nullptr)
				{
					m_pDashEF->SetDelete();
					m_pDashEF = nullptr;
				}
			})
		.Tick([this](_double TimeDelta)
			{
				_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
				_vector vLocalMove = m_pModelCom->GetLocalMove(WorldMatrix, m_pASM->GetCurSocketAnimName());
				m_pTransformCom->LocalMove(vLocalMove);
			})
		.OnExit([this]
			{
			})
		.AddTransition("Rush_End to Idle", "Idle")
			.Predicator([this]
				{
					return m_pASM->isSocketEmpty("FullBody");
				})

		.Build();


	return S_OK;
}

void CEM8200_CopyRush::Tick(_double TimeDelta)
{
	if (m_bActive == false)
		return;

	CGameObject::Tick(TimeDelta);
	m_pFSM->Tick(TimeDelta);
	m_pASM->Tick(TimeDelta);

	{
		_float fTPStartOut;
		if (m_TPStart.Tick(TimeDelta, fTPStartOut))
		{
			for (auto pMtrl : m_pModelCom->GetMaterials())
			{
				pMtrl->GetParam().Floats[2] = fTPStartOut;
			}
			m_pKarenMaskEf->GetParams().Floats[1] = fTPStartOut;
		}

	}

	{
		_float fTPEndOut;
		if (m_TPEnd.Tick(TimeDelta, fTPEndOut))
		{
			for (auto pMtrl : m_pModelCom->GetMaterials())
			{
				pMtrl->GetParam().Floats[2] = fTPEndOut;

			}
			m_pKarenMaskEf->GetParams().Floats[1] = fTPEndOut;
		}
	}
}

void CEM8200_CopyRush::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);
	if (m_bActive)
	{
		m_pKarenMaskEf->SetVisible(true);
		_matrix	SocketMatrix = m_pModelCom->GetBoneMatrix("FacialRoot") * m_pTransformCom->Get_WorldMatrix();
		m_pKarenMaskEf->GetTransform()->Set_WorldMatrix(SocketMatrix);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND_TOON, this);
	}
	else
	{
		m_pKarenMaskEf->SetVisible(false);
	}
}

HRESULT CEM8200_CopyRush::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CEM8200_CopyRush::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();
}

void CEM8200_CopyRush::StartRush(CEM8200* pOwner, CScarletCharacter* pTarget, _fmatrix WorldMatrix, _uint iDamage)
{
	m_pCastedOwner = pOwner;
	m_pTarget = pTarget;
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
	m_iDamage = iDamage;
	m_bActive = true;
	m_pFSM->SetState("Idle");
	ClearDamagedTarget();
}

_float4x4 CEM8200_CopyRush::GetBoneMatrix(const string& strBoneName, _bool bPivotapply)
{
	if (m_pModelCom->Get_BonePtr(strBoneName) == nullptr)
		return XMMatrixIdentity();

	return m_pModelCom->GetBoneMatrix(strBoneName, bPivotapply);
}

_float4x4 CEM8200_CopyRush::GetPivotMatrix()
{
	if (m_pModelCom == nullptr)
		return XMMatrixIdentity();

	return m_pModelCom->GetPivotMatrix();
}

void CEM8200_CopyRush::HitOverlap()
{
	physx::PxOverlapHit hitBuffer[3];
	physx::PxOverlapBuffer overlapOut(hitBuffer, 3);

	SphereOverlapParams param;
	param.fVisibleTime = 0.1f;
	param.iTargetType = CTB_PLAYER;
	param.fRadius = 2.f;
	param.vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	param.vPos.y += 1.f;
	param.overlapOut = &overlapOut;

	if (CGameInstance::GetInstance()->OverlapSphere(param))
	{
		for (int i = 0; i < overlapOut.getNbAnyHits(); ++i)
		{
			auto pTarget = dynamic_cast<CScarletCharacter*>(CPhysXUtils::GetOnwer(overlapOut.getAnyHit(i).actor));
			if (pTarget == nullptr)
				continue;

			if (CheckDamagedTarget(pTarget))
			{
				DAMAGE_PARAM tDamageParams;
				tDamageParams.iDamage = m_iDamage;
				tDamageParams.eAttackType = EAttackType::ATK_HEAVY;
				tDamageParams.eDeBuff = EDeBuffType::DEBUFF_END;
				tDamageParams.pCauser = m_pCastedOwner;
				tDamageParams.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				tDamageParams.vHitPosition = tDamageParams.vHitFrom;
				pTarget->TakeDamage(tDamageParams);
			}
		}
	}
}

_bool CEM8200_CopyRush::CheckDamagedTarget(CScarletCharacter* pTarget)
{
	const auto itr = m_DamagedTargetList.find(pTarget);
	if (itr == m_DamagedTargetList.end())
	{
		m_DamagedTargetList.insert(pTarget);
		return true;
	}

	return false;
}

void CEM8200_CopyRush::ClearDamagedTarget()
{
	m_DamagedTargetList.clear();
}

_double CEM8200_CopyRush::GetRandomTickPerSec()
{
	return m_DefaultTicPerSec * CMathUtils::RandomFloat(0.3f, 0.5f);
}

CEM8200_CopyRush* CEM8200_CopyRush::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEM8200_CopyRush* pInstance = new CEM8200_CopyRush(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM8200_CopyRush");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEM8200_CopyRush::Clone(void* pArg)
{
	CEM8200_CopyRush* pInstance = new CEM8200_CopyRush(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM8200_CopyRush");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM8200_CopyRush::Free()
{
	CGameObject::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pFSM);
	Safe_Release(m_pASM);
	if (m_pDashEF != nullptr)
	{
		m_pDashEF->SetDelete();
		m_pDashEF = nullptr;
	}
	if (m_pKarenMaskEf != nullptr)
		m_pKarenMaskEf->SetDelete();

	Safe_Release(m_pKarenMaskEf);
}

HRESULT CEM8200_CopyRushAnimInstance::Initialize(CModel* pModel, CGameObject* pGameObject)
{
	FAILED_CHECK(CEnemy_AnimInstance::Initialize(pModel, pGameObject));

	m_pASM_Base = CASMBuilder()
		 .InitState("Idle")
		 .AddState("Idle")
		 .SetAnimation(*m_pModel->Find_Animation("AS_em8200_102_AL_wait02"))

		.Build();

	m_mapAnimSocket.insert({ "FullBody", {} });

	return S_OK;
}

void CEM8200_CopyRushAnimInstance::UpdateTargetState(_double TimeDelta)
{
}

CEM8200_CopyRushAnimInstance* CEM8200_CopyRushAnimInstance::Create(CModel* pModel, CGameObject* pGameObject)
{
	CEM8200_CopyRushAnimInstance* pInstance = new CEM8200_CopyRushAnimInstance();

	if (FAILED(pInstance->Initialize(pModel, pGameObject)))
	{
		MSG_BOX("Failed to Created : CEM8200_CopyRushAnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM8200_CopyRushAnimInstance::Free()
{
	__super::Free();
	Safe_Release(m_pASM_Base);
}
