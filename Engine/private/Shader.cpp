#include "stdafx.h"
#include "..\public\Shader.h"
#include "GameInstance.h"
#include "GameUtils.h"
#include "Texture.h"
#include "JsonLib.h"
#include "ImguiUtils.h"

const array<const char*, PARAM_INT_CNT> CShader::s_ParamIntName {
	"g_int_0","g_int_1","g_int_2","g_int_3"
};
const array<const char*, PARAM_FLOAT_CNT> CShader::s_ParamFloatName {
	"g_float_0","g_float_1","g_float_2","g_float_3",
	"g_float_4","g_float_5","g_float_6","g_float_7",
	"g_float_8","g_float_9"
};
const array<const char*, PARAM_FLOAT2_CNT> CShader::s_ParamFloat2Name {
	"g_vec2_0","g_vec2_1","g_vec2_2","g_vec2_3"
};
const array<const char*, PARAM_FLOAT4_CNT> CShader::s_ParamFloat4Name {
	"g_vec4_0","g_vec4_1","g_vec4_2","g_vec4_3"
};
const array<const char*, PARAM_FLOAT4X4_CNT> CShader::s_ParamFloat4x4Name {
	"g_mat_0","g_mat_1"
};
const array<const char*, PARAM_TEXTURE_CNT> CShader::s_ParamTextureName {
	"g_tex_0","g_tex_1","g_tex_2","g_tex_3","g_tex_4","g_tex_5", "g_tex_6", "g_tex_7", "g_tex_8", "g_tex_9"
};
const array<const char*, PARAM_TEXTURE_ON_CNT> CShader::s_ParamTextureOnName {
	"g_tex_on_0","g_tex_on_1","g_tex_on_2","g_tex_on_3","g_tex_on_4","g_tex_on_5", "g_tex_on_6", "g_tex_on_7", "g_tex_on_8", "g_tex_on_9"
};

CShader::CShader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CShader::CShader(const CShader & rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
	, m_InputLayouts(rhs.m_InputLayouts)	
	, m_iNumPasses(rhs.m_iNumPasses)
#ifdef _DEBUG
	, m_CommonTextures(rhs.m_CommonTextures)
	, m_shaderFilePath(rhs.m_shaderFilePath)
	, m_pElements(rhs.m_pElements)
	, m_iNumElements(rhs.m_iNumElements)
#endif
{
	Safe_AddRef(m_pEffect);

	for (auto& pInputLayout : m_InputLayouts)
		Safe_AddRef(pInputLayout);
}

HRESULT CShader::Initialize_Prototype(const _tchar * pShaderFilePath, D3D11_INPUT_ELEMENT_DESC* pElements, const _uint iNumElements)
{
	_uint			iHlslFlag = 0;

#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

	m_shaderFilePath = pShaderFilePath;
	m_pElements = pElements;
	m_iNumElements = iNumElements;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;

#endif
	if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;	

	ID3DX11EffectTechnique*	pTechnique = m_pEffect->GetTechniqueByIndex(0);

	D3DX11_TECHNIQUE_DESC	TechniqueDesc;
	pTechnique->GetDesc(&TechniqueDesc);

	m_iNumPasses = TechniqueDesc.Passes;

	for (_uint i = 0; i < m_iNumPasses; ++i)
	{	
		ID3D11InputLayout*		pInputLayout = nullptr;

		ID3DX11EffectPass*		pPass = pTechnique->GetPassByIndex(i);
		if (nullptr == pPass)
			return E_FAIL;

		D3DX11_PASS_DESC		PassDesc;
		pPass->GetDesc(&PassDesc);	

		if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
			return E_FAIL;

		m_InputLayouts.push_back(pInputLayout);
	}	

	return S_OK;
}

HRESULT CShader::Initialize(void * pArg)
{
	return S_OK;
}

void CShader::Imgui_RenderProperty()
{
	ImGui::Text("NumPasses: %d", m_iNumPasses);

#ifdef _DEBUG
	if (ImGui::Button("ReCompile"))
	{
		ReCompileShader();
	}
#endif
}

HRESULT CShader::Begin(_uint iPassIndex)
{
	if (nullptr == m_pContext || 
		iPassIndex >= m_iNumPasses)
		return E_FAIL;

	ID3DX11EffectTechnique*	pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	ID3DX11EffectPass*	pPass = pTechnique->GetPassByIndex(iPassIndex);
	if (nullptr == pPass)
		return E_FAIL;

	pPass->Apply(0, m_pContext);

	m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex]);


	return S_OK;
}

HRESULT CShader::Set_RawValue(const char* pConstantName, const void* pData, _uint iLength)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectVariable*		pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(pData, 0, iLength);	
}

HRESULT CShader::Set_Matrix(const char * pConstantName, const _float4x4* pMatrix)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectMatrixVariable*	pVariable = m_pEffect->GetVariableByName(pConstantName)->AsMatrix();
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetMatrix((_float*)pMatrix);
}

HRESULT CShader::Set_MatrixArray(const char* pConstantName, const _float4x4* pMatrix, _uint iNumMatrices)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectMatrixVariable*	pVariable = m_pEffect->GetVariableByName(pConstantName)->AsMatrix();
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetMatrixArray((_float*)pMatrix, 0, iNumMatrices);
}

HRESULT CShader::Set_ShaderResourceViewArray(const char * pConstantName, ID3D11ShaderResourceView ** ppSRV, _uint iNumTextures)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable*		pVariable = m_pEffect->GetVariableByName(pConstantName)->AsShaderResource();
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetResourceArray(ppSRV, 0, iNumTextures);
}

HRESULT CShader::Set_ShaderResourceView(const char * pConstantName, ID3D11ShaderResourceView * pSRV)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable*		pVariable = m_pEffect->GetVariableByName(pConstantName)->AsShaderResource();
	if (nullptr == pVariable)
		return E_FAIL;
	
	return pVariable->SetResource(pSRV);	
}

HRESULT CShader::Set_Param_Int(_uint iIdx, const _int* iValue)
{
	Assert(iIdx < PARAM_INT_CNT);

	ID3DX11EffectVariable*		pVariable = m_pEffect->GetVariableByName(s_ParamIntName[iIdx]);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(iValue, 0, sizeof(_int));
}

HRESULT CShader::Set_Param_float(_uint iIdx, const  _float* fValue)
{
	Assert(iIdx < PARAM_FLOAT_CNT);

	ID3DX11EffectVariable*		pVariable = m_pEffect->GetVariableByName(s_ParamFloatName[iIdx]);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(fValue, 0, sizeof(_float));
}

HRESULT CShader::Set_Param_Texture(_uint iIdx, CTexture* pTexture, _uint iTexIdx)
{
	Assert(iIdx < PARAM_TEXTURE_CNT);
	if (FAILED(pTexture->Bind_ShaderResource(this, s_ParamTextureName[iIdx], iTexIdx)))
		return E_FAIL;

	ID3DX11EffectVariable*		pVariable = m_pEffect->GetVariableByName(s_ParamTextureOnName[iIdx]);
	if (nullptr == pVariable)
		return E_FAIL;

	static constexpr _int iOn = 1;
	return pVariable->SetRawValue(&iOn, 0, sizeof(_int));
}

HRESULT CShader::Set_Param_float2(_uint iIdx, const _float2* vValue)
{
	Assert(iIdx < PARAM_FLOAT2_CNT);

	ID3DX11EffectVariable*		pVariable = m_pEffect->GetVariableByName(s_ParamFloat2Name[iIdx]);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(vValue, 0, sizeof(_float2));
}

HRESULT CShader::Set_Param_float4(_uint iIdx, const _float4* vValue)
{
	Assert(iIdx < PARAM_FLOAT4_CNT);

	ID3DX11EffectVariable*		pVariable = m_pEffect->GetVariableByName(s_ParamFloat4Name[iIdx]);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(vValue, 0, sizeof(_float4));
}

HRESULT CShader::Set_Param_float4x4(_uint iIdx, const _float4x4* matValue)
{
	Assert(iIdx < PARAM_FLOAT4X4_CNT);

	ID3DX11EffectMatrixVariable*	pVariable = m_pEffect->GetVariableByName(s_ParamFloat4x4Name[iIdx])->AsMatrix();
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetMatrix((_float*)matValue);
}

HRESULT CShader::Set_Invalidate_Textures()
{
	for (int i = 0; i < PARAM_TEXTURE_CNT; ++i)
	{
		ID3DX11EffectVariable*		pVariable = m_pEffect->GetVariableByName(s_ParamTextureOnName[i]);
		if (nullptr == pVariable)
			return E_FAIL;

		static constexpr _int iOff = 0;
		if (FAILED(pVariable->SetRawValue(&iOff, 0, sizeof(_int))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CShader::Set_Params(const ShaderParams& tParam)
{
	if (FAILED(Set_Invalidate_Textures()))
		return E_FAIL;

	if (FAILED(Set_RawValue("g_Time", &m_fTime, sizeof(_float))))
		return E_FAIL;

	for (size_t i = 0; i < tParam.Ints.size(); ++i)
	{
		if (FAILED(Set_Param_Int((_uint)i, &tParam.Ints[i])))
			return E_FAIL;
	}
	for (size_t i = 0; i < tParam.Floats.size(); ++i)
	{
		if (FAILED(Set_Param_float((_uint)i, &tParam.Floats[i])))
			return E_FAIL;
	}
	for (size_t i = 0; i < tParam.Float2s.size(); ++i)
	{
		if (FAILED(Set_Param_float2((_uint)i, &tParam.Float2s[i])))
			return E_FAIL;
	}
	for (size_t i = 0; i < tParam.Float4s.size(); ++i)
	{
		if (FAILED(Set_Param_float4((_uint)i, &tParam.Float4s[i])))
			return E_FAIL;
	}
	for (size_t i = 0; i < tParam.Float4x4s.size(); ++i)
	{
		if (FAILED(Set_Param_float4x4((_uint)i, &tParam.Float4x4s[i])))
			return E_FAIL;
	}
	for (size_t i = 0; i < tParam.Textures.size(); ++i)
	{
		if (tParam.Textures[i].first == nullptr)
			continue;

		if (FAILED(Set_Param_Texture((_uint)i, tParam.Textures[i].first, tParam.Textures[i].second)))
			return E_FAIL;
	}
		
	return S_OK;
}


HRESULT CShader::Begin_Params(const ShaderParams& tParam)
{
	if (FAILED(Set_Params(tParam)))
		return E_FAIL;
	if (FAILED(Begin(tParam.iPass)))
		return E_FAIL;

	return S_OK;
}

void CShader::Tick(_double TimeDelta)
{
	m_fTime += (_float)TimeDelta;
}

void CShader::SetCommonTexture(const char* pConstantName, const char* pTexFilePath)
{
	if (m_isCloned == false)
	{
		auto pTex = dynamic_cast<CTexture*>(CGameInstance::GetInstance()->Clone_Component(s2ws(pTexFilePath).c_str()));
		Assert(pTex != nullptr);
		m_CommonTextures.emplace(pConstantName, pTex);
		pTex->Bind_ShaderResource(this, pConstantName);
	}
}

void CShader::SaveShaderParam(const ShaderParams& tParams, Json& json)
{
	Json& jsonParams = json["ShaderParams"];

	jsonParams["Ints"] = tParams.Ints;
	jsonParams["Floats"] = tParams.Floats;
	jsonParams["Float2s"] = tParams.Float2s;
	jsonParams["Float4s"] = tParams.Float4s;
	jsonParams["Float4x4s"] = tParams.Float4x4s;

	vector<string> TextureProtoTags;
	for (auto& e : tParams.Textures)
	{
		if (e.first == nullptr)
			TextureProtoTags.push_back("");
		else
			TextureProtoTags.push_back(CGameUtils::ws2s(e.first->GetPrototypeTag()));
	}
		
	jsonParams["Textures"] = TextureProtoTags;
	jsonParams["Pass"] = tParams.iPass;
}

void CShader::LoadShaderParam(ShaderParams& tParams, const Json& json)
{
	const Json& jsonParams = json["ShaderParams"];

	tParams.Ints = jsonParams["Ints"].get<vector<_int>>();
	tParams.Floats = jsonParams["Floats"].get<vector<_float>>();
	tParams.Float2s = jsonParams["Float2s"];
	tParams.Float4s = jsonParams["Float4s"];
	tParams.Float4x4s = jsonParams["Float4x4s"];

	vector<string> TextureProtoTags = jsonParams["Textures"];
	for (auto e : TextureProtoTags)
	{
		if (e.empty())
		{
			tParams.Textures.push_back({ nullptr, 0 });
		}
		else
		{
			tParams.Textures.push_back({dynamic_cast<CTexture*>(CGameInstance::GetInstance()->Clone_Component(CGameUtils::s2ws(e).c_str())), 0});
		}

	}

	tParams.iPass = jsonParams["Pass"];
}

void CShader::Imgui_RenderShaderParams(ShaderParams& tParams)
{
	_int iPass = (_int)tParams.iPass;
	ImGui::InputInt("Pass", &iPass);
	if (iPass < 0) iPass = 0;
	tParams.iPass = (_uint)iPass;

	ImGui::Text("Param Int");
	for (size_t i = 0; i < tParams.Ints.size(); ++i)
	{
		string name = to_string(i) + "Int";
		ImGui::InputInt(name.c_str(), &tParams.Ints[i]);
	}
	if (ImGui::Button("Add Int") && tParams.Ints.size() < PARAM_INT_CNT)
	{
		tParams.Ints.push_back(0);
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Int") && tParams.Ints.empty() == false)
	{
		tParams.Ints.pop_back();
	}


	ImGui::Separator();
	ImGui::Text("Param float");
	for (size_t i = 0; i < tParams.Floats.size(); ++i)
	{
		string name = to_string(i) + "Float";
		ImGui::InputFloat(name.c_str(), &tParams.Floats[i]);
		name += " Slider";
		ImGui::SliderFloat(name.c_str(), &tParams.Floats[i], 0.f, 1.f);
	}
	if (ImGui::Button("Add Float") && tParams.Floats.size() < PARAM_FLOAT_CNT)
	{
		tParams.Floats.push_back(0.f);
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Float") && tParams.Floats.empty() == false)
	{
		tParams.Floats.pop_back();
	}

	ImGui::Separator();
	ImGui::Text("Param float2");
	for (size_t i = 0; i < tParams.Float2s.size(); ++i)
	{
		string name = to_string(i) + "Float2";
		ImGui::InputFloat2(name.c_str(), (_float*)&tParams.Float2s[i]);
	}
	if (ImGui::Button("Add Float2") && tParams.Float2s.size() < PARAM_FLOAT2_CNT)
	{
		tParams.Float2s.push_back(_float2{});
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Float2") && tParams.Float2s.empty() == false)
	{
		tParams.Float2s.pop_back();
	}

	ImGui::Separator();
	ImGui::Text("Param float4");
	for (size_t i = 0; i < tParams.Float4s.size(); ++i)
	{
		string name = to_string(i) + "Float4";
		ImGui::InputFloat4(name.c_str(), (_float*)&tParams.Float4s[i]);
		name += " Color";
		ImGui::ColorEdit4(name.c_str(), (_float*)&tParams.Float4s[i], ImGuiColorEditFlags_PickerHueWheel);
	}
	if (ImGui::Button("Add Float4") && tParams.Float4s.size() < PARAM_FLOAT4_CNT)
	{
		tParams.Float4s.push_back(_float4{});
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Float4") && tParams.Float4s.empty() == false)
	{
		tParams.Float4s.pop_back();
	}

	ImGui::Separator();
	ImGui::Text("Param Texture");
	for (size_t i = 0; i < tParams.Textures.size(); ++i)
	{
		string name;
		if (tParams.Textures[i].first == nullptr)
			name = "Null";
		else
			name = tParams.Textures[i].first->GetTexName(tParams.Textures[i].second);

		ImGui::Text("%d %s", (_int)i, name.c_str());

		ImGui::SameLine();
		const string key = "Switch Texture" + to_string(i);
		CImguiUtils::FileDialog_FileSelector(key.c_str(), ".png,.dds", "../Bin/Resources/", 
			[&](const string& fileName)
		{
			if (false == CGameUtils::FileExist(s2ws(fileName).c_str()))
			{
				MSG_BOX("!!! There no File !!! ");
				return;
			}

			Safe_Release(tParams.Textures[i].first);
			size_t iPos = fileName.find("\\Bin\\Resources");
			string relative = ".." + fileName.substr(iPos, fileName.size() - iPos);

			tParams.Textures[i].first = dynamic_cast<CTexture*>(CGameInstance::GetInstance()->Clone_Component(CGameUtils::s2ws(relative).c_str()));
	
		});
	}
	if (ImGui::Button("Add Texture") && tParams.Textures.size() < PARAM_TEXTURE_CNT)
	{
		tParams.Textures.push_back(pair<CTexture*, _uint>{nullptr, 0u} );
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Texture") && tParams.Textures.empty() == false)
	{
		Safe_Release(tParams.Textures.back().first);
		tParams.Textures.pop_back();
	}
}

#ifdef _DEBUG
void CShader::ReCompileShader()
{
	ID3DX11Effect* pEffect = nullptr;
	_uint			iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

	if (FAILED(D3DX11CompileEffectFromFile(m_shaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice, &pEffect, nullptr)))
	{
		MSG_BOX("!!!!! Failed to ReCompileShader !!!!!");
		Safe_Release(pEffect);
		return;
	}

	for (auto& pInputLayout : m_InputLayouts)
		Safe_Release(pInputLayout);
	m_InputLayouts.clear();
	Safe_Release(m_pEffect);

	m_pEffect = pEffect;

	ID3DX11EffectTechnique*	pTechnique = m_pEffect->GetTechniqueByIndex(0);

	D3DX11_TECHNIQUE_DESC	TechniqueDesc;
	pTechnique->GetDesc(&TechniqueDesc);

	m_iNumPasses = TechniqueDesc.Passes;

	for (_uint i = 0; i < m_iNumPasses; ++i)
	{	
		ID3D11InputLayout*		pInputLayout = nullptr;

		ID3DX11EffectPass*		pPass = pTechnique->GetPassByIndex(i);
		if (nullptr == pPass)
			return;

		D3DX11_PASS_DESC		PassDesc;
		pPass->GetDesc(&PassDesc);	

		if (FAILED(m_pDevice->CreateInputLayout(m_pElements, m_iNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
			return;

		m_InputLayouts.push_back(pInputLayout);
	}

	for (auto& texture : m_CommonTextures)
	{
		texture.second->Bind_ShaderResource(this, texture.first.c_str());
	}

}
#endif

CShader * CShader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pShaderFilePath, D3D11_INPUT_ELEMENT_DESC* pElements, const _uint iNumElements)
{
	CShader*		pInstance = new CShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath, pElements, iNumElements)))
	{
		MSG_BOX("Failed to Created : CShader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CShader::Clone(void * pArg)
{
	CShader*		pInstance = new CShader(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CShader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CShader::Free()
{
	__super::Free();

	for (auto& pInputLayout : m_InputLayouts)
		Safe_Release(pInputLayout);

	m_InputLayouts.clear();

	Safe_Release(m_pEffect);

	if (m_isCloned == false)
	{
		for (auto tex : m_CommonTextures)
		{
			Safe_Release(tex.second);
		}
	}
}
