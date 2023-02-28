#pragma once

#include "Base.h"

BEGIN(Engine)

/* 뭔가를 그려놓기위한 데이터다. */
/* 디퍼드시 : DIFFUSE(반사색), NORMAL(노멀벡터), SHADE(명암)*/

class CRenderTarget final : public CBase
{
private:
	CRenderTarget(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget() = default;

public:
	ID3D11RenderTargetView* Get_RTV() const {
		return m_pRTV;
	}

	ID3D11ShaderResourceView* Get_SRV() const {
		return m_pSRV;
	}
	ID3D11Texture2D* Get_Tex2D() const { return m_pTexture2D; }

	void SetResizable(_bool bResizable) { m_bResizable = bResizable; }

public:
	HRESULT Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4* pClearColor);
	HRESULT Resize(_uint iWidth, _uint iHeight);
	HRESULT Clear();
	void SetIgnoreClearOnce() { m_bIgnoreClearOnce = true; }


#ifdef _DEBUG

public:
	HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
	void Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

#endif

private:
	ID3D11Device*						m_pDevice = nullptr;
	ID3D11DeviceContext*				m_pContext = nullptr;
	_bool								m_bResizable = true;

private:
	ID3D11Texture2D*					m_pTexture2D = nullptr;
	ID3D11RenderTargetView*				m_pRTV = nullptr;
	ID3D11ShaderResourceView*			m_pSRV = nullptr;

private:
	_float4								m_vClearColor;
	DXGI_FORMAT							m_ePixelFormat;
	_bool								m_bIgnoreClearOnce = false;

#ifdef _DEBUG
private:
	_float4x4							m_WorldMatrix;
	_float2 vPos;
	_float2 vSize;
#endif

public:
	static CRenderTarget* Create(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4* pClearColor);
	virtual void Free() override;
};

END