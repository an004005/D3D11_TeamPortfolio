#pragma once

#include "Component.h"

BEGIN(Engine)

enum class EShaderParamType
{
	PARAM_INT,
	PARAM_FLOAT,
	PARAM_FLOAT2,
	PARAM_FLOAT4,
	PARAM_FLOAT4X4,
	PARAM_TEXTURE,
	PARAM_END
};

enum
{
	PARAM_INT_CNT = 4,
	PARAM_FLOAT_CNT = 10,
	PARAM_FLOAT2_CNT = 4,
	PARAM_FLOAT4_CNT = 4,
	PARAM_FLOAT4X4_CNT = 2,
	PARAM_TEXTURE_CNT = 8,
	PARAM_TEXTURE_ON_CNT = 8,
	PARAM_TYPE_CNT = static_cast<_uint>(EShaderParamType::PARAM_END)
};

struct ShaderParams
{
	vector<_int> Ints;
	vector<_float> Floats;
	vector<_float2> Float2s;
	vector<_float4> Float4s;
	vector<_float4x4> Float4x4s;
	vector<pair<class CTexture*, _uint>> Textures;
	_uint iPass;
};

class ENGINE_DLL CShader final : public CComponent
{
protected:
	CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pShaderFilePath, D3D11_INPUT_ELEMENT_DESC* pElements, const _uint iNumElements);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Imgui_RenderProperty() override;

public:
	HRESULT Begin(_uint iPassIndex);

	HRESULT Set_RawValue(const char* pConstantName, const void* pData, _uint iLength);
	HRESULT Set_Matrix(const char* pConstantName, const _float4x4* pMatrix);
	HRESULT Set_MatrixArray(const char* pConstantName, const _float4x4* pMatrix, _uint iNumMatrices);
	HRESULT Set_ShaderResourceViewArray(const char* pConstantName, ID3D11ShaderResourceView** ppSRV, _uint iNumTextures);
	HRESULT Set_ShaderResourceView(const char* pConstantName, ID3D11ShaderResourceView* pSRV);

public:
	HRESULT Set_Param_Int(_uint iIdx, const _int* iValue);
	HRESULT Set_Param_float(_uint iIdx, const  _float* fValue);
	HRESULT Set_Param_Texture(_uint iIdx, class CTexture* pTexture, _uint iTexIdx = 0);
	HRESULT Set_Param_float2(_uint iIdx, const  _float2* vValue);
	HRESULT Set_Param_float4(_uint iIdx, const  _float4* vValue);
	HRESULT Set_Param_float4x4(_uint iIdx, const  _float4x4* matValue);
	HRESULT Set_Invalidate_Textures();
	HRESULT Set_Params(const ShaderParams& tParam);
	HRESULT Begin_Params(const ShaderParams& tParam);

public:
	static void SaveShaderParam(const ShaderParams& tParams, Json& json);
	static void LoadShaderParam(ShaderParams& tParams, const Json& json);
	static void Imgui_RenderShaderParams(ShaderParams& tParams);

public:
	_uint GetNumPasses() const { return m_iNumPasses; }

public:
	static const array<const char*, PARAM_INT_CNT> s_ParamIntName;
	static const array<const char*, PARAM_FLOAT_CNT> s_ParamFloatName;
	static const array<const char*, PARAM_FLOAT2_CNT> s_ParamFloat2Name;
	static const array<const char*, PARAM_FLOAT4_CNT> s_ParamFloat4Name;
	static const array<const char*, PARAM_FLOAT4X4_CNT> s_ParamFloat4x4Name;
	static const array<const char*, PARAM_TEXTURE_CNT> s_ParamTextureName;
	static const array<const char*, PARAM_TEXTURE_ON_CNT> s_ParamTextureOnName;

#ifdef _DEBUG
	void ReCompileShader();
#endif

private:
	ID3DX11Effect*				m_pEffect = nullptr;
	vector<ID3D11InputLayout*>	m_InputLayouts;

private:
	_uint						m_iNumPasses = 0;
	_float						m_fTime = 0.f;

#ifdef _DEBUG
private:
	wstring m_shaderFilePath;
	D3D11_INPUT_ELEMENT_DESC* m_pElements;
	_uint m_iNumElements;
#endif

public:
	static CShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, D3D11_INPUT_ELEMENT_DESC* pElements, const _uint iNumElements);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END
