#include "stdafx.h"
#include "..\public\SA_RotationUI.h"

CSA_RotationUI::CSA_RotationUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CSA_RotationUI::CSA_RotationUI(const CSA_RotationUI& rhs)
	: CUI(rhs)
{
}

HRESULT CSA_RotationUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSA_RotationUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	m_fRadius = 30.0f;
	m_fSpeed = 700.0f;

	return S_OK;
}

void CSA_RotationUI::Tick(_double TimeDelta)
{
	const _float2 PivotPair = GetPivotXY(m_ePivot);

	if (0.1f > m_vScale.x || 0.1f > m_vScale.y)
		return; // 텍스처의 스케일을 조정할 때 0.0f 보다 작아지면 그 다음부터 0 * m_vScale.y 이 되기 때문에 0.1f 보다 작은 경우에는 return 시킨다.

	const _float theta = (m_fAngle - 90.f) * XM_PI / 180.f;

	_float fX = cosf(theta) * m_fRadius;
	_float fY = -sinf(theta) * m_fRadius;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX * m_vScale.x, m_fSizeY * m_vScale.y, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX + PivotPair.x + fX, m_fY + PivotPair.y + fY, 0.f, 1.f));

	// + 는 오른쪽 -는 왼쪽
	m_fAngle_Add += _float(TimeDelta) * m_fSpeed;
	m_fAngle = m_fAngle_Add;
}

void CSA_RotationUI::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
	ImGui::DragFloat("angle", &m_fAngle);
	ImGui::DragFloat("radius", &m_fRadius);
}

CSA_RotationUI * CSA_RotationUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CSA_RotationUI*		pInstance = new CSA_RotationUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSA_RotationUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CSA_RotationUI::Clone(void * pArg)
{
	CSA_RotationUI*		pInstance = new CSA_RotationUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSA_RotationUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSA_RotationUI::Free()
{
	__super::Free();

}
