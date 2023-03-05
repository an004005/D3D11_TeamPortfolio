#include "stdafx.h"
#include "..\public\Texture.h"

#include "GameUtils.h"
#include "Shader.h"
#include "GameUtils.h"
#include "GameInstance.h"

CTexture::CTexture(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CTexture::CTexture(const CTexture & rhs)
	: CComponent(rhs)
	, m_iNumTextures(rhs.m_iNumTextures)
	, m_Textures(rhs.m_Textures)
#ifdef _DEBUG
	, m_TextureNames(rhs.m_TextureNames)
#endif
{
	for (_uint i = 0; i < m_iNumTextures; ++i)
		Safe_AddRef(m_Textures[i]);
}

HRESULT CTexture::Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures)
{		
	// m_pTextures = new ID3D11ShaderResourceView*[iNumTextures];
	m_iNumTextures = iNumTextures;
	m_Textures.resize(m_iNumTextures);
#ifdef _DEBUG
	m_TextureNames.resize(m_iNumTextures);
#endif

	for (_uint i = 0; i < m_iNumTextures; ++i)
	{		
		_tchar	szTexturePath[MAX_PATH] = TEXT("");

		wsprintf(szTexturePath, pTextureFilePath, i);

		/* 문자열 분해하기. */		
		_tchar			szExt[MAX_PATH] = TEXT("");


#ifdef _DEBUG
		_tchar szFileName[MAX_PATH]{};
		_wsplitpath_s(szTexturePath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);
		m_TextureNames[i] = CGameUtils::ws2s(szFileName);
#else
		_wsplitpath_s(szTexturePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);
#endif

		wstring wstrExt = szExt;
		std::transform(wstrExt.begin(), wstrExt.end(), wstrExt.begin(), ::tolower);

		HRESULT		hr = 0;

		if (wstrExt == L".tga")
			return E_FAIL;	
		else if(wstrExt == L".dds")
			hr = DirectX::CreateDDSTextureFromFile(m_pDevice, szTexturePath, nullptr, &m_Textures[i]);
		else		
			hr = DirectX::CreateWICTextureFromFile(m_pDevice, szTexturePath, nullptr, &m_Textures[i]);

		if (FAILED(hr))
		{
			char szFileName[MAX_PATH];
			CGameUtils::wc2c(pTextureFilePath, szFileName);
			IM_LOG("Fail to load Texture : %s", szFileName);
			return E_FAIL;

		}
		
	}

	return S_OK;
}

HRESULT CTexture::Initialize_Prototype(const char* pTextureDirectoryPath)
{
	const _uint iFileCnt = CGameUtils::CountFiles(pTextureDirectoryPath);
	m_Textures.resize(iFileCnt);
#ifdef _DEBUG
	m_TextureNames.resize(iFileCnt);
#endif
	m_iNumTextures = iFileCnt;

	_uint iIdx = 0;

	CGameUtils::ListFiles(pTextureDirectoryPath, [&](const string& filePath)
	{
		wstring wstrFilePath = CGameUtils::s2ws(filePath);

		/* 문자열 분해하기. */		
		_tchar			szExt[MAX_PATH] = TEXT("");

#ifdef _DEBUG
		_tchar szFileName[MAX_PATH]{};
		_wsplitpath_s(wstrFilePath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);
		m_TextureNames[iIdx] = CGameUtils::ws2s(szFileName);
#else
		_wsplitpath_s(wstrFilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);
#endif

		wstring wstrExt = szExt;
		std::transform(wstrExt.begin(), wstrExt.end(), wstrExt.begin(), ::tolower);

		HRESULT		hr = 0;

		if (wstrExt == L".tga")
			return E_FAIL;	
		else if(wstrExt == L".dds")
			hr = DirectX::CreateDDSTextureFromFile(m_pDevice, wstrFilePath.c_str(), nullptr, &m_Textures[iIdx]);
		else		
			hr = DirectX::CreateWICTextureFromFile(m_pDevice, wstrFilePath.c_str(), nullptr, &m_Textures[iIdx]);

		++iIdx;

		if (FAILED(hr))
		{
			IM_LOG("Fail to load Texture : %s", filePath.c_str());
		}
		return S_OK;
	});


	return S_OK;
}

HRESULT CTexture::Initialize(void * pArg)
{
	return S_OK;
}

void CTexture::Imgui_RenderProperty()
{
}

HRESULT CTexture::Bind_ShaderResources(CShader * pShaderCom, const char * pConstantName)
{
	return pShaderCom->Set_ShaderResourceViewArray(pConstantName, m_Textures.data(), m_iNumTextures);
}

HRESULT CTexture::Bind_ShaderResource(CShader * pShaderCom, const char * pConstantName, _uint iTextureIndex)
{
	if (nullptr == pShaderCom || 
		iTextureIndex >= m_iNumTextures)
		return E_FAIL;

	return pShaderCom->Set_ShaderResourceView(pConstantName, m_Textures[iTextureIndex]);	
}

const string& CTexture::GetTexName(_uint iTexIdx)
{
	static string empty;
#ifdef _DEBUG
	return m_TextureNames[iTexIdx];
#endif
	return empty;
}

CTexture * CTexture::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pTextureFilePath, _uint iNumTextures)
{
	CTexture*		pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, iNumTextures)))
	{
		MSG_BOX("Failed to Created : CTexture");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* pTextureDirectoryPath)
{
	CTexture*		pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pTextureDirectoryPath)))
	{
		MSG_BOX("Failed to Created : CTexture");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CTexture::Clone(void * pArg)
{
	CTexture*		pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (_uint i = 0; i < m_iNumTextures; ++i)
		Safe_Release(m_Textures[i]);
}
