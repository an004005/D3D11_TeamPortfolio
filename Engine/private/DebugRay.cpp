#include "stdafx.h"
#include "..\public\DebugRay.h"
#include "DebugDraw.h"
#include "GameInstance.h"
#include "JsonLib.h"

CDebugRay::CDebugRay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
	m_pRendererCom = nullptr;
}

CDebugRay::CDebugRay(const CDebugRay& rhs)
	: CGameObject(rhs)
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
{
	Safe_AddRef(m_pInputLayout);
}

HRESULT CDebugRay::Initialize_Prototype()
{
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);

	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void*		pShaderByteCode;
	size_t			iShaderByteCodeSize;

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeSize);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeSize, &m_pInputLayout)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDebugRay::Initialize(void* pArg)
{
	if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		m_vOrigin = json["Origin"];
		m_vDir = json["Dir"];
		m_fLife = json["Life"];
	}

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_NOW, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;
	return S_OK;
}

void CDebugRay::Tick(_double TimeDelta)
{
	m_fLife -= (_float)TimeDelta;
	if (m_fLife < 0.f)
		m_bDelete = true;
}

void CDebugRay::Late_Tick(_double TimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DEBUG, this);
}

HRESULT CDebugRay::Render()
{
	m_vColor = m_isColl == true ? _float4(1.f, 0.f, 0.f, 1.f) : _float4(0.f, 1.f, 0.f, 1.f);

	m_pEffect->SetWorld(XMMatrixIdentity());

	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	m_pEffect->SetView(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

	RELEASE_INSTANCE(CPipeLine);

	m_pEffect->Apply(m_pContext);

	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pBatch->Begin();

	DX::DrawRay(m_pBatch, m_vOrigin, m_vDir, false, XMLoadFloat4(&m_vColor));

	m_pBatch->End();

	return S_OK;
}

CDebugRay * CDebugRay::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CDebugRay*		pInstance = new CDebugRay(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDebugRay");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CDebugRay::Clone(void* pArg)
{
	CDebugRay*		pInstance = new CDebugRay(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDebugRay");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDebugRay::Free()
{
	__super::Free();
	Safe_Release(m_pInputLayout);
	Safe_Release(m_pRendererCom);

	if (false == m_bCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
}
