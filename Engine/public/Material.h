#pragma once
#include "Component.h"
#include "Model.h"
#include "Shader.h"

BEGIN(Engine)
class ENGINE_DLL CMaterial : public CComponent
{
protected:
	CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMaterial(const CMaterial& rhs);

public:
	virtual HRESULT Initialize_Prototype(const char* pMtrlFilePath);
	virtual void SaveToJson(Json& json) override;
	virtual void Imgui_RenderProperty() override;

public:
	void Tick(_double TimeDelta);
	class CShader* GetShader() { return m_pShader; }
	class CShader* GetShaderInstancing() { return m_pShaderInstancing; }

	virtual void SetAnimType(CModel::TYPE eType){}
	void BindMatrices(class CTransform* pTransform);
	void BindMatrices(const _float4x4& WorldMatrix);
	void BindMatrices(class CTransform* pTransform, class CCamera* pCamera);
	void BindMatrices_Instancing(class CTransform* pTransform);
	void BindMatrices_Instancing(const _float4x4& WorldMatrix);
	virtual void Begin();
	void Begin(_uint iPass);
	ShaderParams& GetParam() { return m_tParams;}

	void Begin_Instancing();

	void SetActive(_bool bActive) { m_bActive = bActive; }
	_bool IsActive() const { return m_bActive; }
	void SetAlphaBlend(_bool bGlass) { m_bAlphaBlend = bGlass; }
	_bool IsAlphaBlend() const { return m_bAlphaBlend; }

	_uint GetInstancePass() { return m_iInstancingPass; }
	void SetInstancePass(_uint iPass) { m_iInstancingPass = iPass; }


public:
	static void LoadMaterialFilePathes(const string& MaterialJsonDir);
	static const string* FindMaterialFilePath(const string& MaterialName);
private:
	static unordered_map<string, string> s_MtrlPathes;

protected:
	class CShader* m_pShader = nullptr;
	class CShader* m_pShaderInstancing = nullptr;
	 ShaderParams m_tParams;

	// 임시로 이 머터리얼을 사용하는 메쉬를 렌더하거나 렌더하지 않는데 사용
	_bool m_bActive = true;
	_uint m_iInstancingPass = 0;
	_bool m_bAlphaBlend = false;

#ifdef _DEBUG
	string m_strFilePath;
#endif

public:
	virtual void Free() override;
	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* pMtrlFilePath);
	virtual CComponent* Clone(void* pArg) override;
	
};

END