#include "stdafx.h"
#include "..\public\ScarletWeapon.h"
#include "Bone.h"
#include "RigidBody.h"
#include "TrailSystem.h"
#include "Monster.h"

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
		//Safe_AddRef(m_Desc.m_pSocket);
		//Safe_AddRef(m_Desc.m_pTransform);
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

_matrix CScarletWeapon::Get_WeaponCenterMatrix()
{
	_matrix vWeaponCenterMatrix = m_pCollider->GetPxWorldMatrix();

	return vWeaponCenterMatrix;
}

void CScarletWeapon::Setup_BoneMatrix(CModel* pModel, _fmatrix Transform)
{
	_matrix	SocketMatrix = pModel->GetBoneMatrix("RightWeapon") * Transform;

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	m_pTransformCom->Set_WorldMatrix(SocketMatrix);
}

_float4 CScarletWeapon::Get_Edge()
{
	_vector vWeaponCenter = m_pCollider->GetPxWorldMatrix().Translation();
	vWeaponCenter = XMVectorSetW(vWeaponCenter, 1.f);
	_vector vWeaponRight = m_pCollider->GetPxWorldMatrix().Right();

	return vWeaponCenter;// +vWeaponRight;
}

void CScarletWeapon::Set_Bright(ESASType eType, _bool bBright)
{
	m_eSasType = eType;
	m_bBright = bBright;
}

void CScarletWeapon::Free()
{
	CGameObject::Free();

	if (m_bCloned)
	{
		//Safe_Release(m_Desc.m_pSocket);
		//Safe_Release(m_Desc.m_pTransform);
		Safe_Release(m_pTrail);
	}
	Safe_Release(m_pRenderer);
	Safe_Release(m_pModel);
	Safe_Release(m_pCollider);
}
