#include "stdafx.h"
#include "..\public\EM0221.h"
#include "GameInstance.h"
#include "ImguiUtils.h"
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
}

void CEM0221::SetUpMatrix(CModel* pModle, const _fmatrix TransformMatrix)
{
	//떨어질때는 tick에서 움직여줘야함
	if (m_bFall == true) return;

	_matrix	SocketMatrix = XMLoadFloat4x4(&m_AddPivotMatrix) * pModle->GetBoneMatrix(m_szTargetBoneName) * TransformMatrix;

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	m_pTransformCom->Set_WorldMatrix(SocketMatrix);
}

void CEM0221::Falling(_double TimeDelta)
{
	if (m_bFall == false) return;

	if (m_bTime > 0.3)
		m_bFall = false;

	m_bTime += TimeDelta;

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vDown = XMVectorSet(0.f, -1.f, 0.f, 0.f);

	m_fFallSpeed += 9.8f * TimeDelta;
	vPos += XMVector3Normalize(vDown) * m_fFallSpeed * TimeDelta;
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
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
}
