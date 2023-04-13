#include "stdafx.h"
#include "..\public\Canvas_BrainFieldMove.h"
#include "GameInstance.h"
#include "PlayerInfoManager.h"

#include "ShaderUI.h"

CCanvas_BrainFieldMove::CCanvas_BrainFieldMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_BrainFieldMove::CCanvas_BrainFieldMove(const CCanvas_BrainFieldMove& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_BrainFieldMove::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_BrainFieldMove::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCanvas_BrainFieldMove::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	_float fBrain = CPlayerInfoManager::GetInstance()->Get_PlayerStat().fBrainFieldMaintain;
	_float fMaxBrain = CPlayerInfoManager::GetInstance()->Get_PlayerStat().fMaxBrainFieldMaintain;
	_float fRatio = 1.0f - (fBrain / fMaxBrain);

	IM_LOG("%f, %f, %f", fBrain, fMaxBrain, fRatio);

	dynamic_cast<CShaderUI*>(Find_ChildUI(L"BrainFieldGauge"))->Set_Floats0(fRatio);
	dynamic_cast<CShaderUI*>(Find_ChildUI(L"BrainFieldGaugeB"))->Set_Floats0(fRatio);
}

void CCanvas_BrainFieldMove::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_BrainFieldMove::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CCanvas_BrainFieldMove::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

}

void CCanvas_BrainFieldMove::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_BrainFieldMove::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

CCanvas_BrainFieldMove * CCanvas_BrainFieldMove::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_BrainFieldMove*		pInstance = new CCanvas_BrainFieldMove(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_BrainFieldMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_BrainFieldMove::Clone(void * pArg)
{
	CCanvas_BrainFieldMove*		pInstance = new CCanvas_BrainFieldMove(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_BrainFieldMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_BrainFieldMove::Free()
{
	CCanvas::Free();
}
