#include "stdafx.h"
#include "..\public\MapNonAnim_Object.h"
#include "GameInstance.h"
#include "PhysXStaticModel.h"
#include "JsonStorage.h"

CMapNonAnim_Object::CMapNonAnim_Object(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMapObject(pDevice, pContext)
{
}

CMapNonAnim_Object::CMapNonAnim_Object(const CMapNonAnim_Object & rhs)
	: CMapObject(rhs)
{
}

HRESULT CMapNonAnim_Object::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CMapNonAnim_Object::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components());

	m_pPxModel->SetPxWorldMatrix(m_pTransformCom->Get_WorldMatrix());

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

void CMapNonAnim_Object::BeginTick()
{
}

void CMapNonAnim_Object::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CMapNonAnim_Object::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CMapNonAnim_Object::Render()
{
	if (m_bVisible == false) return S_OK;

	FAILED_CHECK(__super::Render());

	FAILED_CHECK(m_pModelCom->Render(m_pTransformCom));

	return S_OK;
}

void CMapNonAnim_Object::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);
	m_strModelTag = s2ws(json["ModelTag"]);
}

void CMapNonAnim_Object::SaveToJson(Json & json)
{
	__super::SaveToJson(json);
	json["ModelTag"] = ws2s(m_strModelTag);
}


void CMapNonAnim_Object::Imgui_RenderProperty()
{

	// imgui를 켰을 때만 위치 수정가능
	m_pPxModel->SetPxWorldMatrix(m_pTransformCom->Get_WorldMatrix());
}

wstring CMapNonAnim_Object::MakePxModelProtoTag()
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

HRESULT CMapNonAnim_Object::SetUp_Components()
{
	/* For.Com_Model */
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, m_strModelTag.c_str(), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom));

	const wstring PxModelTag = MakePxModelProtoTag();
	if (nullptr == CGameInstance::GetInstance()->Find_Prototype_Component(LEVEL_NOW, PxModelTag.c_str()))
	{
		FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_NOW,
			PxModelTag.c_str(), CPhysXStaticModel::Create(m_pDevice, m_pContext, ws2s(m_strModelTag).c_str())));
	}

	// todo : 임시로 모든 CMapNonAnim_Object 에 PxModel을 가지도록 설정 추후 수정 바람
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, PxModelTag.c_str(), TEXT("Com_PxModel"),
		(CComponent**)&m_pPxModel));

	return S_OK;
}

CMapNonAnim_Object * CMapNonAnim_Object::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMapNonAnim_Object*		pInstance = new CMapNonAnim_Object(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapNonAnim_Object");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMapNonAnim_Object::Clone(void * pArg)
{
	CMapNonAnim_Object*		pInstance = new CMapNonAnim_Object(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapNonAnim_Object");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapNonAnim_Object::Free()
{
	__super::Free();
	Safe_Release(m_pPxModel);
	Safe_Release(m_pModelCom);
}
