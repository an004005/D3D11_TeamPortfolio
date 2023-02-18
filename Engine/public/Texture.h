#pragma once

#include "Component.h"


BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
protected:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize_Prototype(const char* pTextureDirectoryPath);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Imgui_RenderProperty() override;

public:	
	HRESULT Bind_ShaderResources(class CShader* pShaderCom, const char* pConstantName);
	HRESULT Bind_ShaderResource(class CShader* pShaderCom, const char* pConstantName, _uint iTextureIndex = 0);

	_uint GetNumTex() const { return m_iNumTextures; }
	const string& GetTexName(_uint iTexIdx);

private:
	_uint										m_iNumTextures = 0;
	// ID3D11ShaderResourceView**					m_pTextures = nullptr;
	vector<ID3D11ShaderResourceView*>			m_Textures;

#ifdef _DEBUG
	vector<string> m_TextureNames;
	vector<string> m_TextureFilePath;
#endif

public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures = 1);
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* pTextureDirectoryPath);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};

END