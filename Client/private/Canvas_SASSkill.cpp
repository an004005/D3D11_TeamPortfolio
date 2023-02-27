#include "stdafx.h"
#include "..\public\Canvas_SASSkill.h"
#include "GameInstance.h"
#include "GameUtils.h"

CCanvas_SASSkill::CCanvas_SASSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_SASSkill::CCanvas_SASSkill(const CCanvas_SASSkill& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_SASSkill::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_SASSkill::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	// 처음에 보이지 않을 UI 들
	Find_ChildUI(L"SASSkill_CtrlText1")->SetVisible(false);
	Find_ChildUI(L"SASSkill_CtrlTexture1")->SetVisible(false);

	Find_ChildUI(L"SASSkill_IconNumber1")->SetVisible(false);
	Find_ChildUI(L"SASSkill_IconNumber2")->SetVisible(false);
	Find_ChildUI(L"SASSkill_IconNumber3")->SetVisible(false);
	Find_ChildUI(L"SASSkill_IconNumber4")->SetVisible(false);

	Find_ChildUI(L"SASSkill_AltText1")->SetVisible(false);
	Find_ChildUI(L"SASSkill_AltTexture1")->SetVisible(false);

	Find_ChildUI(L"SASSKill_ColleagueName1")->SetVisible(false);
	Find_ChildUI(L"SASSKill_ColleagueName2")->SetVisible(false);
	Find_ChildUI(L"SASSKill_ColleagueName3")->SetVisible(false);
	Find_ChildUI(L"SASSKill_ColleagueName4")->SetVisible(false);

	Find_ChildUI(L"SASSKill_SuperPower1")->SetVisible(false);
	Find_ChildUI(L"SASSKill_SuperPower2")->SetVisible(false);
	Find_ChildUI(L"SASSKill_SuperPower3")->SetVisible(false);
	Find_ChildUI(L"SASSKill_SuperPower4")->SetVisible(false);

	Find_ChildUI(L"SASSkill_XRight")->SetVisible(false);
	Find_ChildUI(L"SASSkill_XInput")->SetVisible(false);


	return S_OK;
}

void CCanvas_SASSkill::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	InputCtrl_Tick();
	InputAlt_Tick();
	InputX_Tick(TimeDelta);

}

void CCanvas_SASSkill::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
}

void CCanvas_SASSkill::SaveToJson(Json& json)
{
	__super::SaveToJson(json);

}

void CCanvas_SASSkill::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);

}

void CCanvas_SASSkill::Info_Tick(const _bool bPush)
{
	if (true == bPush)
	{
		Find_ChildUI(L"SASSkill_Icon1")->SetVisible(false);
		Find_ChildUI(L"SASSkill_Icon2")->SetVisible(false);
		Find_ChildUI(L"SASSkill_Icon3")->SetVisible(false);
		Find_ChildUI(L"SASSkill_Icon4")->SetVisible(false);

		Find_ChildUI(L"SASSkill_IconNumber1")->SetVisible(true);
		Find_ChildUI(L"SASSkill_IconNumber2")->SetVisible(true);
		Find_ChildUI(L"SASSkill_IconNumber3")->SetVisible(true);
		Find_ChildUI(L"SASSkill_IconNumber4")->SetVisible(true);


	}
	else
	{
		Find_ChildUI(L"SASSkill_Icon1")->SetVisible(true);
		Find_ChildUI(L"SASSkill_Icon2")->SetVisible(true);
		Find_ChildUI(L"SASSkill_Icon3")->SetVisible(true);
		Find_ChildUI(L"SASSkill_Icon4")->SetVisible(true);

		Find_ChildUI(L"SASSkill_IconNumber1")->SetVisible(false);
		Find_ChildUI(L"SASSkill_IconNumber2")->SetVisible(false);
		Find_ChildUI(L"SASSkill_IconNumber3")->SetVisible(false);
		Find_ChildUI(L"SASSkill_IconNumber4")->SetVisible(false);


	}
}

void CCanvas_SASSkill::InputCtrl_Tick()
{
	// Ctrel 눌렀을 때 보여야 하는 UI
	if (CGameInstance::GetInstance()->KeyDown(DIK_LCONTROL))	
	{
		Info_Tick(true);

		Find_ChildUI(L"SASSkill_CtrlText0")->SetVisible(false);
		Find_ChildUI(L"SASSkill_CtrlTexture0")->SetVisible(false);

		Find_ChildUI(L"SASSkill_CtrlText1")->SetVisible(true);
		Find_ChildUI(L"SASSkill_CtrlTexture1")->SetVisible(true);

		Find_ChildUI(L"SASSKill_ColleagueName1")->SetVisible(true);
		Find_ChildUI(L"SASSKill_ColleagueName2")->SetVisible(true);
		Find_ChildUI(L"SASSKill_ColleagueName3")->SetVisible(true);
		Find_ChildUI(L"SASSKill_ColleagueName4")->SetVisible(true);
	}
	// Ctrel 누르지 않을 때 보여야 하는 UI
	if (CGameInstance::GetInstance()->KeyUp(DIK_LCONTROL))
	{
		Info_Tick(false);
		
		Find_ChildUI(L"SASSkill_CtrlText0")->SetVisible(true);
		Find_ChildUI(L"SASSkill_CtrlTexture0")->SetVisible(true);

		Find_ChildUI(L"SASSkill_CtrlText1")->SetVisible(false);
		Find_ChildUI(L"SASSkill_CtrlTexture1")->SetVisible(false);

		Find_ChildUI(L"SASSKill_ColleagueName1")->SetVisible(false);
		Find_ChildUI(L"SASSKill_ColleagueName2")->SetVisible(false);
		Find_ChildUI(L"SASSKill_ColleagueName3")->SetVisible(false);
		Find_ChildUI(L"SASSKill_ColleagueName4")->SetVisible(false);
	}
}

void CCanvas_SASSkill::InputAlt_Tick()
{
	// Alt 눌렀을 때 보여야 하는 UI
	if (CGameInstance::GetInstance()->KeyDown(DIK_LALT))
	{
		Info_Tick(true);

		Find_ChildUI(L"SASSkill_AltText0")->SetVisible(false);
		Find_ChildUI(L"SASSkill_AltTexture0")->SetVisible(false);

		Find_ChildUI(L"SASSkill_AltText1")->SetVisible(true);
		Find_ChildUI(L"SASSkill_AltTexture1")->SetVisible(true);

		Find_ChildUI(L"SASSKill_SuperPower1")->SetVisible(true);
		Find_ChildUI(L"SASSKill_SuperPower2")->SetVisible(true);
		Find_ChildUI(L"SASSKill_SuperPower3")->SetVisible(true);
		Find_ChildUI(L"SASSKill_SuperPower4")->SetVisible(true);
	}
	// Alt 누르지 않을 때 보여야 하는 UI
	if (CGameInstance::GetInstance()->KeyUp(DIK_LALT))
	{
		Info_Tick(false);

		Find_ChildUI(L"SASSkill_AltText0")->SetVisible(true);
		Find_ChildUI(L"SASSkill_AltTexture0")->SetVisible(true);

		Find_ChildUI(L"SASSkill_AltText1")->SetVisible(false);
		Find_ChildUI(L"SASSkill_AltTexture1")->SetVisible(false);

		Find_ChildUI(L"SASSKill_SuperPower1")->SetVisible(false);
		Find_ChildUI(L"SASSKill_SuperPower2")->SetVisible(false);
		Find_ChildUI(L"SASSKill_SuperPower3")->SetVisible(false);
		Find_ChildUI(L"SASSKill_SuperPower4")->SetVisible(false);
	}
}

void CCanvas_SASSkill::InputX_Tick(const _double & dTimeDelta)
{
	// TODO : Text 용 삭제해야 합니당.
	// 만약에 UI 에서 하는 것 이라면 매개변수로 _bool 받아와서 할 것! -> 계속 Tick() 을 돌고 있음 생각하기
	if (CGameInstance::GetInstance()->KeyDown(DIK_X))
	{
		m_bChangeX = !m_bChangeX;
		m_bChangeXButton = true;
	}
	// 요기까지

	if (true == m_bChangeX)	// 오른쪽
	{
		Find_ChildUI(L"SASSkill_XLeft")->SetVisible(false);
		Find_ChildUI(L"SASSkill_XRight")->SetVisible(true);
	}
	else					// 왼쪽
	{
		Find_ChildUI(L"SASSkill_XLeft")->SetVisible(true);
		Find_ChildUI(L"SASSkill_XRight")->SetVisible(false);
	}

	if (true == m_bChangeXButton)
	{
		if (0.0 < m_dChangeX_TimcAcc)
		{
			Find_ChildUI(L"SASSkill_XInput")->SetVisible(true);
		}

		m_dChangeX_TimcAcc += dTimeDelta;

		if (0.2 < m_dChangeX_TimcAcc)
		{
			m_bChangeXButton = false;
			m_dChangeX_TimcAcc = 0.0;
			Find_ChildUI(L"SASSkill_XInput")->SetVisible(false);
		}
	}
}

void CCanvas_SASSkill::SASSkill_UIMove(CTransform * pTransform, const _float2 & vOrigin, const _double & dTimeDelta)
{
	//CGameUtils::GetRandFloat(90.0f, 180.0f);

	//if (false == m_bUIMove)
	//	return;

	if (false == m_bOneCheck)
	{
		m_bOneCheck = true;
		m_bIsDestination = true;
		m_fOriginPosition = { vOrigin.x, vOrigin.y };
		m_fDestination = { vOrigin.x + 3.0f, vOrigin.y - 3.0f };
	}

	static _float fSpeed = 5.0f;

	if (true == m_bIsDestination)
	{
		_vector vPosition = pTransform->Get_State(CTransform::STATE_TRANSLATION);
		
		vPosition += XMVector2Normalize(XMLoadFloat2(&m_fDestination) - vPosition) * _float(dTimeDelta) * fSpeed;

		pTransform->Set_State(CTransform::STATE_TRANSLATION, XMVectorSetW(vPosition, 1.0f));

		// 목표 지점과 현재 지점을 비교한다.
		_vector vDestination = { m_fDestination.x, m_fDestination.y, 0.0f, 1.0f };
		_float fDistance = XMVectorGetX(XMVector3Length(vDestination - vPosition));
		
		if (0.1f > fDistance)
		{
			m_bIsDestination = false;
			m_bIsOriginGoal = true;
		}
	}

	if (true == m_bIsOriginGoal)
	{
		_vector vPosition = pTransform->Get_State(CTransform::STATE_TRANSLATION);

		vPosition += XMVector2Normalize(XMLoadFloat2(&m_fOriginPosition) - vPosition) * _float(dTimeDelta) * fSpeed;
		pTransform->Set_State(CTransform::STATE_TRANSLATION, XMVectorSetW(vPosition, 1.0f));

		// 원래 지점과 현재 지점을 비교한다.
		_vector vDestination = { m_fOriginPosition.x, m_fOriginPosition.y, 0.0f, 1.0f };
		_float fDistance = XMVectorGetX(XMVector3Length(vDestination - vPosition));

		if (0.0f > fDistance)
		{
			m_bIsOriginGoal = false;

			// 모든 값을 초기화 한다.
			m_bUIMove = false;
			m_bOneCheck = false;
			m_bIsDestination = false;
			m_fOriginPosition = { 0.0f, 0.0f };
			m_fDestination = { 0.0f, 0.0f };
		}
	}
}

CCanvas_SASSkill * CCanvas_SASSkill::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_SASSkill*		pInstance = new CCanvas_SASSkill(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_SASSkill");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_SASSkill::Clone(void * pArg)
{
	CCanvas_SASSkill*		pInstance = new CCanvas_SASSkill(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_SASSkill");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_SASSkill::Free()
{
	__super::Free();
}
