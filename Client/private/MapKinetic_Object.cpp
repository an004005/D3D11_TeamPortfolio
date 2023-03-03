#include "stdafx.h"
#include "..\public\MapKinetic_Object.h"
#include "GameInstance.h"
#include "PhysXDynamicModel.h"
#include "JsonStorage.h"
#include "RigidBody.h"

CMapKinetic_Object::CMapKinetic_Object(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMapObject(pDevice, pContext)
{
}

CMapKinetic_Object::CMapKinetic_Object(const CMapKinetic_Object & rhs)
	: CMapObject(rhs)
{
}

HRESULT CMapKinetic_Object::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CMapKinetic_Object::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components(pArg));

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, 1.f, 5.f, 0.f));

	if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		if (json.contains("InitPos"))
		{
			_float4 InitPos = json["InitPos"];
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&InitPos));
		}
	}

	return S_OK;
}

void CMapKinetic_Object::BeginTick()
{
}

void CMapKinetic_Object::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	if (pGameInstance->KeyPressing(DIK_K))
	{
		m_pRigidBody->AddForce(_float3(0.f, 100.f, 0.f));
		m_pRigidBody->AddTorque(_float3(1.f, 2.f, 3.f));
	}
	if (pGameInstance->KeyPressing(DIK_J))
	{
		m_pRigidBody->AddForce(_float3(0.f, 0.f, 1000.f));
	}

	m_pRigidBody->Update_Tick(m_pTransformCom);
}

void CMapKinetic_Object::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

void CMapKinetic_Object::AfterPhysX()
{
	m_pRigidBody->Update_AfterPhysX(m_pTransformCom);
}

HRESULT CMapKinetic_Object::Render()
{
	if (m_bVisible == false) return S_OK;

	FAILED_CHECK(__super::Render());

	FAILED_CHECK(m_pModelCom->Render(m_pTransformCom));

	return S_OK;
}

void CMapKinetic_Object::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);
	m_strModelTag = s2ws(json["ModelTag"]);
}

void CMapKinetic_Object::SaveToJson(Json & json)
{
	__super::SaveToJson(json);
	json["ModelTag"] = ws2s(m_strModelTag);
}

void CMapKinetic_Object::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
	//m_pPxModel->SetPxWorldMatrix(m_pTransformCom->Get_WorldMatrix());
}

wstring CMapKinetic_Object::MakePxModelProtoTag()
{
	_tchar szDriveName[MAX_PATH]{};
	_tchar szDirName[MAX_PATH]{};
	_tchar szFileName[MAX_PATH]{};
	_tchar szExtName[MAX_PATH]{};
	_wsplitpath_s(m_strModelTag.c_str(), szDriveName, MAX_PATH, szDirName, MAX_PATH, szFileName, MAX_PATH, szExtName, MAX_PATH);

	wstring PxModelTag = szDriveName;
	PxModelTag += szDirName;

	const wstring strFileName = wstring(L"PhysX_") + szFileName;
	PxModelTag += strFileName;
	PxModelTag += szExtName;

	return PxModelTag;
}

HRESULT CMapKinetic_Object::SetUp_Components(void* pArg)
{
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, m_strModelTag.c_str(), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom));

	FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", L"Collider", (CComponent**)&m_pRigidBody, pArg));

	// todo : 임시로 모든 CMapNonAnim_Object 에 PxModel을 가지도록 설정 추후 수정 바람
	//FAILED_CHECK(__super::Add_Component(LEVEL_NOW, PxModelTag.c_str(), TEXT("Com_PxModel"),
	//	(CComponent**)&m_pPxModel));

	return S_OK;
}

CMapKinetic_Object * CMapKinetic_Object::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMapKinetic_Object*		pInstance = new CMapKinetic_Object(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapKinetic_Object");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMapKinetic_Object::Clone(void * pArg)
{
	CMapKinetic_Object*		pInstance = new CMapKinetic_Object(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapKinetic_Object");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapKinetic_Object::Free()
{
	__super::Free();
	Safe_Release(m_pPxModel);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRigidBody);
}
