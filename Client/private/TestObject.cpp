#include "stdafx.h"
#include "..\public\TestObject.h"

#include "GameInstance.h"
#include "ControlledRigidBody.h"

CTestObject::CTestObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTestObject::CTestObject(const CTestObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTestObject::Initialize_Prototype()
{
	return CGameObject::Initialize_Prototype();
}

HRESULT CTestObject::Initialize(void* pArg)
{
	CGameObject::Initialize(pArg);
	/* For.Com_Renderer */
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom));

	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_ControlledRigidBody"), TEXT("Collider"),
		(CComponent**)&m_pBody));

	return S_OK;
}

void CTestObject::Late_Tick(_double TimeDelta)
{
	if (CGameInstance::GetInstance()->KeyPressing(DIK_I))
	{
		_float4 disp = _float4{ 1.f, 0.f, 0.f, 0.f } * 5.f;
		m_pBody->Move(disp, (_float)TimeDelta);
	}
	if (CGameInstance::GetInstance()->KeyPressing(DIK_K))
	{
		_float4 disp = _float4{ -1.f, 0.f, 0.f, 0.f } * 5.f;
		m_pBody->Move(disp, (_float)TimeDelta);
	}
	if (CGameInstance::GetInstance()->KeyPressing(DIK_J))
	{
		_float4 disp = _float4{ 0.f, 0.f, 1.f, 0.f } * 5.f;
		m_pBody->Move(disp, (_float)TimeDelta);
	}
	if (CGameInstance::GetInstance()->KeyPressing(DIK_L))
	{
		_float4 disp = _float4{ 0.f, 0.f, -1.f, 0.f } * 5.f;
		m_pBody->Move(disp, (_float)TimeDelta);
	}

	m_vDown.y += -9.8 * TimeDelta;
	m_pBody->Move(m_vDown, (_float)TimeDelta);
}

void CTestObject::AfterPhysX()
{
	_float fPreY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, m_pBody->GetPosition());
	_float fY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	if (fPreY == fY)
		m_vDown.y = 0.f;
}

HRESULT CTestObject::Render()
{
	return S_OK;
}

CTestObject* CTestObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
 	CTestObject*		pInstance = new CTestObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTestObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTestObject::Clone(void* pArg)
{
	CTestObject*		pInstance = new CTestObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTestObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTestObject::Free()
{
	CGameObject::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBody);
}
