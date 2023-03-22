#include "stdafx.h"
#include "..\public\Special_TelephonePole.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "GameInstance.h"
#include "Model.h"
#include "RigidBody.h"
#include "MathUtils.h"

CSpecial_TelephonePole::CSpecial_TelephonePole(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CSpecialObject(pDevice, pContext)
{
}

CSpecial_TelephonePole::CSpecial_TelephonePole(const CSpecial_TelephonePole & rhs)
	: CSpecialObject(rhs)
{
}

HRESULT CSpecial_TelephonePole::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CSpecial_TelephonePole::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components(pArg));

	m_pTransformCom->SetTransformDesc({ 0.1f, XMConvertToRadians(90.f) });

	m_eSpecialObejctType = SPECIAL_TELEPHONEPOLE;

	return S_OK;
}

void CSpecial_TelephonePole::BeginTick()
{
	__super::BeginTick();

	m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(CMathUtils::RandomFloat(-20.f, 20.f)));
}

void CSpecial_TelephonePole::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CSpecial_TelephonePole::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

void CSpecial_TelephonePole::AfterPhysX()
{
	__super::AfterPhysX();
}

HRESULT CSpecial_TelephonePole::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

void CSpecial_TelephonePole::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);

	for (auto tag : json["ModelTags"])
		m_pModelTags.push_back(s2ws(tag));

	if (json.contains("LocalMatrix"))
		m_LocalMatrix = json["LocalMatrix"];
}

void CSpecial_TelephonePole::SaveToJson(Json & json)
{
	__super::SaveToJson(json);

	for (auto tag : m_pModelTags)
		json["ModelTags"].push_back(ws2s(tag));

	json["LocalMatrix"] = m_LocalMatrix;
}

void CSpecial_TelephonePole::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
}

void CSpecial_TelephonePole::TelephonePole_Bend(_float4 vPlayerPos, _float fForce)
{
	/*
	전봇대의 위치와 플레이어의 위치를 차연산하여 방향벡터를 구한 뒤
	해당 벡터로 Right벡터를 구한다
	Right벡터로 fForce만큼 회전하게 하여 끌어당겨지는 것처럼 묘사할 것
	-> 회전하는 기준 축은 끝동으로 잡기
	*/
	
	_float4 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float4 vLookDir = vPlayerPos - vMyPos;
	vLookDir.w = 0.f;

	_float4 vAxis = XMVector3Cross(_float4(0.f, 1.f, 0.f, 0.f), vLookDir);

	m_pTransformCom->Turn_Fixed(vAxis, XMConvertToRadians(fForce));
}

void CSpecial_TelephonePole::TelephonePole_PullOut(_float fForce)
{
	m_pTransformCom->Move(fForce, _float3(0.f, 1.f, 0.f));
}

void CSpecial_TelephonePole::SetUp_BoneMatrix(CModel * pModel, _fmatrix WorldMatrix)
{
	_matrix	SocketMatrix = pModel->GetBoneMatrix("Eff01") * WorldMatrix;

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	m_pTransformCom->Set_WorldMatrix(SocketMatrix);
}

HRESULT CSpecial_TelephonePole::SetUp_Components(void * pArg)
{
	CModel* pModel = nullptr;
	m_pModelTags.push_back(L"Model_TelephonePole");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_TelephonePole", L"Model_TelephonePole", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", L"Collider", (CComponent**)&m_pCollider, pArg));

	return S_OK;
}

CSpecial_TelephonePole * CSpecial_TelephonePole::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpecial_TelephonePole* pInstance = new CSpecial_TelephonePole(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpecial_TelephonePole");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CSpecial_TelephonePole::Clone(void * pArg)
{
	CSpecial_TelephonePole* pInstance = new CSpecial_TelephonePole(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpecial_TelephonePole");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSpecial_TelephonePole::Free()
{
	__super::Free();
}
