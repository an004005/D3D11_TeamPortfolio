#include "stdafx.h"
#include "..\public\ScarletWeapon.h"
#include "Bone.h"
#include "RigidBody.h"
#include "TrailSystem.h"

CScarletWeapon::CScarletWeapon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CScarletWeapon::CScarletWeapon(const CScarletWeapon & rhs)
	:CGameObject(rhs)
{
}

HRESULT CScarletWeapon::Initialize(void * pArg)
{
	ZeroMemory(&m_Desc, sizeof(WEAPON_DESC));

	if (pArg)
	{
		memcpy(&m_Desc, pArg, sizeof(WEAPON_DESC));
		Safe_AddRef(m_Desc.m_pSocket);
		Safe_AddRef(m_Desc.m_pTransform);
	}

	FAILED_CHECK(CGameObject::Initialize(m_Desc.m_pJson));

	return S_OK;
}

void CScarletWeapon::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND_TOON, this);
}

void CScarletWeapon::Setup_BoneMatrix(CModel* pModel, _fmatrix Transform)
{
	_matrix	SocketMatrix = pModel->GetBoneMatrix("RightWeapon") * Transform;

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	m_pTransformCom->Set_WorldMatrix(SocketMatrix);
}

void CScarletWeapon::Collision_Check()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
}

void CScarletWeapon::Free()
{
	CGameObject::Free();

	if (m_bCloned)
	{
		Safe_Release(m_Desc.m_pSocket);
		Safe_Release(m_Desc.m_pTransform);
	}
	Safe_Release(m_pRenderer);
	Safe_Release(m_pModel);
	Safe_Release(m_pCollider);
	Safe_Release(m_pTrail);
}
