#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CContext_LockGuard
{
public:
	CContext_LockGuard(mutex& ContextMtx, _bool bLoading)
		: m_ContextMtx(ContextMtx)
		, m_bLoading(bLoading)
	{
		if (m_bLoading) m_ContextMtx.lock();
	}

	~CContext_LockGuard()
	{
		if (m_bLoading) m_ContextMtx.unlock();
	}

private:
	mutex& m_ContextMtx;
	_bool m_bLoading;
};

class CGraphic_Device final : public CBase
{
	DECLARE_SINGLETON(CGraphic_Device)

public:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;

public:
	HRESULT Ready_Graphic_Device(HWND hWnd, GRAPHIC_DESC::WINMODE WinMode, _uint iWinCX, _uint iWinCY, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut);
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();
	HRESULT Update_SwapChain(HWND hWnd, _uint iWinCX, _uint iWinCY, _bool bIsFullScreen, _bool bNeedUpdate);

	ID3D11Device* GetDevice() { return m_pDevice; }
	ID3D11DeviceContext* GetContext() { return m_pDeviceContext; }

	_uint GetWinCX() const { return m_iWinCX; }
	_uint GetWinCY() const { return m_iWinCY; }

	mutex& GetContextMtx() { return m_ContextMtx; }

private:	

	/* 메모리 할당. (정점버퍼, 인덱스버퍼, 텍스쳐로드) */
	ID3D11Device*			m_pDevice = nullptr;

	/* 바인딩작업. 기능실행 (정점버퍼를 SetStreamSource(), SetIndices(), SetTransform(), SetTexture() */
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

	IDXGISwapChain*			m_pSwapChain = nullptr;	

	// ID3D11ShaderResourceView*
	ID3D11RenderTargetView*		m_pBackBufferRTV = nullptr;
	ID3D11DepthStencilView*		m_pDepthStencilView = nullptr;

	_uint m_iWinCX = 0;
	_uint m_iWinCY = 0;

	mutex m_ContextMtx;

private:
	HRESULT Ready_SwapChain(HWND hWnd, GRAPHIC_DESC::WINMODE WinMode, _uint iWinCX, _uint iWinCY);
	HRESULT Ready_BackBufferRenderTargetView();
	HRESULT Ready_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY);

public:
	virtual void Free() override;
};

END