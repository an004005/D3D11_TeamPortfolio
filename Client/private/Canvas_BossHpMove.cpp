#include "stdafx.h"
#include "..\public\Canvas_BossHpMove.h"
#include "GameInstance.h"
#include "FSMComponent.h"
#include "JsonStorage.h"
#include "UI_Manager.h"

#include "Canvas_Alarm.h"
#include "Canvas_BossHp.h"
#include "ShaderUI.h"

CCanvas_BossHpMove::CCanvas_BossHpMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_BossHpMove::CCanvas_BossHpMove(const CCanvas_BossHpMove& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_BossHpMove::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_BossHpMove::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	m_fCurrentHp = 1.0f;

	m_vMaxDestination = { 0.0f, -5.0f };
	CCanvas::UIMove_FSM();

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	Partner_Initialize();

	return S_OK;
}

void CCanvas_BossHpMove::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	m_pUIMoveFSM->Tick(TimeDelta);	// UI �� ������
	CCanvas::UIHit(TimeDelta);

	m_pCanvas_BossHp->Tick(TimeDelta);
	if (m_pUI_Alarm)
		m_pUI_Alarm->Tick(TimeDelta);
}

void CCanvas_BossHpMove::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

	m_pCanvas_BossHp->Late_Tick(TimeDelta);
	if (m_pUI_Alarm)
		m_pUI_Alarm->Late_Tick(TimeDelta);
}

HRESULT CCanvas_BossHpMove::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CCanvas_BossHpMove::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

	static _float fHp;
	ImGui::DragFloat("Hp", &fHp);
	Set_BossHp(fHp);

	static _float fShild;
	ImGui::DragFloat("Shild", &fShild);
	Set_BossShild(fShild);
}

void CCanvas_BossHpMove::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_BossHpMove::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);

	iEnemLevel = _float(json["Level"]);
	m_eEnemyName = EEnemyName(json["Name"]);
}

void CCanvas_BossHpMove::Set_BossHp(const _float & fHp)
{
	m_pCanvas_BossHp->Set_NoMove();
	
	Find_ChildUI(L"Boss_Hp")->SetVisible(true);
	Find_ChildUI(L"Boss_HPBack")->SetVisible(true);
	dynamic_cast<CShaderUI*>(Find_ChildUI(L"Boss_Hp"))->Set_Floats0(fHp);

	_float fRatio = dynamic_cast<CShaderUI*>(Find_ChildUI(L"Boss_HPBack"))->Get_Floats0();
	if (fRatio > fHp)
		fRatio -= _float(TIME_DELTA) * 0.1f;
	else
		fRatio = fHp;
	dynamic_cast<CShaderUI*>(Find_ChildUI(L"Boss_HPBack"))->Set_Floats0(fRatio);
}

void CCanvas_BossHpMove::Set_BossShild(const _float & fShild)
{
	Find_ChildUI(L"Boss_Shild")->SetVisible(true);
	dynamic_cast<CShaderUI*>(Find_ChildUI(L"Boss_Shild"))->Set_Floats0(fShild);
}

void CCanvas_BossHpMove::Partner_Initialize()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_BossHp.json");
	m_pCanvas_BossHp = dynamic_cast<CCanvas_BossHp*>(pGameInstance->Clone_GameObject_NoLayer(LEVEL_NOW, L"Canvas_BossHp", &json));
	assert(m_pCanvas_BossHp != nullptr && "Failed to Clone : CCanvas_BossHp");

	_float2 fIndex = { 0.0f, 0.0f };
	if (EEnemyName::EM0800 == m_eEnemyName) {
		if (10 == iEnemLevel) fIndex = { 0.0f,10.0f };
		else if (18 == iEnemLevel) fIndex = { 1.0f, 10.0f };
	}
	else if (EEnemyName::EM0320 == m_eEnemyName) fIndex = { 0.0f, 4.0f };
	else if (EEnemyName::EM1100 == m_eEnemyName) fIndex = { 0.0f, 12.0f };
	else if (EEnemyName::EM1200 == m_eEnemyName) fIndex = { 0.0f, 11.0f };
	else if (EEnemyName::EM8200 == m_eEnemyName) fIndex = { 0.0f, 13.0f };

	m_pCanvas_BossHp->Set_LevelName(fIndex.x, fIndex.y);

	if (EEnemyName::EM8200 != m_eEnemyName)
	{
		json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Alarm.json");
		m_pUI_Alarm = dynamic_cast<CCanvas_Alarm*>(pGameInstance->Clone_GameObject_NoLayer(LEVEL_NOW, L"Canvas_Alarm", &json));
		assert(m_pUI_Alarm != nullptr && "Failed to Clone : CCanvas_Alarm");
		m_pUI_Alarm->Set_Appeart();
	}
}

CCanvas_BossHpMove * CCanvas_BossHpMove::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_BossHpMove*		pInstance = new CCanvas_BossHpMove(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_BossHpMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_BossHpMove::Clone(void * pArg)
{
	CCanvas_BossHpMove*		pInstance = new CCanvas_BossHpMove(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_BossHpMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_BossHpMove::Free()
{
	CCanvas::Free();

	Safe_Release(m_pCanvas_BossHp);
	Safe_Release(m_pUI_Alarm);

}
