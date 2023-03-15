#include "stdafx.h"
#include "..\public\LevelUpUI.h"
#include "GameInstance.h"
#include "FSMComponent.h"

// LevelUp 글씨
// g_vec2_0 : x 를 이용해서 Level 을 변경해준다.
// g_vec4_0 : x,y,z 를 이용해서 회색에서 흰색사이를 왔다갔다 한다.  w 를 이용해서 AlphaPlus 을 조절한다.

// LevelUp 흰색 선 배경
// g_vec4_0 : w 를 이용해서 AlphaPlus 을 조절한다.

// LevelUp 흰색 선 배경 뒷 배경
// g_float_0 : 0.2 (회색) 1.0 (흰색) 을 조절한다.

CLevelUpUI::CLevelUpUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CLevelUpUI::CLevelUpUI(const CLevelUpUI& rhs)
	: CUI(rhs)
{
}

HRESULT CLevelUpUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevelUpUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	LevelUp_Initialize();

	m_Timeline.SetCurve("UI_LevelUp");

	return S_OK;
}

void CLevelUpUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_pLevelUpFSM->Tick(TimeDelta);
	LevelUpBack_Tick(TimeDelta);
	LevelUpBackGround_Tick(TimeDelta);
}

void CLevelUpUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CLevelUpUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CLevelUpUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

	m_pLevelUpFSM->Imgui_RenderProperty();	// FSM Imgui 로 확인하기
	m_Timeline.Imgui_RenderEditor();

}

void CLevelUpUI::LevelUp_Initialize()
{
	m_pLevelUpFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
		.AddTransition("Idle to AlphaPlus", "AlphaPlus")
		.Predicator([this] {
		return m_bLevelUp;
	})
		.AddState("AlphaPlus")
		.OnStart([this]
	{
		m_tParams.Float2s[0].x = _float(m_iLevel);
	})
		.Tick([this](_double TimeDelta) {

		m_tParams.Float4s[0].w += _float(TimeDelta);

	})
		.AddTransition("AlphaPlus to Gray", "Gray")
		.Predicator([this] {
		return 1.0f < m_tParams.Float4s[0].w;
	})
		.AddState("Gray")
		.Tick([this](_double TimeDelta) {

		m_tParams.Float4s[0].x -= _float(TimeDelta);
		m_tParams.Float4s[0].y -= _float(TimeDelta);
		m_tParams.Float4s[0].z -= _float(TimeDelta);

	})
		.AddTransition("Gray to White", "White")
		.Predicator([this] {
		return 0.5f > m_tParams.Float4s[0].x;
	})
		.AddState("White")
		.Tick([this](_double TimeDelta) {

		m_tParams.Float4s[0].x += _float(TimeDelta);
		m_tParams.Float4s[0].y += _float(TimeDelta);
		m_tParams.Float4s[0].z += _float(TimeDelta);

	})
		.AddTransition("White to AlphaMinus", "AlphaMinus")
		.Predicator([this] {
		return 1.0f < m_tParams.Float4s[0].x;
	})
		.AddState("AlphaMinus")
		.Tick([this](_double TimeDelta) {

		m_tParams.Float4s[0].w -= _float(TimeDelta);

		if (0.0f > m_tParams.Float4s[0].w)
		{
			m_bLevelUp = false;
			m_bVisible = false;
		}
	})
	.AddTransition("AlphaMinus to Idle", "Idle")
	.OnExit([&] {
		m_tParams.Float4s[0].x = 1.0f;
		m_tParams.Float4s[0].y = 1.0f;
		m_tParams.Float4s[0].z = 1.0f;
		m_tParams.Float4s[0].w = 0.0f;
	})
	.Predicator([this] {
		return m_bLevelUp == false;
	})
		.Build();
}

void CLevelUpUI::LevelUpBack_Tick(const _double & TimeDelta)
{
	if (false == m_bLevelUpBack)
		return;

	// 처음에 알파값이 0 이이다 1까지 올려주고 다시 0으로 내리고 끝낸다.
	if (false == m_bAlpha)
	{
		m_tParams.Float4s[0].w += _float(TimeDelta);

		if (2.0f < m_tParams.Float4s[0].w)
		{
			m_bAlpha = true;
		}
	}
	else
	{
		m_tParams.Float4s[0].w -= _float(TimeDelta) * 1.5f;

		if (0.0f > m_tParams.Float4s[0].w)
		{
			m_tParams.Float4s[0].w = 0.0f;
			m_bAlpha = false;
			m_bLevelUpBack = false;
			m_bVisible = false;
		}
	}
}

void CLevelUpUI::LevelUpBackGround_Tick(const _double & TimeDelta)
{
	if (false == m_bLevelUpBackGround)
		return;
	
	IM_LOG("X %f", m_tParams.Floats[0]);
	
	m_Timeline.Tick(TimeDelta, m_tParams.Floats[0]);

	if(0.0 == m_dLevelBakcGround_TimeAcc)
		m_Timeline.PlayFromStart();

	m_dLevelBakcGround_TimeAcc += TimeDelta;
	if (3.0 < m_dLevelBakcGround_TimeAcc)
	{
		m_dLevelBakcGround_TimeAcc = 0.0;
		m_bLevelUpBackGround = false;
		m_bVisible = false;
	}
}

CLevelUpUI * CLevelUpUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CLevelUpUI*		pInstance = new CLevelUpUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLevelUpUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CLevelUpUI::Clone(void * pArg)
{
	CLevelUpUI*		pInstance = new CLevelUpUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLevelUpUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevelUpUI::Free()
{
	__super::Free();

}
