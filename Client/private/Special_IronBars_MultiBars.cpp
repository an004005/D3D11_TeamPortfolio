#include "stdafx.h"
#include "Special_IronBars_MultiBars.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "GameInstance.h"
#include "Model.h"
#include "RigidBody.h"
#include "MathUtils.h"
#include "Animation.h"
#include "ImguiUtils.h"
#include "Enemy.h"

CSpecial_IronBars_MultiBars::CSpecial_IronBars_MultiBars(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CSpecialObject(pDevice, pContext)
{
}

CSpecial_IronBars_MultiBars::CSpecial_IronBars_MultiBars(const CSpecial_IronBars_MultiBars& rhs)
	: CSpecialObject(rhs)
{
}

HRESULT CSpecial_IronBars_MultiBars::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CSpecial_IronBars_MultiBars::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components(pArg));

	m_pTransformCom->SetTransformDesc({ 0.1f, XMConvertToRadians(90.f) });

	m_eSpecialObejctType = SPECIAL_IRONBARS_MULTIBARS;

	m_pCollider->SetOnTriggerIn([this](CGameObject* pGameObject)
		{
			if (m_bHit)
				return;

			if (auto pTarget = dynamic_cast<CEnemy*>(pGameObject))
			{
				m_bHit = true;
				m_fChangeKineticTimer = 0.f;
			}
		});

	return S_OK;
}

void CSpecial_IronBars_MultiBars::BeginTick()
{
	__super::BeginTick();

	m_pCollider->Set_Kinetic(true);
	m_pCollider->UpdateChange();
}

void CSpecial_IronBars_MultiBars::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_bHit && 0.5f >= m_fChangeKineticTimer)
	{
		m_fChangeKineticTimer += (_float)TimeDelta;

		if (0.5f <= m_fChangeKineticTimer)
		{
			Set_Kinetic(true);
		}
	}

	m_pCollider->Update_Tick(m_pTransformCom);
}

void CSpecial_IronBars_MultiBars::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

void CSpecial_IronBars_MultiBars::AfterPhysX()
{
	__super::AfterPhysX();

	m_pCollider->Update_AfterPhysX(m_pTransformCom);
}

HRESULT CSpecial_IronBars_MultiBars::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

void CSpecial_IronBars_MultiBars::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);

	for (auto tag : json["ModelTags"])
		m_pModelTags.push_back(s2ws(tag));

	if (json.contains("LocalMatrix"))
		m_LocalMatrix = json["LocalMatrix"];
}

void CSpecial_IronBars_MultiBars::SaveToJson(Json & json)
{
	__super::SaveToJson(json);

	for (auto tag : m_pModelTags)
		json["ModelTags"].push_back(ws2s(tag));

	json["LocalMatrix"] = m_LocalMatrix;
}

void CSpecial_IronBars_MultiBars::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	if (ImGui::CollapsingHeader("ChildGizmo"))
	{
		static GUIZMO_INFO Local;
		CImguiUtils::Render_Guizmo(&m_LocalMatrix, Local, true, true);
	}
}

void CSpecial_IronBars_MultiBars::Attach_BoneMatrix(CModel* pModel, CTransform* pTransform, const string& strBoneName)
{
	_matrix	SocketMatrix = XMMatrixInverse(nullptr, m_LocalMatrix) * XMMatrixRotationX(XMConvertToRadians(-90.f)) * pModel->GetBoneMatrix(strBoneName) * pTransform->Get_WorldMatrix();

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	m_pTransformCom->Set_WorldMatrix(SocketMatrix);
}

void CSpecial_IronBars_MultiBars::Attack_BoneMatrix_SetPoint(CModel* pModel, CTransform* pTransform, const string& strBoneName, _float4 vPoint)
{
	if (!m_pCollider->IsKinematic()) Set_Kinetic(true);

	_matrix	SocketMatrix = XMMatrixInverse(nullptr, m_LocalMatrix) 
		* XMMatrixRotationX(XMConvertToRadians(-90.f)) 
		* XMMatrixTranslation(vPoint.x, vPoint.y, vPoint.z)
		* pModel->GetBoneMatrix(strBoneName) 
		* pTransform->Get_WorldMatrix();

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	m_pTransformCom->Set_WorldMatrix(SocketMatrix);
}

void CSpecial_IronBars_MultiBars::Lerp_BoneMatrix(CModel* pModel, CTransform* pTransform, const string& strBoneName, _float4 vPoint, _float fRatio)
{
	if (!m_pCollider->IsKinematic()) Set_Kinetic(true);

	_matrix	SocketMatrix = XMMatrixInverse(nullptr, m_LocalMatrix)
		//* XMMatrixRotationX(XMConvertToRadians(-90.f))
		* XMMatrixTranslation(vPoint.x, vPoint.y, vPoint.z)
		* pModel->GetBoneMatrix(strBoneName)
		* pTransform->Get_WorldMatrix();

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	_float4 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	_float4 vLerpPos = XMVectorLerp(vMyPos, SocketMatrix.r[3], fRatio);
	vLerpPos.w = 1.f;


	_float3 vScale = m_pTransformCom->Get_Scaled();

	_float4 vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	_float4 vLerpLook = XMVectorLerp(vLook, SocketMatrix.r[2], fRatio);
	vLerpLook.w = 0.f;

//	vLerpLook = XMVector3TransformNormal(vLerpLook, XMMatrixRotationY(XMConvtrtToRadians(90.f)));

	_float4 vLerpRight = XMVector3Cross({ 0.f, 1.f, 0.f, 0.f }, vLerpLook);
	_float4 vLerpUp = XMVector3Cross(vLerpLook, vLerpRight);

	vLerpRight = XMVector3Normalize(vLerpRight) * vScale.x;
	vLerpUp = XMVector3Normalize(vLerpUp) * vScale.y;
	vLerpLook = XMVector3Normalize(vLerpLook) * vScale.z;

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vLerpRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vLerpUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLerpLook);

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vLerpPos);
}

void CSpecial_IronBars_MultiBars::Set_Kinetic(_bool bKinetic)
{
	m_pCollider->Set_Kinetic(bKinetic);
	m_pCollider->UpdateChange();
}

void CSpecial_IronBars_MultiBars::Calculate_TargetDir(_float4 vTargetPos, _float fRatio)
{
	_float4 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float3 vDir = XMVector3Normalize(vTargetPos - vMyPos);

	_float3 vScale = m_pTransformCom->Get_Scaled();

	_float4 vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));

	_float4 vLerpRight = XMVectorLerp(vRight, vDir, fRatio);
	vLerpRight.w = 0.f;

	_float4 vLerpLook = XMVector3Cross(vLerpRight, { 0.f, 1.f, 0.f, 0.f });
	_float4 vLerpUp = XMVector3Cross(vLerpLook, vLerpRight);

	vLerpRight = XMVector3Normalize(vLerpRight) * vScale.x;
	vLerpUp = XMVector3Normalize(vLerpUp) * vScale.y;
	vLerpLook = XMVector3Normalize(vLerpLook) * vScale.z;

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vLerpRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vLerpUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLerpLook);
}

void CSpecial_IronBars_MultiBars::Shooting(_float4 vTargetPos)
{
	if (m_pCollider->IsKinematic()) Set_Kinetic(false);
	if (m_bHit)	m_bHit = false;

	_float4 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float3 vDir = { 0.f, 0.f, 0.f };

	if (vTargetPos.Length() == 0.f)
	{
		vDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
	}
	else
	{
		vDir = XMVector3Normalize(vTargetPos - vMyPos);
	}

	vDir *= 50.f;

	m_pCollider->AddVelocity(vDir);
}

void CSpecial_IronBars_MultiBars::Reloading(_float4 vDestPos, _float4 vTargetPos, _float fRatio)
{
	if (!m_pCollider->IsKinematic()) Set_Kinetic(true);

	_float3 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	_float4 vLerpPos = XMVectorLerp(vMyPos, vDestPos, fRatio);
	vLerpPos.w = 1.f;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vLerpPos);

	Calculate_TargetDir(vTargetPos, fRatio);
}

HRESULT CSpecial_IronBars_MultiBars::SetUp_Components(void * pArg)
{
	CModel* pModel = nullptr;
	m_pModelTags.push_back(L"Model_IronBars_MultiBars");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_IronBars_MultiBars", L"Model_IronBars_MultiBars", (CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_RigidBody", L"Collider", (CComponent**)&m_pCollider, pArg));

	return S_OK;
}

CSpecial_IronBars_MultiBars* CSpecial_IronBars_MultiBars::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpecial_IronBars_MultiBars* pInstance = new CSpecial_IronBars_MultiBars(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpecial_IronBars_MultiBars");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CSpecial_IronBars_MultiBars::Clone(void * pArg)
{
	CGameObject* pInstance = new CSpecial_IronBars_MultiBars(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpecial_IronBars_MultiBars");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSpecial_IronBars_MultiBars::Free()
{
	__super::Free();
}
