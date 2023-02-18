#pragma once

#include "Base.h"

BEGIN(Engine)

class CTarget_Manager final : public CBase
{
	DECLARE_SINGLETON(CTarget_Manager)

public:
	CTarget_Manager();
	virtual ~CTarget_Manager() = default;

public:
	ID3D11ShaderResourceView* Get_SRV(const _tchar* pTargetTag);
	ID3D11RenderTargetView* Get_RTV(const _tchar* pTargetTag);
	class CRenderTarget* GetTarget(const _tchar* pTargetTag);

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Add_RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4* pClearColor);
	HRESULT Add_MRT(const _tchar* pMRTTag, const _tchar* pTargetTag);

	HRESULT Begin_MRT(ID3D11DeviceContext* pContext, const _tchar* pMRTTag, _bool bClear = true);
	HRESULT Begin_MRT_WithDepthStencil(ID3D11DeviceContext* pContext, const _tchar* pMRTTag, ID3D11DepthStencilView* pDepthStencilView, _bool bClear = true);
	HRESULT End_MRT(ID3D11DeviceContext* pContext, const _tchar* pMRTTag);

	HRESULT Resize(ID3D11DeviceContext * pContext);

	void CopyRenderTarget(ID3D11DeviceContext * pContext, const _tchar* pDstTag, const _tchar* pSrcTag);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const _tchar* pTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	void Render_Debug(const _tchar* pMRTTag);
#endif // _DEBUG

private:
	map<const _tchar*, class CRenderTarget*>				m_RenderTargets;
	typedef map<const _tchar*, class CRenderTarget*>		RENDERTARGETS;

private:
	/* 동시에 장치에 바인딩 되어야할 렌더타겟들. 최대 여덟개 */
	map<const _tchar*, list<class CRenderTarget*>>			m_MRTs;
	typedef map<const _tchar*, list<class CRenderTarget*>>	MRTS;



private:
	ID3D11RenderTargetView*				m_pBackBufferView = nullptr;
	ID3D11DepthStencilView*				m_pDepthStencilView = nullptr;


#ifdef _DEBUG
private:
	class CVIBuffer_Rect*				m_pVIBuffer = nullptr;
	class CShader*						m_pShader = nullptr;
	_float4x4							m_ViewMatrix, m_ProjMatrix;
#endif


private:
	class CRenderTarget* Find_RenderTarget(const _tchar* pTargetTag);
	list<class CRenderTarget*>* Find_MRT(const _tchar* pMRTTag);

public:
	virtual void Free() override;
};

END