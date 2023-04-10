#include "stdafx.h"
#include "EMGift.h"
#include "GameInstance.h"
#include "ScarletCharacter.h"
#include "Player.h"
#include "PhysX_Manager.h"
#include "Material.h"
CEMGift::CEMGift(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CEMGift::CEMGift(const CEMGift& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEMGift::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());
	m_pModelCom.fill(nullptr);
	return S_OK;
}

HRESULT CEMGift::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));
	m_pModelCom.fill(nullptr);

	SetUpComponents();

	return S_OK;
}

void CEMGift::BeginTick()
{
	__super::BeginTick();
}

void CEMGift::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_pOwner == nullptr) return;

	if (m_DoThrow == false)
	{
		_matrix SocketMatrix = m_pOwner->GetBoneMatrix("HeadTenctacleB9") * m_pOwner->GetTransform()->Get_WorldMatrix();

		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

		m_pTransformCom->Set_WorldMatrix(SocketMatrix);
	}
	else
	{
		if (m_dLife > 0.0)
			m_dLife -= TimeDelta;
		else
			m_bDelete = true;

		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		vPos += XMVector3Normalize(m_vDirection) * 25.f * TimeDelta;
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
		
		DAMAGE_PARAM	dParams;
		dParams.eAttackType = EAttackType::ATK_LIGHT;
		dParams.eDeBuff = m_eGiftBuff;
		dParams.iDamage = 50;
		dParams.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		dParams.pCauser = nullptr;

		if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pOwner))
		{
			if (auto pScarletCharacter = dynamic_cast<CScarletCharacter*>(m_pOwner))
				dParams.pCauser = pScarletCharacter;
		}

		_uint iColType = ECOLLIDER_TYPE_BIT::CTB_PLAYER | ECOLLIDER_TYPE_BIT::CTB_STATIC | ECOLLIDER_TYPE_BIT::CTB_PSYCHICK_OBJ;

		CollisionCheck(m_pTransformCom, dParams, 0.3f, iColType);

		if (m_bHitCheck == true || m_bDelete == true) // hit 체크 및 life 다 떨어진거 체크
		{
			//이펙트 필요하면 넣음
			m_bDelete = true;
		}
	}
}

void CEMGift::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CEMGift::Render()
{
	FAILED_CHECK(__super::Render())

	if(m_eGiftBuff != EDeBuffType::DEBUFF_END)
		m_pModelCom[static_cast<size_t>(m_eGiftBuff)]->Render(m_pTransformCom);

	return S_OK;
}

void CEMGift::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	if (ImGui::Button("Fire"))
		m_eGiftBuff = EDeBuffType::DEBUFF_FIRE;

	if (ImGui::Button("Water"))
		m_eGiftBuff = EDeBuffType::DEBUFF_WATER;

	if (ImGui::Button("Oil"))
		m_eGiftBuff = EDeBuffType::DEBUFF_OIL;

	if (ImGui::Button("Thunder"))
		m_eGiftBuff = EDeBuffType::DEBUFF_THUNDER;
}

void CEMGift::Select_Gift(EDeBuffType eGiftBuff)
{
	m_eGiftBuff = eGiftBuff;

	auto& Materials = m_pModelCom[static_cast<size_t>(m_eGiftBuff)]->GetMaterials();

	for (auto material : Materials)
	{
		material->GetParam().Ints[0] = static_cast<size_t>(m_eGiftBuff);
	}
}

void CEMGift::DoThrow(_fvector vTargetPos)
{
	m_DoThrow = true;
	m_vDirection = vTargetPos - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
}

void CEMGift::SetUpComponents()
{
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom));

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"../Bin/Resources/Model/StaticModel/Monster/SM_Ladder_01b.static_model", L"Model_Fire", 
		(CComponent**)&m_pModelCom[static_cast<size_t>(EDeBuffType::DEBUFF_FIRE)]));

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"../Bin/Resources/Model/StaticModel/Monster/pr_Barrels01_Oil.static_model", L"Model_Oil",
		(CComponent**)&m_pModelCom[static_cast<size_t>(EDeBuffType::DEBUFF_OIL)]));

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"../Bin/Resources/Model/StaticModel/Monster/pr_Barrels01_Water.static_model", L"Model_Water",
		(CComponent**)&m_pModelCom[static_cast<size_t>(EDeBuffType::DEBUFF_WATER)]));

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"../Bin/Resources/Model/StaticModel/Monster/SM_shoppingcart_01.static_model", L"Model_Thunder",
		(CComponent**)&m_pModelCom[static_cast<size_t>(EDeBuffType::DEBUFF_THUNDER)]));


	for (auto model : m_pModelCom)
	{
		for (auto material : model->GetMaterials())
			material->GetParam().iPass = 6;
	}
}

void CEMGift::CollisionCheck(CTransform* pTransform, DAMAGE_PARAM mParam, _float fRad, _uint eColTypes)
{
	_float4 vPos = pTransform->Get_State(CTransform::STATE_TRANSLATION);
	
	if (m_BeforePos.w == -1.f)
	{
		m_BeforePos = vPos;
	}

	physx::PxSweepHit hitBuffer[5];
	physx::PxSweepBuffer sweepOut(hitBuffer, 5);

	SphereSweepParams sParam;
	sParam.sweepOut = &sweepOut;

	_float4	vBulletDir = vPos - m_BeforePos;

	sParam.vUnitDir = _float3(vBulletDir.x, vBulletDir.y, vBulletDir.z);
	sParam.fDistance = sParam.vUnitDir.Length();
	sParam.vUnitDir.Normalize();

	sParam.fRadius = fRad;
	sParam.fVisibleTime = 0.1f;
	sParam.iTargetType = eColTypes;
	sParam.vPos = vPos;

	if (CGameInstance::GetInstance()->SweepSphere(sParam))
	{
		for (int i = 0; i < sweepOut.getNbAnyHits(); ++i)
		{
			auto pTarget = dynamic_cast<CScarletCharacter*>(CPhysXUtils::GetOnwer(sweepOut.getAnyHit(i).actor));

			if (pTarget == nullptr) // 건물, 염력이 적용되는 물체 등에 대한 예외 처리
			{
				m_bHitCheck = true;
				return;
			}
			
			memcpy(&mParam.vHitPosition, &sweepOut.getAnyHit(i).position, sizeof(_float3));
			memcpy(&mParam.vHitNormal, &sweepOut.getAnyHit(i).normal, sizeof(_float3));
			CheckHitTarget(pTarget, CPhysXUtils::GetComponent(sweepOut.getAnyHit(i).actor), mParam);
		}
	}

	m_BeforePos = vPos;
}

void CEMGift::CheckHitTarget(CGameObject * pTarget, CComponent * pHitBox, DAMAGE_PARAM mParam)
{	
	 if (auto pPlayer = dynamic_cast<CPlayer*>(pTarget))
	{
		if (CheckDamagedTarget(pPlayer))
		{
			mParam.pContactComponent = pHitBox;

			auto pHitTarget = dynamic_cast<CScarletCharacter*>(pPlayer);
			pHitTarget->TakeDamage(mParam);

			m_bHitCheck = true;

			ClearDamagedTarget();
		}
		else
			m_bHitCheck = false;
	}	
}

_bool CEMGift::CheckDamagedTarget(CGameObject* pTarget)
{
	const auto itr = m_DamagedTargetList.find(pTarget);
	if (itr == m_DamagedTargetList.end())
	{
		m_DamagedTargetList.insert(pTarget);
		return true;
	}

	return false;
}
void CEMGift::ClearDamagedTarget()
{
	m_DamagedTargetList.clear();
}

CEMGift* CEMGift::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEMGift* pInstance = new CEMGift(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEMGift");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEMGift::Clone(void* pArg)
{
	CEMGift* pInstance = new CEMGift(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEMGift");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEMGift::Free()
{
	__super::Free();
	Safe_Release(m_pRendererCom);

	for (size_t i = 0; i < static_cast<size_t>(EDeBuffType::DEBUFF_END); ++i)
		Safe_Release(m_pModelCom[i]);

}
