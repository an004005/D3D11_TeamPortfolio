#include "stdafx.h"
#include "..\public\Player.h"
#include "GameInstance.h"
#include "Shader.h"
#include "AnimationInstance.h"
#include "Animation.h"
#include "FSMComponent.h"
#include "BaseAnimInstance.h"
#include "Model.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "Controller.h"
#include "ScarletWeapon.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CScarletCharacter(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CScarletCharacter(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	if (FAILED(Setup_AnimSocket()))
		return E_FAIL;

	if (FAILED(Setup_Parts()))
		return E_FAIL;

	m_pTransformCom->SetTransformDesc({ 1.f, XMConvertToRadians(720.f) });

	return S_OK;
}

void CPlayer::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_pController->Tick(TimeDelta);

	MoveStateCheck(TimeDelta);
	BehaviorCheck(TimeDelta);

	m_pASM->Tick(TimeDelta);

	if (m_bCanMove)
	{
		_float fSpeedControl = 0.f;

		if (m_bWalk)				fSpeedControl = 0.1f;

		if (m_bCanRun)				fSpeedControl *= 2.f;

//		if (!m_bCanRun && m_bAir)	fSpeedControl *= 0.5f;

		m_pTransformCom->Move(fSpeedControl, m_vMoveDir);
	}

	for (auto& iter : m_vecWeapon)
		iter->Tick(TimeDelta);
}

void CPlayer::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	for (auto& iter : m_vecWeapon)
		iter->Late_Tick(TimeDelta);

	if (m_bVisible && (nullptr != m_pRenderer))
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

void CPlayer::AfterPhysX()
{
	__super::AfterPhysX();

	for (auto& iter : m_vecWeapon)
	{
		static_cast<CScarletWeapon*>(iter)->Setup_BoneMatrix(m_pModel, m_pTransformCom->Get_WorldMatrix());
	}
}

HRESULT CPlayer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pModel->Render(m_pTransformCom);

	return S_OK;
}

void CPlayer::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
	ImGui::SliderFloat("PlayerTurnSpeed", &m_fTurnSpeed, 0.f, 1000.f);
	if (ImGui::Button("TurnAccess"))
	{
		m_pTransformCom->SetTransformDesc({ 1.f, XMConvertToRadians(m_fTurnSpeed) });
	}

	ImGui::SliderFloat("JumpPower", &m_fJumpPower, 0.f, 100.f);
}

HRESULT CPlayer::SetUp_Components(void * pArg)
{
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRenderer));

	if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		if (json.contains("Model"))
		{
			string ProtoModel = json["Model"];
			m_ModelName = CGameUtils::s2ws(ProtoModel);
			FAILED_CHECK(__super::Add_Component(LEVEL_NOW, m_ModelName.c_str(), m_ModelName.c_str(),
				(CComponent**)&m_pModel));
		}
	}

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("Prototype_Component_LocalController"), TEXT("Com_Controller"),
		(CComponent**)&m_pController));
	
	NULL_CHECK(m_pASM = CBaseAnimInstance::Create(m_pModel, this));
	//NULL_CHECK(m_pState = CFSM_Player::Create(this));

	return S_OK;
}

HRESULT CPlayer::SetUp_AttackFSM()
{
	return S_OK;
}

HRESULT CPlayer::Setup_AnimSocket()
{
	CAnimation*	pAnimation = nullptr;

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_011_AL_walk_start_F"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_016_AL_walk"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_018_AL_walk_end"));

	m_TestAnimSocket.push_back(pAnimation = m_pModel->Find_Animation("AS_ch0100_011_AL_walk_start_F"));
	m_TestAnimSocket.push_back(pAnimation = m_pModel->Find_Animation("AS_ch0100_016_AL_walk"));
	m_TestAnimSocket.push_back(pAnimation = m_pModel->Find_Animation("AS_ch0100_018_AL_walk_end"));

	return S_OK;
}

_bool CPlayer::BeforeCharge(_float fBeforeCharge)
{
	if (fBeforeCharge > m_fBefCharge)
	{
		m_fBefCharge += g_fTimeDelta;
		return false;
	}
	else
	{
		m_fBefCharge = 0.f;
		return true;
	}
}

_bool CPlayer::Charge(_uint iNum, _float fCharge)
{
	if (fCharge > m_fCharge[iNum])
	{
		m_fCharge[iNum] += g_fTimeDelta;
		return false;
	}
	else
	{
		IM_LOG(to_string(m_fCharge[iNum]).c_str());
		return true;
	}
}

CPlayer& CPlayer::Reset_Charge()
{
	m_fBefCharge = 0.f;
	for (auto& iter : m_fCharge)
		iter = 0.f;

	return *this;
}

_bool CPlayer::UseSkillCnt(_uint iLimitType)
{
	switch (iLimitType)
	{
	case STACK_NONCHARGE_FLOOR:
		if (0 < m_UseLimit.m_iNonChargeAttack_Floor) { m_UseLimit.m_iNonChargeAttack_Floor -= 1; return true; }
		break;
	case STACK_NONCHARGE_AIR:
		if (0 < m_UseLimit.m_iNonChargeAttack_Air) { m_UseLimit.m_iNonChargeAttack_Air -= 1; return true; }
		break;
	case STACK_ATTACK_AIR01:
		if (0 < m_UseLimit.m_iAttack_Air01) { m_UseLimit.m_iAttack_Air01 -= 1; return true; }
		break;
	case STACK_ATTACK_AIR02:
		if (0 < m_UseLimit.m_iAttack_Air02) { m_UseLimit.m_iAttack_Air02 -= 1; return true; }
		break;
	case STACK_DOUBLEJUMP:
		if (0 < m_UseLimit.m_iDoubleJump) { m_UseLimit.m_iDoubleJump -= 1; return true; }
		break;
	default:
		break;
	}
	return false;
}

CPlayer& CPlayer::UseLimitReset(void)
{
	m_UseLimit.m_iAttack_Air01 = m_UseLimit.MAX_iAttack_Air01;
	m_UseLimit.m_iAttack_Air02 = m_UseLimit.MAX_iAttack_Air02;
	m_UseLimit.m_iDoubleJump = m_UseLimit.MAX_iDoubleJump;
	m_UseLimit.m_iNonChargeAttack_Air = m_UseLimit.MAX_iNonChargeAttack_Air;
	m_UseLimit.m_iNonChargeAttack_Floor = m_UseLimit.MAX_iNonChargeAttack_Floor;

	return *this;
}

void CPlayer::Jump()
{
	m_fYSpeed = m_fJumpPower;
}

void CPlayer::SmoothTurn_Attack(_double TimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Vector4 vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vPlayerPos.w = 1.f;

	Vector4 vCamLook = pGameInstance->Get_CamLook();
	vCamLook.y = 0.f;
	vCamLook.w = 0.f;
	vCamLook.Normalize();

	Vector4 vTarget = vPlayerPos + vCamLook;

	m_pTransformCom->LookAt_Smooth(vTarget, TimeDelta * 0.5f);
}

CPlayer& CPlayer::Reset_SkillCnt()
{
	m_iSkillUsableCnt = 2;
	return *this;
}

CPlayer & CPlayer::Reset_Gravity()
{
	m_fGravity = 0.f;
	return *this;
}

void CPlayer::BehaviorCheck(_double TimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	m_fPlayRatio = 0.f;
	m_bLeftClick = false;
	
	if (nullptr != m_pModel->GetPlayAnimation())
		m_fPlayRatio = m_pModel->GetPlayAnimation()->GetPlayRatio();
	m_bLeftClick = m_pController->KeyDown(CController::MOUSE_LB);
	m_bDash = m_pController->KeyDown(CController::SHIFT);

	m_bNonCharge = m_pController->KeyUp(CController::C);
	m_bCharge = m_pController->KeyPress(CController::C);

	m_bJump = m_pController->KeyDown(CController::SPACE);

	if (m_bCanTurn_Attack)
	{
		SmoothTurn_Attack(TimeDelta);
	}
}

void CPlayer::MoveStateCheck(_double TimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	m_bWalk = false;
	m_eMoveDir = DIR_END;

	m_vMoveDir = m_pController->GetMoveAxis();

	Vector4 vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vPlayerPos.w = 1.f;

	m_vLastDir = vPlayerPos - m_vBefPos;
	m_vLastDir.w = 0.f;
	m_vBefPos = vPlayerPos;

	if (0.f != XMVectorGetX(XMVector3Length(m_vMoveDir)))
	{
		m_bWalk = true;

		Vector3 vPlayerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		vPlayerLook.y = 0.f;
		vPlayerLook.Normalize();

		Vector3 vPlayerRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		vPlayerRight.y = 0.f;
		vPlayerRight.Normalize();

		Matrix matCam = pGameInstance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);
		_vector vScale, vRotate, vTrans;
		XMMatrixDecompose(&vScale, &vRotate, &vTrans, matCam);
		Matrix matCamRot = XMMatrixRotationQuaternion(vRotate);
		m_vMoveDir = XMVector3TransformNormal(m_vMoveDir, matCamRot);
		m_vMoveDir.y = 0.f;
		m_vMoveDir.Normalize();

		_float fAxis_Look = vPlayerLook.Dot(m_vMoveDir);
		_float fAxis_Right = vPlayerRight.Dot(m_vMoveDir);

		if (fabs(fAxis_Look) > fabs(fAxis_Right))
		{
			if (0.f < fAxis_Look) { m_eMoveDir = DIR_F; }
			else { m_eMoveDir = DIR_B; }
		}
		else
		{
			if (0.f < fAxis_Right) { m_eMoveDir = DIR_R; }
			else { m_eMoveDir = DIR_L; }
		}

		if (m_bCanTurn)
			m_pTransformCom->LookAt_Smooth(vPlayerPos + m_vMoveDir, TimeDelta);

		if (m_bAir)	// 공중 상태에선 회전이 먹지 않음
			m_vMoveDir = vPlayerLook;
	}
}

void CPlayer::LookAtDir(Vector3 Vector)
{
	m_pTransformCom->LookAt(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + XMVectorSet(Vector.x, 0.f, Vector.z, 0.f));
}

HRESULT CPlayer::Setup_Parts()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json Weapon;
	Weapon["Model"] = "../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_190/wp0190.static_model";

	CGameObject*	pGameObject = nullptr;

	WEAPON_DESC		Desc;
	ZeroMemory(&Desc, sizeof(WEAPON_DESC));
	Desc.m_PivotMatrix = m_pModel->GetPivotMatrix();
	Desc.m_pSocket = m_pModel->Get_BonePtr("RightWeapon");
	Desc.m_pTransform = m_pTransformCom;
	Desc.m_pJson = &Weapon;

	pGameObject = pGameInstance->Clone_GameObject_Get(TEXT("Layer_Player"), TEXT("PlayerWeapon"), &Desc);
	m_vecWeapon.push_back(pGameObject);

	return S_OK;
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	for (auto& iter : m_vecWeapon)
		Safe_Release(iter);
	m_vecWeapon.clear();

	Safe_Release(m_pFSM);
	Safe_Release(m_pASM);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pModel);
	Safe_Release(m_pController);
}
