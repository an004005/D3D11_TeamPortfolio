#include "stdafx.h"
#include "..\public\EM0221.h"
#include "GameInstance.h"
#include "ImguiUtils.h"
#include "GameUtils.h"
#include "VFX_Manager.h"
#include "Enemy.h"

CEM0221::CEM0221(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice,pContext)
{
}

CEM0221::CEM0221(const CEM0221 & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEM0221::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	SetUpComponents(pArg);

	m_AddPivotMatrix = CImguiUtils::CreateMatrixFromImGuizmoData({ 0.f, 0.f, 0.f }, { 0.f, 0.f, -90.f, }, { 1.f, 1.f, 1.f });
	m_pNormalEffect = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0220_Boom_Normal");
	m_pNormalEffect->Start_NoAttach(this, true);
	Safe_AddRef(m_pNormalEffect);

	return S_OK;
}

void CEM0221::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	Falling(TimeDelta);
}

void CEM0221::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (m_bVisible)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CEM0221::Render()
{
	FAILED_CHECK(__super::Render());

	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

HRESULT CEM0221::Render_ShadowDepth()
{
	m_pModelCom->Render_ShadowDepth(m_pTransformCom);
	return S_OK;
}

void CEM0221::SetUpComponents(void * pArg)
{
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom));

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("Prototype_Model_em221"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom));

	m_SoundStore.CloneSound("wood_attack_lampbomb");
}

void CEM0221::SetUpMatrix(CModel* pModle, const _fmatrix TransformMatrix)
{
	//떨어질때는 tick에서 움직여줘야함
	if (m_bFall == true) return;

	_matrix	 SocketMatrix = XMLoadFloat4x4(&m_AddPivotMatrix) * pModle->GetBoneMatrix(m_szTargetBoneName) * TransformMatrix;

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	m_pTransformCom->Set_WorldMatrix(SocketMatrix);
	 
}

void CEM0221::Set_Fall()
{
	m_bFall = true;
	m_dFallMaxTime = 0.0;

	//Ray쏘는 위치
	XMStoreFloat4(&m_FallStartPosition, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	//떨어지는 이펙트 생성
	m_pFallEffect = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0220_Boom_Explode");
	m_pFallEffect->Start_NoAttach(this, true);
	Safe_AddRef(m_pFallEffect);
}

void CEM0221::Falling(_double TimeDelta)
{
	if (m_bFall == false) return;

	m_dFallMaxTime += TimeDelta;
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vDown = XMVectorSet(0.f, -1.f, 0.f, 0.f);

	m_fFallSpeed += 9.8f * TimeDelta;
	vPos += XMVector3Normalize(vDown) * m_fFallSpeed * TimeDelta;
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);

	Explosion_Ray();
}

void CEM0221::Explosion()
{
	//터지는 이펙트 생성
	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0220_Explode_8x8")->Start_NoAttach(this, false);

	//떨어지는 이펙트 삭제
	if (m_bCloned)
	{
		if (m_pFallEffect != nullptr)
		{
			m_pFallEffect->SetDelete();
			Safe_Release(m_pFallEffect);
			m_pFallEffect = nullptr;
		}
	}

	//데미지 처리

	physx::PxOverlapHit hitBuffer[3];
	physx::PxOverlapBuffer overlapOut(hitBuffer, 3);

	SphereOverlapParams param;
	param.fVisibleTime = 0.5f;
	param.iTargetType = CTB_PLAYER;
	param.fRadius = 2.f;
	param.vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	param.overlapOut = &overlapOut;

	if (CGameInstance::GetInstance()->OverlapSphere(param))
	{
		dynamic_cast<CEnemy*>(m_pOwner)->HitTargets(overlapOut, 40, EAttackType::ATK_HEAVY);
	}

	//사운드
	m_SoundStore.PlaySound("wood_attack_lampbomb", m_pTransformCom);
}

void CEM0221::Explosion_Ray()
{

	if (m_dFallMaxTime >= 0.8)
	{
		m_bFall = false;
		Explosion();
		return;
	}
	_float4 vOrigin;
	_float4 vDir;
	
	vOrigin = m_FallStartPosition;
	vDir = { 0.f, -1.f, 0.f, 0.f };

	physx::PxRaycastHit hitBuffer[1];
	physx::PxRaycastBuffer t(hitBuffer, 1);

	RayCastParams params;
	params.rayOut = &t;
	params.vOrigin = vOrigin;
	params.vDir = vDir;
	params.fDistance = 10.f;
	params.iTargetType = CTB_STATIC;
	params.bSingle = true;

	if (CGameInstance::GetInstance()->RayCast(params))
	{
		for (int i = 0; i < t.getNbAnyHits(); ++i)
		{
			auto p = t.getAnyHit(i);

			_float4 vPos{ p.position.x, p.position.y , p.position.z, 1.f };

			_float fDist = XMVectorGetX(XMVector4Length(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) - XMLoadFloat4(&vPos)));

			if (fDist <= 1.f)
			{
				Explosion();
				m_bFall = false;
				break;
			}
		}

	}
}


CEM0221 * CEM0221::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEM0221* pInstance = new CEM0221(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0221");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEM0221::Clone(void * pArg)
{
	CEM0221*		pInstance = new CEM0221(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0221");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM0221::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);


	if (m_bCloned)
	{
		if (m_pFallEffect != nullptr)
		{
			m_pFallEffect->SetDelete();
			Safe_Release(m_pFallEffect);
			m_pFallEffect = nullptr;
		}

		if (m_pNormalEffect != nullptr)
		{
			m_pNormalEffect->SetDelete();
			Safe_Release(m_pNormalEffect);
			m_pNormalEffect = nullptr;
		}
	}
	
}
