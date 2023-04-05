#include "stdafx.h"
#include "..\public\Canvas_Quest.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "DefaultUI.h"
#include "ShaderUI.h"

CCanvas_Quest::CCanvas_Quest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_Quest::CCanvas_Quest(const CCanvas_Quest& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_Quest::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_Quest::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	CUI_Manager::GetInstance()->Add_Canvas(L"CCanvas_Quest", this);
	
	Quest_Initialize();

	m_bVisible = true;
	m_fBackGround_StartPos = Find_ChildUI(L"BackGround")->Get_Position();
	Find_ChildUI(L"BackGround")->Set_Position(_float2(-500.0f, m_fBackGround_StartPos.y));

	return S_OK;
}

void CCanvas_Quest::Tick(_double TimeDelta)
{
	if (CGameInstance::GetInstance()->KeyDown(DIK_Q))
	{
		m_fQuestMove = !m_fQuestMove;
		m_bVisible = true;
	}

	if (!m_bVisible) return;

	CCanvas::Tick(TimeDelta);

	Move_Tick(TimeDelta);

}

HRESULT CCanvas_Quest::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	_float2 vFontSize = { 0.5f, 0.5f };
	_float4 vColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	_float2 vPosition = Find_ChildUI(L"ChakeBase")->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", m_vecQuestInfo[m_iQuestIndex].wsQuest0.c_str(), vPosition + _float2(50.0f, 7.0f), 0.f, vFontSize, vColor);
	//pGameInstance->Render_Font(L"Pretendard32", m_vecQuestInfo[m_iQuestIndex].wsQuest1.c_str(), vPosition + _float2(20.0f, -20.0f), 0.f, vFontSize, vColor);

	return S_OK;
}

void CCanvas_Quest::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	if (ImGui::Button("Set_Quest"))
	{
		Set_Quest(0);
	}
	ImGui::SameLine();
	if (ImGui::Button("Set_Quest"))
	{
		Set_SuccessQuest();
	}
}

void CCanvas_Quest::Quest_Initialize()
{
	QUESTINFO tQuestInfo;

	tQuestInfo.wsQuest0 = L"병원에서 나오미의 약을 찾아 중대장에게 가기.";
	tQuestInfo.wsQuest1 = L"어쩌고를 하세요.";
	m_vecQuestInfo.push_back(tQuestInfo);

}

void CCanvas_Quest::Move_Tick(const _double& TimeDelta)
{
	_float2 vPosition = Find_ChildUI(L"BackGround")->Get_Position();
	Find_ChildUI(L"ChakeBase")->Set_Position(_float2(vPosition.x - 100.0f, vPosition.y));
	Find_ChildUI(L"Chake")->Set_Position(_float2(vPosition.x + 130.0f, vPosition.y));

	_float fSpeed = 500.0f;
	if (true == m_fQuestMove)
	{
		if (m_fBackGround_StartPos.x > vPosition.x)
		{
			vPosition.x += _float(TimeDelta) * fSpeed;
			Find_ChildUI(L"BackGround")->Set_Position(_float2(vPosition.x, m_fBackGround_StartPos.y));
		}
		else
		{
			Find_ChildUI(L"BackGround")->Set_Position(m_fBackGround_StartPos);

			m_dMove_TimeAcc += TimeDelta;
			if (m_fSuccessQuest)
			{
				Success(TimeDelta);

				if (3.5 < m_dMove_TimeAcc)
				{
					m_dMove_TimeAcc = 0.0;
					m_fQuestMove = false;
				}
			}
			else
			{
				if (3.0 < m_dMove_TimeAcc)
				{
					m_dMove_TimeAcc = 0.0;
					m_fQuestMove = false;
				}
			}
		}
	}
	else
	{
		if (-500.0f <= vPosition.x)
		{
			vPosition.x -= _float(TimeDelta) * fSpeed;
			Find_ChildUI(L"BackGround")->Set_Position(_float2(vPosition.x, m_fBackGround_StartPos.y));
		}
		else
		{
			m_bVisible = false;
			Find_ChildUI(L"BackGround")->Set_Position(_float2(-500.0f, m_fBackGround_StartPos.y));

			if (m_fSuccessQuest)
				CGameObject::SetDelete();
		}
	}
}

void CCanvas_Quest::Success(const _double& TimeDelta)
{
	_float fRatio = dynamic_cast<CShaderUI*>(Find_ChildUI(L"Chake"))->Get_Floats0();
	fRatio += _float(TimeDelta) * 0.5f;
	if (1.0f < fRatio)	// UITODO : 나중에 글씨에 따라서 길이를 조절하고 싶은 경우 1.0f 를 변수로 받아서 수정하면 된다.
		fRatio = 1.0f;
	dynamic_cast<CShaderUI*>(Find_ChildUI(L"Chake"))->Set_Floats0(fRatio);
}

CCanvas_Quest* CCanvas_Quest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCanvas_Quest* pInstance = new CCanvas_Quest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_Quest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas* CCanvas_Quest::Clone(void* pArg)
{
	CCanvas_Quest* pInstance = new CCanvas_Quest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_Quest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_Quest::Free()
{
	CCanvas::Free();

}