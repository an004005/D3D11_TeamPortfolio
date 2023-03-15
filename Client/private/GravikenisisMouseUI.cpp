#include "stdafx.h"
#include "..\public\GravikenisisMouseUI.h"
#include "GameInstance.h"
#include "VFX_Manager.h"
#include "MapKinetic_Object.h"

CGravikenisisMouseUI::CGravikenisisMouseUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CGravikenisisMouseUI::CGravikenisisMouseUI(const CGravikenisisMouseUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CGravikenisisMouseUI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGravikenisisMouseUI::Initialize(void * pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CGravikenisisMouseUI::BeginTick()
{
	m_pKenisis = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"PsychokinesisGauge_Mouse", TEXT("Layer_UI"));

	Safe_AddRef(m_pKenisis);
	Assert(m_pKenisis != nullptr);

	m_pKenisis->Start_NoAttach(m_pKenisis, true);
}

void CGravikenisisMouseUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_pKenisis == nullptr)
		return;

	//if (dynamic_cast<CMapKinetic_Object*>(m_pKenisis)->IsDeleted())
	//{

	//}
}

void CGravikenisisMouseUI::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	static _float fRatio;
	ImGui::DragFloat("Ratio", &fRatio);
	SetfRatio(fRatio);
}

void CGravikenisisMouseUI::SetfRatio(const _float & fRatio)
{
	//m_pKenisis->GetSecondEffect()->GetParams().Floats[0] = fRatio;
	//m_pKenisis->GetThirdEffect()->GetParams().Floats[0] = fRatio;
}

CGravikenisisMouseUI * CGravikenisisMouseUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGravikenisisMouseUI*      pInstance = new CGravikenisisMouseUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGravikenisisMouseUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CGravikenisisMouseUI::Clone(void * pArg)
{
	CGravikenisisMouseUI*      pInstance = new CGravikenisisMouseUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGravikenisisMouseUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGravikenisisMouseUI::Free()
{
	__super::Free();

	if (m_pKenisis != nullptr && m_pKenisis->IsDeleted() == false)
		m_pKenisis->SetDelete();

	Safe_Release(m_pKenisis);
}