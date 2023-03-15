#include "stdafx.h"
#include "..\public\Target_Manager.h"
#include "RenderTarget.h"

#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "Graphic_Device.h"

IMPLEMENT_SINGLETON(CTarget_Manager)

CTarget_Manager::CTarget_Manager()
{

}

ID3D11ShaderResourceView * CTarget_Manager::Get_SRV(const _tchar * pTargetTag)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(pTargetTag);
	if (nullptr == pRenderTarget)
		return nullptr;

	return pRenderTarget->Get_SRV();	
}

ID3D11RenderTargetView* CTarget_Manager::Get_RTV(const _tchar* pTargetTag)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(pTargetTag);
	if (nullptr == pRenderTarget)
		return nullptr;

	return pRenderTarget->Get_RTV();	
}

CRenderTarget* CTarget_Manager::GetTarget(const _tchar* pTargetTag)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(pTargetTag);
	if (nullptr == pRenderTarget)
		return nullptr;

	return pRenderTarget;
}

HRESULT CTarget_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
#ifdef _DEBUG

	D3D11_VIEWPORT			ViewportDesc;
	ZeroMemory(&ViewportDesc, sizeof ViewportDesc);

	_uint			iNumViewports = 1;

	pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	m_pShader = CShader::Create(pDevice, pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Rect::Create(pDevice, pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

#endif // _DEBUG

	return S_OK;
}


HRESULT CTarget_Manager::Add_RenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4 * pClearColor)
{
	if (nullptr != Find_RenderTarget(pTargetTag))
		return E_FAIL;

	CRenderTarget*		pRenderTarget = CRenderTarget::Create(pDevice, pContext, iWidth, iHeight, ePixelFormat, pClearColor);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(pTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const _tchar * pMRTTag, const _tchar * pTargetTag)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(pTargetTag);

	if(nullptr == pRenderTarget)
		return E_FAIL;

	list<CRenderTarget*>*		pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
	{
		list<CRenderTarget*>		MRTList;

		MRTList.push_back(pRenderTarget);

		m_MRTs.emplace(pMRTTag, MRTList);
	}
	else
		pMRTList->push_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(ID3D11DeviceContext * pContext, const _tchar * pMRTTag, _bool bClear)
{
	list<CRenderTarget*>*		pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	ID3D11RenderTargetView*		pRTVs[8] = { nullptr };

	_uint			iNumViews = 0;

	for (auto& pRTV : *pMRTList)	
	{
		// if (bClear)
		// 	pRTV->Clear();

		pRTVs[iNumViews++] = pRTV->Get_RTV();	
	}

	_uint iNumViewports = 1;
	pContext->RSGetViewports(&iNumViewports, &m_OriginViewPort);

	pContext->OMSetRenderTargets(iNumViews, pRTVs, m_pDepthStencilView);

	return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT_WithDepthStencil(ID3D11DeviceContext* pContext, const _tchar* pMRTTag,
	ID3D11DepthStencilView* pDepthStencilView, _bool bClear)
{
	list<CRenderTarget*>*		pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	ID3D11RenderTargetView*		pRTVs[8] = { nullptr };

	_uint			iNumViews = 0;

	for (auto& pRTV : *pMRTList)	
	{
		// if (bClear)
		// 	pRTV->Clear();

		pRTVs[iNumViews++] = pRTV->Get_RTV();	
	}

	pContext->OMSetRenderTargets(iNumViews, pRTVs, pDepthStencilView);

	return S_OK;
}

HRESULT CTarget_Manager::End_MRT(ID3D11DeviceContext * pContext, const _tchar * pMRTTag)
{
	pContext->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView);

	Safe_Release(m_pBackBufferView);
	Safe_Release(m_pDepthStencilView);

	// pContext->RSSetViewports(1, &m_OriginViewPort);

	return S_OK;
}

HRESULT CTarget_Manager::Begin_RenderTarget(ID3D11DeviceContext* pContext, const _tchar* pTargetTag)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(pTargetTag);

	ID3D11ShaderResourceView*		pSRVs[128] = { nullptr };

	pContext->PSSetShaderResources(0, 128, pSRVs);

	ID3D11RenderTargetView*			pRTV;

	pRenderTarget->Clear();
	pRTV = pRenderTarget->Get_RTV();

	// 기존에 바인딩되어있던 (백버퍼 + 깊이스텐실버퍼)를 얻어옴
	pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	_uint iNumViewports = 1;
	pContext->RSGetViewports(&iNumViewports, &m_OriginViewPort);

	pContext->OMSetRenderTargets(1, &pRTV, m_pDepthStencilView);

	return S_OK;
}

HRESULT CTarget_Manager::Begin_ShadowDepthRenderTarget(ID3D11DeviceContext* pContext, const _tchar* pTargetTag)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(pTargetTag);

	ID3D11ShaderResourceView*		pSRVs[128] = { nullptr };

	pContext->PSSetShaderResources(0, 128, pSRVs);

	ID3D11RenderTargetView*		pRTV;

	pRenderTarget->Clear();
	pRTV = pRenderTarget->Get_RTV();

	/* 기존에 바인딩되어있던(백버퍼 + 깊이스텐실버퍼)를 얻어온다. */
	pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	_uint iNumViewports = 1;
	pContext->RSGetViewports(&iNumViewports, &m_OriginViewPort);

	pContext->OMSetRenderTargets(1, &pRTV, pRenderTarget->GetDepthStencilView());

	pContext->ClearDepthStencilView(pRenderTarget->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	pContext->RSSetViewports(1, &pRenderTarget->GetViewPortDesc());

	return S_OK;
}

HRESULT CTarget_Manager::ClearTargets()
{
	for (auto& target : m_RenderTargets)
	{
		target.second->Clear();
	}
	return S_OK;
}

HRESULT CTarget_Manager::Resize(ID3D11DeviceContext * pContext)
{
	D3D11_VIEWPORT		ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));

	_uint		iNumViewport = 1;

	pContext->RSGetViewports(&iNumViewport, &ViewPortDesc);

	for (auto& Pair : m_RenderTargets)
	{
		if (FAILED(Pair.second->Resize((_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height)))
			return E_FAIL;
	}

#ifdef _DEBUG
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewPortDesc.Width, ViewPortDesc.Height, 0.f, 1.f));
#endif // _DEBUG

	return S_OK;
}

void CTarget_Manager::CopyRenderTarget(ID3D11DeviceContext * pContext, const _tchar* pDstTag, const _tchar* pSrcTag)
{
	auto pDst = Find_RenderTarget(pDstTag);
	auto pSrc = Find_RenderTarget(pSrcTag);
	Assert(pDst != nullptr);
	Assert(pDst != nullptr);

	pContext->CopyResource(pDst->Get_Tex2D(), pSrc->Get_Tex2D());
}

#ifdef _DEBUG

HRESULT CTarget_Manager::Ready_Debug(const _tchar * pTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget*		pTarget = Find_RenderTarget(pTargetTag);

	if (nullptr == pTarget)
		return E_FAIL;

	return pTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);	
}

void CTarget_Manager::Render_Debug(const _tchar* pMRTTag)
{
	list<CRenderTarget*>*		pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
		return;

	if (FAILED(m_pShader->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;

	if (FAILED(m_pShader->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Render(m_pShader, m_pVIBuffer);	
	}
}
#endif // _DEBUG

CRenderTarget * CTarget_Manager::Find_RenderTarget(const _tchar * pTargetTag)
{
	auto	iter = find_if(m_RenderTargets.begin(), m_RenderTargets.end(), CTag_Finder(pTargetTag));

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;	
}

list<class CRenderTarget*>* CTarget_Manager::Find_MRT(const _tchar * pMRTTag)
{
	auto	iter = find_if(m_MRTs.begin(), m_MRTs.end(), CTag_Finder(pMRTTag));

	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

void CTarget_Manager::Free()
{
	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)		
			Safe_Release(pRenderTarget);

		Pair.second.clear();
	}
	m_MRTs.clear();

	for (auto& Pair : m_RenderTargets)
		Safe_Release(Pair.second);

	m_RenderTargets.clear();

#ifdef _DEBUG
	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);

#endif
}