#include "stdafx.h"
#include "..\public\GravikenisisGUI.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "EffectGroup.h"
#include "EffectSystem.h"

CGravikenisisGUI::CGravikenisisGUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CGravikenisisGUI::CGravikenisisGUI(const CGravikenisisGUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CGravikenisisGUI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGravikenisisGUI::Initialize(void * pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_InGameDataGroup/PsychokinesisGauge_G.json");
	m_pGroup = dynamic_cast<CEffectGroup*>(CGameInstance::GetInstance()->Clone_GameObject_Get(LEVEL_NOW, L"Layer_GravikenisisG", L"ProtoVFX_EffectGroup", &json));

	Safe_AddRef(m_pGroup);
	Assert(m_pGroup != nullptr);

	return S_OK;
}

void CGravikenisisGUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	m_pGroup->GetTransform()->CopyState(CTransform::STATE_TRANSLATION, m_pTransformCom);
}

void CGravikenisisGUI::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	static _float fRatio;
	ImGui::DragFloat("Ratio", &fRatio);
	SetfRatio(fRatio);
}

void CGravikenisisGUI::SetfRatio(const _float & fRatio)
{
	m_pGroup->GetSecondEffect()->GetParams().Floats[0] = fRatio;
	m_pGroup->GetThirdEffect()->GetParams().Floats[0] = fRatio;
}

CGravikenisisGUI * CGravikenisisGUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGravikenisisGUI*      pInstance = new CGravikenisisGUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGravikenisisGUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CGravikenisisGUI::Clone(void * pArg)
{
	CGravikenisisGUI*      pInstance = new CGravikenisisGUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGravikenisisGUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGravikenisisGUI::Free()
{
	__super::Free();

	if(m_pGroup != nullptr && m_pGroup->IsDeleted() == false)
		m_pGroup->SetDelete();

	Safe_Release(m_pGroup);
}