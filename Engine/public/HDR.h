#pragma once
#include "Base.h"

BEGIN(Engine)

class CHDR : public CBase
{
	DECLARE_SINGLETON(CHDR)
private:
	CHDR();
	virtual ~CHDR() = default;

public:
	HRESULT Initialize(ID3D11Device* m_pDevice, ID3D11DeviceContext* m_pDeviceContext);
	void PostProcessing(ID3D11ShaderResourceView* pHDRSRV, ID3D11RenderTargetView* pLDRRTV);
	_bool IsOn() const { return m_bOn; }
	void Imgui_Render();

private:
	void DownScale(ID3D11ShaderResourceView* pHDRSRV);
	void Bloom();
	void Blur(ID3D11ShaderResourceView* pInput, ID3D11UnorderedAccessView* pOutput);

	// Final pass that composites all the post processing calculations
	void FinalPass(ID3D11ShaderResourceView* pHDRSRV);

	static void SetDebugName(ID3D11DeviceChild* pObj, const char* pName);
	HRESULT CompileShader(PWCHAR strPath, D3D10_SHADER_MACRO* pMacros, char* strEntryPoint, char* strProfile, DWORD dwShaderFlags, ID3DBlob** ppVertexShaderBuffer);
	// Down scale the full size HDR image
	void Clear();

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

	_bool m_bOn = false;

	// Downscaled scene texture
	ID3D11Texture2D* m_pDownScaleRT = nullptr;
	ID3D11ShaderResourceView* m_pDownScaleSRV = nullptr;
	ID3D11UnorderedAccessView* m_pDownScaleUAV = nullptr;

	// Temporary texture
	ID3D11Texture2D* m_pTempRT[2]{nullptr, nullptr};
	ID3D11ShaderResourceView* m_pTempSRV[2]{nullptr, nullptr};
	ID3D11UnorderedAccessView* m_pTempUAV[2]{nullptr, nullptr};

	// Bloom texture
	ID3D11Texture2D* m_pBloomRT = nullptr;
	ID3D11ShaderResourceView* m_pBloomSRV = nullptr;
	ID3D11UnorderedAccessView* m_pBloomUAV = nullptr;

	// 1D intermediate storage for the down scale operation
	ID3D11Buffer* m_pDownScale1DBuffer = nullptr;
	ID3D11UnorderedAccessView* m_pDownScale1DUAV = nullptr;
	ID3D11ShaderResourceView* m_pDownScale1DSRV = nullptr;

	// Average luminance
	ID3D11Buffer* m_pAvgLumBuffer = nullptr;
	ID3D11UnorderedAccessView* m_pAvgLumUAV = nullptr;
	ID3D11ShaderResourceView* m_pAvgLumSRV = nullptr;

	// Previous average luminance for adaptation
	ID3D11Buffer* m_pPrevAvgLumBuffer = nullptr;
	ID3D11UnorderedAccessView* m_pPrevAvgLumUAV = nullptr;
	ID3D11ShaderResourceView* m_pPrevAvgLumSRV = nullptr;

	UINT m_nWidth;
	UINT m_nHeight;
	UINT m_nDownScaleGroups;
	float m_fMiddleGrey;
	float m_fWhite;
	float m_fAdaptation;
	float m_fBloomThreshold;
	float m_fBloomScale;

	typedef struct
	{
		UINT nWidth;
		UINT nHeight;
		UINT nTotalPixels;
		UINT nGroupSize;
		float fAdaptation;
		float fBloomThreshold;
		UINT pad[2];
	} TDownScaleCB;
	ID3D11Buffer* m_pDownScaleCB = nullptr;

	typedef struct
	{
		float fMiddleGrey;
		float fLumWhiteSqr;
		float fBloomScale;
		UINT pad;
	} TFinalPassCB;
	ID3D11Buffer* m_pFinalPassCB = nullptr;

	typedef struct
	{
		UINT numApproxPasses;
		float fHalfBoxFilterWidth;			// w/2
		float fFracHalfBoxFilterWidth;		// frac(w/2+0.5)
		float fInvFracHalfBoxFilterWidth;	// 1-frac(w/2+0.5)
		float fRcpBoxFilterWidth;			// 1/w
		UINT pad[3];
	} TBlurCB;
	ID3D11Buffer* m_pBlurCB = nullptr;

	// Shaders
	ID3D11ComputeShader* m_pBloomRevealCS = nullptr;
	ID3D11ComputeShader* m_HorizontalBlurCS = nullptr;
	ID3D11ComputeShader* m_VerticalBlurCS = nullptr;

	ID3D11ComputeShader* m_pDownScaleFirstPassCS = nullptr;
	ID3D11ComputeShader* m_pDownScaleSecondPassCS = nullptr;
	ID3D11VertexShader* m_pFullScreenQuadVS = nullptr;
	ID3D11PixelShader* m_pFinalPassPS = nullptr;

	ID3D11SamplerState* m_pSampPoint = nullptr;
	ID3D11SamplerState* m_pSampLinear = nullptr;


public:
	virtual void Free() override;
	
};

END
